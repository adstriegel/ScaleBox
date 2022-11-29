/* 802_11.cc                                                                                      
***********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)                                                                                                                   
 * system developed at the University of Notre Dame.
 *
 *
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 *
 * Students: Xueheng Hu
 *          
 **********************************************************
 * Date: 2016/04/05                                                                                                    
 **********************************************************
*/

#include <netinet/in.h>
#include <stdio.h>

#include <iostream>
#include <vector>
using namespace std;

#include "802_11.h"
#include "ip-utils.h"


char isPacketIP_80211 (char* pPacket) {
	if(pPacket[LENGTH_80211 + OFFSET_LLC_TYPE] == LLC_TYPE_IP_B1 && pPacket[LENGTH_80211 + OFFSET_LLC_TYPE + 1] == LLC_TYPE_IP_B2) {
        return 1;
    } else {
        return 0;
    }
}

char isPacketIPSec_80211 (char* pPacket) {
	if(pPacket[LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL] == IP_PROTOCOL_IPSEC_ESP ||
       pPacket[LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL] == IP_PROTOCOL_IPSEC_AH ) {
        return 1;
    } else {
        return 0;
    }
}

string getDstMac_80211 (char* pPacket) {
    string sResult;
    char szTemp[18];

	char* pData = pPacket + OFFSET_80211_DSTMAC;
	snprintf(szTemp, sizeof(szTemp), "%02x:%02x:%02x:%02x:%02x:%02x",
			 (unsigned char)pData[0], (unsigned char)pData[1], (unsigned char)pData[2], (unsigned char)pData[3], (unsigned char)pData[4], (unsigned char)pData[5]);
    sResult = szTemp;

    return sResult;
}

string getSrcMac_80211 (char* pPacket) {
	string sResult;
    char szTemp[18];

    char* pData = pPacket + OFFSET_80211_SRCMAC;
    snprintf(szTemp, sizeof(szTemp), "%02x:%02x:%02x:%02x:%02x:%02x",
             (unsigned char)pData[0], (unsigned char)pData[1], (unsigned char)pData[2], (unsigned char)pData[3], (unsigned char)pData[4], (unsigned char)pData[5]);
    sResult = szTemp;

    return sResult;
}

char getIPVersion_80211 (char* pPacket) {
    char theVer;

    theVer = (pPacket[LENGTH_80211 + LENGTH_LLC] & 0xF0) >> 4;
    theVer = theVer & 0x0F;

    return theVer;
}

unsigned char getIPv4Protocol_80211 (char * pPacket) {
    return (unsigned char) pPacket[LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL];
}

string getIPv4_Dst_80211 (char* pPacket) {
	string sRet;
	char szTemp[21];
	char* pData = pPacket + LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL_DSTIP;

	sprintf(szTemp, "%d.%d.%d.%d", (unsigned char) pData[0], (unsigned char) pData[1], (unsigned char) pData[2], (unsigned char) pData[3]);
    sRet = szTemp;

	return sRet;
}

string getIPv4_Src_80211 (char* pPacket) {
	string sRet;
    char szTemp[21];
    char* pData = pPacket + LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL_SRCIP;

    sprintf(szTemp, "%d.%d.%d.%d", (unsigned char) pData[0], (unsigned char) pData[1], (unsigned char) pData[2], (unsigned char) pData[3]);
    sRet = szTemp;

    return sRet;
}

char hasIPOptions_80211 (char* pData) {
    if(getIP_HeaderSize_80211(pData) != L3_IPV4_OFFSET) {
        return 1;
    } else {
        return 0;
    }
}

unsigned char getIP_HeaderSize_80211 (char* pData) {
    unsigned char nVal;
    nVal = (pData[LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_VERLEN] & 0x0F) << 2;

    return nVal;
}

char isTCP_80211 (char* pPacket) {
    if (pPacket[LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL] == IP_PROTOCOL_TCP) {
        return 1;
    } else {
        return 0;
    }
}

char isUDP_80211 (char* pPacket) {
    if (pPacket[LENGTH_80211 + LENGTH_LLC + OFFSET_IPV4_PROTOCOL] == IP_PROTOCOL_UDP) {
        return 1;
    } else {
        return 0;
    }
}

unsigned char getTCP_HeaderSize_80211 (char* pData) {
    unsigned char nVal;

    // The math is a bit tricky but think of it like this
    //    The offset is in the upper four bits (upper nibble)
    //    The offset is a multiple of 4 bytes (i.e. 32 bits)
    //    The four position in binary starts 2 spots over
    //    If we shift it over two, that puts the value now starting at Bit 2 (aka starting it at multiples of 4) 
    //    We bit-wise AND with 0x3C to whack any sign bits that carried over                                                                           

    if(hasIPOptions_80211(pData)) {
        int nOffset;
        nOffset = getIP_HeaderSize_80211(pData);

        nVal = (pData[LENGTH_80211 + LENGTH_LLC + nOffset + OFFSET_TCP_DATAOFF] >> 2) & 0x3C;
        return nVal;
    } else {
        nVal = (pData[LENGTH_80211 + LENGTH_LLC + L3_IPV4_OFFSET + OFFSET_TCP_DATAOFF] >> 2) & 0x3C;
		return nVal;
    }
}

unsigned short getTCP_SrcPort_80211 (char* pPacket) {
	unsigned short  nPort;
    int nOffset;

    if(hasIPOptions_80211(pPacket)) {
        nOffset = getIP_HeaderSize_80211(pPacket);
    } else {
        nOffset = L3_IPV4_OFFSET;
    }

    memcpy(&nPort, pPacket + LENGTH_80211 + LENGTH_LLC + nOffset + OFFSET_TCP_SRCPORT, 2);
    nPort = ntohs(nPort);

    return nPort;
}

unsigned short getTCP_DstPort_80211 (char* pPacket) {
	unsigned short  nPort;
    int nOffset;

    if(hasIPOptions_80211(pPacket)) {
        nOffset = getIP_HeaderSize_80211(pPacket);
    } else {
        nOffset = L3_IPV4_OFFSET;
    }

    memcpy(&nPort, pPacket + LENGTH_80211 + LENGTH_LLC + nOffset + OFFSET_TCP_DSTPORT, 2);
    nPort = ntohs(nPort);

    return nPort;
}

unsigned int getTCP_SeqNumber_80211 (char * pPacket) {
    unsigned int lSeqNum;

    int nOffset;

    if(hasIPOptions_80211(pPacket)) {
        nOffset = getIP_HeaderSize_80211(pPacket);
    } else {
        nOffset = L3_IPV4_OFFSET;
    }

    lSeqNum = ntohl(*((unsigned int *)(pPacket + LENGTH_80211 + LENGTH_LLC + nOffset + OFFSET_TCP_SEQNUM)));

    return lSeqNum;
}

unsigned int getTCP_AckNumber_80211 (char * pPacket, int nLength) {
    unsigned int lAckNum;

    int nOffset;

    if(hasIPOptions_80211(pPacket)) {
        nOffset = getIP_HeaderSize_80211(pPacket);
    } else {
		nOffset = L3_IPV4_OFFSET;
    }

    lAckNum = ntohl(*((unsigned int *)(pPacket + LENGTH_80211 + LENGTH_LLC + nOffset + OFFSET_TCP_ACKNUM)));

    return lAckNum;
}
