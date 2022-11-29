

#include <iostream>
using namespace std;

#include "InputIngressVGDM.h"
#include "VGDM.h"

InputIngressVGDM::InputIngressVGDM () : IOModule () {
	m_pVGDM = NULL;
}

InputIngressVGDM::~InputIngressVGDM () {
	if(m_pVGDM != NULL) {
		delete m_pVGDM;
	}
}

void InputIngressVGDM::createVGDM () {
	m_pVGDM = new VGDM ();	
	m_pVGDM->setIngress(this);
}

char InputIngressVGDM::initialize () {
	createVGDM();
	return 1;	
}


char InputIngressVGDM::processPacket (Packet * pPacket) {
	//cout << "  InputIngressVGDM::processPacketIn" << endl;

	if(m_pVGDM != NULL) {		
		return m_pVGDM->processIngressPacket(pPacket);	
	}
}

VGDM * InputIngressVGDM::getVGDM () {
	return m_pVGDM;
}

bool	InputIngressVGDM::extractExtendedDOM (NodeDOM * pNode) {
	return true;
}
