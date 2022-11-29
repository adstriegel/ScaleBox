/*
 * fmnc_measurement_packet.h
 *
 *  Created on: Jun 9, 2014
 *      Author: Aaron
 */

#ifndef FMNC_MEASUREMENT_PACKET_H_
#define FMNC_MEASUREMENT_PACKET_H_

// Measurement packets for FMNC
#include <vector>
using namespace std;

#include <string>
using namespace std;

#include <pthread.h>

#include "../pkt/Packet.h"

#define FMNC_MEASUREMENT_GROUP_NONE		0

/// Base class for one measurement packet
class FMNC_Measurement_Packet
{
	public:
		FMNC_Measurement_Packet();

		void 	setTime (struct timeval * pTime);
		struct timeval * 	getTime ();

		/// Adjust the stored time to be a relative time from an external value
		/// rather than an absolute time value.  The adjustment time value must not
		/// make the resultant time value negative.  If the result will be negative,
		/// the time value in this measurement packet is left as is.
		/// @param pTime The time value to adjust from
		/// @returns True if successful, false if the adjustment failed
		bool 	adjustRelativeTime (struct timeval * pTime);

		virtual bool	instrumentPacket (Packet * pPacket);

		bool instrumentPacket_Base (Packet * pPacket);

		virtual string 	getXML ();

		string getXML_Attributes_Base ();

		string getJSON_Base ();

		/// Retrieve the time value as a string along with the attribute identifier
		string	getAttribute_Time ();
		string  getJSON_Time ();

		string 	getAttribute_Time (timeval * pBaseTime);
		string 	getAttribute_Time (FMNC_Measurement_Packet * pBasePacket);

		string	getAttribute_MetaData ();

		/// Retrieve the time value as a string
		string 	getTime_AsString ();

		void	setMetaData (string sData);
		void	appendMetaData (string sAppend);
		string 	getMetaData ();

		uint16_t	getGroup ();
		void		setGroup (uint16_t nGroup);
		string 		getAttribute_Group();

	private:
		timeval 	m_Time;

		string		m_sMetaData;

		uint16_t	m_nGroup;
};

/// List of measurement packets
class FMNC_List_Measurement_Packets
{
	public:
		FMNC_List_Measurement_Packets ();
		~FMNC_List_Measurement_Packets ();

		string getXML(string sTag);

		void 	addPacket (FMNC_Measurement_Packet * pPacket);

		uint32_t	getCount ();
		string		getCount_AsString();

		/** Retrieve a specific measurement packet at a particular index (zero-indexed)
		 * @param nIndex The index of the packet to retrieve
		 * @returns Valid (non-null) object if a valid index, NULL otherwise
		 */
		FMNC_Measurement_Packet *	getPacket (int nIndex);

	private:
		vector<FMNC_Measurement_Packet *>	m_Packets;

		pthread_mutex_t 				m_MutexList;
};

/// An IPv4 measurement packet with the relevant IPv4 fields recorded
class FMNC_Measurement_Packet_IPv4 : public FMNC_Measurement_Packet
{
	public:
		FMNC_Measurement_Packet_IPv4 ();

		void	setTTL (uint8_t byTTL);
		uint8_t	getTTL ();

		string	getTTL_AsString ();


		void	setTOS (uint8_t byTOS);
		uint8_t	getTOS ();

		string	getTOS_AsString ();

		void	setLength (uint16_t nLength);
		uint16_t	getLength ();

		string	getLength_AsString ();

		void 	setID (uint16_t nID);
		uint16_t	getID ();

		string	getID_AsString ();

		virtual bool instrumentPacket (Packet * pPacket);

		virtual string 	getXML ();

		virtual string getXML_Attributes_IPv4 ();

	protected:
		bool instrumentPacket_IPv4 (Packet * pPacket);

	private:
		// TTL field
		uint8_t		m_byTTL;

		// Type of Service field
		uint8_t		m_byTOS;

		// Length of the packet (as viewed from the IP header)
		uint16_t	m_nLength;

		// The ID of the packet
		uint16_t	m_nID;
};


/// Instrumentation for an inbound SYN packet.  Whereas we do not expect options on
/// subsequent packets, it is worthwhile to note any and all options requested during
/// the initial SYN packet from the client with regards to SACK or other aspects
class FMNC_Measurement_Packet_SYN : public FMNC_Measurement_Packet_IPv4
{
	public:
		FMNC_Measurement_Packet_SYN ();


		virtual bool instrumentPacket (Packet * pPacket);

		void	setOptionLength (uint8_t byLength);
		uint8_t	getOptionLength ();

		string	getOptionLength_AsString ();

		virtual string 	getXML ();

		void	setClientSeqNum (uint32_t lSeqNum);
		uint32_t	getClientSeqNum ();

		string	getClientSeqNum_AsString();

	private:
		// Mirror of the TCP options field (if present)
		uint8_t  	m_byOptions[100];
		uint8_t		m_byOptionLength;

		uint32_t 	m_lClientSeq;

};

/// A simple acknowledgment packet
class FMNC_Measurement_Packet_TCP : public FMNC_Measurement_Packet_IPv4
{
	public:
		FMNC_Measurement_Packet_TCP ();

		virtual string 	getXML ();

		void	setSeqNum (uint32_t lSeqNum);
		uint32_t	getSeqNum ();

		string	getSeqNum_AsString ();

		void	setAckNum (uint32_t lSeqNum);
		uint32_t	getAckNum ();

		string	getAckNum_AsString ();

		virtual bool instrumentPacket (Packet * pPacket);

		void 	addData (string sData);

		void		setFlags (uint8_t byFlags);
		uint8_t		getFlags ();

		string		getFlagsAsString ();


        string getTs_AsString();
        uint32_t getTs();

		bool		isFlagSet_PSH ();
		bool		isFlagSet_ACK ();
		bool		isFlagSet_RST ();
		bool		isFlagSet_FIN ();
		bool		isFlagSet_SYN ();
		bool		isFlagSet_URG ();

	private:
		bool		m_bFlagAck;
		uint32_t 	m_lSeqNum;
		uint32_t	m_lAckNum;
		uint32_t	m_Ts;
		string		m_sData;


		uint8_t		m_byFlags;
};



#endif /* FMNC_MEASUREMENT_PACKET_H_ */
