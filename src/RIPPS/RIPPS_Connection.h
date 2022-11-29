/* RIPPS_Connection.h
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
 **********************************************************
 * Now merged with the ScaleBox infrastructure code 
 **********************************************************
 * $Revision: 1.6 $  $Date: 2008/04/11 21:59:58 $
 **********************************************************
 */

#ifndef __RIPPSCONNECTION_H
#define __RIPPSCONNECTION_H

#include <vector>
using namespace std;

#include <fstream>
using namespace std;

#include <sys/time.h>

// Localized include options
#include "../mem/MemoryPool.h"
#include "RIPPS_PktPair.h"

#include "../stat/Stats.h"

class NodeDOM;

// Connection states

#define CONN_STATUS_OPEN		0
#define CONN_STATUS_CLOSED		1

#define TCP_HANDSHAKE_UNK		0
#define	TCP_HANDSHAKE_SYN		1
#define TCP_HANDSHAKE_SYNACK	2
#define TCP_HANDSHAKE_DONE		3
#define TCP_HANDSHAKE_FIN		4

#define MATCH_RESULT_NOCONN		-2
#define	MATCH_RESULT_NOACK		-1
#define MATCH_RESULT_SUCCESS	0

// Assume that a gap of 1 MB is too much 
#define	SEQNUM_WRAP_GAP			1000000

enum {
	CONN_CLOSE_NONE=0,
	CONN_CLOSE_MON_RST,		// Monitor side (internal LAN) TCP reset
	CONN_CLOSE_MON_FIN,		// Monitor side (internal LAN) TCP fin
	CONN_CLOSE_EXT_RST,		// External side (external WAN) TCP reset
	CONN_CLOSE_EXT_FIN,		// External side (external WAN) TCP fin
	CONN_CLOSE_COUNT,		// Sufficient count achieved
	CONN_CLOSE_MAN_CLOSE,	// Manual close via control system
	CONN_CLOSE_UNKNOWN,		// Undefined
	CONN_CLOSE_END	
};

class RIPPS_Connection;

/// @brief A summary of an older connection that is no longer open
/// The ConnSummary object is used to keep information on older connections
/// with the per-packet slicing information already archived away
class RIPPS_ConnSummary {
	public:
		RIPPS_ConnSummary (RIPPS_Connection * pConn);
		~RIPPS_ConnSummary ();
	
		void		dumpBrief ();		
		
		unsigned int		getMonitorSet ();
		unsigned int		getPacketCount ();
		double				getLRTT ();			
		
		/// @brief Set the reason for closing the connection
		/// In the summary, record the outlying reason why the connection was closed.  This
		/// is used to isolate where the majority of reasons for summary are occurring and
		/// to validate that closes are being caught correctly
		/// @param nReason The reason for the close from the list of enums in the Connection header file
		void				setCloseReason (int nReason);
		int					getCloseReason ();		
		
		unsigned short	getRetransmissions ();	

		/** Retrieve a copy of the statistics for this listing of connections
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);

	private:		
		// External IP
		char		m_ExtIP[16];
		int			m_nIPLength;
		
		// Last packet for the connecton (incoming)
		struct timeval		m_lastData;
		
		// Last packet (ACK) for the connection (outgoing)
		struct timeval		m_lastAck;
		
		// First noticed packet for the connection
		struct timeval		m_firstPkt;
	
		// Internal TCP Port 
		unsigned short	m_nIntPort;
		
		// External TCP Port
		unsigned short 	m_nExtPort;
		
		// Appropriate statistics for the connection
		unsigned int		m_nPkts;			// # of packets observed
		double				m_fLRTT;			// LRTT in seconds
		double				m_fStdDev;		// Std. Dev. of LRTT
		
		unsigned int		m_nMonitorSet;	
		
		unsigned int		m_nPurgedAcks;
		
		int					m_nCloseReason;		// Why the connection was closed/summarized?	
		
		unsigned short	m_nRetransmissions;
		
		RIPPS_ListMonPktsStats	*	m_pStats;
};

/// A specific connection monitored by RIPPS

class RIPPS_Connection {
	public:
		RIPPS_Connection ();
		~RIPPS_Connection ();

		void		dumpBrief_MonPkt ();						
				
		/// @brief Create and populate an expected ack for the list
		/// Given that TCP will send an ACK later on (cumulatively ack'ing all seq #'s before),
		/// record an expected ack under the connection list together with an arrival time
		/// and packet size for later reference.  Note that thread-safety is provided at the host
		/// level, not at the Connection object level.
		/// @param pPacket The packet to derive all information from
		/// @returns A non-NULL MonitorPacket object that was created from the packet for further
		///          manipulation if appropriate
		RIPPS_MonitorPacket *	populateExpectedAck (Packet * pPacket);
		
		void				addExpectedAck (RIPPS_MonitorPacket * pMonPkt);
		
		/// @brief Search for an expected ack in the list of expected acks
		/// Attempt to search list of expected acks for the ack corresponding to this
		/// packet.  If found, return it, otherwise NULL
		/// @param pPacket The outbound packet to search the pending ack list for
		/// @returns Non-NULL valid MonitorPacket object if a match is found, otherwise
		///   NULL		
		RIPPS_MonitorPacket *	matchAck	   (Packet * pPacket);
		
		/// Apply the match to the list of expected acks
		/// Given a match identified elsewhere, apply the matching algorithm and its
		/// appropriate purging to the listing
		/// @param The monitor packet to apply as matched
		/// @returns The return state specifies ownership of the passed in MonitorPacket
		///   object whereby a 0 states that the object is the responsibility of the callee
		char	applyMatch   	(RIPPS_MonitorPacket * pPacket);

		char	matchConnection	(Packet * pPacket, char bReverse=0);

		void			dumpBrief ();
		
		void			dumpDetail ();
		
		/** Populate info will populate the connection information and is only
		 * called from the perspective of the WAN such that the source IP / source
		 * port should represent the identity being connected to outside the monitor
		 * area and the destination IP should correspond with the host under which
		 * this connection is registered.  
		 * @param pPacket Packet to populate information from
		 */
		void 			populateInfo (Packet * pPacket);

		struct timeval		getLastData ();
		struct timeval		getFirstData ();

		/** Retrieve the IP external to the network associated with this
		 * connection
		 * @returns A pointer to a valid IP address
		 */
		char *	getIP_External();

		/** Retrieve the IP internal to the network associated with this
		 * connection. Nominally, this should be identical to the host
		 * under which this connection is stored.
		 * @returns A pointer to a valid IP address
		 */
		char *	getIP_Internal();

		/** Retrieve the TCP port associated with the external host on
		 * this connection. The value is returned in host-wise form
		 * and may need to be converted by htons for comparison versus
		 * an actual packet
		 */
		unsigned short		getPort_External ();

		/** Retrieve the TCP port associated with the internal host on
		 * this connection. The value is returned in host-wise form
		 * and may need to be converted by htons for comparison versus
		 * an actual packet
		 */
		unsigned short		getPort_Internal ();

		int		getIPLength();

		
		List_RIPPSMonitorPkts *	getListPkts();

		void					setMonitorSet (unsigned int nSet);
		unsigned int			getMonitorSet ();

		/// @param pFile The name of the file to write to
		void		dumpFile (char * pFile);
		
		void		determineLogName (char * pOut);
		
		void		setSrcIP (char * pAddress);

		void		setHighestSeq (unsigned int lSeq);
		unsigned int getHighestSeq ();
		
		void		setValidHighestSeq (char bFlag);
		char		getValidHighestSeq ();
		
		/// @brief Determine if the connection can be summarized
		/// @param pCurTime A pointer to the current time to avoid multiple system calls
		/// @returns 1 (true) if the connection can be summarized, 0 (otherwise) 
		char				canSummarize   (struct timeval * pCurTime);		

		unsigned short	getRetransmissions ();
		void				incRetransmissions ();
		
		/// Note the fact that a retransmission occurred for possible archiving and debugging
		void 				noteRetransmission (Packet * pPacket);

		/** Retrieve a copy of the statistics for this connection
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);

	private:
		// General status of the connection (open, closed, unknown)
		int		m_nStatus;
		
		// Observed status of the TCP connection
		int		m_nTCPStatus;
		
		// Last packet for the connecton (incoming)
		struct timeval		m_lastData;
		
		// Last packet (ACK) for the connection (outgoing)
		struct timeval		m_lastAck;
		
		// First noticed packet for the connection
		struct timeval		m_firstPkt;
		
		// External IP (external host) 
		char		m_ExtIP[16];
		int		m_nExtLength;
	
		// TCP port on external host
		unsigned short	m_nExtPort;
		unsigned short	m_nNetExtPort;
				
		// Internal IP (for archival purposes only)
		char		m_IntIP[16];
		int		m_nIntLength;
				
		// TCP port on internal host
		unsigned short	m_nIntPort;
		unsigned short	m_nNetIntPort;
		
		unsigned int		m_nMonitorSet;	
		
		// 	The highest ACK number received
		char				m_bValidSeq;
		unsigned	int 	m_lHighestSeq;
		
		// Retransmissions
		unsigned short	m_nRetransmissions;
		
		// Have we seen close notes on either the monitor and/or WAN side yet?
		char				m_bCloseMon;
		char				m_bCloseWAN;		
		
		// The list of monitored packets
		List_RIPPSMonitorPkts	m_listPkts;
};

enum eRIPPS_ListConnStats {
	RIPPS_LISTCONN_STAT_NODATACONN=0,
	RIPPS_LISTCONN_STAT_FINDMISS_EXT,
	RIPPS_LISTCONN_STAT_FINDMISS_INT,
	RIPPS_LISTCONN_STAT_LAST
};

/** Statistics for the RIPPS list of connections object
 */
class RIPPS_ListConnStats : public Stats {
	public:
		RIPPS_ListConnStats ();
		
		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		void		Get_Title	(int nStat, char * szTitle);			
};

/** A list of monitored connections that is broken down into active
 * connections (still looking for ACK matches) and summarized (historical)
 * connections.
 */
class List_RIPPSConnections {
	public:
		List_RIPPSConnections ();
		~List_RIPPSConnections ();
		
		/// @brief Purge the connection list back to an empty state
		/// All connections are deleted from memory and the list is reset to
		/// an empty state
		void			clear ();
		
		void			addConnection (RIPPS_Connection * pConn);
		
		/// @brief Locate the packet via the TCP info
		/// Find the connection amongst the list of connections for this host.  
		/// @param bReverse  Reverse src and destination ports
		RIPPS_Connection *	findConnection (Packet * pPacket, char bReverse=0);
		
		void			dumpBrief ();
		
		void			dumpDetail ();
				
		/** Convert the specified connection into a connection summary and if
		 * noted, avoid summary if there is no relevant information in the
		 * connection itself (no matched ACKs).
		 * @param pConn The connection to summarize
		 * @param bPurgeEmpty Flag denoting if empty (no observed data) connections
		 *          are to be purged rather than added to the list of summaries
		 */		
		bool 			createSummary (RIPPS_Connection * pConn, bool bPurgeEmpty=true); 		
				
		void			dumpBrief_MonPkt ();				
				
		void			summarizeAll  (char * pHostAddress);
		
		unsigned int			getOpenCount ();
		
		/// @brief Retrieve a specific open connection
		RIPPS_Connection * 	getOpenConnection (int nIndex);		
				
		/// Determine how many matched packets are present in the current set
		/// @param nSet Set to get (0 means all)
		unsigned int		getMonitorCount (unsigned int nSet);
				
		/// Determine how many matched packets are present in the current set
		double				getAvgLRTT (unsigned int nSet);		
		
		/// Archive and remove older summaries
		/// @param nSet The set number from which to start purging
		void				archiveOldData (unsigned int nSet);

		/** Retrieve a copy of the statistics for this listing of connections
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);
		
		
		/** Remove a connection from the list
		 * @param pConn The connection to remove from the list
		 */
		bool			removeConnection (RIPPS_Connection * pConn);
				
	private:
		vector<RIPPS_Connection *>	m_listConns;
		
		vector<RIPPS_ConnSummary *>	m_summaries;
		
		RIPPS_ListConnStats		m_Stats;
};

#endif

