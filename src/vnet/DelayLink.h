/* DelayLink.h
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
 * $Revision: 1.2 $  $Date: 2008/02/25 21:56:31 $
 **********************************************************
 */

#ifndef DELAYLINK_H
#define DELAYLINK_H

#include "../pkt/IOModule.h"
#include <string>
using namespace std;

/** Apply a fixed delay to packets and upon completion of
 * that delay, deliver the packet to the targeted adapter
 * (virtual or otherwise)
 */
class DelayLinkModule : public IOModule {
	public:
		DelayLinkModule ();
		
		~DelayLinkModule ();

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
	
		/** Set the speed of the link (in b/s)
		 * @param fSpeed The new speed for the link in bits / second */
		void 	setLinkSpeed (unsigned int nSpeed);

		/** Get the speed of the link
		 * @returns The speed of the link in bits / second
		 */
		unsigned int	getLinkSpeed ();

		/** Set the physical delay associated with link progation
		 * @param Physical delay for the link (in seconds)
		 */
		void 	setLinkDelay (float fDelay);
		
		/** Retrieve the physical delay (in seconds) associated with this
		 * link and propagation to the target
		 * @returns The link propagation delay (in seconds)
		 */
		float getLinkDelay ();

		/** Set the overhead associated with transmission of packets across this
		 * link relative to Ethernet (802.3). Note that it may be possible for this
		 * value to be negative if the L2 header and trailer is smaller than Ethernet.
		 * @param nOH Overhead for this link (in bytes)
		 */
		void setLinkOverhead (int nOH);

		/** Retrieve the link overhead (in bytes) associated with layer 2
		 * @returns The relative overhead (in bytes) versus Ethernet
		 */
		int getLinkOverhead (); 
	
		/** Set the inter-frame gap associated with this link. By default, an
		 * inter-frame gap is not included. 
		 * @param fIFG The inter-frame gap between packets (in seconds)
		 */
		void setInterFrameGap (float fIFG);
		
		/** Retrieve the inter-frame gap associated with this link
		 * @returns The inter-frame gap for this link (in seconds)
		 */
		float getInterFrameGap ();	
	
	private:

		/** Target adapter denoting where packets will be delivered to
		 * upon completion of the delay time
		 */
		string	m_sTargetAdapter;
		
		
		/** The raw speed of the link in bits / second
		 */
		unsigned int	m_nLinkSpeed;

		/** Raw delay for the link (in seconds) */ 		
		float		m_fLinkDelay;
		
		
		/** Inter-frame gap */
		float		m_fIFG;

	
		/** The actual speed of the link in bytes / second */
		float		m_fActLinkSpeed;
		
		/** The overhead (in bytes) associated with packets using this link.
		 * It may be negative if the Layer 2 overhead is less than that of
		 * Ethernet (802.3).  
		 */
		int	m_nLinkOverhead;	
				
		
};

#endif /*IOSANDBOX_H*/

