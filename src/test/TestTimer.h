

#ifndef _TESTTIMER_H_
#define _TESTTIMER_H_

#include "../pkt/IOModule.h"
#include <string>
using namespace std;

/** Log all or a filtered subset of packets to disk in the
 * specified file
 */
class TestTimerModule : public IOModule {
	public:
		TestTimerModule ();
		
		~TestTimerModule ();

		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();	

		/** Start up the module in terms of linking with any other modules.  The
		 * startModule function can assume that all adapters have been declared
		 * at this point.  
		 * @returns True if successful, false if there was a critical error.
		 */
		virtual bool					startModule ();	

		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		char		processPacket (Packet * pPacket);
	
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);
	
		/** Retrieve a copy of the statistics for this input module
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		virtual NodeDOM *		getStats ();	
	
		/** Process a command from the console or external control
		 * mechanism
		 * @param theCommands The vector of commands to process
		 * @param nOffset The offset at which to begin processing. The offset allows
		 *   us to pass the vector itself with modifications to the underlying
		 *   modules
		 * @returns Enumerated value representing result of command processing
		 */ 
		virtual CommandResult	processExtCommand (const vector<string> & theCommands, int nOffset);		
							
		/** Process a timer event from the associated TimerAggregator
		 * object contained within this class if timers are enabled
		 * for this particular module
		 * @param pEvent The timer event that has occurred
		 * @returns True if successfully processed, false if there was an error
		 */
		virtual bool		processTimer (TimerEvent * pEvent);

	private:

};

#endif

