

#include <iostream>
using namespace std;

#include "PacketSignature.h"

PacketSignature::PacketSignature (int nType) : MemPoolObject (nType) {
	
}

PacketSignature::~PacketSignature () {
	
}

int	PacketSignature::getOffset () {
	return m_Request.m_nOffset;
}


char	PacketSignature::setRequest (SignatureRequest *  pReq) {
	if(pReq != NULL) {
		memcpy(&m_Request, pReq, sizeof(struct SignatureRequest));
		return 1;	
	}
	else {
		cerr << "Error: SignatureRequest struct passed into setRequest (PacketSignature.cc)" << endl;
		cerr << "   is NULL, ignoring setRequest call" << endl;	
		return 0;
	}
}
