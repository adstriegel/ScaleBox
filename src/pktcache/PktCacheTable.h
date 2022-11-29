/* PktCacheTable.h : Definition code for the Timestamp object 
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
 * Development and equipment support has been provided in
 *  part by the following sponsors:
 * 
 *    National Science Foundation    CNS03-47392
 *    Sun Microsystems Academic Excellence Grant
 *    Intel IXP Development Grant 
 *    University of Notre Dame Faculty Grant
 * 
 **********************************************************
 */

#ifndef PKTCACHETABLE_H_
#define PKTCACHETABLE_H_

#include <vector>
using namespace std;

// Base definition of a packet signature
#include "../pkt/PacketSignature.h"

// Timestamp for the last usage of the packet signature
#include "../core/Timestamp.h"


/** An entry in the packet cache table that pertains to a specific
 * signature
 */
class PktCacheEntry {
	public:
		PktCacheEntry ();
		~PktCacheEntry ();
		
		/** Determine the hash location of this specific entry
		 * @param lHashSpace The hash table size
		 * @returns The hash of where to find this entry
		 */
		unsigned long		hashToLocation (unsigned long lHashSpace);
		
		
		Timestamp *			getLastUsed ();
		
		/** Retrieve the number of times that this entry has resolved
		 * to a hit
		 * @returns The number of hits on this entry
		 */
		unsigned long		getNumHits ();
		
		/** Increment the number of hits on this particular entry
		 */
		void				incNumHits ();
		
		/** Retrieve the number of false hits that this entry has been resolved to
		 * @returns The number of hits that were false (collision versus hit)
		 */
		unsigned long		getNumFalseHits ();
		
		/** Increment the number of false hits on this cache entry
		 */
		void				incNumFalseHits ();
		
		/** Set the signature associated with this entry 
		 * @param pSignature A pointer to a valid PacketSignature object
		 * @returns True if successful, false otherwise 
		 */

		char	setSignature (PacketSignature * pSignature);
		
		/** Retireve the signature associated with this entry
		 * @returns The pointer to the packet signature associated with this entry
		 */
		PacketSignature	*	getSignature ();
		
	private:
		PacketSignature	*		m_pSignature;	
		
		// Last hit on this entry
		Timestamp				m_LastUsed;
		
		// Number of times that a hit has resolved to this entry
		unsigned long			m_lHits;
		
		// Number of times that the resolution was false (collision)
		unsigned long			m_lFalseHits;
		
};

/** The base class for the packet cache table that simply serves
 * up the requested data as appropriate 
 */
class PktCacheTable {
	public:
		PktCacheTable ();
		~PktCacheTable ();
		
		
		/** Look into the packet cache table to see if the signature exists
		 * in the table. 
		 * @param pSig		The packet signature to look for (should be a valid object)
		 * @returns 	NULL if no signature was find or the signature is invalid,
		 * 				Non-NULL (valid pointer) if the signature was found in the
		 * 				table 
		 */
		PktCacheEntry * 		lookup (PacketSignature * pSig);
		
		/** Add an entry to the packet cache table and if necessary, apply the
		 * appropriate cache replacement algorithm. The provided entry is assumed
		 * to be under the ownership of the packet cache table and will no longer
		 * be considered valid for the callee to interact with after this function
		 * is called.  
		 * @param pEntry The new entry to add
		 * @returns 0 if the entry was added without issue, 1 if the entry was
		 *  		   added but a replacement was forced, -1 if the entry itself
		 *  			could not be added to the cache and the entry (and its contents)
		 * 			were freed back to the pool.
		 */
		int			addEntry (PktCacheEntry * pEntry);
		
		/** Set the maximum number of entries for the table. A value of 0 specifies
		 * that the table is not constrained by this value
		 * @param lMaxEntries The new value for max entries
		 */
		void			setMaxEntries (unsigned long lMaxEntries);
		
		/** Get the maximum number of entries allowed for the table. A value of 0 specifies
		 * that the table is not constrained by this value
		 * @returns An unsigned long
		 */
		unsigned long	getMaxEntries ();  		

		/** Get the number of entries currently contained within the packet
		 * cache
		 * @returns A valid unsigned long that is the raw number of entries (unique
		 *  hashes stored) in the table
		 */
		unsigned long	getNumEntries ();
		
		/** Set the maximum size of all data for the table. For now, this restriction
		 * is only on the size of the data itself, not the size of the actual
		 * hash values.
		 * @param lMaxSize	New maximum size of the table data (0 if not constrianed)
		 */
		void			setMaxSize (unsigned long lMaxSize);
		
		/** Get the maximum size allowed for all data in the table. For now, this restriction
		 * is only on the size of the data itself, not the size of the actual
		 * hash values.
		 * @returns The maximum size of the table data (in bytes) that the table
		 *    is allowed to grow to before replacement occurs
		 */
		unsigned long	getMaxSize ();
		
		/** Get the total size consumed by data in the cache (excluding the data structures)
		 * @returns The number of bytes consumed by packet data and signatures
		 */
		unsigned long	getCurrentCacheSize (); 
		
	private:
		// The maximum number of entries in the table (0 if not constrained)
		unsigned long			m_lMaxEntries;
		
		// The maximum size of the table allowed (in bytes)
		unsigned long			m_lMaxTableSize;
};

/** Default number of tables for the hash manager */
#define PKTCACHE_DEFAULT_NUM_TABLES			1


/** The outer manager class that contains multiple tables (or a single one) for managing
 * the list of hash tables
 */
class PktCacheHashMgr {
	public:
		PktCacheHashMgr ();
		~PktCacheHashMgr ();
		
		
		/** Fix the number of underlying hash tables to a certain value. The tables will either be
		 * grown or shrunk appropriately
		 * @param nTables The new number of tables (>= 1)
		 * @returns Non-zero (true) if successful, zero (false) otherwise
		 */
		char		setNumberTables (int nTables);
		
		/** Retrieve the number of underyling hash tables at this hash manager. The default
		 * value is 1 (single hash table). 
		 * @returns The number of hash tables
		 */
		int			getNumberTables ();
		
		
		/** Retrieve a specific packet cache table
		 */
		PktCacheTable *		getTable (int nTable);
		
	private:
		vector <PktCacheTable *>	m_Tables;
	
		int			m_nNumTables;
};

#endif /*PKTCACHETABLE_H_*/




