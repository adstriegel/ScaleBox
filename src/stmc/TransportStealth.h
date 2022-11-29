#ifndef TRANSPORTSTEALTH_H_
#define TRANSPORTSTEALTH_H_

class PacketGroup;

/** The base class for defining transport mechanisms available to
 * stealth multicast.  A transport group manages the underlying
 * physical group (actual multicast group).  The transport group
 * takes a virtual group and returns the actual distribution of packets
 * that should be sent out onto the wire
 */
class TransportStealth {
	public:
		TransportStealth ();
		
		
		/** Convert a group of redundant packets (whole payload) into
		 * the appropriate unicast and multicast packets for transmission
		 * @param pInGroup The group of packets to convert for transmission
		 * across this physical transport mechanism
		 * @returns A non-NULL listing of packets for transmission via the
		 * adapter
		 */
		virtual PacketGroup *	convertMulticast (PacketGroup * pInGroup)=0;	
	
	
}; 

/** The downstream client for the transport protocol. This client will
 * decapsulate the packet if appropriate or forward the packet as
 * necessary
 */
class TransportStealthClient {
	public:
		TransportStealthClient ();
		~TransportStealthClient ();
		
	private:	
	
	
};

#endif /*TRANSPORTSTEALTH_H_*/
