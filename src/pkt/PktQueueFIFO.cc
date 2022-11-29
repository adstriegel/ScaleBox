/* PktQueue.cc
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * Development and equipment support has been provided in
 *  part by the following sponsors:
 * 
 *    National Science Foundation    CNS03-47392
 *    Sun Microsystems Academic Excellence Grant
 *    Intel IXP Development Grant 
 *    University of Notre Dame Faculty Grant
 * 
 **********************************************************
 * $Revision: 1.3 $  $Date: 2008/04/11 22:01:11 $
 **********************************************************
 */

#include <iostream>
using namespace std;

#include "Packet.h" 
#include "PktQueue.h"
#include "PktQueueFIFO.h"

PktQueueFIFO::PktQueueFIFO () {
	// No restriction on number of packets
	m_nMaxPacketsQueue = 0;
	
	// No restriction on total size of queue
	m_nMaxSizeQueue = 0;
	
	// Initialize it
	initialize();
}

/////////////////////////////////////////////////////////////

PktQueueFIFO::~PktQueueFIFO () {
	
}

/////////////////////////////////////////////////////////////

char PktQueueFIFO::initialize () {
	// Do any initialization here that we need to do.  Initialization is done
	//  just after all of the XML fields are processed but not necessarily before
	//  the entirety of the XML file is processed
	//
	// Appropriate things to put here are initializing threads, mutexes, i.e.
	//  parts that do not necessarily need to talk to different IO modules
	
	pthread_mutex_init(getMutexQueue(), NULL);
		
	// Return a 1 if everything looks good, otherwise a 0 brings ScaleBox during
	//  startup to a screeching halt
	return 1;	
}

/////////////////////////////////////////////////////////////

bool PktQueueFIFO::startModule () {
	// startModule is invoked once the entirety of the configuration has been
	// initialized.  The typical role of the startModule operation is to get
	// the various threads actually going (as opposed to an early pause/wait
	// state) as well as linking this module with other modules
	
	// The core InputPassThrough code is a good example that uses startModule
	
	// Return true if everything looks good, otherwise a return code of false
	//  again brings ScaleBox to a halt on startup
	
	return true;	
}

/////////////////////////////////////////////////////////////

char PktQueueFIFO::processPacket (Packet * pPacket) {
	// We enter process packet every time whatever chain we are on
	// gets called. We can exist on any output or input chain
	// in the architecture.  
	
	// Do some packet magic
	//	 Extract some fields - see ../util/ip-utils.h for prototypes
	//  Parse whatever we want or record stats
	
	// The return value for the function is key
	//	  1 means that we ate the packet (nobody else should get it) and are
	//   eventually responsible for giving it back to the memory manager
	//     See InputPurge or call gobblePacket to do this
	//   0 means that we just looked at the packet, maybe manipulated it but
	//     it should keep going down the chain
	
	return 0;
}

/////////////////////////////////////////////////////////////

bool	PktQueueFIFO::extractExtendedDOM (NodeDOM * pNode) {
	int			j;
	NodeDOM	*	pChild;
	
	// Feel free to add different tests for exact matches in the form of
	//  attributes listed under the module as
	//
	//     <sandboxID>MyEnemyIsSpiderman</sandboxID>
	
	// True means everything was kosher or at least tolerable in terms of the
	//  XML processing.  A false will bring the loading of ScaleBox to a grinding
	//  halt
	
	return true;
}

/////////////////////////////////////////////////////////////

NodeDOM * PktQueueFIFO::getStats () {
	return NULL;
}

/////////////////////////////////////////////////////////////

int PktQueueFIFO::getNumPackets () {
	int		nVal;
	
	pthread_mutex_lock(getMutexQueue());
	nVal = m_Packets.size();
	pthread_mutex_unlock(getMutexQueue());
	
	return nVal;
}

/////////////////////////////////////////////////////////////
 		
int PktQueueFIFO::getSizePackets () {
	int		j;
	int		nSum = 0;
	
	pthread_mutex_lock(getMutexQueue());
	
	for(j=0; j<m_Packets.size(); j++) {
		nSum += m_Packets[j]->getLength();
	}

	pthread_mutex_unlock(getMutexQueue());
	
	return nSum;
}

/////////////////////////////////////////////////////////////

Packet *	PktQueueFIFO::getPacket (int nPkt) {
	Packet *	pPacket;

	pthread_mutex_lock(getMutexQueue());

	if(nPkt < 0 || nPkt >= m_Packets.size()) { 
		pthread_mutex_unlock(getMutexQueue());	
		return NULL;
	}

	pPacket = m_Packets[nPkt];
	pthread_mutex_unlock(getMutexQueue());	
	
	return pPacket;	
}

/////////////////////////////////////////////////////////////

bool 	PktQueueFIFO::enqueue (Packet * pPacket) {
	if(m_nMaxPacketsQueue > 0) {
		// Do we pass
		if(getNumPackets() < m_nMaxPacketsQueue-1) {
			// We pass, go to the next test
		} else {
			// Don't pass
			return false;
		}
	} else {
		// No restriction (<= 0)	
	}
	
	if(m_nMaxSizeQueue > 0) {
		if(getSizePackets() + pPacket->getLength() > getMaxQueueByteSize()) {
			return false;
		} else {
			// OK, we passed
		}
	} else {
		// No restrictions on size either	
	}
	
	// Add this to queue now since we are OK for both size (number and raw byte size)	
	pthread_mutex_lock(getMutexQueue());	
	m_Packets.push_back(pPacket);
	pthread_mutex_unlock(getMutexQueue());			
	return true;
}

/////////////////////////////////////////////////////////////

void	PktQueueFIFO::setMaxPacketsToQueue (int nPkts) {
	pthread_mutex_lock(getMutexQueue());	
	m_nMaxPacketsQueue = nPkts;
	pthread_mutex_unlock(getMutexQueue());	
}

/////////////////////////////////////////////////////////////

int PktQueueFIFO::getMaxPacketsToQueue () {
	int		nVal;

	pthread_mutex_lock(getMutexQueue());		
	nVal = m_nMaxPacketsQueue;
	pthread_mutex_unlock(getMutexQueue());	
	
	return nVal;
}

/////////////////////////////////////////////////////////////

void PktQueueFIFO::setMaxQueueByteSize (int nSize) {
	pthread_mutex_lock(getMutexQueue());	
	m_nMaxSizeQueue = nSize;
	pthread_mutex_unlock(getMutexQueue());		
}

/////////////////////////////////////////////////////////////

int PktQueueFIFO::getMaxQueueByteSize () {
	int		nVal;

	pthread_mutex_lock(getMutexQueue());		
	nVal = m_nMaxSizeQueue;
	pthread_mutex_unlock(getMutexQueue());	
	
	return nVal;	
}

/////////////////////////////////////////////////////////////

bool PktQueueFIFO::popFront () {
	pthread_mutex_lock(getMutexQueue());		

	if(m_Packets.size() == 0) {
		pthread_mutex_unlock(getMutexQueue());		
		return false; 
	} else {
		m_Packets.pop_front();
		pthread_mutex_unlock(getMutexQueue());		
		return true; 
	}		
}

/////////////////////////////////////////////////////////////

void PktQueueFIFO::dumpInfo () {
	cout << "PktQueueFIFO @";
	printf("0x%X", this);
	cout << " w/" << getNumPackets() << " packets in the queue" << endl;	
}

/////////////////////////////////////////////////////////////

Packet * PktQueueFIFO::popAndGetPacket () {
	Packet *		pPacket;
	deque<Packet *>::iterator		pIter;
	
	pPacket = NULL;
	
	pthread_mutex_lock(getMutexQueue());	
	
	if(m_Packets.size() > 0) {
		pIter = m_Packets.begin();
		pPacket = *pIter;
		m_Packets.pop_front();
	}	
		
	pthread_mutex_unlock(getMutexQueue());			
	
	return pPacket;
}

/////////////////////////////////////////////////////////////





