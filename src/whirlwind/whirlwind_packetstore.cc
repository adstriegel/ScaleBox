/**
 * whirlwind_packetstore.cc
 * Created on Aug, 2015
 * Author: Xuheng Hu (xhu2@nd.edu)
 */

#include <iostream>
using namespace std;

#include "../util/ip-utils.h"
#include "whirlwind_packetstore.h"

Whirlwind_PacketStoreEntry::Whirlwind_PacketStoreEntry () {
	m_pPacket = NULL;
	m_nRefs = 0;
}

Whirlwind_PacketStoreEntry::~Whirlwind_PacketStoreEntry () {
	if (m_pPacket != NULL) {
		m_pPacket->release();
		m_pPacket = NULL;
	}
}

Packet* Whirlwind_PacketStoreEntry::getPacket () {
    return m_pPacket;
}

void Whirlwind_PacketStoreEntry::setPacket (Packet* pPacket) {
    m_pPacket = pPacket;
}

struct timeval* Whirlwind_PacketStoreEntry::getTimeCreation () {
    return &m_TimeCreation;
}

void Whirlwind_PacketStoreEntry::setTimeCreation (struct timeval* pTime) {
    if (pTime != NULL) {
        m_TimeCreation = *pTime;
    }
}

struct timeval* Whirlwind_PacketStoreEntry::getTimeLastUsed () {
    return &m_TimeLastUsed;
}

void Whirlwind_PacketStoreEntry::setTimeLastUsed (struct timeval* pTime) {
    if(pTime != NULL) {
        m_TimeLastUsed = *pTime;
    }
}

unsigned int Whirlwind_PacketStoreEntry::getStoreID () {
    return m_nUniqueStoreID;
}

void Whirlwind_PacketStoreEntry::setStoreID (unsigned int nID) {
	if (nID >= 0) {
        m_nUniqueStoreID = nID;
	} else {
		cerr << "    Illegal packet store ID: " << nID << endl;
	}
}

unsigned int Whirlwind_PacketStoreEntry::getStoreIndex () {
	return m_nStoreIndex;
}

void Whirlwind_PacketStoreEntry::setStoreIndex (unsigned int nIndex) {
	if (nIndex >= 0) {
		m_nStoreIndex = nIndex;
	} else {
		cerr << "    Illegal packet store Index: " << nIndex << endl;
	}
}

void Whirlwind_PacketStoreEntry::incReferences () {
    m_nRefs++;
}

void Whirlwind_PacketStoreEntry::decReferences () {
	if (m_nRefs > 0) {
        m_nRefs--;
	} else {
		cerr << "    Error: decRefs - Store entry already has 0 refs" << endl;
	}
}

int Whirlwind_PacketStoreEntry::getReferences () {
    return m_nRefs;
}

void Whirlwind_PacketStoreEntry::dumpNestedDebug () {
    printf("**Whirlwind_PacketStoreEntry - Nested debug for the entry at 0x%p\n", this);
    cout << "    Packet info: ";
    printf("0x%p", m_pPacket);

    if (m_pPacket != NULL) {
        printf("  %d bytes\n", m_pPacket->getLength());
    } else {
        printf("\n");
    }

    printf("      Referenced %d times\n", m_nRefs);
    printf("      Created at: ");
    displayTime(getTimeCreation());
    cout << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////
Whirlwind_PacketStore::Whirlwind_PacketStore () {
	m_nMaxStoreSize = WW_PKTSTORE_DEFAULT_MAX_SIZE;
	for (int i = 0; i < m_nMaxStoreSize; i++) {
		m_pEntries[i] = NULL;
	}

    m_nNextStoreID = -1;
    m_ulYouthTime = 1000000;
}

Whirlwind_PacketStore::~Whirlwind_PacketStore () {
	for (int i = 0; i < m_nMaxStoreSize; i++) {
        delete m_pEntries[i];
        m_pEntries[i] = NULL;
    }
}

char Whirlwind_PacketStore::initialize () {
	// Create the mutex
    pthread_mutex_init(&m_MutexPacketStore, NULL);
    return 1;
}

unsigned int Whirlwind_PacketStore::getMaxStoreSize () {
	return m_nMaxStoreSize;
}

void Whirlwind_PacketStore::setMaxStoreSize (unsigned int nSize) {
	m_nMaxStoreSize = nSize;
}

int Whirlwind_PacketStore::getNumEntries () {
	return m_nEntries;
}

char Whirlwind_PacketStore::dumpStatus () {
    cout << "Whirlwind Gateway Packet Store Status" << endl;
    cout << "    Entries: " << getNumEntries() << endl;
    cout << "    Next ID: " << m_nNextStoreID << endl;
    return 1;
}

void Whirlwind_PacketStore::doMaintenance () {
	// Iterate through the entries and check for anyone that has zero reference
	for (int i = 0; i < m_nMaxStoreSize; i++) {
		// Determine the age of this entry
		struct timeval cTime;
		gettimeofday(&cTime, NULL);

		unsigned int lTime;
		lTime = calcTimeDiff(&cTime, m_pEntries[i]->getTimeCreation());

		// Still young
		if (lTime < getTime_TooYoung()) {
			continue;
		}

		// No reference and it is too old
		if (m_pEntries[i]->getReferences() <= 0) {
			cout << "*(*#($* --> Found something that needs to be purged, get on it" << endl;
            m_pEntries[i]->dumpNestedDebug();
			purgeStoreEntry(i);

            cout << "*(*#($* --> Done deleting it, get on it" << endl;
		}
	}
}

// Create a sotre entry
Whirlwind_PacketStoreEntry* Whirlwind_PacketStore::createStoreEntry (Packet* pPacket) {
	// Create a new packet from the global mem pool so that we can use it
	Packet* pClonedPacket;
	pClonedPacket = pPacket->cloneFromPool();

	if (pClonedPacket == NULL) {
		cerr << "* Error: Unable to clone the packet, mem pool allocation failed." << endl;
		return NULL;
	}

	// Lock things as now we are going to manipulate things for real
    pthread_mutex_lock(getMutex());

    // Figure out the index                                                                                                                                                    
    m_nNextStoreID++;
    unsigned int nEntry = m_nNextStoreID % m_nMaxStoreSize;

	if (m_pEntries[nEntry] != NULL) {
		purgeStoreEntry(nEntry);
	}

	// Create a new packet store entry to use
	Whirlwind_PacketStoreEntry* pStoreEntry;
    // TODO: Eventually, this should be a memory pool object but for now, dynamic allocation is OK
    //       even if we will end up paying a per-packet allocation cost
    pStoreEntry = new Whirlwind_PacketStoreEntry();

    pStoreEntry->setPacket(pClonedPacket);
    pStoreEntry->setTimeCreation(pClonedPacket->getArrivalTime());
    pStoreEntry->setTimeLastUsed(pClonedPacket->getArrivalTime());
	pStoreEntry->setStoreID(m_nNextStoreID);
	pStoreEntry->setStoreIndex(nEntry);

    m_pEntries[nEntry] = pStoreEntry;
	m_nEntries++;

    // Unlock the mutex now that we are done with manipulating things
    pthread_mutex_unlock(getMutex());

    return m_pEntries[nEntry];
}

int Whirlwind_PacketStore::purgeStoreEntry (int nEntry) {
	if (m_pEntries[nEntry] == NULL) {
		return -1;
	}

	delete m_pEntries[nEntry];
	m_pEntries[nEntry] = NULL;

	m_nEntries--;

	return 0;
}

void Whirlwind_PacketStore::incEntryRefs (unsigned int nEntry) {
	if (nEntry < 0 || nEntry > m_nMaxStoreSize) {
		cerr << "    Error: Illegal entry index for incEntryRefs" << endl;
	}

	if (m_pEntries[nEntry] != NULL) {
		m_pEntries[nEntry]->incReferences();
	}
}

void Whirlwind_PacketStore::decEntryRefs (unsigned int nEntry) {
	if (nEntry < 0 || nEntry > m_nMaxStoreSize) {
		cerr << "    Error: Illegal entry index for decEntryRefs" << endl;
	}

	if (m_pEntries[nEntry] != NULL) {
		m_pEntries[nEntry]->decReferences();

		if (m_pEntries[nEntry]->getReferences() <= 0) {
			pthread_mutex_lock(getMutex());
			purgeStoreEntry(nEntry);
			pthread_mutex_unlock(getMutex());
		}
	}
}

unsigned long Whirlwind_PacketStore::getTime_TooYoung () {
	return m_ulYouthTime;
}

void Whirlwind_PacketStore::setTime_TooYoung (unsigned long lTooYoung) {
	m_ulYouthTime = lTooYoung;
}

pthread_mutex_t* Whirlwind_PacketStore::getMutex () {
	return &m_MutexPacketStore;
}
