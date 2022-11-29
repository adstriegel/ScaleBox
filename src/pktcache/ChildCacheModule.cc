/* ChildCacheModule.cc
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
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#include "../mem/MemoryPool.h"
#include "../pkt/Packet.h"
#include "../util/ip-utils.h"

#include "ChildCacheModule.h"
#include "PktCacheTable.h"

#include "PCPacketFmt.h"

////////////////////////////////////////////////////////////////////////

ChildPktCache::ChildPktCache () {
	m_pPktCache = new PktCacheTable();		
}

////////////////////////////////////////////////////////////////////////

ChildPktCache::~ChildPktCache () {
	if(m_pPktCache != NULL) {
		delete m_pPktCache;
		m_pPktCache = NULL;
	}		
}

////////////////////////////////////////////////////////////////////////

int ChildPktCache::mapNameToID (string sName) {
	if(sName == "SourceSide") {
		return CHILDPKTCACHE_TAP_WAN;
	} else if (sName == "ClientSide") {
		return CHILDPKTCACHE_TAP_CLIENT;
	} else {
		return CHILDPKTCACHE_TAP_NONE;
	}	
}

////////////////////////////////////////////////////////////////////////

bool ChildPktCache::extractExtendedDOM (NodeDOM * pNode) {
	
	// Nothing to process for now
	return true;
}

////////////////////////////////////////////////////////////////////////

char ChildPktCache::initialize () {
	return 1;
}
	
////////////////////////////////////////////////////////////////////////

char ChildPktCache::processTapPacket (Packet * pPacket, int nID) {
	int		nStartPktCacheHdr;
	
	if(nID == CHILDPKTCACHE_TAP_WAN) {
		// Packet came from the source side
		
		// Is it a normal packet or one of ours?
		if (isPacketCacheProtocol(pPacket->getData()+L2_OFFSET)) {
			nStartPktCacheHdr = L2_OFFSET+getIP_HeaderSize(pPacket->getData(), pPacket->getLength());
			
			switch(getPacketCachePktType(pPacket->getData()+nStartPktCacheHdr)) {
				case PKTCACHE_PKT_TYPE_TOKEN:
					break;
				case PKTCACHE_PKT_TYPE_MISS_RSP:	
					break;
				
			}
			
			// Is it a discovery packet?		
		
			// Is it a forced cache update?
		}
	} else if (nID == CHILDPKTCACHE_TAP_CLIENT) {
		// Packet came from the client side
	} else {
		// Unknown ID - do nothing with the packet tap as there was
		//  a configuration error in pushing it to us
		
		// Future: Need a way to flag this (fast) once and log it but not
		//  excessively spam the log
		return 0;
	}
		
	return 0;	
}

////////////////////////////////////////////////////////////////////////

		/** Process a cache discovery request from an upstream 
		 * packet cache that wishes to start caching with us.
		 * There is no return value as the packet should be gobbled
		 * regardless of a success or failure in processing.  
		 * @param pPacket A valid packet to process with a discovery message
		 */
void ChildPktCache::processCacheDiscovery (Packet * pPacket) {
	
	
	// Free the packet back to the pool
	g_MemPool.releaseObject(pPacket);
}


/** Process a data packet with tokenized content and assess
 * if the packet itself should de-tokenized before being
 * transmitted downstream
 * @param pPacket A valid packet that is requesting de-tokenization
 */ 
void ChildPktCache::processTokenizedPacket (Packet * pPacket) {
	int		nTokens;
	int		j;
	PktCacheEntry *		pCacheEntry;
	vector<PacketSignature *> 	missList;
	
	// Need an array of signatures for misses if at all possible
	//   Dynamic allocation of a Vector on the stack, bleh, probably
	//	 not the best course of action
	
	// How many tokens do we have to process?
	nTokens = 0;
	
	// For each token, attempt a lookup into the table to verify
	// that indeed, the token does exist
	for(j=0; j<nTokens; j++) {
		// Did we get a hit?
		
		pCacheEntry = m_pPktCache->lookup(NULL);
		
		if(pCacheEntry != NULL) {
			// Got a hit in the table, create the data packet for output
			
			// Assume for now that only tokens flow with the packets
			//  themselves, not unique data
			
			if(createDataFromToken(pPacket, j, pCacheEntry)) {
				// Success! Nothing to do here then
			} else {
				// Uh oh, time to bail out of processing the packet and
				//  let TCP sort it out
				
				// Spawn a note to the log to capture this
				break;
			}
		}
	}
	
	// Handle any miss requests as appropriate here
	if(missList.size() > 0) {
		// Yes, we had a few misses in the table, time to ask the
		// source to send them to us, all in bulk		
		requestTokenDataFromSource(&missList, pPacket);
	} 
	
	// Free the packet back to the pool
	g_MemPool.releaseObject(pPacket);	
}


/** Process an update as pushed out by the source cache in
 * a pro-active manner. The theory would be that the new
 * content should replace old content in that the new content
 * has a high likelihood of generating future cache hits.  
 * @param pPacket A valid packet containing a cache update
 */
void ChildPktCache::processCacheDataUpdate (Packet * pPacket) {
	
	
	// Free the packet back to the pool	
	g_MemPool.releaseObject(pPacket);
}

bool ChildPktCache::createDataFromToken (Packet * pSourcePkt, int nEntry, PktCacheEntry * pCacheHit) {
	return false;	
}

bool ChildPktCache::requestTokenDataFromSource (vector<PacketSignature *> * pMissList, Packet * pSrcPkt) {
	return false;	
}



		