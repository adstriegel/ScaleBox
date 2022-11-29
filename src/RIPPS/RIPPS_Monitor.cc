/* RIPPS_Monitor.cpp
 **********************************************************
 * This code is part of the RIPPS (Rogue Identifying
 * Packet Payload Slicer) system developed at the University
 * of Notre Dame. 
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Chad Mano (lead)      Yingxin Jiang
 *           Dave Salyers          Dave Cieslak
 *           Qi Liao               Andrew Blaich   
 * 
 **********************************************************
 * $Revision: 1.23 $  $Date: 2008/04/11 22:52:01 $
 ********************************************************** 
 */

#include <iostream>
using namespace std;

#include "RIPPS_Monitor.h"
#include "RIPPS_Host.h"
#include "../util/ip-utils.h"
#include "../mon/Thread_Archive.h"
#include "../mon/Thread_Timer.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

//////////////////////////////////////////////////////////////

RIPPS_MonitorStats::RIPPS_MonitorStats () : Stats () {
	Allocate(RIPPS_MONITOR_STAT_LAST);	
}

//////////////////////////////////////////////////////////////

void RIPPS_MonitorStats::Get_Title	(int nStat, char * szTitle) {	
	switch(nStat) {
		case 	RIPPS_MONITOR_STAT_PKTS_EXT_INCOMING:
			sprintf(szTitle, "ExtInboundPkts");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_EXT_FILTER:
			sprintf(szTitle, "ExtFilterPkts");
			break;		
		case 	RIPPS_MONITOR_STAT_PKTS_EXT_VALIDATED:
			sprintf(szTitle, "ExtValidatePkts");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_INT_OUTGOING:
			sprintf(szTitle, "IntOutboundPkts");
			break;	
		case 	RIPPS_MONITOR_STAT_PKTS_INT_FILTER:
			sprintf(szTitle, "IntFilterPkts");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_INT_UNKHOST:
			sprintf(szTitle, "IntUnknownHostPkts");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_INT_ACK_UNKHOST:
			sprintf(szTitle, "IntUnknownHostAckPkts");
			break;	
		case 	RIPPS_MONITOR_STAT_PKTS_INT_FIN_UNKHOST:
			sprintf(szTitle, "IntUnknownHostFinPkts");
			break;
		case 	RIPPS_MONITOR_STAT_HOST_VALIDATIONS:
			sprintf(szTitle, "HostValidations");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_GOBBLE:
			sprintf(szTitle, "GobbledPkts");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_GOBBLE_DATA:
			sprintf(szTitle, "MarkGobbleDataPkts");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_GOBBLE_YUM:
			sprintf(szTitle, "PureAckGobbles");
			break;	
		case 	RIPPS_MONITOR_STAT_PKTS_EXT_SMALL:
			sprintf(szTitle, "NoSliceTooSmall");
			break;	
		case 	RIPPS_MONITOR_STAT_PKTS_EXT_PUREACK:
			sprintf(szTitle, "ExtPureAcks");
			break;
		case 	RIPPS_MONITOR_STAT_PKTS_SLICE_STING:
			sprintf(szTitle, "StingSlicePkts");
			break;						
		case 	RIPPS_MONITOR_STAT_PKTS_POST_SHAPE:
			sprintf(szTitle, "ShapedPktCount");
			break;									
		case 	RIPPS_MONITOR_STAT_TIMER_SHAPE_QUEUE:
			sprintf(szTitle, "TimerShapeQueue");
			break;		
		case 	RIPPS_MONITOR_STAT_TIMER_POST_SHAPE:
			sprintf(szTitle, "TimerPostShape");
			break;	
		case 	RIPPS_MONITOR_STAT_PKTS_SLICED:
			sprintf(szTitle, "SlicePktCount");
			break;																																						
		default:
			sprintf(szTitle, "F%03d", nStat);
			break;
	}			
}

//////////////////////////////////////////////////////////////

RIPPS_Monitor::RIPPS_Monitor () {
	int		j;
	
	m_bEnableMonitor = true;
	
	m_nSliceTarget = DEFAULT_SLICE_TARGET;
	m_nSliceDelay = DEFAULT_SLICE_DELAY;
	
	m_nMeasureTarget = DEFAULT_MEASURE_TARGET;
	
	m_nMonitorInterval = DEFAULT_MONITOR_INTERVAL;
	
	m_nIdleSummary = DEFAULT_IDLE_SUMMARY;
	
	// The push gap is the additional shaping delay for packets that have the PUSH flag
	//  set
	m_nPushGap = DEFAULT_PUSH_GAP;
	
	m_bNoSlice = 0;
	m_bMonitorSmall = 1;	
	m_bRoutePkts = 1;
			
	m_bReadOnly = 0;
	m_bShowRead_Ext = 0;
	m_bShowRead_Int = 0;	
	
	m_nPurgeTCPOpt = 0;	
	
	m_nPktID = 0;
	
	m_nWindowSize = 0;
	
	m_bLimitShapeRelease = 0;
	
	m_bNoShape = 0;
			
	
	m_nStingSort = STINGSORT_SIMPLE;
	
	m_nSliceGap = DEFAULT_SLICE_GAP;

	m_bMonitorOutAcks = 1;
	
	m_bCanSlice = true;
}

RIPPS_Monitor::~RIPPS_Monitor () {

}

//////////////////////////////////////////////////////////////

bool RIPPS_Monitor::extractExtendedDOM (NodeDOM * pNode) {
	int		j;
	string	sTag;
	string	sData;

	for(j=0; j<pNode->getNumChildren(); j++) {
		sTag = pNode->getChild(j)->getTag();
		sData = pNode->getChild(j)->getData();
		
		if(sTag == "canSlice") {
			setFlag_CanSlice(translateStringToBoolean(sData));		
		} else if(sTag == "numRTTsMeasure") {
			setValue_NumRTTsMeasure(translateStringToInt(sData));
		} else if(sTag == "sliceSize") {
			setValue_SliceSize(translateStringToInt(sData));
		} else if(sTag == "monitorInterval") {
			setValue_MonitorInterval(translateStringToInt(sData));
		} else if(sTag == "sliceDelay") {
			setValue_SliceDelay(translateStringToInt(sData));
		}		
	}
	
	return true;	
}

//////////////////////////////////////////////////////////////

int RIPPS_Monitor::mapNameToID (string sName) {
	if(sName == "WAN") {
		return RIPPS_TAP_WAN;
	} else if (sName == "External") {
		return RIPPS_TAP_WAN;		
	} else if (sName == "Internal") {
		return RIPPS_TAP_MONITORLAN;	
	} else if (sName == "MonitorLAN") {
		return RIPPS_TAP_MONITORLAN;
	} else {
		return RIPPS_TAP_NONE;
	}	
}

//////////////////////////////////////////////////////////////

char RIPPS_Monitor::processTapPacket (Packet * pPacket, int nID) {
	// Will assume that the filter is set up to only pass on IP packets
	//  to the RIPPS module itself, i.e. the tap will only give us
	//  a packet if it is indeed TCP/IP
	
	// TODO Use the actual filter rather than half arsing it
	
	if(!isPacketIP (pPacket->getData(), pPacket->getLength())) {
		return 0;
	}
	
	if(!isTCP(pPacket->getData(), pPacket->getLength())) {
		return 0;
	}
	
	// Hard filtering for the NGW (yes, this is a kludge)
	
	// IP is 10.0.0.1
	// IP is whatever the WAN side gives up (please not 10.0.0.*
	
	
	if(nID == RIPPS_TAP_WAN) {
		// The packet came from the WAN side
		return processPacketWAN(pPacket);
	} else if (nID == RIPPS_TAP_MONITORLAN) {
		// The packet came from the LAN side
		return processPacketMonitor(pPacket);
	} else {
		// Who knows, we don't know why we are getting these packets in particular	
		
	}
	
	return 0;	
}

//////////////////////////////////////////////////////////////////////////

char RIPPS_Monitor::initialize () {

	//cout << "Invoking initialization" << endl;

	// Initialize the thread guard for the host listing	
	pthread_mutex_init(getMutexHostList(), NULL);
	
	enableTimers();

	// Set up a timer to come around and help us clean
	TimerEvent	*		pTimer;
	
	pTimer = TimerEvent::createTimer();
	pTimer->setData(NULL);		

	pTimer->setTimerID(RIPPS_MONITOR_TIMER_MOD_CLEANER);
	pTimer->setInterval_ms(1000);
	pTimer->armTimer();
				
	// Do something with the timer :)
	if(!addTimerEvent(pTimer)) {
		cout << "Error: Unable to add timer to the mix" << endl;
		pTimer->release();
		return 0;
	}	

	return 1;	
}

//////////////////////////////////////////////////////////////////////////

bool RIPPS_Monitor::processTimer (TimerEvent * pEvent) {
	if(pEvent == NULL) {
		return false;
	} 
	
	Packet * pPacket;
	RIPPS_Host *	pHost;
	RIPPS_MonitorPacket *	pMonPkt;
	
	
	switch(pEvent->getTimerID()) {
		// Dispatch one packet for a specific host back onto the chain
		case RIPPS_MONITOR_TIMER_SHAPE_QUEUE:
			//cout << "Timer: Shape queue" << endl;
			m_Stats.Add_Stat(RIPPS_MONITOR_STAT_TIMER_SHAPE_QUEUE, 1);
			
			pHost = (RIPPS_Host *) pEvent->getData();
			
			pPacket = pHost->popShapedPacket();
			
			//printf(" Shaped Pkt ID = 0x%X\n", pPacket);
			
			// This may seem counterintuitive that we are kicking it back to the 
			// WAN-side adapter since the packet will eventually cross over the
			// LAN (monitor) side. The resume operation kicks and eventually it
			// hits the passthrough which eventually does our bidding :)
						
			// Is there an actual packet?
			if(pPacket != NULL) {
				pMonPkt = (RIPPS_MonitorPacket * ) pPacket->getWriteCallbackData();
				
				if(pMonPkt != NULL) {
					pMonPkt->setArrivalTime(pEvent->getInvocationTime());
					pPacket->registerWriteCallback(NULL, NULL);
				} else {
					// Nothing to do
				}
								
				if(!resumePostTap(RIPPS_TAP_WAN, pPacket)) {
					cout << "Warning: Failed to resume after tap, unable to find LAN tap attached to RIPPS" << endl;	
					cout << "  Discarding packet to the memory pool without routing onwards" << endl;
					pPacket->release();
				} else {
					//cout << "  Packet successfully kicked out via the tap" << endl;
				}
			} else {
				cerr << " Warning: Nothing to pop from the shape queue" << endl;
			}
			
			// Release the timer event back to the pool
			pEvent->release();	
			//cout << "End of Timer: Shape queue" << endl;					
			break;	
		
		// Dispatch all remaining packets for a specific host back onto the chain	
		case RIPPS_MONITOR_TIMER_POST_SHAPE:
			//cout << "Timer: Post Shape" << endl;		
			m_Stats.Add_Stat(RIPPS_MONITOR_STAT_TIMER_POST_SHAPE, 1);
			
			pHost = (RIPPS_Host *) pEvent->getData();
									
			pPacket = pHost->popShapedPacket();									
									
			while( pPacket != NULL) {			
				// Is there an actual packet?
				if(pPacket != NULL) {
					resumePostTap(RIPPS_TAP_WAN, pPacket);
				}
				
				// Keep going until the shape queue is empty
				pPacket = pHost->popShapedPacket();
			}
			
			// Release the timer event back to the pool
			pEvent->release();
			//cout << "End of Timer: Post Shape" << endl;								
			break;	
			
		// Attempt to clean up the connection information for the specified
		//  host as the connections have likely been idle for > 1 s	
		case RIPPS_MONITOR_TIMER_CONN_CLEAN:
			m_Stats.Add_Stat(RIPPS_MONITOR_STAT_TIMER_CLEAN_HOST, 1);
			
			pHost = (RIPPS_Host *) pEvent->getData();
									
			// Have the host do cleanup on old connections (summarize them)
			pHost->checkSummarizeConnections(pEvent->getInvocationTime());
			
			// Release the timer event back to the pool
			pEvent->release();			
			break;	
			
		case RIPPS_MONITOR_TIMER_MOD_CLEANER:
			//cout << "Timer: Module RIPPS cleaner" << endl;
		
			int		nHost;
		
			if(getHostCount() > 0) {
				nHost = rand() % getHostCount();
				
				pHost = getHost(nHost);
				
				if(pHost != NULL) {
					pHost->checkSummarizeConnections(pEvent->getInvocationTime());
				}
			}
		
			// Re-arm and go again
			pEvent->armTimer();
		
			if(!addTimerEvent(pEvent)) {
				cout << "Error: Unable to add timer to the mix" << endl;
				pEvent->release();
				return 0;
			}	
			
			break;								
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////////

char RIPPS_Monitor::processPacketWAN (Packet * pPacket) {
	m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_EXT_INCOMING,1);
	
	// TODO - Replace this static offsetting with a dynamic function from the packet
	//        class that actually extracts the address
	
	// Extract a pointer to the destination address 
	char	*	pDestAddress;
	
	pDestAddress = pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP;
	
	// Is this internal host excluded from RIPPS monitoring for all of its packets?
	if(filterPacket_Always(pDestAddress, DEFAULT_IP_LEN)) {
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_EXT_FILTER, 1);
		return 0;
	}
	
	RIPPS_Host	*	pHost;
	
	pHost = findHost(pDestAddress, DEFAULT_IP_LEN);

	if(pHost == NULL) {
		// Create a new host - this is infrequent so we don't need to manage
		//  it via the memory pool
		pHost = new RIPPS_Host();	
		pHost->setIP(pDestAddress);
		pHost->setValidationState(VALIDATE_STATE_RIPPS);
					
		if(!addHost(pHost)) {
			// Ruh roh, operation failed, delete the object but send the packet onwards
			//  anyway
			delete pHost;
			return 0;
		}
	}

	// See if this host needs to be monitored 
	if(!shouldMonitorHost(pHost)) {		
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_EXT_VALIDATED, 1);
		
		// Host may be validated, but does it have a backlog and hence new packets should
		// be ordered behind it?
		if(pHost->hasBacklog()) {
			// We will "shape" the host but only in the sense for preserving order for
			//  TCP packets to that host.  UDP order is not preserved as those flows are
			//  not subject to shaping at this time.  In some sense, it is much like new
			//  flows originated from the host or UDP packets sourced at the host where we
			//  have no control over.    			
			
			shapePacketHost(pHost, pPacket, true);
					
			// TODO Resolve the issue of if / when UDP traffic should be shaped in line with TCP traffic
			//      or at least post a warning to that effect in the documentation
						
			m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_POST_SHAPE, 1);				
			return 1;			
		} else {
			// No backlog, let the normal passthrough take care of things
			return 0;	
		}
	} else {							
		if(getFlag_CanSlice()) {
			return slicePacket(pHost, pPacket);			
		} else { 	
			// Not slicing which means let it flow on through to the passthrough				 			
			//cout << "* Avoiding slicing of packet, just recording expected ack" << endl;
			pHost->addExpectedAck(pPacket);								
			return 0;
		}
	}	
}	
				
//////////////////////////////////////////////////////////////////////////				
												
char RIPPS_Monitor::processPacketMonitor (Packet * pPacket) {	 			
	m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_INT_OUTGOING, 1);

	//pPacket->enableTracing();

	// TODO - Replace this static offsetting with a dynamic function from the packet
	//        class that actually extracts the address
	
	// Extract a pointer to the destination address 
	char	*	pSrcAddress;
	
	pSrcAddress = pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP;
	
	// Is this internal host excluded from RIPPS monitoring for all of its packets?
	if(filterPacket_Always(pSrcAddress, DEFAULT_IP_LEN)) {
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_INT_FILTER, 1);
		return 0;
	}
			
	char		nResult;
	int			nFwdResult;
		
	// Default is to forward
	nFwdResult = HOST_PKT_FWD;
	
	// Outbound, extract the host information
	RIPPS_Host 	*		pHost;
	
	pHost = findHost(pSrcAddress, DEFAULT_IP_LEN);

	// If we have not seen the host before, odds are that we should not do anything
	// to the packet as we don't have any information anyway to match up or gobble.
	// Sure, it might be good to note that said host exists but if it is TCP, we
	// will eventually see something (data or ACK) coming in from the external
	// side
	
	if(pHost == NULL) {
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_INT_UNKHOST, 1);
		return 0;
	}
		
	// TODO The mechanism for grabbing address fields, ACK numbers, and such is
	//      just plain ugly and ill-suited for anything beyond IPv4.  We should
	//  	  really refactor this to encapsulate information extraction in a
	//		  derived object or something to that effect. We do way too much
	//		  fixed offsetting which will be bad later on.
	
	if(getTCPFlag_ACK(pPacket->getData(), pPacket->getLength())) {	
		nFwdResult = pHost->processAck(pPacket);
		matchAck(pHost, pPacket);
	} else {
		nFwdResult = HOST_PKT_FWD;
	}
		
	// Clean up any finished connections
  	if(getTCPFlag_FIN(pPacket->getData(), pPacket->getLength()) || getTCPFlag_RST(pPacket->getData(), pPacket->getLength())) {
  		//cout << "** Outbound FIN or RST" << endl;
		terminateConnection(pHost, pPacket, RIPPS_TAP_MONITORLAN);
		// Never gobble a FIN or RST packet
		return 0;
  	} 

	// To gobble or not to gobble, that is the question
	//   By gobbling a packet, we prevent the sliced TCP fast retransmit packet
	//   from making its merry way out to the external Internet which will generally
	//   be a bad thing if we can avoid it
  	if(nFwdResult == HOST_PKT_GOBBLE) {		
  		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_GOBBLE, 1);
  		
		//cout << " Yum, yum, yum, delicioso ack of" << endl;
		// char szOut[1024];
		//		pPacket->dumpBriefString(szOut, 1024);
		// cout << szOut;
   		
		// Add in a caveat that we let data rich packets escape
		if(!isPureAck(pPacket->getData(), pPacket->getLength())) {
			m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_GOBBLE_DATA, 1); 
			return 0;
		} else {
			m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_GOBBLE_YUM, 1); 			
			gobblePacket(pPacket);			
			return 1;						
		}					
   } else {  	
   		//cout << "  Fwd packet" << endl;
   		//cout << "OUT: ";
   		//pPacket->dumpBrief();
	  	// cout << "The ack will escape!" << endl;
	  
	  	// char  szTemp[1025];

	  	// pPacket->dumpBriefString(szTemp, 1024);
	  	// cout << szTemp;

	  	// if(pHost != NULL) {
	   //pHost->dumpBrief_withOpen();
	  	//}
  	}   

	// By default, don't gobble the packet and let it eventually escape through
	//  the passthrough module to the other adapter	
	return 0;
}

/////////////////////////////////////////////////////////////////////////

bool RIPPS_Monitor::shapePacketHost (RIPPS_Host * pHost, Packet * pPacket, bool bPostShape) {
	TimerEvent	*		pTimer;
		
	pTimer = TimerEvent::createTimer();
	
	pTimer->setData(pHost);

	// Most of the time, a shaped packet implies that we need to set the timer for
	//  a shaped dispatch, i.e. shaping is in full effect yo.  However, when the host
	//  is finished with validation, packets need no longer be shaped which means
	//  only order preservation is required.  In such a case, a flush timer is set that
	//  will attempt to purge the contents of the queue at roughly the same time of
	//	the last shape dispatch. Nominally, all measurements are done anyway meaning
	//  that the data/ACK pair is going to be ignored for reference purposes.  
	
	// Also, we can dodge the whole shape query o rama thing if the slice delay is zero,
	//  simply put in the timer to purge the queue and voila
	
	if(getValue_SliceDelay() > 0 && !bPostShape) {
		pTimer->setTimerID(RIPPS_MONITOR_TIMER_SHAPE_QUEUE);		
		pTimer->setInvocationTime(pHost->computeShapeDispatch());
	} else {
		pTimer->setTimerID(RIPPS_MONITOR_TIMER_POST_SHAPE);
		pTimer->setInvocationTime(pHost->getShaper_LastTime());
	}

	pHost->addShapedPacket(pPacket);	
	
	// Do something with the timer :)
	if(!addTimerEvent(pTimer)) {
		pTimer->release();
		return false;
	}

	return true;	
}

/////////////////////////////////////////////////////////////////////////

char RIPPS_Monitor::slicePacket (RIPPS_Host * pHost, Packet * pPacket) {

	unsigned int			nLengthLeft;
	unsigned short		nSlice;				// How much to slice
	unsigned int			nCumSent;			// How much we have sent out of the packet
	unsigned int			nSeqBase;			// Starting point for the sequence number
	unsigned int			nNewSeq;			// New sequence number for the packet
	int			nIndex;
	char			byHost[4];
	int				nDataOffset;
	
	char				bFlagSYN;
	char				bFlagFIN;
	char				bFlagRST;
	int				j;
	bool				bPushSet;
	
	vector <Packet *>	outQueue;
	

	//printf("RIPPS_Monitor::slicePacket (0x%08X - Len %d)\n", pPacket, pPacket->getLength());
	//pPacket->enableTracing();
	//printf("  pHost: 0x%X\n", pHost);
	//pPacket->dumpBrief();

	// Adjust the data packet number but ignore SYN, FIN, or RST packets
	pHost->incDataPktNum();

	///////////////////
	// Skip slicing of SYN or FIN packets
	
	bFlagSYN = getTCPFlag_SYN(pPacket->getData(), pPacket->getLength());
	bFlagFIN = getTCPFlag_FIN(pPacket->getData(), pPacket->getLength());
	bFlagRST = getTCPFlag_RST(pPacket->getData(), pPacket->getLength());
		
	if(bFlagSYN || bFlagFIN || bFlagRST) {	   	
		
		// TODO To be removed
   	/*if(bFlagFIN || bFlagRST) {
   		// Terminate the connection
   		//cout << "TERMINATION from External Side!!!!" << endl;
			//terminateConnection(pHost, pPacket, RIPPS_TAP_WAN);
   	}*/
   	
		//cout << " Skipping slicing - SYN or FIN or RST flags are set" << endl;
		//pDispatch = pHost->calcNextDispatch(NULL, 0);
   		
   	shapePacketHost (pHost, pPacket);	
   		
		// The packet will automatically be pulled out from the host queue
		//  via the normal timing functionality   			
		//
		// The packet should not continue on the normal input chain as we
		//  have now effectively queued it for shaping via the timing queue
		
   	return 1;		
	}
			
	nLengthLeft = pPacket->getLength();
	
	//cout << "  Packet length: " << nLengthLeft << endl;

	unsigned short		nBaseLength;
	
	nBaseLength = getPacketLength(pPacket->getData(), pPacket->getLength());

	// Subtract out the TCP and IP headers
	nDataOffset = L2_OFFSET + getIP_HeaderSize(pPacket->getData(), pPacket->getLength()) + getTCP_HeaderSize(pPacket->getData(), pPacket->getLength());

	//nLengthLeft -= nDataOffset;
	nLengthLeft = nBaseLength - getIP_HeaderSize(pPacket->getData(), pPacket->getLength()) - getTCP_HeaderSize(pPacket->getData(), pPacket->getLength());
	

	//cout << "** New Packet **" << endl;
	//cout << "  Slicing the packet of an initial payload of " << nLengthLeft << " bytes" << endl; 
	//cout << "    Pkt Capture Length: " << pPacket->nLength << endl;
	//cout << "    Slice Target:       " << g_theMonitor.calcSliceAmount() << endl;
	//cout << "    TCP Seq: " << (unsigned int) getTCP_SeqNumber(pPacket->byData,pPacket->nLength) << endl;
	//cout << "    TCP Ack: " << (unsigned int) getTCP_AckNumber(pPacket->byData,pPacket->nLength) << endl;


	//cout << "    Data Offset: " << nDataOffset << endl;
	//cout << "     Consisting of " << L2_OFFSET << " L2 bytes, " << (int) getIP_HeaderSize(pPacket->byData, pPacket->nLength) << " L3 bytes, " << (int) getTCP_HeaderSize(pPacket->byData, pPacket->nLength) << " L4 bytes" << endl;
	//cout << "     Payload of " << nLengthLeft << " bytes" << endl;

	// Calculate the base sequence number
	nSeqBase = getTCP_SeqNumber(pPacket->getData(),pPacket->getLength());
	//nSeqBase -= nLengthLeft;
	nCumSent = 0; 
	
	nIndex = 0;
	

	//cout << "    Src IP: ";
	//dumpIPv4(pPacket->byData+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP);
	//cout << endl;
	
	//cout << "    Dst IP: ";
	//dumpIPv4(pPacket->byData+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP);
	//cout << endl;

	// Skip the slicing business and cut to the chase

//	if(isPureAck(pPacket->byData, pPacket->nLength)) {
//		cout << " Pure Ack!!!" << endl;
//	}
			
		
	// Is this packet a retransmission and should we ignore it?
	if(!pHost->shouldMonitorPkt_WAN(pPacket)) {
		//cout << " ** Packet is a retransmission" << endl;
		
		/* New school dispatch mechanism */	   		
   	shapePacketHost (pHost, pPacket);	
   		
		// The packet will automatically be pulled out from the host queue
		//  via the normal timing functionality   			
		//
		// The packet should not continue on the normal input chain as we
		//  have now effectively queued it for shaping via the timing queue
		
   	return 1;		

		
		//pPacket->restrictRelease(pDispatch);		
		
		//cout << "*** Ignoring a retransmission" << endl;		
	}
		
	// Don't do any sort of slicing or shaping or pure ACK packets
	if(isPureAck(pPacket->getData(), pPacket->getLength())) {
		//cout << "  ** Packet is a pure ACK" << endl;
		/* New school dispatch mechanism */	   		
   		shapePacketHost (pHost, pPacket);	
   	
   		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_EXT_PUREACK, 1);
   		
		// The packet will automatically be pulled out from the host queue
		//  via the normal timing functionality   			
		//
		// The packet should not continue on the normal input chain as we
		//  have now effectively queued it for shaping via the timing queue
	
		return 1;
	}
	
	//cout << "Checking to see if the packet is too small" << endl;
	
	// Is the packet payload less than or equal to the slice amount, i.e. no
	//  slicing is required as we are already there?
	
	if(nLengthLeft <= getValue_SliceSize()) {
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_EXT_SMALL,1);		
		
		/* New school dispatch mechanism */	   		
   		shapePacketHost (pHost, pPacket);	
   		
		// The packet will automatically be pulled out from the host queue
		//  via the normal timing functionality   			
		//
		// The packet should not continue on the normal input chain as we
		//  have now effectively queued it for shaping via the timing queue
									
	  	return 1;				
	}
	
	// No sting for now, just packet shaping
	
	//cout << " Surrender, just kick it out" << endl;
	
	//shapePacketHost(pHost, pPacket);
	//return 1;
	
	// TCP Sting
	//
	//   TCP Sting (Savage - USENIX 98) rearranges the TCP packets to force additional acks.  In
	//   short, this takes the Sting work and pushes it much, much harder.  In short, we take the
	//   first packet and place it just before the end to force a gap and hence acks in between
	//   as there is a non-contiguous block of data.  In the absence of a NAK, we will see a
	//   significant increase in the number of ACKs.  This comes at the potential of ACK
	//   ambiguity, i.e. now we can only loosely match up an ACK as the ACK # on the forced ACK
	//   does not change.  We know the number we should see but if losses occur, we will misalign
	//   the observed LRTT (increasing it), especially in the case of shaped outputs.  
		
	m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_SLICE_STING, 1);		
	
	// Calculate the base sequence number
	nSeqBase = getTCP_SeqNumber(pPacket->getData(),pPacket->getLength());		
	nCumSent = 0; 	
	nIndex = 0;			
	
	// On the push flag, it should only be set on the last packet, not the first one nor
	//  individual packets	
	bPushSet = getTCPFlag_PSH(pPacket->getData(), pPacket->getLength());
		
	if(bPushSet) {
		//cout << "  Push flag was set, clearing the flag" << endl;
		setTCPFlag_PSH(pPacket->getData(), pPacket->getLength(), 0);
	}
	
	// Iterate through the packet creating slices appropriately
	while(nLengthLeft >= 0) {
				
		// Figure out how much to include in this slice
		nSlice = getValue_SliceSize();
		
		if(nSlice >= nLengthLeft) {
			nSlice = nLengthLeft;
			nLengthLeft = 0;
			
			// If the push flag was set, enable it here for the last packet
			if(bPushSet) {
				setTCPFlag_PSH(pPacket->getData(), pPacket->getLength(), 1);
			}
						
		} else {
			nLengthLeft -= nSlice;			
		}
				
		// Update the cumulative bytes we have sent
		nCumSent += nSlice;
		
		// Calc and write the new sequence number to the packet
		nNewSeq = nSeqBase + nCumSent - nSlice;
		
						
		// Create a new packet
		Packet		*	pSplitPacket;

		pSplitPacket = Packet::createPacket();
		
		if(pSplitPacket == NULL) {
			// Uh oh, ran out of memory - we are probably in trouble
			
			// TODO Clean out the currently allocated vector of sliced packets
						
			cerr << " Error: No space in the memory pool for new packets" << endl;				

			// TODO Signal to the central monitor that we are in big trouble!!!
			return 0;			
		}
		
		// Should have a valid pSplitPacket container at this point
				
		// Copy the intro data to the new packet holder
		memcpy(pSplitPacket->getData(), pPacket->getData(), nDataOffset);
		
		pSplitPacket->setArrivalTime(pPacket->getArrivalTime());				
		pSplitPacket->setLength(nDataOffset + nSlice);
		
		// Copy the partial subset of data to the packet as the actual data
		//	 The slice size is dictated by the monitor and is randomized to prevent successful spoofing
		//    by the end host
		
		//cout << " Copying the packet contents" << endl;
		//cout << "  Copying to: " << nDataOffset << endl;
		//cout << "  Copying from: " << nDataOffset+nCumSent-nSlice << " to " << nDataOffset + nCumSent << endl;
		//cout << "  Copying " << nSlice << " bytes" << endl;
		
		memcpy(pSplitPacket->getData()+nDataOffset, pPacket->getData()+nDataOffset+nCumSent-nSlice, nSlice);
						
		// Update the packet appropriately with a new TCP seq #, a new length, and updated checksums
		writeTCP_SeqNumber(nNewSeq, pSplitPacket->getData(), pSplitPacket->getLength());
		
		// Write the new length to the packet
		setPacketLength(pSplitPacket->getData(), nSlice+getIP_HeaderSize(pSplitPacket->getData(), pSplitPacket->getLength()) + getTCP_HeaderSize(pSplitPacket->getData(), pSplitPacket->getLength()));
	
		// Update the TCP checksum (if necessary)
		updateIPv4_Checksum(pSplitPacket->getData(), pSplitPacket->getLength());
				
		// Update the IP checksum
		updateTCP_Checksum(pSplitPacket->getData(), pSplitPacket->getLength());			

/*								
		if(g_theMonitor.getOption_PurgeTCPOpt()) {
			// Add in healthier/faster TCP option munging in a more efficient manner later
			unsigned short		nAdjust;
						
			if(nIndex > 0) {	
				nAdjust = purgeTCPOptions(pSplitPacket->byData, pSplitPacket->nLength); 
				pSplitPacket->nLength -= nAdjust;
			} else if (nIndex == 0 && g_theMonitor.getOption_PurgeTCPOpt() == 2) {
				nAdjust = purgeTCPOptions(pSplitPacket->byData, pSplitPacket->nLength); 
				pSplitPacket->nLength -= nAdjust;				
			}			
		}						
*/

  		//g_theArchive.archiveString("* TCP Sting packet for inner LAN sending\n");
		//g_theArchive.archivePacket(pSplitPacket);
				
		outQueue.push_back(pSplitPacket);
				
		if(nLengthLeft == 0) {
			break;
		}
		
		nIndex++;				
	}
	
	//cout << "* Done w/slicing of packets" << endl;
	
	unsigned int     nExpSeqNum;

	nExpSeqNum = getTCP_SeqNumber (pPacket->getData(), -1);

	//cout << "Starting sequence number for the rearranged blocks is " << (unsigned int) nExpSeqNum << endl;

	// Gobble the original as we certainly don't have any business with that packet :)
	gobblePacket(pPacket);
				
	int k;

	// Only need to re-arrange if we have more than 2 newly sliced packets
	//
	//    P0 P1 P2 .... PN-2 PN-1
	//      re-arrange to
	//    P1 P2 P3 ...  P0 PN-1
	
	//	for(k=0; k<outQueue.size(); k++) {
	//  printf("  Address of outQ[%d] is 0x%X\n", k, outQueue[k]);
	//}

	if(getStingSortMode() == STINGSORT_PSHEND) {
		if(outQueue.size() > 2) {			
			// Do the swap-a-roo dance to offer the TCP Sting behavior
			Packet *	pFirstPacket;
			
			pFirstPacket = outQueue[0];
		
			// Shift everyone else down by one		
			for(k=1; k<outQueue.size()-1; k++) {
				outQueue[k-1] = outQueue[k];
			}
		
			// Place the first packet later in the sequence
			outQueue[outQueue.size()-2] = pFirstPacket;
		}
	} else if(getStingSortMode() == STINGSORT_SIMPLE) {
		// Alt method -> put the out of order packet at the end of the burst
		if(outQueue.size() > 2) {
		  Packet * pFirstPacket;
	
		  pFirstPacket = outQueue[0];
	
		  for(k=1; k<outQueue.size(); k++) {
		    outQueue[k-1] = outQueue[k];
		  }
	
		  outQueue[outQueue.size()-1] = pFirstPacket;
		}
	} 

	//cout << "* Done w/rearranging of packets" << endl;


	//for(k=0; k<outQueue.size(); k++) {
	//  printf("  Address of outQ[%d] is 0x%X\n", k, outQueue[k]);
	//}

			
	// Dispatch the queue for output to the adapter
	//   This is a TCP Sting variant, packets should be gobbled
	
	int					nQueueSize;
	Packet *		pDispPkt;
			
	nQueueSize = outQueue.size();

	// Iterate through the queue to release each packet with appropriate shaping
	for(j=0; j<nQueueSize; j++) {
		//cout << "  [" << j << "] dispatch pkt" << endl;
		pDispPkt = outQueue[j];
		
/*		if(getStingSortMode() == STINGSORT_PSHEND) {
			bFlagPSH = getTCPFlag_PSH (pDispPkt->getData(), pDispPkt->getLength());
		} else {
			bFlagPSH = 0;
		}*/

		// Spacing if appropriate for shaping the packets (i.e. inter-packet gap)
		//
		//  The host is referenced in case there is a previous backlog to ensure that
		//  adequate spacing occurs, regardless of the backlog		
		
		/* New school dispatch mechanism */	   		
   		shapePacketHost (pHost, pDispPkt);	
					   			   					
		// Update the host information on the monitor regarding this specific packet
		//  Gobble if appropriate all but the last packet in the listing

		if(getStingSortMode() == STINGSORT_PSHEND) {
			// Three cases:
			//   - The initial packets all ACK'ing the same contiguous block
			//	 - The 2nd to last which will cumulatively ack all pkts
			//	 - The last packet which is the only one not to be gobbled
			if(j<nQueueSize-2) {
			 	// Set the expected ack to be the same as the incoming sequence number since we will
				// see the same sequence number due to the gap in packet reception
				//cout << "  [" << j << "] Gobbled expected ack of " << (unsigned int) nExpAck << endl;				
				
				// In short, if this is the last of the TCP Sting ACKs, add in dummy
				//  superfluous ACK catchers to prevent excess ACKs escaping
				if(j+1 == nQueueSize-2) {
					pHost->addExpectedAck(pDispPkt, GOBBLE_PKT_YES, nExpSeqNum, 5);
				} else {
					pHost->addExpectedAck(pDispPkt, GOBBLE_PKT_YES, nExpSeqNum, 0);
				}					
			} else if(j == nQueueSize-2) {
				// This one is trickier, the expected ack is for the contiguous block through the
				// next to last packet	
			  	//cout << "  [" << j << "] Gobbled next to last ack of " << (unsigned int) (nExpAck+getValue_SliceSize()*(j+1)) << endl;			  
			  	// Add in an expected ACK + 2 superfluous ACK catchers
				pHost->addExpectedAck(pDispPkt, GOBBLE_PKT_YES, (unsigned int) nExpSeqNum+ (unsigned int) getValue_SliceSize()*(j+1), 2); 
			} else {
				// Add this one as normal			
			  	//cout << "  [" << j << "] No gobble of ack for final ack" << endl;
			  	//cout << "    Size of packet is " << getPacketLength(pDispPkt->byData, -1) << endl;
			  	//cout << "    Seq # of packet is " << getTCP_SeqNumber(pDispPkt->byData, -1) << endl;
				pHost->addExpectedAck(pDispPkt);				
			}
		}
		else if (getStingSortMode() == STINGSORT_SIMPLE) {
		  	// Alt method
		  	//   No longer sorted from     1 2 3 4 5 6 to 2 3 4 5 1 6
		  	//   Sort direct order w/swap  2 3 4 5 6 1	
		  	if(j<nQueueSize-1) {
		    	if(j+1 == nQueueSize-1) {
		      		pHost->addExpectedAck(pDispPkt, GOBBLE_PKT_YES, nExpSeqNum, 5);
		    	} else {
		      		pHost->addExpectedAck(pDispPkt, GOBBLE_PKT_YES, nExpSeqNum, 0);
		    	}
		  	} else {
		    	pHost->addExpectedAck(pDispPkt, GOBBLE_PKT_SEQONLY, nExpSeqNum, 0);
		  	}
		}
		
		//cout << " --> Finished expected ack processing" << endl;
		
		//char szTemp[500];

		//sprintf(szTemp, "--> Adding the %d'th packet to the out queue of the adapter\n", j);
		//g_theArchive.archiveString(szTemp);
		//g_theArchive.archivePacket(pDispPkt);

		//cout << " X-Mit bytes --> start at " << (unsigned int) getTCP_SeqNumber(pDispPkt->byData, -1) << endl;
						
		// Once we give the packet to the adapter, it is the under the ownership of the
		//  adapter (memory-wise)		
		//g_theMonitor.addOutPacket(m_pAdapter, pDispPkt);
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_SLICED,1);		
	}		

	//printf("***  Finished slicing the packet\n");
	return 1;
	//cout << " Finished dispatch queue **" << endl;
}

///////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::terminateConnection (RIPPS_Host * pHost, Packet * pPacket, int nDirection) {
	// The input really should not be a NULL
	if(pHost == NULL) {
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_PKTS_INT_FIN_UNKHOST, 1);
		return;
	}
	
	if(nDirection == RIPPS_TAP_MONITORLAN) {
  		//cout << "TERMINATION from Monitor Side!!!!" << endl;
		pHost->closeConnectionMon(pPacket);
	} else {
  		//cout << "TERMINATION from External Side!!!!" << endl;	
		pHost->closeConnectionExt(pPacket);
		
		// Set up a timer to come around and help us clean
		TimerEvent	*		pTimer;
		
		pTimer = TimerEvent::createTimer();
		pTimer->setData(pHost);		
	
		pTimer->setTimerID(RIPPS_MONITOR_TIMER_CONN_CLEAN);
		pTimer->setInterval_ms(1500);
		pTimer->armTimer();
					
		// Do something with the timer :)
		if(!addTimerEvent(pTimer)) {
			cout << "Error: Unable to add timer to the mix" << endl;
			pTimer->release();
			return;
		}		
	}		
}

/////////////////////////////////////////////////////////////

RIPPS_Host * RIPPS_Monitor::findHost (char * pAddress, int nAddrLen) {
	RIPPS_Host		*		pHost;

	// Try to find the host in the list
	pthread_mutex_lock(getMutexHostList());
	pHost = m_HostList.findHost_byIP(pAddress);
	pthread_mutex_unlock(getMutexHostList());
	
	return pHost;
}

/////////////////////////////////////////////////////////////

pthread_mutex_t *	RIPPS_Monitor::getMutexHostList () {
	return &m_MutexHostList;	
}

/////////////////////////////////////////////////////////////

bool	RIPPS_Monitor::addHost (RIPPS_Host * pHost) {
	RIPPS_Host 	* pDuplicate;
	
	if(pHost == NULL) {
		cerr << "Warning: attempted to add a NULL host to the monitor list, ignoring request" << endl;
		return false;
	}

	// Attempt to find it in the list			
	pDuplicate = findHost(pHost->getIP(), DEFAULT_IP_LEN);
	
	if(pDuplicate != NULL) {
		cerr << "Warning: attempted to add a duplicate host to the list where the host was already" << endl;
		cerr << "  in the list" << endl;
		dumpIPv4(pHost->getIP());
		printf("\n");
		return false;
	}
	
	pthread_mutex_lock(getMutexHostList());
	m_HostList.addHost(pHost);
	pthread_mutex_unlock(getMutexHostList());
	
	return true;
}

//////////////////////////////////////////////////////////////////////

bool RIPPS_Monitor::matchAck (RIPPS_Host * pHost, Packet * pPacket) {
	
	if(pHost == NULL) {
		return false;
	}
		
	if(pHost->getValidationState() == VALIDATE_STATE_RESOLVED) {		
		return true;
	}
		
	// Should that host be freed from monitoring (i.e. fast path for processing)?
	if(pHost->getMonitorCount() >= getValue_NumRTTsMeasure()) {		
		// Transition the host to a non-monitoring state
		pHost->setValidationState(VALIDATE_STATE_RESOLVED);
		
		m_Stats.Add_Stat(RIPPS_MONITOR_STAT_HOST_VALIDATIONS, 1);
		
		// TODO Add in code to support archival logs on a per-module basis registered
		//      via the central monitor or logger
		
		//char		szTemp[200];
		// Whoot, the host is now free!
		//g_theArchive.archiveTime();
		//sprintf(szTemp, " R Finished validation for host %d.%d.%d.%d\n", (unsigned char) pAddress[0], (unsigned char) pAddress[1], 
		//						(unsigned char) pAddress[2], (unsigned char) pAddress[3]);
								
		//g_theArchive.archiveString(szTemp); 
		
		pHost->setLastValidation(pPacket->getArrivalTime());
		
		// Temporarily disabled in light of gobbling.  Connection summaries should really be generated
		//  anyway by virtue of the FIN or RST flags of TCP, not manually like this
		// pHost->summarizeAllConnections();

		// Figure out if the host is wireless/etc. here as well
		int		nResult;
		
		nResult = pHost->validateHost();
		
		// TODO Do something with the host validation result
	}		
	
	return true;
}

/////////////////////////////////////////////////////////////////////

bool RIPPS_Monitor::filterPacket_Always (char * pData, int nLength) {
	return false;
}

/////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::measureAll () {	
	pthread_mutex_lock(getMutexHostList());

	m_HostList.forceValidationState(VALIDATE_STATE_RIPPS);
	
	pthread_mutex_unlock(getMutexHostList());	
}

/////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::checkValidations () {
	struct timeval		curTime;

	gettimeofday(&curTime, NULL);	
	
	pthread_mutex_lock(getMutexHostList());

	//m_HostList.checkValidations (&curTime, getValue_MonitorInterval());
	
	pthread_mutex_unlock(getMutexHostList());
}

/////////////////////////////////////////////////////////////////////

bool RIPPS_Monitor::shouldMonitorHost (RIPPS_Host * pHost) {
	if(pHost == NULL) {
		return false;
	} else {
		if(pHost->getValidationState() == VALIDATE_STATE_RESOLVED) {
			return false;
		} else {
			return true;
		}		
	}
}

//////////////////////////////////////////////////////////////

void RIPPS_Monitor::setValue_SliceSize (int nSliceSize) {
	if(nSliceSize > 0) {
		m_nSliceTarget = nSliceSize;
	} else {
		// Do nothing
		cerr << "Warning: Ignoring illegal value of " << nSliceSize << " for slice size" << endl;	
	}
}

//////////////////////////////////////////////////////////////

int RIPPS_Monitor::getValue_SliceSize () {
	return m_nSliceTarget;
}

//////////////////////////////////////////////////////////////

void	RIPPS_Monitor::setFlag_CanSlice (bool bSlice) {
	m_bCanSlice = bSlice;
}

//////////////////////////////////////////////////////////////

bool	RIPPS_Monitor::getFlag_CanSlice 	() {
	return m_bCanSlice;
}

//////////////////////////////////////////////////////////////

void RIPPS_Monitor::setValue_SliceDelay (unsigned int nDelay) {
	m_nSliceDelay = nDelay;
}

//////////////////////////////////////////////////////////////

unsigned int RIPPS_Monitor::getValue_SliceDelay () {
	return m_nSliceDelay;	
}

//////////////////////////////////////////////////////////////

unsigned int RIPPS_Monitor::getValue_NumRTTsMeasure () {
	return m_nMeasureTarget;	
}

////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::setValue_NumRTTsMeasure (unsigned int nTarget) {
	m_nMeasureTarget = nTarget;	
}

////////////////////////////////////////////////////////////////////

unsigned int RIPPS_Monitor::getValue_MonitorInterval () {
	return m_nMonitorInterval;	
}

////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::setValue_MonitorInterval (unsigned int nInterval) {
	m_nMonitorInterval = nInterval;
}

////////////////////////////////////////////////////////////////////

unsigned short	RIPPS_Monitor::getNextID () {
	return m_nPktID++;	
}

////////////////////////////////////////////////////////////////////
		
RIPPS_Host * RIPPS_Monitor::getHost (int nIndex) {
	RIPPS_Host * pHost;
	
	// Guard with the appropriate thread safety with reference to
	// changes in the content of the host list
	pthread_mutex_lock(getMutexHostList());
	pHost = m_HostList.getHost(nIndex);	
	pthread_mutex_unlock(getMutexHostList());
	
	return pHost;
}
		
////////////////////////////////////////////////////////////////////		
		
int RIPPS_Monitor::getHostCount () {
	int			nCount;

	// Guard with the appropriate thread safety with reference to
	// changes in the content of the host list
	pthread_mutex_lock(getMutexHostList());
	nCount = m_HostList.getSize();
	pthread_mutex_unlock(getMutexHostList());	

	return nCount;
}

////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::setValue_IdleSummary (unsigned int nMS) {
	m_nIdleSummary = nMS;	
}

////////////////////////////////////////////////////////////////////
		
unsigned int RIPPS_Monitor::getValue_IdleSummary () {
	return m_nIdleSummary;
}

////////////////////////////////////////////////////////////////////

void	RIPPS_Monitor::checkSummarizeConnections () {
	int		nHosts;
	int		nOrigCount;
	RIPPS_Host * 	pHost;
	
	nHosts = getHostCount();
	nOrigCount = nHosts;
	
	struct timeval 		curTime;	
	
	// What is the current system time?
	gettimeofday(&curTime, NULL);	
	
	while(nHosts > 0) {
		// Bail out if the host list has been modified
		if(getHostCount() != nOrigCount) {
			break;
		}
		
		pHost = getHost(nHosts-1);
		
		if(pHost != NULL) {
			pHost->checkSummarizeConnections(&curTime);
		}				
		
		nHosts--;
	}
}

////////////////////////////////////////////////////////////////////

void RIPPS_Monitor::setStingSortMode (unsigned char bStingSort) {
	m_nStingSort = bStingSort;
}

////////////////////////////////////////////////////////////////////

unsigned char RIPPS_Monitor::getStingSortMode () {
	return m_nStingSort	;
}

////////////////////////////////////////////////////////////////////

void	RIPPS_Monitor::setThresholdWireless (double fVal) {
	m_fThresholdWireless = fVal;	
}

////////////////////////////////////////////////////////////////////

double RIPPS_Monitor::getThresholdWireless () {
	return m_fThresholdWireless;
}

////////////////////////////////////////////////////////////////////

void	RIPPS_Monitor::setThresholdWired (double fVal) {
	m_fThresholdWired = fVal;
}

////////////////////////////////////////////////////////////////////

double RIPPS_Monitor::getThresholdWired () {
	return m_fThresholdWired;
}

////////////////////////////////////////////////////////////////////

NodeDOM * RIPPS_Monitor::getStats (NodeDOM * pRoot) {
	NodeDOM *	pNode;
	RIPPS_Host *	pHost;
	int				j;
	
	pNode = m_Stats.getStats(pRoot, "RIPPS");	
	
	if(pNode != NULL) {
		// Add the info here since it was successful in the stats object

		for(j=0; j<getHostCount(); j++) {
			pHost = getHost(j);
			
			if(pHost != NULL) {
				pHost->getStats(pNode);	
			}
		}			
	}	
	
	return pNode;
}

////////////////////////////////////////////////////////////////////

CommandResult	RIPPS_Monitor::processExtCommand (const vector<string> & theCommands, int nOffset) {
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}
	
	if(theCommands[nOffset] == "status") {
		pthread_mutex_lock(getMutexHostList());	
		m_HostList.dumpBrief();
		pthread_mutex_unlock(getMutexHostList());	
		return PROCESS_CMD_SUCCESS;		
	}
	
	return PROCESS_CMD_UNKNOWN;	
}

////////////////////////////////////////////////////////////////////

bool RIPPS_Monitor::processWriteCallback (Packet * pPacket, void * pData) {
	RIPPS_MonitorPacket	*	pMonPkt;
	
	pMonPkt = (RIPPS_MonitorPacket *) pData;
	
	if(pMonPkt == NULL) {
		return false;
	} else {
		struct timeval			pollSysTime;		
		
		// Sample the current time		
		gettimeofday(&pollSysTime, NULL);	
		
		// This has a wee bit o danger associated with it, it may be better to
		// just do an estimate of the shaping penalty applied rather than
		// trying to get a real sample
		pMonPkt->setArrivalTime(&pollSysTime); 
	}	
}

