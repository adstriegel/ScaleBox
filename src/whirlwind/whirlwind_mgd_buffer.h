/*
 * whirlwind_mgd_buffer.h
 *
 *  Created on: Nov 27, 2013
 *      Author: striegel
 */

#include <vector>


#ifndef WHIRLWIND_MGD_BUFFER_H_
#define WHIRLWIND_MGD_BUFFER_H_

#define WW_DEFAULT_BUFFER_ALLOC		1500
#define WW_INTEGRITY_BUFFER_DETAIL	0

/** An individual allocation that will be managed by the overarching managed buffer
 */
class Whirlwind_Buffer_Allocation
{
	public:
		Whirlwind_Buffer_Allocation ();
		~Whirlwind_Buffer_Allocation ();

		/** Allocate memory for the purpose of the buffer
		 * @param nSize The size (in bytes) to allocate to the buffer. When in doubt, use WW_DEFAULT_BUFFER_ALLOC
		 * @returns True if successful, false otherwise
		 */
		bool				allocateData (int nSize);

		/** Retrieve a pointer to the allocated buffer
		 * @returns Null if the buffer has not been allocated, non-null otherwise
		 */
		unsigned char * 	getData ();

		/** Retrieve the size allocated to the buffer
		 * @returns Size allocated to the buffer
		 */
		unsigned int 		getSize ();

		/** Determines if the allocation is the default size
		 * @returns True if the buffer is the default size, false if it is non-standard
		 */
		bool				isDefaultSize ();

		/** Check the integrity of the object.  The WW_INTEGRITY_BUFFER_DETAIL may be set in the header
		 * file to force a byte by byte validation for access rights / allocation
		 */
		bool				checkIntegrity ();

		/** Retrieve the owner tag for this particular buffer (debugging only)
		 */
		unsigned int		getOwnerTag ();

		/** Set the owner tag for this particular buffer (debugging only)
		 * @param nTag The new tag for this buffer
		 */
		void				setOwnerTag (unsigned int nTag);

	private:
		unsigned char *		m_pData;
		unsigned int		m_nSize;
		unsigned int 		m_nOwnerTag;
};


/** A managed buffer that represents memory in the overarching ScaleBox pool. The managed
 * buffer is unique from the Packet construct in that it can hold multiple sets of data
 * buffers representing packet flows tied together to appear to be contiguous.
 */
class Whirlwind_Managed_Buffer
{
	public:
		/** Check the integrity of this managed buffer.  Odds are pretty good that this is sort of
		 * a binary thing (seg fault or OK).
		 * @returns True if we think it is OK, false otherwise
		 */
		bool	checkIntegrity ();

		/** A simple dump to the console tabbed in as requested where the content is
		 * summarized in a single line
		 * @param nTabs The number of tabs for the output to be biased in
		 */
		void	dumpConsole_Brief (int nTabs);

		/** A detailed ump to the console tabbed in as requested
		 * @param nTabs The number of tabs for the output to be biased in
		 */
		void	dumpConsole (int nTabs);

		/** Retrieve the number of separate buffers managed by this particular collection
		 * @returns Number of buffers (zero or greater)
		 */
		int 	getNumBuffers ();

		/** Retrieve the total size contained within this particular managed buffer
		 * @returns Total bytes managed by this particular buffer manager
		 */
		unsigned int	getTotalSize ();

		/** Access the nIndex element in the contiguous buffer.  Don't fall off the edge though as
		 * that would be bad.
		 */
		unsigned char operator [] (int nIndex);

	private:
		// Listing of all of the vectors that we control
		vector<Whirlwind_Buffer_Allocation *> 	m_Buffers;
};


#endif /* WHIRLWIND_MGD_BUFFER_H_ */
