/* Packet.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * $Revision: 1.6 $  $Date: 2008/04/11 22:52:23 $
 **********************************************************
 */

#ifndef __PACKET_H_
#define __PACKET_H_

#include <sys/time.h>					// timeval struct
#include "../mem/MemPoolObject.h"	// Base MemPoolObject class
#include "../core/StdHeaders.h"

#include <vector>
using namespace std;

/** The maximum buffer inside of a given packet */
#define MAX_PKT_SIZE				2000

#define PKT_WRITECALLBACK_TYPE_NONE		-1

// A packet signature may be attached as applicable to the packet for
//  re-use rather than having each module compute a signature separately
class PacketSignature;

class IOModule;

/** An individual packet read in by the adapter or a packet
 * to be sent out 
 */
class Packet : public MemPoolObject {
	public:
		Packet ();
		~Packet ();

		/** Attempt to retrieve a packet object from the global management pool
		 * and if necessary, allocate a new object.
		 * @returns A pointer to a valid Packet object
		 */		
		static Packet *	createPacket ();

		/** Return a string describing the class info. This is similar to
		 * the Java basic dump that describes the class name.
		 * @returns A valid string containing the class information
		 */
		virtual	string getClassInfo ();
		
		/** Retrieve a pointer to the arrival time associated with this
		 *  packet
		 * @returns A pointer to the internal arrival time structure 
		 */
		struct timeval * getArrivalTime ();
		
		/** Associate an arrival or creation time with this specific packet
		 * @param pTime The new arrival or creation time of the packet
		 */
		void		setArrivalTime (struct timeval * pTime);
		
		/** Retrieve the length of the packet data 
		 * @returns The length of the packet data in bytes
		 */
		int		getLength ();
		
		/** Set the length of the packet data
		 * @param nSize The new length of the packet data
		 */
		void		setLength (int nSize);
		
		/** Retrieve a pointer to the beginning of the packet data. The
		 * length of the data is subject to MAX_PKT_SIZE.
		 * @returns A non-NULL byte accessible pointer to the data
		 */
		char *	getData   ();
		
		/** Set the data and length of the data. Data that is over length
		 * of MAX_PKT_SIZE will be silently discarded and truncated.
		 * @param nLength The size of the data to copy in
		 * @param pData  A pointer to the source of the data
		 */
		void		setData (int nLength, char * pData);

		/** Clean the object before giving it back from the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		void clean ();
		
		
		/** Clone (copy) the packet using an allocation from the global
		 * memory pool.
		 * @returns A valid pointer to a Packet object copied from this
		 * object.
		 */
		Packet *		cloneFromPool ();
		
		/** Add a signature to the packet that may be re-used by other
		 * modules rather than recomputing multiple times
		 * @param pSig A valid pointer to a signature
		 */
		void			addSignature	(PacketSignature * pSig);
		
		/** Set the next nPad bytes after the end of the packet data to
		 * zero for the purposes of computing signatures
		 * @param nPad The number of bytes (positive) to force to zero
		 * at the end of the data
		 */
		void			zeroPad			(int nPad);
		
		/** Set the packet to have a resume module that causes it to
		 * start the processing chain after a specific point in the chain
		 * rather than starting at the beginning of the chain every
		 * time. By default, packets will always start processing at
		 * the beginning of the chain.
		 * @param pModule	Module to set the packet for (NULL means none)
		 */
		void		setResumeAfterModule (IOModule * pModule);
		
		/** Determine if the packet processing should resume after a particular
		 * point in the chain rather than starting from the beginning of
		 * the chain
		 * @returns NULL if resume is not ncessary, a non-NULL value denotes a
		 *   		pointer to the module that should be invoked for processing
		 */
		IOModule *	getResumeAfterModule ();
		
		/** Enable a callback when this packet is actually discarded or
		 * written out to a module along with some sort of identifying
		 * data.  Only one register callback is allowed at present to
		 * minimize the per packet overhead but a vector of callbacks
		 * may be added if necessary
		 * @param pCallModule The module to call back to
		 * @param pData The generic data to associate with the callback
		 * @param nType The type which is by default -1 (unknown) can be useful when multiple callbacks can be invoked
		 * @returns True if successful, false if the spot is otherwise occupied
		 */
		bool			registerWriteCallback (IOModule * pCallModule, void * pData, int nType=PKT_WRITECALLBACK_TYPE_NONE);
		
		/** Return the data associated with the callback allowing one to "cheat"
		 * by not really doing a callback and just placing data out there with
		 * the packet that can then be referred to later.
		 */
		void *		getWriteCallbackData  ();
		
		/** Attempt to invoke a callback
		 * @returns True if there was one invoked, false otherwise
		 */
		bool			invokeWriteCallback ();
		
		/** Determine if tracing is enabled for the purpose of debugging where
		 * the packet is going in the ScaleBox infrastructure 
		 * @returns True if tracing is enabled for this packet, false otherwise
		 */
		bool			isTraceEnabled ();
		
		/** Enable tracing for this particular packet such that when it hits
		 * core parts of the architecture, the architecture will denote where
		 * the packet is going
		 */
		void			enableTracing ();
		
		/** Disable tracing for this particular packet. Tracing by default is
		 * disabled for all packets.
		 */		
		void			disableTracing ();
		
		/** Return the maximum size that one can use with this type of packet
		 * @returns Maximum size of a packet (in bytes)
		 */
		virtual unsigned int	getMaxSize ();
		

		void	showHex 	(int nStart, int nStop);
		void	showText 	(int nStart, int nStop);

		uint16_t	forceUnusedZero ();


	private:
		char 		m_byData[MAX_PKT_SIZE];
		int	 	m_nLength;
						
		// When was the packet created or when did it arrive to the system?
		struct timeval		m_arrTime;			
	
		vector<PacketSignature *>		m_sigList;
		
		/** The resume module allows a packet to resume in the middle of
		 * the chain rather than entirely from the start of an input or
		 * output chain. 
		 */
		IOModule *		m_pResumeModule;
		
		// Callback module and data
		IOModule	*		m_pWriteCallback;
		void		*		m_pWriteCallbackData;
		int					m_nWriteCallbackData_Type;
		
		/** Tracing state for the packet itself allowing specific packet
		 * debugging rather than watching all of the packets passing 
		 * through. The value is false by default.
		 */
		bool				m_bShouldTrace;
};



#endif /*PACKET_H_*/
