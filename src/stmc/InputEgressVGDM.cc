

#include <iostream>
using namespace std;

#include "InputEgressVGDM.h"
#include "InputIngressVGDM.h"
#include "VGDM.h"

InputEgressVGDM::InputEgressVGDM () : IOModule () {
	m_pVGDM = NULL;
}

InputEgressVGDM::~InputEgressVGDM () {
	
}

char InputEgressVGDM::initialize () {
	// Nothing, all done by InputIngressVGDM
	return 1;	
}

void InputEgressVGDM::linkIngress (InputIngressVGDM * pIngress) {
	if(pIngress == NULL) {
		cerr << "Error: The passed in ingress VGDM module to the egress VGDM" << endl;
		cerr << "    module was NULL.  Unable to link the egress to the VGDM." << endl;
		return;
	}
	
	m_pVGDM = pIngress->getVGDM();
	
	if(m_pVGDM != NULL) {
		m_pVGDM->setEgress(this);
	} else {
		cerr << "Error: InputIngressVGDM module was not initialized. Cannot" << endl;
		cerr << "   retrieve the VGDM for proper linkage. Make sure to call" << endl;
		cerr << "   initialize on the ingress VGDM input module first" << endl;
		return;
	}
}

char InputEgressVGDM::processPacket (Packet * pPacket) {
	return m_pVGDM->processEgressPacket(pPacket);	
}

bool	InputEgressVGDM::extractExtendedDOM (NodeDOM * pNode) {
	return true;
}

