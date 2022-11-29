
#include "InputSignature.h"
#include "Packet.h"

#include "PktSigXOR.h"
#include "PktSigMD5.h"

InputSignature::InputSignature () : IOModule () {
	
	setType("InputSignature");	
}

InputSignature::~InputSignature () {
	
}

char		InputSignature::initialize () {
	return 1;
}			

char		InputSignature::processPacket (Packet * pPacket) {
	if(shouldIgnorePacket(pPacket)) {
		return 0;
	}
	
	computeSignatures(pPacket);
	return 0;	
}


void	InputSignature::addSignatureType (SignatureRequest theSig) {
	m_SigRequests.push_back(theSig);	
}

char 	InputSignature::shouldIgnorePacket(Packet * pPacket) {
	return 0;
}


void	InputSignature::computeSignatures (Packet * pPacket) {
	int			j;
	PacketSignature	*	pSig;
	
	cout << " Computing signatures" << endl;
	
	for(j=0; j<m_SigRequests.size(); j++) {
		switch(m_SigRequests[j].m_sigType) {
			case SIGNATURE_TYPE_XOR:
				pSig = computeXOR(&(m_SigRequests[j]),pPacket);
				break;
			case SIGNATURE_TYPE_MD5:
				pSig = computeMD5(&(m_SigRequests[j]),pPacket);				
				break;
			default:
				break;
		}
	}
}

bool InputSignature::extractExtendedDOM (NodeDOM * pNode) {
	return true;
}

