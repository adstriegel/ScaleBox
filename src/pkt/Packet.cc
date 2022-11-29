/* Packet.cc
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * $Revision: 1.6 $  $Date: 2008/04/11 22:52:23 $
 **********************************************************
 */

#include <iostream>
using namespace std;

#include "Packet.h"
#include "../mem/MemoryPool.h"
#include "PacketSignature.h"

Packet::Packet() : MemPoolObject (MEMPOOL_OBJ_PACKET) {
	clean();
	/*
	m_arrTime.tv_sec = 0;
	m_arrTime.tv_usec = 0;	
	m_nLength = 0;
	m_pResumeModule = NULL;*/
}

Packet::~Packet () {
	m_pResumeModule = NULL;
}

		
Packet *	Packet::createPacket () {
	Packet 	*	pPacket;
	
	pPacket = (Packet *) g_MemPool.getObject(MEMPOOL_OBJ_PACKET);
	
	return pPacket;	
}

IOModule * Packet::getResumeAfterModule () {
	return m_pResumeModule;
}

void Packet::setResumeAfterModule (IOModule * pModule) {
	m_pResumeModule = pModule;
}

void Packet::clean () {
	PacketSignature	*	pSig;
	
	m_nLength = 0;
	m_arrTime.tv_sec = 0;
	m_arrTime.tv_usec = 0;
	
	m_pResumeModule = NULL;	
		
	while(m_sigList.size() > 0) {		
		pSig = m_sigList.front();
		
		g_MemPool.releaseObject(pSig);
		m_sigList.erase(m_sigList.begin());
	}
	
	// Callback data (for precise outbound timing)
	m_pWriteCallback = NULL;
	m_pWriteCallbackData = NULL;
	m_nWriteCallbackData_Type = PKT_WRITECALLBACK_TYPE_NONE;
	
	// Trace status
	m_bShouldTrace = false;	
}	

string Packet::getClassInfo () {
	string		theString;
	
	theString = "Packet";
	
	return theString;	
}


void Packet::setArrivalTime(struct timeval * pTime) { 
	memcpy(&m_arrTime, pTime, sizeof(struct timeval));
}

struct timeval * Packet::getArrivalTime () {
	return &m_arrTime;
}

int Packet::getLength () {
	return m_nLength;
}
		
void Packet::setLength (int nSize) {
	if(nSize >= 0) {		
		m_nLength = nSize;
	}
	else {
		m_nLength = 0;
	}
}
		
char * Packet::getData   () {
	return m_byData;
}
		
void Packet::setData (int nLength, char * pData) {
	setLength(nLength);
	memcpy(m_byData, pData, getLength());	
}

void Packet::showHex (int nStart, int nStop)
{
	int		j;

	for(j=nStart; j<nStop; j++)
	{
		printf("%02X", (unsigned char) m_byData[j]);
	}
}

void Packet::showText (int nStart, int nStop)
{
	int		j;

	for(j=nStart; j<nStop; j++)
	{
		printf("%c", m_byData[j]);
	}
}


Packet * Packet::cloneFromPool () {
	Packet	*		pNewPkt;
	
	pNewPkt = (Packet *) g_MemPool.getObject(MEMPOOL_OBJ_PACKET);
	
	if(pNewPkt == NULL) {
		cerr << "Failed to clone packet from the memory pool, aborting clone..." << endl;
		return NULL;
	}

	memcpy(pNewPkt->m_byData, m_byData, m_nLength);
	pNewPkt->m_nLength = m_nLength;
	pNewPkt->m_arrTime = m_arrTime;	

	// Copy the underlying signatures
	//cout << "Need to copy underlying sigs!!!!" << endl;
	
	return pNewPkt;
}

void Packet::addSignature	(PacketSignature * pSig) {
	m_sigList.push_back(pSig);	
}

void Packet::zeroPad			(int nPad) {
	if(nPad+m_nLength < MAX_PKT_SIZE) {
		memset(m_byData+m_nLength, 0, nPad);	
	} else {
		// Too far, ignoring
	}
}

bool Packet::registerWriteCallback (IOModule * pCallModule, void * pData, int nType) {
	if(m_pWriteCallback == NULL) {
		m_pWriteCallback = pCallModule;
		m_pWriteCallbackData = pData;
		m_nWriteCallbackData_Type = nType;
		return true;
	} else {
		if(pData != NULL) {
			m_pWriteCallback = NULL;
			m_pWriteCallbackData = pData;
		}
		
		return false;
	}	
}

void * Packet::getWriteCallbackData  () {
	return m_pWriteCallbackData;
}

bool Packet::invokeWriteCallback () {
	if(m_pWriteCallback != NULL) {
		struct timeval		curTime;
		// What is the current system time?
		gettimeofday(&curTime, NULL);

		this->setArrivalTime(&curTime);

		if(m_nWriteCallbackData_Type == PKT_WRITECALLBACK_TYPE_NONE)
		{
			return m_pWriteCallback->processWriteCallback(this, m_pWriteCallbackData);
		}
		else
		{
			return m_pWriteCallback->processWriteCallback(this, m_pWriteCallbackData, m_nWriteCallbackData_Type);
		}
	} else {	
		return false;
	}	
}

bool Packet::isTraceEnabled () {
	return m_bShouldTrace;	
}

void Packet::enableTracing () {
	m_bShouldTrace = true;
}

void Packet::disableTracing () {
	m_bShouldTrace = false;	
}

//////////////////////////////////////////////////////////

unsigned int Packet::getMaxSize () {
	return MAX_PKT_SIZE;
}

//////////////////////////////////////////////////////////

uint16_t	Packet::forceUnusedZero ()
{
	int		j;

	memset(getData()+getLength(), 0, getMaxSize()-getLength());

	return getMaxSize()-getLength();
}
