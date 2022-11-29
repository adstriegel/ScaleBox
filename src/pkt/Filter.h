

#ifndef FILTER_H_
#define FILTER_H_

#include <vector>
using namespace std;

#include "../core/StdHeaders.h"

class FilterRule;
class Packet;

class NodeDOM;

/** A filter for a specific module that encompasses
 * a set of filter rules. 
 */
class Filter {
	public:
		Filter ();
		~Filter ();
		
		/** Population function that fills a Filter object and its
		 * rules given the appropriate node in an XML DOM structure.
		 * @param pNode	The node in the DOM tree to start from
		 * @returns True if successful, false if unsuccessful. A false
		 * return does not make any guarantees about the partial validity
		 * of the filter.
		 */
		bool	populateFromDOM (NodeDOM * pNode);
		
		/** Add a rule to the filter set
		 * @param pRule A pointer to a valid FilterRule object
		 */
		void		addRule (FilterRule * pRule);
		
		/** Compare the packet versus the ruleset to determine if
		 * a packet should be included (1 - true) or excluded 
		 * (0 - false). A packet that passes the filter should be
		 * considered for processing by the appropriate module. 
		 * 
		 * The default behavior with zero rules is to exclude packets
		 * via the filter creating a default ignore type of filter set
		 * similar to a firewall. 
		 * 
		 * Rules are evaluated in a short-circuit manner with order
		 * created precedence for matches. 
		 * 
		 * @param pPacket The packet to examine for passing the filter
		 * @returns True if it passes (should be considered), false otherwise 
		 */
		bool		passFilter (Packet * pPacket);
		
	private:
		vector<FilterRule *>		m_Rules;
	
};

#endif /*FILTER_H_*/
