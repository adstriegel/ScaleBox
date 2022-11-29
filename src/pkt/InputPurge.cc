/* InputPurge.cc
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
 * $Revision: 1.4 $  $Date: 2008/04/08 14:04:50 $
 **********************************************************
 */
 
#include <iostream>
using namespace std;

#include "InputPurge.h"
#include "../mem/MemoryPool.h"
#include "Packet.h"
#include "../util/ip-utils.h"

#include "../xml/NodeDOM.h"

InputPurge::InputPurge () : IOModule () {
	setType("InputPurge");	
	
	m_pStats = new StatsIOPurge();
}

InputPurge::~InputPurge () {
	
}

char InputPurge::initialize () {
	return 1;	
}


char InputPurge::processPacket (Packet * pPacket) {
	int		j;
	//cout << "IP - Purge ID'd a packet" << endl;

	// Does this apply to all (nothing listed)?
	if(m_CatchEthernet.size() == 0 && m_CatchIPv4.size() == 0) {
		purgePacket(pPacket);
		return 1;
	}
	
	if(m_CatchEthernet.size() > 0) {
		int		j;
		
		for(j=0; j<m_CatchEthernet.size(); j++) {
			NetAddressEthernet *	pAddrEth;
			
			pAddrEth = m_CatchEthernet[j];
			
			// Check destination exatct match
			if(*pAddrEth == pPacket->getData()+OFFSET_ETH_DSTMAC) {
				purgePacket(pPacket);
				return 1;
			}
			
			// Check source exact match
			if(*pAddrEth == pPacket->getData()+OFFSET_ETH_SRCMAC) {
				purgePacket(pPacket);
				return 1;
			}
		}
	}

	if(isPacketIP(pPacket->getData(), pPacket->getLength()) && m_CatchIPv4.size() > 0) {
		int		j;
		
		for(j=0; j<m_CatchIPv4.size(); j++) {
			NetAddressIPv4 *	pAddrIP;
			
			pAddrIP = m_CatchIPv4[j];
			
			// Check source exatct match
			if(*pAddrIP == pPacket->getData()+OFFSET_IPV4_PROTOCOL_SRCIP) {
				purgePacket(pPacket);
				return 1;
			}
						
			// Check destination exact match
			if(*pAddrIP == pPacket->getData()+OFFSET_IPV4_PROTOCOL_DSTIP) {
				purgePacket(pPacket);
				return 1;
			}
		}
	}
	
	
	return 0;
}

bool 	InputPurge::purgePacket (Packet * pPacket) {
	g_MemPool.releaseObject(pPacket);
	m_pStats->Add_Stat(STAT_IOPURGE_PKTCOUNT, 1);
	return true;
}

bool	InputPurge::extractExtendedDOM (NodeDOM * pNode) {
	int			j;
	string		sData;
	NetAddressIPv4		*	pAddrIP;
	NetAddressEthernet  *	pAddrEth;
	
	for(j=0; j<pNode->getNumChildren(); j++) {
		if(pNode->getChild(j)->getTag() == "Ethernet") {
			sData = pNode->getChild(j)->getData();			
			pAddrEth = NetAddressEthernet::createFromString(sData);
			if(pAddrEth != NULL) {
				m_CatchEthernet.push_back(pAddrEth);
			}
		}			
		if(pNode->getChild(j)->getTag() == "IPv4") {
			sData = pNode->getChild(j)->getData();			
			pAddrIP = NetAddressIPv4::createFromString(sData);
			if(pAddrIP != NULL) {
				m_CatchIPv4.push_back(pAddrIP);
			}
		}					
	}	
	
	return true;
}

NodeDOM * InputPurge::getStats (NodeDOM * pRoot) {
	if(m_pStats != NULL) {
		return m_pStats->getStats(pRoot, "Purge");
	} else {
		return NULL;
	}
}

StatsIOPurge::StatsIOPurge () {
	Allocate(STAT_IOPURGE_END);	
}

void StatsIOPurge::Get_Title	(int nStat, char * szTitle) {
	switch(nStat) {
		case 	STAT_IOPURGE_PKTCOUNT:
			sprintf(szTitle, "PktCount");
			break;
		default:
			sprintf(szTitle, "F%03d", nStat);
			break;
	}	
}
