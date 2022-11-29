

#ifndef __DATABLOCK_H_
#define __DATABLOCK_H_

#include <vector>
using namespace std;


// Different sizes for data blocks with each having their own slot in the memory
//  pool allocation to simplify block allocation

//  Tiny		ACK, ARP, simple packets
//	Small		Real-time / VoIP packets / Multimedia
// 	Medium		Normal packets
//  Large		Jumbo frame friendly allocation
//	Huge		Buffered writes from the virtual application

#define DATABLOCK_SIZE_TINY		128
#define DATABLOCK_SIZE_SMALL	512
#define DATABLOCK_SIZE_MEDIUM	2500
#define DATABLOCK_SIZE_LARGE	10000
#define DATABLOCK_SIZE_HUGE		64000

enum eDataBlock {
	DATABLOCK_TYPE_NONE=0,
	DATABLOCK_TYPE_TINY,
	DATABLOCK_TYPE_SMALL,
	DATABLOCK_TYPE_MEDIUM,
	DATABLOCK_TYPE_LARGE,
	DATABLOCK_TYPE_HUGE,
	DATABLOCK_TYPE_CUSTOM
};

/** A data block is a block of data written down by a virtual-based application
 * to the underlying virtual host stack for sending out either to the virtual
 * network or onwards to the actual Internet. The rationale for having the data
 * block is to avoid per-application additions to the memory pool where possible
 * and to hide the underlying nature of reads / writes to the virtual network
 * stack.  
 */
class DataBlock {
	public:
		DataBlock (int nType);
		
		~DataBlock ();
	
		/** Create a new DataBlock by either pulling
		 * an address from the global memory pool or by allocating a
		 * new object. 
		 * @returns A pointer to a valid DataBlock object if successful
		 */
		static  DataBlock *	create (int nType);		
					
		/** Retrieve the size of the allocated data block in bytes
		 * @returns The size of the allocated block (in bytes)
		 */
		unsigned int		getAllocationSize ();
		
		/** Return how many bytes are still free
		 * @returns The number of free bytes
		 */
		unsigned int		getFreeBytes ();
		
		/** Return how many bytes are used up in the block by valid data
		 * @returns The number of used bytes
		 */
		unsigned int		getUsedBytes ();
		
		/** Append data to the end of the used block 
		 * @param pData The data to append
		 * @param nSize The size of the data to append
		 * @returns True if successful, false otherwise
		 */
		bool	appendData (char * pData, unsigned int nSize);
		
		/** Reset the append pointer to start data addition back at the
		 * beginning of the block.  Note that this does not clear the
		 * block itself
		 * @returns True if successful, false otherwise
		 */
		bool	clearBlock ();
		
		/** Perform the clearBlock operation and scrub the data back to
		 * all zeros
		 * @returns True if successful, false otherwise
		 */
		bool	scrubBlock ();
		
	protected:

		/** Set the allocation block to a new size, invalidating and
		 * freeing any previously allocated memory
		 * @param nSize The new size of the block (in bytes)
		 * @returns True if successful, false otherwise
		 */
		bool		setAllocationSize (unsigned int nSize);
		
	private:
		
		/** The allocated block of memory for this data block 
		 */
		unsigned int			m_nAllocSize;
		
		/** The size of the data block being used (in bytes)
		 */
		unsigned int			m_nUsedSize;
};

/** The DataBlockGroup object captures multiple DataBlock objects as part of
 * a contiguous super block of data. 
 */
class DataBlockGroup {
	public:
		DataBlockGroup ();
		
		/** Retrieve the number of data blocks in this particular group
		 * @returns The number of data block objects in the group
		 */
		unsigned int	getCountDataBlocks ();
		
		/** Get the consumed size of the data within the data blocks
		 * @returns The number of consumed data bytes in this group
		 */
		unsigned int	getConsumedSize ();
		
		
		
		DataBlock *		getDataBlock (int nBlock);
		
	private:
		vector<DataBlock *>		m_DataBlocks;
};

#endif
