/* PktQueue.h
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
 * $Revision: 1.5 $  $Date: 2008/04/11 22:01:03 $
 **********************************************************
 */
 
 #ifndef PKTQUEUE_H_
 #define PKTQUEUE_H_
 
 #include <pthread.h>
 
 #include "IOModule.h"
 
 /** The basic packet queue as included in the base adapter class. The
  * queue may be overriden to react to various parameters. The queue itself
  * will receive packets added by virtue of the addOutputQueue function of
  * the adapter and packets removed by virtue of the adapter write thread.
  * Note that this class is primarily a virtual class and that the PktQueueFIFO
  * is the appropriate queue to use for rudimentary queuing behavior.
  */
 class PktQueue : public IOModule {
 	public:
 		PktQueue ();
 		~PktQueue ();	
 	
		/** Initialize an input module by applying any necessary 
		 * startup functionality such as threads, thread safety, or 
		 * other initializations. 
		 * @returns True if successful, false otherwise
		 */
		char		initialize ()=0;	

		/** Start up the module in terms of linking with any other modules.  The
		 * startModule function can assume that all adapters have been declared
		 * at this point.  
		 * @returns True if successful, false if there was a critical error.
		 */
		virtual bool					startModule ()=0;	

		/** Process a packet coming along the input chain. Appropriately
		 * process the packet and/or create packets for output based on this
		 * packet. The actual chaining is taken care of at the Adapter level
		 * when the input chain is started upon reception of a packet.
		 * @param pPacket A valid Packet object for processing
		 * @returns 1 if this function has gobbled it (no more processing) or
		 *          0 if the packet should be processed by future modules
		 */ 
		virtual char		processPacket (Packet * pPacket)=0;
	
		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the 
		 * extended information specific to the child class. 
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */		
		virtual bool		extractExtendedDOM (NodeDOM * pNode)=0;
	
		/** Retrieve a copy of the statistics for this input module
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		virtual NodeDOM *	getStats ()=0;	 	
 	
 		
 		/** Retrieve the size of the queue in packets 
 		 * @returns The size of the queue in packets */
 		virtual int getNumPackets ()=0;
 		
 		/** Retrieve the size of the queue in bytes
 		 * @returns The size of the queue in bytes */
 		virtual int getSizePackets ()=0;

 		/** Retrieve a specific packet from the queue of packets
 		 * @returns A pointer to a valid packet object if in the range of
 		 *    packets or NULL if out of range
 		 */
 		virtual Packet *	getPacket (int nPkt)=0;	
 	
		/** Pop the front of the queue to delete the first item (in the element
		 * sense). Note that only the item is removed from the underlying list, the
		 * Packet object associated with the front element is not touched and
		 * should still be valid.
		 * @returns True if successful, false otherwise
		 */
		virtual bool 	popFront ()=0; 	
 	
 		/** Enqueue a packet in the queue as appropriate 
 		 * @returns True if successful, false is unable to queue 
 		 * */
 		virtual bool 	enqueue (Packet * pPacket)=0;
 	
  		/** Dump information related to this queue 
 		 */
 		virtual void		dumpInfo ();	
 	
 		/** Do an atomic pop for the next packet (dependent on queue behavior)
 		 * and return that packet as a result if there is data in the
 		 * queue
 		 * @returns A pointer to a valid Packet object if there is data in
 		 *   the queue
 		 */
 		virtual Packet *	popAndGetPacket ()=0;
 	
 	protected:
 		/** Retrieve a pointer to the mutex for the queue that guards all
 		 * manipulation of the queue itself
 		 * @returns A valid pointer to the mutex for the queue
 		 */
 		pthread_mutex_t *	getMutexQueue ();
 	
 	private:
 		/** Mutex for the queue */ 	
		pthread_mutex_t	m_MutexQueue;
 	
 	
 	
};
 
#endif

 