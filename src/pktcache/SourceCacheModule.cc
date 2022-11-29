/* SourceCacheModule.cc
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * Development and equipment support has been provided in
 *  part by the following sponsors:
 * 
 *    National Science Foundation    CNS03-47392
 *    Sun Microsystems Academic Excellence Grant
 *    Intel IXP Development Grant 
 *    University of Notre Dame Faculty Grant
 * 
 **********************************************************
 */

#include "SourceCacheModule.h"

#include "../mem/MemoryPool.h"
#include "../pkt/Packet.h"
#include "../util/ip-utils.h"

#include "../xml/NodeDOM.h"

#include "PktCacheTable.h"

#include "PCPacketFmt.h"

#include "../core/LogEvent.h"


//////////////////////////////////////////////////////////////

PktCacheSrcModStats::PktCacheSrcModStats () : Stats () {
	Allocate(PKTCACHE_SRCMOD_STAT_LAST);	
}

//////////////////////////////////////////////////////////////

void PktCacheSrcModStats::Get_Title	(int nStat, char * szTitle) {	
	switch(nStat) {
		case PKTCACHE_SRCMOD_STAT_NUM_PKTS:
			sprintf(szTitle, "NumPktsAll");
			break;
		
		case PKTCACHE_SRCMOD_STAT_NUM_PKTS_SRC:
			sprintf(szTitle, "NumPktsSrcSide");
			break;
		
		case PKTCACHE_SRCMOD_STAT_NUM_PKTS_CHILD:
			sprintf(szTitle, "NumPktsChildSide");
			break;
		
		case PKTCACHE_SRCMOD_STAT_NUM_PKTS_NONIP:
			sprintf(szTitle, "NumPktsNonIP");
			break;
			
		case PKTCACHE_SRCMOD_STAT_SIZE_PKTS_NONIP:
			sprintf(szTitle, "SizePktsNonIP");
			break;
			
		case PKTCACHE_SRCMOD_STAT_NUM_PKTS_IPV6:
			sprintf(szTitle, "NumPktsIPv6");
			break;
			
		case PKTCACHE_SRCMOD_STAT_SIZE_PKTS_IPV6:
			sprintf(szTitle, "SizePktsIPv6");
			break;
			
		case PKTCACHE_SRCMOD_STAT_NUM_PKTS_NONUDPTCP:
			sprintf(szTitle, "NumPktsNotIPData");
			break;
			
		case PKTCACHE_SRCMOD_STAT_SIZE_PKTS_NONUDPTCP:
			sprintf(szTitle, "SizePktsNotIPData");
			break;

		case PKTCACHE_SRCMOD_STAT_NUM_PKTS_TOOSMALL:
			sprintf(szTitle, "NumPktsTooSmall");
			break;
			
		case PKTCACHE_SRCMOD_STAT_SIZE_PKTS_TOOSMALL:
			sprintf(szTitle, "SizePktsTooSmall");
			break;
			
		default:
			sprintf(szTitle, "F%03d", nStat);
			break;
	}			
}

////////////////////////////////////////////////////////////////////////

SourceCacheModule::SourceCacheModule () {
	m_nMinPayloadSize = PKTCACHE_DEFAULT_MINPAYLOAD_SIZE;
	
	m_nFingerprintMode = PKTCACHE_FINGERPRINT_MODE_WHOLE;
	m_nFingerprintType = PKTCACHE_DEFAULT_FINGERPRINT_TYPE;
	m_nFingerprintWindow = PKTCACHE_DEFAULT_FINGERPRINT_WINDOW;
	m_nFingerprintOffset = PKTCACHE_DEFAULT_FINGERPRINT_OFFSET;
	
}

////////////////////////////////////////////////////////////////////////

SourceCacheModule::~SourceCacheModule () {


}

////////////////////////////////////////////////////////////////////////

int SourceCacheModule::mapNameToID (string sName) {
	if(sName == "SourceSide") {
		return SOURCEPKTCACHE_TAP_SRC;
	} else if (sName == "ClientSide") {
		return SOURCEPKTCACHE_TAP_CHILD;
	} else {
		return SOURCEPKTCACHE_TAP_NONE;
	}	
}

////////////////////////////////////////////////////////////////////////

bool SourceCacheModule::extractExtendedDOM (NodeDOM * pNode) {
	int		j;
	string	sTag;
	string	sData;

	for(j=0; j<pNode->getNumChildren(); j++) {
		sTag = pNode->getChild(j)->getTag();
		sData = pNode->getChild(j)->getData();
		
		if(sTag == "numHashTables") {
			if(m_HashMgr.setNumberTables(translateStringToInt(sData))) {
				
			} else {
				return false;
			}
		} else if (sTag == "minPktSize") {
			
		}
	}	
	
	
	// Nothing to process for now
	return true;
}

////////////////////////////////////////////////////////////////////////

char SourceCacheModule::initialize () {
	return 1;
}
	
////////////////////////////////////////////////////////////////////////

char SourceCacheModule::processTapPacket (Packet * pPacket, int nID) {
	int		nStartPktCacheHdr;
	bool	bCacheResult;
	
	m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS, 1);
	
	if(nID == SOURCEPKTCACHE_TAP_SRC) {
		// Packet came from the source side (can we cache it)?
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS_SRC, 1);
	
		bCacheResult = attemptConservePacket(pPacket);
		
	} else if (nID == SOURCEPKTCACHE_TAP_CHILD) {
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS_CHILD, 1);
		// Packet came from the client side
	} else {
		// Unknown ID - do nothing with the packet tap as there was
		//  a configuration error in pushing it to us
		
		// Future: Need a way to flag this (fast) once and log it but not
		//  excessively spam the log
		return 0;
	}
		
	return 0;	
}

////////////////////////////////////////////////////////////////////////

bool SourceCacheModule::attemptConservePacket (Packet * pPacket) {
	// Packet must be IP
	if(!isPacketIP(pPacket->getData(), pPacket->getLength())) {
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS_NONIP, 1);
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_SIZE_PKTS_NONIP, pPacket->getLength());
		return false;
	}
	
	// Packet must be IPv4 - if it is IP but not v4, must be v6 
	if(getIPVersion(pPacket->getData(), pPacket->getLength()) != 4) {
		// Keep an eye on this for now, fix it later if volume is sufficient
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS_IPV6, 1);
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_SIZE_PKTS_IPV6, pPacket->getLength());
		return false;		
	}

	unsigned char		byProto;
	
	byProto = getIPv4Protocol(pPacket->getData());
	
	// Must be UDP or TCP for the protocol (for now)
	if ( byProto == IP_PROTOCOL_TCP || byProto == IP_PROTOCOL_UDP ) {
		
	} else {
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS_NONUDPTCP, 1);
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_SIZE_PKTS_NONUDPTCP, pPacket->getLength());
		return false;		
	}
	
	// What is the minimum size threshold for consideration?
	if(pPacket->getLength() < getValue_MinPayloadSize()) {
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_NUM_PKTS_TOOSMALL, 1);
		m_Stats.Add_Stat(PKTCACHE_SRCMOD_STAT_SIZE_PKTS_TOOSMALL, pPacket->getLength());
		return false;			
	}
	
	int		nPayloadOffset;
	
	nPayloadOffset = -1;
	
	// Made it here, are we doing TCP or UDP? We need to know so we can skip to the
	//  payload
	if(byProto == IP_PROTOCOL_TCP) {
		
	} else if (byProto == IP_PROTOCOL_UDP) {
		
	}
	
	switch(m_nFingerprintMode) {
		case PKTCACHE_FINGERPRINT_MODE_WHOLE:
			// Once we have the offset, we can begin the cache procedure
			attemptWholePacketCache(pPacket, nPayloadOffset);			
			break;
		case PKTCACHE_FINGERPRINT_MODE_PARTIAL:
			// Once we have the offset, we can begin the cache procedure
			attemptPartialPacketCache(pPacket, nPayloadOffset);			
			break;
	}
		
	return false;
}

////////////////////////////////////////////////////////////////////////	

int	SourceCacheModule::attemptPartialPacketCache (Packet * pPacket, int nOffset) {
	if(nOffset < 0) {
		return -1;
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////////////

int	SourceCacheModule::attemptWholePacketCache (Packet * pPacket, int nOffset) {
	
	return 0;
}

////////////////////////////////////////////////////////////////////////

NodeDOM * SourceCacheModule::getStats (NodeDOM * pRoot) {
	NodeDOM *	pNode;
	int				j;
	
	pNode = m_Stats.getStats(pRoot, "SrcPktCache");	
		
	return pNode;	
}

bool	SourceCacheModule::setValue_MinPayloadSize (int nMinSize) {
	if(nMinSize <= 0) {
		logFaultHighEvent("Warning: Bad value passed in for minimum payload size, ignoring", "SourceCacheModule", "setValue_MinPayloadSize");			
		return false;
	}
	
	m_nMinPayloadSize = nMinSize;
	
	return true;
}
		
int		SourceCacheModule::getValue_MinPayloadSize () {
	return m_nMinPayloadSize;
}


