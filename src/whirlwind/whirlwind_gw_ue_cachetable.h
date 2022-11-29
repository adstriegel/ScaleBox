/*
 * whirlwind_gw_ue_cachetable.h
 *
 * The model of the User Element (UE) cache table as envisioned by the Whirlwind Gateway.
 */

#ifndef WHIRLWIND_GW_UE_CACHETABLE_H_
#define WHIRLWIND_GW_UE_CACHETABLE_H_


#include "../pkt/Packet.h"
#indlude "../stat/Stats.h"

/** An entry in the table of a user element (UE) as viewed by the Whirlwind Gateway
 */
class Whirlwind_GW_UE_CacheEntry
{
	public:
		Whirlwind_GW_UE_CacheEntry ();
		~Whirlwind_GW_UE_CacheEntry ();

		/** What is the offset into the data payload on which this particular signature is
		 * drawn?
		 * @returns Offset (in bytes) into the packet payload
		 */
		unsigned int       getDataOffset ();

		/** Set the byte-wise offset into the data payload on which the signature is drawn
		 * @param nOffset Byte-wise offset into the data payload
		 */
		void               setDataOffset (unsigned int nBytes);

		struct timeval*    getLastUsedTime ();
		void               setLastUsedTime (struct timeval* pVal);

		struct timeval*    getCreationTime ();
		void               setCreationTime (struct timeval* pVal);
  

		/** Get a pointer to the packet payload
		 * @returns A non-null pointer to the original packet payload
		 */
		unsigned char*	getData ();	

		/** 
		 * Copy the content from a particular source (likely a packet payload) into this particular
		 * cache entry
		 * @param pData    The payload to copy
		 * @param nLength  The number of bytes to copy
		 * @returns True if successful, false otherwise
		 */
		bool            copyPayload (unsigned char* pData, int nLength);


	private:
		// What is the signature?

		// What is the offset into the data?
		unsigned int		        m_nOffset;

		// What is the actual data (up to a full packet payload)?
		Whirlwind_Managed_Buffer	m_Buffer;

		// When was it created?
                struct timeval        m_CreationTime;

		// When was it last accessed?
		struct timeval        m_LastUsedTime;

		// What is the state of the entry (confirmed, should be there, being evicted, etc)?

		// Statistics for this particular entry

		// Detailed statistics for the entry


};

class Whirlwind_GW_UE_CacheTable
{
	public:
		Whirlwind_GW_UE_CacheTable ();
		~Whirlwind_GW_UE_CacheTable ();

};



#endif /* WHIRLWIND_GW_UE_CACHETABLE_H_ */
