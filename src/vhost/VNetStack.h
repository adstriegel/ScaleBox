

#ifndef __VNETSTACK_H_
#define __VNETSTACK_H_

#include <vector>
using namespace std;

#include "VSocketDatagram.h"
#include "VSocketStream.h"

#include "VNetStackModule.h"

class VirtualHost;


/** A virtual network stack that manages the connections for a particular host.
 * The network stack receives packets and receives packets in both the read
 * and write directions from the various sockets created to interact with
 * applications and the various adapters connected to the host itself.  
 */
class VNetStack : public MultiIFModule {
	public:
		VNetStack ();
		
		~VNetStack ();

		/** Initialize an input module by registering the adapter it is 
		 * associated with and applying any necessary startup functionality such
		 * as threads, thread safety, or other initializations. 
		 * @returns True if successful, false otherwise
		 */
		virtual char		initialize ();		
		
		/** Process a packet from one of the underlying tap modules and
		 * appropriate process the packet to the same behavior as a processPacket
		 * call from a normal IOModule object
		 */
		char	processTapPacket (Packet * pPacket, int nID);		
		
		/** Create a datagram socket for transmission / receipt for an application
		 * @param nReqPort The port number to request, dynamically assigned if not specified. A collision with an
		 *         existing socket will result in a failure to allocate the requested socket.
		 * @returns A pointer to a valid VSocketDatagram if successful, NULL otherwise
		 */
		VSocketDatagram * 	createDatagramSocket (unsigned short nReqPort=0);

		/** Create a datagram socket for transmission / receipt for an application
		 * @param nReqPort The port number to request, dynamically assigned if not specified. A collision with an
		 *         existing socket will result in a failure to allocate the requested socket.
		 * @returns A pointer to a valid VSocketStream if successful, NULL otherwise
		 */
		VSocketStream * 	createStreamSocket (unsigned short nReqPort=0);		
		
		/** Retrieve the virtual host of which this virtual network stack is
		 * associated with
		 * @returns A pointer to the virtual host that this stack is currently associated with
		 */
		VirtualHost	*	getHost ();
		
		/** Set the virtual host for which this virtual network stack is associated with
		 * @param pHost The new virtual host to associate with the stack
		 * @returns True if successful, false otherwise
		 */
		bool	setHost (VirtualHost * pHost);
	
		
	private:
		/** The host that this stack is associated with */
		VirtualHost		m_pHost;
	
};

/** A thread dedicated to a particular host-wise network stack. The thread is responsible for
 * dispatching packets out onto the virtual (or real) network
 * @param pStack A pointer to the network stack object managed by the thread
 */
void * 	Thread_VNetStack (void * pStack);

#endif
