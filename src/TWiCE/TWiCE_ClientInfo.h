/*
 * TWiCE_ClientInfo.h
 *
 *  Created on: Jul 6, 2011
 *      Author: striegel
 */

#ifndef TWICE_CLIENTINFO_H_
#define TWICE_CLIENTINFO_H_

#include "../pkt/Packet.h"

#include <vector>
using namespace std;

/** An entry in the client table for the gateway
 *
 */
class TWiCEClientEntry
{
	public:

	private:
		// The MAC address of the client (highly relevant)

		// The IP address of the client (somewhat relevant)

		// When it appeared
		struct timeval		m_JoinTime;

		// When it last got a packet
		struct timeval		m_LastPktTime;
};

/** The list of clients being watched over by the gateway
 *
 */
class TWiCEClientList
{
	public:

		/** Retrieve the number of clients that are currently
		 * registered with the gateway
		 * @returns The number of clients
		 */
		int		getNumberClients ();


	private:
		vector<TWiCEClientEntry *>	m_Clients;
};

#endif /* TWICE_CLIENTINFO_H_ */
