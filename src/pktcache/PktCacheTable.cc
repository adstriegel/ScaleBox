
#include <iostream>
using namespace std;

#include "PktCacheTable.h"

///////////////////////////////////////////////////////

PktCacheEntry::PktCacheEntry () {
	m_pSignature = NULL;
	m_lHits = 0;
	m_lFalseHits = 0;	
}

///////////////////////////////////////////////////////

Timestamp *	PktCacheEntry::getLastUsed () {
	return &m_LastUsed;
}

///////////////////////////////////////////////////////

unsigned long PktCacheEntry::getNumHits () {
	return m_lHits;	
}

///////////////////////////////////////////////////////

void PktCacheEntry::incNumHits () {
	m_lHits++;
}

///////////////////////////////////////////////////////

unsigned long PktCacheEntry::getNumFalseHits () {
	return m_lFalseHits;
}

///////////////////////////////////////////////////////

void PktCacheEntry::incNumFalseHits () {
	m_lFalseHits++;
}

///////////////////////////////////////////////////////

char	PktCacheEntry::setSignature (PacketSignature * pSignature) {
	if(pSignature != NULL) {
		m_pSignature = pSignature; 
		return 1;
	} else {
		return 0;
	}
}

///////////////////////////////////////////////////////

PacketSignature	*	PktCacheEntry::getSignature () {
	return m_pSignature;
}

///////////////////////////////////////////////////////

PktCacheTable::PktCacheTable () {
	m_lMaxEntries = 0;		// Not constrained
	m_lMaxTableSize = 0;			// Not constrained
}



PktCacheTable::~PktCacheTable () {
	
}


PktCacheEntry * PktCacheTable::lookup (PacketSignature * pSig) {
	return NULL;	
}

int  PktCacheTable::addEntry (PktCacheEntry * pEntry) {
	return -1;	
}

void PktCacheTable::setMaxEntries (unsigned long lMaxEntries) {
	m_lMaxEntries = lMaxEntries;	
}

		
unsigned long PktCacheTable::getMaxEntries () {
	return m_lMaxEntries;
}

unsigned long PktCacheTable::getNumEntries () {
	return 0;
}
		
void	PktCacheTable::setMaxSize (unsigned long lMaxSize) {
	m_lMaxTableSize = lMaxSize;		
}
		
unsigned long	PktCacheTable::getMaxSize () {
	return m_lMaxTableSize;	
}
		
unsigned long	PktCacheTable::getCurrentCacheSize () {
	return 0;	
}


PktCacheHashMgr::PktCacheHashMgr () {
	m_nNumTables = PKTCACHE_DEFAULT_NUM_TABLES;
	
}


PktCacheHashMgr::~PktCacheHashMgr () {
	
}

char PktCacheHashMgr::setNumberTables (int nTables) {
	cout << "To do: Add functionality later" << endl;
	return 1;
}


