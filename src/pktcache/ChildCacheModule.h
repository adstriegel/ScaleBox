#ifndef CHILDCACHEMODULE_H_
#define CHILDCACHEMODULE_H_

#include "../pkt/IOModule.h"
#include "../pkt/MultiIFModule.h"
#include "PktCacheTable.h"

#include <vector>
using namespace std;

class		PktCacheTable;
class		ChildPktCache;

enum		ChildPktTaps {
	CHILDPKTCACHE_TAP_NONE=0,
	CHILDPKTCACHE_TAP_WAN,			// Traffic coming from the source (WAN-side)
	CHILDPKTCACHE_TAP_CLIENT		// Traffic from the client outwards
};

/** The class to manage the packet cache from a unified perspective. The manager
 * will have at least one module (packet tap) feeding into it for the ingres /
 * inbound traffic and an optional module on the egress side (client-side) if
 * TCP ACK servicing is desired.
 */
class ChildPktCache : public MultiIFModule {
	public:
		ChildPktCache ();
		~ChildPktCache ();
	
		/** Retrieve a pointer to the packet cache table
		 * @returns A valid pointer to a PktCacheTable object
		 */
		PktCacheTable *		getCache ();

		/** Given the name of a tap module, map that module name to a specific
		 * ID that will be used whenever packets are seen on that particular
		 * tap. This function must be overridden to encapsulate the behavior
		 * how the names map to IDs.
		 * @param sName The name of the module
		 * @returns The ID to use for that module where -1 is a failure in name resolution
		 */
		virtual int 	mapNameToID (string sName);	
		
		/** Process a packet from one of the underlying tap modules and
		 * appropriate process the packet to the same behavior as a processPacket
		 * call from a normal IOModule object
		 */
		virtual char	processTapPacket (Packet * pPacket, int nID);

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */					
		bool extractExtendedDOM (NodeDOM * pNode);

		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */		
		char initialize ();
		
	protected:
	
		/** Process a cache discovery request from an upstream 
		 * packet cache that wishes to start caching with us.
		 * There is no return value as the packet should be gobbled
		 * regardless of a success or failure in processing.
		 * Note: The packet passed into the function should not be
		 * considered valid after this function call.  
		 * @param pPacket A valid packet to process with a discovery message
		 */
		void	processCacheDiscovery (Packet * pPacket);
		
		/** Process a data packet with tokenized content and assess
		 * if the packet itself should de-tokenized before being
		 * transmitted downstream.
		 * Note: The packet passed into the function should not be
		 * considered valid after this function call.		 
		 * @param pPacket A valid packet that is requesting de-tokenization
		 */ 
		virtual void	processTokenizedPacket (Packet * pPacket);

		/** Process an update as pushed out by the source cache in
		 * a pro-active manner. The theory would be that the new
		 * content should replace old content in that the new content
		 * has a high likelihood of generating future cache hits.
		 * Note: The packet passed into the function should not be
		 * considered valid after this function call.   
		 * @param pPacket A valid packet containing a cache update
		 */
		virtual void	processCacheDataUpdate (Packet * pPacket);


		/** Create a data packet for dispatch based on the original source
		 * packet and the hit in the cache table
		 * @param pSourcePkt The original source packet
		 * @param nEntry	 The token relation to the source packet (for TCP computations and unique data)
		 * @param pCacheHit  The hit in the cache table
		 * @returns True if the data packet creation was successful, false otherwise
		 */
		virtual bool 	createDataFromToken (Packet * pSourcePkt, int nEntry, PktCacheEntry * pCacheHit);

		/** Request one or more pieces of data from the source cache
		 * as the data did not exist in the cache table (cache miss). 
		 * @param pMissList	A vector of PacketSignature pointers that are the exact list to request
		 * @param pSrcPkt	The source of the misses for figuring out whom to talk to (or towards)
		 * @returns True if sucessful, false otherwise with errors logged by the function
		 */
		virtual bool	requestTokenDataFromSource (vector<PacketSignature *> * pMissList, Packet * pSrcPkt);		

	private:
		PktCacheTable	*		m_pPktCache;	
			
};


#endif /*CHILDCACHEMODULE_H_*/

