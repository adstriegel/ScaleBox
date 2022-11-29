/* Stats.h : Adapted from the GenMCast module for ns-2
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#ifndef STATS_H_
#define STATS_H_

#include <stdint.h>

#include <string>
using namespace std;

#include <iostream>
#include <fstream>
using namespace std;



class NodeDOM;

enum {
	FIELD_TYPE_ADDITIVE = 0,				// Add the field like normal
	FIELD_TYPE_CALC,						// Calculated file -> don't bother copying
	FIELD_TYPE_STATIC,						// Fixed value -> does not change

	// Do not erase this last entry!
	FIELD_TYPE_LAST
};

/** @brief The base class for deriving all stat objects
 * 
 *  The stats object allows all of the various multicast entities to share a common stat
 * basis. As a result, one can simply query each node, get the stats, and add/combine all of
 * them together. 
 */
class Stats {
	public:
		Stats  ();
		~Stats ();

		/** Re-compute the various computation sensitive fields
		 */
		virtual void		Calc_Data ();

		/** Add a value to a specific stat field
		 * @param nStat The index to the stat field
		 * @param nVal  The value to adjust the stat by
		 */
		void				Add_Stat (int nStat, uint64_t nVal);

		/** Retrieve a specific stat field value
		 * @param nStat The index to the stat field
		 */
		uint64_t			Get_Stat (int nStat);
		
		/** Set a stat field to a specific value
		 * @param nStat The index to the stat field
		 * @param nVal  The value to set the stat field to
		 */
		void				Set_Stat (int nStat, uint64_t nVal);
		
		/** Set a stat field to reflect a specific type
		 * @param nStat	The index to the stat field
		 * @param nType	The type of the field
		 */
		void				Set_Stat_Type (int nStat, char byType);

		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle	The char field to populate with the title
		 */
		virtual void		Get_Title (int nStat, char* szTitle);

		/// @brief Get the value for this field as a string
		/// Retrieve the value of this particular field as a string.  This can be particularly
		///  useful for derived fields that are floating point results derived from the
		///  core results
		virtual void		GetValueString (int nStat, char* szValue);

		/** Dump the contents of the stat object to the console
		 */
		virtual void		Dump ();

		/** Reset the stat entity to the initial values
		 */
		virtual void		Reset ();    
    
    	/** Allocate the block of stat values to create an array of nSize size.
    	 * @param nSize The number of stat fields that should be present
    	 */
		void				Allocate (int nSize);

		/** Combine two stat fields together in an additive sense
		 */
		void				operator += (Stats &);

    	/** Set the time associated with this stat object to allow for 
    	 * time-dependent stat sampling
    	 */
	  	void                Set_Time (double fTime);    

		/** Output the contents of the stat object via a specific XML format
		 */
    	void                writeGIPSE_XML (char* pFileName);

		/** Append the statistics for this stat object to the root using the
		 * specified tag as a parent
		 */
		NodeDOM*	        getStats (NodeDOM* pRoot, string sTag);

		/// @brief Sync this stat object with the stat object provided
		/// It is assumed that both stat objects are of the same type
		void		        syncAll (Stats* pOtherStat);

		/// Compute the different of p2 - p1 in this stats object.  It is
		/// assumed that both are of the same type / length
		void		        computeDiff (Stats* p1, Stats* p2);

		/// Take the contents of this stat and record it to a comma-separated value stream
		///   Each value is written, separated by a comma, no carriage return is added
		void 		        logCSV (ofstream& outStream);

		/// Take the contents of this stat and record it to a comma-separated value stream
		///   Each value is written, separated by a comma, no carriage return is added
		void 		        logCSV_TimeAvg (ofstream& outStream, double fTime);

		/// Take the contents of this stat and record it to a comma-separated value stream
		///   Each value is written, separated by a comma, no carriage return is added
		void 		        logCSV_FieldNames (ofstream& outStream);



	protected:
		/** The type for each field used primarily to denote if fields can be
		 * combined in an additive sense or are derived fields
		 */
		char*		        m_pbyType;
		
		/** Each individual stat field that is simply a 64 bit unsigned integer
		 * point field
		 */
		uint64_t*	        m_pnStats;
		
		/** The size of the stat array 
		 */
		int					m_nSize;
    
    	/** Time value - to be mapped to UNIX time */
    	double              m_fTime;   // Used for time-wise calculations
};

#endif /*STATS_H_*/
