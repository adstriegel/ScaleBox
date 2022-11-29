#ifndef INPUTLOG_H_
#define INPUTLOG_H_

#include "IOModule.h"
#include <string>
using namespace std;

enum LogConsoleMode {
	LOGCONSOLE_MODE_BRIEF,
	LOGCONSOLE_MODE_FULL
};

enum LogFileMode {
	LOGFILE_MODE_CSV,
	LOGFILE_MODE_TCPDUMP,
	LOGFILE_MODE_XML
};

/** Log all or a filtered subset of packets to disk in the
 * specified file
 */
class InputLog : public IOModule {
	public:
		InputLog ();
		
		~InputLog ();

		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();	

		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		char		processPacket (Packet * pPacket);
	
		/** Set the flag as to whether or not content should be
		 * logged to the screen for examination
		 * @param bVal True if information should be logged to the screen, false otherwise */
		void		setFlag_LogConsole (char bVal);
		
		char		getFlag_LogConsole ();

		/** Set the mode regarding how information is displayed to the console via stdout.
		 * The mode describes the level of detail (verbosity) of the data displayed
		 * @param nVal The mode for displaying the information to the console
		 */
		void		setMode_LogConsole (int nVal);
		
		/** Retrieve the mode regarding how information is displayed to the console via stdout
		 * @returns The mode for displaying the information to the console
		 */
		int		getMode_LogConsole ();
		
		/** Set the ID string displayed when logging to the console.  By default,
		 * the string IL is used such that logging on Adapter eth0 would produce
		 * IL(eth0).  The ID string may be changed to differentiate different logging
		 * modules along the InputModule chain of the Adapter.
		 * @param sConsoleID A new string to denote the logging module
		 */  
		void		setConsoleID (string sConsoleID);
		
		/** Retrieve the ID displayed when logging to the console.  The ID can be
		 * set to identify logging modules at different points along the input
		 * module chain.
		 * @returns A valid string object 
		 */
		string	getConsoleID ();
	
		/** Set the flag as to whether or not content should be
		 * logged to a file for examination
		 * @param bVal True if information should be logged to a file, false otherwise */				
		void		setFlag_LogFile   (char bVal);
		
		char		getFlag_LogFile	();

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);
	
		/** Retrieve a copy of the statistics for this input module
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		virtual NodeDOM *		getStats ();	
	
	private:
		/** Should the log module output to the screen. By default, this flag is
		 * enabled 
		 */
		char		m_bLogConsole;
		
		/** The mode regarding the method of display with regards to verbosity and parsing
		 */
		int		m_nConsoleMode;
		
		/** The identifier displayed to the screen for this specific logger. By default,
		 * the identifier will by IL(Adapter Name). 
		 */
		string 	m_sConsoleID;
		
		char		m_bLogFile;
		string	m_sFile;
	
};

#endif /*INPUTLOG_H_*/
