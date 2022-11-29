/* StatManager.h
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
 
#ifndef __STATMANAGER_H
#define __STATMANAGER_H

#include <vector>
using namespace std; 

#include <pthread.h>

#include "../core/StdHeaders.h"
#include "../core/Console.h"

#define DEFAULT_STAT_PORT 		10050
#define DEFAULT_SEND_INTERVAL	5			// Send data every 5 seconds	

class TCPServerSocket;
class TCPSocket;

/** The stat manager class is responsible for corraling stats at a
 * centralized location for either recording at the end of an
 * experimental run or for the purpose of connecting an external
 * listening tool available in the support tools for ScaleBox
 */
class StatManager {
	public:
		StatManager ();
		
		~StatManager ();
	
		/** Create the socket and start it listening on its own thread
		 * for new incoming connections
		 */
		bool		setupStreamServer ();
	
		/** Start up the underlying objects for the stat manager and
		 * the streaming port 
		 */
		bool		startup();
	
		/** Retrieve the port associated for streaming
		 * @returns The port number that will be used for the server 
		 */
		unsigned short		getPortStream ();

		/** Set the port number to be used for streaming statistics
		 * @param nPort The new port number for the streaming server
		 */
		void	setPortStream (unsigned short nPort);		
	
		/** Return a handle to the server socket object
		 * @returns If initiated, a valid pointer to a server socket object
		 */
		TCPServerSocket  *	getServerSocket ();
	
		/** Should the various child threads keep running?
		 * @returns True if yes, false otherwise */
		bool	keepRunning ();
	
		/** Add a client to be served by the thread periodically streaming
		 * out statistics
		 * @param pClientSocket A pointer to a client socket object
		 */
		void		addClient (TCPSocket * pClientSocket);

		/** Retrieve the number of clients currently subscribing for streaming
		 * of statistics from the statistic server
		 * @returns Number of clients subscribing
		 */
		unsigned int		getClientCount ();
	
		/** Retrieve a specific client from the list of clients
		 * @param nClient The client to retrieve
		 * @returns A pointer to a valid client object
		 */
		TCPSocket *			getClient (int nClient);
	
		/** Stream the information string to all clients
		 * @param sData The information to stream
		 */
		void		sendAll (string & sData);
	
		/** Retrieve the sending interval for statistics
		 * @returns Sending interval between polling and blasting to all
		 *  subscribers (in seconds)
		 */
		unsigned int getSendInterval ();
	
		/** Process a command from the console or external control
		 * mechanism.  
		 * 
		 * Show basic information for the timer manager
		 * 
		 *   cmd timer status
		 * 
		 * @param pCommands The vector of commands to process
		 * @param nOffset The offset at which to begin processing. The offset allows
		 *   us to pass the vector itself with modifications to the underlying
		 *   modules
		 * @returns Enumerated value representing result of command processing
		 */ 
		CommandResult	processCommand (const vector<string> & theCommands, int nOffset);	
	
	private:	
		/** Thread for listening for new incoming connections */
		pthread_t		m_ListenThread;
		
		/** Thread for dispatching streaming data to existing connections */
		pthread_t		m_StreamThread;
		
		/** Mutex to synchronize traversal of the client list with modifications
		 * to the list itself
		 */
		pthread_mutex_t		m_mutexClientList;

	
		unsigned short	m_nStreamPort;
		TCPServerSocket	*	m_pServerSocket;
	
		unsigned int				m_nSendInterval;
		
		// Vector of currently live subscribers that we will be streaming
		//  statistics to
		vector<TCPSocket *> 	m_Clients;
};


extern 	StatManager		g_StatManager;

#endif
