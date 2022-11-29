/* RIPPS_Host.h
 **********************************************************
 * This code is part of the RIPPS (Rogue Identifying
 * Packet Payload Slicer) system developed at the University
 * of Notre Dame. 
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Chad Mano (lead)      Yingxin Jiang
 *           Dave Salyers          Dave Cieslak
 *           Qi Liao               Andrew Blaich   
 * 
 **********************************************************
 * $Revision: 1.10 $  $Date: 2008/05/08 18:07:56 $
 **********************************************************
 */

#ifndef __RIPPS_HOST_H
#define __RIPPS_HOST_H

#include <sys/time.h>
#include <pthread.h>				// For the host mutex

#include <vector>
using namespace std;

#include "../pkt/PktQueueFIFO.h"
#include "../mem/MemoryPool.h"
#include "RIPPS_Connection.h"
#include "../stat/Stats.h"

class NodeDOM;

#define DEFAULT_IP_LEN		4
#define DEFAULT_MAC_LEN		6

#define MAX_IP_LENGTH		16
#define MAX_MAC_LENGTH		6

enum {
  GOBBLE_PKT_NO=0,
  GOBBLE_PKT_YES=1,
  GOBBLE_PKT_SEQONLY=2,
  GOBBLE_PKT_USESEQ				// Use the predicted ack from the packet but still gobble 
};

enum SliceState {
	SLICESTATE_NOSLICE=0,
	SLICESTATE_SLICING
};

enum RIPPS_ConnMedium {
	RIPPS_CONN_MEDIUM_UNKNOWN=0,
	RIPPS_CONN_MEDIUM_WIRED=1,
	RIPPS_CONN_MEDIUM_WIRELESS=2 
};
	
enum {
	VALIDATE_STATE_NONE=0,		// Nothing has been done (new host)
	VALIDATE_STATE_RIPPS,		// Engaging RIPPS on the host
	VALIDATE_STATE_LRTT,		// Monitoring LRTT (passive) only
	VALIDATE_STATE_RESOLVED		// State has been resolved
};

enum HostPktFwd {
	HOST_PKT_FWD=0,			// Forward the packet as normal (default)
	HOST_PKT_GOBBLE			// Gobble up the packet
};

enum eRIPPS_HostStats {
	RIPPS_HOST_STAT_PKTS_EXT_INCOMING=0,
	RIPPS_HOST_STAT_PKTS_RETRANS,
	RIPPS_HOST_STAT_LAST
};

/** Statistics for the RIPPS monitor item
 */
class RIPPS_HostStats : public Stats {
	public:
		RIPPS_HostStats ();
};

/** An individual host being observed by the overarching RIPPS monitor
 * module.
 * 
 * The RIPPS_Host object is not part of the memory pool as the object itself
 * is not highly dynamic in that hosts are not actively allocated and discarded.
 * Rather hosts tend to be semi-static fixtures whose properties and observed
 * data/ACK pairs will be highly dynamic.
 * 
 */
class RIPPS_Host {	
	public:
		RIPPS_Host ();
		
		RIPPS_Host (int nDirection, Packet * pPacket);
			
		/** Initialize the basic variables for a host and the default
		 * values
		 */
		void 	initialize ();
				
		/** Retrieve a copy of the statistics for this host
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);
		
		
		void	dumpBrief ();
		
		/** Dump the stats of this particular host with a view into the
		 * open / unresolved sequence number / ack pairs. This call is used
		 * primarily for debugging issues with RIPPS rather than for general
		 * operation
		 */
		void	dumpBrief_withOpen ();
		
		/// @brief Retrieve the length of the IP field used for the host
		/// Note that dual-addressing is currently not supported
		/// @returns Length in bytes of the IP address for the host
		int		getIP_Length ();
	
		/// @brief Retrieve a pointer to the IP address for the host
		/// @returns A valid pointer to byte-addressable IP address
		char *	getIP ();
		
		/// @brief Set the IP address
		/// Set the IP address for this host and the length of said IP address.
		/// If the IP address is too long, it is simply ignored and an error is
		/// sent to cerr
		/// @param pIP The IP address to copy in
		/// @param nLength The length of the IP address in pIP in bytes
		void	setIP (char * pIP, int nLength=DEFAULT_IP_LEN);   
	
		char	isMatch_IP (char * pIP, int nLength=DEFAULT_IP_LEN);
		char	isMatch_MAC (char * pMAC, int nLength=DEFAULT_MAC_LEN);
	
		char *	getMAC ();
		
		int		getMAC_Length ();
		
		void	setMAC (char * pMAC,int nLength=DEFAULT_MAC_LEN);
	
		int		getConnectionMedium ();
		
		void	setConnectionMedium (int nMedium);
	
		/// @brief Add an expected ACK to the list of pending LRTTs
		/// Add an expected ack number to the list of pending ACKs.  Record the arrival
		/// time and the packet characteristics as appropriate.  
		/// @param pPacket A valid pointer to the incoming data packet
		/// @param bGobble Flag to denote if the rcvd ack should be gobbled on exit
		/// @param lNewExpAck The new expected ack to override the derived expected ack
		/// @param nExtraAcks Extra ACKs cloned from this one that are strictly to catch non-matched ACKs
		///							from the client, default should be 0
		void	addExpectedAck (Packet * pPacket, char bGobble, unsigned int lNewExpAck, int nExtraAcks);
		
		/// @brief Add an expected ACK to the list of pending RTTs
		/// This function simply calls the longer addExpectedAck function with the gobble flag
		/// disabled and the new expected ack value ignored
		/// @param pPacket A valid pointer to the incoming data packet
		void addExpectedAck (Packet * pPacket);
		
		/// @brief Should this packet be monitored?
		/// Examine if this packet should be monitored and/or sliced. This function
		/// simply filters packets that are viewed as a retransmission from the source to avoid
		/// issues of ambiguity in the list. It also updates/takes into account the slice cycle
		/// as mandated by the monitor.
		/// @param pPacket The source packet to monitor
		/// @returns 1 if the packet should be monitored (true), 0 otherwise
		char shouldMonitorPkt_WAN (Packet * pPacket);
				
		/// @brief Process a received ack from a host
		/// Lock the mutex for this specific host and search the open connection/ack list for
		/// a match to the acknowledgement contained within the packet. The function will return
		/// what to do with the packet itself
		/// @param pPacket The packet to examine for matching an ack
		/// @returns HOST_PKT_FWD if the packet should be forwarded, otherwise HOST_PKT_GOBBLE
		///  to denote that the packet should be squelched rather than forwarded
		int		processAck (Packet * pPacket);
		
		/// Connection closing driven by external entity
		void	closeConnectionExt (Packet * pPacket);

		/** A close signal for a connection as observed on the monitor
		 * side as driven by the host (TCP FIN, RST).  Since it is host
		 * driven, we can assume that the host no longer will be sending
		 * additional ACKs for data as it thinks the connection is
		 * largely complete.  The connection will be transitioned to
		 * a summary.
		 * @param pPacket The packet with the FIN or RST
		 */	
		bool	closeConnectionMon (Packet * pPacket);
	
		char	shouldApplyRIPPS ();
	
		char	getValidationState ();
		void	setValidationState (char nNewState);
		
		/** @brief If this hosts should be reassessed, get it going
		 * Based on the time (in seconds) given, determine if the host still has a
		 * time for the last assessment that is still valid and if appropriate
		 * start the reassessment
		 * @param pCurTime 			The current time		 
		 * @returns True if reassessment was started, false if no state changed
		 */		
		bool	checkReassess (struct timeval * pCurTime);
	
		/// @brief Get the most recent batch of LRTT values for this host
		double		getLRTT ();
		
		/** @brief Get the last time this host was validated
		 * @returns A copy of the last time at which this particular host finished
		 *   its last validation period
		 */
		struct timeval 	getLastValidation ();

		/** Manually set the time for which this host was last validated. A
		 * validation is determined to be complete from when the critical number
		 * of monitor packets have been successfully observed.
		 * @param pTime A pointer to a valid struct timeval containing the
		 *   time at which the last validation was completed. 		
		 */
		void		setLastValidation (struct timeval * pTime);
	
		unsigned int		getMonitorCount ();
		
		void		startNewSet ();
		
		/** Retrieve the time at which the host thinks the last packet in the
		 * shaping queue subject to shaping will be sent.  This does not include
		 * any packets that arrive after monitoring is complete but packets still
		 * exist in the shaping queue.
		 * @returns A pointer to a valid timeval struct containing the best guestimate
		 *   for when the last packet in the shaping queue will leave  
		 */
		struct timeval *	getShaper_LastTime ();

		/** Set the time at which the host thinks the last packet in the shaping queue
		 * will be sent (or at least attempted to be sent).  
		 * @param pTailExit A pointer to a valid struct timeval denoting when the last
		 *   packet will be forwarded onwards to actual transmission
		 */
		void setShaper_LastTime (struct timeval * pTailExit);


		/// Take all open connections (not closed) and move them to the summary state
		void  summarizeAllConnections ();


		/** @brief Update last dispatch to reflect when the next packet should leave
		 * Through an internal guestimate of the time at which the last packet in the 
		 * shaping queue will be released, compute when the next packet will be released
		 * based on the host-specific shaping time.  
		 * @returns A valid pointer to the internal last dispatch time which denotes
		 * 		  when the callee's packet should be released in absolute time
		 */		 
		struct timeval *   computeShapeDispatch ();
		

		/** Set the shaping delay (minimum inter-packet delay) to be employed when 
		 * packets are sliced to this particular host. While the host is in a monitoring
		 * mode, all TCP packets will be shaped accordingly.
		 * @param nDelayUS Shaping delay between packets in microseconds(0 means no shaping whatsoever)
		 */
	 	void	setValue_ShapeDelay (unsigned int nDelayUS);
	 	
		/** Get the shaping delay (minimum inter-packet delay) to be employed when 
		 * packets are sliced to this particular host. While the host is in a monitoring
		 * mode, all TCP packets will be shaped accordingly.
		 * @returns Shaping delay between packets in microseconds (0 means no shaping whatsoever)
		 */	 	
	 	unsigned int getValue_ShapeDelay ();

		/// @brief Determine if there is a queuing effect versus the current time
		/// Look at the last dispatch time versus the current time to determine if
		/// there is a queue of backlogged packets. Note that this function is only
		/// mostly accurate as it is dependent on how often the shaping buffer itself
		/// gets emptied
		/// @returns 1 if there may be a backlog, 0 if there should not be 
		bool		hasBacklog ();	
		
		/** @brief Change the current monitor set
		 *  The monitor set is used to group specific connections together in different
		 *  monitoring periods for the purpose of archival information. 
		 *  @param nSet  The new set
		 */		
		void		setMonitorSet (unsigned int nSet);
		
		/** Retrieve the current monitor set for this host
		 *  @returns The current monitor set
		 */
		unsigned int	getMonitorSet ();
		

      /** Determine if this host has the connection from the perspective of
       * the monitor side. If it does, return true, otherwise false
       */
      bool    hasConnection (Packet * pPacket);
		
		
		void		startMonitor ();
		
					
		void		incDataPktNum ();
		
		/// @brief Retrieve the current data packet number seen by the host
		/// The data packet number is a simple counter denoting how many packets on
		/// all of the various connections that it has seen
		int			getDataPktNum ();

		/// Check the monitor for summarizing the active connections
		/// @param pCurTime The current system time to avoid multiple system calls
		void		checkSummarizeConnections (struct timeval * pCurTime);				


		/// Archive all data from this data monitoring set backwards
		/// @param nSet The set from which to archive data backwards in time
		void		archiveData		(unsigned int nSet);
			
		/// Restrict data to the most recent sets of data
		/// @param nMaxSets The maximum number of monitor data sets that can be
		///          		  kept by a host
		///
		void		restrictPastData (unsigned int nMaxSets);
			
		/** Set the host-specific decay time from when a successful assessment
		 * is still valid for.
		 * @param nDecayTime The amount of time in seconds that a host is considered "safe"
		 *                   from further evaluation
		 */	
		void		setAssessDecayTime (unsigned int nDecayTime);	

		/** Get the host-specific decay time from when a successful assessment
		 * is still valid for.
		 * @returns The amount of time in seconds that a host is considered "safe"
		 *                   from further evaluation
		 */	
		unsigned int	getAssessDecayTime ();
		
		
		/** Look at the most recent monitor set for the host and validate it
		 * with respect to the medium type.
		 * @returns The medium type derived by the host analysis
		 */ 
		char		validateHost ();
	
		/** Add a packet destined for that host that is subject to shaping
		 * by the host itself. The timer for pulling the packets out of the
		 * shape buffer should be set externally by the caller.  
		 * @param pPacket The packet to be added to the shaping queue
		 */
		void		addShapedPacket (Packet * pPacket);
	
		/** Pop the frontmost shaped packet for dispatch to the next module
		 * in the input chain after the RIPPS packet tap.
		 * @returns A pointer to a valid Packet object if any still exists in the
		 *  queue 
		 */		
		Packet *	popShapedPacket ();
		
	protected:

		/** Increment the number of packets successfully monitored during this
		 * interval for the host.  There should be at most one thread modifying
		 * the monitor count.
		 */		
		void		addMonPkt 	();
	
		/** Retrieve a pointer to the mutex associated with all information access
		 * to this particular host
		 */
		pthread_mutex_t *	getMutexHost ();
			
	private:
		/** Value in seconds that an assessment of this host has from a
		 * successful assessment before it expires
		 */
		unsigned int		m_nAssessDecayTime;
	
		// IP Address (allow support for v6)
		char	m_byIP[MAX_IP_LENGTH];
		int		m_nIPLength;

		// MAC Address
		char	m_byMAC[MAX_MAC_LENGTH];
		int		m_nMACLength;
		
		// Connection Medium
		int		m_nConnMedium;

		// Mutex to protect this specific host.  The mutex is primarily to protect the connection
		//  list which in turns contains expected acknowledgements
		pthread_mutex_t 			m_MutexHost;

		
		/** The time at which the last shaped packet for this host is scheduled to
		 * be dispatched
		 */
		struct timeval		m_latestShapeTime;
		
		/** The time between successive shapings to be employed for this particular
		 * host in terms of microseconds.
		 */
		unsigned int		m_nShapeUS;
		
				
		// Validation info
		//	Time of last validation engagement
		//	Validation status (engage RIPPS, yea/nay)

		struct timeval	m_LastValidation;			// Last validation
		char	m_nValidateState;
		
		// Monitored Connection Info
		List_RIPPSConnections	m_listConnections;
		
		// Current monitor set for a host
		unsigned int		m_nMonitorSet;
		
		// Last tally for monitored packets
		unsigned int		m_nMonitorPkts;
		
		// A list of packets to dispatch for the host that allows ack clocking
		// if relevant
		unsigned int			m_nSliceState;
		unsigned int			m_nSliceCount;
		
		// Data packet number - destined for this host
		int						m_nDataPktNum;	
		
		// System time for relative dispatching
		//   Saving this as a variable allows this to carry between multiple calls
		//   and use the notion of the packet number to determine when to gather
		//   system time again
		struct timeval			m_pollSysTime;
		int						m_nPollDataPktNum;					
		
		/** Outbound packet queue for external shaping that will be
		 * triggered via the timer for dispatching packets that are
		 * shaped.
		 */
		PktQueueFIFO			m_PktQueue;
		
		RIPPS_HostStats		m_Stats;			
};

/// List of hosts being monitored by the RIPPS system kept in various orderings to
/// expedite searching
class List_RIPPSHosts {
	public:
		List_RIPPSHosts ();
		~List_RIPPSHosts ();

		/** Add a host to the list of currently observed hosts
		 * Note that the host is not checked for uniqueness before it is
		 * pushed onto the list as that is assumed to be the responsibility
		 * of the callee.
		 * @param pHost A pointer to a valid RIPPS_Host object
		 */		
		void		addHost (RIPPS_Host * pHost);
		
		/// @brief Attempt to find a host by IP
		/// Search the list of hosts to find an IP address.  For now, this is just a
		/// simple linear search without optimization. If necessary, optimizations may
		/// be added to accelerate performance.
		/// @returns NULL if host was not found, otherwise a pointer to a valid Host object
		RIPPS_Host *		findHost_byIP	(char * pIP, int nLength=DEFAULT_IP_LEN);
		
		/// @brief Attempt to find a host by MAC
		/// Search the list of hosts to find an MAC address.  For now, this is just a
		/// simple linear search without optimization. If necessary, optimizations may
		/// be added to accelerate performance.
		/// @returns NULL if host was not found, otherwise a pointer to a valid Host object		
		RIPPS_Host *		findHost_byMAC	(char * pMAC, int nLength=DEFAULT_MAC_LEN);
	
		void		dumpBrief ();
		
		/// Dump the host list with a brief summary of open matches
		void		dumpBrief_withOpen ();
		
		void 		forceValidationState (char nState);
	
		/** Given a particular time for the system, iterate through the entirety of
		 * the host list and check to see if the host should have its state returned 
		 * to a validation state. Time is only passed in once to avoid excessive system
		 * calls for the overall time.  
		 * 
		 * As this operation does touch each and every host on the list, it should not
		 * be called excessively if possible. Moreover, it is important to note that
		 * hosts must successively be resolved (medium determination) in order to not
		 * have their data and ACK packets monitored.  
		 * 
		 * @param pCurTime  A pointer to a valid timeval struct that contains the current system time
		 */
		void		checkReassessments (struct timeval * pCurTime);

		/** Get a specific host in the list
		 * @param  nHost The number of the host to retrieve
		 * @returns A valid pointer if the host requested is within the list range,
		 *   otherwise a NULL pointer
		 */
		RIPPS_Host *   getHost (int nHost);

		// Get the size of the host list
		int      getSize ();

	private:
		// The actual allocated hosts.  For now, we will just do linear searches and 
		//  abstract functionality to allow for performance improvements later
		vector<RIPPS_Host *>	m_List;
	

};



#endif

