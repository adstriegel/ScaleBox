

#include "OutputWrite.h"
#include "Adapter.h"
#include "Packet.h"
#include "../mem/MemoryPool.h"

#include "../xml/NodeDOM.h"

#include "../util/ip-utils.h"

OutputWrite::OutputWrite () {
	m_bCalcQueueTime = false;
	m_pStats = new StatsOutputWrite;
	setType("OutputWrite");
}

OutputWrite::~OutputWrite () {
	delete m_pStats;
	m_pStats = NULL;
}

char	OutputWrite::processPacket (Packet * pPacket) {
	m_pStats->Add_Stat(STAT_OWRITE_PKTCOUNT, 1);
	m_pStats->Add_Stat(STAT_OWRITE_PKTSIZE, pPacket->getLength());
	
	if(m_bCalcQueueTime) {
		struct timeval		curTime;
		// What is the current system time?
		gettimeofday(&curTime, NULL);		
		
		m_pStats->Add_Stat(STAT_OWRITE_INTQUEUE, calcTimeDiff(&curTime, pPacket->getArrivalTime()) / 1000);
	}

	getAdapter()->writePacket(pPacket);
	
	// Callback for write making it fairly close to the actual write time
	//		Will not do anything if there was not one present
	pPacket->invokeWriteCallback();
	
	//	getAdapter()->writePacket(pPacket);
	// Give the packet back to the global memory pool
	g_MemPool.releaseObject(pPacket);
	return 1;	
}

char OutputWrite::initialize () {
	return 1;
}

	
bool OutputWrite::extractExtendedDOM (NodeDOM * pNode) {
	int			j;
	NodeDOM	*	pChild;
	
	for(j=0; j<pNode->getNumChildren(); j++) {
		if(pNode->getChild(j)->getTag() == "calcQueueTime") {
			if(pNode->getChild(j)->getData() == "yes" || pNode->getChild(j)->getData() == "Yes") {
				setFlag_CalcQueueTime(true);
			} else {
				setFlag_CalcQueueTime(false);
			}
		}	
	}
		
 	return true;
}

bool OutputWrite::getFlag_CalcQueueTime () {
	return m_bCalcQueueTime;	
}


void	OutputWrite::setFlag_CalcQueueTime (bool bFlag) {
	m_bCalcQueueTime = bFlag;
}

NodeDOM * OutputWrite::getStats (NodeDOM * pRoot) {
	return m_pStats->getStats(pRoot, "stats:OutputWrite");	
}



StatsOutputWrite::StatsOutputWrite () {
	Allocate(STAT_OWRITE_END);
}

	
void StatsOutputWrite::Get_Title (int nStat, char * szTitle) {
	switch(nStat) {
		case 	STAT_OWRITE_PKTCOUNT:
			sprintf(szTitle, "PktCount");
			break;
		case STAT_OWRITE_PKTSIZE:
			sprintf(szTitle, "PktSize");		
			break;
		case STAT_OWRITE_INTQUEUE:
			sprintf(szTitle, "IntQueueDelay");		
			break;
		case 	STAT_OWRITE_AVG_INTQUEUE:
			sprintf(szTitle, "AvgIntQueueDelay");		
			break;			
		default:
			sprintf(szTitle, "F%03d", nStat);
			break;
	}		
}

/** Re-compute the various computation sensitive fields
 */
void StatsOutputWrite::Calc_Data	() {
		
}



