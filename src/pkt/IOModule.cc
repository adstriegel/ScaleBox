/* IOModule.h
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

#include <iostream>
using namespace std;

#include "IOModule.h"
#include "Filter.h"

#include "Adapter.h"

#include "../mem/MemoryPool.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

#include "Packet.h"

#include "../mon/Thread_Timer.h"

#include "PktSupport.h"
#include "../pktcache/PktCacheSupport.h"
#include "../sandbox/SandboxSupport.h"
#include "../vnet/VNetSupport.h"
#include "../test/TestSupport.h"
#include "../RIPPS/RIPPS_Support.h"
#include "../packetcache/PacketCacheSupport.h"

#include "../packetcache/PacketCacheSupport.h"

#include "../TWiCE/TWiCE_Support.h"
#include "../fmnc/fmnc_support.h"
#include "../whirlwind/whirlwind_support.h"

IOModule::IOModule () {
	m_pAdapter = NULL;
	m_pFilter = NULL;
	m_pTimerAggregator = NULL;
	m_nChainDirection = IOMODULE_CHAIN_UNKNOWN;
}

IOModule::~IOModule () {
	
}

////////////////////////////////////////////////////////////////////

bool IOModule::processTimer (TimerEvent * pEvent) {
	// I don't do no stinkin timers as the base class but no need to make
	// this purely virtual if derived classes aren't going to use timers
	// in the first place
	return false;
}

////////////////////////////////////////////////////////////////////

void IOModule::enableTimers () {
	// See if we already have an aggregator allocated, we should not
	if (m_pTimerAggregator == NULL) {
		// Create a new one
		m_pTimerAggregator = new TimerAggregator ();
		
		m_pTimerAggregator->setIOModule(this);
		
		// Register the aggregator with the central time service
		g_TimerManager.registerTimerAggregator(m_pTimerAggregator);		
	} else {
		cerr << "Timers are already enabled for this IOModule, ignoring call" << endl;
		cerr << "  to enableTimers()" << endl;
	}	
}

////////////////////////////////////////////////////////////////////

bool IOModule::addTimerEvent (TimerEvent * pTimer) {
	if(m_pTimerAggregator == NULL) {
		cerr << "Warning: Unable to add timer as timers have not been enabled for this module (" << getName() << endl;
		return false;
	} else {
		return m_pTimerAggregator->addTimerEvent(pTimer);
	}	
}

////////////////////////////////////////////////////////////////////

void IOModule::setAdapter (Adapter * pAdapter) {
	m_pAdapter = pAdapter;	
}

Adapter * IOModule::getAdapter () {
	return m_pAdapter;
}

void	IOModule::setFilter  (Filter * pFilter) {
	if(m_pFilter != NULL) {
		delete m_pFilter;
		m_pFilter = NULL;
	}
	
	m_pFilter = pFilter;	
}

bool IOModule::passFilter (Packet * pPacket) {
	//cout << "  passFilter (InputModule)" << endl;
	//printf("   IM @ 0x%X  ---   Filter = 0x%X \n", this, m_pFilter);
	
	if(m_pFilter == NULL) {
		return true;
	} else {
		return m_pFilter->passFilter(pPacket);	
	}
}

////////////////////////////////////////////////////////

bool IOModule::extractBaseDOM  (NodeDOM * pNode) {
	int		j;
		
	for(j=0; j<pNode->getNumChildren(); j++) {
		cout << " Tag is " << pNode->getChild(j)->getTag() << endl;
		
		if(pNode->getChild(j)->getTag() == "name") {
			setName(pNode->getChild(j)->getData());
		}		
		else if(pNode->getChild(j)->getTag() == "Filter")
		{
			if(m_pFilter == NULL)
			{
				m_pFilter = new Filter();
			}

			if(m_pFilter != NULL)
			{
				m_pFilter->populateFromDOM(pNode->getChild(j));
			}
			else
			{
				cerr << "* Error: Filter allocation unsuccessful" << endl;
			}
		}
	}

	return true;	
}

bool IOModule::startModule () {
	// Nothing to do in the base case
	return true;
}

////////////////////////////////////////////////////////

string IOModule::getName () {
	return m_sName;
}

////////////////////////////////////////////////////////

void IOModule::setName (string sName) {
	m_sName = sName;
}

////////////////////////////////////////////////////////

NodeDOM * 	IOModule::getStats (NodeDOM * pRoot) {
	return NULL;
}

////////////////////////////////////////////////////////

string IOModule::getType () {
	return m_sModuleType;	
}

////////////////////////////////////////////////////////

void IOModule::setType (string sType) {
	m_sModuleType = sType;
}

////////////////////////////////////////////////////////

void IOModule::gobblePacket (Packet * pPacket) {
	g_MemPool.releaseObject(pPacket);	
}

////////////////////////////////////////////////////////

void IOModule::setChainDirection (char nDir) {
	m_nChainDirection = nDir;
}

////////////////////////////////////////////////////////

char IOModule::getChainDirection () {
	return m_nChainDirection;
}
	
////////////////////////////////////////////////////////

bool IOModule::resumePacketProcessing (Packet * pPacket) {
	if(getAdapter() == NULL) {
		cerr << "No adapter specified for IOModule, unable to resume packet processing" << endl;
		return false;
	}
	
	if(getChainDirection() == IOMODULE_CHAIN_UNKNOWN) {
		cerr << "No direction specified for IOModule, unable to resume packet processing" << endl;
		return false;		
	}
	
	pPacket->setResumeAfterModule(this);
	
	if(getChainDirection() == IOMODULE_CHAIN_INCOMING) {
		return getAdapter()->addInputQueue(pPacket);	
	} else if (getChainDirection() == IOMODULE_CHAIN_OUTGOING) {
		return getAdapter()->addOutputQueue(pPacket);
	} else {
		cerr << "Unknown chain direction for IOModule, unable to resume packet processing" << endl;
		return false;		
	}	
		
	
	return true;
}

/////////////////////////////////////////////////////////////////////////

bool IOModule::matchName (string sSearch) {
	if(sSearch == "*") {
		return true;
	}
	
	if(sSearch == m_sName) {
		return true;
	} else {
		return false;
	}	
}

/////////////////////////////////////////////////////////////////////////

CommandResult IOModule::processCommand (const vector<string> & theCommands, int nOffset) {	
	// Check if the size is sufficient
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}
	
	// See if the name field is for us	
	if(!matchName(theCommands[nOffset])) {
		return PROCESS_CMD_DIFFTARGET;
	}
	
	// OK, we have at least the name we think	
	return processExtCommand(theCommands, nOffset+1);
}

/////////////////////////////////////////////////////////////////////////

CommandResult IOModule::processExtCommand (const vector<string> & theCommands, int nOffset) {
	return PROCESS_CMD_UNKNOWN;	
}

/////////////////////////////////////////////////////////////////////////

bool	IOModule::processWriteCallback (Packet * pPacket, void * pData) {
	return false;
}

/////////////////////////////////////////////////////////////////////////

bool	IOModule::processWriteCallback (Packet * pPacket, void * pData, int nType) {
	return false;
}

/////////////////////////////////////////////////////////////////////////

IOModule * 		mapModuleFromName(string sName) {
    cout << "mapModuleFromName: " << sName << endl;

	IOModule	*	pModule;
	
	pModule = NULL;
	
	// ------  Core Packet Functionality for ScaleBox --------
	
	pModule = mapModuleFromName_Pkt (sName);
	
	if(pModule != NULL) {
		return pModule;
	}
	
	// ------  Sandbox functionality -------
	
	pModule = mapModuleFromName_Sandbox (sName);
	
	if(pModule != NULL) {
		return pModule;
	}	

	// ------  VNet - Virtual Networking support --------

	pModule = mapModuleFromName_VNet (sName);
	
	if(pModule != NULL) {
		return pModule;
	}	
	
	// ------  PktCache - Packet Caching Module --------

#ifdef PKTCACHE_MODULE_ENABLED

	pModule = mapModuleFromName_PktCache (sName);
	
	if(pModule != NULL) {
		return pModule;
	}
	
#endif

#ifdef PACKETCACHE_MODULE_ENABLED

	pModule = mapModuleFromName_PacketCache (sName);
	
	if(pModule != NULL) {
		return pModule;
	}
	
#endif

#ifdef TWICE_MODULE_ENABLED
	pModule = mapModuleFromName_TWiCE (sName);

	if(pModule != NULL) {
		return pModule;
	}

#endif

#ifdef FMNC_MODULE_ENABLED
	pModule = mapModuleFromName_FMNC (sName);

	if(pModule != NULL) {
		return pModule;
	}

#endif

	// ------  PktCache - Packet Caching Module --------

#ifdef TEST_MODULE_ENABLED

	pModule = mapModuleFromName_Test (sName);
	
	if(pModule != NULL) {
		return pModule;
	}
	
#endif	

#ifdef RIPPS_MODULE_ENABLED	
	
	pModule = mapModuleFromName_RIPPS (sName);
	
	if(pModule != NULL) {
		return pModule;
	}
#endif

/* #ifdef WHIRLWIND_MODULE_ENABLED */

/*     pModule = mapModuleFromName_Whirlwind (sName); */

/*     if(pModule != NULL) { */
/*         return pModule; */
/*     } */
/* #endif */

	
	// Insert your modular generation calls here	
	
	return NULL;
}


