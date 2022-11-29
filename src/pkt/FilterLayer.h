#ifndef FILTERLAYER_H_
#define FILTERLAYER_H_

class Packet;
class NodeDOM;

#include "../core/StdHeaders.h"

#include "../util/net-const.h"

/** The base class for filter according to a layer.
 */
class FilterLayer { 
	public:
		FilterLayer();
		~FilterLayer ();
		
		/** Given an XML/DOM input node, extract the relevant
		 * information to puplate the filter layer
		 * @param pNode The DOM node to process
		 * @returns True if successful, false if there was an error
		 */
		virtual bool		populateFromDOM (NodeDOM * pNode)=0;
		
		/** Given a packet as an input, determine if the
		 * packet matches the filter
		 * @param pPacket The packet to examine
		 * @returns 1 if a match, 0 if not a match
		 */
		virtual char		matchFilter (Packet * pPacket)=0;
		
	private:	
		
};

/** A filter instance for Layer 2 Ethernet (802 dot 3).
 */
class FilterL2Ethernet : public FilterLayer {
	public:
		FilterL2Ethernet ();
		~FilterL2Ethernet ();

		/** Given a packet as an input, determine if the
		 * packet matches the filter
		 * @param pPacket The packet to examine
		 * @returns 1 if a match, 0 if not a match
		 */
		char		matchFilter (Packet * pPacket);
		
		void		enableFilterSrcMAC (bool bEnable);
		void		setFilterSrcMAC (char * byMAC);

		void		enableFilterDstMAC (bool bEnable);
		void		setFilterDstMAC (char * byMAC);

		void		enableFilterTypeLen (bool bEnable);
		void		setFilterTypeLen (char * byTypeLen);
		
		/** Given an XML/DOM input node, extract the relevant
		 * information to puplate the filter layer
		 * @param pNode The DOM node to process
		 * @returns True if successful, false if there was an error
		 */
		bool		populateFromDOM (NodeDOM * pNode);		
		
	
	private:	
		bool 		m_bFilterSrcMAC;
		char		m_bySrcMAC[ETH_MAC_LENGTH];
		
		bool		m_bFilterDstMAC;
		char		m_byDstMAC[ETH_MAC_LENGTH];
		
		bool		m_bFilterTypeLen;
		char		m_byTypeLen[ETH_MAC_TYPELEN];
	
};

/** A filter for layer 3 IPv4 information
 */
class FilterL3IPv4 : public FilterLayer {
	public:
		FilterL3IPv4 ();
		~FilterL3IPv4 ();
	
		/** Given a packet as an input, determine if the
		 * packet matches the filter
		 * @param pPacket The packet to examine
		 * @returns 1 if a match, 0 if not a match
		 */
		char		matchFilter (Packet * pPacket);

		void		enableFilterSrcIP (bool bEnable);

		/* Set the filter for the source IP.  The mask is the netmask that should be applied and
		 * the result is what is tested against post mask (bitwise AND).  For an exact match, use
		 * the full slash 32 (ex. 255.255.255.0).  For subnets, use the appropriate contiguous match.
		 */
		
		void		setFilterSrcIP_Mask   (char * pMask);
		void		setFilterSrcIP_Result (char * pResult);

		void		enableFilterDstIP (bool bEnable);

		void		setFilterDstIP_Mask   (char * pMask);
		void		setFilterDstIP_Result (char * pResult);


		void		enableFilterProtocol (bool bEnable);
		void		setFilterProtocol (unsigned char byProtocol);
	
		/** Given an XML/DOM input node, extract the relevant
		 * information to puplate the filter layer
		 * @param pNode The DOM node to process
		 * @returns True if successful, false if there was an error
		 */
		bool		populateFromDOM (NodeDOM * pNode);			
	
	
	private:	
		bool		m_bFilterSrcIP;
		char		m_bySrcMask[4];
		char		m_bySrcResult[4];
		
		bool		m_bFilterDstIP;
		char		m_byDstMask[4];
		char		m_byDstResult[4];
		
		bool		m_bFilterProtocol;
		unsigned char		m_byProtocol;
};

/** Create a filter layer object and populate it from a given
 * DOM/XML node object. The returned object is the responsibility
 * of the callee.
 * @param A pointer to the DOM node to process containing the
 *    information as elements of the node
 * @returns A valid FilterLayer object if created successfully,
 *    otherwise NULL if incomplete or invalid information existed
 */
FilterLayer	*	createFilterLayer_FromDOM (NodeDOM * pNode);

#endif /*FILTERLAYER_H_*/
