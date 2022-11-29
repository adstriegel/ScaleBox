
#include <iostream>
#include <string>
using namespace std;

#include <arpa/inet.h>
#include <string.h>

#include "../util/ip-utils.h"
#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"
#include "FilterLayer.h"
#include "Packet.h"

FilterLayer::FilterLayer() {
	
}

FilterLayer::~FilterLayer () {
	
}

FilterL2Ethernet::FilterL2Ethernet () {
	m_bFilterSrcMAC = false;
	m_bFilterDstMAC = false;
	m_bFilterTypeLen = false;
}		

FilterL2Ethernet::~FilterL2Ethernet () {
	
}

char	FilterL2Ethernet::matchFilter (Packet * pPacket) {
	//cout << "      -> matchFilter (FilterL2Ethernet)" << endl;
	
	// We operate from a pass/no pass. If it does not violate any of the
	//  requested matches, it passes
	
	if(m_bFilterSrcMAC) {
		if(memcmp(m_bySrcMAC, pPacket->getData()+0, ETH_MAC_LENGTH) != 0) {
			return 0;
		}
	}
	
	if(m_bFilterDstMAC) {
		if(memcmp(m_byDstMAC, pPacket->getData()+ETH_MAC_LENGTH, ETH_MAC_LENGTH) != 0) {
			return 0;
		}		
	}
	
	if(m_bFilterTypeLen) {
		if(memcmp(m_byTypeLen, pPacket->getData()+ETH_MAC_LENGTH*2, ETH_MAC_TYPELEN) != 0) {
			return 0;
		}
	}
	
	return 1;	
}
		
void	FilterL2Ethernet::enableFilterSrcMAC (bool bEnable) {
	m_bFilterSrcMAC = bEnable;
}

void	FilterL2Ethernet::setFilterSrcMAC (char * byMAC) {
	memcpy(m_bySrcMAC, byMAC, ETH_MAC_LENGTH);
}

void	FilterL2Ethernet::enableFilterDstMAC (bool bEnable) {
	m_bFilterDstMAC = bEnable;	
}

void	FilterL2Ethernet::setFilterDstMAC (char * byMAC) {
	memcpy(m_byDstMAC, byMAC, ETH_MAC_LENGTH);	
}

void	FilterL2Ethernet::enableFilterTypeLen (bool bEnable) {
	m_bFilterTypeLen = bEnable;	
}

void	FilterL2Ethernet::setFilterTypeLen (char * byTypeLen) {
	memcpy(m_byTypeLen, byTypeLen, ETH_MAC_TYPELEN);
}

bool	FilterL2Ethernet::populateFromDOM (NodeDOM * pNode) {
	int			j;
	
	// Iterate through the elements
	for(j=0; j<pNode->getNumElements(); j++) {
		string		sElem;	
		
		sElem = pNode->getElement(j)->getName();
		
		if(sElem == "TypeLen") {
			unsigned short		nTypeLen;
			
			sscanf(pNode->getElement(j)->getValue().c_str(), "0x%hX", &nTypeLen);
			
			printf("  TypeLen is 0x%X\n", nTypeLen);
			
			m_bFilterTypeLen = true;			
		} else {
			cerr << "Warning: Unknown filter component for the Ethernet filter" << endl;
			cerr << "  Requested filter of " << sElem << " will be ignored" << endl;	
		}
	}
	
	if(m_bFilterTypeLen || m_bFilterDstMAC || m_bFilterDstMAC) { 
		return true;	
	} else {
		cerr << "Error: No actual filter fields specified for the Ethernet filter" << endl;
		return false;
	}
}

FilterL3IPv4::FilterL3IPv4 ()
{
	m_bFilterSrcIP = false;
	m_bFilterDstIP = false;

	m_bFilterProtocol = false;
}

FilterL3IPv4::~FilterL3IPv4 ()
{

}

void FilterL3IPv4::enableFilterSrcIP (bool bEnable)
{
	m_bFilterSrcIP = bEnable;
}

void FilterL3IPv4::setFilterSrcIP_Mask (char * pMask)
{
	memcpy(m_bySrcMask, pMask, 4);
}


void FilterL3IPv4::setFilterSrcIP_Result (char * pResult)
{
	memcpy(m_bySrcResult, pResult, 4);
}

void FilterL3IPv4::enableFilterDstIP (bool bEnable)
{
	m_bFilterDstIP = bEnable;
}

void FilterL3IPv4::setFilterDstIP_Mask (char * pMask)
{
	memcpy(m_byDstMask, pMask, 4);
}


void FilterL3IPv4::setFilterDstIP_Result (char * pResult)
{
	memcpy(m_byDstResult, pResult, 4);
}

void FilterL3IPv4::enableFilterProtocol (bool bEnable)
{
	m_bFilterProtocol = bEnable;
}

void FilterL3IPv4::setFilterProtocol (unsigned char byProtocol)
{
	m_byProtocol = byProtocol;
}

bool FilterL3IPv4::populateFromDOM (NodeDOM * pNode)
{
	int			j;

	cout << "  Populating L3 Filter with " << pNode->getNumElements() << " elements to parse " << endl;

	// Iterate through the elements
	for(j=0; j<pNode->getNumElements(); j++) {
		string		sElem;

		sElem = pNode->getElement(j)->getName();

		cout << "Testing element in IPv4 L3 Filter of name " << sElem << endl;

		if(sElem == "SrcIPMask") {
			if(inet_pton(AF_INET, pNode->getElement(j)->getValue().c_str(), m_bySrcMask) == 1)
			{
				m_bFilterSrcIP = true;
			}
			else
			{
				cerr << " inet_pton failed for source IP mask of " << pNode->getElement(j)->getValue() << endl;
			}
		}
		else if(sElem == "SrcIPResult") {
			if(inet_pton(AF_INET, pNode->getElement(j)->getValue().c_str(), m_bySrcResult) == 1)
			{
				m_bFilterSrcIP = true;
			}
			else
			{
				cerr << " inet_pton failed for source IP result of " << pNode->getElement(j)->getValue() << endl;
			}
		}
		else if(sElem == "DstIPResult") {
			if(inet_pton(AF_INET, pNode->getElement(j)->getValue().c_str(), m_byDstResult) == 1)
			{
				m_bFilterDstIP = true;
			}
			else
			{
				cerr << " inet_pton failed for dest IP mask of " << pNode->getElement(j)->getValue() << endl;
			}
		}
		else if(sElem == "DstIPResult") {
			if(inet_pton(AF_INET, pNode->getElement(j)->getValue().c_str(), m_byDstResult) == 1)
			{
				m_bFilterDstIP = true;
			}
			else
			{
				cerr << " inet_pton failed for dest IP result of " << pNode->getElement(j)->getValue() << endl;
			}
		}
		else if(sElem == "Protocol") {
			uint8_t		byProtocol;

			sscanf(pNode->getElement(j)->getValue().c_str(), "%d", &byProtocol);
			setFilterProtocol(byProtocol);
			enableFilterProtocol(true);
		}

		else {
			cerr << "Warning: Unknown filter component for the Layer 3 IPv4 filter" << endl;
			cerr << "  Requested filter of " << sElem << " will be ignored" << endl;
		}
	}

	if(m_bFilterProtocol || m_bFilterDstIP || m_bFilterSrcIP) {
		return true;
	} else {
		cerr << "Error: No actual filter fields specified for the Layer 3 IPv4 filter" << endl;
		return false;
	}
}

char FilterL3IPv4::matchFilter (Packet * pPacket)
{
	int 		j;

	// Is the packet even IP?  If it is not, it passes
	if(!isPacketIP(pPacket->getData(), pPacket->getLength()))
	{
		return 0;
	}

	//cout << "  Filter check (Layer 3 - IPv4)" << endl;

	if(m_bFilterSrcIP)
	{
		uint8_t		byResult[4];

		for(j=0; j<4; j++)
		{
			byResult[j] = *(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP+j) & m_bySrcMask[j];
		}

		if(memcmp(byResult, m_bySrcResult, 4) != 0)
		{
			//cout << "      Failed Src IP filter -> Net mask result was ";
			//dumpIPv4((char *) byResult);
			//cout << endl;

			return 0;
		}
	}

	if(m_bFilterDstIP)
	{
		uint8_t		byResult[4];

		for(j=0; j<4; j++)
		{
			byResult[j] = *(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP+j) & m_byDstMask[j];
		}

		if(memcmp(byResult, m_byDstResult, 4) != 0)
		{
			//cout << "      Failed Dest IP filter -> Net mask result was ";
			//dumpIPv4((char *) byResult);
			//cout << endl;

			return 0;
		}
	}

	if(m_bFilterProtocol)
	{
		if(m_byProtocol != *(pPacket->getData() + L2_OFFSET + OFFSET_IPV4_PROTOCOL))
		{
			return 0;
		}
	}

	return 1;
}

FilterLayer * createFilterLayer_FromDOM (NodeDOM * pNode) {
	// Validate the object is not NULL
	if(pNode == NULL) {
		return NULL;
	}
	
	// Select the appropriate case
	if(pNode->getTag() == "Ethernet") {
		FilterL2Ethernet 	*	pEthernet;
		
		pEthernet = new FilterL2Ethernet();
		
		if(pEthernet->populateFromDOM(pNode)) {
			return pEthernet;
		} else {
			delete pEthernet;
			return NULL;
		}
	}
	else if (pNode->getTag() == "IPv4")
	{
		FilterL3IPv4		* pIPv4;

		pIPv4 = new FilterL3IPv4();

		if(pIPv4->populateFromDOM(pNode)) {
			return pIPv4;
		}
		else
		{
			delete pIPv4;
			return NULL;
		}
	}
	else {
		cerr << "Warning: Unknown tag for layer filter of >> " << pNode->getTag() << " << " << endl;
		cerr << "   Ignoring the tag" << endl;	
		return NULL;
	}
	
	return NULL;	
}		
	
