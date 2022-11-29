#ifndef STATITEM_H_
#define STATITEM_H_

#include <string>
using namespace std;

/** The StatItem base class that encapsulates a stat object  
 */
class StatItem {
	public:
		StatItem ();
		
		/** Set the name associated with this individual stat item
		 * @param sName The new name for the statistic
		 */
		void		setName (string sName);
		
		/** Retrieve the name associated with this individual stat item
		 * @returns A valid string object containing the name of the state
		 */
		string	getName ();
		
		
	private:
		string		m_sName;
};

#endif /*STATITEM_H_*/
