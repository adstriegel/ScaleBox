#ifndef FILEREPLAYIO_H_
#define FILEREPLAYIO_H_

#include <string>
using namespace std;

#include "../pkt/IOModule.h"

#include <pthread.h>


/** Replay a given source file (tcpdump or otherwise) onto the
 * specified adapter stream (typically a virtual adapter)
 */
class FileReplayModule : public IOModule {
	public:
		FileReplayModule ();
		~FileReplayModule ();

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
		
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		 bool		extractExtendedDOM (NodeDOM * pNode);	
		 
		 /** Retrieve the file name associated with this playback
		  * @returns A copy of the file name string
		  */
		 string		getFileName ();
		 
		 /** Set the file name to be used when preparing to send information
		  * into the adapter / module information flow
		  * @param sFile The file name to open
		  */
		 void		setFileName (string sFile);
		 
	private:
		string		m_sFileName;
		
		/** Thread for reading from the file */
		pthread_t		m_ThreadFileRead;			
};

void * Thread_FileReplay (void * pArg);

#endif /*FILEREPLAYIO_H_*/
