

#ifndef __APPBASE_H_
#define __APPBASE_H_



class ApplicationBase : public IOModule {
	public:
		ApplicationBase ();
		~ApplicationBase ();
		
		/** Packet processing in the rawest form that gives all of the data
		 * for the packet (L2 and up). In general, this function should not be
		 * overridden by a derived class unless absolutely necessary.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		virtual char		processPacket (Packet * pPacket);

		/** Initialize through any necessary startup functionality such
		 * as threads, thread safety, or other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();		
		
		
	private:
		
};

/** The base thread for an application that constitutes the main thread
 * of an app
 * @param pApp A pointer to the application object being managed by the
 *   thread
 */
void * Thread_Application (void * pApp);

#endif
