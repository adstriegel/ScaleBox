#ifndef NODEDOM_H_
#define NODEDOM_H_

#include <string>
using namespace std;

#include <vector>
using namespace std;

#include "../mem/MemPoolObject.h"
#include "../core/StdHeaders.h"

class		NodeElem;

/** A node in a DOM (Document Object Model) for XML. A DOM
 * is in essence a node in a tree where each node represents
 * a tag and its data in the XML data file.
 * 
 * Note that this instantation is a crude approximation of
 * the Java DOM and does not follow all XML specifications.
 * Eventual plans include porting to support Xerxes or other
 * more robust tools.
 */
class NodeDOM : public MemPoolObject {
	public:
		NodeDOM ();
		~NodeDOM ();
		
		/** Attempt to retrieve a NodeDOM object from the global management pool
		 * and if necessary, allocate a new object.
		 * @returns A pointer to a valid NodeDOM object
		 */		
		static NodeDOM *	createNodeDOM ();		
		
		/** Retrieve the parent associated with this element.
		 * The parent node of the root element is NULL.
		 * @returns Valid NodeDOM object unless this is the
		 * root element
		 */
		NodeDOM *		getParent ();
		
		/** Set the parent node associated with this element.
		 * @param pParent The new parent for this node
		 */
		void				setParent (NodeDOM * pParent);
	
		/** Set the tag associated with this node
		 * @param sTag The new tag for this node
		 * @returns 1 if it is a valid tag, 0 otherwise
		 */
		char				setTag (string sTag);
	
		/** Retrieve the tag associated with this node
		 * @returns A valid string object containing the tag
		 */
		string			getTag ();
	
		/** Set the data information associated with this node
		 * @param sData The new data for this node
		 */
		void				setData (string sData);

		/** Set the data information associated with this node
		 * @param sData The new data for this node
		 */
		void				setData (char * pszData);
		
		/** Retrieve the data associated with this node. A blank
		 * string denotes that the node itself has no data.
		 */
		string			getData ();
		
		/** Retrieve the number of children associated with the node
		 * @returns The number of child nodes under this node
		 */
		int				getNumChildren ();
		
		/** Retrieve a specific child underneath this node
		 * @param nChild The zero-indexed child to retrieve
		 * @returns A valid NodeDOM pointer if the child exists, otherwise NULL
		 */
		NodeDOM *		getChild (int nChild);
	
		/** Retrieve the number of elements associated with this node
		 * @returns The number of elements for this node as an integer
		 */
		int				getNumElements ();
		
		/** Retrieve a specific element of this node
		 * @param nElem The zero-indexed element to retrieve a pointer to
		 * @returns A valid NodeElem pointer if the element exists, otherwise NULL
		 */
		NodeElem *		getElement (int nElem);
	
		/** Add a child node underneath this node. The child itself does not need
		 * to have a unique tag relative to other children.
		 * @param pChild A pointer to a valid child node to add
		 */
		void				addChild (NodeDOM * pChild);
		
		/** Add an element to this node. Elements denote sub-characteristics
		 * of the node itself.  Examples might include font size, font type.
		 * Each element must be unique with respect to the other elements.
		 * @param pElem A pointer to a valid NodeElem object to associate with this
		 *  node
		 * @returns 1 if the operation succeeded, 0 (false) otherwise. The most
		 *  likely cases of failure are elements without a name or duplicate
		 *  element names. 
		 */
		char				addElement (NodeElem * pElem);
	
		/** Add an element to this node. Elements denote sub-characteristics
		 * of the node itself.  Examples might include font size, font type.
		 * Each element must be unique with respect to the other elements.
		 * @param sName  The name of the element identifier as a C++ string
		 * @param sValue The value for the element as a C++ string
		 * @returns 1 if the operation succeeded, 0 (false) otherwise. The most
		 *  likely cases of failure are elements without a name or duplicate
		 *  element names. 
		 */		
		char				addElement (string sName, string sValue);

		/** Add an element to this node. Elements denote sub-characteristics
		 * of the node itself.  Examples might include font size, font type.
		 * Each element must be unique with respect to the other elements.
		 * @param sName  The name of the element identifier as a C++ string
		 * @param sValue The value for the element as a null terminated C-style string
		 * @returns 1 if the operation succeeded, 0 (false) otherwise. The most
		 *  likely cases of failure are elements without a name or duplicate
		 *  element names. 
		 */		
		char				addElement (string sName, char * pszValue);
	
	
		/** Search the list of elements for an exact string match of
		 * the requested element name. The search is case and length
		 * sensitive
		 * @param sName The name to search for in the element list
		 * @returns A valid pointer to a NodeElem object if found, NULL otherwise
		 */
		NodeElem *			findElement (string sName);
	
		/** Search the children of this node to find all children that
		 * have a match to the search tag specified. The search is case
		 * sensitive and length sensitive.
		 * @param sSearchTag The string to search for an exact match
		 * @returns A vector containing NodeDOM objects. A size of 0 on the
		 * vector indicates that no matches were found.
		 */
		vector<NodeDOM *>			findChild (string sSearchTag);
	
		/** Search the children of this node to find the first child that
		 * has a match to the search tag specified. The search is case
		 * sensitive and length sensitive.
		 * @param sSearchTag The string to search for an exact match
		 * @returns A pointer to the first NodeDOM object in the list if found,
		 *  NULL if no matches are found 
		 */
		NodeDOM *			findFirstChild (string sSearchTag);		
	
		/** Clean the object before giving it back to the pool as
		 * appropriate.  Special care should be taken in derived functions
		 * to make this as minimal as possible.
		 */
		void clean ();	
		
		/** Release this object to the memory pool after first cleaning out
		 * (releasing) all of the child information under this node. After
		 * calling this function, the object is no longer under the jurisdiction
		 * of the callee
		 */
		void release ();
	
		/** Dump the content of the XML file to the console for validation
		 */
		void			dumpConsole ();
	
		/** Display the DOM tree in a reasonable to read format to the console
		 */
		void			displayConsole ();		
		
		/** Create a string-based representation of the XML data through
		 * recursion and gradually appending information regarding the
		 * content of the XML object and its children
		 * @param sPopulate The string to populate
		 * @returns True if successful, false otherwise 
		 * */
		bool		createString (string & sPopulate);
	
	protected:
		void			dumpConsole_Recurse (int nLevel);
	
		void			displayConsole_Recurse (int nLevel);		
		
	private:	
		NodeDOM	*		m_pParent;
		
		string			m_sTag;
		string			m_sData;
	
		vector<NodeDOM *>		m_Children;
		vector<NodeElem *>	m_Elements;
};

/** Translate the value in the string from the text description into
 * a nominal boolean value. The function will take various interpretations of
 * a true value in a case sensitive manner ranging in the values of (Yes, True, 1)
 * while any other value will be treated as false.  
 * @param sString The value string to translate to a boolean value
 * @returns A translated value according to the function description
 */
bool translateStringToBoolean (string sString);

/** Translate the value in the string from the text description into
 * a nominal integer value. 
 * @param sString The value string to translate to a nmeric value
 * @returns A translated value according to the function description
 */
int  translateStringToInt (string sString);

#endif /*NODEDOM_H_*/
