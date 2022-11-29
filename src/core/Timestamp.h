/* Timestamp.h : Definition code for the Timestamp object 
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
 * Development and equipment support has been provided in
 *  part by the following sponsors:
 * 
 *    National Science Foundation    CNS03-47392
 *    Sun Microsystems Academic Excellence Grant
 *    Intel IXP Development Grant 
 *    University of Notre Dame Faculty Grant
 * 
 **********************************************************
 */


#ifndef __TIMESTAMP_H_
#define __TIMESTAMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/** A timestamp object for the purposes of centralizing time management
 * as appropriate along with comparison / computation. The class is 
 * in essence a wrapper around the standard C high resolution timer
 * struct. Note that this object is not managed by the memory pool as
 * it is likely to exist as part of a memory pool object rather than
 * on its own.  
 */
class Timestamp {
	public:
		Timestamp ();
		~Timestamp ();
	
		/** Set the current time from the system clock for the
		 * high resolution system timer
		 */
		void	setToCurrentTime ();
		

		void  	clearTime ();

		bool	isClear ();


		/** Set the time using an externally provided time
		 * @param theTime The time to set the timestamp to
		 */
		void	setTime (timeval theTime);
		
		/** Compute the difference of this timestamp versus the
		 * other timestamp
		 */
		Timestamp operator + (Timestamp tsComp);

		/** Compute the difference of this timestamp versus the
		 * other timestamp
		 */
		Timestamp operator - (Timestamp tsComp);
		
		/** Compare to see if this timestamp is less than the
		 * other timestamp
		 * @param tsComp The timestamp to compare
		 * @returns True if less than, false otherwise 
		 */
		bool	operator < (Timestamp tsComp);
		
		/** Compare to see if this timestamp is less than or equal to the
		 * other timestamp
		 * @param tsComp The timestamp to compare
		 * @returns True if less than or equal, false otherwise 
		 */
		bool	operator <= (Timestamp tsComp);		

		/** Compare to see if this timestamp is greater than the
		 * other timestamp
		 * @param tsComp The timestamp to compare
		 * @returns True if greater than, false otherwise 
		 */
		bool	operator > (Timestamp tsComp);		

		/** Compare to see if this timestamp is greater than or equalthe
		 * other timestamp
		 * @param tsComp The timestamp to compare
		 * @returns True if greater than or equal, false otherwise 
		 */
		bool	operator >= (Timestamp tsComp);		
		
		
		/** Compare to see if this timestamp is less than the
		 * other timestamp
		 */
		bool	operator == (Timestamp tsComp);
		
		
		/** Get the seconds component of this particular timestamp
		 * @returns Seconds component of the timestamp
		 */
		long int	getSeconds ();
		
		/** Get the microseconds component of this particular timestamp.
		 * Note that this is solely the microseconds portion.
		 * @returns The microseconds portion of the timestamp
		 */
		long int	getMicroseconds ();
		
		/** Set the newly minted Timestamp to contain the same internal
		 * timestamp as listed in the operand
		 * @param op2  The new values to use
		 */ 
		Timestamp operator = (Timestamp op2);
		

		struct timeval * 	getCoreTime ();

	private:
		/** The time associated with this particular timestamp */	
		struct timeval		m_Time;		
};




#endif


