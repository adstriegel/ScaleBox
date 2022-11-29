/* AdapterVirtual.cc
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith
 * 
 **********************************************************
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#include "AdapterVirtual.h"
#include "../pkt/Packet.h"

#include <iostream>
using namespace std;

#include "../mem/MemoryPool.h"
#include "../pkt/Thread_IO.h"
#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

#include "../util/mgmt.h"

AdapterVirtual::AdapterVirtual () : Adapter () {
	
}

AdapterVirtual::~AdapterVirtual () {
	
}

void AdapterVirtual::readPacket () {
	yieldCPU();	
}

void	AdapterVirtual::startDevice () {
	startThreads();	
}

void	AdapterVirtual::dumpBasicInfo  () {
	
}

void AdapterVirtual::writePacket (Packet * pPacket) {
	// Do nothing, should be gobbled elsewhere
}

bool AdapterVirtual::extractExtendedDOM (NodeDOM * pNode) {	
	return true;	
}
		
		