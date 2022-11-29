/* TWiCE_PacketPool.cc
 **********************************************************
 * TWiCE: Transparent Wireless Capacity Enhancement
 **********************************************************
 * TWiCE is an extension to the ScaleBox code that focuses
 * on improvements to wireless capacity by the clever removal
 * of redundant content.
 *
 * Inspiration for TWiCE is drawn from results observed as
 * part of the NetSense cell phone study.  TWiCE is currently
 * being modernized as Proximity Secure Mixing (PSM).
 *
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *
 * 	http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 *
 * Students: Xueheng Hu			   xhu2@nd.edu
 *
 * Prior students that contributed to the precursors to TWiCE
 * and PSM include Dave Salyers, Yingxin Jiang, and Xiaolong Li.
 *
 ************************************************************
 * Support for the development of TWiCE was drawn in part from
 * sources including the National Science Foundation, Sprint,
 * Alcatel-Lucent, and the University of Notre Dame.
 ************************************************************
 */

#include <iostream>
using namespace std;

#include "../pkt/Packet.h"

#include "../util/ip-utils.h"
#include "TWiCE_CacheTable.h"

/////////////////////////////////////////////////////////////////////

TWiCECacheEntry::TWiCECacheEntry ()
{
	m_pPoolEntry = NULL;
	m_nOffset = 0;
	m_nHits = 0;
	m_nMisses = 0;
	m_nValue = 0;
}

/////////////////////////////////////////////////////////////////////

TWiCECacheEntry::~TWiCECacheEntry ()
{


}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::dumpSingleLine ()
{
	printf("  Value: %d    Hits: %d    Misses: %d  Offset: %d  LU: ", m_nValue, m_nHits, m_nMisses, m_nOffset);
	displayTime(&m_LastUsedTime);
}

void TWiCECacheEntry::dumpDebug ()
{
	// Detailed debug dump for troubleshooting

	printf("Debug dump for TWiCECacheEntry object at %p\n", this);
	printf("   Pool Entry : %p\n", m_pPoolEntry);

	if(m_pPoolEntry != NULL)
	{
		m_pPoolEntry->dumpNestedDebug();
	}

	printf("   Offset     : %d\n", m_nOffset);
	printf("   Hits       : %d\n", m_nHits);
	printf("   Misses     : %d\n", m_nMisses);
	printf("   Value:     : %d\n", m_nValue);

	// Show the time info
	printf("   Created    : ");
	displayTime(&m_CreationTime);
	printf("\n");

	printf("   Last Used  : ");
	displayTime(&m_LastUsedTime);
	printf("\n");

}

int TWiCECacheEntry::getHits ()
{
	return m_nHits;
}

/////////////////////////////////////////////////////////////////////

struct timeval * TWiCECacheEntry::getLastUsedTime ()
{
	return &m_LastUsedTime;
}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::setLastUsedTime (struct timeval * pVal)
{
	if(pVal != NULL)
	{
		m_LastUsedTime = *pVal;
	}
}

/////////////////////////////////////////////////////////////////////

bool	TWiCECacheEntry::isExpired  (struct timeval * pCurrentTime)
{
	if(compareTime(&m_ExpiryTime,pCurrentTime))
	{
		return false;
	}
	else
	{
		return true;
	}
}


/////////////////////////////////////////////////////////////////////

void	TWiCECacheEntry::setExpiryTime (struct timeval * pTime)
{
	m_ExpiryTime = *pTime;
}

/////////////////////////////////////////////////////////////////////

struct timeval * TWiCECacheEntry::getExpiryTime ()
{
	return &m_ExpiryTime;
}

/////////////////////////////////////////////////////////////////////

struct timeval * TWiCECacheEntry::getCreationTime ()
{
	return &m_CreationTime;
}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::setCreationTime (struct timeval * pVal)
{
	if(pVal != NULL)
	{
		m_CreationTime = *pVal;
	}
}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::setOffset (int nBytes)
{
	if(nBytes >= 0)
	{
		m_nOffset = nBytes;
	}
	else
	{
		cerr << "Error: Bad value for the offset" << endl;
		return;
	}
}

/////////////////////////////////////////////////////////////////////

int TWiCECacheEntry::getOffset ()
{
	return m_nOffset;

}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::setValue (unsigned int nValue)
{
	m_nValue = nValue;
}

/////////////////////////////////////////////////////////////////////

unsigned int TWiCECacheEntry::getValue ()
{
	return m_nValue;
}

/////////////////////////////////////////////////////////////////////

TWiCEPacketPoolEntry  * TWiCECacheEntry::getPoolReference ()
{
	return m_pPoolEntry;
}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::setPoolReference (TWiCEPacketPoolEntry * pPoolEntry)
{
	m_pPoolEntry = pPoolEntry;
}

/////////////////////////////////////////////////////////////////////

bool TWiCECacheEntry::exceedsAge (struct timeval * pCurrentTime, unsigned int lTooOld)
{
	unsigned int	lTimeDiff;

	if(m_LastUsedTime.tv_sec > pCurrentTime->tv_sec)
	{
		// Hmmm, last used time is greater than curren time.  Definitely not old
		return false;
	}

	lTimeDiff = pCurrentTime->tv_sec - m_LastUsedTime.tv_sec;

	if(lTimeDiff >= lTooOld)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/////////////////////////////////////////////////////////////////////

char *	TWiCECacheEntry::resolveDataReference ()
{
	if(getPoolReference() == NULL)
	{
		return NULL;
	}
	else
	{
		return m_pPoolEntry->getPacket()->getData()+getOffset();
	}
}

int TWiCECacheEntry::getMisses ()
{
	return m_nMisses;
}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::registerHit 	()
{
	m_nHits++;
}

/////////////////////////////////////////////////////////////////////

void TWiCECacheEntry::registerMiss 	()
{
	m_nMisses++;
}

/////////////////////////////////////////////////////////////////////

TWiCECacheTable::TWiCECacheTable ()
{
	m_nMaxTableSize = TWICECACHETABLE_DEFAULT_MAX_SIZE;
	m_nMaxChainSize = TWICECACHETABLE_DEFAULT_MAX_CHAIN;

	// Allocate / initialize our pointer list for entries
	m_ppEntries = new TWiCECacheEntry * [m_nMaxTableSize];

	int 	j;

	for(j=0; j<m_nMaxTableSize; j++)
	{
		m_ppEntries[j] = NULL;
	}

	m_nEntries = 0;

	m_nWindowSize = TWICECACHETABLE_DEFAULT_WINDOW_SIZE;
	m_nEntry_TooOld = TWICECACHETABLE_DEFAULT_TOO_OLD;

	m_nEntry_TooOld_HitMultiplier = TWICECACHETABLE_DEFAULT_TOO_OLD_HIT_MULT;
	m_nMinGapCacheCommit = TWICECACHETABLE_DEFAULT_MINGAP_CACHE_COMMIT;

	m_pPacketPool = NULL;
}

TWiCECacheTable::~TWiCECacheTable ()
{


}

unsigned int TWiCECacheTable::getEntryTooOld_HitMultiplier ()
{
	return m_nEntry_TooOld_HitMultiplier;
}

void TWiCECacheTable::setEntryTooOld_HitMultiplier (unsigned int lTooOld)
{
	if(lTooOld > 0)
	{
		m_nEntry_TooOld_HitMultiplier = lTooOld;
	}
}

unsigned int TWiCECacheTable::getMinGapBetweenCacheCommit ()
{
	return m_nMinGapCacheCommit;
}

void TWiCECacheTable::setMinGapBetweenCacheCommit (unsigned int nGap)
{
	// No restrictions as this may be zero and in fact, it is typically by default
	m_nMinGapCacheCommit = nGap;
}

int TWiCECacheTable::getMaxChainSize ()
{
	return m_nMaxChainSize;
}

void TWiCECacheTable::setMaxChainSize (int nChainSize)
{
	if(nChainSize > 0)
	{
		m_nMaxChainSize = nChainSize;
	}
}

int TWiCECacheTable::getMaxTableSize ()
{
	return m_nMaxTableSize;
}

void TWiCECacheTable::setMaxTableSize (int nSize)
{
	int j;

	if(nSize > 0)
	{
		if(nSize < m_nMaxTableSize)
		{
			// TODO: Need to add in resizing code

			TWiCECacheEntry ** ppNewEntries;

			ppNewEntries = new TWiCECacheEntry * [nSize];

			if(ppNewEntries == NULL)
			{
				cerr << "Error: Failed to create a new cache table of size " << nSize << " entries" << endl;
				cerr << "  Ignoring request for TWiCE" << endl;
				return;
			}

			for(j=0; j<m_nMaxTableSize; j++)
			{
				ppNewEntries[j] = m_ppEntries[j];
			}

			for(j=m_nMaxTableSize; j<nSize; j++)
			{
				m_ppEntries[j] = NULL;
			}

			m_nMaxTableSize = nSize;
		}
		else
		{
			// Ruh roh, no shrinky
			cerr << "Error: Cannot shrink TWiCE Cache Table" << endl;
			cerr << "  Ignoring request to shrink from " << m_nMaxTableSize << " to " << nSize << endl;
			return;
		}
	}
}

TWiCE_CacheTableStats::TWiCE_CacheTableStats () : Stats ()
{
	Allocate(TWICE_CACHETABLE_STAT_LAST);
}

void TWiCE_CacheTableStats::GetValueString (int nStat, char * szValue)
{
	double fValue;

//	cout << "Virtual overridden function" << endl;
	fValue = 0;

	switch(nStat)
	{
		case TWICE_CACHE_AVG_CONSIDERCACHE_CREATE_CACHE_OFFSET:
			// Compute it
			if(Get_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE) > 0)
			{
				fValue = (double) Get_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE_OFFSET) / (double) Get_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE);
			}

			sprintf(szValue, "%f", fValue);
			break;

		default:
			Stats::GetValueString(nStat, szValue);
			break;
	}

}

void TWiCE_CacheTableStats::Get_Title	(int nStat, char * szTitle)
{
	switch(nStat)
	{
		case TWICE_CACHE_CKSUM_CORRECT_MATCH_INTRA:
			sprintf(szTitle, "IntraPktMatch");
			break;
		case TWICE_CACHE_CONSIDERCACHE_NOHITS:
			sprintf(szTitle, "ConsCacheNoHits");
			break;
		case TWICE_CACHE_CONSIDERCACHE_HASHITS:
			sprintf(szTitle, "ConsCacheHasHits");
			break;
		case TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE:
			sprintf(szTitle, "CreateCacheCount");
			break;
		case TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE_OFFSET:
			sprintf(szTitle, "CreateCacheCumOffset");
			break;
		case TWICE_CACHE_AVG_CONSIDERCACHE_CREATE_CACHE_OFFSET:
			sprintf(szTitle, "CreateCacheAvgOffset");
			break;
		case TWICE_CACHE_MAINT_REMOVED_ENTRIES:
			sprintf(szTitle, "MaintRemovedEntries");
			break;
		case TWICE_CACHE_MAINT_REMOVED_ENTRIES_NOHITS:
			sprintf(szTitle, "MaintRemovedEntriesNoHits");
			break;
		case TWICE_CACHE_MAINT_REMOVED_ENTRIES_HITS:
			sprintf(szTitle, "MaintRemovedEntriesHits");
			break;
		case TWICE_CACHE_MAINT_REMOVED_ENTRIES_MISSES:
			sprintf(szTitle, "MaintRemovedEntriesCumMisses");
			break;
		case TWICE_CACHE_CKSUM_COMPS:
			sprintf(szTitle, "ChecksumComps");
			break;
		case TWICE_CACHE_MISS:
			sprintf(szTitle, "CountMissAll");
			break;
		case TWICE_CACHE_CKSUM_MATCH_VALUE:
			sprintf(szTitle, "CkSumMatchValue");
			break;
		case TWICE_CACHE_CKSUM_CORRECT_MATCH:
			sprintf(szTitle, "CkSumMatchCorrect");
			break;
		case TWICE_CACHE_CKSUM_MISTAKE_PCT:
			sprintf(szTitle, "CkSumMistakePct");
			break;
		case TWICE_CACHE_CKSUM_MISTAKE:
			sprintf(szTitle, "CkSumMistakeCount");
			break;
		case TWICE_CACHE_CKSUM_MISS_PIGEON:
			sprintf(szTitle, "CkSumPigeon");
			break;
		case TWICE_CACHE_CONSIDERCACHE_TOOCLOSE:
			sprintf(szTitle, "SkipTooCloseCount");
			break;


		case TWICE_CACHE_HIT_TYPE_COUNT_TIGHT:
			sprintf(szTitle, "HitTypeTight");
			break;

		case TWICE_CACHE_HIT_TYPE_COUNT_SHORT:
			sprintf(szTitle, "HitTypeShort");
			break;

		case TWICE_CACHE_HIT_TYPE_COUNT_SHORT_MAX:
			sprintf(szTitle, "HitTypeShortMax");
			break;

		case TWICE_CACHE_HIT_TYPE_COUNT_MEDIUM_MIN:
			sprintf(szTitle, "HitTypeMediumMin");
			break;

		case TWICE_CACHE_HIT_TYPE_COUNT_MEDIUM:
			sprintf(szTitle, "HitTypeMedium");
			break;

		case TWICE_CACHE_HIT_TYPE_COUNT_LUCKY:
			sprintf(szTitle, "HitTypeLucky");
			break;

		default:
			sprintf(szTitle, "F%d", nStat);
			break;
	}
}


char TWiCECacheTable::initialize  ()
{
	// Create the top N lists
	m_pRankHits = new TWiCECacheTopList();

	// Creat the mutex
	pthread_mutex_init(&m_MutexCache, NULL);
	return 1;
}

pthread_mutex_t * TWiCECacheTable::getMutex ()
{
	return &m_MutexCache;
}


void TWiCECacheTable::updateTopLists ()
{
	int			j;

	// Update the internal top N lists

	// Reset the old lists
	m_pRankHits->reset();

	pthread_mutex_lock(getMutex());

	for(j=0; j<m_nMaxTableSize; j++)
	{
		if(m_ppEntries[j] != NULL)
		{
			//cout << " Attempting to rank w/hits of " << m_ppEntries[j]->getHits() << endl;
			m_pRankHits->rankEntry(m_ppEntries[j], m_ppEntries[j]->getHits());
			//cout << "  Done ranking" << endl;
		}
	}

	pthread_mutex_unlock(getMutex());
}

TWiCECacheEntry * TWiCECacheTable::attemptFindHit_NoPool (Packet * pBasePacket, int nOffset, int * pnMatch,
												   	      int nMaxSearch, int nCacheModRule)
{
	cout << "***** Attempting to find a hit *****" << endl;

	if(pBasePacket == NULL)
	{
		cerr << "Error: Base packet for attempting to find a cache hit is NULL" << endl;
		cerr << "  Ignoring attempt to find a cache hit" << endl;
		return NULL;
	}

	int		nTotalSize;

	// Compute to make sure we have enough bytes to extract a signature

	nTotalSize = pBasePacket->getLength();
	nTotalSize -= nOffset;

	if(nTotalSize < getWindowSize())
	{
		cerr << "Warning: Not enough content in the packet to do a full checksum / signature extraction" << endl;
		cerr << "  The packet has " << nTotalSize << " bytes remaining (Offset of " << nOffset << " bytes)" << endl;
		return NULL;
	}

	// Loop until the right edge of our window reaches the edge of where we should be scanning.  If the window size
	// is the same as the block size, that means we do one lookup

	int		nBytesRemaining;
	int		nDistanceOffset;
	int		nLastSave;

	// We start at the offset + 0
	nDistanceOffset = 0;
	nLastSave = 0;

	// Seed the initial bytes remaining counter that will count down as we lookup / shift

	if(nMaxSearch == 0)
	{
		// We are going until the end
		nBytesRemaining = nTotalSize - getWindowSize();
	}
	else
	{
		// Nope, we are doing this in a block-wise manner methinks
		nBytesRemaining = nMaxSearch - getWindowSize();
	}

	if(nBytesRemaining < 0)
	{
		cerr << "Warning: Not enough bytes present in the search window and the block limitation" << endl;
		return NULL;
	}

	// Repeat this until we find a hit or reach the end of the search window
	// Work with pBasePacket from here on out for the purposes of cache saving / comparison

	pthread_mutex_lock(getMutex());

	while(nBytesRemaining >= 0)
	{
		// Our order of events
		//  1. Compute the checksum
		//  2. See if we have a hit
		//       If yes, see if it is a real hit or a collision
		//			If yes, we be done, time to let someone else find the right edge
		//		 If no, should we add it to the cache?

		// Compute the checksum

		// For now, we are just doing XOR

		int nMode;
		int nTableEntry;
		unsigned int		nResultXOR;

		nMode = 0;
		nTableEntry = -1;

		nResultXOR = computeChecksum_XOR(pBasePacket->getData()+nOffset+nDistanceOffset,getWindowSize());
		m_Stats.Add_Stat(TWICE_CACHE_CKSUM_COMPS, 1);

		nTableEntry = nResultXOR % m_nMaxTableSize;

		printf("Ck: DB: %p   Offset: %d  Value: %d  Window: %d\n", pBasePacket->getData(), nOffset+nDistanceOffset, nResultXOR, getWindowSize());

		cout << "* Resolving the table entry, result post modulus is " << nTableEntry << endl;
		cout << "   Result XOR was " << nResultXOR << endl;

		// Did we resolve to an entry in the table?  We pretty much always do as the current modulus
		//  function in place ensures that we are always between 0 and the N-1 entry in the cache
		//  entry array.

		if(m_ppEntries[nTableEntry] != NULL)
		{
			cout << " Existing entry was not NULL, verifying...." << endl;

			// There is already something there

			// Are we a match for that entry?
			if(m_ppEntries[nTableEntry]->getValue() == nResultXOR)
			{
				// The indicator (XOR) is a hit, but is it really?

				// The reason we need to check is that we may end up with collisions, particularly if we
				// use a bad discriminator like an XOR.  This ensures that yes, the match is indeed really
				// a match, not just a coincidental alignment ala the Pigeonhole Principle.

				// What is the Pigeonhole Prinicple?
				//
				// If you have P pigeons to put into N holes and P > N, that means that at least one
				// hole is going to have more than 1 pigeon.  Bad, bad, bad for us as that means we goofed
				// and think we have the exact same content when we do not.

				// Compare the window we constructed the hash over in the Input packet to our entry in the
				// table where its hash was constructed.  If memcmp tells us a difference of zero, we win!

//				cout << "   Mem, mem cmp it up" << endl;

				// Let's make sure our data reference is still good
				char *	pDataRef;

				pDataRef = m_ppEntries[nTableEntry]->resolveDataReference();

				m_Stats.Add_Stat(TWICE_CACHE_CKSUM_MATCH_VALUE, 1);

				if(pDataRef == NULL)
				{
					cerr << "Error: Could not conduct a memcmp as the cache entry points to a bogus pool" << endl;
					cerr << "  packet or the pool packet has a bogus packet inside it.  Ignoring comparison" << endl;
					cerr << "  Dumping table entry information " << endl;
					m_ppEntries[nTableEntry]->dumpDebug();
				}
				else if(memcmp(pBasePacket->getData()+nOffset+nDistanceOffset, pDataRef, getWindowSize()) == 0)
				{
					m_Stats.Add_Stat(TWICE_CACHE_CKSUM_CORRECT_MATCH, 1);

					//printf("  Pkt: %X vs. Pkt: %X\n", pBasePacket, m_ppEntries[nTableEntry]->getPoolReference()->getPacket());
					//printf("    Hit was at offset of %d, window of %d\n", nOffset+nDistanceOffset, getWindowSize());
					//printf("    Table Entry: %d\n", nTableEntry);
					//printf("    Arr Time (In): ");
					//displayTime(pBasePacket->getArrivalTime());
					//printf("\n");

					//printf("    Arr Time (Cache): ");
					//displayTime(m_ppEntries[nTableEntry]->getPoolReference()->getPacket()->getArrivalTime());
					//printf("\n");

					// Yes, we have bananas today.  This is pure awesome sauce and we really, really, really
					//  like to see this number go up.

					// Categorize the hit type
					categorizeHit(m_ppEntries[nTableEntry], pBasePacket->getArrivalTime());

					m_ppEntries[nTableEntry]->registerHit();
					m_ppEntries[nTableEntry]->setLastUsedTime(pBasePacket->getArrivalTime());

					// TODO: We may need to adjust this if the entry gets hot and then fades.  There could be some potential
					//  issues if we hotspot, i.e. burst of activity then none.  We just get stuck with a dud in the cache
					//  library as we are doing things additively
					adjustTime(m_ppEntries[nTableEntry]->getExpiryTime(), TWICECACHETABLE_DEFAULT_EXPIRY_HIT_ADJUST);

					//pPoolEntry->setLastUsedTime(pBasePacket->getArrivalTime());

					if(pnMatch != NULL)
					{
						*pnMatch = nOffset + nDistanceOffset;
					}

					pthread_mutex_unlock(getMutex());
					return m_ppEntries[nTableEntry];
				}
				else
				{
					m_Stats.Add_Stat(TWICE_CACHE_MISS, 1);
					m_Stats.Add_Stat(TWICE_CACHE_CKSUM_MISTAKE, 1);

					// Ruh roh.  The pigeonhole principle gets us.  The hash looked good but our beer
					// goggles were fail and our discriminating hash function stinks.  This is kind of bad
					// as we effectively wasted that memcmp.  Here is to hoping we were on an architecture
					// where that was not too much of a hit.

					// TODO: For speed issues on multiple misses, does that mean we might probabilistically
					//       engage attempting to find a hit after multiple misses?

//					cout << "** Miss on an existing entry, offset = " << nOffset + nDistanceOffset << endl;

					// Something was wonky, register a misfire on the cache entry
					m_ppEntries[nTableEntry]->registerMiss();

					considerCacheWithMode(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket, nDistanceOffset);
				}
			}
			else
			{
				m_Stats.Add_Stat(TWICE_CACHE_MISS, 1);
				m_Stats.Add_Stat(TWICE_CACHE_CKSUM_MISS_PIGEON, 1);

				// Conflict but wrong value in the spot, discriminating function FTW!
				considerCacheWithMode(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket, nDistanceOffset);
			}
		}
		else
		{
			cout << "  Empty entry in hash table" << endl;

			// Nope, nothing there
			m_Stats.Add_Stat(TWICE_CACHE_MISS, 1);
			m_Stats.Add_Stat(TWICE_CACHE_MISS_EMPTY_SLOT, 1);

			cout << "   Considering cache" << endl;
			considerCacheWithMode(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket, nDistanceOffset, bSavedInPool, pPoolEntry);
			cout << "   Finished with consideration" << endl;
		}

		nDistanceOffset++;
		nBytesRemaining--;
	}

	pthread_mutex_unlock(getMutex());
	return NULL;



}


/////////////////////////////////////////////////////////////////////////////////////////

TWiCECacheEntry * TWiCECacheTable::attemptFindHit (Packet * pBasePacket, int nOffset, int * pnMatch,
												   int nMaxSearch, int nCacheModRule, TWiCEPacketPool * pPool,
												   char & bSavedInPool, TWiCEPacketPoolEntry * pPoolEntry)
{
	cout << "***** Attempting to find a hit *****" << endl;

	if(pBasePacket == NULL)
	{
		cerr << "Error: Base packet for attempting to find a cache hit is NULL" << endl;
		cerr << "  Ignoring attempt to find a cache hit" << endl;
		return NULL;
	}

	int		nTotalSize;

	// Compute to make sure we have enough bytes to extract a signature

	nTotalSize = pBasePacket->getLength();
	nTotalSize -= nOffset;

	if(nTotalSize < getWindowSize())
	{
		cerr << "Warning: Not enough content in the packet to do a full checksum / signature extraction" << endl;
		cerr << "  The packet has " << nTotalSize << " bytes remaining (Offset of " << nOffset << " bytes)" << endl;
		return NULL;
	}

	// Loop until the right edge of our window reaches the edge of where we should be scanning.  If the window size
	// is the same as the block size, that means we do one lookup

	int		nBytesRemaining;
	int		nDistanceOffset;
	int		nLastSave;

	// We start at the offset + 0
	nDistanceOffset = 0;
	nLastSave = 0;

	// Seed the initial bytes remaining counter that will count down as we lookup / shift

	if(nMaxSearch == 0)
	{
		// We are going until the end
		nBytesRemaining = nTotalSize - getWindowSize();
	}
	else
	{
		// Nope, we are doing this in a block-wise manner methinks
		nBytesRemaining = nMaxSearch - getWindowSize();
	}

	if(nBytesRemaining < 0)
	{
		cerr << "Warning: Not enough bytes present in the search window and the block limitation" << endl;
		return NULL;
	}

	// Repeat this until we find a hit or reach the end of the search window


	// The bSavedInPool tells us if that packet was put into the TWiCE packet pool.  If it was, we will
	// clean it up / release it to the ScaleBox pool later, typically when all of its cache entries are
	// too old without recent hits.  If we do not save it, that means we need to release it here in the
	// searching for a hit function.
	//
	// Note that we do not modify the inbound packet here, we save that for the main processing function
	// as a decision point if we get a hit as we still need to resolve if the child node is part of our
	// "caching club" and if that downstream client is likely to have it.


	// Work with pBasePacket from here on out for the purposes of cache saving / comparison

	// Hmmm, we are going to have some magic once we start modding the packet

	pthread_mutex_lock(getMutex());

//	printf("* Starting search with offset of %d and %d remaining to search\n", nOffset, nBytesRemaining);

	while(nBytesRemaining >= 0)
	{
		// Our order of events
		//  1. Compute the checksum
		//  2. See if we have a hit
		//       If yes, see if it is a real hit or a collision
		//			If yes, we be done, time to let someone else find the right edge
		//		 If no, should we add it to the cache?

		// Compute the checksum

		// For now, we are just doing XOR

		int nMode;
		int nTableEntry;
		unsigned int		nResultXOR;

		nMode = 0;
		nTableEntry = -1;

		nResultXOR = computeChecksum_XOR(pBasePacket->getData()+nOffset+nDistanceOffset,getWindowSize());
		m_Stats.Add_Stat(TWICE_CACHE_CKSUM_COMPS, 1);

		nTableEntry = nResultXOR % m_nMaxTableSize;

		printf("Ck: DB: %p   Offset: %d  Value: %d  Window: %d\n", pBasePacket->getData(), nOffset+nDistanceOffset, nResultXOR, getWindowSize());

		cout << "* Resolving the table entry, result post modulus is " << nTableEntry << endl;
		cout << "   Result XOR was " << nResultXOR << endl;

		// Did we resolve to an entry in the table?  We pretty much always do as the current modulus
		//  function in place ensures that we are always between 0 and the N-1 entry in the cache
		//  entry array.

		if(m_ppEntries[nTableEntry] != NULL)
		{
			cout << " Existing entry was not NULL, verifying...." << endl;

			// There is already something there

			// Are we a match for that entry?
			if(m_ppEntries[nTableEntry]->getValue() == nResultXOR)
			{
				// The indicator (XOR) is a hit, but is it really?

				// The reason we need to check is that we may end up with collisions, particularly if we
				// use a bad discriminator like an XOR.  This ensures that yes, the match is indeed really
				// a match, not just a coincidental alignment ala the Pigeonhole Principle.

				// What is the Pigeonhole Prinicple?
				//
				// If you have P pigeons to put into N holes and P > N, that means that at least one
				// hole is going to have more than 1 pigeon.  Bad, bad, bad for us as that means we goofed
				// and think we have the exact same content when we do not.

				// Compare the window we constructed the hash over in the Input packet to our entry in the
				// table where its hash was constructed.  If memcmp tells us a difference of zero, we win!

//				cout << "   Mem, mem cmp it up" << endl;

				// Let's make sure our data reference is still good
				char *	pDataRef;

				pDataRef = m_ppEntries[nTableEntry]->resolveDataReference();

				m_Stats.Add_Stat(TWICE_CACHE_CKSUM_MATCH_VALUE, 1);

				if(pDataRef == NULL)
				{
					cerr << "Error: Could not conduct a memcmp as the cache entry points to a bogus pool" << endl;
					cerr << "  packet or the pool packet has a bogus packet inside it.  Ignoring comparison" << endl;
					cerr << "  Dumping table entry information " << endl;
					m_ppEntries[nTableEntry]->dumpDebug();
				}
				else if(memcmp(pBasePacket->getData()+nOffset+nDistanceOffset, pDataRef, getWindowSize()) == 0)
				{
					m_Stats.Add_Stat(TWICE_CACHE_CKSUM_CORRECT_MATCH, 1);

					// Check if that match was in this same packet, i.e. we can't register a hit
					//  on an intra-packet basis (for now)

					if(pBasePacket == m_ppEntries[nTableEntry]->getPoolReference()->getPacket())
					{
						if(nOffset+nDistanceOffset == m_ppEntries[nTableEntry]->getOffset())
						{
							cout << "Why are you calling me again on the same exact block????" << endl;
							printf("     Offset of %d vs. prior cache entry based on %d\n", nOffset+nDistanceOffset, m_ppEntries[nTableEntry]->getOffset());
							printf("       Offset is made up of %d + %d w/%d bytes remaining\n", nOffset, nDistanceOffset, nBytesRemaining);

							m_ppEntries[nTableEntry]->dumpDebug();

							// Repeat call on the same block, not our fault entirely
							nDistanceOffset++;
							nBytesRemaining--;
							continue;
						}

						m_Stats.Add_Stat(TWICE_CACHE_CKSUM_CORRECT_MATCH_INTRA,1);
						//categorizeHit(m_ppEntries[nTableEntry], pBasePacket->getArrivalTime());
						//m_ppEntries[nTableEntry]->registerHit();
						// Nothing to do here

						//printf("** Intra-packet hit, need to dig\n");
						//printf("     Offset of %d vs. prior hit in %d\n", nOffset+nDistanceOffset, m_ppEntries[nTableEntry]->getOffset());
						//printf("    Data: ");
						//pBasePacket->showHex(nOffset+nDistanceOffset, nOffset+nDistanceOffset+getWindowSize());
						//printf("    Text: ");
						//pBasePacket->showText(nOffset+nDistanceOffset, nOffset+nDistanceOffset+getWindowSize());
						//printf("\n");

						// Jump the whole window over as it is pointless otherwise
						nDistanceOffset += getWindowSize();
						nBytesRemaining -= getWindowSize();
						continue;
					}
					else
					{
//						cout << "** Holy jamoley!  We have a winnah!" << endl;

						//printf("  Pkt: %X vs. Pkt: %X\n", pBasePacket, m_ppEntries[nTableEntry]->getPoolReference()->getPacket());
						//printf("    Hit was at offset of %d, window of %d\n", nOffset+nDistanceOffset, getWindowSize());
						//printf("    Table Entry: %d\n", nTableEntry);
						//printf("    Arr Time (In): ");
						//displayTime(pBasePacket->getArrivalTime());
						//printf("\n");

						//printf("    Arr Time (Cache): ");
						//displayTime(m_ppEntries[nTableEntry]->getPoolReference()->getPacket()->getArrivalTime());
						//printf("\n");

						// Yes, we have bananas today.  This is pure awesome sauce and we really, really, really
						//  like to see this number go up.

						// Categorize the hit type
						categorizeHit(m_ppEntries[nTableEntry], pBasePacket->getArrivalTime());

						m_ppEntries[nTableEntry]->registerHit();
						m_ppEntries[nTableEntry]->setLastUsedTime(pBasePacket->getArrivalTime());

						// TODO: We may need to adjust this if the entry gets hot and then fades.  There could be some potential
						//  issues if we hotspot, i.e. burst of activity then none.  We just get stuck with a dud in the cache
						//  library as we are doing things additively
						adjustTime(m_ppEntries[nTableEntry]->getExpiryTime(), TWICECACHETABLE_DEFAULT_EXPIRY_HIT_ADJUST);

						pPoolEntry->setLastUsedTime(pBasePacket->getArrivalTime());

						if(pnMatch != NULL)
						{
							*pnMatch = nOffset + nDistanceOffset;
//							cout << "Start of the left edge is identified as " << *pnMatch << endl;
						}

						pthread_mutex_unlock(getMutex());
						return m_ppEntries[nTableEntry];
					}
				}
				else
				{
					m_Stats.Add_Stat(TWICE_CACHE_MISS, 1);
					m_Stats.Add_Stat(TWICE_CACHE_CKSUM_MISTAKE, 1);

					// Ruh roh.  The pigeonhole principle gets us.  The hash looked good but our beer
					// goggles were fail and our discriminating hash function stinks.  This is kind of bad
					// as we effectively wasted that memcmp.  Here is to hoping we were on an architecture
					// where that was not too much of a hit.

					// TODO: For speed issues on multiple misses, does that mean we might probabilistically
					//       engage attempting to find a hit after multiple misses?

//					cout << "** Miss on an existing entry, offset = " << nOffset + nDistanceOffset << endl;

					// Something was wonky, register a misfire on the cache entry
					m_ppEntries[nTableEntry]->registerMiss();

					considerCacheWithMode(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket, nDistanceOffset, bSavedInPool, pPoolEntry);

/*
					// Should we replace that entry in the table?
					TWiCECacheEntry *	pCacheEntry;

					if(nDistanceOffset - nLastSave > getMinGapBetweenCacheCommit())
					{
						pCacheEntry = considerCache(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket);
					}
					else
					{
						m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_TOOCLOSE, 1);
						pCacheEntry = NULL;
					}

					// If that result is non-NULL, we need to link it up with our packet pool.  It also means
					//  that the packet has effectively now been added to the packet pool and should not be
					//  released to the memory pool until all of its references let go of it

					if(pCacheEntry != NULL)
					{
						bSavedInPool = 1;
						pPool->linkCacheEntry(pCacheEntry, pBasePacket);
						nLastSave = nDistanceOffset;
					} */

				}
			}
			else
			{
				m_Stats.Add_Stat(TWICE_CACHE_MISS, 1);
				m_Stats.Add_Stat(TWICE_CACHE_CKSUM_MISS_PIGEON, 1);

				// Conflict but wrong value in the spot, discriminating function FTW!

				considerCacheWithMode(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket, nDistanceOffset, bSavedInPool, pPoolEntry);

//				cout << "** Miss caught by hash function, offset = " << nOffset + nDistanceOffset << endl;

				/*
				TWiCECacheEntry *	pCacheEntry;

				if(nDistanceOffset - nLastSave > getMinGapBetweenCacheCommit())
				{
					pCacheEntry = considerCache(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket);
				}
				else
				{
					pCacheEntry = NULL;
				}


				// If that result is non-NULL, we need to link it up with our packet pool.  It also means
				//  that the packet has effectively now been added to the packet pool and should not be
				//  released to the memory pool until all of its references let go of it
				if(pCacheEntry != NULL)
				{
					nLastSave = nDistanceOffset;
					bSavedInPool = 1;
					pPool->linkCacheEntry(pCacheEntry, pBasePacket);
				}
				*/
			}
		}
		else
		{
			cout << "  Empty entry in hash table" << endl;

			// Nope, nothing there
			m_Stats.Add_Stat(TWICE_CACHE_MISS, 1);
			m_Stats.Add_Stat(TWICE_CACHE_MISS_EMPTY_SLOT, 1);

			cout << "   Considering cache" << endl;
			considerCacheWithMode(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket, nDistanceOffset, bSavedInPool, pPoolEntry);
			cout << "   Finished with consideration" << endl;

/*
			// Should we replace that entry in the table?
			TWiCECacheEntry *	pCacheEntry;

			if(nDistanceOffset - nLastSave > getMinGapBetweenCacheCommit())
			{
//				cout << "Consider cache" << endl;
				pCacheEntry = considerCache(nTableEntry, nResultXOR, nOffset+nDistanceOffset, pBasePacket);
//				cout << "Consider cache done" << endl;
			}
			else
			{
				m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_TOOCLOSE, 1);
				pCacheEntry = NULL;
			}

			// If that result is non-NULL, we need to link it up with our packet pool.  It also means
			//  that the packet has effectively now been added to the packet pool and should not be
			//  released to the memory pool until all of its references let go of it
			if(pCacheEntry != NULL)
			{
				nLastSave = nDistanceOffset;
				bSavedInPool = 1;
//				cout << "  Linking cache entry" << endl;
				pPool->linkCacheEntry(pCacheEntry, pBasePacket);
//				cout << "  Done linking cache entry" << endl;
			}
			*/
		}

		nDistanceOffset++;
		nBytesRemaining--;
	}

	pthread_mutex_unlock(getMutex());
	return NULL;
}

///////////////////////////////////////////////////////////////////////////

unsigned int TWiCECacheTable::computeChecksum_XOR (char * pData, int nWindow)
{
	unsigned int	*		pDataPtr;
	unsigned int			nResult;
	int			j;

	nResult	=	0;

	pDataPtr	= (unsigned int *) (pData);

	for(j=0; j<nWindow; j+=sizeof(int)) {
		nResult = nResult ^ *(pDataPtr);
		pDataPtr++;
	}

	return nResult;
}

void	TWiCECacheTable::dumpTopList   (int nList)
{
	updateTopLists();

	switch(nList)
	{
		case TWICE_CACHE_RANKING_HITS:
			m_pRankHits->dumpList("Top Hits");
			break;
	}


}


///////////////////////////////////////////////////////////////////////////

int		TWiCECacheTable::getWindowSize()
{
	return m_nWindowSize;
}

///////////////////////////////////////////////////////////////////////////

void 	TWiCECacheTable::setWindowSize (int nSize)
{
	if(m_nWindowSize > 0)
	{
		m_nWindowSize = nSize;
	}
	else
	{
		cerr << "Warning: Bad window size for cache table, ignoring" << endl;
		cerr << "  Requested window size was " << nSize << endl;
		return;
	}
}

///////////////////////////////////////////////////////////////////////////

char TWiCECacheTable::removeCacheEntry (int nEntry, int nSubEntry, TWiCEPacketPoolEntry * pPoolEntry)
{
	if(m_ppEntries[nEntry] == NULL)
	{
		cerr << "Error: Cannot remove an entry from the cache that is already NULL" << endl;
		return 1;
	}

	// Identify the particular entry we are using
	TWiCECacheEntry *	pCacheEntry;

	pCacheEntry = m_ppEntries[nEntry];

	// Rip out the appropriate stats

	m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES, 1);

	if(pCacheEntry->getHits() == 0)
	{
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_NOHITS, 1);
	}
	else
	{
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_HITS, pCacheEntry->getHits());
	}

	if(pCacheEntry->getMisses() > 0)
	{
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_HASMISSES, 1);
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_MISSES, pCacheEntry->getMisses());
	}

	// How old is the entry in second when it was purged?

	struct timeval 	cTime;
	int		nTimeDiff;

	gettimeofday(&cTime, NULL);

	nTimeDiff = cTime.tv_sec - pCacheEntry->getCreationTime()->tv_sec;

	m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_AGE, nTimeDiff);

	if(pCacheEntry->getHits() > 0)
	{
		nTimeDiff = cTime.tv_sec - pCacheEntry->getLastUsedTime()->tv_sec;
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_AGE_LASTUSED, nTimeDiff);
	}

	// OK, we need to purge the fact that this cache entry referenced that packet that
	//  we had pooled and if we are the last one referencing the original packet, give it
	//  back up to the packet queue.

//	TWiCEPacketPoolEntry *	pPoolEntry;

	// Find the packet in the cached pool that this entry was pointing to
//	pPoolEntry = m_ppEntries[nEntry]->getPoolReference();

	// Already provided via an argument - no need to search now

	// First, remove the reference to the outer packet pool
	if(pPoolEntry != NULL)
	{
		pPoolEntry->decReference(pCacheEntry);

		// See if we need to get rid of it (right now)
		m_pPacketPool->checkPoolPurge(pPoolEntry);
		pPoolEntry = NULL;

		// TODO: Do we need to queue or pool the pool objects to avoid new / delete costs?

		// Old version
		// The pool entry is eventually cleaned up by a periodic timer that is invoked at the
		//  gateway to purge old entries that do not have any references
	}
	else
	{
		cerr << "Error: The entry being tossed from the cache was not pointing to anything (ruh roh)" << endl;

	}



	// Now, onto this particular entry

	// Free it back to the pool or wherever it came from
	delete pCacheEntry;
	pCacheEntry = NULL;

	// Mark the original entry as NULL
	m_ppEntries[nEntry] = NULL;
	m_nEntries--;

	return 1;
}


char TWiCECacheTable::removeCacheEntry_NoPool (int nEntry, int nSubEntry, TWiCEPacketPoolEntry * pPoolEntry)
{
	if(m_ppEntries[nEntry] == NULL)
	{
		cerr << "Error: Cannot remove an entry from the cache that is already NULL" << endl;
		return 1;
	}

	// Identify the particular entry we are using
	TWiCECacheEntry *	pCacheEntry;

	pCacheEntry = m_ppEntries[nEntry];

	// Rip out the appropriate stats

	m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES, 1);

	if(pCacheEntry->getHits() == 0)
	{
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_NOHITS, 1);
	}
	else
	{
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_HITS, pCacheEntry->getHits());
	}

	if(pCacheEntry->getMisses() > 0)
	{
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_HASMISSES, 1);
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_MISSES, pCacheEntry->getMisses());
	}

	// How old is the entry in second when it was purged?

	struct timeval 	cTime;
	int		nTimeDiff;

	gettimeofday(&cTime, NULL);

	nTimeDiff = cTime.tv_sec - pCacheEntry->getCreationTime()->tv_sec;

	m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_AGE, nTimeDiff);

	if(pCacheEntry->getHits() > 0)
	{
		nTimeDiff = cTime.tv_sec - pCacheEntry->getLastUsedTime()->tv_sec;
		m_Stats.Add_Stat(TWICE_CACHE_MAINT_REMOVED_ENTRIES_AGE_LASTUSED, nTimeDiff);
	}


	// Now, onto this particular entry

	// Free it back to the pool or wherever it came from
	delete pCacheEntry;
	pCacheEntry = NULL;

	// Mark the original entry as NULL
	m_ppEntries[nEntry] = NULL;
	m_nEntries--;

	return 1;
}




TWiCEPacketPool	*	TWiCECacheTable::getPacketPool ()
{
	return m_pPacketPool;
}

void TWiCECacheTable::setPacketPool (TWiCEPacketPool * pPacketPool)
{
	m_pPacketPool = pPacketPool;
}


int	TWiCECacheTable::considerCacheWithMode_NoPool (int nEntry, int nValue, int nOffset, Packet * pPacket, int nDistanceStart)
{
	int		nWindowResult;

	// Consider this packet for inclusion in the cache table

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE, 1);

	cout << "--| considerCacheWithMode" << endl;
//	printf("   nEntry: %d  nValue: %d  nOffset: %d\n", nEntry, nValue, nOffset);

	// Sanity check on the range
	if(nEntry < 0 || nEntry >= m_nMaxTableSize)
	{
		cerr << "** Error: Illegally requested entry of " << nEntry << endl;
		return -1;
	}


	if(m_ppEntries[nEntry] == NULL)
	{
		m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_EMPTY, 1);
	}
	else if (m_ppEntries[nEntry]->isExpired(pPacket->getArrivalTime()))
	{
		if(m_ppEntries[nEntry]->getHits() > 0)
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS,1);
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS_OLD,1);
		}
		else
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS,1);
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS_OLD,1);
		}

		removeCacheEntry_NoPool(nEntry, 0, pPoolEntry);
	}
	else
	{
		// Not expired
		if(m_ppEntries[nEntry]->getHits() > 0)
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS_NOTOLDENOUGH,1);
		}
		else
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS_NOTOLDENOUGH,1);
		}

		// Was this supposed to be a keyframe though?

		nWindowResult = nDistanceStart % (m_nWindowSize / TWICECACHETABLE_DEFAULT_RATIO_I_FRAME);

		if(!nWindowResult)
		{
			// Why, yes, yes it was.  Need to ensure that this packet tries again for a keyframe on
			//  the next attempt to cache
		}

		return 0;
	}

	// OK, let's overwrite what is there
	//
	//  Provided this cache entry had referenced something in the pool, tell that pool
	//  that one less item in the cache now references it which can mean expulsion from
	//  the pool if its reference count drops to zero
	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE, 1);

	// Need to eventually globally pool this one too
	m_ppEntries[nEntry] = new TWiCECacheEntry ();

	// Check for successful allocation
	if(m_ppEntries[nEntry] == NULL)
	{
		cerr << "*** Error: Unable to allocate a new block of memory" << endl;
		return 0;
	}

	m_ppEntries[nEntry]->setValue(nValue);
	m_ppEntries[nEntry]->setOffset(nOffset);

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE_OFFSET, nOffset);

	// Set the creation time and last used time information
	m_ppEntries[nEntry]->setCreationTime(pPacket->getArrivalTime());
	m_ppEntries[nEntry]->setLastUsedTime(pPacket->getArrivalTime());

//	cout << "  -| Overwrite consideration (entry population - part 1)" << endl;
//	printf("  Packet information was 0x%X\n", pPacket);

	// The expiry is directly dependent on where we are at in the packet. We define
	//  different levels of expiry that are shaped by where in the packet we are at and follow
	//  the logic of an I, P, B sort of MPEG arrangement.
	//
	// The I or key frame receives a healthy expiry and is based on the logic that the average length
	//  of a match is likely to be much longer than our search window.  We currently define this
	//  as a ratio of the window size divided by two (32 bytes).
	//
	// The P or delta frame receives a reasonable expiry and is a sub-multiple of the I ratio.  We
	//  define this as the window size divided by eight (8 bytes)
	//
	// The B frames are the simplest and easiest to discard.  They receive no actual expiry and require
	//  a hit to actually stay existing in the system
	//
	// We compute this based on the distance from the start of this current invocation of the wireless
	//  packet search.  It is from the current invocation in case we are invoked multiple times for
	//  the cases of multiple saveable blocks within the packet

	struct timeval 	*	pExpiry;

	pExpiry = m_ppEntries[nEntry]->getExpiryTime();

	// What was the expiry?
//	printf("  Validating expiry timers: 0x%X vs. 0x%X\n", pExpiry, pPacket->getArrivalTime());

	*pExpiry = *(pPacket->getArrivalTime());

//	cout << "  -| Overwrite consideration (entry population - part 2)" << endl;

	nWindowResult = nDistanceStart % (m_nWindowSize / TWICECACHETABLE_DEFAULT_RATIO_I_FRAME);

	if(!nWindowResult)
	{
		// We have an I or key frame that we should note
		//printf("I Frame\n");
		adjustTime(pExpiry, TWICECACHETABLE_DEFAULT_EXPIRY_I_FRAME);

//		cout << "  -| Overwrite consideration (entry population - I frame)" << endl;
	}
	else
	{
//		cout << "  -| Overwrite consideration (entry population - P frame)" << endl;

		nWindowResult = nWindowResult % (m_nWindowSize / TWICECACHETABLE_DEFAULT_RATIO_P_FRAME);

		if(!nWindowResult)
		{
			// This is a P frame, give it a fairly fast but not awful expiry time
			//printf(" P Frame\n");
			adjustTime(pExpiry, TWICECACHETABLE_DEFAULT_EXPIRY_P_FRAME);
		}
		else
		{
			// This is a B frame, it gets peanuts
			//printf("  B Frame\n");
			adjustTime(pExpiry, TWICECACHETABLE_DEFAULT_EXPIRY_B_FRAME);
		}
	}

	m_nEntries++;

	// Finish up everything
	return 1;
}



///////////////////////////////////////////////////////////////////////////

int	TWiCECacheTable::considerCacheWithMode (int nEntry, int nValue, int nOffset, Packet * pPacket,
				   	   	   int nDistanceStart, char & bSavedInPool, TWiCEPacketPoolEntry * pPoolEntry)
{
	int		nWindowResult;

	// Consider this packet for inclusion in the cache table

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE, 1);

	cout << "--| considerCacheWithMode" << endl;
	printf("   nEntry: %d  nValue: %d  nOffset: %d\n", nEntry, nValue, nOffset);
	printf("   pPoolEntry: 0x%X\n", pPoolEntry);

	// Sanity check on the range
	if(nEntry < 0 || nEntry >= m_nMaxTableSize)
	{
		cerr << "** Error: Illegally requested entry of " << nEntry << endl;
		return -1;
	}


	if(m_ppEntries[nEntry] == NULL)
	{
		m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_EMPTY, 1);
	}
	else if (m_ppEntries[nEntry]->isExpired(pPacket->getArrivalTime()))
	{
		if(m_ppEntries[nEntry]->getHits() > 0)
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS,1);
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS_OLD,1);
		}
		else
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS,1);
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS_OLD,1);
		}

		removeCacheEntry(nEntry, 0, pPoolEntry);
	}
	else
	{
		// Not expired
		if(m_ppEntries[nEntry]->getHits() > 0)
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS_NOTOLDENOUGH,1);
		}
		else
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS_NOTOLDENOUGH,1);
		}

		// Was this supposed to be a keyframe though?

		nWindowResult = nDistanceStart % (m_nWindowSize / TWICECACHETABLE_DEFAULT_RATIO_I_FRAME);

		if(!nWindowResult)
		{
			// Why, yes, yes it was.  Need to ensure that this packet tries again for a keyframe on
			//  the next attempt to cache
		}

		return 0;
	}

	cout << "  -| Overwrite consideration" << endl;

	// OK, let's overwrite what is there
	//
	//  Provided this cache entry had referenced something in the pool, tell that pool
	//  that one less item in the cache now references it which can mean expulsion from
	//  the pool if its reference count drops to zero

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE, 1);

	// Need to eventually globally pool this one too
	m_ppEntries[nEntry] = new TWiCECacheEntry ();

	// Check for successful allocation
	if(m_ppEntries[nEntry] == NULL)
	{
		cerr << "*** Error: Unable to allocate a new block of memory" << endl;
		return 0;
	}

	m_ppEntries[nEntry]->setValue(nValue);
	m_ppEntries[nEntry]->setOffset(nOffset);

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE_OFFSET, nOffset);

	// Set the creation time and last used time information
	m_ppEntries[nEntry]->setCreationTime(pPacket->getArrivalTime());
	m_ppEntries[nEntry]->setLastUsedTime(pPacket->getArrivalTime());

	cout << "  -| Overwrite consideration (entry population - part 1)" << endl;
	printf("  Packet information was 0x%X\n", pPacket);

	// The expiry is directly dependent on where we are at in the packet. We define
	//  different levels of expiry that are shaped by where in the packet we are at and follow
	//  the logic of an I, P, B sort of MPEG arrangement.
	//
	// The I or key frame receives a healthy expiry and is based on the logic that the average length
	//  of a match is likely to be much longer than our search window.  We currently define this
	//  as a ratio of the window size divided by two (32 bytes).
	//
	// The P or delta frame receives a reasonable expiry and is a sub-multiple of the I ratio.  We
	//  define this as the window size divided by eight (8 bytes)
	//
	// The B frames are the simplest and easiest to discard.  They receive no actual expiry and require
	//  a hit to actually stay existing in the system
	//
	// We compute this based on the distance from the start of this current invocation of the wireless
	//  packet search.  It is from the current invocation in case we are invoked multiple times for
	//  the cases of multiple saveable blocks within the packet

	struct timeval 	*	pExpiry;

	pExpiry = m_ppEntries[nEntry]->getExpiryTime();

	// What was the expiry?
	printf("  Validating expiry timers: 0x%X vs. 0x%X\n", pExpiry, pPacket->getArrivalTime());


	*pExpiry = *(pPacket->getArrivalTime());

	cout << "  -| Overwrite consideration (entry population - part 2)" << endl;

	nWindowResult = nDistanceStart % (m_nWindowSize / TWICECACHETABLE_DEFAULT_RATIO_I_FRAME);

	if(!nWindowResult)
	{
		// We have an I or key frame that we should note
		//printf("I Frame\n");
		adjustTime(pExpiry, TWICECACHETABLE_DEFAULT_EXPIRY_I_FRAME);

		cout << "  -| Overwrite consideration (entry population - I frame)" << endl;
	}
	else
	{
		cout << "  -| Overwrite consideration (entry population - P frame)" << endl;

		nWindowResult = nWindowResult % (m_nWindowSize / TWICECACHETABLE_DEFAULT_RATIO_P_FRAME);

		if(!nWindowResult)
		{
			// This is a P frame, give it a fairly fast but not awful expiry time
			//printf(" P Frame\n");
			adjustTime(pExpiry, TWICECACHETABLE_DEFAULT_EXPIRY_P_FRAME);
		}
		else
		{
			// This is a B frame, it gets peanuts
			//printf("  B Frame\n");
			adjustTime(pExpiry, TWICECACHETABLE_DEFAULT_EXPIRY_B_FRAME);
		}
	}

	m_nEntries++;

	// Finish up everything
	printf("    pPoolEntry: 0x%X   nEntry: %d\n", pPoolEntry, nEntry);


	cout << "  -| Overwrite consideration (entry population - set Reference)" << endl;
	m_ppEntries[nEntry]->setPoolReference(pPoolEntry);

	cout << "  -| Overwrite consideration (entry population - start linking)" << endl;


	// Do the mutual linking
	pPoolEntry->incReference(m_ppEntries[nEntry]);




	cout << "  -| Overwrite consideration (entry population - linking finito)" << endl;



//	pPool->linkCacheEntry(m_ppEntries[nEntry], pPacket);
	bSavedInPool = 1;
	return 1;
}

///////////////////////////////////////////////////////////////////////////


TWiCECacheEntry * TWiCECacheTable::considerCache (int nEntry, int nValue, int nOffset, Packet * pPacket)
{
	cout << "**** ERROR: Should not be used" << endl;

	// Consider this packet for inclusion in the cache table
	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE, 1);

	// Is the cache entry empty?

	if(m_ppEntries[nEntry] == NULL)
	{
		// Nothing there, so this is a winner
		m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_EMPTY, 1);
	}
	else
	{
//		cout << " --> Cache slot @ " << nEntry << " is NOT EMPTY" << endl;

		// Something there, need to check a few things

		// Is the existing entry old enough to evict?  Well, the answer depends on if it has ever had any
		// hits in the past.  If it has, well, we perhaps need to consider it differently.  For now, we allow
		// the number of hits to influence the age at which the entry is considered too old.

		if(m_ppEntries[nEntry]->getHits() > 0)
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS,1);

			// The multiplier applies
			if(m_ppEntries[nEntry]->exceedsAge(pPacket->getArrivalTime(), this->getEntryTooOld() * this->getEntryTooOld_HitMultiplier()))
			{
//				cout << " *** REPLACE IT -> Entry is too old (Multiplier) ***" << endl;
				m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS_OLD,1);
				removeCacheEntry(nEntry, 0, NULL);
			}
			else
			{
				m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_HASHITS_NOTOLDENOUGH,1);
				return NULL;
			}
		}
		else
		{
			m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS,1);

			// The multiplier applies
			if(m_ppEntries[nEntry]->exceedsAge(pPacket->getArrivalTime(), this->getEntryTooOld()))
			{
				m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS_OLD,1);
//				cout << " *** REPLACE IT -> Entry is too old (No Multiplier) ***" << endl;
				removeCacheEntry(nEntry, 0, NULL);
			}
			else
			{
				m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_NOHITS_NOTOLDENOUGH,1);
				return NULL;
			}
		}

		// For now, no, hit means drop
		//return NULL;
	}

	// OK, let's overwrite what is there
	//
	//  Provided this cache entry had referenced something in the pool, tell that pool
	//  that one less item in the cache now references it which can mean explusion from
	//  the pool if its reference count drops to zero

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE, 1);

	if(m_ppEntries[nEntry] != NULL)
	{
		if(m_ppEntries[nEntry]->getPoolReference() != NULL)
		{
			m_ppEntries[nEntry]->getPoolReference()->decReference(m_ppEntries[nEntry]);
		}

		// Delete / set it to NULL to force a seg fault if we booch it
		delete m_ppEntries[nEntry];
		m_ppEntries[nEntry] = NULL;

		// Cleanup to help easy pool removal?

		// Maybe, add that later!
		//  TODO: Should it be here or later, dunno, probably here
	}

	// Need to eventually globally pool this one too
	m_ppEntries[nEntry] = new TWiCECacheEntry ();

	m_ppEntries[nEntry]->setValue(nValue);
	m_ppEntries[nEntry]->setOffset(nOffset);

	m_Stats.Add_Stat(TWICE_CACHE_CONSIDERCACHE_CREATE_CACHE_OFFSET, nOffset);

	// Set the creation time and last used time information
	m_ppEntries[nEntry]->setCreationTime(pPacket->getArrivalTime());
	m_ppEntries[nEntry]->setLastUsedTime(pPacket->getArrivalTime());

	m_nEntries++;

	// Give back the pointer showing that we meant business
	return m_ppEntries[nEntry];
}

///////////////////////////////////////////////////////////////////////////

char TWiCECacheTable::dumpStatus ()
{
	cout << "TWiCE Cache Table at the Gateway status" << endl;
	cout << "  Entries: " << m_nEntries << " out of "<< getMaxTableSize() << endl;
	cout << "  Hash Settings:   Window = " << getWindowSize() << endl;
}

unsigned int TWiCECacheTable::getEntryTooOld ()
{
	return m_nEntry_TooOld;
}

void TWiCECacheTable::setEntryTooOld (unsigned int lTooOld)
{
	m_nEntry_TooOld = lTooOld;
}

TWiCE_CacheTableStats * TWiCECacheTable::getStats()
{
	return &m_Stats;
}

TWiCE_CacheTableStats * TWiCECacheTable::getStats_Diff()
{
	return &m_DiffStats;
}

TWiCE_CacheTableStats * TWiCECacheTable::getStats_Prior()
{
	return &m_PriorStats;
}

void TWiCECacheTable::saveStats_Prior()
{
	m_PriorStats.syncAll(&m_Stats);
}

void TWiCECacheTable::updateStats_Diff()
{
	m_DiffStats.computeDiff(&m_PriorStats,&m_Stats);
}



/////////////////////////////////////////////////////////////////////

int TWiCECacheTable::categorizeHit (TWiCECacheEntry * pEntry, struct timeval * pCurrentTime)
{
	// Note: The last hit cannot have been updated before this is called

	// Categorize the hit based on the time since the hit

	unsigned int		lDiff;

	lDiff = calcTimeDiff (pCurrentTime, pEntry->getLastUsedTime());

	if(lDiff < TWICE_CACHEHIT_GAP_TIGHT)
	{
		m_Stats.Add_Stat(TWICE_CACHE_HIT_TYPE_COUNT_TIGHT, 1);
		return TWICE_CACHEHIT_TYPE_TIGHT;
	}
	else if(lDiff < TWICE_CACHEHIT_TYPE_SHORT)
	{
		m_Stats.Add_Stat(TWICE_CACHE_HIT_TYPE_COUNT_SHORT, 1);
		return TWICE_CACHEHIT_TYPE_SHORT;
	}
	else if(lDiff < TWICE_CACHEHIT_GAP_SHORT_MAX)
	{
		m_Stats.Add_Stat(TWICE_CACHE_HIT_TYPE_COUNT_SHORT_MAX, 1);
		return TWICE_CACHEHIT_TYPE_SHORT_MAX;
	}
	else if(lDiff < TWICE_CACHEHIT_GAP_MEDIUM_MIN)
	{
		m_Stats.Add_Stat(TWICE_CACHE_HIT_TYPE_COUNT_MEDIUM_MIN, 1);
		return TWICE_CACHEHIT_TYPE_MEDIUM_MIN;
	}
	else if(lDiff < TWICE_CACHEHIT_GAP_MEDIUM)
	{
		m_Stats.Add_Stat(TWICE_CACHE_HIT_TYPE_COUNT_MEDIUM, 1);
		return TWICE_CACHEHIT_TYPE_MEDIUM;
	}
	else
	{
		m_Stats.Add_Stat(TWICE_CACHE_HIT_TYPE_COUNT_LUCKY, 1);
		return TWICE_CACHEHIT_TYPE_LUCKY;
	}
}

TWiCECacheEntryRanking::TWiCECacheEntryRanking()
{
	m_lMetric = 0;
	m_pEntry = NULL;
}

unsigned int TWiCECacheEntryRanking::getMetric ()
{
	return m_lMetric;
}

void TWiCECacheEntryRanking::setMetric (unsigned int lMetric)
{
	m_lMetric = lMetric;
}

TWiCECacheEntry * TWiCECacheEntryRanking::getEntry ()
{
	return m_pEntry;
}

void TWiCECacheEntryRanking::setEntry (TWiCECacheEntry * pEntry)
{
	m_pEntry = pEntry;
}

TWiCECacheTopList::TWiCECacheTopList()
{
	m_nRankSize = TWICE_CACHE_RANKING_DEFAULT_SIZE;
	m_nRankFilled = 0;

	m_pRankEntries = new TWiCECacheEntryRanking[m_nRankSize];
}

TWiCECacheTopList::~TWiCECacheTopList()
{
	if(m_pRankEntries != NULL)
	{
		delete m_pRankEntries;
	}
}

int TWiCECacheTopList::getRankSize()
{
	return m_nRankSize;
}

void TWiCECacheTopList::setRankSize(int nSize)
{
	if(nSize >= 1)
	{
		m_nRankSize = nSize;
	}
}

void TWiCECacheTopList::reset ()
{
	int		j;

	m_nRankFilled = 0;

	for(j=0; j<m_nRankSize; j++)
	{
		m_pRankEntries[j].setEntry(NULL);
		m_pRankEntries[j].setMetric(0);
	}
}

void TWiCECacheTopList::dumpList (char * szRanking)
{
	printf("Ranking for %s\n", szRanking);

	int		j;

	for(j=0; j<m_nRankFilled; j++)
	{
		printf("%2d)", j);
		m_pRankEntries[j].getEntry()->dumpSingleLine();
		printf("\n");
	}
}

void TWiCECacheTopList::rankEntry (TWiCECacheEntry * pEntry, unsigned int lMetric)
{
	int		k;

	if(pEntry == NULL)
	{
		return;
	}

	if(m_nRankFilled == 0)
	{
		m_pRankEntries[m_nRankFilled].setEntry(pEntry);
		m_pRankEntries[m_nRankFilled].setMetric(lMetric);
		m_nRankFilled++;
		return;
	}

	for(k=m_nRankFilled; k>0; k++)
	{
		if(lMetric <= m_pRankEntries[k-1].getMetric())
		{
			if(k==m_nRankSize)
			{
				// You are not even better than the last ranking in the
				//  list.  No sense checking further
				return;
			}
			else if(m_nRankFilled < m_nRankSize)
			{
				cout << "Tacking to the end since I am smaller and hash later" << endl;
				cout << "   " << lMetric << " vs. " << m_pRankEntries[k-1].getMetric() << endl;

				// Tack it on the end, we have not fully populated the
				//  list
				m_pRankEntries[m_nRankFilled].setEntry(pEntry);
				m_pRankEntries[m_nRankFilled].setMetric(lMetric);
				m_nRankFilled++;
				return;
			}
			else
			{
				// The good college try.  You made it onto the list
				//  with your stellar performance but not all the way
				//  to the top.  The k+1'th entry is your reward. You
				//  are less than the k'th entry
				m_pRankEntries[k-1].setEntry(pEntry);
				m_pRankEntries[k-1].setMetric(lMetric);
				return;
			}
		}
		else
		{
			cout << "  Bump, bump down the road" << endl;
			dumpList("Post Bump");

			// Alright, the candidate is better, bump the current entry
			//  down to the next one only if we are at rank N-1 and
			//  below
			if(k<m_nRankSize)
			{
				m_pRankEntries[k] = m_pRankEntries[k-1];
			}
		}
	}

	if(k==0)
	{
		m_pRankEntries[0].setEntry(pEntry);
		m_pRankEntries[0].setMetric(lMetric);
	}
}

