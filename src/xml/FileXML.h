#ifndef FILEXML_H_
#define FILEXML_H_

#include <string>
using namespace std;

class NodeDOM;
class NodeElem;

enum parseXMLState {
	PARSEXML_STATE_BASE=0,		// Start state
	PARSEXML_STATE_STARTTAG,	// Read start tag delimiter (<)
	PARSEXML_STATE_TAGNAME,		// Currently reading the tag name
	PARSEXML_STATE_TAGINFO,		// Finished the tag name, looking for elements or
								// the end of the tag
	PARSEXML_STATE_ELEMNAME,	// Reading in an element name
	PARSEXML_STATE_ELEMEQ,		// Finished element name, looking for =
	PARSEXML_STATE_LEFTQUOTE,	// Read the = sign, looking for "
	PARSEXML_STATE_ELEMVALUE,	// Read the left quote, reading the value
	PARSEXML_STATE_RIGHTQUOTE,	// Read in the right quote, done with the element
								//  Same as PARSEXML_STATE_TAGINFO
	PARSEXML_STATE_ENDTAG,		// Just ended the tag itself via the end of tag delimiter (>)
								// Anything before the closing delimiter is data
	PARSEXML_STATE_CLOSETAGSTART,	// Closing tag via <, example is </Test>
									//  We saw the < and are looking for /
	PARSEXML_STATE_CLOSETAG,	// Saw the /, looking for the tag name
	PARSEXML_STATE_ENDCLOSETAG,	// Just finished closing the tailing tag
	PARSEXML_STATE_TESTSHORTCLOSE,		// Potential short close via />
	PARSEXML_STATE_STARTCOMMENT,			// ! following a the <
	PARSEXML_STATE_STARTCOMMENTDASH, 	// First dash after the !
	PARSEXML_STATE_COMMENT,					// Seen the second dash, inside the comment
	PARSEXML_STATE_EXITCOMMENTDASH,		// Saw one dash in the comment
	PARSEXML_STATE_EXITCOMMENTDASH2		// Saw two dashes in a row, waiting for the >
};

/** Return a string that denotes that current XML state */
string convertStateToString (parseXMLState theVal);

/** A single XML file that has the ability to extract output
 * as a DOM or via other methods
 */
class FileXML {
	public:
		FileXML ();
		~FileXML ();
			
		/** Set the filename to be opened or written to for
		 * this XML file
		 * @param The new filename that does not include the path
		 */
		void		setFile (string sFile);
		
		/** Retrieve the filename to be opened or written to
		 * @returns A valid string object containing the file name
		 */
		string		getFile ();
		
		/** Set the path where the file is or will be located
		 * @param sPath The new path that does not include the filename
		 */
		void		setPath (string sPath);
		
		/** Retrieve the path where the filename will reside
		 * @returns A valid string object containing the path
		 */
		string		getPath ();
		
		/** Retrieve the full location of the file
		 * @returns A valid string object containing the path and filename
		 */ 
		string		getFullLocation ();
	
	
		/** Populate the internal DOM structure from the specified
		 * filename within the object. The function will attempt to open
		 * the file (read-only) and populate the DOM structure based on
		 * the data within the file
		 * @returns 1 if the operation was successful, 0 otherwise
		 */ 
		char		readDOMfromFile ();
	
		/** Retrieve the root node of the DOM
		 * @returns The root pointer of the DOM tree
		 */
		NodeDOM *	getDOMRoot ();
	
		/** Change the root element associated with the DOM. If there
		 * is an existing DOM, it will be freed back as appropriate.
		 * @param pRoot The new root for the DOM
		 */
		void 	setDOMRoot (NodeDOM * pRoot);
	
		/** Similar to the createChild function, the createElement function takes
		 * in a given node and creates an element and attempts to add it as an
		 * element underneath the node. Note that unlike children, each element
		 * name must be unique which enforces greater rigor on checking previous
		 * values. If successful, the new NodeElem object is returned.
		 * @param pNode The node under which to add the element (if allowed)
		 * @param sElem The name of the element
		 * @returns A valid pointer to NodeElem object if successful, NULL otherwise
		 */
		NodeElem *	createElement (NodeDOM * pNode, string sElem);
		
		/** Create a child based on the tag information under the current node, 
		 * populate its parent information, and add the child underneath the
		 * parent as an actual child.  Return the newly allocated child
		 * node if successful.
		 * @param pNode The current node that will be the parent
		 * @param sTag The tag of the new child node to create
		 * @returns A valid NodeDOM object if successful, NULL if unsuccessful
		 */
		NodeDOM * 	createChild (NodeDOM * pNode, string sTag);	
	
	private:	
		/** The file name where the XML file resides or will reside
		 */
		string		m_sFile;
		
		/** The path where the XML file resides or will reside
		 */
		string		m_sPath;
		
		/** A pointer to the root of the DOM itself. By default, this 
		 * is NULL until properly set by the readDOMfromFile function
		 */
		NodeDOM *	m_pDOMRoot;
};

#endif /*FILEXML_H_*/
