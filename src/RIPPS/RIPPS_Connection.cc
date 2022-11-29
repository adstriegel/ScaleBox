/* RIPPS_Connection.cc
 **********************************************************
 * This code is part of the RIPPS (Rogue Identifying
 * Packet Payload Slicer) system developed at the University
 * of Notre Dame. 
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * Students: Chad Mano (lead)      Yingxin Jiang
 *           Dave Salyers          Dave Cieslak
 *           Qi Liao               Andrew Blaich   
 * 
 * 
 * 
 **********************************************************
 * $Revision: 1.9 $  $Date: 2008/04/11 21:59:58 $
 ********************************************************** 
 */

#include <netinet/in.h>

#include <iostream>
#include <fstream>
using namespace std;

#include <sys/time.h>

#include "RIPPS_Connection.h" 
//#include "RIPPS_Monitor.h"

#include "../util/ip-utils.h"
//#include "../mon/Thread_Archive.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"


RIPPS_ConnSummary::RIPPS_ConnSummary (RIPPS_Connection * pConn) {
	List_RIPPSMonitorPkts *		pList;

	m_lastData = pConn->getLastData();
	m_firstPkt = pConn->getFirstData();
	
	m_nIntPort = pConn->getPort_Internal();
	m_nExtPort = pConn->getPort_External();
	
	if(pConn->getIPLength() < 16) {
		memcpy(m_ExtIP, pConn->getIP_External(), pConn->getIPLength());
		m_nIPLength = pConn->getIPLength();
	} else {
		m_nIPLength = 0;
	}
	
	pList = pConn->getListPkts();
	
	m_nPkts = pList->getCountMatches();
	m_fLRTT = pList->computeLRTT();	
	m_fStdDev = pList->computeStdDev();	
	
	m_nMonitorSet = pConn->getMonitorSet();
	
	// TODO Update info from the connection info on purged acks
	//m_nPurgedAcks = pConn->getListPkts()->getPurgedAcks();
	m_nPurgedAcks = 0;	
	m_nRetransmissions = pConn->getRetransmissions();
	
	m_pStats = new RIPPS_ListMonPktsStats(pConn->getListPkts()->getStatsObject());
}

RIPPS_ConnSummary::~RIPPS_ConnSummary () {
	if(m_pStats != NULL) {
		delete m_pStats;
		m_pStats = NULL;
	}
}

void RIPPS_ConnSummary::dumpBrief () {
	cout << "     Conn @ ";

	time_t		theTime;
	struct tm	 *  loctime;

	char		szOutBuffer[256];
	
	theTime = m_firstPkt.tv_sec;
	loctime = localtime(&theTime);

	strftime (szOutBuffer, 256, "%d %b %y  %H:%M:%S", loctime);  
	cout << szOutBuffer << " to "; 	
	 
	dumpIPv4(m_ExtIP);
	cout << " (" << m_nExtPort << " : " << m_nIntPort << ")  LRTT: " << m_fLRTT * 1000.0 << " ms on " << m_nPkts << " packets (SD " << m_fStdDev * 1000.0  << " ms) " << m_pStats->Get_Stat(RIPPS_MONPKT_STAT_PURGED_ACKS) << " MM  " << m_nRetransmissions << " RT" << endl;	
}

unsigned int RIPPS_ConnSummary::getMonitorSet () {
	return m_nMonitorSet;
}

unsigned int RIPPS_ConnSummary::getPacketCount () {
	return m_nPkts;	
}

double RIPPS_ConnSummary::getLRTT () {
	return m_fLRTT;
}

NodeDOM * RIPPS_ConnSummary::getStats (NodeDOM * pRoot) {
	return NULL;	
}

RIPPS_Connection::RIPPS_Connection () {
	m_bValidSeq = 0;
	
	// Clear the connection to zero
	memset(&m_IntIP, 0, 16);

	// Clear the connection to zero
	memset(&m_ExtIP, 0, 16);

	
	// Initialize all of the time structs
	memset(&m_lastData, 0, sizeof(struct timeval));
	memset(&m_lastAck, 0, sizeof(struct timeval));
	memset(&m_firstPkt, 0, sizeof(struct timeval));
	
	// Connection is not considered close (as of yet)
	m_bCloseWAN = 0;
	m_bCloseMon = 0;
	
	m_nRetransmissions = 0;
}


RIPPS_Connection::~RIPPS_Connection () {
	//cout << "  Connection object destructor" << endl;
}

void	RIPPS_Connection::dumpBrief () {
	
}

void	RIPPS_Connection::dumpBrief_MonPkt () {
	cout << "    Connection details (Port: " << m_nIntPort << " to Dest IP: ";
	dumpIPv4(m_ExtIP);	
	cout << " : " << m_nExtPort << ")" << endl;

	cout << "      Mon Pkts:   Open " << m_listPkts.getCountOpen() << "  Matched: " << m_listPkts.getCountMatches() << endl;

	m_listPkts.dumpBriefList_Open();
}

char RIPPS_Connection::canSummarize (struct timeval * pCurTime) {
	struct timeval 	*		pLatest;
		
	// Need to determine which is greater, data or ack?	
	if(compareTime(&m_lastData, &m_lastAck)) {
		pLatest = &m_lastData;
	} else {
		pLatest = &m_lastAck;
	}
	
	// TODO Replace with actual fetching from the monitor module for the setting
	//if( (calcTimeDiff(pCurTime, pLatest) / 1000) >= g_theRIPPSMonitor.getIdleSummary()) {
	
	// Check to see the difference versus the current time
	if( (calcTimeDiff(pCurTime, pLatest) / 1000) >= 5000) {
		return 1;	
	} else {
		return 0;
	}		
}
	
void RIPPS_Connection::setMonitorSet (unsigned int nSet) {
	m_nMonitorSet = nSet;
}

void RIPPS_Connection::setSrcIP (char * pAddress) {
	memcpy(m_IntIP, pAddress, 4);
}


unsigned int RIPPS_Connection::getMonitorSet () {
	return m_nMonitorSet;
}
				
void RIPPS_Connection::determineLogName (char * pOut) {
	sprintf(pOut, "%d.%d.%d.%d-T%d-Set%d-SP%d-DP%d.csv", (unsigned char) m_IntIP[0], (unsigned char) m_IntIP[1], (unsigned char) m_IntIP[2], (unsigned char)  m_IntIP[3], (unsigned int) m_firstPkt.tv_sec, m_nMonitorSet, m_nIntPort, m_nExtPort);		
}
		
unsigned short RIPPS_Connection::getRetransmissions () {
	return m_nRetransmissions;
}

void RIPPS_Connection::incRetransmissions () {
	m_nRetransmissions++;	
}
		
		
void RIPPS_Connection::dumpFile (char * pFile) {
	ofstream		outFile;
			
	outFile.open(pFile);
	
	// Output the header info (non-pkt listing) using a * comment field
	
	time_t		theTime;
	struct tm	 *  loctime;
	
	theTime = m_firstPkt.tv_sec;
	
	loctime = localtime(&theTime);
	
	char		szOutBuffer[512];
	
        /*
	strftime (szOutBuffer, 256, "*      First Pkt: %d %b %y    %H:%M:%S", loctime);  
	outFile << szOutBuffer << endl; 	
	
	outFile << "*      Mon Pkts:   Open " << m_listPkts.getCountOpen() << "  Matched: " << m_listPkts.getCountMatches() << endl;
        */

	int			j;
	RIPPS_MonitorPacket * 	pMonPkt;
	
	for(j=0; j<m_listPkts.getCountMatches(); j++) {
		pMonPkt = m_listPkts.getMonPkt(j);
		pMonPkt->formatShortDump(szOutBuffer);
                outFile << j << " ";
		outFile << szOutBuffer << endl;
	}	
	
							//	for(j=0; j<m_listPkts.getCountRetransmissions(); j++) {
							//		pMonPkt = m_listPkts.getRetransmission(j);
							//		pMonPkt->formatShortDump(szOutBuffer);
							//		outFile << "#RT# " << j << " ";
							//		outFile << szOutBuffer << endl;	
							//	}

							//	for(j=0; j<m_listPkts.getCountNoGobble(); j++) {
							//	  pMonPkt = m_listPkts.getNoGobble(j);
							//	  pMonPkt->formatShortDump(szOutBuffer);
	  //	  outFile << "#NG# " << j << " ";
	  //	  outFile << szOutBuffer << endl;
	  //	}
	
	outFile.close();
}		
		
void RIPPS_Connection::dumpDetail () {
	cout << "     Connection details (Port: " << m_nIntPort << " to Dest IP: ";
	dumpIPv4(m_ExtIP);	
	cout << " : " << m_nExtPort << ")" << endl;

	cout << "      Mon Pkts:   Open " << m_listPkts.getCountOpen() << "  Matched: " << m_listPkts.getCountMatches() << endl;
	
	time_t		theTime;
	struct tm	 *  loctime;
	
	theTime = m_firstPkt.tv_sec;
	
	loctime = localtime(&theTime);
	
	char		szOutBuffer[256];
	
	strftime (szOutBuffer, 256, "      First Pkt: %d %b %y    %H:%M:%S\n", loctime);  
	cout << szOutBuffer; 	
	
	m_listPkts.dumpBrief();
}


void RIPPS_Connection::noteRetransmission (Packet * pPacket) {
	// Note the presence of a retransmission
	
	RIPPS_MonitorPacket		*		pMonPkt;
	
	pMonPkt = RIPPS_MonitorPacket::createMonPkt();
		
	if(pMonPkt == NULL) {
		//g_theArchive.archiveString("* Error allocating new MonitorPacket object in Connection::noteRetransmission");
		cerr << "Error: Memory allocation failed on monitor packet, ignoring recording of retransmission" << endl;
		cerr.flush();
		return;
	}

	pMonPkt->setArrivalTime(pPacket->getArrivalTime());		

	pMonPkt->setAck(getTCP_SeqNumber(pPacket->getData(), pPacket->getLength()) + getPacketLength(pPacket->getData(), pPacket->getLength()) - getIP_HeaderSize(pPacket->getData(), pPacket->getLength()) -  getTCP_HeaderSize(pPacket->getData(), pPacket->getLength()));

	pMonPkt->setPktSize(pPacket->getLength());

	m_lastData = *(pPacket->getArrivalTime());			
	
	m_listPkts.noteRetransmission(pMonPkt);
}

RIPPS_MonitorPacket *	 RIPPS_Connection::populateExpectedAck (Packet * pPacket) {
	RIPPS_MonitorPacket	*	pMonPkt;

	//cout << "    Connection::addExpectedAck " << endl;

	if(getTCPFlag_SYN(pPacket->getData(), pPacket->getLength()) || getTCPFlag_FIN(pPacket->getData(), pPacket->getLength())
	   || getTCPFlag_RST(pPacket->getData(), pPacket->getLength())) {
		//cout << "  Ignoring for ack due to presence of SYN or FIN" << endl;
		return NULL;
	}
	
	//cout << "      Passed flag check" << endl;
	
	pMonPkt = RIPPS_MonitorPacket::createMonPkt();

	//cout << "***---> Flag status (fresh info) is ";
	//printf("%d\n", pMonPkt->getFlag_Gobble());
	
	if(pMonPkt == NULL) {
		//g_theArchive.archiveString("* Error allocating new MonitorPacket object in Connection::addExpectedAck");
		cerr << "Error: Memory allocation failed on monitor packet, ignoring expected ack" << endl;
		cerr.flush();
		return NULL;
	}
	
	// Set the arrival time for appropriate computation of LRTT
	//   If we have shaped the packet (i.e. restricted release), make sure to take that into account
	//   and not to use the original capture time as we are measuring LRTT on the wire/wireless medium
	
	// TODO Build in the callback mechanism to fix this on the actual outbound write
	//      The main question will be how to do that in a way that does not break everything
	//      in a massive way, perhaps at the module level with an event?  
	//
	// Since ScaleBox makes it hard to do a callback on write (unlike the original hack we did for RIPPS),
	//  time will be biased if a large value for shaping delay is selected.  That is bad anyway but
	//  double bad while we are not accounting for it.  
	
	//if(pPacket->bRestrictedRelease) {
	//	pMonPkt->setArrivalTime(&(pPacket->releaseTime));
	//} else {
	//	pMonPkt->setArrivalTime(pPacket->getArrivalTime());
	//}
	
	pMonPkt->setArrivalTime(pPacket->getArrivalTime());
	
	//cout << "      Incoming packet:    Seq #: " << getTCP_SeqNumber(pPacket->byData, pPacket->nLength);
	//cout << "   Ack #" << getTCP_AckNumber(pPacket->byData, pPacket->nLength) << endl;
	//cout << "   AT: " << pPacket->capTime.tv_sec << " s, " << pPacket->capTime.tv_usec << " ms" << endl;
	
	// Non-TCP length
	
	//cout << "   IP length: " << getPacketLength(pPacket->byData, pPacket->nLength) << endl;
	//cout << "   Non-IP length: " << getPacketLength(pPacket->byData, pPacket->nLength) - 40 << endl;
	//cout << "   Guestimated Ack: " << getTCP_SeqNumber(pPacket->byData, pPacket->nLength) + getPacketLength(pPacket->byData, pPacket->nLength) - 
	//											getIP_HeaderSize(pPacket->byData, pPacket->nLength) -  getTCP_HeaderSize(pPacket->byData, pPacket->nLength) << endl;

	//cout << "      Setting ack from the packet" << endl;
	
	// Set the ack we expect to see on the response
	pMonPkt->setAck(getTCP_SeqNumber(pPacket->getData(), pPacket->getLength()) + getPacketLength(pPacket->getData(), pPacket->getLength()) - getIP_HeaderSize(pPacket->getData(), pPacket->getLength()) -  getTCP_HeaderSize(pPacket->getData(), pPacket->getLength()));

	setHighestSeq(pMonPkt->getAck());
	setValidHighestSeq(1);

	//cout << "      Adding the open ack to the list" << endl;

	pMonPkt->setPktSize(pPacket->getLength());

	// Prep the callback such that we will stamp packets correctly
	pMonPkt->setCallback(pPacket);
	
	// Update when we saw the last data packet
	m_lastData = *(pPacket->getArrivalTime());		
	
	//cout << "***---> Flag status is ";
	//printf("%d\n", pMonPkt->getFlag_Gobble());
	
	//if(pMonPkt->getFlag_Gobble()) {
	//	cout << "********** WARNING! Gobble is set somehow!!!!!!! " << endl;
	//}
	
	//cout << "    *Exit* -> Connection::addExpectedAck " << endl;	
	return pMonPkt;
}

void RIPPS_Connection::setHighestSeq (unsigned int lSeq) {
	m_lHighestSeq = lSeq;
}

unsigned int RIPPS_Connection::getHighestSeq () {
	return m_lHighestSeq;
}

void RIPPS_Connection::setValidHighestSeq (char bFlag) {
	m_bValidSeq = bFlag;
}

char RIPPS_Connection::getValidHighestSeq () {
	return m_bValidSeq;
}

int RIPPS_Connection::getIPLength () {
	// TODO Fix this correctly
	return m_nIntLength;
}

void RIPPS_Connection::addExpectedAck (RIPPS_MonitorPacket * pMonPkt) {
	// Add this to the list of packets we are looking for acks for
	if(m_listPkts.addOpenPacket(pMonPkt)) {
		// OK, nothing to do
	} else {
		cerr << "Error: Unable to add expected ack to list, releasing monitor info back to pool" << endl;
		pMonPkt->release();	
	}
}	

/////////////////////////////////////////////////////////////////////

RIPPS_MonitorPacket * RIPPS_Connection::matchAck (Packet * pPacket) {
	RIPPS_MonitorPacket	*	pMonPkt;
	
	m_lastAck = *(pPacket->getArrivalTime());
	
	//cout << "  Searching for ack #" << getTCP_AckNumber(pPacket->getData(),pPacket->getLength()) << endl;
	
	pMonPkt = m_listPkts.findPacket_byExpAck(getTCP_AckNumber(pPacket->getData(),pPacket->getLength()));

	if(pMonPkt == NULL) {
		//cout << "**<- No match found! ->**" << endl;
		return NULL;
//		return MATCH_RESULT_NOACK;
	}

	//cout << " **** Match found!" << endl;

	// OK, we found one!
	pMonPkt->setDepartureTime(pPacket->getArrivalTime());
	
	//	m_listPkts.applyMatchToPacket(pMonPkt);
	
	return pMonPkt;
//	return MATCH_RESULT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////

char RIPPS_Connection::applyMatch (RIPPS_MonitorPacket * pMonPkt) {
	return m_listPkts.applyMatchToPacket(pMonPkt);
}

/////////////////////////////////////////////////////////////////////
	
void RIPPS_Connection::populateInfo (Packet * pPacket) {
	m_nStatus = CONN_STATUS_OPEN;
	m_firstPkt = *(pPacket->getArrivalTime());
	
	// Validate IPv4
	
	if(getIPVersion(pPacket->getData(), pPacket->getLength()) != 4) {
		cerr << " Ignoring data packet -> not IPv4 protocol" << endl;
		return;
	}

	// This function is called when a new connection is identified from the WAN
	//  side of the monitor.  Therefore, the destination IP / destination port
	//  must be inside the monitor part of the network and the source IP / source port
	//  are external to the monitor portion of the network.
	
	memcpy(m_ExtIP, pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP,4);
	m_nIntLength = 4;
	
	memcpy(&m_nExtPort, pPacket->getData()+L2_OFFSET+L3_IPV4_OFFSET+OFFSET_TCP_SRCPORT, 2);
	m_nNetExtPort = m_nExtPort;
	m_nExtPort = ntohs(m_nNetExtPort);

	// Internal side of network
	
	memcpy(&m_nIntPort, pPacket->getData()+L2_OFFSET+L3_IPV4_OFFSET+OFFSET_TCP_DSTPORT, 2);
	m_nNetIntPort = m_nIntPort;
	m_nIntPort = ntohs(m_nNetIntPort);	
	
	
	m_firstPkt = *(pPacket->getArrivalTime());		
}

struct timeval	RIPPS_Connection::getLastData () {
	return m_lastData;	
}

struct timeval	RIPPS_Connection::getFirstData () {
	return m_firstPkt;	
}

char * RIPPS_Connection::getIP_External () {
	return m_ExtIP;	
}

char * RIPPS_Connection::getIP_Internal () {
	return m_IntIP;	
}

unsigned short	RIPPS_Connection::getPort_Internal () {
	return m_nIntPort;
}

unsigned short RIPPS_Connection::getPort_External () {
	return m_nExtPort;
}

NodeDOM * RIPPS_Connection::getStats (NodeDOM * pRoot) {
	NodeDOM	*	pNode;
	NodeDOM *	pDataNode;
	NodeElem * 	pElem;	
	
	pNode = NodeDOM::createNodeDOM();
	pNode->setTag("Connection");
	
	pRoot->addChild(pNode);
	
	pElem = NodeElem::createNodeElem();
	pElem->setName("ExtIP");
	pElem->setValue(IPv4toString(m_ExtIP));
	pNode->addElement(pElem);
	
	pElem = NodeElem::createNodeElem();
	pElem->setName("IntIP");
	pElem->setValue(IPv4toString(m_IntIP));
	pNode->addElement(pElem);	
	
	pElem = NodeElem::createNodeElem();
	pElem->setName("ExtPort");
	pElem->setValue(m_nExtPort);
	pNode->addElement(pElem);	
	
	pElem = NodeElem::createNodeElem();
	pElem->setName("IntPort");
	pElem->setValue(m_nIntPort);
	pNode->addElement(pElem);	

	pElem = NodeElem::createNodeElem();
	pElem->setName("MonitorSet");
	pElem->setValue(m_nMonitorSet);
	pNode->addElement(pElem);
		
	char	szTemp[64];
	
	sprintf(szTemp, "%d.%d", m_lastData.tv_sec, m_lastData.tv_usec);
	pDataNode = NodeDOM::createNodeDOM();
	pDataNode->setTag("LastData");
	pDataNode->setData(szTemp);
	pNode->addChild(pDataNode);	
	
	sprintf(szTemp, "%d.%d", m_lastAck.tv_sec, m_lastAck.tv_usec);
	pDataNode = NodeDOM::createNodeDOM();
	pDataNode->setTag("LastACK");
	pDataNode->setData(szTemp);
	pNode->addChild(pDataNode);		
		
	sprintf(szTemp, "%d.%d", m_firstPkt.tv_sec, m_firstPkt.tv_usec);
	pDataNode = NodeDOM::createNodeDOM();
	pDataNode->setTag("FirstPkt");
	pDataNode->setData(szTemp);	
	pNode->addChild(pDataNode);

	sprintf(szTemp, "%d", m_nRetransmissions);
	pDataNode = NodeDOM::createNodeDOM();
	pDataNode->setTag("DataRTs");
	pDataNode->setData(szTemp);
	pNode->addChild(pDataNode);	
	
	return pNode;	
}
		
/////////////////////////////////////////////////////////		
		
List_RIPPSMonitorPkts *	RIPPS_Connection::getListPkts() {
	return &m_listPkts;	
}

/////////////////////////////////////////////////////////

char	RIPPS_Connection::matchConnection (Packet * pPacket, char bReverse) {	
					
	if(!bReverse) {
		if(memcmp(m_ExtIP, pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP,4) != 0) {
			return 0;
		}
				
		if(m_nExtPort != getTCP_SrcPort(pPacket->getData())) {
			return 0;
		}
		
		if(m_nIntPort != getTCP_DstPort(pPacket->getData())) {
			return 0;
		}
	} else {
		if(memcmp(m_ExtIP, pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP,4) != 0) {
			return 0;
		}		
		
		if(m_nIntPort != getTCP_SrcPort(pPacket->getData())) {
			return 0;
		}
		
		if(m_nExtPort != getTCP_DstPort(pPacket->getData())) {
			return 0;
		}		
	}

	return 1;
}

/////////////////////////////////////////////////////////

RIPPS_ListConnStats::RIPPS_ListConnStats () {
	Allocate(RIPPS_LISTCONN_STAT_LAST);		
}

/////////////////////////////////////////////////////////

void	RIPPS_ListConnStats::Get_Title	(int nStat, char * szTitle) {
	switch(nStat) {
		case 	RIPPS_LISTCONN_STAT_NODATACONN:
			sprintf(szTitle, "ConnsNoData");
			break;	
		case RIPPS_LISTCONN_STAT_FINDMISS_EXT:
			sprintf(szTitle, "FindMissExternal");
			break;	
		case RIPPS_LISTCONN_STAT_FINDMISS_INT:
			sprintf(szTitle, "FindMissInternal");
			break;			
		default:
			sprintf(szTitle, "F%03d", nStat);
			break;			
	}
}

/////////////////////////////////////////////////////////

List_RIPPSConnections::List_RIPPSConnections () {

}

/////////////////////////////////////////////////////////

List_RIPPSConnections::~List_RIPPSConnections () {
	clear();
}

/////////////////////////////////////////////////////////

void	List_RIPPSConnections::dumpBrief () {
	unsigned int		j;
	RIPPS_ConnSummary * pSum;
	RIPPS_Connection *	pConn;
	
	cout << "    " << m_summaries.size() << " finished monitoring" << endl;
		
	for(j=0; j<m_summaries.size(); j++) {
		pSum = m_summaries[j];
		pSum->dumpBrief();
	}
	
	cout << "    " << m_listConns.size() << " open connection(s) currently being monitored" << endl;
	for(j=0; j<m_listConns.size(); j++) {
		pConn = m_listConns[j];
		
		pConn->dumpDetail();
	}	
}

/////////////////////////////////////////////////////////
		
void List_RIPPSConnections::dumpBrief_MonPkt () {
	unsigned int		j;
	RIPPS_Connection *	pConn;
		
	cout << "    " << m_listConns.size() << " open connection(s) currently being monitored" << endl;
	for(j=0; j<m_listConns.size(); j++) {
		pConn = m_listConns[j];
		
		pConn->dumpBrief_MonPkt();
	}	
}

/////////////////////////////////////////////////////////
		
void List_RIPPSConnections::dumpDetail () {
	unsigned int		j;
	RIPPS_Connection *	pConn;
	
	for(j=0; j<m_listConns.size(); j++) {
		pConn = m_listConns[j];
		
		pConn->dumpDetail();
	}
}

/////////////////////////////////////////////////////////

void List_RIPPSConnections::clear () {
	RIPPS_Connection	*	pConn;
	int				j;
	
	for(j=0; j<m_listConns.size(); j++) {
		pConn = m_listConns[j];
		delete pConn;
	}
	
	m_listConns.clear();
}

/////////////////////////////////////////////////////////

void List_RIPPSConnections::addConnection (RIPPS_Connection * pConnection) {
	m_listConns.push_back(pConnection);
}

/////////////////////////////////////////////////////////

RIPPS_Connection * List_RIPPSConnections::findConnection (Packet * pPacket, char bReverse) {
	RIPPS_Connection		*		pConn;
	int			j;

	if(getIPVersion(pPacket->getData(), pPacket->getLength()) != 4) {
		cerr << " Ignoring packet -> not IPv4 protocol" << endl;
		return NULL;
	}


	// Key off of destination address in the packet first
	for(j=0; j<m_listConns.size(); j++) {
		pConn = m_listConns[j];
			
		if(pConn->matchConnection(pPacket, bReverse)) {
			return pConn;
		}
	}
	
	if(bReverse) {
		m_Stats.Add_Stat(RIPPS_LISTCONN_STAT_FINDMISS_EXT, 1);
	} else {
		m_Stats.Add_Stat(RIPPS_LISTCONN_STAT_FINDMISS_INT, 1);
	}

	return NULL;
}

/////////////////////////////////////////////////////////

RIPPS_Connection * List_RIPPSConnections::	getOpenConnection (int nIndex) {
	if(nIndex < 0 || nIndex >= m_listConns.size()) {
		return NULL;
	}
	
	return m_listConns[nIndex];	
}

/////////////////////////////////////////////////////////

bool List_RIPPSConnections::removeConnection (RIPPS_Connection * pConn) {
	vector<RIPPS_Connection *>::iterator		listIterate;
	int			j;

	if(m_listConns.size() == 0) {
		cerr << "Error: List of connections is zero length!" << endl;
		cerr << "   List_Connections::removeConnection" << endl;
		return false;
	}

	listIterate = m_listConns.begin();
	
	for(j=0; j<m_listConns.size(); j++) {
		if(*(listIterate) == pConn) {
			m_listConns.erase(listIterate);
			delete pConn;
			return true;
		}
		
		listIterate++;
	}	
	
	cerr << "Error: Unable to find connection to remove from list of connections" << endl;
	cerr << "   Deleting connection anyway" << endl;
	delete pConn;
	cerr << "   Delete successful!" << endl;
	return false;		
}

///////////////////////////////////////////////////////////

bool List_RIPPSConnections::createSummary (RIPPS_Connection * pConn, bool bPurgeEmpty) {
	RIPPS_ConnSummary	*	pSummary;
	List_RIPPSMonitorPkts	*	pPktList;
	
	if(pConn == NULL) {
		cerr << "Connection to summarize is NULL, avoiding summary" << endl;
		return false;
	}
	
	// This operation should always succeed as the list is always allocated
	//  as part of the connection object
	pPktList = pConn->getListPkts();
	
	// Check to see if the connection actually has data
	if(pPktList->getCountMatches() == 0) {
		//cout << "No data detected, ignoring" << endl;
		m_Stats.Add_Stat(RIPPS_LISTCONN_STAT_NODATACONN, 1);
		removeConnection(pConn);	
		return false;				
	}
	
	// Create a summary first
	pSummary = new RIPPS_ConnSummary(pConn);
	m_summaries.push_back(pSummary);
	
	removeConnection(pConn);	
	return true;
}

///////////////////////////////////////////////////////////

unsigned int List_RIPPSConnections::getMonitorCount (unsigned int nSet) {
	unsigned int		nSum;
	unsigned int		j;
	
	// First check the summaries that match the specified set
	nSum = 0;
	
	for(j=0; j<m_summaries.size(); j++) {
		if(nSet == 0 || m_summaries[j]->getMonitorSet() == nSet) {
			nSum += m_summaries[j]->getPacketCount();
		} else if(m_summaries[j]->getMonitorSet() > nSet) {
			break;
		}
	}	

	for(j=0; j<m_listConns.size(); j++) {
		List_RIPPSMonitorPkts * 	pList;	

		if(nSet == 0 || m_listConns[j]->getMonitorSet() == nSet) {	
			pList = m_listConns[j]->getListPkts();
			nSum += pList->getCountMatches();
		}
	}
	
	return nSum;
}

double List_RIPPSConnections::getAvgLRTT (unsigned int nSet) {
	unsigned int		nPkts;
	unsigned int		j;
	double				fSum;
	
	// First check the summaries that match the specified set
	fSum = 0;
	nPkts = 0;
	
	for(j=0; j<m_summaries.size(); j++) {
		if(nSet == 0 || m_summaries[j]->getMonitorSet() == nSet) {
			nPkts += m_summaries[j]->getPacketCount();
			fSum += m_summaries[j]->getPacketCount() * m_summaries[j]->getLRTT(); 
		} else if(m_summaries[j]->getMonitorSet() > nSet) {
			break;
		}
	}	

	for(j=0; j<m_listConns.size(); j++) {
		List_RIPPSMonitorPkts * 	pList;	

		if(nSet == 0 || m_listConns[j]->getMonitorSet() == nSet) {	
			pList = m_listConns[j]->getListPkts();
			nPkts += pList->getCountMatches();
			fSum += pList->getCountMatches() * pList->computeLRTT(); 
		}
	}
	
	return fSum / (double) nPkts;	
}

void List_RIPPSConnections::summarizeAll (char * pHostAddress) {
	while(m_listConns.size() > 0) {
		m_listConns[0]->setSrcIP(pHostAddress);		
		createSummary(m_listConns[0]);
	}	
}

unsigned int List_RIPPSConnections::getOpenCount () {
	return m_listConns.size();
}

void List_RIPPSConnections::archiveOldData (unsigned int nSet) {
	// TODO Fill in code to archive old datasets	
}

NodeDOM * List_RIPPSConnections::getStats (NodeDOM * pRoot) {
	NodeDOM	*	pNode;
	NodeElem *	pElem;
	int			j;
	
	pNode = NodeDOM::createNodeDOM();
	pNode->setTag("ConnectionList");
	
	pElem = NodeElem::createNodeElem();
	pElem->setName("ActiveConns");
	pElem->setValue(m_listConns.size());
	pNode->addElement(pElem);
	
	pElem = NodeElem::createNodeElem();
	pElem->setName("SummarizedConns");
	pElem->setValue(m_summaries.size());
	pNode->addElement(pElem);
	
	pRoot->addChild(pNode);
	
	for(j=0; j<m_listConns.size(); j++) {
		m_listConns[j]->getStats(pNode);
	}

	for(j=0; j<m_summaries.size(); j++) {
		m_summaries[j]->getStats(pNode);
	}	
	
	return pNode;	
}

