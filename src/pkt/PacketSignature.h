#ifndef PACKETSIGNATURE_H_
#define PACKETSIGNATURE_H_

#include "../mem/MemPoolObject.h"
#include "../core/StdHeaders.h"
#include "InputSignature.h"

class		Packet;

/** A generic base class for deriving signatures or checksums
 * that identify a packet. The signature allows for comparisons
 * of the same type (MD5, Rabin, XOR).
 */
class PacketSignature : public MemPoolObject {
	public:
		/** Construct a new PacketSignature object
		 * @param nType The type that is unique with respect to the
		 * overall list of memory managed objects
		 */
		PacketSignature (int nType);
		
		~PacketSignature ();
		
		/** Clean the object before giving it back from the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		virtual void clean ()=0;	
	
		/** Compute the signature as appropriate and fill the internal copy of
		 * the requested signature 
		 * @param pReq The parameters generating this signature as a valid pointer to
		 *  a SignatureRequest struct
		 * @param pPacket The packet to compute the signature for
		 * @returns 1 if successful, 0 if failure 
		 */
		virtual char		computeSignature (SignatureRequest * pReq, Packet * pPacket)=0;
	
		/** Return the offset from the packet for which the signature
		 * computation should start
		 * @returns The offset in bytes to start computing the signature from
		 */
		int			getOffset ();
	
	protected:
		/** Populate the information regarding what generated this request
		 * @param pReq The parameters generating this signature as a valid pointer to
		 *  a SignatureRequest struct
		 * @returns 1 if it succeeds, 0 if it fails
		 */
		char		setRequest (SignatureRequest *  pReq);
	
	private:
		/** The signature request that originated the signature */
		SignatureRequest		m_Request;
	
};

#endif /*PACKETSIGNATURE_H_*/
