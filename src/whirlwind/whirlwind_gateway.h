/***************************************************************************************************
 * whirlwind_gateway.h
 ***************************************************************************************************
 * The gateway is the bridge that sits in-line between the WAN (Wide-Area network, aka the Internet)
 * and the downstream clients. It is the foundation of the Whirlwind architecture.
 *
 * PI: Dr. Aaron Striegel    striegel@nd.edu
 * Student: Xueheng Hu       xhu2@nd.edu
 **********************************************************************************************
 */
#ifndef WHIRLWIND_GATEWAY_H_
#define WHIRLWIND_GATEWAY_H_

#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <unordered_map>

using namespace std;

#include "../mem/MemoryPool.h"
#include "../pkt/Packet.h"
#include "../pkt/MultiIFModule.h"
#include "../stat/Stats.h"
#include "../core/Console.h"
#include "../core/Timestamp.h"

#include "whirlwind_mastercache.h"

// Code adapted from
// http://www.netrino.com/Embedded-Systems/How-To/Big-Endian-Little-Endian

#define endianfixs(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
		       (((uint16_t)(A) & 0x00ff) << 8))

#define endianfixl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
    (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
    (((uint32_t)(A) & 0x0000ff00) << 8)  | \
		       (((uint32_t)(A) & 0x000000ff) << 24))


/** Traffic can come in one of two sides, either from the wireless clients or from
 *  the upstream Internet side
 */
enum eWhirlwind_GatewayTaps {
    WW_TAP_NONE = 0,
    WW_TAP_WAN,                  // Traffic from the Internet side
    WW_TAP_WIRELESS              // Traffic from the wireless side
};

/** 
 * Enumerated values for the different types of stats that the
 * whilrwind gateway will keep track of
 */
enum eWhirlwind_GatewayStats {
    WW_GATEWAY_WAN_PKTS_ALL,             // Raw number of packets received from the WAN-side
    WW_GATEWAY_WAN_VOL_ALL,              // Raw bandwidth total received from the WAN-side
    WW_GATEWAY_WAN_PKTS_DOWN,            // Raw number of packets received from the downlink
    WW_GATEWAY_WAN_VOL_DOWN,             // Raw bandwidth total received from the downlink
    WW_GATEWAY_WAN_PKTS_UP,              // Raw
    WW_GATEWAY_WAN_VOL_UP,               // Raw
    WW_GATEWAY_WAN_PKTS_TCP,             // Packet count - TCP (WAN)
    WW_GATEWAY_WAN_VOL_TCP,              // Volume of TCP packets
    WW_GATEWAY_WAN_PKTS_UDP,             // Packet count - UDP (WAN)
    WW_GATEWAY_WAN_VOL_UDP,              // Volume of UDP packets
    WW_GATEWAY_WAN_PKTS_IPV4,            // Packet count - IPv4
    WW_GATEWAY_WAN_VOL_IPV4,             // Volume of IPv4 packets
    WW_GATEWAY_WAN_PKTS_IPV6,            // Packet count - IPv6
    WW_GATEWAY_WAN_VOL_IPV6,             // Volume of IPv6 packets
    WW_GATEWAY_WAN_PKTS_ENCV4,           // Encrypted packets (IPsec - v4)
    WW_GATEWAY_WAN_VOL_ENCV4,            // Encrypted packets (IPsec - v4) - Volume
    WW_GATEWAY_WAN_PKTS_ENCV6,           // Encrypted packets (IPsec - v6)
    WW_GATEWAY_WAN_VOL_ENCV6,            // Encrypted packets (IPsec - v6) - Volume
    WW_GATEWAY_WAN_PKTS_OTHER,           // Non-IP packets - Count
    WW_GATEWAY_WAN_VOL_OTHER,            // Non-IP packets - Volume
    WW_GATEWAY_WAN_PKTS_GRE,             // Packet count - GRE (WAN)
    WW_GATEWAY_WAN_VOL_GRE,              // Volume of GRE packets

	WW_GATEWAY_WAN_PKTS_TOOBIG,
	WW_GATEWAY_WAN_VOL_TOOBIG,

    WW_GATEWAY_WAN_PKTS_TOOSMALL,        // Count of packets that are too small
    WW_GATEWAY_WAN_VOL_TOOSMALL,         // Volume of packets that area too small

	WW_GATEWAY_WAN_VOL_DOWN_HTTP,             //
	WW_GATEWAY_WAN_VOL_DOWN_HTTPS,            //
	WW_GATEWAY_WAN_VOL_DOWN_OTHER,

	WW_GATEWAY_WAN_CACHE_HIT_PKTS,
	WW_GATEWAY_WAN_CACHE_HIT_VOL,

    WW_GATEWAY_WAN_CACHE_HIT_PKTS_HTTP,  // Hit from TCP packet
    WW_GATEWAY_WAN_CACHE_HIT_PKTS_HTTPS, // Hit from UDP packet
    WW_GATEWAY_WAN_CACHE_HIT_VOL_HTTP,   // Hit from TCP packet (volume)
    WW_GATEWAY_WAN_CACHE_HIT_VOL_HTTPS,  // Hit from UDP packet (volume)
	WW_GATEWAY_WAN_CACHE_HIT_VOL_OTHER,

    WW_GATEWAY_STAT_LAST
};

/** Statistics for the Whirlwind gateway
 */
class Whirlwind_GatewayStats : public Stats {
    public:
        Whirlwind_GatewayStats ();

	    /** 
	     * Retrieve the title for a given field
	     * @param nStat     The index to the stat field
	     * @param szTitle   The char field to populate with the title
	     */
	    virtual void Get_Title (int nStat, char* szTitle);

};

#define WW_GATEWAY_DEFAULT_MIN_PKTSIZE_FOR_CACHE       256
#define WW_GATEWAY_TIMER_ID_LOG_WRITE                  1
#define WW_GATEWAY_TIMER_INTERVAL_LOG_WRITE            60000000
#define WW_GATEWAY_STATS_DUMP_INTERVAL                 600 // in seconds

enum eWhirlwind_CacheResults {
    WW_CACHE_RESULT_UNKNOWN = 0,      // Probably bad if we have this value
    WW_CACHE_RESULT_NOCHANGE,         // Packet was unchanged, just pass it on, we are not caching
                                      // it at all due to it likely being HTTPS, IPsec, or something
    WW_CACHE_RESULT_HIT,              // Hit and it was tokenized
};

/** 
 * The Whirlwind Gateway is a multi-interface ScaleBox module that bridges two NICs in order to allow
 * it to sit in-line with traffic to downstream wireless nodes.
 */
class Whirlwind_Gateway : public MultiIFModule {
    public:
        Whirlwind_Gateway ();        
		~Whirlwind_Gateway ();

        /**
         * Initialize an input module by applying any necessary
         * startup functionality such as threads, thread safety, or
         * other initializations.
         * @returns   True if successful, false otherwise
         */
        char                    initialize ();

		/** 
		 * Given the name of a tap module, map that module name to a specific
		 * ID that will be used whenever packets are seen on that particular
		 * tap. This function must be overridden to encapsulate the behavior
		 * how the names map to IDs.
		 * @param sName The name of the module
		 * @returns The ID to use for that module where -1 is a failure in name resolution
		 */
		virtual int             mapNameToID (string sName);
		
        /**
         * Parse the DOM to populate the file where the DOM
         * is generated from an XML input file. The DOM should contain
         * the type of adapter and the underlying input and output
         * modules. The extractExtended function takes in the
         * extended information specific to the child class.
         * @param     pNode The base node to process for information
         * @returns   True if successful, false otherwise
         */
        virtual bool            extractExtendedDOM (NodeDOM* pNode);

        /** 
		 * Process a timer event from the associated TimerAggregator
         * object contained within this class if timers are enabled
         * for this particular module
         * @param pEvent The timer event that has occurred
         * @returns True if successfully processed, false if there was an error
         */
        virtual bool            processTimer (TimerEvent* pEvent);

        /** 
		 * Process a command from the console or external control
         * mechanism
         * @param theCommands The vector of commands to process
         * @param nOffset The offset at which to begin processing. 
		 *                The offset allows us to pass the vector
         *                itself with modifications to the underlying modules
         * @returns Enumerated value representing result of command processing
         */
        virtual CommandResult   processExtCommand (const vector<string>& theCommands, int nOffset);

	    /** 
	     * Process a packet from one of the underlying tap modules and
	     * appropriate process the packet to the same behavior as a processPacket
	     * call from a normal IOModule object
	     */
	    virtual char            processTapPacket (Packet* pPacket, int nID);

	    /** 
	     * Process a packet that came in on the WAN side
	     */
	    char                    processPacket_WAN (Packet* pPacket);

        /** 
	     * Process a packet that came in on the Wireless side
         */
	    char                    processPacket_Wireless (Packet* pPacket);

	    /**
         * @param pInPacket      The input packet to consider for caching
         * @param nPayloadOffset The offset into the actual data payload (from the start of the packet) in bytes
	     * @param nResult        
         */
        int                     evalMasterCache (Packet* pInPacket, int nPayloadOffset);

		int                     evalMasterCache_80211 (Packet* pPacket, int nPayloadOffset);

	    /** 
         * The minimum packet size that will be considered by the gateway for cache which typically tends to
	     * be the L2, L3, L4 header info and one core block. It can be smaller but that can
	     * be problematic.
	     *
	     * @returns The minimum packet size for consideration
	     */
	    int                     getMinPktSizeForCache ();

	    /** 
	     * The minimum packet size that will be considered by the gateway cache which typically tends to
	     * be the L2, L3, L4 header info and one core block. It can be smaller but that can
	     * be problematic. Note that this is the whole packet size being specified, not just the
	     * payload size.
	     *
	     * @param nMinSize The minimum size that will be considered for caching (in bytes)
	     */
	    void                    setMinPktSizeForCache (int nMinSize);

	    struct timeval*         computeNextLogWrite (char byTimeMode);

	    void                    dumpStats ();

    protected:
		void                    loadHostInfo (string sTraceName);

		void                    dumpStats_HostRE ();

		void                    logStatsToFile ();

		void                    updateStats_Prior ();

		void                    updateStats_Diff ();

        /** 
	     * Set the file path on which to look for the files with the
         * appropriate pattern
         * @param sPath The path starting point to look for files
         */
        void                    setPcapPath (string sPath);

        /** 
	     * Retrieve the file path for which to look for the files with
         * the appropriate pattern
         */
        string                  getPcapPath ();

        /** 
	     * Add an individual file for processing. Note that this
         * function is not thread-safe and is ideally only called once during the
         * startup of the code.  The addFile will act as a relative path to the
         * path specified via setFilePath.
         * Note that time does not carry between files.
         * @param sFile The name of the file (including path if necessary)
         * @returns True if the file was successfully located, false otherwise
         */
        bool                    addFile (string sFile);

        /** 
	     * Retrieve the number of files noted by this adapter for processing
         * @returns The number of files in the list
         */
        int                     getFileCount ();

        /** 
	     * Retrieve a specific file name from the list of files for playback
         * @param nFile The index into the file name list to retrieve
         * @returns The name of the file at the requested position if present,
         *  otherwise an empty string
         */
        string                  getFile (int nFile);

        /** 
	     * Process the pcap file in the list requested and parse the initial header
         * @param  nOffset   From where (in terms of # pkts) to start file processing
         * @param  nPackets  How far we will go (# of pkts to read)
         * information
         */
        bool                    processPcapFile (string sTraceName, int nOffset = 1, int nPackets = INT_MAX);

        /** 
	     * Read and process the next packet in the pcap
         */
        void                    doNextPcapPacket (bool skipFlag, unsigned int nMediumType);

    private:
	    Whirlwind_MasterCache*    m_pMasterCache;
        
	    int                       m_nMinPktSizeForCache;

		int                       m_nSigWindow;

	    // The real stats
	    Whirlwind_GatewayStats    m_Stats;    

	    // A placeholder for differentials
        Whirlwind_GatewayStats    m_PriorStats;

        // The prior stats for comparison
        Whirlwind_GatewayStats    m_DiffStats;

	    ofstream                  m_LogFile;

	    struct timeval            m_nextLogWrite;

        // Currently open file object
        ifstream                  m_CurrentFile;                              // Stream into the currently open file

        bool                      m_bFlipEndian;                              // Endian-ness of file is different than current arch

		string                    m_sCurrentTrace;                            // Name of the current pcap trace being processed

	    string                    m_sPcapPath;

		string                    m_sDictPath;

		int                       m_nCurrentIndex;

	    vector<string>            m_FileNames;

	    long                      m_lastStatsDump;

	    int                       m_nCurPktIndex;

        // Flag to denote if we should align the time of the files as we read them to stay contiguous,
        // i.e. file zero gets left as is, files 1 .. N start at the time from when the last set of
        // data was read.
        //
        // XML Configuration:
        //              <aligntimes>No</aligntimes>
        //              <!-- Yes or No -->

        // This is basically a two-stage process for every packet in the file
        //              Remove the time bias / start of the file
        //              Add in based on our accumulated time
    	bool                      m_bAdjustTime;

	    Timestamp                 m_Time_CurrentFileAdjust;       // How much do we adjust the file currently open?

        Timestamp                 m_Time_AccumulatedAdjust;       // Total accumulated time, adjust at the end of a file
	                                                           // We remove that from every entry and add accum time
	    Timestamp                 m_Time_LastRelativeTime;        // 

		unordered_map<string, string>    m_IPtoHostMap;

		unordered_map<string, double>    m_Host_RE;
	    // The list of UEs that this gateway is managing and / or knows about
	    // Whirlwind_ListUEs	  m_ListUEs;
};

#endif /* WHIRLWIND_GATEWAY_H_ */
