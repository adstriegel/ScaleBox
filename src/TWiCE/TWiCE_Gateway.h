/* TWiCE_Gateway.h
 **********************************************************
 * This code is part of the TWiCE (Transparent Wireless
 * Capacity Enhancement) system developed at the University
 * of Notre Dame.
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 *
 * Students: Dirk VanBruggen		Shu Liu
 *			 Dave Salyers			Yingxin Jiang
 *
 **********************************************************
 * $Revision: 1.12 $  $Date: 2008/04/11 22:52:01 $
 **********************************************************
 */

#ifndef __TWICE_GATEWAY_H_
#define __TWICE_GATEWAY_H_

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

#include "TWiCE_CacheTable.h"
#include "TWiCE_PacketPool.h"

/** Traffic can come in one of two sides, either from the wireless clients or from
 * the upstream Internet side
 */
enum eTWiCE_GatewayTaps {
	TWICE_TAP_NONE=0,
	TWICE_TAP_WAN,			// Traffic from the Internet side
	TWICE_TAP_WIRELESS		// Traffic from the wireless side
};


/** Enumerated values for the different types of stats that the RIPPS monitor
 * will keep track of
 */
enum eTWiCE_GatewayStats {
	TWICE_GATEWAY_WAN_PKTS,		// Raw number of packets received from the WAN-side
	TWICE_GATEWAY_WAN_VOL,		// Raw bandwidth total received from the WAN-side
	TWICE_GATEWAY_WAN_PKTS_TCP,		// Packet count - TCP (WAN)
	TWICE_GATEWAY_WAN_VOL_TCP,		// Volume of TCP packets
	TWICE_GATEWAY_WAN_PKTS_UDP,		// Packet count - UDP (WAN)
	TWICE_GATEWAY_WAN_VOL_UDP,		// Volume of UDP packets
	TWICE_GATEWAY_WAN_PKTS_IPV4,	// Packet count - IPv4
	TWICE_GATEWAY_WAN_VOL_IPV4,		// Volume of IPv4 packets
	TWICE_GATEWAY_WAN_PKTS_IPV6,	// Packet count - IPv6
	TWICE_GATEWAY_WAN_VOL_IPV6,		// Volume of IPv6 packets
	TWICE_GATEWAY_WAN_PKTSV4_ENC,			// Encrypted packets (IPsec - v4)
	TWICE_GATEWAY_WAN_VOLV4_ENC,			// Encrypted packets (IPsec - v4) - Volume
	TWICE_GATEWAY_WAN_PKTSV6_ENC,			// Encrypted packets (IPsec - v6)
	TWICE_GATEWAY_WAN_VOLV6_ENC,			// Encrypted packets (IPsec - v6) - Volume
	TWICE_GATEWAY_WAN_PKTS_OTHER,			// Non-IP packets - Count
	TWICE_GATEWAY_WAN_VOL_OTHER,			// Non-IP packets - Volume
	TWICE_GATEWAY_WAN_FILTER_PKTS_SIZE,		// Size filtering count due to size - count
	TWICE_GATEWAY_WAN_FILTER_VOL_SIZE,		// Size filtering count due to size - volume

	TWICE_GATEWAY_WIRELESS_PKTS,		// Raw number of packets received from wireless side
	TWICE_GATEWAY_WIRELESS_VOL,			// Raw volume of packets received from wireless side
	TWICE_GATEWAY_WIRELESS_PKTS_TWICE,	// TWiCE-specific inbound wireless packets - count
	TWICE_GATEWAY_WIRELESS_VOL_TWICE,	// TWiCE-specific inbound wireless packets - volume

	TWICE_GATEWAY_WIRELESS_PKTS_NORMAL,	// Non-TWiCE inbound wireless packets - count
	TWICE_GATEWAY_WIRELESS_VOL_NORMAL,	// Non-TWiCE inbound wireless packets - volume

	TWICE_GATEWAY_WAN_WCACHE_TOO_SMALL_PKTS,		// Count of packets that are too small
	TWICE_GATEWAY_WAN_WCACHE_TOO_SMALL_VOL,			// Volume of packets that are too small

	TWICE_GATEWAY_WAN_WCACHE_HITS,		// Packets with a hit
	TWICE_GATEWAY_WAN_WCACHE_HIT_LENGTH,	// Length of the hits (cumulative)
	TWICE_GATEWAY_WAN_WCACHE_HIT_MULTI,		// Multiple hits per packet
	TWICE_GATEWAY_WAN_WCACHE_HIT_MULTI_POTENTIAL,
	TWICE_GATEWAY_WAN_WCACHE_HITS_TCP,		// Hit from TCP packet
	TWICE_GATEWAY_WAN_WCACHE_HITS_UDP,		// Hit from UDP packet
	TWICE_GATEWAY_WAN_WCACHE_HITS_TCP_VOL,	// Hit from TCP packet (volume)
	TWICE_GATEWAY_WAN_WCACHE_HITS_UDP_VOL,	// Hit from UDP packet (volume)


	TWICE_GATEWAY_WAN_WCACHE_CLONE_PKT,
	TWICE_GATEWAY_WAN_WCACHE_RECURSE,

	TWICE_GATEWAY_WAN_WCACHE_NOHIT,
	TWICE_GATEWAY_WAN_WCACHE_NOHIT_RECURSE,
	TWICE_GATEWAY_WAN_WCACHE_NOHIT_NOCACHE,

	TWICE_GATEWAY_WAN_MATCH_COUNT_LTE96,
	TWICE_GATEWAY_WAN_MATCH_COUNT_LTE128,
	TWICE_GATEWAY_WAN_MATCH_COUNT_LTE256,
	TWICE_GATEWAY_WAN_MATCH_COUNT_LTE512,
	TWICE_GATEWAY_WAN_MATCH_COUNT_LTE1024,
	TWICE_GATEWAY_WAN_MATCH_COUNT_GT1024,

	TWICE_GATEWAY_WAN_PKTS_MATCH,
	TWICE_GATEWAY_WAN_PKTS_TCP_MATCH,
	TWICE_GATEWAY_WAN_PKTS_UDP_MATCH,


	TWICE_GATEWAY_STAT_LAST
};

/** Statistics for the TWiCE gateway
 */
class TWiCE_GatewayStats : public Stats {
	public:
		TWiCE_GatewayStats ();

		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);


};

#define TWICEGW_DEFAULT_MINPKTSIZETOCACHE		256

#define TWICEGW_TIMER_ID_POOL_MAINTENANCE			1
#define TWICEGW_TIMER_POOL_MAINTENANCE_INTERVAL		1000000

#define TWICEGW_TIMER_ID_LOG_WRITE				2
#define TWICEGW_TIMER_LOG_WRITE_INTERVAL		60000000


enum eTWiCE_WCacheResults
{
	TWICE_WCACHE_RESULT_UNKNOWN=0,		// Probably bad if we have this value
	TWICE_WCACHE_RESULT_NOCHANGE,		// Packet was unchanged, just pass it on, we are not caching
										// it at all due to it likely being HTTPS, IPsec, or something
	TWICE_WCACHE_RESULT_HIT,			// Hit and it was tokenized, whoot
};

/** The monitor object that contains the settings of the RIPPS monitoring
 *  subsystem
 */
class TWiCE_Gateway : public MultiIFModule {

	public:
		TWiCE_Gateway();
		~TWiCE_Gateway ();

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

		/** Process a packet that came in on the WAN side
		 */
		char processPacket_WAN (Packet * pPacket);

		/** The minimum packet size that will be considered by the TWiCE cache which typically tends to
		 * be the L2, L3, L4 header info and one core block.  It can be smaller but that can
		 * be problematic.
		 *
		 * @returns The minimum packet size for consideration
		 */
		int		getMinimumPacketSizeToTryCache ();

		/** The minimum packet size that will be considered by the TWiCE cache which typically tends to
		 * be the L2, L3, L4 header info and one core block.  It can be smaller but that can
		 * be problematic.  Note that this is the whole packet size being specified, not just the
		 * payload size.
		 *
		 * @param nMinSize The minimum size that will be considered for caching (in bytes)
		 */
		void 	setMinimumPacketSizeToTryCache (int nMinSize);

		/// @param pInPacket The input packet to consider for caching
		/// @param nResult   The result (passed via reference)
		/// @param nPayloadOffset The offset into the actual data payload (from the start of the packet) in bytes
		/// @param pBasePacket    A copy of the packet used to place into the packet pool, NULL on first call, non-NULL on recursed calls
		/// @param bSavedInPool   Flag denoting if the packet was saved in the pool, i.e. do not release it out
		Packet *	tryWirelessPacketCache (Packet * pInPacket, int & nResult, int nPayloadOffset, Packet * pBasePacket, char & bSavedInPool, TWiCEPacketPoolEntry * pPoolEntry);



		struct timeval * 	computeNextPoolMaintenance (char byTimeMode);

		struct timeval * 	computeNextLogWrite (char byTimeMode);


		/// @brief Set the packet pool maintenance interval (in microseconds)
		void	setInterval_PoolMaintenance (unsigned int lInterval);
		unsigned int 	getInterval_PoolMaintenance ();

	protected:
		void	logStatsToFile ();

	private:
		// The real stats
		TWiCE_GatewayStats    m_Stats;

		// A placeholder for differentials
		TWiCE_GatewayStats    m_DiffStats;

		// The prior stats for comparison
		TWiCE_GatewayStats    m_PriorStats;



		TWiCECacheTable	*	  m_pCacheTable;

		TWiCEPacketPool	*	  m_pPacketPool;

		// Settings
		int		m_nMinPktSizeTryCache;

		unsigned int		m_nInterval_PoolMaintenance;
		struct timeval		  m_nextPoolMaintenance;

		struct timeval		  m_nextLogWrite;


		// Mutex for packet pool entries
		pthread_mutex_t 			m_MutexPacketPool;

		ofstream		m_LogFile;

};

#endif /* TWICE_GATEWAY_H_ */
