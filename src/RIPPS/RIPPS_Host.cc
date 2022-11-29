/* RIPPS_Host.cc
 **********************************************************
 * This code is part of the RIPPS (Rogue Identifying
 * Packet Payload Slicer) system developed at the University
 * of Notre Dame. 
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * Students: Chad Mano (lead)      Yingxin Jiang
 *           Dave Salyers          Dave Cieslak
 *           Qi Liao               Andrew Blaich   
 **********************************************************
 * $Revision: 1.13 $  $Date: 2008/05/08 18:07:56 $
 **********************************************************
 */

#include <iostream>
using namespace std;
 
//#include "Adapter.h"			// For direction only

#include "RIPPS_Host.h"
//#include "../mon/Thread_Archive.h"

#include "../pkt/Packet.h"			// Packet object
#include "../util/ip-utils.h"		// IP packet offsets

#include "RIPPS_Monitor.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

//////////////////////////////////////////////////////////////

RIPPS_HostStats::RIPPS_HostStats () : Stats () {
	Allocate(RIPPS_HOST_STAT_LAST);	
}

////////////////////////////////////////////////////////////////

RIPPS_Host::RIPPS_Host () {
	initialize();	
}

////////////////////////////////////////////////////////////////

RIPPS_Host::RIPPS_Host (int nDirection, Packet * pPacket) {
	initialize();

	// TODO - Fix the host identity extraction for RIPPS to be more IPv6 friendly

	if(nDirection == RIPPS_TAP_MONITORLAN) {
		// We want src IP, src MAC
		memcpy(m_byMAC, pPacket->getData()+OFFSET_ETH_SRCMAC, DEFAULT_MAC_LEN);		
		memcpy(m_byIP, pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP, DEFAULT_IP_LEN);
	} else if(nDirection == RIPPS_TAP_WAN) {
		// We want dst IP, dst MAC
		memcpy(m_byMAC, pPacket->getData()+OFFSET_ETH_DSTMAC, DEFAULT_MAC_LEN);		
		memcpy(m_byIP, pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP, DEFAULT_IP_LEN);		
	}
}

pthread_mutex_t * RIPPS_Host::getMutexHost () {
	return &m_MutexHost;	
}

////////////////////////////////////////////////////////////////

void RIPPS_Host::initialize () {
	// Must remain the same
	m_nIPLength = DEFAULT_IP_LEN;
	m_nMACLength = DEFAULT_MAC_LEN;
	m_nConnMedium = RIPPS_CONN_MEDIUM_UNKNOWN;
	m_nValidateState = VALIDATE_STATE_NONE;
	m_nMonitorSet = 1;
	
	m_latestShapeTime.tv_sec = 0;
	m_latestShapeTime.tv_usec = 0;
	
	m_nMonitorPkts = 0;
	m_nDataPktNum = 0;
	
	pthread_mutex_init(getMutexHost(), NULL);
		
	m_nSliceState = SLICESTATE_SLICING;
	m_nSliceCount = 0;
	
	m_nPollDataPktNum = -1;		
	
	m_nAssessDecayTime = DEFAULT_MONITOR_INTERVAL;
	
	// Minimum inter-packet gap when slicing is imposed
	m_nShapeUS = DEFAULT_SLICE_DELAY;
	
	// Initial value for polled system time on creation of host	
	gettimeofday(&m_pollSysTime, NULL);	
	
	m_PktQueue.initialize();
}

////////////////////////////////////////////////////////////////

void RIPPS_Host::dumpBrief_withOpen () {
	dumpBrief();
	
	pthread_mutex_lock(getMutexHost());	
	m_listConnections.dumpBrief_MonPkt();
	pthread_mutex_unlock(getMutexHost());
}

////////////////////////////////////////////////////////////////

void RIPPS_Host::dumpBrief () {
	int j;

	cout << "  Host @ ";
	printf("0x%X\n", this);

	pthread_mutex_lock(getMutexHost());

	switch(m_nValidateState) {
		case VALIDATE_STATE_NONE:
			cout << "  No Mon State  ";
			break;
		case VALIDATE_STATE_RIPPS:
			cout << "  RIPPS Mon     ";
			break;
		case VALIDATE_STATE_LRTT:
			cout << "   LRTT Mon     ";
			break;
		case VALIDATE_STATE_RESOLVED:
			cout << "    Mon Done    ";  
			break;
	}

	switch(m_nConnMedium) {
		case RIPPS_CONN_MEDIUM_UNKNOWN:
			cout << "Unknown  ";
			break;
		case RIPPS_CONN_MEDIUM_WIRED:
			cout << "Wired    ";
			break;
		case RIPPS_CONN_MEDIUM_WIRELESS:
			cout << "Wireless ";
			break;
	}
	
	cout << "|  IP: ";

	if(m_nIPLength == 4) {	
		dumpIPv4(m_byIP);	
	} else {
		cout << "TBA (IPv6)";
	}
	
/*	
	cout << " |  MAC: ";
	
	for(j=0; j<m_nMACLength; j++) {
		cout << (unsigned char) m_byMAC[j];
		
		if(j<5) {
			cout << ":";
		}
	}*/
	
	cout << "   Last Monitor: ";
	displayTime(&m_LastValidation);
	
	cout << endl;
		
	m_listConnections.dumpBrief();

	pthread_mutex_unlock(getMutexHost());
}

////////////////////////////////////////////////////////////////

struct timeval * RIPPS_Host::getShaper_LastTime () {
	return &m_latestShapeTime;	
}

////////////////////////////////////////////////////////////////

void RIPPS_Host::setShaper_LastTime (struct timeval * pTailExit) {
	m_latestShapeTime = *pTailExit;	
}

////////////////////////////////////////////////////////////////

void RIPPS_Host::setValue_ShapeDelay (unsigned int nDelayUS) {
	m_nShapeUS = nDelayUS;
}

////////////////////////////////////////////////////////////////

unsigned int RIPPS_Host::getValue_ShapeDelay () {
	return m_nShapeUS;	
}

////////////////////////////////////////////////////////////////

struct timeval * RIPPS_Host::computeShapeDispatch () {	
	pthread_mutex_lock(getMutexHost());
	
	// If we are on a different data packet number, update the last polled system time
	if(m_nPollDataPktNum != getDataPktNum()) {
		gettimeofday(&m_pollSysTime, NULL);
		m_nPollDataPktNum = getDataPktNum();		
	}

	// Adjust the dispatch time by a slice delay if we are monitoring since the next packet
	// needs to be separated by at least slice delay from the latest dispatch packet if at
	// all possible. Note that due to imprecise shaping, OS context switches, and time resolutions
	// that this is simply an approximate spacing and should not be considered completely
	// accurate	
	adjustTime(&m_latestShapeTime, getValue_ShapeDelay());

	// Compare the time of our adjusted last dispatch (X + Shape Gap) versus the perceived current time
	if(compareTime(&m_latestShapeTime, &m_pollSysTime)) {
		// Non-zero means that the dispatch time was greater than the current time. This means
		// we have a backlog of packets to be shaped for that host. The correct dispatch time
		// should be after our backlog of packets
	} else {
		// Current time is greater than or equal to the dispatch with the appropriate shaping.
		// In other words, sufficient shaping does exist  
		// 	Therefore we should use the current time
		m_latestShapeTime = m_pollSysTime;
	}
		
	pthread_mutex_unlock(getMutexHost());		
		
	return &m_latestShapeTime;
}


///////////////////////////////////////////////////////////////////////////////

bool RIPPS_Host::hasBacklog () {	
	if(m_PktQueue.getNumPackets() > 0) { 
		return true;
	} else {
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////

void RIPPS_Host::setMonitorSet (unsigned int nSet) {
	m_nMonitorSet = nSet;	
}

///////////////////////////////////////////////////////////////////////////////

unsigned int	RIPPS_Host::getMonitorSet () {
	return m_nMonitorSet;	
}

///////////////////////////////////////////////////////////////////////////////

unsigned int RIPPS_Host::getMonitorCount () {
	return m_listConnections.getMonitorCount(m_nMonitorSet);	
}

double RIPPS_Host::getLRTT () {
	return m_listConnections.getAvgLRTT(m_nMonitorSet);	
}

////////////////////////////////////////////////////////////////

void	RIPPS_Host::setLastValidation (struct timeval * pTime) {
	m_LastValidation = *pTime;
}



void RIPPS_Host::setIP (char * pIP, int nLength) {
	if(nLength <= 0) {
		cerr << "Invalid length for IP address of " << nLength << endl;
		cerr << "  Ignoring call to Host::setIP" << endl;
		return;
	}

	if(nLength > MAX_IP_LENGTH) {
		cerr << "Warning: Attempting to copy in an IP address greater than " << MAX_IP_LENGTH << " bytes." << endl;
		cerr << "  Ignoring call to Host::setIP" << endl;
		return;
	}

	memcpy(m_byIP, pIP, nLength);
	m_nIPLength = nLength;
}

char * RIPPS_Host::getIP () {
	return m_byIP;
}

int RIPPS_Host::getIP_Length () {
	return m_nIPLength;
}

char RIPPS_Host::isMatch_IP (char * pIP, int nLength) {
//	if(nLength <= 0 || nLength > MAX_IP_LENGTH) {
//		cerr << "Invalid length for IP address of " << nLength << endl;
//		cerr << "  Returning as no match for call to Host::isMatch_IP" << endl;
//		return -1;	
//	}

	if(memcmp(pIP, m_byIP, nLength)) {
		return 0;
	} else {
		return 1;
	}
}

char RIPPS_Host::isMatch_MAC (char * pMAC, int nLength) {
//	if(nLength <= 0 || nLength > MAX_MAC_LENGTH) {
//		cerr << "Invalid length for MAC address of " << nLength << endl;
//		cerr << "  Returning as no match for call to Host::isMatch_MAC" << endl;
//		return -1;	
//	}

	if(memcmp(pMAC, m_byMAC, nLength)) {
		return 0;
	} else {
		return 1;
	}
}

char * RIPPS_Host::getMAC () {
	return m_byMAC;
}

void RIPPS_Host::setMAC (char * pMAC, int nLength) {
	if(nLength <= 0) {
		cerr << "Invalid length for MAC address of " << nLength << endl;
		cerr << "  Ignoring call to Host::setMAC" << endl;
		return;
	}

	if(nLength > MAX_MAC_LENGTH) {
		cerr << "Warning: Attempting to copy in an MAC address greater than " << MAX_MAC_LENGTH << " bytes." << endl;
		cerr << "  Ignoring call to Host::setMAC" << endl;
		return;
	}

	memcpy(m_byMAC, pMAC, nLength);
	m_nMACLength = nLength;
}

int RIPPS_Host::getMAC_Length () {
	return m_nMACLength;
}

int RIPPS_Host::getConnectionMedium () {
	return m_nConnMedium;
}

void RIPPS_Host::setConnectionMedium (int nMedium) {
	m_nConnMedium = nMedium;
}


char RIPPS_Host::shouldApplyRIPPS () {
	if(m_nValidateState != VALIDATE_STATE_LRTT && m_nValidateState != VALIDATE_STATE_RESOLVED) {
		return 1;
	} else {
		return 0;
	}
}

void RIPPS_Host::closeConnectionExt (Packet * pPacket) {
	RIPPS_Connection	*		pConn;
		
	pthread_mutex_lock(getMutexHost());				
		
	pConn = m_listConnections.findConnection(pPacket);
	
	if(pConn == NULL) {
		//cerr << " Ext Connection close but none exists internally" << endl;
		//pPacket->dumpBrief();		
		pthread_mutex_unlock(&m_MutexHost);				
		return;
	}	
	
	//cout << "Closing connection for packet " << endl;
	//pConn->dumpBrief();
	//cout << "******" << endl;	
	
	// Stamp the file name to make sure that we are good to go for the dump file name
//	dumpIPv4(m_byIP);
//	cout << endl;	
	pConn->setSrcIP(m_byIP);

	//cout << "  Bypassing normal summary creation mechanism" << endl;	
	//m_listConnections.createSummary(pConn);	
	pthread_mutex_unlock(getMutexHost());			
}

bool RIPPS_Host::closeConnectionMon (Packet * pPacket) {
	RIPPS_Connection	*		pConn;
	
	pthread_mutex_lock(getMutexHost());			
		
	pConn = m_listConnections.findConnection(pPacket,1);
	
	if(pConn == NULL) {
		//cerr << " Int Connection close but none exists internally" << endl;
		//pPacket->dumpBrief();		
		pthread_mutex_unlock(getMutexHost());				
		return false;
	}	
	
	//cout << "Closing connection for packet " << endl;
	//pConn->dumpBrief();
	//cout << "******" << endl;
	
//	dumpIPv4(m_byIP);
//	cout << endl;
//	cout << "  Bypassing normal summary creation mechanism" << endl;	

	pConn->setSrcIP(m_byIP);
	m_listConnections.createSummary(pConn);			
	pthread_mutex_unlock(getMutexHost());			
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////

void RIPPS_Host::addExpectedAck (Packet * pPacket) {
	addExpectedAck(pPacket, 0, 0, 0); 	
}

///////////////////////////////////////////////////////////////////////////////

void RIPPS_Host::incDataPktNum () {
	pthread_mutex_lock(getMutexHost());	
	m_nDataPktNum++;
	pthread_mutex_unlock(getMutexHost());	
}

///////////////////////////////////////////////////////////////////////////////		
		
int RIPPS_Host::getDataPktNum () {
	return m_nDataPktNum;
}		

char RIPPS_Host::shouldMonitorPkt_WAN (Packet * pPacket) {
	RIPPS_Connection	*		pConn;
	char					bResult;		

	// Lock the mutex for the host to prevent the internal connection or ack list from
	//  being manipulated
	pthread_mutex_lock(getMutexHost());	
	
	// Connection-wise access is not protected by a mutex as the changes in connections
	//  are much more dynamic than the changes in hosts
	pConn = m_listConnections.findConnection(pPacket);	
	
	if(pConn != NULL) {
		// The packet is a re-transmission if the sequence number is less than the highest sequence
		//  number we have seen
		if(pConn->getValidHighestSeq() && getTCP_SeqNumber(pPacket->getData(), -1) < pConn->getHighestSeq()) {
			if(pConn->getValidHighestSeq() && (pConn->getHighestSeq() - getTCP_SeqNumber(pPacket->getData(), -1) >= SEQNUM_WRAP_GAP)) { 
				bResult = 1;				
			} else {			
				m_Stats.Add_Stat(RIPPS_HOST_STAT_PKTS_RETRANS, 1);
				pConn->incRetransmissions();
				pConn->noteRetransmission(pPacket);
				bResult = 0;				
			}
		} else {
			// Check for the wrap around
			bResult = 1;
		}
	} else {
		// Connection is not in list, definitely monitor
		bResult = 1;
	}

	m_nSliceCount++;
	
	if(bResult) {
		// Do we get overridden in terms of not slicing by the state
		if(m_nSliceState == SLICESTATE_NOSLICE) {
			bResult = 0;
		}
	}	
	

	
	pthread_mutex_unlock(getMutexHost());
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

bool RIPPS_Host::hasConnection (Packet * pPacket) {
   RIPPS_Connection *  pConn;

	pthread_mutex_lock(getMutexHost());

   pConn = m_listConnections.findConnection(pPacket);

   if(pConn == NULL) {
   	pthread_mutex_unlock(getMutexHost());
   	return false;
   } else {
   	pthread_mutex_unlock(getMutexHost());
     	return true;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RIPPS_Host::addExpectedAck (Packet * pPacket, char bGobble, unsigned int lNewExpAck, int nExtraAcks) {
	RIPPS_Connection	*	pConn;
	RIPPS_MonitorPacket * 	pMonPkt;
	
	// Lock the mutex for the host to prevent the internal connection or ack list from
	//  being manipulated
	pthread_mutex_lock(getMutexHost());
	
	//cout << "  RIPPS_Host::addExpectedAck -> Adding expected ack " << endl;
	
	if(getValidationState() == VALIDATE_STATE_RESOLVED) {
		pthread_mutex_unlock(getMutexHost());
		return;
	}

	// Connection-wise access is not protected by a mutex as the changes in connections
	//  are much more dynamic than the changes in hosts
	pConn = m_listConnections.findConnection(pPacket);
	
	if(pConn == NULL) {
		pConn = new RIPPS_Connection();
		
		pConn->setSrcIP(m_byIP);

   	if(pConn == NULL) {
			//g_theArchive.archiveString("* Error allocating new Connection object in Host::addExpectedAck");
			cerr << "* Error allocating new Connection object in Host::addExpectedAck" << endl;
			cerr.flush();
			pthread_mutex_unlock(getMutexHost());
			return;
		}

		//cout << "--> Created a new connection" << endl;
		
		pConn->populateInfo(pPacket);
		pConn->setMonitorSet(m_nMonitorSet);				
		m_listConnections.addConnection(pConn);

		//cout << "--> Added the connection" << endl;
		//m_listConnections.dumpBrief();
	} 
	
	//cout << "  Populating expected ack" << endl;
	
	pMonPkt = pConn->populateExpectedAck(pPacket);		
	
	//cout << "  Done populating expected ack" << endl;

	// Set the gobble flag of the monitor packet to denote if the corresponding ack needs to be
	//  gobbled and not forwarded back to the external host	
	if(pMonPkt != NULL) {
		// There are two instances of positive gobbling, classic Sting and also where
		//  we allow gobbling despite seeing a nicely aligned sequence number
	  	if(bGobble == GOBBLE_PKT_YES || bGobble == GOBBLE_PKT_USESEQ) {
	  		//cout << "Setting the gobble flag! **************" << endl;
			pMonPkt->setFlag_Gobble(bGobble);
	  	}

		pMonPkt->setDataSrcPkt(getDataPktNum());
		
		// The seq only and yes settings will cause us to use the special expected ack
		if(bGobble == GOBBLE_PKT_SEQONLY || bGobble == GOBBLE_PKT_YES) {
			pMonPkt->setAck(lNewExpAck);
		}	
			
		pConn->addExpectedAck(pMonPkt);
	}
	
	int		j;		
	
	//cout << "Extra acks is set to " << nExtraAcks << endl;
	
	for(j=0; j<nExtraAcks; j++) {
		pMonPkt = RIPPS_MonitorPacket::createMonPkt();
		pMonPkt->setFlag_Gobble(GOBBLE_PKT_YES);
		pMonPkt->setDataSrcPkt(getDataPktNum());
		pMonPkt->setAck(lNewExpAck);
		
		pConn->addExpectedAck(pMonPkt);						
	}
			
	pthread_mutex_unlock(getMutexHost());
	
	//cout << "  * (Exit) * Host::addExpectedAck -> Adding expected ack " << endl;	
}

////////////////////////////////////////////////////////////////////////////////////////////

int RIPPS_Host::processAck (Packet * pPacket) {
	RIPPS_Connection	*		pConn;
	int							nResult;

	// Lock the mutex for the host to prevent the internal connection or ack list from
	//  being manipulated
	pthread_mutex_lock(getMutexHost());

	// This bypass is disabled as it does not buy us that much anyway, i.e. ignoring ack matches
	//  after monitoring is disabled as the list is likely empty of expected acks anyway
	// 
	//  Without bypassing this, we have a weird nebulous state where acks still need to be
	//  squelched from earlier activities in slicing (TCP Sting variant) but yet our monitoring
	//  on the connection is done.  Letting the increased ACKs through would cause retransmissions
	//  which would not be a desirable feature but at worst case would result in only temporary
	//  instability of the TCP mechanisms. 
	// 	
	//	 For now, we will set it up such that it does simply does not add expected ACKs to the
	//  list if the host is in the resolved state
	//	
	// Is the validation of this host done and we don't need to worry about matching additional
	//  acks?
	//if(getValidationState() == VALIDATE_STATE_RESOLVED) {
	//	pthread_mutex_unlock(&m_MutexHost);
	//	return HOST_PKT_FWD;
	//}

	pConn = m_listConnections.findConnection(pPacket,1);
	
	if(pConn == NULL) {
		// Ignore it, we didn't find an existing connection, will only create on new data
		//cout << "  -- No matching connection found, ignoring ack packet" << endl;
		pthread_mutex_unlock(getMutexHost());
		return HOST_PKT_FWD;
	} else {
		RIPPS_MonitorPacket *		pMonPkt;
		
		pMonPkt = pConn->matchAck(pPacket);
		
		// No packet found, just forward it since we have nothing
		if(pMonPkt == NULL) {
		  pthread_mutex_unlock(getMutexHost());
		  return HOST_PKT_FWD;
		}

		// Check for gobbling - we may need to squelch the forwarding of this ack at times 
		if(pMonPkt->getFlag_Gobble()) {
			nResult = HOST_PKT_GOBBLE;		
			//pMonPkt->dumpDetail();
		} else {
			nResult = HOST_PKT_FWD;
		}
		
		if(!pConn->applyMatch(pMonPkt)) {
			cerr << "Error: Returning badly matched match in processAck to memory pool" << endl;
			pMonPkt->release();
			pthread_mutex_unlock(getMutexHost());	
			return HOST_PKT_GOBBLE;
		}

		// Found a match, so increment the current count of matches
		addMonPkt();		
		
		pthread_mutex_unlock(getMutexHost());	
		return nResult;
	}
	
	pthread_mutex_unlock(getMutexHost());	
	return HOST_PKT_FWD;
}

char RIPPS_Host::getValidationState () {
	return m_nValidateState;
}

void RIPPS_Host::setValidationState (char nState) {
	m_nValidateState = nState;
}

/////////////////////////////////////////////////////////////////////////

bool RIPPS_Host::checkReassess (struct timeval * pCurTime) {
	pthread_mutex_lock(getMutexHost());	
	
	// If we are not in a resolved state, don't touch it
	if(m_nValidateState != VALIDATE_STATE_RESOLVED) {
		pthread_mutex_unlock(&m_MutexHost);	
		return false;
	}
	
	// Must be in a resolved state, do we need to poke the system
	// and push it into a reassess state?
	if(pCurTime->tv_sec - getAssessDecayTime() >= m_LastValidation.tv_sec) {
		pthread_mutex_unlock(&m_MutexHost);
		startMonitor();			
		return true;
	} else {
		pthread_mutex_unlock(getMutexHost());	
		return false;
	}		
}

/////////////////////////////////////////////////////////////////////////

void RIPPS_Host::checkSummarizeConnections (struct timeval * pCurTime) {
	RIPPS_Connection		*		pConn;
	int							j;
	int							nConns;
	
	pthread_mutex_lock(getMutexHost());		
	
	nConns = m_listConnections.getOpenCount();
	
	for(j=0; j<nConns; j++) {
		pConn = m_listConnections.getOpenConnection(j);
		
		if(pConn != NULL) {
			if(pConn->canSummarize(pCurTime)) {
			  //cout << " Summarizing the connection noted by " << endl;
			  //pConn->dumpDetail();
				m_listConnections.createSummary(pConn);
				nConns--;
				j--;
			}	
		}	
	}
	
	pthread_mutex_unlock(getMutexHost());			
}

void RIPPS_Host::startMonitor () {
	pthread_mutex_lock(getMutexHost());		
	m_nValidateState = VALIDATE_STATE_RIPPS;
	pthread_mutex_unlock(getMutexHost());		
	
	startNewSet();	
}

void RIPPS_Host::startNewSet () {
	pthread_mutex_lock(getMutexHost());		
	m_nMonitorSet++;
	m_nMonitorPkts=0;			
	pthread_mutex_unlock(getMutexHost());			
}

void RIPPS_Host::summarizeAllConnections () {
	pthread_mutex_lock(getMutexHost());	
	m_listConnections.summarizeAll(m_byIP);	
	pthread_mutex_unlock(getMutexHost());		
}

////////////////////////////////////////////////////////////////

NodeDOM * RIPPS_Host::getStats (NodeDOM * pRoot) {
	NodeDOM	*	pNode;
	NodeElem * 	pElem;
	
	pNode = NodeDOM::createNodeDOM();
	
	pNode->setTag("Host");
	
	pElem = NodeElem::createNodeElem();

	pElem->setName("IP");
	pElem->setValue(IPv4toString(getIP()));
	
	pNode->addElement(pElem);	
	
	pRoot->addChild(pNode);
	
	pthread_mutex_lock(getMutexHost());
	m_listConnections.getStats(pNode);	
	pthread_mutex_unlock(getMutexHost());		
	
	return pNode;
}	

////////////////////////////////////////////////////////////////

void RIPPS_Host::addMonPkt 	() {
	m_nMonitorPkts++;
}

////////////////////////////////////////////////////////////////

void	RIPPS_Host::archiveData		(unsigned int nSet) {
	m_listConnections.archiveOldData(nSet);
}

void RIPPS_Host::addShapedPacket (Packet * pPacket) {
	// Packet queue is already thread safe, no need to guard it additionally
	m_PktQueue.enqueue(pPacket);
}

	
void RIPPS_Host::restrictPastData (unsigned int nMaxSets) {
	if(nMaxSets > m_nMonitorSet) {
		return;
	}
	
	archiveData(m_nMonitorSet - nMaxSets);
}

////////////////////////////////////////////////////////////////

void RIPPS_Host::setAssessDecayTime (unsigned int nDecayTime) {
	m_nAssessDecayTime = nDecayTime;	
}

////////////////////////////////////////////////////////////////

unsigned int RIPPS_Host::getAssessDecayTime () {
	return m_nAssessDecayTime;
}

////////////////////////////////////////////////////////////////

char	RIPPS_Host::validateHost () {
	// TODO Complete validation code for the host
	return RIPPS_CONN_MEDIUM_UNKNOWN;
}

////////////////////////////////////////////////////////////////
		
Packet * RIPPS_Host::popShapedPacket () {
	Packet * pPacket;	
		
	pPacket = m_PktQueue.popAndGetPacket();
		
	return pPacket;
}			

////////////////////////////////////////////////////////////////

List_RIPPSHosts::List_RIPPSHosts () {

}

List_RIPPSHosts::~List_RIPPSHosts () {

}

/// Dump the host list with a brief summary of open matches
void	List_RIPPSHosts::dumpBrief_withOpen () {
	RIPPS_Host	*	pHost;
	unsigned int			j;

	for(j=0; j<m_List.size(); j++) {
		pHost = m_List[j];
		cout << "  " << j << "  ";
		pHost->dumpBrief_withOpen();
	}			
}

///////////////////////////////////////////////////////////////

void List_RIPPSHosts::addHost (RIPPS_Host * pHost) {
	if(pHost == NULL) {
		cerr << "Error: Tried to push a NULL host onto the list" << endl;
		return;
	}
	
	m_List.push_back(pHost);
}

///////////////////////////////////////////////////////////////

RIPPS_Host * List_RIPPSHosts::findHost_byIP (char * pIP, int nLength) {
	RIPPS_Host	*	pHost;
	unsigned int			j;
	
	if(nLength <= 0 || nLength > MAX_IP_LENGTH) {
		cerr << "Invalid length for IP address of " << nLength << endl;
		cerr << "  Ignoring search of List_RIPPSHosts via findHost_byIP" << endl;
		return NULL;
	}
	
	for(j=0; j<m_List.size(); j++) {
		pHost = m_List[j];
		
		if(pHost->isMatch_IP(pIP, nLength)) {
			return pHost;
		}
	}
	
	return NULL;
}

///////////////////////////////////////////////////////////////

RIPPS_Host * List_RIPPSHosts::findHost_byMAC (char * pMAC, int nLength) {
	RIPPS_Host	*	pHost;
	unsigned int			j;

	if(nLength <= 0 || nLength > MAX_MAC_LENGTH) {
		cerr << "Invalid length for MAC address of " << nLength << endl;
		cerr << "  Ignoring search of List_RIPPSHosts via findHost_byMAC" << endl;
		return NULL;
	}

	
	for(j=0; j<m_List.size(); j++) {
		pHost = m_List[j];
	
		if(pHost->isMatch_MAC(pMAC)) {
			return pHost;
		}
	}
	
	return NULL;
}

void List_RIPPSHosts::dumpBrief () {
	RIPPS_Host	*	pHost;
	unsigned int			j;

	cout << "List_RIPPSHosts::dumpBrief with " << m_List.size() << " hosts on the list" << endl;

	for(j=0; j<m_List.size(); j++) {
		pHost = m_List[j];		
		printf("   RIPPS_Host object @ 0x%X\n", pHost);
		cout << "  " << j << "  ";
		pHost->dumpBrief();
	}		
}

void List_RIPPSHosts::forceValidationState (char nState) {
	RIPPS_Host	*	pHost;
	unsigned int			j;

	for(j=0; j<m_List.size(); j++) {	
		pHost = m_List[j];
		pHost->startMonitor();
	}
}

int List_RIPPSHosts::getSize () {
  return m_List.size();
}

/////////////////////////////////////////////////////////////////////////

RIPPS_Host * List_RIPPSHosts::getHost (int nHost) {
  if(nHost >= getSize() || nHost < 0) {
    return NULL;
  }
  return m_List[nHost];
}

/////////////////////////////////////////////////////////////////////////

void List_RIPPSHosts::checkReassessments (struct timeval * pCurTime) {
	RIPPS_Host	*	pHost;
	unsigned int			j;

	for(j=0; j<m_List.size(); j++) {	
		pHost = m_List[j];
		pHost->checkReassess(pCurTime);		
	}	
}


