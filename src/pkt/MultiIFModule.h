/* MultiIFModule.h
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
 * $Revision: 1.5 $  $Date: 2008/04/08 19:56:59 $
 **********************************************************
 */

#ifndef MULTIIFMODULE_H_
#define MULTIIFMODULE_H_

#include <vector>
using namespace std;

#include "IOModule.h"

class Packet;
class MultiIFModule;

/** An I/O module that taps a given adapter stream (input or output)
 * and then relays the packet to the target module while noting the
 * interface and adapter that the tap itself came from.  
 */
class PacketTap : public IOModule {
	public:
		PacketTap ();
		~PacketTap ();

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
				
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		bool		extractExtendedDOM (NodeDOM * pNode);
		
		/** Retrieve the unique ID (within the context of the overarching module)
		 * to identify this specific tap
		 * @returns A valid integer value unique for a tap within a given module
		 */
		int		getID ();

		/** Set the unique ID for this tap within the context of a module
		 * @param nID The new ID for the tap conveyed when packets are
		 *  			  passed up to the overarching module
		 */		
		void		setID (int nID);		
			
		/** Start up the module in terms of linking with any other modules.  The
		 * startModule function can assume that all adapters have been declared
		 * at this point.  
		 * @returns True if successful, false if there was a critical error.
		 */
		virtual bool					startModule ();			
		
		/** Retrieve the identity of the tap used for identifying the location
		 * and purpose of this specific tap with regards to the overarching module.
		 * The tap ID is kept separate from the name of the module itself to allow
		 * for the name to be meaningful in a larger context, the tap ID relevant
		 * only to the overarching module
		 * @returns A copy of the string denoting the tap ID 
		 */
		string		getTapID ();

		/** Set the identity of the tap used for identifying the location
		 * and purpose of this specific tap with regards to the overarching module.
		 * The tap ID is kept separate from the name of the module itself to allow
		 * for the name to be meaningful in a larger context, the tap ID relevant
		 * only to the overarching module
		 * @returns A copy of the string denoting the tap ID 
		 */			
		void		setTapID (string sTapID);	
				
		/** Write a packet to the outbound queue of the adapter that this particular
		 * PacketTap is associated with. Note that this simply starts the packet on the
		 * outbound chain of modules.  The actual write may be a bit later and the
		 * caller may want to register a callback on via the packet itself.
		 * @param   pPacket The packet to write out
		 * @returns True if successful (most likely), false otherwise
		 */
		bool		writeOut (Packet * pPacket);

	private:	
		int					m_nID;
		string				m_sTapID;			// String of the tap ID
		string				m_sTargetName;
		MultiIFModule	*	m_pTargetModule;
};


/** A module that takes in packets from multiple vantage points
 * to avoid declaring individual input and output modules versus
 * a single unified entity that serves the same purpose. In some
 * sense, the MultiIFModule (Multiple Interface Module)
 * can be thought of as resembling an application whose input 
 * is a tap into the underlying directional packets.
 */ 
class MultiIFModule : public IOModule {
	public:
		MultiIFModule ();
		~MultiIFModule ();
	
		/** Process a packet coming along the chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the chain is started upon reception or transmission of a packet.
		 * While this function may be overridden, generally MultIFModule objects
		 * will not directly exist on an input or output chain of an adapter
		 * and hence should not see activity via this function.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		virtual char		processPacket (Packet * pPacket);	
	
		/** Process a packet from one of the underlying tap modules and
		 * appropriate process the packet to the same behavior as a processPacket
		 * call from a normal IOModule object
		 */
		virtual char	processTapPacket (Packet * pPacket, int nID)=0;
		
		/** Given the name of a tap module, map that module name to a specific
		 * ID that will be used whenever packets are seen on that particular
		 * tap. This function must be overridden to encapsulate the behavior
		 * how the names map to IDs. In general, each tap should have a unique
		 * ID within the context of this module if the module will need to
		 * hold packets outside of the normal input or output chain behavior.
		 * @param sName The name of the module
		 * @returns The ID to use for that module where -1 is a failure in name resolution
		 */
		virtual  int 	mapNameToID (string sName)=0;	
		
		/** Register a tap for reverse linkage from this module. Note that this
		 * function is not thread safe as it is assumed to only occur during startup
		 * of ScaleBox. 
		 * @param pTap The tap to register 
		 */
		void	registerTap (PacketTap * pTap);
		
		/** Find the packet tap with the associated ID.  If there are multiple packet
		 * taps with the same ID (possible with a monitoring module), only the first
		 * item in the list will be returned. Note that this function is not thread
		 * safe versus registerTap as it assumes the tap list is static during normal operations.
		 * @param nID The ID of the packet tap to look for
		 * @returns A pointer to a valid PacketTap object if the ID is found, NULL otherwise
		 */
		PacketTap *	findTapByID (int nID);
		
		/** Have the packet resume processing on the chain after the tap noted
		 * by the listed ID. A failure to resume processing means that ownership of
		 * the packet is still in the hands of the callee.  
		 * @param nTapID The tap ID connected to this module to resume processing after
		 * @param pPacket A pointer to a valid Packet object that will be the one resuming processing
		 * @returns True if successful, false otherwise
		 */ 
		bool	resumePostTap (int nTapID, Packet * pPacket);
		
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

		/** Write the packet to the adapter associated with the specified tap ID
		 * and add the specified packet to the outbound processing chain for that
		 * particular adapter
		 * @param nTapID The tap ID we want to work with
		 * @param pPacket The packet to write out
		 * @returns True if successful, false otherwise
		 */
		bool 	writeBackViaTap (int nTapID, Packet * pPacket);
		
	private:
		/** A list of all of the taps that are registered to point into this
		 * specific multi-interface module
		 */
		vector<PacketTap *>		m_Taps;
	
	
};

#endif /*MULTIIFMODULE_H_*/
