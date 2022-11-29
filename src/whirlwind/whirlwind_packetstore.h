/**
 *
 */

#ifndef WHIRLWIND_PACKETSTORE_H_
#define WHIRLWIND_PACKETSTORE_H_

#include "../core/StdHeaders.h"
#include "../pkt/Packet.h"

#include <pthread.h>
#include <sys/time.h>

#include <vector>
using namespace std;

#define WW_PKTSTORE_DEFAULT_MAX_SIZE  1500000

/** 
 * An entry in the Whirlwind packet store that is effectively a keeper of the original packet
 * object until it is finally released (purged) from the store as decided by the cache replacement
 * policy (typically age-based). Conceptually, this is just a slightly bigger wrapper for the
 * original packet object capturing who / what references this particular packet object.
 **/
class Whirlwind_PacketStoreEntry {
    public:
	    Whirlwind_PacketStoreEntry ();
		~Whirlwind_PacketStoreEntry ();

        /** 
		 * Retrieve the original packet that is in the packet store
         * @returns A pointer to the packet that this entry  stores
         */
        Packet*             getPacket ();
        void                setPacket (Packet* pPacket);

        struct timeval*     getTimeCreation ();
        void                setTimeCreation (struct timeval* pTime);

        struct timeval*     getTimeLastUsed ();
        void                setTimeLastUsed (struct timeval* pTime);

		/**
         * The unique ID for the packet
         * @returns Unique ID for the base packet maintained by this store entry
         */
        unsigned int        getStoreID();

        /**
         * Set the store entry unique ID
         * @param ulID  New unique ID for the store entry
         */
        void                setStoreID(unsigned int nID);

		unsigned int        getStoreIndex ();

		void                setStoreIndex (unsigned int nIndex);

        /** 
		 * Tag this packet item as being referenced by an entry in the cache meaning
         * that the packet should not be returned to the mem pool
         */
        void                incReferences ();

        void                decReferences ();

		int                 getReferences ();

		void                dumpNestedDebug ();

    private:
		Packet*             m_pPacket;

		// Data and time (for statistics)
		struct timeval      m_TimeCreation;

		struct timeval      m_TimeLastUsed;

		unsigned int        m_nUniqueStoreID;

		unsigned int        m_nStoreIndex;

		// How many time it is referenced? 
		int                 m_nRefs;
};

/** 
 * The Whirlwind packet sotre is the collection of packets being kept at the
 * gateway. It represents a superset of all packets that the client nodes
 * might have.
 */
class Whirlwind_PacketStore {
    public:
	    Whirlwind_PacketStore ();
		~Whirlwind_PacketStore ();

        char               initialize ();

		unsigned int       getMaxStoreSize ();

		void               setMaxStoreSize (unsigned int nSize);

        /** 
		 * Retrieve the number of entries in the packet sotre
         * @returns The number of entries in the store
         */
        int                getNumEntries ();

        char               dumpStatus (); 

        // Do the maintenance on the packet store
        void               doMaintenance ();

        // Create a store entry
        // If success, return the entry index, otherwise return -1
        Whirlwind_PacketStoreEntry*  createStoreEntry (Packet* pPacket);

		int                          purgeStoreEntry (int nEntry);

		void                         incEntryRefs (unsigned int nEntry);

		void                         decEntryRefs (unsigned int nEntry);

        unsigned long                getTime_TooYoung ();

        void                         setTime_TooYoung (unsigned long lYouthTime);

    protected:
        pthread_mutex_t*             getMutex ();

    private:
        Whirlwind_PacketStoreEntry*  m_pEntries[WW_PKTSTORE_DEFAULT_MAX_SIZE];

		unsigned int                 m_nMaxStoreSize;

		unsigned int                 m_nNextStoreID;

		unsigned int                 m_nEntries;

        // Mutex for packet store entries
        pthread_mutex_t              m_MutexPacketStore;

        // When is a store entry considered too young to purge?
        unsigned long                m_ulYouthTime;
};

#endif /* WHIRLWIND_PACKETSTORE_H_ */
