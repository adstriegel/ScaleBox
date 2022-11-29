



#include "VNetStack.h"


VNetStack::VNetStack () {
	m_pHost = NULL;
	
}

VNetStack::~VNetStack () {
	
}

char VNetStack::initialize () {
	return 1;
}

char VNetStack::processTapPacket (Packet * pPacket, int nID) {
	int		nDir;
	int		nAdapter;
	
	// Direction is the ID modulus 2
	nDir = nID % 2;
	
	// Adapter is the ID divided by 2
	nAdapter = nID / 2;
	
	if(m_pHost == NULL) {
		// Did not gobble it, no host to help us out
		return 0;
	}
	
	Adapter *		pAdapter;
	
	pAdapter = m_pHost->getAdapter(nAdapter);
	
	if(pAdapter == NULL) {
		// Ruh roh, bad adapter
		logFaultHighEvent("Warning: Adapter query to virtual host failed to return a valid Adapter pointer", "VNetStack", "processTapPacket");
		return 0;
	}
	
	// OK, at this point we know the adapter that said information came in on
	
	
	return 0;
}


VirtualHost	* VNetStack::getHost () {
	return m_pHost;
}

bool VNetStack::setHost (VirtualHost * pHost) {
	if(pHost != NULL) {
		m_pHost = pHost;
		return true;
	} else {
		logFaultHighEvent("Warning: Attempting to change the virtual network stack to be associated with a NULL host", "VNetStack", "setHost");
		return false;
	}
}

