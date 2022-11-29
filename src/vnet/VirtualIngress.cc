/* VirtualIngress.cc
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
 * $Revision: 1.3 $  $Date: 2008/02/27 22:03:05 $
 **********************************************************
 */

#include "VirtualIngress.h" 
 
////////////////////////////////////////////////////////////////////////

VirtualIngressModule::VirtualIngressModule () {
	
}

////////////////////////////////////////////////////////////////////////

VirtualIngressModule::~VirtualIngressModule () {
	
} 

int VirtualIngressModule::mapNameToID (string sName) {
	return -1;
}

////////////////////////////////////////////////////////////////////////

char VirtualIngressModule::processTapPacket (Packet * pPacket, int nID) {
	return 0;
}

////////////////////////////////////////////////////////////////////////

bool VirtualIngressModule::extractExtendedDOM (NodeDOM * pNode) {
	
	return true;	
}

