
#include <iostream>
using namespace std;

#include "InputPassthrough.h"
#include "Packet.h"

#include "../pkt/Adapter.h"
#include "../mon/Monitor.h"

#include "../xml/NodeDOM.h"

InputPassthrough::InputPassthrough () {
	m_pTarget = NULL;
	m_sTargetName = "";
	m_pStats = new StatsInputPassthrough();
	setType("InputPassthrough");	
}

InputPassthrough::~InputPassthrough () {
	delete m_pStats;
}

char InputPassthrough::initialize () {
	return 1;	
}

char InputPassthrough::processPacket (Packet * pPacket) {
	m_pStats->Add_Stat(STAT_PTHROUGH_PKTCOUNT, 1);
	
	if(m_pTarget == NULL) {
		// No target, allow further processing
		return 0;
	} else {
		// Add the packet without modification to the output queue of the
		//  target adapter
		
		if(pPacket->isTraceEnabled()) {
			cout << "*-> Passthrough to " << m_sTargetName << " ";
			printf(" on pkt 0x%p", pPacket);
			cout << " <-*" << endl;
		}
		
		m_pTarget->addOutputQueue(pPacket);
		return 1;		
	}
}


bool InputPassthrough::extractExtendedDOM (NodeDOM * pNode) {
	bool		bOutputTarget;
	int			j;
	
	bOutputTarget = false;
	
	for(j=0; j<pNode->getNumChildren(); j++) {
		if(pNode->getChild(j)->getTag() == "targetName") {
			m_sTargetName = pNode->getChild(j)->getData();
			bOutputTarget = true;
/*			if(bOutputTarget == true) {
				cerr << "Multipoint output not yet supported on passthrough, ignoring" << endl;
				cerr << "  this target selection..." << endl;
			}
			else if(lookupAdapter(pNode->getChild(j)->getData())) {
				bOutputTarget = true;
			} else {
				cerr << "Failed to find adapter target of " << pNode->getChild(j)->getData() << " in the" << endl;
				cerr << " global monitor adapter list, ignoring target setting" << endl;	
			}*/
			
		}			
	}
	
	if(!bOutputTarget) {
		cerr << "Error: Output target for passthrough adapter not defined" << endl;
		return false;
	}
	
	return true;
}

bool	InputPassthrough::startModule () {
	if(lookupAdapter(m_sTargetName)) {
		return true;
	} else {
		cerr << "Error: Failed to find adapter target of " << m_sTargetName << " in the" << endl;
		cerr << " global monitor adapter list, ignoring target setting" << endl;	
		cerr << " InputPassthrough startModule call failed" << endl; 
		return false;
	}	
}

bool	InputPassthrough::lookupAdapter (string sName) {
	Adapter		*	pAdapter;
	int				j;
	
	for(j=0; j<g_Monitor.getCountAdapters(); j++) {
		pAdapter = g_Monitor.getAdapter(j);
		
		if(pAdapter->getName() == sName) {
			m_pTarget = pAdapter;
			return true;
		}
	}
	
	return false;
}

NodeDOM * InputPassthrough::getStats (NodeDOM * pRoot) {			
	if(m_pStats != NULL) {
		return m_pStats->getStats(pRoot, "Passthrough");
	} else {
		return NULL;
	}	
}

StatsInputPassthrough::StatsInputPassthrough () {
	Allocate(STAT_PTHROUGH_END);	
}

void StatsInputPassthrough::Get_Title	(int nStat, char * szTitle) {
	switch(nStat) {
		case 	STAT_PTHROUGH_PKTCOUNT:
			sprintf(szTitle, "PktCount");
			break;
		default:
			sprintf(szTitle, "F%03d", nStat);
			break;
	}	
}


