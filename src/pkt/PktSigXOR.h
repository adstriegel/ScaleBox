#ifndef PKTSIGXOR_H_
#define PKTSIGXOR_H_

#include "PacketSignature.h"

class Packet;

/** The XOR-based signature of a given packet. The purpose of the
 * XOR code is primarily for testing and flow purposes with other
 * signatures such as MD5 (PktSigMD5) and others offering significantly
 * more robust signatures.
 */
class PktSigXOR : public PacketSignature  {
	public:
		PktSigXOR ();
		~PktSigXOR ();	
	
		/** Compute the signature as appropriate and fill the internal copy of
		 * the requested signature 
		 * @param pReq The parameters generating this signature as a valid pointer to
		 *  a SignatureRequest struct
		 * @param pPacket The packet to compute the signature for
		 * @returns 1 if successful, 0 if failure 
		 */
		char		computeSignature (SignatureRequest * pReq, Packet * pPacket);
	
		/** Clean the object before giving it back from the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		void clean ();	
			
	private:
	
		/** The checksum as recorded by a running XOR with zero padding at the
		 * end as appropriate 
		 */
		int			m_nChecksum;
	
};

/** Derive an XOR-based signature of the payload as specified in the SignatureRequest
 * structure on the information contained within the specified packet
 * @param pReq	A pointer to a valid signature request. Information of note is
 *  the offset and offset type.
 * @param pPacket A pointer to a valid packet for computing the signature
 * @returns A valid XOR packet signature for appending to the list of packet
 * signatures
 */
PktSigXOR *		computeXOR (SignatureRequest * pReq, Packet * pPacket);

#endif /*PKTSIGXOR_H_*/
