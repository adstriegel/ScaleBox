/* TWiCE_PacketPool.h
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

#ifndef TWICE_PACKETPOOL_H_
#define TWICE_PACKETPOOL_H_

// Include the base headers to ensure broad compatibility
#include "../core/StdHeaders.h"

#include <vector>
using namespace std;

#include <sys/time.h>					// timeval struct
#include <pthread.h>

#include "../pkt/Packet.h"

/** The information regarding the status of a client with respect to particular blocks
 *  of data as distributed by TWiCE
 */
class TWiCEBlockClientInfo
{
	public:

	private:
		// Reference to the actual client info

		// What is the state?
		//	 Encrypted, decrypted, etc.


};

/** A particular block of data within a packet
 */
class TWiCEPacketBlockInfo
{
	public:
		TWiCEPacketBlockInfo ();
		~TWiCEPacketBlockInfo ();

		unsigned short		getOffset ();
		void				setOffset (unsigned short nOffset);

		unsigned short		getLength ();
		void				setLength (unsigned short nLength);

		Packet *			getDataPtr ();
		void				setPacket (Packet * pPacket);



	private:
		// Note that this object most certainly does not own the packet object, it is
		//  just a reference to that object for keeping things easy
		Packet	*			m_pPacket;

		// Where does this block start?
		unsigned short		m_nOffset;

		// How long is this particular block?
		unsigned short		m_nLength;

		// What is the state of the clients on this particular block of
		//  information?
		vector<TWiCEBlockClientInfo *> 	m_ClientInfo;

		// Security information for this block

		// Statistics regarding this block
		//	 When was this block last used?
		int					m_nHits;
		struct timeval 		m_LastUsedTime;
};

// For the purposes of enabling passing by a pointer
class TWiCECacheEntry;


/** An entry in the TWiCE packet pool that is effectively a keeper of the original packet
 * object until it is finally released (purged) from the pool as decided by the cache replacement
 * policy (typically age-based).  Conceptually, this is just a slightly bigger wrapper for the
 * original packet object capturing who / what references this particular packet object.
 *  */
class TWiCEPacketPoolEntry
{
	public:
		TWiCEPacketPoolEntry ();

		/** Retrieve the original packet that is in the packet pool
		 * @returns A pointer to the packet that this entry in the pool references
		 */
		Packet * getPacket ();


		void setPacket (Packet * pPacket);

		struct timeval *	getLastUsedTime ();
		void setLastUsedTime (struct timeval * pTime);

		struct timeval *	getCreationTime ();
		void setCreationTime (struct timeval * pTime);

		/** Tag this packet pool item as being referenced by an entry in the cache meaning
		 * that the packet should not be returned to the pool
		 */
		void	incReference(TWiCECacheEntry * pRef);


		void    decReference(TWiCECacheEntry * pRef);

		void	dumpNestedDebug   ();


		int		getReferences ();

		bool 	checkUniqueReferences ();

		// Packet pool ID (for forcing uniqueness)

		/** The unique ID for the packet
		 * @returns Unique ID for the base packet maintained by this pool entry
		 */
		unsigned long 	getPoolID();

		/** Set the pool entry unique ID
		 * @param ulID 	New unique ID for the pool entry and packet
		 */
		void			setPoolID(unsigned long ulID);


		void			setOriginalPacket (Packet * pOrigPacket);
		Packet * 		getOriginalPacket ();

	private:
		Packet *	m_pPacket;
		Packet * 	m_pOriginalPacket;

		// Date / time (for statistics)
		struct timeval		m_CreationTime;
		struct timeval		m_LastUsed;

		// How many times is it referenced?
		int		m_nRefs;

		vector<TWiCECacheEntry *>	m_Refs;

		// Unique ID (for hashing / locating)
		unsigned long 		m_lUniquePoolID;

};


/** The TWiCE packet pool is the collection of packets being kept at the
 * gateway.  It represents a superset of all packets that the client nodes
 * might have
 */
class TWiCEPacketPool
{
	public:
		TWiCEPacketPool ();
		~TWiCEPacketPool ();

		/** Retrieve the number of entries in the packet pool
		 * @returns The number of entries in the pool
		 */
		int		getNumEntries ();


		TWiCEPacketPoolEntry *  findByPoolID(unsigned long ulPoolID);
		TWiCEPacketPoolEntry *	findByPacket (Packet * pPacket);

		// Take care of linking that entry in with the packet pool
		char		linkCacheEntry (TWiCECacheEntry * pCacheEntry, Packet * pPacket);

		char		dumpStatus ();

		/// Compute the total width of the match in bytes
		int 		findRightBound (Packet * pInPacket, TWiCECacheEntry * pCacheHit, int nStartHit, int nWindowSize);

		char 		checkPoolPurge (TWiCEPacketPoolEntry * pPoolEntry);

		/// Do the maintenance on the packet pool
		void		doMaintenance  ();

		char		initialize ();

		TWiCEPacketPoolEntry * findByID (unsigned long ulID);


		// Create a pool entry
		TWiCEPacketPoolEntry * createCloneInPool (Packet * pPacket);


		unsigned long 			getTime_TooYoung ();
		void					setTime_TooYoung (unsigned long);

	protected:
		pthread_mutex_t *		getMutex();

	private:
		vector<TWiCEPacketPoolEntry *>	m_pEntries;

		// Mutex for packet pool entries
		pthread_mutex_t 			m_MutexPacketPool;

		unsigned long 				m_ulNextPoolID;

		// When is a pool entry considered too young to purge?
		unsigned long			    m_ulYouthTime;
};



#endif /* TWICE_PACKETPOOL_H_ */
