#ifndef INPUTPASSTHROUGH_H_
#define INPUTPASSTHROUGH_H_

#include <string>
using namespace std;

#include "IOModule.h"
#include "Adapter.h"
#include "../stat/Stats.h"

class StatsInputPassthrough;

/** A simple instantation of the input module that
 * simply passes the packet directly to a different
 * adapter for the purposes of passthrough
 */
class InputPassthrough : public IOModule {
	public:
		InputPassthrough ();
		~InputPassthrough ();
		
		/** Attempt to access the global monitor to find the adapter
		 * with the given name.  Note that the name is the name field
		 * of the adapter which is descriptive and may be different than
		 * any driver specific naming.  
		 * @param sName	The name of the adapter to look up
		 * @returns True if successful, false if failed
		 */
		bool	lookupAdapter (string sName);
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);

		/** Start up the passthrough module.  It is critical for correct
		 * operation that the target adapter has been declared and instantiated.
		 * @returns True if successful, false if there was a critical error. 
		 */
		virtual bool					startModule ();		
		
		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * Note: The passthrough module will always gobble packets for writing
		 *  		out to its respective target adapter.  Do not place modules
		 * 		after the passthrough module. 
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 		
		virtual char		processPacket (Packet * pPacket);		
		
		/** Initialize an input module by registering the adapter it is 
		 * associated with and applying any necessary startup functionality such
		 * as threads, thread safety, or other initializations. Note that this is
		 * distinct from startModule which is executed only after all modules and
		 * adapters have been created and initialized.  
		 * @returns True if successful, false otherwise
		 */
		virtual char		initialize ();		
		
		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);			
		
	private:
		// The adapter that will get the packets recorded
		//  by this input module
		Adapter	*		m_pTarget;
		
		string			m_sTargetName;
		
		StatsInputPassthrough	*		m_pStats;
};

/** Enumerated values for the InputPassthrough module statistics
 */
enum eStatsPassThrough {
	STAT_PTHROUGH_PKTCOUNT,
	STAT_PTHROUGH_END	
};


/** Statistics for the InputPassthrough module 
 */
class StatsInputPassthrough : public Stats
{
	public:
		StatsInputPassthrough ();
	
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);		
		
	private:
}; 


#endif /*INPUTPASSTHROUGH_H_*/
