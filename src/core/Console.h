/* Console.h
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
 * $Revision: 1.3 $  $Date: 2008/03/06 21:09:32 $
 **********************************************************
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>
using namespace std;

enum CommandResult {
	/** Unknown command */
	PROCESS_CMD_UNKNOWN=0,
	/** Command is meant for a different target, ignored by this one */
	PROCESS_CMD_DIFFTARGET,
	/** Successfully procesed the command */
	PROCESS_CMD_SUCCESS,
	/** Warning in processing the command */
	PROCESS_CMD_WARNING,	
	/** Error in processing the command */
	PROCESS_CMD_ERROR,
	/** Syntax error */
	PROCESS_CMD_SYNTAX
}; 

/** Monitor the standard input and process the commands as appropriate
 */
void doConsole ();

/** Process a specific command
 * @param sCmd The command to process
 * @returns True if successful, false otherwise
 */
CommandResult	processCommand (string & sCmd);

/** Spawn a thread to process the series of commands pointed to in the
 * pCmd object
 * @param pCmd A pointer to a valid string object containing the data
 *    to process
 */
bool	spawnCommandThread (string * pCmd);

/** A worker thread spawned to execute a series of commands from start
 * to finish
 */
void * Thread_Command (void * pArg);

#endif /*CONSOLE_H_*/
