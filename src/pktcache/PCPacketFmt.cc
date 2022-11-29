
#include "../util/ip-utils.h"
#include "PCPacketFmt.h"

///////////////////////////////////////////////////////////////////

bool		isPacketCacheProtocol (char * pData) {
	if(((unsigned char) pData[OFFSET_IPV4_PROTOCOL]) == PROTO_PCACHE) {
		return true;
	} else {
		return false;
	}
}

///////////////////////////////////////////////////////////////////

int	getPacketCachePktType (char * pData) {
	return (unsigned char) (pData[PCACHEHDR_OFFSET_TYPE]);	
}
