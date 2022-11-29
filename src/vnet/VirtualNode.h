#ifndef VIRTUALNODE_H_
#define VIRTUALNODE_H_

#include "../pkt/MultiIFModule.h"

/** A node in the virtual network
 */
class VirtualNode : public MultiIFModule {
	public:
		VirtualNode ();
		~VirtualNode ();
		
		/** Process a packet from one of the underlying tap modules and
		 * appropriate process the packet to the same behavior as a processPacket
		 * call from a normal IOModule object
		 */
		virtual char	processTapPacket (Packet * pPacket, int nID);
		
		/** Given the name of a tap module, map that module name to a specific
		 * ID that will be used whenever packets are seen on that particular
		 * tap. This function must be overridden to encapsulate the behavior
		 * how the names map to IDs.
		 * @param sName The name of the module
		 * @returns The ID to use for that module where -1 is a failure in name resolution
		 */
		virtual  int 	mapNameToID (string sName);		
	
	
	
};

#endif /*VIRTUALNODE_H_*/
