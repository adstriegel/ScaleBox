/* PktScheduler.cc
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
 * $Revision: 1.2 $  $Date: 2008/02/25 22:09:39 $
 **********************************************************
 */
 
 #include "PktScheduler.h"
 
 
 PktScheduler::PktScheduler () {
 	
 }
 
 PktScheduler::~PktScheduler () {
 	
 }
 
 char PktScheduler::initialize () {
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

bool PktScheduler::startModule () {
	// startModule is invoked once the entirety of the configuration has been
	// initialized.  The typical role of the startModule operation is to get
	// the various threads actually going (as opposed to an early pause/wait
	// state) as well as linking this module with other modules
	
	// The core InputPassThrough code is a good example that uses startModule
	
	// Return true if everything looks good, otherwise a return code of false
	//  again brings ScaleBox to a halt on startup
	
	return true;	
}

char PktScheduler::processPacket (Packet * pPacket) {
	// We enter process packet every time whatever chain we are on
	// gets called. We can exist on any output or input chain
	// in the architecture.  
		
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

bool	PktScheduler::extractExtendedDOM (NodeDOM * pNode) {
	int			j;
	NodeDOM	*	pChild;
	
	// Feel free to add different tests for exact matches in the form of
	//  attributes listed under the module as
	//
	//     <sandboxID>MyEnemyIsSpiderman</sandboxID>
		
	// True means everything was kosher or at least tolerable in terms of the
	//  XML processing.  A false will bring the loading of ScaleBox to a grinding
	//  halt
	
	return true;
}

NodeDOM * PktScheduler::getStats () {
	return NULL;
}
