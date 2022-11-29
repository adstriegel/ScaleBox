#ifndef PACKETGROUP_H_
#define PACKETGROUP_H_

#include "../mem/MemPoolObject.h"

/** A group of packets 
 */
class PacketGroup : public MemPoolObject {
	public:
		PacketGroup ();
		~PacketGroup ();	
	
};

#endif /*PACKETGROUP_H_*/
