

#ifndef __VNETSTACKMODULE_H_
#define __VNETSTACKMODULE_H_

enum eVNetStackModDir {
	VNETSTACKMOD_DIR_INBOUND=0,
	VNETSTACKMOD_DIR_OUTBOUND
};

/** The VNetStackModule object forms the basis for modules that will process
 * and send packets in the network stack.  It is derived from the MultiIFModule
 * class primarily for ease of use in the statistics and command framework. 
 */
class VNetStackModule : public MultiIFModule {
	public:
		VNetStackModule ();
		
		~VNetStackModule ();
		
		/** Initialize an input module by registering the adapter it is 
		 * associated with and applying any necessary startup functionality such
		 * as threads, thread safety, or other initializations. 
		 * @returns True if successful, false otherwise
		 */
		virtual char		initialize ();		
		
		/** Process a packet in either the inbound or outbound directions. 
		 * @returns 1 if the packet was handled by the stack module, 0 if it was
		 *   not meaning that more processing is required
		 */
		char	processTapPacket (Packet * pPacket, int nID);			
	
	private:
	
};


#endif

