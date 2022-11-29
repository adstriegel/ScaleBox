/* InputModule.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith
 * 
 **********************************************************
 * $Revision: 1.1 $  $Date: 2008/02/13 04:10:03 $
 **********************************************************
 */

#ifndef INPUTMODULE_H_
#define INPUTMODULE_H_

#include <string>
using namespace std;

#include <vector>
using namespace std;

class Adapter;
class Packet;
class Filter;
class NodeDOM;

/** The base class for input modules.  An input module is
 * defined as an interested party for the adapter input. The
 * input module will receive and parse the packet as read in
 * by the adapter.  The input module should then either drop
 * the packet (rare) or pass the packet to the next input
 * module in the chain. 
 */
class InputModule {
	public:
		InputModule ();
		~InputModule ();
		
		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
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

		/** Set the adapter associated with this input module
		 * @param pAdapter The new adapter for the input module
		 */
		void				setAdapter (Adapter * pAdapter);
		
		/** Change the filter established for this input module. The filter
		 * allows the input module to filter packets on a variety of levels
		 * (Ethernet, IPv4, UDP, TCP) for the purposes of exclusionary or
		 * inclusionary rules.
		 * 
		 * If there is already an existing filter, it will be freed back to
		 * memory via delete.
		 * 
		 * @param pFilter The new filter
		 */
		void				setFilter  (Filter * pFilter);
		
		/** See if the packet passes the filter for the input module
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

		/** Set the name of the input module. The module name is used for identification
		 * of the module within the context of the adapter or beyond but does not need
		 * to necessarily be unique.  
		 * @param sName The new name for the input module
		 */
		void			setName (string sName);
		
		/** Process a command intended for this module.  In order for this
		 * functionality to be enabled, the module itself must have a unique
		 * global name.
		 * @param theCmd A vector of strings pre-tokenized by the outlying caller
		 * @returns True if successful, false if an error occurred
		 */
		virtual bool			processCommand (vector<string> theCmd);
		
		/** Retrieve a copy of the statistics for this input module
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		virtual NodeDOM *		getStats ();
		
	protected:
	
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
		
		/** The filter (optional) associated with the given input module.
		 */
		Filter  *			m_pFilter;
		
		/** The name associated with this input module.  It should be unique
		 * if possible within the context of this specific adapter but otherwise
		 * will default to the type of module being used
		 */
		string				m_sName; 
};

/** Map a specified name, typically via XML/DOM to an
 * actual class name. In short, the function simply takes
 * care of the new operation mapping and the rest is
 * accomplished by the base and extended XML population
 * functions for the base and child classes.
 * @param sName The proper name (case sensitive) of the module to request
 * @returns A valid pointer to an IOModule (or child class) object
 *   if successful, false otherwise 
 */
IOModule 	*		createInputModuleFromName (string sName);



#endif /*INPUTMODULE_H_*/
