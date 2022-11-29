/* whirlwind_mastercache.cc
**********************************************************
* Implementation of the whirlwind_mastercache class
*
* PI:       Dr. Aaron Striegel    striegel@nd.edu
*
* Students: Xueheng Hu            xhu2@nd.edu
*
************************************************************
*
************************************************************
*/

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

#include "../mem/MemoryPool.h"
#include "../util/ip-utils.h"
#include "../util/SpookyV2.h"
#include "whirlwind_mastercache.h"

Whirlwind_MasterCacheEntry::Whirlwind_MasterCacheEntry () {
    m_nHits = 0;
	m_nHitLenTally = 0;
	m_nHitGapTally = 0;
    m_nSignature = 0;
    m_nOffset = 0;
	memset(&m_TimeCreation, 0, sizeof(timeval));
	memset(&m_TimeLastHit, 0, sizeof(timeval));
	m_pStoreEntry = NULL;
}

Whirlwind_MasterCacheEntry::~Whirlwind_MasterCacheEntry () {

}

bool Whirlwind_MasterCacheEntry::checkSanity () {
    return true;
}

void Whirlwind_MasterCacheEntry::dumpInfo_Short () {
    if (checkSanity()) {
        printf("  Value: %ld    Hits: %d    Offset: %d    LU: ", m_nSignature, m_nHits, m_nOffset);
        displayTime(&m_TimeLastHit);
    } else {
        cerr << "Problem with sanity check!" << endl;
    }
}

void Whirlwind_MasterCacheEntry::dumpInfo () {
    if (checkSanity()) {
	    // Detailed debug dump for troubleshooting
	    printf("Debug dump for Whirlwind_MasterCacheEntry object at %p\n", this);

	    printf("    Offset     : %d\n", m_nOffset);
	    printf("    Hits       : %d\n", m_nHits);
	    printf("    Signature  : %ld\n", m_nSignature);

	    // Show the time info
	    printf("    Created    : ");
	    displayTime(&m_TimeCreation);
	    printf("\n");

	    printf("    Last Hit   : ");
	    displayTime(&m_TimeLastHit);
	    printf("\n");
    } else {
	    cerr << "Problem with sanity check!" << endl;
    }
}

int Whirlwind_MasterCacheEntry::getOffset () {
    return m_nOffset;
}

void Whirlwind_MasterCacheEntry::setOffset (int nBytes) {
    if (nBytes >= 0) {
        m_nOffset = nBytes;
    } else {
        cerr << "Error: Bad value for the offset" << endl;
    }
}

uint64_t Whirlwind_MasterCacheEntry::getSignature () {
    return m_nSignature;
}

void Whirlwind_MasterCacheEntry::setSignature (uint64_t nValue) {
    m_nSignature = nValue;
}

struct timeval* Whirlwind_MasterCacheEntry::getTimeCreation () {
    return &m_TimeCreation;
}

void Whirlwind_MasterCacheEntry::setTimeCreation (struct timeval* pVal) {
    if (pVal != NULL) {
        m_TimeCreation = *pVal;
    }
}

struct timeval* Whirlwind_MasterCacheEntry::getTimeLastHit () {
    return &m_TimeLastHit;
}

void Whirlwind_MasterCacheEntry::setTimeLastHit (struct timeval* pVal) {
    if (pVal != NULL) {
        m_TimeLastHit = *pVal;
    }
}

int Whirlwind_MasterCacheEntry::getAvgHitLen () {
	unsigned int avgLen = 0;
	if (m_nHits > 0)
		avgLen = m_nHitLenTally / m_nHits;

	return avgLen;
}

void Whirlwind_MasterCacheEntry::registerHitLength (unsigned int nLen) {
	m_nHitLenTally += nLen;
}

unsigned int Whirlwind_MasterCacheEntry::getAvgHitGap () {
	unsigned int avgGap = 0;
	if (m_nHits > 1) {
		avgGap = m_nHitGapTally / (m_nHits - 1);
	}

	return avgGap;
}

void Whirlwind_MasterCacheEntry::registerHitGap (unsigned int nGap) {
	m_nHitGapTally += nGap;
}

int Whirlwind_MasterCacheEntry::getHits () {
    return m_nHits;
}

void Whirlwind_MasterCacheEntry::registerHit () {
    m_nHits++;
}

Whirlwind_PacketStoreEntry* Whirlwind_MasterCacheEntry::getPktStoreEntry () {
	return m_pStoreEntry;
}

void Whirlwind_MasterCacheEntry::setPktStoreEntry (Whirlwind_PacketStoreEntry* pStoreEntry) {
	m_pStoreEntry = pStoreEntry;
}

int Whirlwind_MasterCacheEntry::extendMatchRight (Packet* pInPacket, int nStartHit, int nWindowSize) {
    char* pDataComp;
	char* pRefData;
    int nPossibleComp;
    int nRefPossibleComp;
	int nOffsetPastWindow;

	nPossibleComp = 0;
	nRefPossibleComp = 0;

    // We know that we had a hit equivalent to the window size. Start from there and keep going
    pDataComp = pInPacket->getData() + nStartHit + nWindowSize;

    // How many possible bytes remain?
    nPossibleComp = pInPacket->getLength() - nStartHit - nWindowSize;

    // Get the overall packet reference that blongs to this entry
    pRefData = resolveDataReference();
    pRefData += nWindowSize;

	if (m_pStoreEntry != NULL) {
        nRefPossibleComp = m_pStoreEntry->getPacket()->getLength() - getOffset();
		nRefPossibleComp -= nWindowSize;
	}

    if (nRefPossibleComp < nPossibleComp) {
	    nPossibleComp = nRefPossibleComp;
    }

    nOffsetPastWindow = 0;
    while (nPossibleComp > 0) {
	    if (pDataComp[nOffsetPastWindow] == pRefData[nOffsetPastWindow]) {
	        nOffsetPastWindow++;
	    }
	    nPossibleComp--;
    }

    return nOffsetPastWindow;
}

char* Whirlwind_MasterCacheEntry::resolveDataReference () {
	Packet* pRefPacket = m_pStoreEntry->getPacket();

    if(pRefPacket == NULL) {
        return NULL;
    } else {
        return (pRefPacket->getData() + getOffset());
    }
}

//////////////////////////////////////////////////////////////////////////
Whirlwind_CacheTableStats::Whirlwind_CacheTableStats () : Stats () {
    Allocate(WW_MASTERCACHE_STAT_LAST);
}

void Whirlwind_CacheTableStats::GetValueString (int nStat, char* szValue) {
    double fValue;
    fValue = 0;
}

void Whirlwind_CacheTableStats::Get_Title (int nStats, char* szTitle) {
    return;
}

//////////////////////////////////////////////////////////////////////////
Whirlwind_MasterCache::Whirlwind_MasterCache () {
    m_nMaxCacheEntries = WW_MASTERCACHE_DEFAULT_MAX_ENTRIES;
	m_nMaxCacheChains = WW_MASTERCACHE_DEFAULT_MAX_CHAINS;

	for (int i = 0; i < m_nMaxCacheChains; i++) {
		m_CacheChainList[i].clear();
	}

    m_nEntries = 0;
    m_nWindowSize = WW_MASTERCACHE_DEFAULT_HASH_WINDOW;
    m_nMinGapCommit = WW_MASTERCACHE_DEFAULT_MINGAP_COMMIT;
	m_pPacketStore = new Whirlwind_PacketStore ();
	m_EntryHitCnt = vector<int>();
	m_EntryHitLen = vector<int>();
	m_EntryHitGap = vector<double>();
} 

Whirlwind_MasterCache::~Whirlwind_MasterCache () {

	if (m_pPacketStore != NULL)
    	delete m_pPacketStore;
	m_pPacketStore = NULL;
}

char Whirlwind_MasterCache::initialize () {
    // Adjust the maximum size of the mem pool
    // g_MemPool.changeMaxPoolSize(MEMPOOL_OBJ_PACKET, WW_MASTERCACHE_DEFAULT_MAX_SIZE); 
    // Creat the mutex
    pthread_mutex_init(&m_MutexCache, NULL);
    return 1;
}

int Whirlwind_MasterCache::getMaxCacheEntries () {
    return m_nMaxCacheEntries;
}

void Whirlwind_MasterCache::setMaxCacheEntries (int nSize) {
    int i;
    if (nSize > 0) {
        if(nSize > m_nMaxCacheEntries) {
            // TODO: Need to add in resizing code

        } else {
            // Ruh roh, no shrinky
            cerr << "Error: Cannot shrink TWiCE Cache Table" << endl;
            cerr << "       Ignoring request to shrink from " << m_nMaxCacheEntries << " to " << nSize << endl;
            return;
        }
    }
}

int Whirlwind_MasterCache::getMaxCacheChains () {
	return m_nMaxCacheChains;
}

void Whirlwind_MasterCache::setMaxCacheChains (int nLen) {

}

int Whirlwind_MasterCache::getNumEntries () {
	return m_nEntries;
}

int Whirlwind_MasterCache::getWindowSize () {
    return m_nWindowSize;
}

void Whirlwind_MasterCache::setWindowSize (int nSize) {
    if(nSize > 0) {
        m_nWindowSize = nSize;
    }
}

struct timeval* Whirlwind_MasterCache::getCurrentTime () {
	return &m_TimeCurrent;
}

void Whirlwind_MasterCache::setCurrentTime (struct timeval* pVal) {
	if (pVal != NULL) 
		m_TimeCurrent = *pVal;
}

char Whirlwind_MasterCache::evaluateCache (Packet* pBasePacket, int nOffset, int* pnHits, int* pnHitLen,
							               int nMaxSearch, int nCacheModRule) {
    if (pBasePacket == NULL) {
        cerr << "Error: Base packet for cache evaluation is NULL" << endl;
        cerr << "       Ignoring attempt to find a cache hit" << endl;
        return -1;
    }

    // Compute to make sure we have enough bytes to extract a signature
    int nTotalSize;
    nTotalSize = pBasePacket->getLength();
    nTotalSize -= nOffset;

    if(nTotalSize < getWindowSize()) {
        cerr << "Warning: Not enough content in the packet to do a full checksum / signature extraction" << endl;
        cerr << "         The packet has " << nTotalSize << " bytes remaining (Offset of " << nOffset << " bytes)" << endl;
        return -1;
    }

    // Loop until the right edge of our window reaches the edge of where we should be scanning. 
    // If the window size is the same as the block size, that means we do one lookup
    int nBytesRemaining;       // # of bytes remaining for sweep
    int nDistanceOffset;       // # of bytes sweeped so far from the original payload offset
    int nLastSaveOffset;       // Position for last commit
    
    // Seed the initial bytes remaining counter that will count down as we lookup / shift
    if (nMaxSearch == 0) {
        // We are going until the end
        nBytesRemaining = nTotalSize - getWindowSize();
    } else {
        // Nope, we are doing this in a block-wise manner methinks
        nBytesRemaining = nMaxSearch - getWindowSize();
    }

    if (nBytesRemaining < 0) {
        cerr << "Warning: Not enough bytes present in the search window and the block limitation" << endl;
        return -1;
    }

    // We start at the nOffset + 0
    nDistanceOffset = 0;

    // We can't simply set this to 0 before the very first commit
    nLastSaveOffset = 0 - getMinGapToCommit();

    // Now it's time to create an packet store entry and return the store entry index
	Whirlwind_PacketStoreEntry* pNewStoreEntry = m_pPacketStore->createStoreEntry(pBasePacket);
	if (pNewStoreEntry == NULL ) {
		cerr << "Error: Unable to create a packet store entry" << endl;
		return -1;
	}

    // Repeat this until we find a hit or reach the end of the search window
    // Work with pBasePacket from here on out for the purposes of cache saving / comparison
    pthread_mutex_lock(getMutex());

    while (nBytesRemaining >= 0) {
        // Our order of events
        //  1. Compute the checksum / signature
        //  2. See if we have a hit
        //       If yes, see if it is a real hit or a collision
        //               If yes, we be done, time to let someone else find the right edge
        //       If no, should we add it to the cache?

        // Can use any collision-resistant algorithm, need to be fairly efficient though.
	    uint64_t nResultSig = 0;
        int nIndex = 0;

		nResultSig = computeSigSpooky(pBasePacket->getData() + nOffset + nDistanceOffset, getWindowSize());
        nIndex = nResultSig % m_nMaxCacheChains;

		Whirlwind_CacheEntryChain::iterator entryRef;
		entryRef = m_CacheChainList[nIndex].find(nResultSig);

        // Did we resolve to an entry in the table? We pretty much always do as the current modulus
        // function in place ensures that we are always between 0 and the N-1 entry in the cache
        // entry array.
		Whirlwind_MasterCacheEntry* pCurCacheEntry;
		Whirlwind_PacketStoreEntry* pCurStoreEntry;

        if (entryRef != m_CacheChainList[nIndex].end() && (pCurCacheEntry = entryRef->second.first) != NULL) {
            // There is already a valid entry with matching signature there
			m_Stats.Add_Stat(WW_MASTERCACHE_CKSUM_MATCH, 1);

            pCurStoreEntry = pCurCacheEntry->getPktStoreEntry();

            // Compare the window we constructed the hash over in the Input packet to our entry in the
            // table where its hash was constructed. If memcmp tells us a difference of zero, we win!
		    char* pDataRef;
            pDataRef = pCurCacheEntry->resolveDataReference();

            if(pDataRef == NULL) {
                cerr << "Error: the cache entry points to a bogus pool" << endl;
                cerr << "       Dumping table entry information: " << endl;
                pCurCacheEntry->dumpInfo_Short();
            } else {
                // Given that the fingerprint it practically collision free, we can save the memcmp here. 
                // Check to see if the match is from the same packet. We can't register a hit for a intra-packet basis for now
                if (pNewStoreEntry == pCurStoreEntry) {
		            if (nOffset + nDistanceOffset == pCurCacheEntry->getOffset()) {
			            // Now it's called again on the same exact block!
						cout << "    We have a hit on the same exact block!" << endl;
			            pCurCacheEntry->dumpInfo();
			            nDistanceOffset++;
			            nBytesRemaining--;
			        } else {
                        // Intra-packet hit 
			            // Jump the whole window over as it is pointless otherwise
			            nDistanceOffset += getWindowSize();
			            nBytesRemaining -= getWindowSize();
			        }
			        continue;

		        } else {   
			        // We have a real inter-packet hit

                    // Update the use history list (move this one to the back)
					Whirlwind_CacheAccessHistory::iterator useRef = entryRef->second.second;
					m_CacheUseHistory.splice(m_CacheUseHistory.end(), m_CacheUseHistory, useRef);

                    // Change hit stats
					if (pCurCacheEntry->getHits() > 0) {
						struct timeval* tLastHit = pCurCacheEntry->getTimeLastHit();
						struct timeval* tCurrHit = pBasePacket->getArrivalTime();

						if (tCurrHit->tv_sec < 0 || tLastHit->tv_sec < 0) {
							cerr << "****    Why we're getting nagative timestamp?" << endl;
						}
                        // Compute the time passed since last hit
						double nHitGap;
						nHitGap = (double)(tCurrHit->tv_sec - tLastHit->tv_sec) * 1000;
						nHitGap += (double)(tCurrHit->tv_usec - tLastHit->tv_usec) / 1000;
						
                        if (nHitGap > 0.0) {
						    logHitGap(nHitGap);
						}   
						//pCurCacheEntry->registerHitGap((unsigned int)nHitGap);
					}

			        pCurCacheEntry->registerHit();
			        pCurCacheEntry->setTimeLastHit(pBasePacket->getArrivalTime());
                        
                    int nExtra = pCurCacheEntry->extendMatchRight(pBasePacket, nOffset + nDistanceOffset, getWindowSize());

                    nDistanceOffset += (getWindowSize() + nExtra);
					nBytesRemaining -= (getWindowSize() + nExtra);
						
					logHitLength(getWindowSize() + nExtra);

                    // pnHitLen and pnHits are stats for the incomming packet
                    *pnHitLen += (getWindowSize() + nExtra);
					*pnHits++;

					continue;
		        }
		    }
	    } else {
            // Cache miss or the matched entry is null, consider for commit
            m_Stats.Add_Stat(WW_MASTERCACHE_MISS_TOTAL, 1);
            m_Stats.Add_Stat(WW_MASTERCACHE_MISS_EMPTYSLOT, 1);

	        int nCommitRet = -1;
	        if (nDistanceOffset - nLastSaveOffset >= getMinGapToCommit()) {
	            nCommitRet = commitCacheEntry(nResultSig, nOffset + nDistanceOffset, pNewStoreEntry);
	        }

	        if (nCommitRet > 0) {
		        nLastSaveOffset = nDistanceOffset;
	        }
        }

        nDistanceOffset++;
        nBytesRemaining--;
    }

    pthread_mutex_unlock(getMutex());
    return 0;
}

int Whirlwind_MasterCache::commitCacheEntry (uint64_t nValue, int nOffset, 
                                             Whirlwind_PacketStoreEntry* pStoreRef) {
	Packet* pPacket = pStoreRef->getPacket();
	if (pPacket == NULL) {
		cerr << "    ** Error - Commit: Refer to a NULL packet in the packet store" << endl;
		return 0;
	}    

	int nChainIndex  = nValue % m_nMaxCacheChains;

    // 1. Check if the master cache reaches max size
	//    if yes, kick out the oldest entry (via fingerprint)
    //    otherwise, continue to populate the entry at proper location 
	if (m_nEntries >= WW_MASTERCACHE_DEFAULT_MAX_ENTRIES) {
		// locate the oldest entry by its fingerprint in the histroy list
		uint64_t nOldestFP;
		nOldestFP = m_CacheUseHistory.front();
        // evict the entry from the master cache
		evictCacheEntry(nOldestFP);
	}

    // 2. Generate and set the new entry
    Whirlwind_MasterCacheEntry* pNewEntry = new Whirlwind_MasterCacheEntry();
    if (pNewEntry == NULL) {
        cerr << "*** Error - Commit: Unable to allocate a new block of memory for commit." << endl;
	    return 0;
    }

    m_Stats.Add_Stat(WW_MASTERCACHE_CONSIDERCACHE_COMMIT, 1);

    pNewEntry->setSignature(nValue);
    pNewEntry->setOffset(nOffset);
    pNewEntry->setTimeCreation(pPacket->getArrivalTime());
    pNewEntry->setTimeLastHit(pPacket->getArrivalTime());
	pNewEntry->setPktStoreEntry(pStoreRef);

	unsigned int nStoreIndex = pStoreRef->getStoreIndex();
	m_pPacketStore->incEntryRefs(nStoreIndex);

    // 3. Add it to the use history and the master cache
	m_CacheUseHistory.push_back(nValue);
	Whirlwind_CacheAccessHistory::iterator useRef = m_CacheUseHistory.end();
	useRef--;

	m_CacheChainList[nChainIndex].insert(std::make_pair(nValue, std::make_pair(pNewEntry, useRef)));

    m_nEntries++;

    return 1;
}

int Whirlwind_MasterCache::evictCacheEntry (uint64_t nSignature) {
    // Locate the cache entry in the proper chain
    int nChainIdx = nSignature % WW_MASTERCACHE_DEFAULT_MAX_CHAINS;
	Whirlwind_MasterCacheEntry* pCacheEntry;
	pCacheEntry = m_CacheChainList[nChainIdx].at(nSignature).first;

	if (pCacheEntry == NULL) {
		cerr << "    Cannot evict a NULL cache entry." << endl;
		return 0;
	}

    // Rip out the appropriate stats
    m_Stats.Add_Stat(WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_TOTAL, 1);

    if (pCacheEntry->getHits() == 0) {
	    m_Stats.Add_Stat(WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_NOHITS, 1);
    } else {
	    m_Stats.Add_Stat(WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_HASHITS, 1);
	    m_Stats.Add_Stat(WW_MASTERCACHE_MAINT_REMOVED_ENTRIES_HITS, pCacheEntry->getHits());
    }

    // To-do How old the current entry is in secs

	Whirlwind_PacketStoreEntry* pStoreEntry = pCacheEntry->getPktStoreEntry();

	if (pStoreEntry != NULL) {
		unsigned int nStoreIndex = pStoreEntry->getStoreIndex();
	    m_pPacketStore->decEntryRefs(nStoreIndex);
	    pCacheEntry->setPktStoreEntry(NULL);	
	}

    // time to log the entry stats before deleting
	int nHits = pCacheEntry->getHits();
	int nHitLen = pCacheEntry->getAvgHitLen();
	unsigned int nHitGap = pCacheEntry->getAvgHitGap();

	if (nHits > 0) {
		logHitCount(nHits);
	}

    // Clean up
    delete pCacheEntry;
	m_CacheChainList[nChainIdx].erase(nSignature);
	m_CacheUseHistory.pop_front();

    m_nEntries--;

    return 1;
}

uint64_t Whirlwind_MasterCache::computeSignature (Packet* pPacket, int nOffset, int nType, int nWindow) {
    switch (nType) {
	    case WW_SIG_TYPE_XOR:
		    return computeSigXOR(pPacket->getData() + nOffset, nWindow);

	    case WW_SIG_TYPE_JENKINS:
		    return computeSigSpooky(pPacket->getData() + nOffset, nWindow);

	    case WW_SIG_TYPE_RABIN:
		    return computeSigRabin(pPacket->getData() + nOffset, nWindow);

	    default:
		    cerr << "    Unknown Sig Type: " << nType << endl;
		    return -1;
    }
}

uint32_t Whirlwind_MasterCache::computeSigXOR (char* pData, int nWindow) {
	unsigned int* pDataPtr;
    unsigned int nResult;

    pDataPtr = (unsigned int*)(pData);
	nResult = 0;

    for (int i; i < nWindow; i += sizeof(int)) {
        nResult = nResult ^ *(pDataPtr);
        pDataPtr++;
    }

    return nResult;
}

uint64_t Whirlwind_MasterCache::computeSigRabin (char* pData, int nWindow) {
	return 0;
}

uint64_t Whirlwind_MasterCache::computeSigSpooky (char* pData, int nWindow) {
    unsigned int* pDataPtr;
    unsigned int nResult;

    return SpookyHash::Hash64(pData, nWindow, 0);
}

int Whirlwind_MasterCache::getMinGapToCommit () {
    return m_nMinGapCommit;
}

void Whirlwind_MasterCache::setMinGapToCommit (int nVal) {
    m_nMinGapCommit = nVal;
}

void Whirlwind_MasterCache::dumpCacheStats (string sTraceName) {
    // Wrap up the stats from in-mem cache entries;
	for (int i = 0; i < m_nMaxCacheChains; i++) {
		for (auto it = m_CacheChainList[i].begin(); it != m_CacheChainList[i].end(); ++it) {
			Whirlwind_MasterCacheEntry* pEntry = it->second.first;
			if (pEntry != NULL) {
				int nHitCnt = pEntry->getHits();
				if (nHitCnt > 0)
					logHitCount(nHitCnt);
			}
		}
	}

	ofstream oHFile;
	string oHName = "./HitCnt-" + sTraceName + ".csv";

	oHFile.open(oHName.c_str());
	for (int i = 0; i < m_EntryHitCnt.size(); i++) {
		oHFile << m_EntryHitCnt[i] << endl;
	}
	oHFile.close();

	ofstream oLFile;
	string oLName = "./HitLen-" + sTraceName + ".csv";

	oLFile.open(oLName.c_str());
	for (int i = 0; i < m_EntryHitLen.size(); i++) {
		oLFile << m_EntryHitLen[i] << endl;
	}
	oLFile.close();

	ofstream oGFile;
	string oGName = "./HitGap-" + sTraceName + ".csv";

    oGFile.open(oGName.c_str());
    for (int i = 0; i < m_EntryHitGap.size(); i++) {
        oGFile << m_EntryHitGap[i] << endl;
    }
    oGFile.close();
}

void Whirlwind_MasterCache::logHitCount (int nVal) {
	m_EntryHitCnt.push_back(nVal);
}

void Whirlwind_MasterCache::logHitLength (int nVal) {
    m_EntryHitLen.push_back(nVal);
}

void Whirlwind_MasterCache::logHitGap (double nVal) {
    m_EntryHitGap.push_back(nVal);
}

pthread_mutex_t* Whirlwind_MasterCache::getMutex () {
    return &m_MutexCache;
}

Whirlwind_CacheTableStats* Whirlwind_MasterCache::getStats () {
    return &m_Stats;
}

Whirlwind_CacheTableStats* Whirlwind_MasterCache::getStats_Diff () {
    return &m_DiffStats;
}

Whirlwind_CacheTableStats* Whirlwind_MasterCache::getStats_Prior () {
    return &m_PriorStats;
}

void Whirlwind_MasterCache::saveStats_Prior() {
    m_PriorStats.syncAll(&m_Stats);
}

void Whirlwind_MasterCache::updateStats_Diff() {
    m_DiffStats.computeDiff(&m_PriorStats, &m_Stats);
}
