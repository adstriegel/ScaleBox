/* SourceCacheModule.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */


#ifndef SOURCECACHEMODULE_H_
#define SOURCECACHEMODULE_H_

#include "../stat/Stats.h"
#include "../pkt/MultiIFModule.h"

#include "PktCacheTable.h"

enum	SourceCachePktTaps {
	SOURCEPKTCACHE_TAP_NONE=0,
	SOURCEPKTCACHE_TAP_SRC,		// Traffic coming from the source 
	SOURCEPKTCACHE_TAP_CHILD	// Traffic from the child cache side
};

/** Enumerated values for the different types of stats that the source cache
 * module will keep track of
 */
enum ePktCache_SrcModStats {
	PKTCACHE_SRCMOD_STAT_NUM_PKTS=0,
	PKTCACHE_SRCMOD_STAT_NUM_PKTS_SRC,			// Packets on the source side
	PKTCACHE_SRCMOD_STAT_NUM_PKTS_CHILD,		// Packets on the child tap side
	PKTCACHE_SRCMOD_STAT_NUM_PKTS_NONIP,		// Number of non-IP packets
	PKTCACHE_SRCMOD_STAT_SIZE_PKTS_NONIP,		// Cumulative size of non-IP packets
	PKTCACHE_SRCMOD_STAT_NUM_PKTS_IPV6,			// Number of v6 packets
	PKTCACHE_SRCMOD_STAT_SIZE_PKTS_IPV6,		// Cumulative size of v6 packets	
	PKTCACHE_SRCMOD_STAT_NUM_PKTS_NONUDPTCP,	// Number of v4 - not UDP or TCP
	PKTCACHE_SRCMOD_STAT_SIZE_PKTS_NONUDPTCP,	// Cumulative size of v4 - not UDP or TCP
	PKTCACHE_SRCMOD_STAT_NUM_PKTS_TOOSMALL,		// Number of packets that are too small
	PKTCACHE_SRCMOD_STAT_SIZE_PKTS_TOOSMALL,	// Cumulative size packets that are too small		
	PKTCACHE_SRCMOD_STAT_LAST
};


/** Statistics for the Source Packet Cache module item
 */
class PktCacheSrcModStats : public Stats {
	public:
		PktCacheSrcModStats ();
		
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);			
};

/** The source cache is the upstream cache responsible for tokenizing
 * content that has seen a hit at the module itself.  The module has
 * multiple taps into the network adapters, one close to the source
 * content (data servers) and one on the WAN / Internet side to allow
 * for the module to process reponse messages from downstream child
 * caches (cache misses, discovery responses).   
 */
class SourceCacheModule : public MultiIFModule {
	public:
		SourceCacheModule ();
		~SourceCacheModule ();
	

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

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */					
		bool extractExtendedDOM (NodeDOM * pNode);

		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */		
		char initialize ();
		
		/** Attempt to conserve the packet by checking the cache for
		 * a previous existence of the packet (or parts therein). 
		 * @param pPacket The packet to attempt to conserve
		 * @returns True if the packet was conserved, owned by us and should not be
		 *    forwarded or false, the packet should continue onwards without
		 *    modification in the chain
		 */
		bool attemptConservePacket (Packet * pPacket);
		
		int		attemptWholePacketCache (Packet * pPacket, int nOffset);
		
		
		int		attemptPartialPacketCache (Packet * pPacket, int nOffset);

		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);		
		
		/** Set the threshold in bytes for the minimum packet size
		 * that will be considered by the packet cache.  This refers
		 * to the data payload and effectively ignores the L2 / L3
		 * headers.  
		 * @param nMinSize Minimum data payload size (in bytes) to examine
		 * @returns True if successful, false otherwise
		 */
		bool	setValue_MinPayloadSize (int nMinSize);
		
		/** Get the threshold in bytes for the minimum data payload
		 * size to consider
		 * @returns The minimum data payload size being considered
		 */
		int		getValue_MinPayloadSize ();
		
		/** Set the appropriate mode for the fingerprint mode with
		 * respect to window-based versus whole-packet fingerprinting
		 * @param Mode to employ
		 * @returns True if successful, false otherwise
		 */  
		bool	setValue_FingerprintMode (int nMode);
		
		/** Retrieve the mode for fingerprinting that we will be using
		 * which determines either a window or whole payload based
		 * approach
		 * @returns The mode for the fingerprint 
		 */
		int		getValue_FingeprintMode ();

		/** Set the size of the window if partial packet caching is
		 * allowed. This setting is meaningless if whole payload
		 * caching is used
		 * @param The size of the window (in bytes)
		 * @returns True if valid, false otherwise
		 */
		bool	setValue_FingerprintWindow (int nWindowSize);
		
		/** Retrieve the size of the window to be used for the
		 * fingerprinting 
		 * @returns The size of the window (in bytes)
		 */
		int		getValue_FingerprintWindow ();
		
		/** Set the type of the hash / fingerprint to use both
		 * for extracting the fingerprint and then to store in the
		 * hash table
		 * @param nType The type of fingerprint to use
		 * @returns True if valid, false otherwise
		 */
		bool	setValue_FingerprintType (int nType);
		
		/** Retrieve the type of fingerprint to use for extraction
		 * of the hash / fingerprint.
		 * @returns The type of fingerprint to use
		 */
		int		getValue_FingerprintType ();

		/** Set the base offset to use for all packets with respect
		 * to where to start the fingerprint extraction
		 * @param nOffset The new offset in bytes
		 * @returns True if valid, false otherwise
		 */
		bool	setValue_FingerprintOffset (int nOffset);
		
		/** Retrieve the offset to use for all packets when
		 * starting the fingerprint computation
		 * @returns The offset to use for starting fingerprint extraction (in bytes)
		 */
		int		getValue_FingerprintOffset ();		
		
		
	private:
		PktCacheHashMgr			m_HashMgr;	
		
		
		PktCacheSrcModStats		m_Stats;
	
		int						m_nMinPayloadSize;
		
		int		m_nFingerprintMode;
		int		m_nFingerprintWindow;
		int		m_nFingerprintType;
		int		m_nFingerprintOffset;
};

#define PKTCACHE_DEFAULT_MINPAYLOAD_SIZE		64

#define PKTCACHE_FINGERPRINT_MODE_WHOLE			1
#define	PKTCACHE_FINGERPRINT_MODE_PARTIAL		2

#define PKTCACHE_FINGERPRINT_TYPE_MD5			1


#define PKTCACHE_DEFAULT_FINGERPRINT_MODE		PKTCACHE_FINGERPRINT_MODE_WHOLE
#define PKTCACHE_DEFAULT_FINGERPRINT_TYPE		PKTCACHE_FINGERPRINT_TYPE_MD5
#define PKTCACHE_DEFAULT_FINGERPRINT_WINDOW		32
#define PKTCACHE_DEFAULT_FINGERPRINT_OFFSET		0

#endif /*SOURCECACHEMODULE_H_*/

