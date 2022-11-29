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
 * $Revision: 1.11 $  $Date: 2008/04/08 19:59:37 $
 **********************************************************
 */

#ifndef IOMODULE_H_
#define IOMODULE_H_

#include <string>
using namespace std;

#include <vector>
using namespace std;

#include <deque>
using namespace std;

#include "../core/Console.h"			// For command result definition

class Adapter;
class Packet;
class Filter;
class NodeDOM;
class TimerEvent;
class TimerAggregator;

#define IOMODULE_CHAIN_UNKNOWN		0
#define IOMODULE_CHAIN_INCOMING		1
#define IOMODULE_CHAIN_OUTGOING		2

// What is the processing result?  Should it be passed to the next module or did this particular
//  module consume (queue, discard) the packet?
#define	PROCESSPKT_RESULT_CONTINUE	0
#define PROCESSPKT_RESULT_CONSUMED	1


/** The base module definition for input and output
 */
class IOModule {
	public:
		IOModule ();
		virtual ~IOModule ();
						
		/** Process a packet coming along the chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the chain is started upon reception or transmission of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		virtual char		processPacket (Packet * pPacket)=0;

		/** Initialize an input module by registering the adapter it is 
		 * associated with and applying any necessary startup functionality such
		 * as threads, thread safety, or other initializations. 
		 * @returns True if successful, false otherwise
		 */
		virtual char		initialize () =0;

		/** Set the adapter associated with this module.
		 * Note that each module need not have an associated adapter.
		 * @param pAdapter The new adapter for the module
		 */
		void				setAdapter (Adapter * pAdapter);
		
		/** Process a timer event from the associated TimerAggregator
		 * object contained within this class if timers are enabled
		 * for this particular module
		 * @param pEvent The timer event that has occurred
		 * @returns True if successfully processed, false if there was an error
		 */
		virtual bool		processTimer (TimerEvent * pEvent);

		/** Enable timers for this particular IOModule. By default,
		 * timers are not enabled to reduce the memory footprint of the
		 * overall system. Enabling timers will allocate the appropriater
		 * aggregation object and then register said aggregation object with
		 * the master Timer manager.  
		 */
		void	enableTimers ();
		
		/** Add a timer for management by the aggregation object with the
		 * assumption that the timer is already populated and armed with the
		 * appropriate data.  
		 * @param pTimer The timer to add to the list
		 * @returns True if successful, false otherwise
		 */
		bool	addTimerEvent (TimerEvent * pTimer);
		
		/** Process a command from the console or external control
		 * mechanism.  If the command is not understood, it will be passed to the
		 * processExtCommand virtual function that can be overridden by the
		 * derived classes. The command functionality is referenced by specifying
		 * the exact module name or module chain of an adapter followed by a
		 * set of arguments separated by spaces.  
		 * 
		 * Show the stats for the pcap-eth1 adapter on the input chain:
		 * 
		 *   pcap-eth1 ic show stats
		 * 
		 * @param pCommands The vector of commands to process
		 * @param nOffset The offset at which to begin processing. The offset allows
		 *   us to pass the vector itself with modifications to the underlying
		 *   modules
		 * @returns Enumerated value representing result of command processing
		 */ 
		CommandResult	processCommand (const vector<string> & theCommands, int nOffset);
		
		/** Process a command from the console or external control
		 * mechanism
		 * @param theCommands The vector of commands to process
		 * @param nOffset The offset at which to begin processing. The offset allows
		 *   us to pass the vector itself with modifications to the underlying
		 *   modules
		 * @returns Enumerated value representing result of command processing
		 */ 
		virtual CommandResult	processExtCommand (const vector<string> & theCommands, int nOffset);		
						
		/** Change the filter established for this module. The filter
		 * allows the module to filter packets on a variety of levels
		 * (Ethernet, IPv4, UDP, TCP) for the purposes of exclusionary or
		 * inclusionary rules.
		 * 
		 * If there is already an existing filter, it will be freed back to
		 * memory via delete.
		 * 
		 * @param pFilter The new filter
		 */
		void				setFilter  (Filter * pFilter);
		
		/** See if the packet passes the filter for the module
		 * @param pPacket The packet to check against the filter (if it exists)
		 * @returns True if the packet should be considered by the module, false otherwise
		 */
		bool				passFilter (Packet * pPacket);

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractBaseXML function takes in the most
		 * basic input that is central to all adapters.  The virtualized
		 * function of extractExtendedDOM is for child-specific 
		 * information. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */
		bool		extractBaseDOM  (NodeDOM * pNode);
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		virtual bool		extractExtendedDOM (NodeDOM * pNode)=0;		
		
		/** Start up the module in terms of linking with any other modules.  The
		 * startModule function can assume that all adapters have been declared
		 * at this point.  
		 * @returns True if successful, false if there was a critical error.
		 */
		virtual bool					startModule ();
		
		/** Retrieve the name of the module. The module name is used for identification
		 * of the module within the context of the adapter but does not need to be stated
		 * nor necessarily unique within the global context or even the adapter itself.
		 * @returns A copy of the string containing the name of the module
		 */
		string		getName ();

		/** Set the name of the module. The module name is used for identification
		 * of the module within the context of the adapter or beyond but does not need
		 * to necessarily be unique.  
		 * @param sName The new name for the module
		 */
		void			setName (string sName);
				
		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		virtual NodeDOM *		getStats (NodeDOM * pRoot);
		
		/** Retrieve the module type as a string
		 * @returns A valid string object usually containing the class name to capture
		 *  the type of the child object
		 */
		string			getType ();

		/** Set the module type as a string
		 * @param sType A valid type string typically representing the derived class
		 */		
		void				setType (string sType);
		
		/** Gobble the packet and give it back to the memory manager
		 */
		void			gobblePacket (Packet * pPacket);
		
		/** Set the chain direction associated with this module with
		 * respect to its attached adapter to allow resumption of processing
		 * following delays or packets generated outside the adapter. Note that
		 * this value is not required to be input or output and may be unknown.
		 * @param nDir The direction with respect to the chain (input or output)
		 */
		void	setChainDirection (char nDir);

		/** Get the chain direction associated with this module with
		 * respect to its attached adapter to allow resumption of processing
		 * following delays or packets generated outside the adapter. Note that
		 * this value is not required to be input or output and may be unknown.
		 * @returns The direction with respect to the chain (input or output)
		 */		
		char	getChainDirection ();
		
		/** Resume processing of the packet (if possible) by the underlying adapter
		 * if possible. A return value of false denotes that the packet handoff failed
		 * and that the packet is still the responsibility of the callee. 
		 * @param pPacket Packet to resume processing
		 * @returns True if successful, false otherwise
		 */
		bool		resumePacketProcessing (Packet * pPacket);		
		
		/** Process the fact that a write callback has been invoked due to a
		 * packet finally being written out due to interposed queuing delay
		 * or other mechanisms
		 * @param pPacket The packet involved in the callback
		 * @param pData The data associated with the callback
		 * @returns True if successful, false otherwise 
		 */
		virtual bool	processWriteCallback (Packet * pPacket, void * pData);

		/** Process the fact that a write callback has been invoked due to a
		 * packet finally being written out due to interposed queuing delay
		 * or other mechanisms
		 * @param pPacket The packet involved in the callback
		 * @param pData The data associated with the callback
		 * @param nType The type for the callback (if specified earlier)
		 * @returns True if successful, false otherwise
		 */
		virtual bool	processWriteCallback (Packet * pPacket, void * pData, int nType);


		
	protected:
	
		/** Does the name of this module match the name requested in the search?
		 * @param sSearch The search string
		 * @returns True if an exact match, false otherwise
		 */
		bool				matchName (string sSearch);
	
		/** Retrieve the adapter associated with the input module
		 * @returns A pointer to the current Adapter associated with this input module.
		 * A value of NULL implies that no Adapter is associated with the input module
		 * and has likely not yet been initialized or added to an Adapter input chain.
		 */
		Adapter *		getAdapter ();
		
		/** Retrieve the filter associated with the input module
		 * @returns A pointer to the current Filter object. If NULL, a filter has
		 *  not (or may never be) set up for this input module.
		 */
		Filter  *		getFilter ();
				
	private:
		/** The adapter that this InputModule is associated with 
		 */
		Adapter * 			m_pAdapter;	

		/** Denote the status of the module with respect to whether it is on the
		 * read chain or the write chain of the adapter. This value is essential
		 * to allow a module with delay to resume processing after itself on
		 * the correct chain.
		 */
		char				m_nChainDirection;
		
		/** The filter (optional) associated with the given input module.
		 */
		Filter  *			m_pFilter;
		
		/** The name associated with this input module.  It should be unique
		 * if possible within the context of this specific adapter but otherwise
		 * will default to the type of module being used
		 */
		string				m_sName; 
		
		/** The type identified as a string for the module itself.  This should
		 * be the base type and only configurable in the constructor of the 
		 * child object
		 */
		string				m_sModuleType;		
		
		/** A pointer to the TimerAggregator object which aggregates all timer
		 * events for this particular module for presentation to the centralized
		 * TimerManager. By default, this object is NULL and must be enabled by
		 * calling the enableTimers function.
		 */
		TimerAggregator *	m_pTimerAggregator;
		

			
};

/** Attempt to resolve an I/O module name from the specified string. The
 * resulting module will then be returned for future processing. The name
 * field refers to the unique class-level type of name (InputLog, InputPurge),
 * not the actual name that might be used to idenify one particular instance
 * of a given type (IL0, IL1, IL-WANSide).  The naming convention for modules
 * within ScaleBox (stmc, pktcache, RIPPS) should append an underscore and
 * the module name to this base function.  
 * @param	sName		The name of the module (class-wise name)
 * @returns A pointer to a valid IOModule object if successfully resolved,
 *          NULL otherwise 
 */
IOModule * 		mapModuleFromName(string sName);

#endif /*IOMODULE_H_*/


