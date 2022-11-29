

#ifndef VGDM_H_
#define VGDM_H_

class InputIngressVGDM;
class InputEgressVGDM;
class Packet;
/* VGDM.h
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


/** The unifying entity closer to the source that does the
 * conversions to stealth multicast.
 */
class VGDM {
	public:
		VGDM ();
		~VGDM ();
		
		/** Process a packet that arrived on the ingress side
		 * @returns 1 if we gobbled it, 0 if it continues onwards
		 */
		char		processIngressPacket (Packet * pPacket);

		/** Process a packet that arrived on the egress side
		 * @returns 1 if we gobbled it, 0 if it continues onwards
		 */
		char		processEgressPacket (Packet * pPacket);
		
		/** Filter packets that are received from the incoming
		 * adapter based on pre-defined criteria such as inclusion
		 * lists, not IP, or other rules.  A filtered packet should
		 * simply be forwarded on to the output adapter in a passthrough
		 * manner.  
		 * @param pPacket Packet to examine for filtering
		 * @returns 1 if filtered (ignore it), 0 if packet can continue onwards in
		 * the VGDM ingress functions
		 */
		char		ingressFilter (Packet * pPacket);
	
		/** Process control messages on the ingress (source side) of
		 * the packets
		 * @param pPacket Packet to examine for control messages
		 * @returns 1 if packet handled by control (do not forward or continue),
		 *          0 if the packet should continue onwards 
		 */
		char 		ingressProcessControl (Packet * pPacket);
	
	
		/** Process the packet to determine if should be queued for
		 * possibly being sent by stealth multicast, never queued,
		 * or cloned for a background examination
		 * @param pPacket The packet to examine
		 * @returns 1 if the packet is taken by the VGDM for queueing (do not forward),
		 *  0 if the packet should continue onwards
		 */
		char		ingressCheckQueue		 (Packet * pPacket);
	
	
		/** Filter packets that should not be processed by the VGDM on the
		 * egress side
		 * @param pPacket Packet to examine for filtering
		 * @returns 1 if filtered (ignore it), 0 if packet can continue onwards
		 * in the VGDM egress filter functions
		 */
		char		egressFilter (Packet * pPacket);
	
		/** Set the input module associated with ingress (incoming) operations
		 * for the VGDM.  The ingress can be thought of as the LAN-side or
		 * source side view for packets that are highly likely to be amenable
		 * to stealth multicast.
		 * @param pIngress A valid InputIngressVGDM object
		 */
		void		setIngress (InputIngressVGDM * pIngress);

		/** Set the input module associated with egress (exit) operations
		 * for the VGDM.  The egress or exit side captures incoming control
		 * packets from downstream stealth multicast entities. It does not
		 * attempt to apply stealth multicast to the egress (client) side.   
		 * @param pEgress A valid InputEgressVGDM object
		 */
		void		setEgress  (InputEgressVGDM * pEgress);
	
	private:
		/** The input module for the ingress (source) side. The ingress module
		 * is the one that actually has ownership of this VGDM object.
		 */
		InputIngressVGDM	*	m_pIngress;
		
		/** The input module for the egress (client) side that captures incoming
		 * control packets from downstream stealth multicast entities.  
		 */
		InputEgressVGDM *		m_pEgress;
	
};

#endif /*VGDM_H_*/
