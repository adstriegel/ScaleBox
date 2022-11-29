/* RIPPS_Support.cc
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
 * $Revision: 1.4 $  $Date: 2008/04/08 19:50:26 $
 **********************************************************
 */
 
#include "../pkt/IOModule.h"
#include "RIPPS_Support.h"
#include "RIPPS_Monitor.h" 
#include "RIPPS_PktPair.h"

#include <iostream>
using namespace std;
 
IOModule * 		mapModuleFromName_RIPPS (string sName) {
	IOModule * pModule;
	
	pModule = NULL;

#ifdef RIPPS_MODULE_ENABLED		
	if(sName == "RIPPS_Monitor") {
		pModule = new RIPPS_Monitor();
	}	
#endif
		
	return pModule; 		
}

MemPoolObject *	mapObjectFromName_RIPPS (string sName) {
	MemPoolObject * pPoolObj;

	pPoolObj = NULL;

#ifdef RIPPS_MODULE_ENABLED	
	if(sName == "RIPPS_MonitorPkt") {
		pPoolObj = new RIPPS_MonitorPacket();
	}	
#endif

	return pPoolObj;
}

