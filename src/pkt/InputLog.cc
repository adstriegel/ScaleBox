// InputLog.cc
//
// The InputLog module is a simple IOModule for the purposes of dumping packets as
// seen to the console or for potentially writing / saving packets to disk.

#include "InputLog.h"
#include "Packet.h"
#include "Adapter.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

#include "../util/ip-utils.h"

#include <iostream>
using namespace std;

InputLog::InputLog () : IOModule () {
	m_bLogConsole = 1;
	m_nConsoleMode = LOGCONSOLE_MODE_BRIEF;
	m_sConsoleID = "IL";
	setType("Log");	
}


InputLog::~InputLog () {
	
}

char InputLog::initialize () {
	return 1;	
}


char InputLog::processPacket (Packet * pPacket) {

	cout << "processPacket (InputLog)" << endl;

	// Output to the console if the flag is appropriate set to display
	//
	// The appropriate location to tweak if a packet should have made it to this
	// module in the first place is in the filter for the module itself
	if(m_bLogConsole) {
		cout << m_sConsoleID << "(" << getAdapter()->getName() << "):  ";
		printf("0x%p %d bytes  ", pPacket, pPacket->getLength());
		
		if(isPacketIP(pPacket->getData(), pPacket->getLength())) {
			printf("IPv4 ");
			dumpIPv4(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP);
			printf(" to ");
			dumpIPv4(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP);
			printf("\n");
		} else {
			printf("  Not IP\n");
		}
	}
	
	return 0;
}

void InputLog::setFlag_LogConsole (char bVal) {
	m_bLogConsole = bVal;	
}
		
char InputLog::getFlag_LogConsole () {
	return m_bLogConsole;
}

void InputLog::setMode_LogConsole (int nVal) {
	m_nConsoleMode = nVal;
}

int InputLog::getMode_LogConsole () {
	return m_nConsoleMode;
}
	
void InputLog::setConsoleID (string sConsoleID) {
	m_sConsoleID = sConsoleID;	
}
		
string InputLog::getConsoleID () {
	return m_sConsoleID;	
}

bool	InputLog::extractExtendedDOM (NodeDOM * pNode) {
	int			j;
	NodeDOM	*	pChild;
	
	for(j=0; j<pNode->getNumChildren(); j++) {
		if(pNode->getChild(j)->getTag() == "ConsoleID") {
			setConsoleID(pNode->getChild(j)->getData());		
		}	
	}
	
	return true;
}

NodeDOM * InputLog::getStats () {
	return NULL;
}

	
