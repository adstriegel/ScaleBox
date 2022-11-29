#ifndef NODEELEM_H_
#define NODEELEM_H_

#include <string>
using namespace std;

#include "../mem/MemPoolObject.h"

/** An attribute in an XML document inside of a DOM is in essence
 * simply a name/value pair.   
 */
// TODO: Fix this to a proper naming (attribute)
class NodeElem : public MemPoolObject {
	public:
		NodeElem ();
		~NodeElem ();
		
		/** Attempt to retrieve a NodeElem object from the global management pool
		 * and if necessary, allocate a new object.
		 * @returns A pointer to a valid NodeElem object
		 */		
		static NodeElem *	createNodeElem ();			
		
		/** Set the name (identifier) associated with the 
		 * element.  Within the context of a node, each element
		 * must have a unique name.
		 * @param sName The new name for the element
		 */ 
		void		setName (string sName);
	
		/** Get the name associated with the element
		 * @returns A valid string object containing the name of the
		 * element
		 */
		string		getName ();
	
		/** Set the value associated with the element. The value
		 * may be a blank string as appropriate for the element
		 * itself.
		 * @param sValue The new value for the element
		 */
		void		setValue (string sValue);
		
		/** Place a integer value to associate with the element
		 * @param nValue The new value to be placed in string
		 *   representation for the element
		 */
		void		setValue (int nValue);
		
		/** Retrieve the value associated with the element
		 * @returns A valid string object containing the value of the
		 * element
		 */
		string		getValue ();
		
		/** Retrieve the name/value pairing as appropriate XML
		 * text for writing or display to the screen
		 * @return Valid XML code containing both the name and value
		 *  pairing appropriately coded in XML
		 */
		string		getXML ();
		
		/** Clean the object before giving it back to the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		void clean ();			
		
	private:	
		/** The name of the element itself. Each element must have a
		 * unique name within the context of an individual node (XML
		 * tag). 
		 */
		string		m_sName;
		
		/** The value of the element. There are no specific restrictions
		 * on the value of the element
		 */
		string		m_sValue;	
};

string	createAttributeString (string sAttrib, string sValue);

#endif /*NODEELEM_H_*/
