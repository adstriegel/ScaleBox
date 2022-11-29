

/**
 *  RFC 1321 compliant MD5 implementation
 *
 *  Copyright (C) 2001-2003  Christophe Devine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 * Modified By D. Salyers, A. Striegel for encapsulation into a class. 
 */

#ifndef PKTSIGMD5_H_
#define PKTSIGMD5_H_

#include <string>
using namespace std;

#include <iostream>
using namespace std;

#include <istream>
using namespace std;

#include "PacketSignature.h"

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

/** MD5 context struct.  Not Needed outside of the MD5 class */
typedef struct
{
    uint32 total[2];
    uint32 state[4];
    uint8 buffer[64];
} md5_context;

/** MD5 value structure */
typedef struct
{
    uint32 value[4];
} md5_val;

class PktSigMD5 : public PacketSignature  {
	public:
		PktSigMD5 ();
		
      /** The MD5 Copy Constructor */
      PktSigMD5(const PktSigMD5 &copy);		
		
		~PktSigMD5 ();	
		
		/** Clean the object before giving it back from the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		void clean ();			
	
		/** Compute the signature as appropriate and fill the internal copy of
		 * the requested signature 
		 * @param pReq The parameters generating this signature as a valid pointer to
		 *  a SignatureRequest struct
		 * @param pPacket The packet to compute the signature for
		 * @returns 1 if successful, 0 if failure 
		 */
		char		computeSignature (SignatureRequest * pReq, Packet * pPacket);	

	  	/** Calculates the MD5 of the input.
	   @param input Pointer to the input that the MD5 will be
	                calculated over.
	   @param lenght The length of the input.
		*/
	  	void calcMD5(uint8 *input, uint32 length);
	
	  	/** Returns the MD5 value.  The value has to have been
	  	 * calculated first!
	  	 * @return The MD5 value of the input
	  	 */
	  	md5_val getMD5();
	
	  	/** Overload of the assignment operator.
	  	 * @param copy The MD5 object that will be 
	  	 * @return The MD5 copy
	  	 * 
	  	 */
	  	PktSigMD5 &operator=(const PktSigMD5 &copy);
	
	  	void setMD5(char md5Sum[]);
	
	  	/** Overload of the = operator. Works as expected. */
	  	friend bool operator== (const PktSigMD5 &a, const PktSigMD5 &b);
	
	  	/** Overload of the != operator. Works as expected. */
	  	friend bool operator!= (const PktSigMD5 &a, const PktSigMD5 &b);
	
	  	/** Overload of the > operator. Works as expected. */
	  	friend bool operator> (const PktSigMD5 &a, const PktSigMD5 &b);
	
		/** Overload of the < operator. Works as expected. */
  		friend bool operator< (const PktSigMD5 &a, const PktSigMD5 &b);
	
	  	/** Overload of the >= operator. Works as expected. */
	  	friend bool operator>= (const PktSigMD5 &a, const PktSigMD5 &b);
	
	  	/** Overload of the <= operator. Works as expected. */
	  	friend bool operator<= (const PktSigMD5 &a, const PktSigMD5 &b);
	
	  	//friend istream &operator >>(istream &in, MD5 &checksum);
	  	friend ostream &operator<<(ostream &out, const PktSigMD5 &checksum);
	  	
    private:

        /** Initializes the MD5 calculation variables To start
             the MD5 calculation process
         */
        void start();

        /** Starts the MD5 calculation.
            @param input The data to calculate the MD5 over.
            @param length The length of the data.
         */
        void update(uint8 *input, uint32 length);

        /** Finalizes the MD5 value.
         */
        void finish();

        /** Processes 64 bytes of data.
         */
        void process(uint8 data[64]);

        /** The MD5 context varaible */
        md5_context m_ctx;

        /** The generated MD5 value */
        md5_val m_value;	
};

/** Derive an MD5-based signature of the payload as specified in the SignatureRequest
 * structure on the information contained within the specified packet
 * @param pReq	A pointer to a valid signature request. Information of note is
 *  the offset and offset type.
 * @param pPacket A pointer to a valid packet for computing the signature
 * @returns A valid MD5 packet signature for appending to the list of packet
 * signatures
 */
PktSigMD5 *		computeMD5 (SignatureRequest * pReq, Packet * pPacket);

#endif /*PKTSIGMD5_H_*/

