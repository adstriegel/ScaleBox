/* RIPPS_PktPair.h
 **********************************************************
 * This code is part of the RIPPS (Rogue Identifying
 * Packet Payload Slicer) system developed at the University
 * of Notre Dame. 
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Chad Mano (lead)      Yingxin Jiang
 *           Dave Salyers          Dave Cieslak
 *           Qi Liao               Andrew Blaich   
 * 
 **********************************************************
 * $Revision: 1.4 $  $Date: 2008/04/11 22:00:29 $
 ********************************************************** 
 */

#ifndef __RIPPS_PKTPAIR_H
#define __RIPPS_PKTPAIR_H

#include <vector>
using namespace std;

#include <sys/time.h>

#include "../mem/MemPoolObject.h"
#include "../pkt/Packet.h"
#include "../stat/Stats.h"

enum MonPktState {
	MONPKT_STATE_NONE=0,
	MONPKT_STATE_ARRIVE,
	MONPKT_STATE_DEPART
};

#define	ACK_GAP_LIMIT		1000000

/** A group of packets being monitored (typically data+ack) 
 */
class RIPPS_MonitorPacket : public MemPoolObject {
	public:
		RIPPS_MonitorPacket ();
		~RIPPS_MonitorPacket ();
		
		static RIPPS_MonitorPacket *	createMonPkt ();

		/** Clean the object before giving it back to the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		void clean ();	
		
		/// @brief Fetch the arrival time for the packet at the ingress
		/// @returns A pointer to a valid timeval struct
		struct timeval *	getArrivalTime ();
		
		/// @brief Fetch the departure time for the packet at the egress
		/// @returns A pointer to a valid timeval struct
		struct timeval *	getDepartureTime ();
		
		/// Fetch the current state of the packet (matched or not)
		/// @returns The state of the packet regarding what has been recorded
		int		getState ();
		
		/// Determine if a packet has been matched with an ack yet or not
		/// @returns	1 if matched, 0 if not matched with an ack
		char	isMatched ();
		
		/// @brief Set the arrival time at the RIPPS subsystem
		///	Record the time when the packet came in from the outside
		/// world. The packet state will be set to MONPKT_STATE_ARRIVE.
		/// @param pTime	High resolution timer struct for arrival time
		void		setArrivalTime (struct timeval * pTime);
	
		/// @brief Set the exit time
		/// Record the time when the packet came back from the internal
		/// portion of the monitored network. A departure time recording will trigger
		/// the internal calculation of LRTT and set the state to MONPKT_STATE_DEPART.
		///	@param	pTime  High resoltuion timer struct for exit time
		void		setDepartureTime (struct timeval * pTime); 
	
		/// @brief Calculate the local round trip time (LRTT)
		/// Using the arrival and departure time, calculate the local round
		/// trip time (LRTT) in microseconds
		/// @returns LRTT in microseconds
		long int	getLRTT_us ();
	
		/// @brief Fetch the local round trip time (LRTT) in seconds
		/// @returns Floating point representation in seconds
		float			getLRTT ();
		
		/// @brief Set the expected ack associated with this packet
		/// @param nAck The expected ack (32 bit)
		void			setAck (unsigned int nAck);
		
		/// @brief Retrieve the expected ack associated with this packet
		/// @returns The 32-bit expected TCP ack number
		unsigned int				getAck ();
		
		/// @brief Dump the contents in a one-line format to stdout
		void		dumpBrief ();
		
		void			dumpDetail ();

		void  setPktSize (unsigned int nSize);
		
		/// @brief Display the arrival time to stdout
		void 			dumpAT ();		
		
		/// Place a callback such that just before this packet is written, our arrival time
		/// will be updated to reflect the new output time.  Otherwise, we really can't tell
		/// when the packet truly left versus how we queued it for shaping or otherwise.  
		void			setCallback (Packet * pCallPkt);
		
		void			formatShortDump (char * pBuffer);
	
		/// @brief Set the flag to determine if the packet should be gobbled
		/// If a TCP Sting-like rearrangement is employed, this function allows the
		/// expected ack packet to be gobbled on a FIFO basis until a later ACK
		/// arrives on the internal LAN side.  The default is not to gobble.
		/// @param bFlag 1 if packet should be gobbled, 0 if not
		void			setFlag_Gobble (char bFlag);
	
		/// @brief Return if the packet should be gobbled
		/// @returns 1 if the packet should be gobbled, zero otherwise
		char			getFlag_Gobble ();
		
		/// @brief Set the data source packet number
		/// For packets sliced by RIPPS, this allows the tracing back of a monitor
		/// packet to a source data packet
		void			setDataSrcPkt  (int nPktNum);		
		int				getDataSrcPkt  ();
		
		/// @brief Set the packet number to uniquely identify this monitor packet in a connection
		/// Given that the TCP Sting variant of RIPPS allows multiple monitor packets to share
		/// the same ack, this allows identification of the order to which various monitor
		/// packets were generated. 
		void			setPktNum 	  (int nPktNum);
		int				getPktNum 	  ();
	
		
	private:
		// State of the monitor packet (is it matched, arrived, or no state)
		char				m_nState;
		
		// Ingress arrival time
		//	When did it arrive at the edge of the network?
		struct timeval		m_ArrTime;
		
		// Egress departure time
		//	When did it exit from the edge of the network?
		struct timeval		m_DepTime;
		
		// Expected ack number (32 bit ack number)
		unsigned	int m_lAck;

		// Actual packet size (monitoring only)
		unsigned int   m_lPktSize;
		
		// TCP Sting gobble packet
		//   Should this packet be gobbled on the exit path since it was intentionally re-arranged
		//   to force more acks?
		char			  m_bStingGobble;		
		
		// Data packet source (as seen from WAN-side order)
		int				  m_nDataSrcPkt;
		
		// Packet number as recorded for monitoring
		int				  m_nPktNum;		
		
		// Link to the packet holder if we register for a callback.  If we are about to
		//  be destroyed in the destructor, make sure to tell that packet that it does not have
		//  to call us back any more.  
		Packet 	*	m_pLinkCallback;
};

// Monitor packet stats

class RIPPS_ListMonPktsStats : public Stats {
	public:
		RIPPS_ListMonPktsStats ();
		
		/** Copy constructor for allocating / copying all internal data */
		RIPPS_ListMonPktsStats (RIPPS_ListMonPktsStats * pCopy);
		
};

enum eRIPPSMonPktStats {
	RIPPS_MONPKT_STAT_PKT_BADEXPACKORDER=0,
	RIPPS_MONPKT_STAT_PKT_OO,
	RIPPS_MONPKT_STAT_DUMMY_MATCH,
	RIPPS_MONPKT_STAT_PURGED_ACKS,
	RIPPS_MONPKT_STAT_LAST
};

/// @brief A list of monitored packets
///
/// Packets are kept in a sorted vector with the expectation that packets are maintained
/// in a sorted order based on the expected acknowledgement number.  The sorting is used
/// to allow for faster processing with the reception of an incoming packet to allow for
/// removal of packets already covered in the cumulative acknowledgement functionality of
/// TCP.  
///
/// The list itself actually contains two lists, a list of outstanding (open) ACKs and
/// a list of matched acks.  Statistics are kept regarding packets matched and those that
/// were purged.
///
/// Known Issue: The wrap around of TCP will not function correctly as the function looks
///  for the packet to exceed the last value in the list. 
///
/// Future: Implement a derived class that directly hashes to values rather than placing
///  assumptions on the sorted nature of the list provided that most ACK #'s are matched
///  on the reverse path (for example as with the TCP Sting variant of RIPPS)

class List_RIPPSMonitorPkts {
	public:
		List_RIPPSMonitorPkts ();
		~List_RIPPSMonitorPkts ();
	
		/// @brief Add an open packet (no ack seen yet) to the end of the list
		/// Append a packet with its expected ack to the end of the monitor list with the
		/// expectation that packets should be arriving in order. Note that the addition
		/// of expected acks is subject to two settings in the global monitor object,
		/// namely that allowance of identical expected acks and whether or not packets
		/// can be added in a sorted list.		
		/// @param pPacket The monitor packet to add to the list
		/// @returns True if successful, false if monitor packet was not added
		bool		addOpenPacket (RIPPS_MonitorPacket * pMonPkt);
	
		/// Locate an open packet by virtue of its expected ack number
		/// @returns NULL if search failed, otherwise a valid MonitorPacket object
		RIPPS_MonitorPacket * findPacket_byExpAck (unsigned int lExpAck);
	
		/// @brief Move this packet from the list of open (outstanding) matches to the matched list
		/// Remove this packet from the open ACK list and place it in the matched list.  If applicable,
		/// purge ACKs that are old from the list to minimize search times and the outstanding
		/// ACK list
		char		applyMatchToPacket (RIPPS_MonitorPacket * pPacket);
	
		int			getCountMatches ();
		
		int			getCountOpen		();
	
		int			getCountRetransmissions ();
	
		/// Compute the LRTT in seconds over the last N packets
		/// @param nWindow Window restriction of last N packets, 0 means all packets in match list
		float		computeLRTT			(int nWindow=0);
		
		/// Compute standard deviation across the entire recorded packets
		double		computeStdDev		();
		
		void		dumpBrief				();
		
		void 		dumpDetail		();
	
		/// Purge open packets if they are too old (obvious timeout in terms of LRTT)
		/// @param Distance to which a packet is considered to be old
		void		purgeOld				(float fTime);
	
		void 		purgeOldAcks (unsigned int lAck, unsigned int lNumLimit);
	
			
		RIPPS_MonitorPacket *		getMonPkt (int nPkt);
	
		/// @brief Dump the list of open monitor packets in one-line form
		void 	dumpBriefList_Open ();
		
		/// Note the presence of a retransmission for that specific segment				
		void		noteRetransmission (RIPPS_MonitorPacket * pMonPkt);
	
		char 		matchesExistingRetransmission (RIPPS_MonitorPacket * pPacket);
	
		RIPPS_MonitorPacket * 	getRetransmission (int nIndex);

		int   getCountNoGobble();

		RIPPS_MonitorPacket *  getNoGobble (int nIndex);
	
		/** Retrieve a pointer to the stats object associated with this list
		 */
		RIPPS_ListMonPktsStats	  *  getStatsObject();
		
	private:
		vector<RIPPS_MonitorPacket *>		m_ListOpen;
		vector<RIPPS_MonitorPacket *>		m_ListMatched;
		
		// List of retransmitted packets
		vector<RIPPS_MonitorPacket *>		m_ListRT;

		// List of packets that escape (no gobble)
		vector<RIPPS_MonitorPacket *>         m_ListNoGobble;
		
		char											m_bNewData;
		
		float											m_fLRTT;		
		
		int						m_nCurrentPkt;
				
		RIPPS_ListMonPktsStats		m_Stats;
};

#endif

