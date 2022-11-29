/* RIPPS_Monitor.h
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
 * $Revision: 1.12 $  $Date: 2008/04/11 22:52:01 $
 **********************************************************
 */
 
#ifndef __RIPPS_MONITOR_H
#define __RIPPS_MONITOR_H

#include <vector>
using namespace std;

#include <string>
using namespace std;

#include "../mem/MemoryPool.h"
#include "../pkt/MultiIFModule.h"
#include "../stat/Stats.h"
#include "../pkt/Packet.h"

#include "RIPPS_Host.h"
#include "../core/Console.h"


#define	DEFAULT_SLICE_TARGET		60
#define	DEFAULT_SLICE_DELAY		100			// Default delay of 100 microseconds

/** The default number of packets to measure before a host is considered validated */
#define DEFAULT_MEASURE_TARGET		20

#define	DEFAULT_MONITOR_INTERVAL	300			// Default monitoring interval of once per minute
#define DEFAULT_IDLE_SUMMARY		10000		// Default of 3 s of no packets before a connection can
														// be summarized
#define	DEFAULT_PUSH_GAP			3000		// Default push gap to use in microseconds
#define	DEFAULT_SLICE_GAP			1000		// Default gap between slices



#define	DEFAULT_THRESHOLD_WIRED		3.0	// 3.0 ms default definite wired
#define	DEFAULT_THRESHOLD_WIRELSS	6.0	// 6.0 ms default definite wireless

enum eStingSortMode {
	STINGSORT_SIMPLE=0,				// Simple is swap first to last ordering 
	STINGSORT_PSHEND=1,				// PSH end maintains the last packet always at the end
	STINGSORT_CHECKERBOARD=2 			// Checkerboard swaps elements to get progressive acks along the way
	};
	

enum eRIPPSMonitorTimer {
	RIPPS_MONITOR_TIMER_SHAPE_QUEUE=1,		// Outbound shape queue for a given host
	RIPPS_MONITOR_TIMER_POST_SHAPE,			// Timer to purge the shape queue for a given host
	RIPPS_MONITOR_TIMER_CONN_CLEAN,			// Cleaner timer set after the FIN/RST was observed on the WAN side to allow
														//  for ample time of any remaining ACKs to be processed
	RIPPS_MONITOR_TIMER_MOD_CLEANER,			// Cleaner operations at the module level
};

/// A host or subnet that is not to be monitored by the RIPPS subsystem
class FilterHost {
	public: 
		FilterHost ();
		
		/// Set the filter using a mask and value
		/// @param pMask The mask to apply
		/// @param pAddress The address to match against
		/// @param nLength The length of the filter  
		void		setFilter (char * pMask, char * pAddress, int nLength);
		
		/// Determine if this address is a match for the filter
		/// @param pTest The address to test
		/// @param nLength The length of the address to test
		/// @returns 1 if it is a match, 0 if it is not, < 0 if there is a length mismatch (IPv4 vs IPv6)
		int			isMatch (char * pTest, int nLength);
				
	private:
		char		m_byAddress[16];
		char		m_byMask[16];
		int			m_nLength;
		
		// How many times has the filter been hit?
		int			m_nHits;		
};


/** Traffic can come in one of two sides, either the monitor side (LAN-side) or the
 * external WAN / unmonitored side.  
 */
enum eRIPPS_MonitorTaps {
	RIPPS_TAP_NONE=0,
	RIPPS_TAP_WAN,			// Traffic coming from the outside to the monitor side
	RIPPS_TAP_MONITORLAN	// Traffic outbound from the monitor side
};

/** Enumerated values for the different types of stats that the RIPPS monitor
 * will keep track of
 */
enum eRIPPS_MonitorStats {
	RIPPS_MONITOR_STAT_PKTS_EXT_INCOMING=0,		// Packets seen as the "WAN" side
	RIPPS_MONITOR_STAT_PKTS_EXT_FILTER,			// Packets ignored by default from the WAN
	RIPPS_MONITOR_STAT_PKTS_EXT_VALIDATED,		// Packets to hosts already validated by RIPPS
	RIPPS_MONITOR_STAT_PKTS_INT_OUTGOING,		// Packets going from the monitor to external side
	RIPPS_MONITOR_STAT_PKTS_INT_FILTER,			// Packets ignored by default from the WAN	
	RIPPS_MONITOR_STAT_PKTS_INT_UNKHOST,		// TCP packets from unseen hosts
	RIPPS_MONITOR_STAT_PKTS_INT_ACK_UNKHOST,	// Ack from a previously unseen host
	RIPPS_MONITOR_STAT_PKTS_INT_FIN_UNKHOST,	// Connection termination from an unseen host 
	RIPPS_MONITOR_STAT_HOST_VALIDATIONS,		// Number of host validations completed
	RIPPS_MONITOR_STAT_PKTS_GOBBLE,				// Packets gobbled to prevent them from being sent on
	RIPPS_MONITOR_STAT_PKTS_GOBBLE_DATA,		// Packets that are not a pure ACK but were otherwise marked for gobbling
	RIPPS_MONITOR_STAT_PKTS_GOBBLE_YUM,			// Tasty pure ACK packets gobbled by the module
	RIPPS_MONITOR_STAT_PKTS_EXT_PUREACK,		// Pure ACKs coming from the external world (impling an internal server)
	RIPPS_MONITOR_STAT_PKTS_EXT_SMALL,			// Packets ignored for slicing due to insufficient size
	RIPPS_MONITOR_STAT_PKTS_SLICE_STING,		// Packets sliced and re-arranged per TCP Sting
	RIPPS_MONITOR_STAT_PKTS_POST_SHAPE,			// Packets appended to the shape queue after validation for order preservation
	RIPPS_MONITOR_STAT_TIMER_SHAPE_QUEUE,		// Timer invocations for shaping of the outbound packets
	RIPPS_MONITOR_STAT_TIMER_POST_SHAPE,		// Timer invocations for post queue shaping after validation
	RIPPS_MONITOR_STAT_PKTS_SLICED,				// Count of sliced packets sent out
	RIPPS_MONITOR_STAT_TIMER_CLEAN_HOST,		// Cleanup timers for individual hosts
	RIPPS_MONITOR_STAT_LAST	
};

/** Statistics for the RIPPS monitor item
 */
class RIPPS_MonitorStats : public Stats {
	public:
		RIPPS_MonitorStats ();
		
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);			
};

/** The monitor object that contains the settings of the RIPPS monitoring
 *  subsystem
 */
class RIPPS_Monitor : public MultiIFModule {
	
	public:	
		RIPPS_Monitor();		
		~RIPPS_Monitor ();
		
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
	
		/** Process packets coming in from the packet tap on the external (WAN) side
		 * that are bound for the internal (LAN) side of the network. Packets coming
		 * from the WAN side may be sliced before entering the LAN side of the 
		 * network.   
		 * @param pPacket A pointer to a valid Packet object
		 * @returns 1 if the packet was held for shaping / queuing, 0 if it should simply
		 *   carry onwards
		 */ 
		char processPacketWAN (Packet * pPacket);	

		/** Process packets that are coming from the internal (monitored) side of the
		 * RIPPS device that will be destined for the external (WAN) side of the device.
		 * Whereas packets from the external side will always be forwarded to the the
		 * internal side, packets from the internal side that have been sliced may
		 * be blocked before being forwarded to the external side and hence the
		 * return code for the function may need to reflect that.    
		 * @param pPacket A pointer to a valid Packet object
		 * @returns 1 if the packet should be gobbled, 0 if it should continue onwards
		 */ 	
		char processPacketMonitor (Packet * pPacket);	
	
		/** The heart of RIPPS itself that will slice packets and shape them as
		 * appropriate.  The slicePacket function will create the packets from the
		 * source data packet and refer to the queue for the host itself to determine
		 * how to shape outbound packets to the host. The typical behavior is that
		 * inbound packets are shaped before being dispatched by giving them to the
		 * host-wise packet queue whose content is slowly freed out by the overarching
		 * timer for the RIPPS module itself.
		 * @param pHost A pointer to a valid host object
		 * @param pPacket A pointer to a valid packet object
		 * @returns 1 if the packet is being enqueued by the slicer, 0 if the packet
		 *  is not enqueued by the slicer and should be handled by other modules
		 */
		char slicePacket (RIPPS_Host * pHost, Packet * pPacket);
			
		/** Shape the packets to a given host to ensure a minimum inter-packet
		 * delay when packets are under strict monitoring operations by RIPPS
		 * @param pHost The host whose packets are being shaped
		 * @param pPacket The packet to shape
		 * @param bPostShape Flag to denote if the packet is being added to the end of the
		 *  shaping queue simply for order preservation, not for shaping. 
		 * @returns True if successful, false otherwise 
		 */	
		bool shapePacketHost (RIPPS_Host * pHost, Packet * pPacket, bool bPostShape=false);	
			
		/// Dump stats to cout
		void		dumpStats ();
		
		void		dumpSettings ();
	
		
		/// Clear all host state
		void		clearState ();
				
		/// Add a value to a specific statistic in the monitor subsystem
		///	@param nStat The stat to modify (see Monitor header file)
		/// @param fValue The amount to modify the stat
		void		addStat (int nStat, float fValue);
		
		/// Reset all statistics to zero
		void		resetStats ();
		
		/// Add an address to filter to the list
		void		addFilter (FilterHost * pFilter);
		
		
		void 	getStatName (char * pString, int nStat);	
		
		void	dumpRawStats ();
		
		/** @brief Add the host to the list from the packet characteristics 
		 * @param pData Pointer to the IPv4 address
		 * @param pPacket The source packet data
		 * @returns A pointer to the newly allocated host object
		 */
		//RIPPSHost *  addHost (char * pIP, PacketHolder * pPacket);

		/** Add a host to the RIPPS monitoring pool. The host should be a pointer
		 * to a valid object and an address not already in the list of monitored
		 * hosts.
		 * @param pHost A pointer to a valid RIPPS_Host object
		 * @returns True if successful, false if unsuccessful
		 */ 
		bool	addHost (RIPPS_Host * pHost);

		/** @brief Determine if the packet should be excluded from monitoring from RIPPS
		 * Examine the packet should be considered for monitoring or is excluded from
		 * all monitoring activities (passive or active)
		 * @param pAddress Pointer to the address in question	
		 * @param nLen Length of address (in bytes)	 
		 * @returns True if the packet should be filtered, false if it can be monitored
		 */
		bool		filterPacket_Always (char * pAddress, int nLen);
				
		/** Is this host in the process of being monitored or should it be placed into
		 * a monitoring state?
		 * @param pHost Pointer to a valid host object		 
		 * @returns False means host is not monitored, true means host should be monitored
		 */		
		bool shouldMonitorHost (RIPPS_Host * pHost);
		

		/// Force the summarization to disk of all open connections
		void           summarizeAll ();
		
		/// Set the delay between successive packets sliced by RIPPS
		/// A value of zero forces no delay whatsoever between packets
		/// @param fDelay The new delay in microseconds
		void		setValue_SliceDelay			(unsigned int nDelay);
		
		/// Get the delay between successive packets sliced by RIPPS
		/// @returns The delay between packets in microseconds
		unsigned int		getValue_SliceDelay ();
		
		
		/// Turn RIPPS by-pass filtering on or off
		void		setOption_BypassFilter (char bFilter);
		
		char		getOption_BypassFilter ();
		
				

		
		
		
		/** Retrieve the flag regarding the slice setting. If the slice
		 * setting is true, slicing operations are enabled. Otherwise, slicing is
		 * effectively disabled and the system is effectively a passive monitoring system 
		 * as outlined in the WENS06 paper by Mano and Cieslak. The default value unless otherwise
		 * stated is for slicing to be enabled.
		 * @returns True if slicing is enabled, false otherwise
		 */
		bool		getFlag_CanSlice ();
		
		/** Set the flag regarding if the core slicing of RIPPS is enabled or
		 * disabled. With slicing enabled, packet slicing will take place, otherwise
		 * monitoring will still occur to the desired threshold, albeit without
		 * slicing (purely passive).  
		 * @param bSlice The new value for the slice setting
		 */
		void		setFlag_CanSlice (bool bSlice);
							
		/** Calculate the target slice amount
		 * @returns The number of bytes (max) to include in the slice
		 */
		int	getValue_SliceSize ();

		/** Set the value for the slice target size for packets
		 * @param nValue The new value for the slice target size
		 */
		void	setValue_SliceSize (int nValue);	
				
		/** Add an expected ACK to the list of acknowledgements for the outgoing
		 * host. We note each particular expected ACK for the purposes of measuring
		 * the RTT itself.
		 * @param pHost A pointer to a valid RIPPS_Host object
		 * @param pPacket The packet containing the expected ACK itself
		 * @returns True if successful, false otherwise
		 */
		bool		addExpectedAck (RIPPS_Host * pHost, Packet * pPacket);
		
		/** Attempt to match a received ack to the LRTT list. If the host
		 * does not exist, the ACK is ignored until we see packets on the external
		 * link coming into this module.  
		 * @param pHost A valid pointer to a RIPPS_Host object denoting the monitored
		 *  host responsible for generating the ACK
		 * @param pPacket The packet containing the ACK itself
		 * @returns True if successful, false otherwise
		 */
		bool		matchAck (RIPPS_Host * pHost, Packet * pPacket);
														
		/** Terminate a connection that is in the listing of ongoing
		 * connections
		 * @param pHost Host associated with the connection
		 * @param pPacket Packet that is inbound or outbound
		 * @param nDirection Direction where the packet was found 
		 */
		void 		terminateConnection (RIPPS_Host * pHost, Packet * pPacket, int nDirection);
		
		/** Attempt to find the host-wise entry from a particular address. The
		 * host information is the collection point for all slicing and monitoring
		 * TCP data/ACK pairs for a particular host.
		 * @param pAddress The IP address to search for in the list of hosts (assume v4 for now)
		 * @returns NULL if no match was found, non-NULL valid RIPPS_Host pointer otherwise
		 */
		RIPPS_Host *		findHost (char * pAddress, int nLength);
		
		/** Retrieve the number of packets that should be captured and noted before a host is
		 * assessed for validation in terms of its medium type. 
		 * @returns The number of packets before a host medium validation is attempted
		 */ 
		unsigned int	getValue_NumRTTsMeasure ();

		/** The number of packets that should be captured and noted before a host is
		 * assessed for validation in terms of its medium type
		 * @param nTarget The number of RTTs to capture before attempting validation
		 */ 		
		void				setValue_NumRTTsMeasure (unsigned int nTarget);
		
		/** Get the value for the monitor interval (in seconds)
		 * @returns The current value for the monitor interval
		 */
		unsigned int		getValue_MonitorInterval ();
		
		/** Set the value for the monitor interval (in seconds) between validations of
		 * the host connectivity mechanism
		 * @param nInterval The new interval (in seconds)
		 */
		void				setValue_MonitorInterval (unsigned int nInterval);
						
		void 		measureAll ();
		
		void		checkValidations ();
										
		unsigned short	getNextID ();
						
		/// @brief Retrieve a specific host from the list of hosts
		/// This function allows an external entity to walk through the list of hosts
		/// and to extract information on a host-wise basis without blocking modifications
		/// at the monitor level (host-level mutex only). Note that this function is
		/// thread-safe with regards to the monitor and its underlying host list.
		/// @param nIndex The index of the host to retrieve
		/// @returns Valid RIPPSHost object if in a valid index range, NULL otherwise 
		RIPPS_Host * getHost (int nIndex);
		
		/// @brief Retrieve the number of hosts currently noted by the monitor
		/// Note that this function is thread-safe with regards to the monitor and 
		///	 its underlying host list.		
		/// @returns The count of the number of hosts noted by the monitor
		int			getHostCount ();
		
		
		/// @brief Set the number of milliseconds by which a connection can be summarized
		void	setValue_IdleSummary (unsigned int nMS);
		
		/// @brief Retrieve the number of milliseconds by which a connection can be summarized
		unsigned int	getValue_IdleSummary ();
		
		/// Check the monitor for summarizing the active connections
		void	checkSummarizeConnections ();		
		
		
		
		void	setStingSortMode (unsigned char bStingSort);
		unsigned char	getStingSortMode ();
		
		
		void			setThresholdWireless (double fVal);
		double		getThresholdWireless ();

		void			setThresholdWired (double fVal);
		double		getThresholdWired ();

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

	protected:
		/// Start up the monitoring thread
		int			startMonitor ();
		
		
		/** Retrieve the mutex associated with the host list
		 * @returns A pointer to a valid pthread mutex handle
		 */
		pthread_mutex_t *		getMutexHostList ();
		
	private:
		/** Are the monitoring aspects of RIPPS enabled? This simply refers to the
		 * fact that RIPPS will be paying attention to packets. By default, the
		 * monitor itself is enabled.
		 */		
		bool		m_bEnableMonitor;
				
		// Settings for RIPPS
		unsigned int		m_nSliceDelay;				// Delay between packets (in ms) when sliced

		// Slice target size
		int			m_nSliceTarget;			// Average target size for slices
		
		// ID for differentiating ID field in the IP header
		unsigned short		m_nPktID;
		
		// Target amount of samples to monitor for a host to assert whether or not it is good
		// or bad
		unsigned int			m_nMeasureTarget;
		
		// Monitoring interval (in seconds) between re-validating a host
		unsigned int		m_nMonitorInterval;
				
				
		// The number of milliseconds without the reception of a packet where a connection
		//  can be considered a valid candidate for summarization				
		unsigned int		m_nIdleSummary;		
		
		// Additional shaping gap after a push packet (default is zero)
		unsigned int		m_nPushGap;
		
		// Minimum gap between different slices
		unsigned int		m_nSliceGap;
		
		// Slice duty cycle
		//  Slice duty cycle in terms of number of packets, starts in the on state
		//	 Done on a connection-wise basis
		unsigned short	m_nSliceCycleOn;
		unsigned short 	m_nSliceCycleOff;
		
		
		/** Periodic cleanup of connections for which we did not observe
		 * a FIN coming from the monitor side.
		 */
		unsigned int	m_nScanCleanupInterval;
		 
		/** Hosts to check for on the list for periodic cleanup in case we
		 * missed the FIN or RST information 
		 */
		unsigned int	m_nScanCleanupHosts; 
				
		/** The list of hosts being monitored. The mutex for this list is
		 * contained at the monitor as all host-wise operations should go
		 * through this module.
		 */
		List_RIPPSHosts					m_HostList;
		
		// Mutex to protect host list as both adapters exert influence on this list
		pthread_mutex_t 			m_MutexHostList;
		

		// Threshold for wired vs. wireless
		//		Wired < Unknown < Wireless
		double		m_fThresholdWired;
		double		m_fThresholdWireless;

		
		bool			m_bCanSlice;
		
		/////////////////////////////////////////////////////////////
		// Options for the RIPPS monitoring system
		
		unsigned char		m_nStingSort;
		
		char		m_bEnableFiltering;		// Do we allow hosts to not be monitored by RIPPS?
		
		char		m_bPreserveOrder;		// Preserve ordering of packets under monitoring
		
		char		m_bSkipReassemble;		// Skip reassembly of packets
		
		char		m_bNoSlice;
		
		char		m_bMonitorSmall;		// Watch RTT's on sub-slice sized packets
		
		char		m_bRoutePkts;			// Route or pass-through packets
		
		char		m_nPurgeTCPOpt;			// Purge TCP options
	
		char		m_bLimitShapeRelease;	// Limit how much we can release of shaped packets
		
		char		m_bNoShape;				// Do not shape the output
		
		char		m_bAppendExpAckOnly;		// Only append to the end of the exp ack list, no insertions
		char		m_bUniqueExpAckOnly;		// Only allow unique exp acks in the list, no duplicates
		
		char            m_bMonitorOutAcks;
			
		unsigned short		m_nWindowSize;	// 0 means no change, otherwise set to the size
		
		// Debugging components
		char		m_bShowWrite;			// Show writes to the adapters at the console
		
		char 		m_bReadOnly;				// Read but do not forward packets
		char		m_bShowRead_Ext;		// Show the packets read in by the ext adapter
		char		m_bShowRead_Int;		// Show the packets read in by the int adapter
		
		
	
		
		RIPPS_MonitorStats 	m_Stats;	
		
};

//extern RIPPSMonitor		g_theRIPPSMonitor;

#endif

