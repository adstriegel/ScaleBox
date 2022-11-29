/*
 * whirlwind_mastercache.h
 *
 * The Whirlwind Master Cache table is an optimized table that acts as a superset for all cache data
 * that is maintained by the Whirlwind Gateway.
 *
 * Conceptually, one can think of the Master Cache table as a tracker that knows all of the data cached
 * at various downstream UEs. The Master Cache effectively contains only signatures and references to
 * which clients contain the various pieces of information.
 *
 */

#ifndef WHIRLWIND_MASTERCACHE_H_
#define WHIRLWIND_MASTERCACHE_H_

#include <stdint.h>

#include <list>
#include <vector>
#include <unordered_map>

#include "../pkt/Packet.h"
#include "../stat/Stats.h"
#include "whirlwind_packetstore.h"

using namespace std;

/** An entry in the master cache entry table
 */
class Whirlwind_MasterCacheEntry {
    public:
        Whirlwind_MasterCacheEntry ();
	    ~Whirlwind_MasterCacheEntry ();

	    /** 
	     * A sanity check that can be executed against an entry that will try to navigate all pointers
	     * and entries for the purpose of validation
	     *
	     * @return True if correct, false if errors are present
	     */
	    bool               checkSanity ();

	    /** 
	     * Dump a short summary of the entry to stdout
	     * @param bCheckSanity Add a sanity check to the mix (pre-printout)
	     */
	    void               dumpInfo_Short ();

	    /** 
	     * Dump a long form of the entry to stdout
	     * @param bCheckSanity Add a sanity check to the mix (pre-printout)
	     */
	    void               dumpInfo ();

	    /**
	     * Retrieve the offset into the packet pool on which the signature for
	     * this cache entry is based
	     * @return The signature for the basis of the cache entry
	     */
	    int                getOffset ();

	    // Change the offset for this cache entry from the into the referenced packet.
	    // The packet offset is in bytes from the begining of that packet
	    void               setOffset (int nBytes);

	    // Get the signature
	    uint64_t           getSignature ();

	    // Set the signature
	    void               setSignature (uint64_t nValue);

	    // Get the creation time
        struct timeval*    getTimeCreation ();

	    // Set the creation time
        void               setTimeCreation (struct timeval* pVal);

	    // Get the last hit time
	    struct timeval*    getTimeLastHit ();

	    // Set the last hit time
        void               setTimeLastHit (struct timeval* pVal);

	    struct timeval*    getTimeExpiry ();

	    void               setTimeExpiry (struct timeval* pVal);

	    bool               isExpired (struct timeval* pCurrentTime);

	    int                getAvgHitLen ();

		void               registerHitLength (unsigned int nLen);

		unsigned int       getAvgHitGap ();

		void               registerHitGap (unsigned int nGap);

	    int                getHits ();

	    void               registerHit ();

	    int                extendMatchRight (Packet* pInPacket, int nStartHit, int nWindowSize);

	    char*              resolveDataReference ();

		// Get the packet store reference pointed by this cache entry 
        Whirlwind_PacketStoreEntry* getPktStoreEntry ();

        // Set the packet store entry that this cache entry is referencing
        void                        setPktStoreEntry (Whirlwind_PacketStoreEntry* pStoreEntry);

    private:
	    // When was this entry created?
	    struct timeval	   m_TimeCreation;

	    // When was the last hit?
        struct timeval	   m_TimeLastHit;

	    // How many hits have we seen on this entry?
        int                m_nHits;

		unsigned int       m_nHitLenTally;

		unsigned int       m_nHitGapTally;

	    // The actual hash value as computed by some signature machenism
	    // Need to eventually modularize it, for now we are doing a XOR
        uint64_t           m_nSignature;

        // What offset does our hash map to in the original packet
	    int                m_nOffset;

        // Referenced packet store entry (where the ref packet located)
		Whirlwind_PacketStoreEntry*       m_pStoreEntry;

};

// Max number of cache entries allowed
#define WW_MASTERCACHE_DEFAULT_MAX_ENTRIES         12000000
// Max length, i.e., number of chains, with multiple entries per chain
#define WW_MASTERCACHE_DEFAULT_MAX_CHAINS          1102729
#define WW_MASTERCACHE_DEFAULT_HASH_WINDOW         64
#define WW_MASTERCACHE_DEFAULT_MINGAP_COMMIT       16
#define WW_MASTERCACHE_DEFAULT_EXPIRY              20000000    //  milli-sec
#define WW_MASTERCACHE_DEFAULT_EXPIRY_ADJUST_HIT   20000000    //  milli-sec

/** Statistics for the Whirlwind master cache
 */
class Whirlwind_CacheTableStats : public Stats {
    public:
        Whirlwind_CacheTableStats ();

	    /** Retrieve the title for a given field
	     * @param nStat    The index to the stat field
	     * @param szTitle  The char field to populate with the title
	     */
	    virtual void    Get_Title (int nStat, char* szTitle);

	    /// @brief Get the value for this field as a string
	    /// Retrieve the value of this particular field as a string. This can be particularly
	    ///  useful for derived fields that are floating point results derived from the
	    ///  core results
	    virtual void    GetValueString (int nStat, char* szValue);
};

enum eWhirlwind_MasterCacheStats {
	WW_MASTERCACHE_CKSUM_MATCH,

	// Stats regarding hits and misses
	WW_MASTERCACHE_HIT_TOTAL,
	WW_MASTERCACHE_HIT_INTER,
	WW_MASTERCACHE_HIT_INTRA,

	WW_MASTERCACHE_MISS_TOTAL,
	WW_MASTERCACHE_MISS_EMPTYSLOT,
	WW_MASTERCACHE_MISS_COLLISION_CKSUM,
	WW_MASTERCACHE_MISS_COLLISION_INDEX,

	// Stats regarding considering caching a block
	WW_MASTERCACHE_CONSIDERCACHE_TOTAL,
	WW_MASTERCACHE_CONSIDERCACHE_EMPTY,
	WW_MASTERCACHE_CONSIDERCACHE_HASHITS,
	WW_MASTERCACHE_CONSIDERCACHE_HASHITS_NOTEXPIRED,
	WW_MASTERCACHE_CONSIDERCACHE_NOHITS,
	WW_MASTERCACHE_CONSIDERCACHE_NOHITS_NOTEXPIRED,
	WW_MASTERCACHE_CONSIDERCACHE_COMMIT,

	// Stats regarding cache eviction
	WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_TOTAL,
	WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_NOHITS,
	WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_HASHITS,

	WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_HITS,
	WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_AGE,

	WW_MASTERCACHE_STAT_LAST
};

enum eWhirlwind_SigatureType {
    WW_SIG_TYPE_XOR = 0,
    WW_SIG_TYPE_JENKINS,
    WW_SIG_TYPE_RABIN,
};

typedef std::unordered_map<uint64_t, 
	                       pair<Whirlwind_MasterCacheEntry*, list<uint64_t>::iterator>> Whirlwind_CacheEntryChain;
typedef std::list<uint64_t> Whirlwind_CacheAccessHistory;


class Whirlwind_MasterCache {
    public:
	    Whirlwind_MasterCache ();
	    ~Whirlwind_MasterCache ();

	    char        initialize ();

	    /** 
	     * Conduct a sanity check on all of the information in the object and the various
	     * children in this master cache
	     * @returns True if all is well, false if there is an error
	     */
	    bool        checkSanity ();

	    void        dumpInfo_Short (bool bCheckSanity);

	    void        dumpInfo (bool bCheckSanity);

	    int         getMaxCacheEntries ();

	    void        setMaxCacheEntries (int nSize); 

		int         getMaxCacheChains ();

		void        setMaxCacheChains (int nSize);

		int         getNumEntries ();

	    int         getWindowSize ();

	    void        setWindowSize (int nSize);

		Whirlwind_PacketStore*      getPacketStore ();

		void                        setPacketStore (Whirlwind_PacketStore* pStore);

		struct timeval*             getCurrentTime ();

		void                        setCurrentTime (struct timeval* pVal);

        /** 
	     * Check the cache table on this particular packet starting
         * at the specified offset.
         *
         * @param pInPacket     The packet that is being analyzed. This should not be modded in here,
         *                      only the location of a cache hit noted
         * @param nOffset       The location (in bytes) on which to start searching
         * @param pnHits        A pointer to a valid int that will contain the # of hits
         *                      from the InPacket (if any)
         * @param nMaxSearch    The maximum length to search for a hit from the offset
         *                      which can be useful for blocking.  <= 0 means to search
         *                      until the end of the packet.  This is in bytes for the
         *                      search (ex 128 bytes).
         * @param nCacheModRule What are the rules governing modification of the cache while
         *                      processing this particular packet?  Default is zero which
         *                      encourages aggressive caching behavior.
         * @returns  If there is a hit, a pointer to a valid Whirlwind_MasterCacheEntry object,
         *           otherwise NULL
         */
        char                        evaluateCache (Packet* pInPacket, int nOffset, int* pnHits, int* pnHitLen,
						                           int nMaxSearch, int nCacheModRule);

        /** 
	     * Consider this packet for inclusion into the master cache 
         * @param nIndex            The entry chain index where the signature / fingerprint lines up to
         * @param nSignature        The value of the checksum (modularize this later)
         * @param nOffset           The offset at where this checksum was computed (absolute on packet)
         * @param pStoreReft        The reference to the packet store
	     *
         * @returns                 1 if entry was committed, 0 otherwise
	     */
	    int                         commitCacheEntry (uint64_t nSignature, int nOffset, 
                                                      Whirlwind_PacketStoreEntry* pStoreRef);

	    int                         evictCacheEntry (uint64_t nSignature);

        /**                                                                  
         * Compute 64-bit long signature based on the packet, offset, and given type
        */
        uint64_t                    computeSignature (Packet* pPacke, int nOffset, int nType, int nWindow);

		uint32_t                    computeSigXOR (char* pData, int nWindow);

        uint64_t                    computeSigRabin (char* pData, int nWindow);

		uint64_t                    computeSigSpooky (char* pData, int nWindow);

        // Retrieve the minimum gap between successive commits to the cache by a single packet
        int                         getMinGapToCommit ();

        void                        setMinGapToCommit (int nGap);

	    void                        dumpCacheStats (string sTraceName);

		void                        logHitCount (int nVal);

		void                        logHitLength (int nVal);

		void                        logHitGap (double nVal);

	    Whirlwind_CacheTableStats*  getStats ();

        Whirlwind_CacheTableStats*  getStats_Diff();

        Whirlwind_CacheTableStats*  getStats_Prior();

        void                        saveStats_Prior();

        void                        updateStats_Diff();

		pthread_mutex_t*            getMutex ();

    private:
        // Basically a two-level index, 1-index is the modula, 2-index is the fingerprint
		Whirlwind_CacheEntryChain           m_CacheChainList[WW_MASTERCACHE_DEFAULT_MAX_CHAINS];

        // Keep track of all the in-cache entries as an array-based FIFO queue
	    Whirlwind_CacheAccessHistory        m_CacheUseHistory;

	    // What is the size of the master cache?  This should not be resized while running
	    // live as a matter of course but during initialization, it may need to be blitzed.
	    int                                 m_nMaxCacheEntries;

		int                                 m_nMaxCacheChains;

	    // Tally for the number of valid entries
        int                                 m_nEntries;

	    // Configure information regarding the signature / checksum mechnism
	    int                                 m_nWindowSize;

	    int                                 m_nMinGapCommit;

		Whirlwind_PacketStore*              m_pPacketStore;

		struct timeval                      m_TimeCurrent;

	    // Stats for the master cache table
	    Whirlwind_CacheTableStats           m_Stats;
	    Whirlwind_CacheTableStats           m_DiffStats;
	    Whirlwind_CacheTableStats           m_PriorStats;

	    // Mutex for cache entries
	    pthread_mutex_t                     m_MutexCache; 

		vector<int>                         m_EntryHitCnt;
		vector<int>                         m_EntryHitLen;
		vector<double>                      m_EntryHitGap;
};

#endif /* WHIRLWIND_MASTERCACHE_H_ */
