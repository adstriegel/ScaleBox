#ifndef FILTERRULE_H_
#define FILTERRULE_H_

#include <vector>
using namespace std;

class Packet;
class FilterLayer;
class NodeDOM;

/** A specific rule in the set of filters. The FilterRule
 * object is a collection of layer-based filters that
 * denote specific inclusion or exclusion characteristics.
 * Unless otherwise noted, each rule is a collection of
 * AND-wise patterns to validate taken in short-circuit
 * order. 
 */
class FilterRule {
	public:
		FilterRule ();
		~FilterRule ();
				
		/** Check the packet against the filter with a
		 * prescribed result based on the exclusion flag
		 * inside the filter rule. 
		 * @param pPacket A valid pointer to a packet that will
		 * 	be compared against the filter
		 * @returns True if the packet should pass (include) or
		 *          false if the packet should fail (exclude)
		 */ 
		bool		checkFilter (Packet * pPacket);
		
		/** Denote if a rule match recommends exclusion or
		 * inclusion. By default, a rule is marked for inclusion
		 * rather than exclusion
		 * @param bExclude The new exclusion value
		 */
		void		setExclude (bool bExclude);
		
		/** Returns whether or not the rule is targeted towards
		 * inclusion (default) or exclusion
		 * @returns True if a rule match (hit) should denote exclusion,
		 *  false otherwise
		 */
		bool		shouldExclude ();
	
		/** Add a filter that is applied to a specific layer
		 * to the rule. 
		 * 
		 * For instance, a rule encompassing looking for HTTP
		 * might contain a layer 2 (Ethernet) filter for IP,
		 * a layer 3 (IP) filter for TCP, and a layer 4 (TCP)
		 * filter for port 80
		 * @param pLayer A valid pointer to a FilterLayer object
		 */ 
		void		addFilterLayer (FilterLayer * pLayer);
	
		/** Retrieve the number of times this rule has been tested
		 * against. Note that short-circuiting of earlier rules may
		 * prevent later rules from being tested.
		 * @returns The number of times this rule has been tested
		 */
		unsigned int		getTests ();
		
		/** Retrieve the number of times a successful match has been
		 * identified.
		 * @returns The number of matches (hits) for this particular rule
		 */
		unsigned int		getHits  ();
		
		/** Reset the counters for the filter rule
		 */
		void					resetCounters ();
		
		/** Population function that fills a FilterRule object and its
		 * rules given the appropriate node in an XML DOM structure.
		 * @param pNode	The node in the DOM tree to start from
		 * @returns True if successful, false if unsuccessful. A false
		 * return does not make any guarantees about the partial validity
		 * of the filter.
		 */
		bool	populateFromDOM (NodeDOM * pNode);	
		
	protected:
		char		isMatch (Packet * pPacket);
	
	private:	
		/** Does the filter denote exclusion or inclusion?
		 */
		bool		m_bExclusion;
		
		/** The number of tests run against the filter
		 */
		unsigned int		m_nTests;
		
		/** The number of hits on the filter
		 */
		unsigned int		m_nHits;
		
		/** The vector of layer filters to use for testing
		 */
		vector<FilterLayer *>		m_Layers;
};

#endif /*FILTERRULE_H_*/
