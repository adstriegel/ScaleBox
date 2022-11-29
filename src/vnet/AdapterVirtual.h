/* AdapterVirtual.h
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

#ifndef ADAPTERVIRTUAL_H_
#define ADAPTERVIRTUAL_H_

#include "../pkt/Adapter.h"
#include "../util/ip-utils.h"
#include "../util/net-const.h"		// Various network constants

class Packet;

/** A class derived from the base Adapter class to provide functional
 * for a virtual (simulated) network.  The virtual adapter allows for
 * testing of module inter-operability without involving multiple hosts.  
 */
class AdapterVirtual : public Adapter {
	public:
		AdapterVirtual ();
		~AdapterVirtual ();

		/** Dump the basic information to the console 
		 */		
		virtual void			dumpBasicInfo  ();
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		bool		extractExtendedDOM (NodeDOM * pNode);


		/** Read a packet and give it to the input chain
		 */
		void			readPacket ();		
			
		/** Update the packet currently being read in by the read
		 * thread.  This function is used solely for the purpose of
		 * callback from the libpcap callback function as there is only
		 * one argument passed in.
		 */
		void			setReadPacket (Packet * pPacket);

		/** Start up the device as called from the parent class.  The function
		 * simply calls openDevice to start up the threads for the class
		 */
		virtual void			startDevice ();
		
		/** Write this specific packet to the adapter itself for
		 * direct output onto the network medium
		 * @param pPacket The packet to add to the queue for output
		 */
		virtual void			writePacket (Packet * pPacket);
		

		 		
	private:
		
};

#endif /*ADAPTERVIRTUAL_H_*/
