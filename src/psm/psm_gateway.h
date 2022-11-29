/* psm_gateway.h
 **********************************************************
 * This code is part of the PSM (Proximity Secure Mixing)
 * system developed at the University of Notre Dame. The
 * gateway is the MITM (Man In The Middle) / proxy box
 * that mobile nodes direct their traffic towards.
 *
 * For now, we focus on the gateway being in the "cloud"
 * and leave a close-to-AP or base station version to be
 * developed later as those require active participation
 * by a provider as opposed to simply tunneling to the
 * cloud.
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 *
 * Students: Dirk Van Bruggen
 * 			 Shu Liu
 * 			 Lei Meng
 * 			 Xueheng Hu
 *
 **********************************************************
 * $Revision: 1.12 $  $Date: 2008/04/11 22:52:01 $
 **********************************************************
 */

#ifndef __PSM_GATEWAY_H_
#define __PSM_GATEWAY_H_

#include <vector>
using namespace std;

#include <string>
using namespace std;

#include <fstream>
using namespace std;

#include "../mem/MemoryPool.h"
#include "../pkt/MultiIFModule.h"
#include "../stat/Stats.h"
#include "../pkt/Packet.h"

#include "../core/Console.h"

#include "psm_mobile_client.h"

/** The PSM gateway uses the application construct of ScaleBox whereby packets
 * can come in from multiple adapters / interfaces
 */
enum ePSM_GatewayTaps {
	PSM_TAP_NONE=0,
	PSM_TAP_WAN,				// Traffic from the general Internet
	PSM_TAP_VPN_TUN_SIMPLE,		// Traffic from the VPN socket tunnels (typically Android)
	PSM_TAP_VPN_TUN_FULL		// Real VPN traffic (future) from iOS devices
};


/** Enumerated values for the different types of stats that the PSM gateway
 * will keep track of
 */
enum ePSM_GatewayStats {
	PSM_GATEWAY_WAN_PKTS,		// Raw number of packets received from the WAN-side
	PSM_GATEWAY_WAN_VOL,		// Raw bandwidth total received from the WAN-side

	PSM_GATEWAY_VPNTUN_PKTS,	// Raw number of packets received from all VPN tunnels
	PSM_GATEWAY_VPNTUN_VOL,		// Raw bandwidth total received from all VPN tunnels

	PSM_GATEWAY_VPNTUN_SIMPLE_PKTS,		// Simple tunneled VPN packets (UDP) - count
	PSM_GATEWAY_VPNTUN_SIMPLE_VOL,		// Simple tunneled VPN packets (UDP) - volume

	PSM_GATEWAY_STAT_LAST
};

/** Statistics for the PSM gateway
 */
class PSM_GatewayStats : public Stats {
	public:
		PSM_GatewayStats ();

		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);


};


// We are by default in a monitor only mode
#define PSM_GATEWAY_FLAG_MONITORONLY_DEFAULT		1

// The default port to which simple UDP tunnels should be directed
#define PSM_GATEWAY_SIMPLE_TUNNEL_PORT_DEFAULT		8000

/** The PSM gateway is the overall manager for all traffic coming in / leaving from
 * the intermediate cloud box doing processing. It is based on the MultiIFModule to
 * theoretically allow it to use multiple adapters though more likely than not, it
 * will simply be doing only a single adapter for all in and outbound traffic.
 */
class PSM_Gateway : public MultiIFModule {

	public:
		PSM_Gateway();
		~PSM_Gateway ();

		/** Given the name of a tap module, map that module name to a specific
		 * ID that will be used whenever packets are seen on that particular
		 * tap. This function must be overridden to encapsulate the behavior
		 * how the names map to IDs.
		 * @param sName The name of the module
		 * @returns The ID to use for that module where -1 is a failure in name resolution
		 */
		virtual int 	mapNameToID (string sName);

		/** Process a packet from one of the underlying tap modules and
		 * appropriate process the packet to the same behavior as a processPacket
		 * call from a normal IOModule object
		 */
		virtual char	processTapPacket (Packet * pPacket, int nID);

		/** Initialize an input module by applying any necessary
		 * startup functionality such as threads, thread safety, or
		 * other initializations.
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the
		 * extended information specific to the child class.
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */
		virtual bool	extractExtendedDOM (NodeDOM * pNode);

		/** Process a timer event from the associated TimerAggregator
		 * object contained within this class if timers are enabled
		 * for this particular module
		 * @param pEvent The timer event that has occurred
		 * @returns True if successfully processed, false if there was an error
		 */
		virtual bool		processTimer (TimerEvent * pEvent);

		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);

		/** Process a command from the console or external control
		 * mechanism
		 * @param theCommands The vector of commands to process
		 * @param nOffset The offset at which to begin processing. The offset allows
		 *   us to pass the vector itself with modifications to the underlying
		 *   modules
		 * @returns Enumerated value representing result of command processing
		 */
		virtual CommandResult	processExtCommand (const vector<string> & theCommands, int nOffset);


		/** Process the fact that a write callback has been invoked due to a
		 * packet finally being written out due to interposed queuing delay
		 * or other mechanisms
		 * @param pPacket The packet involved in the callback
		 * @param pData The data associated with the callback
		 * @returns True if successful, false otherwise
		 */
		virtual bool	processWriteCallback (Packet * pPacket, void * pData);


		/** Get the port number being used for inbound simple tunnels
		 *
		 * @returns The port number being used
		 */
		unsigned short 	getPort_SimpleTunnel ();

		/** Set the port number for the simple tunnel. As the simple tunnel uses UDP,
		 * this can be changed on-demand though that is not recommended.
		 *
		 * @param nPort		The new port (recommended to be > 1000)
		 */
		void			setPort_SimpleTunnel (unsigned short nPort);

		/** Get the state regarding whether or not the gateway should only monitor
		 * and forward packets rather than actively trying to apply various
		 * optimizations.
		 *
		 * @returns True if only monitoring, false if in-band optimizations enabled
		 */
		bool			getFlag_MonitorOnly ();

		/** Set the state regarding if the gateway should try to optimize the in-band
		 * connections to the mobile nodes. For testing purposes, it is generally
		 * recommended to start in monitor mode and then turn on optimizations
		 * once proper forwarding has been verified.
		 *
		 * @param bMonitorOnly The new optimization setting
		 */
		void			setFlag_MonitorOnly (bool bMonitorOnly);

	protected:
		void	logStatsToFile ();

	private:
		// The client manager
		PSM_MobileClient_Manager *	m_pClientMgr;

		// The real stats
		PSM_GatewayStats    m_Stats;

		// The port for inbound simple tunneling
		unsigned short		m_nPort_SimpleTunnel;

		// Monitor only
		bool				m_bMonitorOnly;

};

#endif /* TWICE_GATEWAY_H_ */



