/*
 * psm_flow.h
 *
 *  Created on: Oct 26, 2012
 *      Author: striegel
 */

#ifndef PSM_FLOW_H_
#define PSM_FLOW_H_

#include <vector>
using namespace std;

#include <string>
using namespace std;

#include <fstream>
using namespace std;


#include "psm_mobile_client.h"

/** An individual flow that is being handled by the PSM gateway.  One could think of
 * a flow as a particular connection tuple.
 */
class PSM_Flow
{
	private:
		// What does the core (payload) of the packet look like?
		//
		// The packet as it comes to us (the gateway) has our destination IP and our
		// destination port if they are doing simple tunneling.  Inside of that packet,
		// there is the actual destination IP and the actual destination port, likely
		// done from nearly an entire IP in IP perspective.

		// Inbound tuple info
		NetAddress		m_ActualClientSourceIP;
		NetAddress		m_ActualClientSourcePort;

		NetAddress		m_ActualDestinationPort;
		NetAddress		m_ActualDestinationIP;

		// The value of the IP protocol field (in the payload)
		unsigned char	m_byProtocol;

		// What should the outbound info be translated to? Since we are bringing together
		// multiple mobile clients, we need to do a NAT on those particular connections. For
		// now, we are going to keep things fairly simple separating all ports but eventually
		// we can get sophisticated and separate by dest IP / port.
		NetAddress		m_TranslatedSourcePort;

		// Allow us to quickly check on the client that this flow is associated with
		PSM_MobileClient	* 	m_pClient;
};

/** A simple list of flows
 *
 */
class PSM_List_Flows
{
	public:
		PSM_List_Flows ();

	private:
		vector<PSM_Flow *>	m_Flows;


};

/** The flow port captures the state of a given port or a given inbound port (WAN / Internet side) and
 * address.  The flow port is on the WAN or Internet side of the translation.  Since the IP on the
 * translation side is constant (or at least relatively so until we allow a range of addresses), that
 * means that we can only distinguish packets coming "back" in by virtue of their port and protocol.
 */
class PSM_Flow_Port
{
	public:
		PSM_Flow_Port ();
		~PSM_Flow_Port ();

		/** Get the flag noting whether or not this particular port or port + address
		 * combination has been allocated.
		 *
		 * @returns True if allocated, false otherwise
		 */
		bool			getFlag_Allocated ();

		/** Set the flag noting whether or not this particular port or this port + destination
		 * address has been allocated.
		 */
		void			setFlag_Allocated (bool bAlloc);

		unsigned short	getPort ();
		void			setPort (unsigned short nPort);

		void				updateLastPacketTime (Packet * pPacket);
		void				setLastPacketTime (struct timeval * pTime);
		struct timeval * 	getLastPacketTime ();

		void				updateAllocationTime (Packet * pPacket);
		void				setAllocationTime  	 (struct timeval * pTime);
		struct timeval * 	getAllocationTime ();

		PSM_MobileClient * 	getClient ();
		void				setClient (PSM_MobileClient * pClient);

		unsigned char		getProtocol();
		void				setProtocl (unsigned char byProtocol);

	private:


		// Is this port allocated?  Technically, we could divine this by the bucket or list this
		//  is in but this allows for extra sanity checking.
		bool				m_bAllocated;


		// Denote whether or not this is discriminated by address
		bool				m_bDiscriminateAddress;

		// The actual port
		unsigned short		m_nPort;

		// When did we see the last packet?
		struct timeval 		m_LastPacket;

		// When did this allocation get done?
		struct timeval		m_Allocation;

		// Which client is this for (if allocated)?
		PSM_MobileClient *	m_pClient;
};

/** A list of ports either allocated or available to the flow translator
 *
 */
class PSM_List_Flow_Ports
{



};

// Where do we start allocating ports (UDP)?
#define PSM_FLOWTRANSLATE_STARTPORT_UDP		40000

// Where do we start allocating ports (TCP)?
#define PSM_FLOWTRANSLATE_STARTPORT_TCP		40000

// How many ports do we allocate in a block?
#define PSM_FLOWTRANSLATE_ALLOC_BLOCK		100

// How long until an idle flow expires?
#define PSM_FLOWTRANSLATE_EXPIRE_TIME		60

/** A flow translator takes a given flow and converts it either from an externally translated
 * address to the internal address or vice versa.  The translator keeps track of open / allocated
 * ports.
 */
class PSM_Flow_Translator
{
	public:

		/** Create more identifiers to allow us to translate more ports
		 * @param byProtocol The protocol (TCP or UDP) in question
		 * @returns True if successful, false if we failed to get more ports
		 */
		bool		getMoreFlowPorts (unsigned char byProtocol);


	private:
		unsigned int		m_nNextPort_TCP;
		unsigned int		m_nNextPort_UDP;
};


#endif /* PSM_FLOW_H_ */
