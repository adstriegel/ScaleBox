#ifndef INPUTEGRESSVGDM_H_
#define INPUTEGRESSVGDM_H_

#include "../pkt/IOModule.h"

class VGDM;
class InputIngressVGDM;

/** Input module for processing packets at the egress (WAN-side) for the
 * VGDM as part of stealth multicast 
 */
class InputEgressVGDM : public IOModule {
	public:
		InputEgressVGDM ();
		~InputEgressVGDM ();
		
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
		
		/** Link this module to the VGDM via the ingress VGDM module.
		 * This must be only called after the input module itself has been
		 * initialized as the VGDM object itself is not created until after
		 * the InputIngressVGDM object has been associated with an adapter
		 * @param pIngress A valid pointer to the InputIngressVGDM moduel
		 */
		void 		linkIngress (InputIngressVGDM * pIngress);
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);		
		
	private:
		VGDM		*		m_pVGDM;
	
}; 

#endif /*INPUTEGRESSVGDM_H_*/
