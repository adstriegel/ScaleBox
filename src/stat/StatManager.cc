/* StatManager.cc
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
 * $Revision: 1.2 $  $Date: 2008/04/04 21:43:23 $
 **********************************************************
 */
 
#include <iostream>
using namespace std;
 
#include "../util/PracticalSocket.h" 
 
#include "StatManager.h"
#include "ThreadClientStat.h"
#include "../mon/Monitor.h"


StatManager		g_StatManager;

StatManager::StatManager () {
	m_nStreamPort = DEFAULT_STAT_PORT;
	m_pServerSocket = NULL;
	m_nSendInterval = DEFAULT_SEND_INTERVAL;
}

StatManager::~StatManager () {
	if(m_pServerSocket != NULL) {
		delete m_pServerSocket;
		m_pServerSocket = NULL;
	}
}

bool StatManager::startup () {
	return setupStreamServer();
}

bool StatManager::setupStreamServer () {
	int nResult;
	
	if(m_pServerSocket != NULL) {
		delete m_pServerSocket;
		m_pServerSocket = NULL;
	}
	
	pthread_mutex_init(&m_mutexClientList, NULL);
	
	try {
		m_pServerSocket = new TCPServerSocket(m_nStreamPort);    
  	} catch (SocketException &e) {
    	cerr << e.what() << endl;
    	return false;
  	}	
	
	// Spawn the thread to do the listening for the server
	nResult = pthread_create(&m_ListenThread, NULL, Thread_ClientStatListen, this);
	if(nResult) {
		cerr << "* Error creating streaming stat thread (listen) " << endl;
		cerr << "    Code: " << nResult << endl;		
		return false;
	}	
	
	// Spawn the thread to do the actual gathering / writing of the stats to
	//  all of the subscribing clients
	nResult = pthread_create(&m_StreamThread, NULL, Thread_ClientStat, this);
	if(nResult) {
		cerr << "* Error creating streaming stat thread " << endl;
		cerr << "    Code: " << nResult << endl;		
		return false;
	}		
	
	return true;	
}

TCPServerSocket * StatManager::getServerSocket () {
	return m_pServerSocket;	
}

bool StatManager::keepRunning () {
	return g_Monitor.shouldKeepRunning();
}

void StatManager::addClient (TCPSocket * pClientSocket) {
	if(pClientSocket != NULL) {
		pthread_mutex_lock(&m_mutexClientList);
		m_Clients.push_back(pClientSocket);	
		pthread_mutex_unlock(&m_mutexClientList);		
	}
}

unsigned int StatManager::getClientCount () {
	int nSize;
	
	pthread_mutex_lock(&m_mutexClientList);
	nSize =  m_Clients.size();	
	pthread_mutex_unlock(&m_mutexClientList);
	return nSize;
}

TCPSocket * StatManager::getClient (int nClient) {
	TCPSocket * pClient;

	pClient = NULL;
	
	pthread_mutex_lock(&m_mutexClientList);
	
	if(nClient < m_Clients.size() && nClient >= 0) {
		pClient = m_Clients[nClient];
	}
	
	pthread_mutex_unlock(&m_mutexClientList);
	return pClient;
}

void StatManager::sendAll (string & sData) {
	int			j;
	TCPSocket *	pClient;
	char			szEOD[40];
	
	vector<TCPSocket *>::iterator		theIter;
	
	pthread_mutex_lock(&m_mutexClientList);		
	
	sprintf(szEOD, "\n##EOS##\n");
	
	for(j=0; j<m_Clients.size(); j++) {
		pClient = m_Clients[j];
		
		try {
			pClient->send(sData.c_str(), sData.size());
			pClient->send(szEOD, strlen(szEOD));
		} catch (SocketException & e) {
			// Some issue with the socket, kick it out of the list
			theIter = m_Clients.begin();
			m_Clients.erase(theIter+j);
			j--;
		}
	}
	
	pthread_mutex_unlock(&m_mutexClientList);	
}

unsigned int StatManager::getSendInterval () {
	return m_nSendInterval;	
}

//////////////////////////////////////////////////////////

CommandResult	StatManager::processCommand (const vector<string> & theCommands, int nOffset) {
		
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}
	
	cout << "Stat manager process" << endl;
	
	if(theCommands[nOffset] == "status") {
		int			j;
		
		cout << "Streaming Statistic Server [Status]" << endl;
		cout << "   " << getClientCount() << " clients connected" << endl;				
		return PROCESS_CMD_SUCCESS;
	}
	
	
	return PROCESS_CMD_UNKNOWN;	
}


      
    
    
