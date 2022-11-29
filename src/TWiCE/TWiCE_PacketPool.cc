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

#include "../util/ip-utils.h"

#include "TWiCE_PacketPool.h"
#include "TWiCE_CacheTable.h"

TWiCEPacketPoolEntry::TWiCEPacketPoolEntry() {
	m_pPacket = NULL;
	m_nRefs = 0;
	m_pOriginalPacket = NULL;
}

struct timeval * TWiCEPacketPoolEntry::getCreationTime() {
	return &m_CreationTime;
}

Packet * TWiCEPacketPoolEntry::getOriginalPacket ()
{
	return m_pOriginalPacket;
}

void TWiCEPacketPoolEntry::setOriginalPacket (Packet * pPacket)
{
	m_pOriginalPacket = NULL;
}

void TWiCEPacketPoolEntry::setCreationTime(struct timeval * pTime) {
	if (pTime != NULL) {
		m_CreationTime = *pTime;
	}
}

void TWiCEPacketPoolEntry::setLastUsedTime (struct timeval * pTime) {
	if(pTime != NULL) {
		m_LastUsed = *pTime;
	}
}

unsigned long TWiCEPacketPoolEntry::getPoolID()
{
	return m_lUniquePoolID;
}

void TWiCEPacketPoolEntry::setPoolID(unsigned long ulID)
{
	m_lUniquePoolID = ulID;
}


void TWiCEPacketPoolEntry::dumpNestedDebug() {
	printf("** Nested debug for the pool entry at 0x%X\n", this);
	cout << "    Packet info: ";
	printf("0x%X", m_pPacket);

	if (m_pPacket != NULL) {
		printf("  %d bytes\n", m_pPacket->getLength());
	} else {
		printf("\n");
	}

	printf("      Referenced %d times\n", m_nRefs);
	printf("       Created: ");
	displayTime(getCreationTime());
	printf("\n");
}

Packet * TWiCEPacketPoolEntry::getPacket() {
	return m_pPacket;
}

void TWiCEPacketPoolEntry::setPacket(Packet * pPacket) {
	m_pPacket = pPacket;
}

void TWiCEPacketPoolEntry::incReference(TWiCECacheEntry * pEntry) {

	dumpNestedDebug();

	printf("      Pushing back ....\n");

	m_Refs.push_back(pEntry);

	printf("      Increment ....\n");

	m_nRefs++;
}

bool TWiCEPacketPoolEntry::checkUniqueReferences() {
	// TODO: Fill in code for verifying uniqueness

	return true;
}

int TWiCEPacketPoolEntry::getReferences() {
	return m_nRefs;
}

void TWiCEPacketPoolEntry::decReference(TWiCECacheEntry * pEntry) {
	for (int j = 0; j < m_Refs.size(); j++) {
		if (m_Refs[j] == pEntry) {
			m_Refs.erase(m_Refs.begin() + j);
			break;
		}
	}

	m_nRefs--;
}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

TWiCEPacketPool::TWiCEPacketPool() {
	m_ulNextPoolID = 1;
	m_ulYouthTime = 1000000;
}

///////////////////////////////////////////////////////////

TWiCEPacketPool::~TWiCEPacketPool() {

}

///////////////////////////////////////////////////////////

TWiCEPacketPoolEntry * TWiCEPacketPool::findByPacket(Packet * pPacket) {
	int j;

	for (j = 0; j < m_pEntries.size(); j++) {
		if (m_pEntries[j]->getPacket() == pPacket) {
			return m_pEntries[j];
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////

TWiCEPacketPoolEntry * TWiCEPacketPool::findByPoolID(unsigned long ulPoolID) {
	int j;

	for (j = 0; j < m_pEntries.size(); j++) {
		if (m_pEntries[j]->getPoolID() == ulPoolID) {
			return m_pEntries[j];
		}
	}

	return NULL;
}

// Create a pool entry
TWiCEPacketPoolEntry * TWiCEPacketPool::createCloneInPool (Packet * pPacket) {
	Packet * pClonedPacket;

//	m_Stats.Add_Stat(TWICE_GATEWAY_WAN_WCACHE_CLONE_PKT, 1);

	// This creates a new packet from the pool that we can use
	pClonedPacket = pPacket->cloneFromPool();

	if(pClonedPacket == NULL)
	{
		printf("* Error: Unable to clone the packet, pool allocation failed\n");
		return NULL;
	}

	// Create a new packet pool entry to use
	TWiCEPacketPoolEntry * 	pPoolEntry;

	// TODO: Eventually, this should be a memory pool object but for now, dynamic allocation is OK
	//       even if we will end up paying a per-packet allocation cost
	pPoolEntry = new TWiCEPacketPoolEntry();

	// This is a bit dangerous and really is just for debugging.  We will technically
	//  use this later for a tripwire to avoid the default cloning behavior.  That code
	//  will be a bit tricky.
	pPoolEntry->setOriginalPacket(pPacket);

	pPoolEntry->setPacket(pClonedPacket);
	pPoolEntry->setCreationTime(pClonedPacket->getArrivalTime());

	// Set the last used time to be the same as the creation time
	pPoolEntry->setLastUsedTime(pClonedPacket->getArrivalTime());

	// Lock things as now we are going to manipulate things for real
	pthread_mutex_lock(getMutex());

	// Set the entry to be unique
	pPoolEntry->setPoolID(m_ulNextPoolID);
	m_ulNextPoolID++;

	// At this point, one can do whatever with pPacket.  Note that we have
	//  a clone saved, keeping track of pPacket is still someone's responsibility
	m_pEntries.push_back(pPoolEntry);

	// Unlock the mutex now that we are done with manipulating things
	pthread_mutex_unlock(getMutex());

	return pPoolEntry;
}




// Take care of linking that entry in with the packet pool
char TWiCEPacketPool::linkCacheEntry(TWiCECacheEntry * pCacheEntry,
		Packet * pPacket) {
	TWiCEPacketPoolEntry * pSearchPool;
	int nError;

	// Does this packet exist in the pool?
//	cout << "   ** Waiting for the shiny mutex" << endl;
	nError = pthread_mutex_lock(getMutex());
//	cout << "   ** Got the shiny mutex" << endl;

	if (nError != 0) {
		cerr << "Error: Mutex lock failed in TWiCEPacketPool::linkCacheEntry"
				<< endl;
		cerr << "  Error code was " << nError << endl;
		cerr << "  Bad things are probably going to happen just as a FYI"
				<< endl;
	}

	pSearchPool = findByPacket(pPacket);

	if (pSearchPool != NULL) {
		//cout << "* Hit on an existing packet already in the packet pool" << endl;

		// Nothing to do
	} else {
		//cout << "* Creating a new entry with attributes of " << endl;
		//cout << "    Length = " << pPacket->getLength() << endl;

		// Make a new packet pool object
		pSearchPool = new TWiCEPacketPoolEntry();
		pSearchPool->setPacket(pPacket);
		pSearchPool->setCreationTime(pPacket->getArrivalTime());
		m_pEntries.push_back(pSearchPool);

		// TODO: Populate creation time / etc.
	}

	// Adjust the packet pool entry to catch that we are pointing to this particular
	//  cache entry
	pSearchPool->incReference(pCacheEntry);

	// Adjust the cache entry so it knows whom it is pointing to for when it gets
	//  evicted from the cache such that we can decrease / remove the references
	pCacheEntry->setPoolReference(pSearchPool);

	pthread_mutex_unlock(getMutex());
	return 1;
}

///////////////////////////////////////////////////////////////////////////////

char TWiCEPacketPool::dumpStatus() {
	cout << "TWiCE Gateway Packet Pool Status" << endl;
	cout << "  Entries: " << m_pEntries.size() << endl;
	cout << "  Next ID: " << m_ulNextPoolID << endl;
	return 1;
}

///////////////////////////////////////////////////////////////////////////////

char TWiCEPacketPool::checkPoolPurge(TWiCEPacketPoolEntry * pPoolEntry) {
	int j;

	pthread_mutex_lock(getMutex());

	for (j = 0; j < m_pEntries.size(); j++) {
		if (m_pEntries[j] == pPoolEntry) {
			break;
		}
	}

	if (j >= m_pEntries.size()) {
		cerr << "* Error: Could not find the entry to purge" << endl;
		pPoolEntry->dumpNestedDebug();
		pthread_mutex_unlock(getMutex());
		return 0;
	}

	// If the number of references is zero, get rid of it
	if (m_pEntries[j]->getReferences() <= 0) {
		// Successful??
		//pPoolEntry->dumpNestedDebug();

		// OK, we know which entry we are
		m_pEntries[j]->getPacket()->release();
		m_pEntries[j]->setPacket(NULL);
		delete m_pEntries[j];
		m_pEntries.erase(m_pEntries.begin() + j);
	}

	pthread_mutex_unlock(getMutex());

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

char TWiCEPacketPool::initialize() {
	// Creat the mutex
	pthread_mutex_init(&m_MutexPacketPool, NULL);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

pthread_mutex_t * TWiCEPacketPool::getMutex() {
	return &m_MutexPacketPool;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long TWiCEPacketPool::getTime_TooYoung ()
{
	return m_ulYouthTime;
}

///////////////////////////////////////////////////////////////////////////////

void TWiCEPacketPool::setTime_TooYoung (unsigned long lTooYoung)
{
	m_ulYouthTime = lTooYoung;
}

///////////////////////////////////////////////////////////////////////////////

void TWiCEPacketPool::doMaintenance() {
	// Iterate through the listing and check for anything that has zero references

	int j;

	pthread_mutex_lock(getMutex());

	for (j = 0; j < m_pEntries.size(); j++) {
		// Determine the entry age

		struct timeval cTime;

		// What is the current time?
		//
		// TODO: Allow time to be virtual instead of absolute for the purposes of playback
		gettimeofday(&cTime, NULL);

		unsigned int lTime;

		lTime = calcTimeDiff(&cTime, m_pEntries[j]->getCreationTime());

		if (lTime < getTime_TooYoung()) {
			// Entry is too young to purge, will just have to wait a bit.  We might just be in the process
			//  of building this entry, need to give it time to grow / improve
			continue;
		}

		// No references and it is too old
		if (m_pEntries[j]->getReferences() <= 0) {
			cout << "*(*#($* --> Found something that needs to be purged, get on it" << endl;
			m_pEntries[j]->dumpNestedDebug();
			//cout << "  Total references is " << m_pEntries[j]->getReferences() << endl;

			// Well, perhaps you should free that packet to the pool, just maybe

			// Release the packet (the one that we cloned)
			m_pEntries[j]->getPacket()->release();
			m_pEntries[j]->setPacket(NULL);

			// Delete the entry
			delete m_pEntries[j];
			m_pEntries.erase(m_pEntries.begin() + j);

			cout << "*(*#($* --> Done deleting it, get on it" << endl;

			j--;
		}
	}

	pthread_mutex_unlock(getMutex());
}

///////////////////////////////////////////////////////////////////////////////

int TWiCEPacketPool::findRightBound(Packet * pInPacket,
		TWiCECacheEntry * pCacheHit, int nStartHit, int nWindowSize) {
	int nOffsetPastWindow;
	char * pDataComp;
	int nPossibleComp;
	int nPoolPossibleComp;

	// OK, we know that we had a hit equivalent to the window size.  Start from there and keep going
	pDataComp = pInPacket->getData() + nStartHit + nWindowSize;

	// How many possible bytes remain?
	nPossibleComp = pInPacket->getLength() - nStartHit - nWindowSize;

//	cout << "     " << nPossibleComp << " bytes remain for a possible contiguous match" << endl;

	// Get the overall packet pool reference
	char * pPoolData;

	pPoolData = pCacheHit->resolveDataReference();
	pPoolData += nWindowSize;

	// Do we have a good pool reference?
	//pCacheHit->dumpDebug();

	nPoolPossibleComp = pCacheHit->getPoolReference()->getPacket()->getLength()
			- pCacheHit->getOffset();

	if (nPoolPossibleComp < nPossibleComp) {
//		cout << "   Heads up, we are limited by the cached / pooled packet, not the inbound packet" << endl;
		nPossibleComp = nPoolPossibleComp;

//		cout << "     Possible pool comp is " << nPossibleComp << endl;

		if (nPossibleComp < 0) {
			// Hmmm, worst case is that we should be zero
//			cout << "  Length of packet in pool: " << pCacheHit->getPoolReference()->getPacket()->getLength() << endl;
//			cout << "   Cache Offset:            " << pCacheHit->getOffset();
		}
	}

	// How much is remaining in the pool for comparison?

	nOffsetPastWindow = 0;

	while (nPossibleComp > 0) {
		if (pDataComp[nOffsetPastWindow] == pPoolData[nOffsetPastWindow]) {
			nOffsetPastWindow++;
		} else {
			break;
		}

		nPossibleComp--;
	}

	// Compute the right edge of the overall hit and include the windows such that we have an effective
	//  length of the match starting from the identified window
	return nOffsetPastWindow + nWindowSize;
}

