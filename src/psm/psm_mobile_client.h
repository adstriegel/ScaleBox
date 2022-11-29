/*
 * psm_mobile_client.h
 *
 *  Created on: Oct 26, 2012
 *      Author: striegel
 */

#ifndef __PSM_MOBILE_CLIENT_H_
#define __PSM_MOBILE_CLIENT_H_

#include <vector>
using namespace std;

#include <string>
using namespace std;

#include <fstream>
using namespace std;

#include "../mem/MemoryPool.h"
#include "../stat/Stats.h"
#include "../pkt/Packet.h"

/* Set up statistics for the particular client
 *
 */

/* Statistics for the overall client manager
 *
 */


/** A single client that is currently using the services of the gateway. For now,
 * we will not make this a poolable object until we start to significantly ramp up
 * in terms of clients.
 *
 */
class PSM_MobileClient
{
	public:
		PSM_MobileClient ();
		~PSM_MobileClient();

	private:
		// The address of the particular client
		NetAddress		m_Address;

		// When did we first see info from the client
		struct timeval	m_CreationTime;

		// When did we see the last packet from the client
		struct timeval	m_LastClientPkt;

		// Are we optimizing at all for the client
		bool	m_bMonitorOnly;

	// To what extent should we optimize
};

/** The manager for all of the clients at this particular gateway
 *
 */
class PSM_MobileClient_Manager
{
	public:
		PSM_MobileClient_Manager ();
		~PSM_MobileClient_Manager ();

		PSM_Mobile_Client * findClientByAddress (NetAddress * pAddress);

	private:

};

#endif /* PSM_MOBILE_CLIENT_H_ */
