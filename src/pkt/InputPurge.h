/* InputPurge.h
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
 * $Revision: 1.4 $  $Date: 2008/04/08 14:04:50 $
 **********************************************************
 */

#ifndef INPUTPURGE_H_
#define INPUTPURGE_H_

#include "IOModule.h"

#include "../stat/Stats.h"

// Temporary include files
#include "address/NetAddressEthernet.h"
#include "address/NetAddressIPv4.h"

#include <vector>
using namespace std;

class Packet;
class StatsIOPurge;

/** An input module established solely to remove packets
 * from further consideration to streamline filter rules.
 * Examples might include listening hosts that do not
 * need to consider ARPs or other non-IP information.
 */
class InputPurge : public IOModule {
	public:
		InputPurge ();
		~InputPurge ();

		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();	

		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		char		processPacket (Packet * pPacket);
		
		unsigned int		getPurgeCount   ();

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);
		
		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);		
		
	protected:
		
		/** Do the actual purge
		 * @param pPacket The packet to release back to the general pool
		 * @returns True if successful, false otherwise
		 */
		bool		purgePacket (Packet * pPacket);
		
	private:

		StatsIOPurge	*		m_pStats;	
		
		// Temporary patch until filters are fully functional
		//
		// As we would like to have a nice, robust rule mechanism for filters,
		// this is a temporary fix to allow for ignoring the stat server
		// and background ssh channels for the code where in-band
		// control is necessary due to out-of-band control not being
		// an option. 
		// 
		// While the shortcuts for tagging will still stay compatible when
		// filters are fully enabled, this mechanism will likely disappear depending
		// upon the necessary performance requirements.  
		vector<NetAddressEthernet *>	m_CatchEthernet;
		vector<NetAddressIPv4 *>		m_CatchIPv4;		
};

/** Enumerated values for the InputPassthrough module statistics
 */
enum eStatsPurge {
	STAT_IOPURGE_PKTCOUNT,
	STAT_IOPURGE_END	
};


/** Statistics for the InputPassthrough module 
 */
class StatsIOPurge : public Stats
{
	public:
		StatsIOPurge ();
	
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);		
		
	private:
}; 


#endif /*INPUTPURGE_H_*/
