/*
 * TWiCE_CacheTable.h
 *
 *  Created on: Jul 6, 2011
 *      Author: striegel
 */

#ifndef TWICE_CACHETABLE_H_
#define TWICE_CACHETABLE_H_

#include "../pkt/Packet.h"
#include "TWiCE_PacketPool.h"

#include "../stat/Stats.h"

/** An individual entry in the cache table.  A cache entry represents a stored signature
 * that is the left-most hash or identifier to a contiguous block.  A hit on this entry
 * means that the left-most bytes are the same.  More bytes that are the same may follow.
 */
class TWiCECacheEntry
{
	public:
		TWiCECacheEntry ();
		~TWiCECacheEntry ();

		/** Change the offset for this cache entry from into the referenced
		 * packet from the cache pool. The packet offset is in bytes from the
		 * beginning of that packet.
		 */
		void		setOffset (int nBytes);

		/** Retrieve the offset into the packet pool on which the signature for this
		 * cache entry is based
		 * @returns The signature for the basis of the cache entry
		 */
		int			getOffset ();

		struct timeval *	getLastUsedTime ();
		void				setLastUsedTime (struct timeval * pVal);

		struct timeval * 	getCreationTime ();
		void				setCreationTime (struct timeval * pVal);

		void				addHit ();
		int					getHits ();

		void				addMiss ();
		int					getMisses ();

		/** Compare the packet versus this internal packet.  Use the offset
		 * to determine if the block indeed is a match
		 * @param pCompare The packet to compare to
		 * @param nOffset  The offset from which to start the comparison
		 */
		char		isHit (Packet * pCompare, int nOffset);

		// This is signature-dependent, needs to be modularized

		void				setValue (unsigned int nValue);
		unsigned int		getValue ();

		// Resolve the data reference via the offset information
		char *			resolveDataReference ();

		// Register a hit

		void			registerHit 	();
		void			registerMiss 	();

		/// @brief Get the pool entry for this particular cache entry
		/// Get the pool entry for this particular cache entry. Each cache entry contains a reference
		/// to the overall cached packet pool as multiple cache entries may point to the same base
		/// packet depending on how hits occur from the underlying content.  To avoid replicating each
		/// packet for each entry, a pool is kept that represents a superset of all real payload content
		/// that was cached. The pool reference is particular to the TWiCE module and should not be confused
		/// with the memory pool for ScaleBox.
		/// @returns The reference contained within this cache entry, generally speaking a NULL reference
		///           is a bad thing except for perhaps cleanup
		TWiCEPacketPoolEntry*  getPoolReference ();

		/// @brief Set the pool entry for this particular cache entry
		/// Set the pool entry for this particular cache entry. Each cache entry contains a reference
		/// to the overall cached packet pool as multiple cache entries may point to the same base
		/// packet depending on how hits occur from the underlying content.  To avoid replicating each
		/// packet for each entry, a pool is kept that represents a superset of all real payload content
		/// that was cached. The pool reference is particular to the TWiCE module and should not be confused
		/// with the memory pool for ScaleBox.
		/// @param pPoolEntry The new reference point for the TWiCE packet pool, this may be NULL
		void			setPoolReference (TWiCEPacketPoolEntry  * pPoolEntry);

		void			dumpDebug ();

		void			dumpSingleLine ();

		/// Check to see if the entry is too old
		/// @param pCurrentTime The reference point for the current time
		/// @param lTooOld      When the entry is considered too old (in seconds)
		bool 			exceedsAge (struct timeval * pCurrentTime, unsigned int lTooOld);

		void	                setExpiryTime (struct timeval * pTime);
		struct timeval*         getExpiryTime ();
		bool			isExpired  (struct timeval * pCurrentTime);


	private:
		// Our reference to the full packet in the pool
		TWiCEPacketPoolEntry*        m_pPoolEntry;

		// What offset does our hash map to in the original packet
		int			     m_nOffset;

		// Creation time
		struct timeval		     m_CreationTime;

		// Last time used
		struct timeval		     m_LastUsedTime;

		// Expirt time (when is this entry considered no longer valid and may be replaced)
		struct timeval		     m_ExpiryTime;

		// Number of hits
		int			     m_nHits;
		int			     m_nMisses;

		// The actual hash value as computed by the Rabin fingerprint
		//	Need to eventually modularize it or something

		// For now, we are doing an XOR-based entry
		unsigned int		     m_nValue;
};

#define TWICECACHETABLE_DEFAULT_MAX_SIZE				1024000
#define TWICECACHETABLE_DEFAULT_MAX_CHAIN				1
#define TWICECACHETABLE_DEFAULT_WINDOW_SIZE				64
#define TWICECACHETABLE_DEFAULT_TOO_OLD					5
#define TWICECACHETABLE_DEFAULT_MINGAP_CACHE_COMMIT		8
#define TWICECACHETABLE_DEFAULT_TOO_OLD_HIT_MULT		5

#define TWICECACHETABLE_DEFAULT_EXPIRY_HIT_ADJUST		1000000

#define TWICECACHETABLE_DEFAULT_EXPIRY_I_FRAME			10000000	// 10 s
#define TWICECACHETABLE_DEFAULT_EXPIRY_P_FRAME			1000000		// 1 s
#define TWICECACHETABLE_DEFAULT_EXPIRY_B_FRAME			0		// 1 s

#define TWICECACHETABLE_DEFAULT_RATIO_I_FRAME			2
#define TWICECACHETABLE_DEFAULT_RATIO_P_FRAME			8


/** Statistics for the TWiCE gateway
 */
class TWiCE_CacheTableStats : public Stats {
	public:
		TWiCE_CacheTableStats ();

		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);

		/// @brief Get the value for this field as a string
		/// Retrieve the value of this particular field as a string.  This can be particularly
		///  useful for derived fields that are floating point results derived from the
		///  core results
		virtual void		GetValueString (int nStat, char * szValue);
};



enum eTWiCE_CacheTableStats {

	TWICE_CACHE_CKSUM_COMPS,
	TWICE_CACHE_MISS,
	TWICE_CACHE_MISS_EMPTY_SLOT,
	TWICE_CACHE_CKSUM_MATCH_VALUE,
	TWICE_CACHE_CKSUM_CORRECT_MATCH,
	TWICE_CACHE_CKSUM_CORRECT_MATCH_INTRA,
	TWICE_CACHE_CKSUM_MISTAKE_PCT,
	TWICE_CACHE_CKSUM_MISTAKE,
	TWICE_CACHE_CKSUM_MISS_PIGEON,
	TWICE_CACHE_CONSIDERCACHE_TOOCLOSE,

	// Stats regarding considering caching a block
	TWICE_CACHE_CONSIDERCACHE,
	TWICE_CACHE_CONSIDERCACHE_EMPTY,
	TWICE_CACHE_CONSIDERCACHE_HASHITS,
	TWICE_CACHE_CONSIDERCACHE_HASHITS_OLD,
	TWICE_CACHE_CONSIDERCACHE_HASHITS_NOTOLDENOUGH,

	TWICE_CACHE_CONSIDERCACHE_NOHITS,
	TWICE_CACHE_CONSIDERCACHE_NOHITS_OLD,
	TWICE_CACHE_CONSIDERCACHE_NOHITS_NOTOLDENOUGH,
	TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE,
	TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE_OFFSET,

	// Avg values computed
	TWICE_CACHE_AVG_CONSIDERCACHE_CREATE_CACHE_OFFSET,


	// Stats when we purge entries from the cache
	TWICE_CACHE_MAINT_REMOVED_ENTRIES,
	TWICE_CACHE_MAINT_REMOVED_ENTRIES_NOHITS,
	TWICE_CACHE_MAINT_REMOVED_ENTRIES_HITS,

	TWICE_CACHE_MAINT_REMOVED_ENTRIES_HASMISSES,
	TWICE_CACHE_MAINT_REMOVED_ENTRIES_MISSES,

	TWICE_CACHE_MAINT_REMOVED_ENTRIES_AGE,
	TWICE_CACHE_MAINT_REMOVED_ENTRIES_AGE_LASTUSED,

	// Gap on the various hits (bucketized)
	TWICE_CACHE_HIT_TYPE_COUNT_TIGHT,
	TWICE_CACHE_HIT_TYPE_COUNT_SHORT,
	TWICE_CACHE_HIT_TYPE_COUNT_SHORT_MAX,
	TWICE_CACHE_HIT_TYPE_COUNT_MEDIUM_MIN,
	TWICE_CACHE_HIT_TYPE_COUNT_MEDIUM,
	TWICE_CACHE_HIT_TYPE_COUNT_LUCKY,


	TWICE_CACHETABLE_STAT_LAST
};

// Categorize the gaps between cache hits (in microseconds)

#define TWICE_CACHEHIT_TYPE_UNKNOWN 	0
#define TWICE_CACHEHIT_TYPE_TIGHT		1
#define TWICE_CACHEHIT_TYPE_SHORT		2
#define TWICE_CACHEHIT_TYPE_SHORT_MAX	3
#define TWICE_CACHEHIT_TYPE_MEDIUM_MIN	4
#define TWICE_CACHEHIT_TYPE_MEDIUM		5
#define TWICE_CACHEHIT_TYPE_LUCKY		6		// Probably should not have happened, just dumb luck

// A tight gap probably pertains to UDP-ish streaming traffic.  This is a tight bound where
//  no queuing is applied nor really necessary.  Pretty much this is OWB-central for traffic
//  as it fundamentally just looks like +1 hop
//
//   Definition: 10 ms or less between a cache hit
#define TWICE_CACHEHIT_GAP_TIGHT		10000

// The short gap where we might be able to get away with holding for OWB-like grouping iff
//  we are at the last hop on a cellular or wireless network.  This is probably pushing it and would
//  need to be tuned.  There is a good chance though that this occurs naturally by virtue of normal
//  queuing and no artificiality is necessary
//
//   Definition: 50 ms or less between a cache hit
#define TWICE_CACHEHIT_GAP_SHORT		50000

// The short max is at the bleeding edge for say a cellular network where queuing might still lead
//  us to an OWB-like type of effect.  We could never inject artificial queuing for this long but
//  it could exist naturally in the queue backlog such as with wireless.  Our original 802.11 paper
//  pushed close to this envelope
//
//   Definition: 100 ms or less between a cache hit
#define TWICE_CACHEHIT_GAP_SHORT_MAX		100000		// 100 ms between the hits

// The medium hit is where packet caching (RE) work tends to shine.  The medium min gap refers to
//  a general LTE-ish setting where the phone caches 10MB of data with a net rate of 30 Mb/s.  Taking
//  the back of the envelope calculations, that works out to roughly 2.5 seconds worth of full-sized
//  packets.  Obviously, that is a bit oversimplifying it but this gives us an indication of the lower
//  end of how tight we could set the downstream cache and still have a reasonable chance of success
//
// Note that this works out based on our experiments to roughly 4350 packets, estimate only
//
//   Definition: 2.5 seconds or less between a cache hit
#define TWICE_CACHEHIT_GAP_MEDIUM_MIN		2500000

// The medium hit refers to cases where the entry has stayed viable even for its first hit in the cache
//  table.  Basically, this is not OWB-friendly traffic but it is traffic that is seeing lots of hits.
//  This is prime wireless packet caching traffic and the most likely setting for the cache
//
//   Definition: 5 seconds or less between a cache hit
#define TWICE_CACHEHIT_GAP_MEDIUM			5000000



#define TWICE_CACHE_RANKING_DEFAULT_SIZE	10

#define TWICE_CACHE_RANKING_HITS			1

class TWiCECacheEntryRanking
{
	public:
		TWiCECacheEntryRanking();

		unsigned int getMetric ();
		void setMetric (unsigned int lMetric);

		TWiCECacheEntry * getEntry ();
		void setEntry (TWiCECacheEntry * pEntry);

	private:
		unsigned int 		m_lMetric;
		TWiCECacheEntry	*	m_pEntry;

};

/// A top-N list set up to help query what is doing what based on a particular metric
///  or optimization
class TWiCECacheTopList
{
	public:
		TWiCECacheTopList();
		~TWiCECacheTopList();

		int		getRankSize();
		void	setRankSize(int nSize);

		void	reset ();

		void	rankEntry (TWiCECacheEntry * pEntry, unsigned int lMetric);

		void	dumpList  (char * szRanking);

	private:
		int		m_nRankFilled;
		int		m_nRankSize;

		TWiCECacheEntryRanking *	m_pRankEntries;
};


/** The cache table at the gateway which drives all downstream client
 * cache tables.
 */
class TWiCECacheTable
{
	public:
		TWiCECacheTable ();
		~TWiCECacheTable ();

		void setMaxTableSize (int nMaxSize);
		int  getMaxTableSize ();

		void setMaxChainSize (int nMaxChainSize);
		int  getMaxChainSize();

		/** Attempt to find a hit in the table on this particular packet starting
		 * at the specified offset.
		 *
		 * @param pBasePacket  The packet that is being analyzed. This should not be modded in here,
		 * 						only the location of a cache hit noted
		 * @param nOffset  The location (in bytes) on which to start searching
		 * @param pnMatch  A pointer to a valid int that will contain where the match
		 *                  was made (if any)
		 * @param nMaxSearch    The maximum length to search for a hit from the offset
		 *                        which can be useful for blocking.  <= 0 means to search
		 *                        until the end of the packet.  This is in bytes for the
		 *                        search (ex 128 bytes).
		 * @param nCacheModRule What are the rules governing modification of the cache while
		 *                       processing this particular packet?  Default is zero which
		 *                       encourages aggressive caching behavior.
		 * @param pPool			The packet pool
		 * @param pBasePacket	The reference copy to place into the packet pool
		 * @returns If there is a hit, a pointer to a valid TWiCECacheEntry object,
		 *           otherwise NULL
		 */
		TWiCECacheEntry * attemptFindHit (Packet * pBasePacket, int nOffset, int * pnMatch,
										  int nMaxSearch, int nCacheModRule, TWiCEPacketPool * pPool,
										  char & bSavedInPool, TWiCEPacketPoolEntry * pPoolEntry);



		int		getWindowSize();
		void 	setWindowSize (int nSize);

		/** Compute a XOR-based checksum across the range specified starting at pData which
		 * is assumed to be offset appropriately.
		 *
		 * @param pData    The data pointer offset appropriately for where to start
		 * @param nWindow  The window size over which to compute the checksum
		 */
		unsigned int computeChecksum_XOR (char * pData, int nWindow);


		// Consider this item for inclusion in the cache and the pool

		/** Consider this packet for inclusion into the cache and overall packet
		 * pool (if not already included).
		 *
		 * @param nEntry   				The entry in the table where this checksum lines up to
		 * @param nValue   				The value of the checksum (modularize this later)
		 * @param nOffset  				The offset at where this checksum was computed (absolute on packet)
		 * @param pPacket  				The reference to the original source packet
		 * @param nDistanceStart		The distance from the start (on this particular search)
		 * @param bSavedInPool			Flag if the packet is saved in the pool (by reference)
		 * @returns 1 if entry was committed, 0 otherwise
		 */
		TWiCECacheEntry * considerCache (int nEntry, int nValue, int nOffset, Packet * pPacket);


		int	considerCacheWithMode (int nEntry, int nValue, int nOffset, Packet * pPacket,
						   	   	   int nDistanceStart, char & bSavedInPool, TWiCEPacketPoolEntry * pPoolEntry);

		// The no pool variant
		int	considerCacheWithMode_NoPool (int nEntry, int nValue, int nOffset, Packet * pPacket, int nDistanceStart);


		char dumpStatus ();

		/// @brief Retrieve the time when an entry is considered old and may be replaced
		unsigned int	getEntryTooOld ();
		void			setEntryTooOld (unsigned int lTooOld);

		/// @brief Retrieve the multiplier for old entries when they have at least one hit
		unsigned int	getEntryTooOld_HitMultiplier ();
		void			setEntryTooOld_HitMultiplier (unsigned int lTooOld);


		/// @brief Retrieve the minimum gap between successive commits to the cache by a single packet
		unsigned int	getMinGapBetweenCacheCommit ();
		void			setMinGapBetweenCacheCommit (unsigned int nGap);


		char			removeCacheEntry (int nEntry, int nSubEntry, TWiCEPacketPoolEntry * pPoolEntry);

		TWiCE_CacheTableStats * getStats();
		TWiCE_CacheTableStats * getStats_Diff();
		TWiCE_CacheTableStats * getStats_Prior();

		void saveStats_Prior();
		void updateStats_Diff();


		int		categorizeHit (TWiCECacheEntry * pEntry, struct timeval * pVal);

		char 	initialize ();


		void	dumpTopList   (int nList);

		void	updateTopLists ();


		// Get / set the packet pool (initialization only)

		TWiCEPacketPool	*	  getPacketPool ();
		void				  setPacketPool (TWiCEPacketPool * pPool);


	protected:
		pthread_mutex_t *		getMutex();

	private:
		TWiCECacheEntry **	m_ppEntries;

		// How many base entries?
		int			m_nMaxTableSize;

		// How deep could it go with regards to chains on collisions?
		//   The default is 1
		int			m_nMaxChainSize;

		// Config information regarding the signature / checksum mechanism

		int			m_nWindowSize;

		// Tally for the number of entries
		int			m_nEntries;

		// When is an entry considered too old?
		unsigned int	m_nEntry_TooOld;

		// Multiplier when the entry has at least one hit
		unsigned int	m_nEntry_TooOld_HitMultiplier;

		// What is the minimum gap between parts of a packet that may be committed to the cache?
		unsigned int	m_nMinGapCacheCommit;

		TWiCE_CacheTableStats	m_Stats;
		TWiCE_CacheTableStats	m_PriorStats;
		TWiCE_CacheTableStats	m_DiffStats;


		// Mutex for cache entries
		pthread_mutex_t 			m_MutexCache;

		TWiCECacheTopList	*		m_pRankHits;

		// Packet pool (kept in the gateway)
		TWiCEPacketPool	*	  		m_pPacketPool;
};

#endif /* TWICE_CACHETABLE_H_ */

