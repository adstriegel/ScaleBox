/* ThreadClientStat.cc
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
 * $Revision: 1.2 $  $Date: 2008/04/04 21:43:38 $
 **********************************************************
 */

#include <pthread.h>

#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
using namespace std;

#include "../util/PracticalSocket.h"  // For Socket, ServerSocket, and SocketException
#include "../util/mgmt.h"
#include "../xml/NodeDOM.h"

#include "../mon/Monitor.h"

#include "ThreadClientStat.h"
 
#include "StatManager.h" 
 
void * Thread_ClientStat (void * pArg) {
	StatManager		*		pMgr;
	NodeDOM 	*			pDataDOM;
	string			sOutData;
	
	pMgr = (StatManager *) pArg;
	
	if(pMgr == NULL) {
		cerr << "Bad pointer for manager passed to thread for statistic stream server" << endl;
		return NULL;
	}	
	
	while(pMgr->keepRunning()) {	
		// Gather the data
		pDataDOM = g_Monitor.gatherStats();
		sOutData = "";
		if(pDataDOM != NULL) {
			pDataDOM->createString(sOutData);
			//cout << sOutData << endl;
		}		
		
		// Send the data
		pMgr->sendAll(sOutData);
		
		// Release the data back to the memory manager
		if(pDataDOM != NULL) {
			pDataDOM->release();
		}
		
		// Wait until the next send time
		microsleep(pMgr->getSendInterval() * 1000000);
	}
	
	return NULL;	
}

void * Thread_ClientStatListen (void * pArg) {
	StatManager		*		pMgr;
	TCPServerSocket *		pServerSocket;
	
	pMgr = (StatManager *) pArg;
	
	if(pMgr == NULL) {
		cerr << "Bad pointer for manager passed to thread for statistic stream server" << endl;
		return NULL;
	}
	
	while(pMgr->keepRunning()) {
		pServerSocket = pMgr->getServerSocket();
		
		TCPSocket *		pClientSocket;
		
		pClientSocket = pServerSocket->accept();
		
		// We have a new client now
		
		pMgr->addClient(pClientSocket);
	}
}



 