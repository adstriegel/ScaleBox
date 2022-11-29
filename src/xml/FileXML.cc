
#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include "FileXML.h"
#include "../mem/MemoryPool.h"
#include "NodeDOM.h"
#include "NodeElem.h"

FileXML::FileXML () {
	m_pDOMRoot = NULL;
}

FileXML::~FileXML () {
	
}
			
void FileXML::setFile (string sFile) {
	m_sFile = sFile;	
}

string FileXML::getFile () {
	return m_sFile;	
}

		
void FileXML::setPath (string sPath) {
	m_sPath = sPath;	
}
		
string FileXML::getPath () {
	return m_sPath;	
}
	
string FileXML::getFullLocation () {
	return m_sPath + m_sFile;
}


/** Return a string that denotes that current XML state */
string convertStateToString (parseXMLState theVal) {
	switch(theVal) {	
		case PARSEXML_STATE_BASE:
			return "BaseState";
		
		case PARSEXML_STATE_STARTTAG:
			return "StartTag";
			
		case PARSEXML_STATE_TAGNAME:	
			return "TagName";	
		
		case PARSEXML_STATE_STARTCOMMENT:
			return "StartComment <!";
		
		case PARSEXML_STATE_STARTCOMMENTDASH:
			return "StartCmntDash <!-";
		
		case PARSEXML_STATE_COMMENT:
			return "InComment";	
			
		case PARSEXML_STATE_ENDTAG:
			return "EndTag";

/*
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
};*/
	}
	
	return "";
}
	
	
	
char FileXML::readDOMfromFile () {
	ifstream		theFile;
	
	cout << "Attempting to open file of " << getFullLocation() << endl;
	theFile.open(getFullLocation().c_str());
	
	
	if(!theFile) {
		cerr << "Error: Failed to open XML file for reading as a DOM" << endl;
		return 0;
	}
	
	// Tokenize the file as we were able to open it successfully
	parseXMLState		curState = PARSEXML_STATE_BASE;
	char		readChar;
	string		sTag;
	string		sElem;
	string		sData;
	
	NodeDOM 	*		pNewRoot;
	NodeDOM 	*		pCurrent;
	
	// pNewChild when non-NULL denotes that the current child has been
	//  allocated
	NodeDOM		*		pNewChild;
	NodeElem	*		pElem;
	
	pNewRoot = (NodeDOM *) g_MemPool.getObject(MEMPOOL_OBJ_DOMNODE);
	pCurrent = pNewRoot;
	pNewChild = NULL;
	
	if(pNewRoot == NULL) {
		cerr << "Error: Unable to fetch a NodeDOM object from the memory pool." << endl;
		cerr << "  Aborting XML to DOM transformation of the file " << getFullLocation() << endl;
		return 0;
	}
	
	sTag = "";
	sData = "";

	// Read one character at a time. Since XML reads will typically not be
	// on a critical path, we are not concerned about the I/O cost. This
	// can be optimized if necessary.	
	while(theFile.good()) {
		theFile.get(readChar);
	
//		cout << "  RC = " << readChar << "  State = " << curState;
		if(pCurrent != NULL) {
			//printf("  Cur = 0x%X  Tag: %s   NC=%X  State=%d (%s)\n", pCurrent, pCurrent->getTag().c_str(), pNewChild, curState, convertStateToString(curState).c_str());
			//if(readChar >= 32) {
			//	printf("  rChar: %c  Tag: %s   NC=%X  State=%d (%s)\n", readChar, pCurrent->getTag().c_str(), pNewChild, curState, convertStateToString(curState).c_str());
			//}
			//else {
			//	printf("  rChar: %d  Tag: %s   NC=%X  State=%d (%s)\n", readChar, pCurrent->getTag().c_str(), pNewChild, curState, convertStateToString(curState).c_str());
			//}
		} 
		
		switch(curState) {
			// Base state
			//   We are looking for the < delimiter to denote the beginning of
			//   a tag
			case PARSEXML_STATE_ENDCLOSETAG:
			case PARSEXML_STATE_BASE:	
				if(readChar == '<') {
					curState = PARSEXML_STATE_STARTTAG;
					sTag = "";
				}
				break;
			
			// Entering a comment - have <! so far
			case PARSEXML_STATE_STARTCOMMENT:
				if(readChar == '-') {										
					curState = PARSEXML_STATE_STARTCOMMENTDASH;
					break;
				} else {
					cerr << "Warning: Badly formed comment that contains only <!, missing tailing dashes" << endl;
					curState = PARSEXML_STATE_ENDCLOSETAG;
					break;
				}
				break;
			
			// Entering a comment - have <!- so far
			case PARSEXML_STATE_STARTCOMMENTDASH:
				if(readChar == '-') {										
					curState = PARSEXML_STATE_COMMENT;
					break;
				} else {
					cerr << "Warning: Badly formed comment that contains only <!-, missing tailing dash" << endl;
					curState = PARSEXML_STATE_ENDCLOSETAG;
					break;
				}			
				break;

			// Inside the comment
			case PARSEXML_STATE_COMMENT:
				if(readChar == '-') {
					curState = PARSEXML_STATE_EXITCOMMENTDASH;
					break;
				} else {
					// Do nothing
				}
			
				break;
			
			// Determine if we are leaving the comment, have seen a dash so far
			case	PARSEXML_STATE_EXITCOMMENTDASH:
				if(readChar == '-') {
					curState = PARSEXML_STATE_EXITCOMMENTDASH2;
					break;
				} else {
					curState = PARSEXML_STATE_COMMENT;
				}
			
				break;
			
			// Now we've seen two dashes, the trailing > and we are back to normal non-commented operations
			case PARSEXML_STATE_EXITCOMMENTDASH2:
				if(readChar == '>') {
					curState = PARSEXML_STATE_ENDCLOSETAG;
					break;
				} else {
					curState = PARSEXML_STATE_COMMENT;
				}
			
			
				break;			
				
			case PARSEXML_STATE_STARTTAG:
				// Ignore spaces
				if(readChar == ' ') {
					continue;	
				} else if (readChar == '/') {
					curState = PARSEXML_STATE_CLOSETAG; 
					break;
				} else if (readChar == '!') {
					curState = PARSEXML_STATE_STARTCOMMENT;
					break;
				}
				
				// We'll assume a classical C++ naming convention
				if(readChar < 'A' || readChar > 'z') {
					cerr << "Error: Invalid character denoting the beginning of the tag in" << endl;
					cerr << "   the XML file for DOM transformation (" << getFullLocation() << ")" << endl;
					printf("    Character %c\n", readChar);
					
					// Release the root and its children back
					g_MemPool.releaseObject(pNewRoot);
					pNewRoot = NULL;
					pCurrent = NULL;
					return 0;
				} else {			
					// Any actual characters form the basis of the tag
					sTag = readChar;
					curState = PARSEXML_STATE_TAGNAME;
				}
				break;
			
			// In the tag itself. A space or other non-letter or number denotes
			// the end of the tag
			case PARSEXML_STATE_TAGNAME:
			
				if(readChar == '>') {
					// The end of the tag delimiter is given and there are no elements to
					// the tag itself	
					//
					// This case is unique in that this is the only time we actually create
					//  the node itself. Hence, pNewChild is only marginally useful for this
					//  case.
					curState = PARSEXML_STATE_ENDTAG;
					sData = "";
					
//					cout << "End of tag where tag is " << sTag << endl;
//					printf("   pNewChild %X  pCurrent %X   pNewRoot %X\n", pNewChild, pCurrent, pNewRoot);
					
					if(pNewChild == NULL) {										
						pNewChild = createChild(pCurrent, sTag);
						
						if(pNewChild == NULL) {
							cerr << "Error: Unable to add a child tag of <" << sTag << ">" << endl;
							cerr << "  Aborting in  XML to DOM transformation of the file " << getFullLocation() << endl;
							g_MemPool.releaseObject(pNewRoot);
							return 0;							
						}
						
						// Shift the focus to this for any elements, data, or new children
						pCurrent = pNewChild;
						pNewChild = NULL;	
					}					
				} else if (readChar == ' ' || readChar < '0' || readChar > 'z' ) {
					// Read in a space or other character that is non-numeric which means the tag has now terminated
					//
					// Unlike the > token, we have now terminated a tag name without the >
					// delimiter simply by virtue of the space or non-alphanumeric character
					//
					// We cannot solely use the > delimiter as elements are separated by space and Elem="Value" after the
					//  tag but before the > delimiter
					curState = PARSEXML_STATE_TAGINFO;
					
//					cout << "Delineated bound via non-alpha character" << endl;						
//					cout << "  Tag detected as " << sTag << endl;
					
					pNewChild = createChild(pCurrent, sTag);
						
					if(pNewChild == NULL) {
						cerr << "Error: Unable to add a child tag of <" << sTag << ">" << endl;
						cerr << "  Aborting in  XML to DOM transformation of the file " << getFullLocation() << endl;
						g_MemPool.releaseObject(pNewRoot);
						return 0;							
					}
							
					pCurrent = pNewChild;
				} else {
					// Should be a valid character now
					sTag += readChar;
				}
				
				break;
				
			case PARSEXML_STATE_TAGINFO:
			case PARSEXML_STATE_RIGHTQUOTE:
				if(readChar == '>') {
					// The end of the tag delimiter is given and there are no more elements to
					// the tag itself	
					curState = PARSEXML_STATE_ENDTAG;
					sData = "";
					
					if(pNewChild == NULL) {					
						pNewChild = createChild(pCurrent, sTag);
							
						if(pNewChild == NULL) {
							cerr << "Error: Unable to add a child tag of <" << sTag << ">" << endl;
							cerr << "  Aborting in  XML to DOM transformation of the file " << getFullLocation() << endl;
							g_MemPool.releaseObject(pNewRoot);
							return 0;							
						}
							
						pCurrent = pNewChild;
					}
				} else if (readChar >= 'A' && readChar <= 'z') {
					sElem = readChar;
					curState = PARSEXML_STATE_ELEMNAME;
				} else {
					// Just ignore it
				}
											
				break;
				
			case PARSEXML_STATE_ELEMNAME:
				// Once we start an element name, it must be terminated by = and double quotes			
				if(readChar == '=') {	
					// Finished reading element name and the = sign
					curState = PARSEXML_STATE_LEFTQUOTE;

//					cout << "  Created element of " << sElem << endl;
					
					pElem = createElement(pCurrent, sElem);
					
					if(pElem == NULL) {
						cerr << "Error: Bad element, aborting conversion from XML to DOM" << endl;
						g_MemPool.releaseObject(pNewRoot);
						return 0;
					}
				}
				else if (readChar == ' ' || readChar < '0' || readChar > 'z' ) {									
					curState = PARSEXML_STATE_ELEMEQ;
					
					pElem = createElement(pCurrent, sElem);
					
					if(pElem == NULL) {
						cerr << "Error: Bad element, aborting conversion from XML to DOM" << endl;
						g_MemPool.releaseObject(pNewRoot);
						return 0;
					}	
				} else {
					// Must be a valid character, add it to the element name
					sElem += readChar;	
				}			
				break;
				
			case PARSEXML_STATE_ELEMEQ:
				// The element name has been ended at this point, we must see an = sign to continue
				if(readChar == '=') {
					curState = PARSEXML_STATE_LEFTQUOTE;						
				}
				break;
			
			case PARSEXML_STATE_LEFTQUOTE:
				// The equals sign has been seen, we now need to see a quote (ASCII character 34)			
				if(readChar == 34) {
					curState = PARSEXML_STATE_ELEMVALUE;
					sElem = "";	
				}
				break;
				
			case PARSEXML_STATE_ELEMVALUE:
				// The value is fairly simple, anything in the quotes is game
				if(readChar == 34) {
					curState = PARSEXML_STATE_RIGHTQUOTE;
					// Commit the value
					
//					cout << "  Value is " << sElem << endl;
					
					if(pElem != NULL) {
						pElem->setValue(sElem);
//						cout << "Element contains " << pElem->getName() << "=" << pElem->getValue() << endl;
					}
				} else {
					sElem += readChar;
				}
				break;
			
			case PARSEXML_STATE_ENDTAG:
				// Options:
				//		Any text is data
				//		The end of tag delimeter via /> or </Tag>
				//	Tokens:
				//		<
				//		/
				// 		Anything else
				
				if(readChar == '/') {
					curState = PARSEXML_STATE_TESTSHORTCLOSE;
				}
				else if(readChar == '<') {
					curState = PARSEXML_STATE_CLOSETAGSTART;
					// Commit the data to the node
					pCurrent->setData(sData);
					sData = "";
				}
				else {
					sData += readChar;	
				}
				
				break;			
				
			case PARSEXML_STATE_TESTSHORTCLOSE:
				if(readChar == '>') {
					curState = PARSEXML_STATE_ENDCLOSETAG;
					// Move up one in the hierarchy to our current parent
					//  if applicable
					if(pCurrent != pNewRoot) {
						pCurrent = pCurrent->getParent();
					}					
				} else {
					// Whoops, that really wasn't the end of a tag
					sData += '/';
					sData += readChar;
					curState = PARSEXML_STATE_ENDTAG;
				}
				
				break;
		
			case PARSEXML_STATE_CLOSETAGSTART:
				if(readChar == '/') {	
					curState = PARSEXML_STATE_CLOSETAG; 
				} else if (readChar == '!') {
					curState = PARSEXML_STATE_STARTCOMMENT;
					break;					
				} else if (readChar >= 'A' && readChar <= 'z') {
					// Alpha value, this is a new tag, not a closing tag
					sTag = readChar;
					curState = PARSEXML_STATE_TAGNAME;
					pNewChild = NULL;
//					cout << "Starting new tag" << endl;
				} else {
					// Ignore the rest
				}
				
				break;
				
			case PARSEXML_STATE_CLOSETAG:
				// Going to cheat and expect direct embedding with the proper
				//  close tag (i.e. good hierarchy)
				
				if(readChar == '>') {
					curState = PARSEXML_STATE_ENDCLOSETAG;
					// Move up one in the hierarchy to our current parent
					//  if applicable
					if(pCurrent != pNewRoot) {
						pCurrent = pCurrent->getParent();
					}
				}
				break;
		
			
			default:
			
				break;
		}
	}	
	
	//cout << "  Setting up root" << endl;
	
	setDOMRoot(pNewRoot);
	return 1;	
}

NodeElem *	FileXML::createElement (NodeDOM * pNode, string sElem) {
	NodeElem *	pElem;
	
	pElem = (NodeElem *) g_MemPool.getObject(MEMPOOL_OBJ_DOMELEM);		
	
	if(pElem == NULL) {
		cerr << "Error: Unable to fetch a NodeElem object from the memory pool." << endl;
		cerr << "  Aborting XML to DOM transformation of the file " << getFullLocation() << endl;
		return NULL;							
	}		

	pElem->setName(sElem);
		
	if(!pNode->addElement(pElem)) {
		cerr << "Error: Duplicate or bad element name for tag <" << pNode->getTag() << ">" << endl;
		cerr << "  of " << sElem << endl;
		cerr << "  Ignoring element addition" << endl;
		g_MemPool.releaseObject(pElem);
		return NULL;
	}
	
	return pElem;
}	

NodeDOM * 	FileXML::createChild (NodeDOM * pNode, string sTag) {
	NodeDOM *	pChild;
	
	pChild = (NodeDOM *) g_MemPool.getObject(MEMPOOL_OBJ_DOMNODE);		
	
	if(pChild == NULL) {
		cerr << "Error: Unable to fetch a NodeDOM object from the memory pool." << endl;
		cerr << "  Aborting XML to DOM transformation of the file " << getFullLocation() << endl;
		return NULL;							
	}		
		
	// Set the tag of the child
	if(!pChild->setTag(sTag)) {
		cerr << "Error: Invalid child node tag name for createChild" << endl;
		g_MemPool.releaseObject(pChild);
		return NULL;
	}
	
	// Set the child's parent
	pChild->setParent(pNode);
	
	// Add this child to the parent node as a child
	pNode->addChild(pChild);
	
	return pChild;
}
	
NodeDOM *	FileXML::getDOMRoot () {
	return m_pDOMRoot;
}
	
void FileXML::setDOMRoot (NodeDOM * pRoot) {
	if(m_pDOMRoot != NULL) {
		g_MemPool.releaseObject(m_pDOMRoot);
		m_pDOMRoot = NULL;
	}
	
	m_pDOMRoot = pRoot;		
}

