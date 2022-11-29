

#include <iostream>
using namespace std;

#include "VGDM.h"
#include "InputIngressVGDM.h"
#include "InputEgressVGDM.h"


VGDM::VGDM () {
	m_pIngress = NULL;
	m_pEgress = NULL;
}

VGDM::~VGDM () {
	
}

void VGDM::setIngress(InputIngressVGDM * pIngress) {
	m_pIngress = pIngress;
}

void VGDM::setEgress (InputEgressVGDM * pEgress) {
	m_pEgress = pEgress;
}

char VGDM::processIngressPacket (Packet * pPacket) {
	// Basic VGDM flow
	//		1. Check versus filter
	//				Filter non-IP, non-UDP or non-TCP
	//				Filter any excluded hosts
	//		2. Check to see if the packet is a control message
	//				Server side may support VGDM properties
	//		3. Determine if the packet should be queued or
	//  		 if the packet should be noted for background
	//			 analysis only

	// Should this packet be filtered and ignored?
	if(ingressFilter(pPacket)) {
		return 0;
	}
	
	// See if this is a control message
	if(ingressProcessControl(pPacket)) {
		return 1;
	}

	// Final step: See if the packet should be queued or
	// 				immediately dispatched onwards with a
	//					local copy possibly kept for determining
	//					if we could have offered a benefit with
	//					stealth multicast
	if(ingressCheckQueue(pPacket)) {
		return 1;
	}
	
	return 0;	
}

char VGDM::processEgressPacket (Packet * pPacket) {
	return 0;
}


char VGDM::ingressFilter (Packet * pPacket) {
	return 0;	
}

char VGDM::ingressProcessControl (Packet * pPacket) {
	return 0;	
}

char VGDM::ingressCheckQueue		 (Packet * pPacket) {
	// Does the application exist in our list of existing applications
		


	// Compute the checksum for the packet
	
	
	
	
	// Can this packet join an existing virtual group?
	
	return 0;
}


