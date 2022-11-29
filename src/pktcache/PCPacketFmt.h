#ifndef PCPACKETFMT_H_
#define PCPACKETFMT_H_

// Definition of the packet formats and respective structs / classes
//  to support creation / extraction content

/** Protocol field for the IP header to use to identify packet cache
 * packets out versus ICMP, UDP, and TCP packets. The number is simply
 * used to differentiate packets and is not fixed at this time.
 */
#define PROTO_PCACHE		128

/** Given a pointer to the beginning of the IP header, determine if the
 * packet is indeed a packet destined for the packet cache protocol
 * @param byData Pointer to the beginning of the IP data
 * @returns True if the packet is for packet caching, false otherwise
 */ 
bool		isPacketCacheProtocol (char * byData);

// Packet cache packet format

// Intro header - all packet cache packets
//
// |--------|---------|--------|-------|  
// |  Type  |   Len   | Flags  |  Rsvd |
// |--------|---------|--------|-------|  
//
// Type	8 bits	Type of the packet (data, discovery, etc)
//	Len	8 bits	Length of core headers before content in 4 byte blocks
//	Flags	8 bits	TBD
//	Rsvd 	8 bits	TBD (included to round to 32 bits)
//

#define	PCACHEHDR_OFFSET_TYPE		0
#define 	PCACHEHDR_OFFSET_LEN			1
#define 	PCACHEHDR_OFFSET_FLAGS		2
#define 	PCACHEHDR_OFFSET_RSVD		3

/** Retrieve the packet cache type from the packet cache packet given the
 * start of the packet cache data header
 * @param pData A pointer to the beginning of the packet cache data
 * @returns The type of the packet cache packet retrieved from the header that
 *   maps to a PktCachePacketTypes
 */
int	getPacketCachePktType (char * pData);

/** Translate the packet cache type to a human readable string 
 * @param nType The type of the packet from the header
 * @param pBuffer The location to place the type is as a zero-delimited string
 * @param nBufLength The length of the buffer of where to write
 */
void	translatePacketCacheTypeToString (int nType, char * pBuffer, int nBufLength);

/** Decode the packet cache header into the target string buffer
 * @param pData A pointer to the beginning of the packet cache header
 * @param pBuffer The location of where to place the decoded packet header information
 * @param nBufLength The maximum length of data allowed for the buffer
 */
void	decodePacketCacheBaseHeader (char * pData, char * pBuffer, int nBufLength);

enum 	PktCachePacketTypes {
	PKTCACHE_PKT_TYPE_TOKEN=1,		// Basic type is the data (tokenized) packet
	PKTCACHE_PKT_TYPE_MISS_REQ=2,	// Request for retransmission by child to source cache
	PKTCACHE_PKT_TYPE_MISS_RSP=3,	// Miss response from source to child
};
 
// Packet cache tokenized data packet
//
// |--------|---------|--------|-------|  
// | Tokens | TT | TC | Flags  |  Rsvd |
// |--------|---------|--------|-------|  
// |   L4 content if applicable        |
// |             ....                  | 
// |--------|---------|--------|-------|  
// |   Token 0 + support data          |
// |             ....                  | 
// |--------|---------|--------|-------|  
// |   Token 1 + support data          |
// |             ....                  | 
// |--------|---------|--------|-------|  
//
// Tokens	8 bits	Number of tokens (0-255)
//	TT			4 bits	Token/fingerprint type
//	TC			4 bits	Token configuration
//	Flags		8 bits	Bit 15	T		TCP (1) vs. block (0)
//							Bit 14	A2		Ack gets +2 bits from SN for delta differentiation (TCP only)
//							Bit 13	C		Checksum enabled		 	
//
// Layer 4 Content - TCP (if T flag is enabled)
//
// |--------|---------|--------|-------| 
//	|       DP         |       SP       |
// |--------|---------|--------|-------| 
// |         Base Seq Number           |
// |--------|---------|--------|-------| 
// |         Base Ack Number           |
// |--------|---------|--------|-------| 
//
//	DP		16 bits		TCP destination port
//	SP		16 bits		TCP source port
//	SN		32 bits		Base sequence number
// AN		32 bits		Base acknowledgment number
//
// The SN and AN form the basis from which the delta on each token is computed
//

// Tokenized Entry - TCP with delta, unique content enabled
//
// |--------|---------|--------|-------|
// |		  		Token	X					   |
// |--------|---------|--------|-------|
// |      Length      |  Flags | TCP FL| 
// |--------|---------|--------|-------|
// |   L4   |      Pre    |    Post    |
// |--------|---------|--------|-------|
// |   Delta SN       |   Delta AN     | 
// |--------|---------|--------|-------|
// |    L4 option data (if present)*   |
// |--------|---------|--------|-------|
// |    Pre - unique data before token*|
// |--------|---------|--------|-------|
// |  Post - unique data before token* |
// |--------|---------|--------|-------|
//
// Token X		N bits (typically 32)		Token / fingerprint X
// Length		16 bits							Length of untokenized data
// Flags  		8 bits							Flags - to be determined
//	TCP Flags	8 bits							Flags from original TCP packet
// L4				4 bits							Length of L4 data in 4 byte blocks
//	Pre			14 bits							Length of pre-token unique data (bytes)
// Post			14 bits							Length of post-token unique data (bytes)
// Delta SN		16 bits							Delta of sequence number versus end of last token data
// Delta AN		16 bits							Delta of ack number versus cumulative ACK
// L4 option data		??							TCP options data (may not be present)
//	Pre					??							May not be present
//	Post					??							May not be present
//
// Bleh - delta SN and length are quite redundant to one another.  We could theoretically
//  purge delta SN and use only length + pre + post
//
// The ack number is an interesting dilemma.  In theory, we could just tag the last
//  packet with the highest ACK and be done with it.  However, I am guessing that
//  if we do that, the TCP Options timestamp is not going to appreciate it. This way
//  we can keep on the straight and narrow if absolutely necessary.  
//

/** Compute the length of a token block (token itself and supporting data) based
 * on the information inside the token block
 * @param byData 						Pointer to the beginning of the token block
 * @returns	Size of the token block (in bytes)
 */
int		computeTokenBlockLength	(char * byData); 

/** Retrieve the delta in terms of the sequence number from the token block. 
 * @param pData	Pointer to the beginning of the token block
 * @param bExtAck	Flag denoting if the ACK has been extended by 2 bits of the MSbs of the delta SN
 * @returns Bytes to adjust the sequence number 
 */
unsigned short		getDeltaSNfromTokenBlock (char * pData, bool bExtAck);

/** Retrieve the delta in terms of the ack number from the token block. 
 * @param pData	Pointer to the beginning of the token block
 * @param bExtAck	Flag denoting if the ACK has been extended by 2 bits of the MSbs of the delta SN
 * @returns Bytes to adjust the sequence number 
 */
unsigned short		getDeltaANfromTokenBlock (char * pData, bool bExtAck);

/** Retrieve the length of the layer 4 option data in absolute bytes. The usage of
 * the resulting layer 4 information will vary depending upon the transport layer
 * in question. 
 * @param pData Pointer to the beginning of the token block
 * @returns The length of the L4 information in bytes
 */
unsigned short		getLengthLayerFourfromTokenBlock (char * byData);

/** Retrieve the length of the pre-token unique data in absolute bytes. The 
 * pre-token unique data will appear before the de-tokenized data in the resulting
 * packet.  
 * @param pData Pointer to the beginning of the token block
 * @returns The length of the L4 information in bytes
 */
unsigned short		getLengthPreUniquefromTokenBlock (char * byData);

/** Retrieve the length of the post-token unique data in absolute bytes. The 
 * post-token unique data will appear after the de-tokenized data in the resulting
 * packet.  
 * @param pData Pointer to the beginning of the token block
 * @returns The length of the L4 information in bytes
 */
unsigned short		getLengthPostUniquefromTokenBlock (char * byData);

 
#endif /*PCPACKETFMT_H_*/
