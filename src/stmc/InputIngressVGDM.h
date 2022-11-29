/* InputIngress.h
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
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#ifndef INPUTINGRESSVGDM_H_
#define INPUTINGRESSVGDM_H_

#include "../pkt/IOModule.h"

class		VGDM;

/** Input module for processing packets at the ingress for the
 * VGDM as part of stealth multicast 
 */
class InputIngressVGDM : public IOModule {
	public:
		InputIngressVGDM ();
		~InputIngressVGDM ();
		
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
		
		
		/** Return a pointer to the Virtual Group Detection Manager (VGDM)
		 * associated with this ingress node. This is used to allow the 
		 * egress input module to associate itself with the VGDM for proper
		 * message/packet passing from both sides of the dual-homed VGDM.
		 * @returns A pointer to the currently allocated VGDM object. A NULL
		 * value denotes that the InputIngressVGDM module has not yet been
		 * initialized.
		 */
		VGDM *	getVGDM ();
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);		
		
	protected:
		/** Create the VGDM and link the ingress portion of it to this
		 * input module. 
		 */
		void		createVGDM ();
		
	private:
		/** A pointer to the VGDM for the node. The InputIngressVGDM module
		 * is the effective owner of the VGDM object itself with regards to
		 * cleanup. The InputEgressVGDM object simply shares a reference to
		 * the VGDM.
		 */	
		VGDM		*		m_pVGDM;
	
}; 	
	
#endif /*INPUTINGRESSVGDM_H_*/
