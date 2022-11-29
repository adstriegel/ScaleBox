/**********************************************************************************************
 * 802_11.h 
 **********************************************************************************************
 * Utility functions to help process 802.11 packets;
 *
 * PI: Dr. Aaron Striegel    striegel@nd.edu
 * Student: Xueheng Hu       xhu2@nd.edu                                                                         
 **********************************************************************************************
 */
#ifndef IEEE_802_11_H_
#define IEEE_802_11_H_

#define OFFSET_RADIOTAP_LENGTH   2

#define OFFSET_80211_DSTMAC      4
#define OFFSET_80211_SRCMAC      10

#define OFFSET_LLC_TYPE          6

#define LLC_TYPE_IP_B1           (char) 0x08
#define LLC_TYPE_IP_B2           (char) 0x00

#define LENGTH_80211             26       
#define LENGTH_LLC               8


char isPacketIP_80211 (char* pPacket);

char isPacketIPSec_80211 (char* pPacket);

string getDstMac_80211 (char* pPacket);

string getSrcMac_80211 (char* pPacket);

char getIPVersion_80211 (char* pPacket);

unsigned char getIPv4Protocol_80211 (char* pPacket);

string getIPv4_Dst_80211 (char* pPacket);

string getIPv4_Src_80211 (char* pPacket);

unsigned char getIP_HeaderSize_80211 (char* pData);

unsigned char getTCP_HeaderSize_80211 (char* pData);

unsigned short getTCP_SrcPort_80211 (char* pPacket);

unsigned short getTCP_DstPort_80211 (char* pPacket);

unsigned int getTCP_SeqNumber_80211 (char* pPacket);

unsigned int getTCP_AckNumber_80211 (char* pPacket);

#endif /* IEEE_802_11_H_ */
