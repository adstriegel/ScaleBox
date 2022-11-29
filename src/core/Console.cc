


#include <iostream>
using namespace std;

#include <vector>
using namespace std;

#include <string.h>

#include "Console.h"

#include "../pkt/Adapter.h"

#include "../mem/MemoryPool.h"
#include "../mon/Monitor.h"

#include "../util/mgmt.h"
#include "../util/tokenize.h"

#include "../pkt/IOModule.h"

void exitSystem () {
	g_Monitor.shutdown();	
}

/** Display a help message for the console to the screen */
void consoleHelp () {
	cout << "ScaleBox Console Help" << endl;
	cout << "   help, ?     This help listing" << endl;
	cout << "   quit, q     Exit / quite ScaleBox" << endl;
	cout << endl;
	cout << " Commands to various components" << endl;
	cout << "   cmd showstats   Show the stat dump" << endl;
	cout << "   cmd mempool XX  Interact with the global memory pool" << endl;
	cout << endl;
	cout << " To interact with a module, the syntax is typically as follows:" << endl;
	cout << "    cmd mod XXXX C1 C2 C3" << endl << endl;
	cout << " where XXX is the name of the module (specified in the XML config file)" << endl;
	cout << "   and C1, C2, C3, ... is the actual command passed to the module." << endl;
}

void tokenize (char * pszCmd, vector<string> & theCmd) {
	char 	* pTok;
	string	theTok;
	
	theCmd.clear();
	
	pTok = strtok(pszCmd, " ");
	
	while(pTok != NULL) {
		theTok = pTok;
		
		theCmd.push_back(theTok);		
		
		pTok = strtok(NULL, " ");
	}
}

void doConsole () {
	char	szCmd[201];
	
	vector<string> 	theCmd;
	string		sCmd;

	// Loop around waiting for an input

	while(g_Monitor.shouldKeepRunning()) {
		cout << "SBox>";
		cout.flush();
	
		cin.getline(szCmd, 201);
		
		sCmd = szCmd;
		
		processCommand(sCmd);
		
		// Compare this versus known instructions
		
		//	q,quit			Quit
		//	?,help			Show help
		//  memstat			Status of memory pool
/*		
		if(strcmp(szCmd, "q") == 0) {
			exitSystem();
			return;
		}
		
		if(strcmp(szCmd, "quit") == 0) {
			exitSystem();
			return;
		}

		if(strcmp(szCmd, "?") == 0) {
			consoleHelp();
			continue;
		}
		
		if(strcmp(szCmd, "help") == 0) {
			consoleHelp();
			continue;
		}
		
		if(strncmp(szCmd, "memstat", 200) == 0) {
			g_MemPool.dumpStatus();
			continue;
		}
		
		tokenize(szCmd, theCmd);
		
		int j, k;
		
		for(j=0; j<theCmd.size(); j++) {
			cout << "Token " << j << ": " << theCmd[j] << endl;
		}
		
		if(theCmd.size() <= 1) {	
			cout << "Command not understood: " << szCmd << endl;
		} else {
			// React to the first token	
			
			if(theCmd[0] == "list" || theCmd[0] == "l") {
				// List command - list information about specific systems / modules	
				
				if(theCmd[1] == "adapter" || theCmd[1] == "a") {
					// List the adapters
					cout << "Defined Adapters: " << g_Monitor.getCountAdapters() << endl;
					
					for(k=0; k<g_Monitor.getCountAdapters(); k++) {
						
					}						
				}
				else if(theCmd[1] == "modules" || theCmd[1] == "m") {
					// List the adapters
					cout << "Defined Adapters: " << g_Monitor.getCountAdapters() << endl;
					
					for(k=0; k<g_Monitor.getCountAdapters(); k++) {
						g_Monitor.getAdapter(j)->listModules();
					}						
				}				
			}
			else if (theCmd[0] == "stat" || theCmd[0] == "s") { 
				g_Monitor.dumpStat(theCmd[1]);					
			}
		}
		*/
	}
}

CommandResult	processCommand (string & sCmd) {
	vector<string>		cmdTokens;
	
	Tokenize(sCmd, cmdTokens, " \t");
				
	if(cmdTokens.size() == 0) {
		//cout << " No command specified" << endl;
		return PROCESS_CMD_SYNTAX;
	}
	
	if(cmdTokens[0] == "q") {
		exitSystem();
		return PROCESS_CMD_SUCCESS;
	}	
	
	if(cmdTokens[0] == "quit") {
		exitSystem();
		return PROCESS_CMD_SUCCESS;
	}	
	
	if(cmdTokens[0] == "exit") {
		exitSystem();
		return PROCESS_CMD_SUCCESS;
	}

	if(cmdTokens[0] == "sleep-micro") {
		if(cmdTokens.size() < 2) {
			cerr << " Unable to process sleep-micro command, no duration specified" << endl;
			return PROCESS_CMD_SYNTAX;
		} else {
			microsleep(atoi(cmdTokens[1].c_str()));
			return PROCESS_CMD_SUCCESS;
		}
	}

	if(cmdTokens[0] == "sleep-milli") {
		if(cmdTokens.size() < 2) {
			cerr << " Unable to process sleep-milli command, no duration specified" << endl;
			return PROCESS_CMD_SYNTAX;
		} else {
			microsleep(atoi(cmdTokens[1].c_str())*1000);
			return PROCESS_CMD_SUCCESS;
		}
	}

	if(cmdTokens[0] == "sleep") {
		if(cmdTokens.size() < 2) {
			cerr << " Unable to process sleep command, no duration specified" << endl;
			return PROCESS_CMD_SYNTAX;
		} else {
			microsleep(atoi(cmdTokens[1].c_str())*1000000);
			return PROCESS_CMD_SUCCESS;
		}
	}	
	
	if(cmdTokens[0] == "disp") {		
		int j;
		
		for(j=1; j<cmdTokens.size(); j++) {
			cout << cmdTokens[j];
		}
		
		cout << endl;
		return PROCESS_CMD_SUCCESS;
	}	
	
	if(cmdTokens[0] == "cmd") {
		return(g_Monitor.processCommand(cmdTokens, 1));	
	}	
	
	if(cmdTokens[0] == "help" || cmdTokens[0] == "?") {
		consoleHelp();
	}
	
	return PROCESS_CMD_UNKNOWN;	
}

bool	spawnCommandThread (string * pCmd) {
	pthread_t		threadInfo;
	int		nResult;
	
	// In some sense, we don't care about the actual pthread handle as we
	//  are just going to let it wander over to completion
		
	nResult = pthread_create(&threadInfo, NULL, Thread_Command, pCmd);
		
	if(nResult) {
		cerr << "* Error creating command thread " << endl;
		cerr << "    Code: " << nResult << endl;
		return false;
	}		
	
	return true;
}


void * Thread_Command (void * pArg) {
	string	*	pCmd;
	
	// The input argument is the raw data field (unprocessed command set)
	//  from the outlying XML file
	
	pCmd = (string *) pArg;	
	
	//cout << "Command(s) to process: " << endl;
	//cout << *pCmd << endl;
	
	// Split according to each line of commands
	
	vector<string> theCmds;
	
	Tokenize(*pCmd, theCmds, "\n");
	
	int		j;
	
	for(j=0; j<theCmds.size(); j++) {
		processCommand(theCmds[j]);
	}
	
	delete pCmd;

	return NULL;
}

