#ifndef PARENTCACHEMODULE_H_
#define PARENTCACHEMODULE_H_

#include "../pkt/IOModule.h"
#include "../pkt/MultiIFModule.h"

class		PktCacheTable;

enum		ChildPktTaps {
	CHILDPKTCACHE_TAP_NONE=0,
	CHILDPKTCACHE_TAP_WAN,			// Traffic coming from the source (WAN-side)
	CHILDPKTCACHE_TAP_CLIENT		// Traffic from the client outwards
};

/** Packet cache on the parent side (egress side). The egress side
 * refers to the fact that this module sits on the adapter-side where
 * traffic is primarily outbound to downstream clients.  The egress
 * module assists with identifying content that is amenable to caching
 * and then tokenizing said traffic.
 * 
 */
class ParentPktCacheModuleEgress : public IOModule {
	public:
		ParentPktCacheModuleEgress ();
		~ParentPktCacheModuleEgress ();
		
		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();			
		
		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		char		processPacket (Packet * pPacket);		
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);		
		
	private:
		PktCacheTable	*		m_pPktCache;
	
};

/** Packet cache on the parent side (ingress side). The ingress refers
 * to the fact that this module sits on the inbound channel to the packet
 * cache and intercepts control messages bound for the packet cache
 * from downstream child caches (cache misses, discovery responses, etc)
 */
 
class ParentPktCacheModuleIngress : public IOModule {
	public:
		ParentPktCacheModuleEgress ();
		~ParentPktCacheModuleEgress ();
		
		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();			
		
		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		char		processPacket (Packet * pPacket);		
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);		
		
	private:
		PktCacheTable	*		m_pPktCache;
	
};


#endif /*PARENTCACHEMODULE_H_*/
