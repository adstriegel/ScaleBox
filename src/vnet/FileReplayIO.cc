
#include <iostream>
using namespace std;

#include "FileReplayIO.h"
#include "../mem/MemoryPool.h"
#include "../pkt/Packet.h"

FileReplayModule::FileReplayModule () : IOModule () {	
	setType("FileReplayModule");	
}

FileReplayModule::~FileReplayModule () {
	
}

char FileReplayModule::initialize () {
	nResult = pthread_create(&m_ThreadFileRead, NULL, Thread_FileReplay, this);
	
	if(nResult) {
		cerr << "* Error creating file replay module thread for " << getName() << endl;
		cerr << "    Code: " << nResult << endl;		
		exit(-1);
	}
		
	return 1;	
}


char FileReplayModule::processPacket (Packet * pPacket) {
	return 0;
}

bool	FileReplayModule::extractExtendedDOM (NodeDOM * pNode) {
	return true;
}

void * Thread_FileReplay (void * pArg) {
		
}


