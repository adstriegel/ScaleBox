/* MultiIFModule.cc
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
 * $Revision: 1.10 $  $Date: 2008/04/11 22:52:47 $
 **********************************************************
 */

#include <iostream>
using namespace std;

#include "Adapter.h"
#include "Packet.h"

#include "../xml/NodeDOM.h"

#include "../mon/Monitor.h"			// Monitor related functions

#include "MultiIFModule.h"

PacketTap::PacketTap () : IOModule () {
	m_nID = -1;
	m_pTargetModule = NULL;
	m_sTargetName = "";
}

PacketTap::~PacketTap () {
	m_nID = -1;
	m_pTargetModule = NULL;	
	m_sTargetName = "";
}

char PacketTap::initialize () {
	return 1;	
}

char PacketTap::processPacket (Packet * pPacket) {
//	cout << "** Process packet" << endl;

	if(!passFilter(pPacket))
	{
		return PROCESSPKT_RESULT_CONTINUE;
	}

	if(m_pTargetModule != NULL) {
		return m_pTargetModule->processTapPacket(pPacket, m_nID);	
	}
	else
	{
		return PROCESSPKT_RESULT_CONTINUE;
	}
}

bool PacketTap::startModule () {
	// Iterate through the MultiIFModule list defined at the monitor
	IOModule		*	pModule;
	int				j;
	
	for(j=0; j<g_Monitor.getCountModules(); j++) {
		pModule = g_Monitor.getModule(j);
		
		if(pModule->getName() == m_sTargetName) {
			m_pTargetModule = (MultiIFModule *) pModule;
			
			// Attempt to look up our identity from the MultiIFModule
			//  We convey the ID whenever a packet is passed from this
			//  tap of an information flow to the overarching module
			m_nID = m_pTargetModule->mapNameToID(getTapID());			
			m_pTargetModule->registerTap(this);			
			
			return true;
		}
	}
	
	return false;	
}
				
bool PacketTap::extractExtendedDOM (NodeDOM * pNode) {
	bool		bOutputTarget;
	bool		bTapID;
	int			j;
	
	bOutputTarget = false;
	bTapID = false;
	
	for(j=0; j<pNode->getNumChildren(); j++) {
		if(pNode->getChild(j)->getTag() == "targetModule") {
			m_sTargetName = pNode->getChild(j)->getData();
			bOutputTarget = true;			
		}
		else if(pNode->getChild(j)->getTag() == "tapID") {
			setTapID(pNode->getChild(j)->getData());
			bTapID = true;						
		}					
	}
	
	if(!bOutputTarget) {
		cerr << "Error: Target for packet tap not defined. Packet tap must be targeted towards" << endl;
		cerr << "       a specific multi-interface module." << endl;
		return false;
	}
	
	if(!bTapID) {
		cerr << "Error: Identity for packet tap not set which prevents mapping" << endl;
		cerr << "       from the overarching module." << endl;
		return false;
	}	
	
	return true;	
}

		
int	 PacketTap::getID () {
	return m_nID;
}

void PacketTap::setID (int nID) {
	m_nID = nID;
}

string PacketTap::getTapID () {
	return m_sTapID;		
}		

void PacketTap::setTapID (string sTapID) {
	m_sTapID = sTapID;
}

//////////////////////////////////////////////////////////

bool PacketTap::writeOut (Packet * pPacket)
{
	Adapter * pAdapter;

	pAdapter = getAdapter();
	pAdapter->addOutputQueue(pPacket);
	return true;
}


//////////////////////////////////////////////////////////

MultiIFModule::MultiIFModule () : IOModule () {
	
}

//////////////////////////////////////////////////////////

MultiIFModule::~MultiIFModule () {
	
}

//////////////////////////////////////////////////////////

char MultiIFModule::processPacket (Packet * pPacket) {
	// Default behavior is not to touch it
	return 0;	
}

//////////////////////////////////////////////////////////

void MultiIFModule::registerTap (PacketTap * pTap) {
	if(pTap != NULL) {	
		m_Taps.push_back(pTap);
	}	
}

//////////////////////////////////////////////////////////

PacketTap *	MultiIFModule::findTapByID (int nID) {
	int		j;
	
	for(j=0; j<m_Taps.size(); j++) {
		if(m_Taps[j]->getID() == nID) {
			return m_Taps[j];
		}
	}
	
	return NULL;	
}

//////////////////////////////////////////////////////////

bool MultiIFModule::resumePostTap (int nTapID, Packet * pPacket) {
	PacketTap *	pTap;
	
	pTap = findTapByID(nTapID);
	
	if(pPacket->isTraceEnabled()) {
		cout << "** Resume post tap!" << endl;
	}
	
	if(pTap != NULL) {
		return pTap->resumePacketProcessing(pPacket);
	} else {
		return false;
	}		
}

//////////////////////////////////////////////////////////

bool MultiIFModule::writeBackViaTap (int nTapID, Packet * pPacket)
{
	PacketTap *	pTap;

	pTap = findTapByID(nTapID);

	if(pPacket->isTraceEnabled()) {
		cout << "** writeBackViaTap requested!" << endl;
	}

	if(pTap != NULL) {
		return pTap->writeOut(pPacket);
	} else {
		return false;
	}
}


//////////////////////////////////////////////////////////

bool	MultiIFModule::processWriteCallback (Packet * pPacket, void * pData) {
	return false;
}

//////////////////////////////////////////////////////////

bool	MultiIFModule::processWriteCallback (Packet * pPacket, void * pData, int nType) {
	return false;
}
		
