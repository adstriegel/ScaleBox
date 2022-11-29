#ifndef OUTPUTWRITE_H_
#define OUTPUTWRITE_H_

#include "IOModule.h"
#include "../stat/Stats.h"

class Packet;
class StatsOutputWrite;
class NodeDOM;

/** The last placeholder in an output chain that will actually write
 * the packet out to the adapter itself
 */
class OutputWrite : public IOModule {
	public:
		OutputWrite ();
		~OutputWrite ();
	
		/** Process a packet coming along the output chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the output chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		char		processPacket (Packet * pPacket);	
	
		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();	
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);	
		 
		 /** Retrieve the flag regarding if the total queued time should
		  * be recorded for performance assessment
		  * @returns 1 (True) if it should be recorded, 0 (false) otherwise
		  */
		 bool		getFlag_CalcQueueTime ();
		 
		 /** Set the flag regarding if the total queued time should
		  * be recorded for performance assessment
		  * @param bFlag Flag denoting if queue time should be saved
		  */
		 void		setFlag_CalcQueueTime (bool bFlag);
		  
		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		virtual NodeDOM *		getStats (NodeDOM * pRoot);


	
	private:
		/** Flag regarding if the queue time should be recorded
		 * at the monitor */
		bool		m_bCalcQueueTime;
	
		StatsOutputWrite	*		m_pStats;
};



/** Enumerated values for the InputPassthrough module statistics
 */
enum eStatsOutputWrite {
	STAT_OWRITE_PKTCOUNT,
	STAT_OWRITE_PKTSIZE,
	STAT_OWRITE_INTQUEUE,				// Summative queue time in ms
	STAT_OWRITE_AVG_INTQUEUE,			// Avg queue time in ms
	STAT_OWRITE_END	
};


/** Statistics for the OutputWrite module 
 */
class StatsOutputWrite : public Stats
{
	public:
		StatsOutputWrite ();
	
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);		
		
		/** Re-compute the various computation sensitive fields
		 */
		virtual void		Calc_Data	();
		
		
	private:
}; 

#endif /*OUTPUTWRITE_H_*/
