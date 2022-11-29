
#include <iostream>
using namespace std;

#include "PktSigXOR.h"
#include "Packet.h"

#include "../mem/MemoryPool.h"

PktSigXOR::PktSigXOR () : PacketSignature (MEMPOOL_OBJ_PKTSIGXOR) {
	
}

PktSigXOR::~PktSigXOR () {
	
	
}

char	PktSigXOR::computeSignature (SignatureRequest * pReq, Packet * pPacket) {

	if(!setRequest(pReq)) {
		return 0;	
	}
	
	int	*		pDataPtr;
	int			nResult;
	int			j;
		
	nResult	=	0;

	pDataPtr	= (int *) (pPacket->getData()+getOffset());

	// Zero pad it
	pPacket->zeroPad(sizeof(int));

	
	for(j=0; j<pPacket->getLength(); j+=sizeof(int)) {
		nResult = nResult ^ *(pDataPtr);
		pDataPtr++;
	}

	m_nChecksum = nResult;

	printf("Checksum: 0x%X\n", m_nChecksum);

	pPacket->addSignature(this);

	return 1;
}
	
void PktSigXOR::clean () {
	
}



PktSigXOR *		computeXOR (SignatureRequest * pReq, Packet * pPacket) {
	PktSigXOR	*		pSig;
	
	// Try to get a packet signature from the memory pool
	pSig = (PktSigXOR *) g_MemPool.getObject(MEMPOOL_OBJ_PKTSIGXOR);
		
	if(pSig == NULL) {
		cerr << "Error: Could not allocate a new XOR signature, aborting signature generation" << endl;
		return NULL;
	}
	
	if(!pSig->computeSignature(pReq, pPacket)) {
		return NULL;	
	}
		
	return pSig;
}
