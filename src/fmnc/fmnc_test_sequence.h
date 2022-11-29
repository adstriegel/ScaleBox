/*
 * fmnc_test_sequence.h
 *
 *  Created on: Jun 4, 2014
 *      Author: striegel
 */

#ifndef FMNC_TEST_SEQUENCE_H_
#define FMNC_TEST_SEQUENCE_H_

#include <vector>
using namespace std;

#include <pthread.h>

#include "../util/ParamDictionary.h"

#include "fmnc_connection_tcp_slice.h"

class FMNC_Connection_TCP_Slice;
class FMNC_Test_Sequence;

/** The FMNC_Transmit_BufferBlock captures the behavior for the data that will likely
 * be sent that constitutes a single block for an overall test sequence.
 */
class FMNC_Transmit_BufferBlock
{
	public:
		FMNC_Transmit_BufferBlock ();

		/** Set the pointer for the current data block.  Note that it is the responsibility
		 * of the caller to have checked and handled any memory allocations if there is
		 * data that is allocated previously in this buffer block
		 * @param pData The new internal buffer
		 */
		void setData (char * pData);

		/** Retrieve a pointer to the data associated with this particular block
		 * @returns Pointer to the core data block (zero indexed)
		 */
		char * getData ();

		/** Set the length associated with this particular data block
		 *
		 */
		void setLength (uint32_t lLength);

		/** Retrieve the length of this data block
		 * @returns Number of bytes in the data block
		 */
		uint32_t getLength ();

		/** Set the status flag with regards to ownership.  If true, the data
		 * block is owned by whomever made this data block.  In other words, the
		 * block and its respective allocations should be freed when one is done
		 * using this block.  If it is false, it means that this is shared data that
		 * should not be freed.
		 * @param bOwnData The new state of ownership for this buffer block
		 */
		void setFlag_Ownership (bool bOwnData);

		/** Get the status flag with regards to ownership.  If true, the data
		 * block is owned by whomever made this data block.  In other words, the
		 * block and its respective allocations should be freed when one is done
		 * using this block.  If it is false, it means that this is shared data that
		 * should not be freed.
		 * @returns The flag for ownership of this block
		 */
		bool getFlag_Ownership ();

		static FMNC_Transmit_BufferBlock createSimple (int nMode);

		/** Allocate a new data block of the requested size
		 * @param lLength The new block size (in bytes)
		 * @returns True if successful, false otherwise
		 */
		bool allocateBlock (uint32_t lLength);

		void setPosition (uint32_t lPosition);
		uint32_t getPosition ();

		void populateSimple (int nMode);

	private:
		// What is the length of data?
		uint32_t	m_lLength;

		// Pointer to the data
		char *		m_pData;

		// Does this buffer own the data and should discard it when it is done?
		bool		m_bOwnData;

		// Overall position (start of block) for the overall buffer
		uint32_t	m_lPosition;
};

/** The FMNC_Transmit_Buffer represents a sequence of buffer blocks that can be
 * accessed for transmission to a particular client.  Various fill modes and garbage
 * modes may be specified without using actual allocated data.
 */
class FMNC_Transmit_Buffer
{
	public:
		FMNC_Transmit_Buffer ();
		~FMNC_Transmit_Buffer ();


		/** Copy data from this sequence of buffer blocks into the destination specified
		 * @param pDestination Where the data goes
		 * @param lStart The starting byte from which to access
		 * @param lLength The number of bytes to copy over
		 * @return The number of bytes copied
		 */
		uint32_t	copyData (char * pDestination, uint32_t lStart, uint32_t lLength);

		bool getFlag_FillGarbage ();
		void setFlag_FillGarbage (bool bFlag);

		int getFillMode ();
		void setFillMode (int nMode);

		/** Add a specific buffer block to the mix by appending it to the
		 * end of the list of buffer blocks that we have
		 */
		bool	addBufferBlock (FMNC_Transmit_BufferBlock * pBlock);

		/** Return the number of bytes in the buffer across all buffer blocks
		 * @returns Retrieve the total length
		 */
		uint32_t	getTotalLength ();

		/** Dump / debug the information contained within this transmit buffer
		 * @param bDumpContent Flag denoting if actual content should be dumped
		 */
		void		dump_TransmitBuffer (bool bDumpContent);


		/** Retrieve the number of buffer block objects present
		 * @returns The count in terms of objects
		 */
		int			getCount ();

		void updatePositions ();

		int	 findBufferWithPosition (uint32_t lPosition);

		bool		swapOrder (int nSwap1, int nSwap2);

		/** Remove / delete a particular block from the buffer
		 * @param nBlock The block to remove (zero-indexed)
		 * @returns True if successful, false otherwise
		 */
		bool		removeBlock (int nBlock);

	private:
		vector<FMNC_Transmit_BufferBlock *> 	m_Data;

		bool	m_bFillGarbage;
		int		m_nFillMode;
};




class FMNC_Transmission_TCP
{
	public:
		FMNC_Transmission_TCP ();

		void setParent (FMNC_Test_Sequence * pParent);
		FMNC_Test_Sequence * getParent ();

		timeval		m_timeOut;

        uint32_t m_spacingUS;
		// Where does this particular transmssion start in terms of the buffer?

		void	setStart (uint32_t lStart);
		uint32_t	getStart ();

		// How much should be sent on this particular transmission in the payload?

		void 	setLength (uint32_t lLength);
		uint32_t	getLength();

		void	setSeqNum (uint32_t lSeqNum);
		uint32_t	getSeqNum();

		// Given a base sequence number (start of the buffer, start=0), recompute the
		// sequence number for this particular transmission as appropriates
		void	adjustSeqNum (uint32_t lBase);

		void	incrementTime   (timeval * pAdjTime);


		void	setOutTime (uint32_t lSec, uint32_t lUS);
		uint32_t getOutTime_us ();

		bool	getFlag_PSH ();
		void	setFlag_PSH (bool bFlagPush);

		/** Retrieve the group associated with this particular TCP slice. Note that the
		 * default value (FMNC_MEASUREMENT_GROUP_NONE) means that no group is specified
		 * @returns The group value
		 */
		uint16_t	getGroup ();

		/** Change the group value for this particular TCP slicing.  See getGroup for
		 * more details with respect to the settings.  A value of FMNC_MEASUREMENT_GROUP_NONE
		 * means that no group is specified.
		 * @param nGroup The new group value
		 */
		void		setGroup (uint16_t nGroup);

		/** Retrieve the expected ACK number for this particular slice
		 * @returns The expected ACK number from the client
		 */
		uint32_t	getExpectedAckNumber ();

		/** Set the expected ACK number for this particular slice
		 * @param nAckNum The expected ACK number
		 */
		void		setExpectedAckNumber (uint32_t nAckNum);

	private:
		FMNC_Test_Sequence * 	m_pParent;

		// What is the start byte of the buffer to send?
		uint32_t	m_lStart;

		// What is the length of this block to send?
		uint32_t	m_lLength;

		// What is the starting sequence number that I am sending for this transmission?
		uint32_t	m_lSeqNum;

		// When should this start on the outbound queue?

		// Should the PSH flag be set?
		bool		m_bSetPush;

		// The group number for this slice (if specified)
		uint16_t	m_nGroup;

		// The expected ack number for this particular packet
		uint32_t	m_lExpectedAck;
};

class FMNC_Test_Sequence
{
	public:

		vector<FMNC_Transmission_TCP *>	m_Transmissions;
		FMNC_Test_Sequence ();
		~FMNC_Test_Sequence ();

		void	addTransmit (FMNC_Transmission_TCP * pTransmit);

		FMNC_Connection_TCP_Slice *	getConnection ();
		void setConnection (FMNC_Connection_TCP_Slice * pConnection);

		void populatePredefined (int nPredef);

		void adjustSeqNumber (uint32_t lSeqNum);

		void adjustDispatchTime (timeval * pAdjTime);

		// Shape the output queue and set the time from right now subject to the initial delay
		//  request
		void shapeOutput (uint32_t lMicroDelay, uint32_t lInitialDelay);

		/** Given a packet, pop the next item from the buffer to populate the data payload of the
		 * packet. Fill in the packet length and sequence number and packet payload.
		 */
		bool populatePacketWithNextSend (Packet * pPacket, uint32_t * pStime);

		/** Retrieve the next item that needs to be sent
		 */
		FMNC_Transmission_TCP * getNextSend ();

		bool	isDone ();

		void setNextToSend (int nNext);
		void incrementNextToSend ();
		int getNextToSend ();

		string	getXML ();

		string 	getTestConfig();
		void	setTestConfig(string sConfig);

		uint32_t	getTestLength ();


		/// Retrieve the slice size to be used for the packets (when applicable). The slice size is the number of bytes that
		/// effectively represents the MSS for the TCP payload
		uint16_t	getSliceSize ();
		void		setSliceSize (uint16_t nSlice);

		void		setSliceSpacing (uint32_t lSpaceSec, uint32_t lSpaceMicroSec);
		uint32_t	getSliceSpacing_Sec ();
		uint32_t	getSliceSpacing_MicroSec ();

		/// The web demo test is the most basic test sequence whereby the FMNC overview / information is embedded as part of
		/// the request.  A d3js script is then used to plot particular results for the results.
		///
		/// @param nFixed     The number of fixed slices
		/// @param nStepSeq   The number of stepped sequences
		/// @param nStepMult  Maximum step multiplier versus the slice size
		/// @param lID		  The ID to be used for the iframe embed
		void	createTest_WebDemo (uint16_t nFixed, uint16_t nStepSeq, uint16_t nStepMult, uint32_t lID);
		void	createTest_TrainSizeWebDemo (uint8_t type,uint16_t size,uint16_t gap, uint16_t length,uint16_t rmin,uint16_t max, uint8_t step,uint32_t lID);
		void	createTest_TrainGapWebDemo (uint8_t type,uint16_t size, uint16_t length,uint16_t rmin,uint16_t max,uint32_t lID);
		void	createTest_TrainFAWebDemo (uint16_t l,uint16_t s, uint16_t rmin,uint16_t rmax,uint16_t gap, uint32_t lID);
		void	createTest_TrainFACellularWebDemo (uint16_t rmin,uint16_t rmax,double alpha, uint32_t lID);

		/// Handle the request for a particular PNG file
		bool 	wrapFile_PNG (uint16_t nFixed, uint16_t nStepSeq, uint16_t nStepMult);


		FMNC_Transmit_Buffer *	getTransmitBuffer ();


		void 	dump_TransmitSequence ();

		/// Given a transmission buffer that is already populated, slice up the content
		/// into slices of size nSlice and if bFlip is specified, flip it to force the
		/// additional data points
		/// @param nSlice The size of each slice (value of 0 means use MSS)
		/// @param bFlip  Determines if we should flip
		void 	sliceFixed (uint16_t nSlice, bool bFlip);


		/// Slice the file up for a response
		/// @param sFile        The name of the file template to use
		/// @param pDictionary  The dictionary information (if given)
		/// @param bSlice       Denotes whether or not to flip the last slice
		void constructFileWithParams (string sFile, ParamDictionary * pDictionary, bool bFlip);


		void setFlag_CreateDictionary (bool bCreateDict);
		bool getFlag_CreateDictionary ();

		ParamDictionary * getDictionary ();
		bool			  allocateDictionary ();
        uint32_t temp_Spacing;

	protected:
		void 	predefined_2 (bool bFlip);

		/// Load the file specified into the buffers used for transmission
		uint32_t	loadFileIntoBuffers (string sFile);


		string	generate_iframe    (uint32_t lID);

	private:
		FMNC_Transmit_Buffer			m_TransmitBuffer;

		// Which connection am I associated with?
		FMNC_Connection_TCP_Slice *		m_pConnection;

		int		m_nNextToSend;

		string		m_sTestConfig;

		// What is the slice size for packet payloads?
		uint16_t		m_nSliceSize;

		// What is the typical spacing between packets?
		uint32_t		m_nSliceSpace_Sec;
		uint32_t		m_nSliceSpace_MicroSec;

		pthread_mutex_t 	m_MutexList;

		// Flag denoting if we should populate the dictionary on the fly
		//   Dictionary population is primarily if we need to live reflect things
		//   back.
		bool				m_bFlag_CreateDictionary;

		// Parameter entries
		ParamDictionary * 	m_pDictionary;
};



#endif /* FMNC_TEST_SEQUENCE_H_ */
