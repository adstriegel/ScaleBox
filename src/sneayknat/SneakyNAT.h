/* SneakyNAT.h
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
 * $Revision: 1.1 $  $Date: 2008/02/14 21:38:26 $
 **********************************************************
 */
 
 
#ifndef SNEAKYNAT_H_
#define SNEAKYNAT_H_

/** A module that takes in packets from multiple vantage points
 * to avoid declaring individual input and output modules versus
 * a single unified entity that serves the same purpose. In some
 * sense, the MultiIFModule (Multiple Interface Module)
 * can be thought of as resembling an application whose input 
 * is a tap into the underlying directional packets.
 */ 

class SneakyNATModule : public MultiIFModule { 
	public:
		SneakyNATModule ();
		~SneakyNATModule ();
	
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
						
	private:	
};


#endif

