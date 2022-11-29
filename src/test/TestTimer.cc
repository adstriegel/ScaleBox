/* TestTimer.cc
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
 * $Revision: 1.3 $  $Date: 2008/04/04 21:46:21 $
 **********************************************************
 */

#include <iostream>
using namespace std;

#include "TestTimer.h"
#include "../pkt/Packet.h"
#include "../pkt/Adapter.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

#include "../mon/Thread_Timer.h"

void testTimer () {
	TestTimerModule		timerModule;
	
	timerModule.initialize();	
	
	timerModule.startModule();
	
}

TestTimerModule::TestTimerModule () : IOModule () {
}


TestTimerModule::~TestTimerModule () {
	
}

char TestTimerModule::initialize () {
	// Do any initialization here that we need to do.  Initialization is done
	//  just after all of the XML fields are processed but not necessarily before
	//  the entirety of the XML file is processed
	//
	// Appropriate things to put here are initializing threads, mutexes, i.e.
	//  parts that do not necessarily need to talk to different IO modules
	
	// Return a 1 if everything looks good, otherwise a 0 brings ScaleBox during
	//  startup to a screeching halt
		
	return 1;	
}

bool TestTimerModule::startModule () {
	// startModule is invoked once the entirety of the configuration has been
	// initialized.  The typical role of the startModule operation is to get
	// the various threads actually going (as opposed to an early pause/wait
	// state) as well as linking this module with other modules
	
	// The core InputPassThrough code is a good example that uses startModule
	
	// Return true if everything looks good, otherwise a return code of false
	//  again brings ScaleBox to a halt on startup
	
	// Turn on timers
	enableTimers();
		
	return true;	
}

char TestTimerModule::processPacket (Packet * pPacket) {
	// We enter process packet every time whatever chain we are on
	// gets called. We can exist on any output or input chain
	// in the architecture.  
	
	cout << " In the timer test module, I saw a packet" << endl;	
	cout << "   It was " << pPacket->getLength() << " bytes long" << endl;
	
	// Do some packet magic
	//	 Extract some fields - see ../util/ip-utils.h for prototypes
	//  Parse whatever we want or record stats
	
	// The return value for the function is key
	//	  1 means that we ate the packet (nobody else should get it) and are
	//   eventually responsible for giving it back to the memory manager
	//     See InputPurge or call gobblePacket to do this
	//   0 means that we just looked at the packet, maybe manipulated it but
	//     it should keep going down the chain
	
	return 0;
}

bool	TestTimerModule::extractExtendedDOM (NodeDOM * pNode) {
	int			j;
	NodeDOM	*	pChild;
	
	// Feel free to add different tests for exact matches in the form of
	//  attributes listed under the module as
	//
	//     <sandboxID>MyEnemyIsSpiderman</sandboxID>
	
	for(j=0; j<pNode->getNumChildren(); j++) {
		
	}
	
	// True means everything was kosher or at least tolerable in terms of the
	//  XML processing.  A false will bring the loading of ScaleBox to a grinding
	//  halt
	
	return true;
}

NodeDOM * TestTimerModule::getStats () {
	return NULL;
}

CommandResult	TestTimerModule::processExtCommand (const vector<string> & theCommands, int nOffset) {
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}
		
	if(theCommands[nOffset] == "startCBR-1s") {
		TimerEvent	*	pEvent;
		
		pEvent = TimerEvent::createTimer();
		
		pEvent->setTimerID (120);
		pEvent->setInterval_ms(1000);
		pEvent->armTimer();
		
		addTimerEvent(pEvent);
		return PROCESS_CMD_SUCCESS;
	}


	return PROCESS_CMD_UNKNOWN;	
}

bool TestTimerModule::processTimer (TimerEvent * pEvent) {
	struct timeval 	currentTime;	
		
	switch(pEvent->getTimerID()) {
		case 120:
			gettimeofday(&currentTime, NULL);
		
			cout << "Timer invocation @ t=" << currentTime.tv_sec << "." << currentTime.tv_usec << endl;
			pEvent->armTimer();
			addTimerEvent(pEvent);			
			
			break;	
	}
	
	return true;	
}
	

	