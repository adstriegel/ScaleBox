/* NodeDOM.cc
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
 * $Revision: 1.6 $  $Date: 2008/04/09 03:21:43 $
 **********************************************************
 */
 

#include <iostream>
using namespace std;

#include "NodeDOM.h"
#include "NodeElem.h"
#include "../mem/MemoryPool.h"

bool translateStringToBoolean (string sString) {
	if(sString == "True") {
		return true;
	} else if (sString == "Yes") {
		return true;
	} else if (sString == "1") {
		return true;
	} else {
		return false;
	}	
}

int  translateStringToInt (string sString) {
	return atoi(sString.c_str());	
}


NodeDOM::NodeDOM () : MemPoolObject (MEMPOOL_OBJ_DOMNODE) {
	m_pParent = NULL;
	m_sTag = "";
	m_sData = "";	
}

NodeDOM::~NodeDOM () {
	// Clean - get rid of the sub-objects
	clean();
}

NodeDOM * NodeDOM::createNodeDOM () {
	NodeDOM *	pNode;
	
	pNode = (NodeDOM *) g_MemPool.getObject(MEMPOOL_OBJ_DOMNODE);		

	return pNode;
}	

void NodeDOM::release () {
	clean();
	g_MemPool.releaseObject(this);
}
	
void NodeDOM::clean () {	
	NodeDOM	*	pNode;
	NodeElem * 	pElem;
	
	m_pParent = NULL;

	// Pay for cleaning it twice where the second clean call
	//  will not have to do much at all
	//
	// The first cleaning call will actually traverse the depth of
	//  the tree to purge all children. A DOM tree cannot be cyclic
	//  thus we are in essence performing a depth first plumbing of
	//	the tree
	while(m_Children.size() > 0) {
		pNode = m_Children.front();
		pNode->clean();
		
		// Give that specific node back to the memory pool
		g_MemPool.releaseObject(pNode);
		
		m_Children.erase(m_Children.begin());
	}			
	
	pNode = NULL;
	
	while(m_Elements.size() > 0) {
		pElem = m_Elements.front();
		
		// No cleaning here as the elements do not necessarily constitute
		//  part of the tree
		
		g_MemPool.releaseObject(pElem);		
		m_Elements.erase(m_Elements.begin());
	}	
}	
		
NodeDOM * NodeDOM::getParent () {
	return m_pParent;	
}

		
void NodeDOM::setParent (NodeDOM * pParent) {
	m_pParent = pParent;	
}
	
char NodeDOM::setTag (string sTag) {
	if(sTag.length() == 0) {
		return 0;
	}
	
	m_sTag = sTag;	
	return 1;
}
	
string NodeDOM::getTag () {
	return m_sTag;	
}
	
void NodeDOM::setData (string sData) {
	m_sData = sData;
}

void NodeDOM::setData (char * pszData) {
	m_sData = pszData;	
}
		
string NodeDOM::getData () {
	return m_sData;
}
		
int NodeDOM::getNumChildren () {
	return m_Children.size();	
}

NodeDOM * NodeDOM::getChild (int nChild) {
	if(nChild < 0 || nChild >= m_Children.size()) {
		return NULL;
	}
	
	return m_Children[nChild];	
}
	
int NodeDOM::getNumElements () {
	return m_Elements.size();	
}
	
NodeElem * NodeDOM::getElement (int nElem) {
	if(nElem < 0 || nElem >= m_Elements.size()) {
		return NULL;
	}
	
	return m_Elements[nElem];	
}
	
void NodeDOM::addChild (NodeDOM * pChild) {
	if(pChild == NULL) {
		return;
	}
		
	// There is no restriction on uniqueness of children
	m_Children.push_back(pChild);	
}


char NodeDOM::addElement (string sName, char * pszValue) {	
	string		sValue;
	
	sValue = pszValue;
	
	return addElement(sName, sValue);
}

char NodeDOM::addElement (string sName, string sValue) {	
	if(sName == "") {
		cerr << "Error: Cannot add element with identifier of zero length" << endl;
		return 0;
	}
	
	NodeElem * pElem;
	
	pElem = (NodeElem *) g_MemPool.getObject(MEMPOOL_OBJ_DOMELEM);	
		
	if(pElem == NULL) {
		cerr << " Error: Memory allocation failed for creating a new element to add" << endl;
		cerr << "    Element ->  name: " << sName << " value: " << sValue << endl;
		return 0;
	}
	
	if(!addElement(pElem)) {
		g_MemPool.releaseObject(pElem);
	}		
}
		
char NodeDOM::addElement (NodeElem * pElem) {
	// Validate that the element is a valid element (pointer-wise)
	if(pElem == NULL) {
		return 0;
	}
	
	// Each element must be unique, search the existing list of
	// elements to verify uniqueness
	if(findElement(pElem->getName()) != NULL) {
		cerr << "Error: Cannot add element to the list as the identifier is not unique" << endl;
		cerr << "   Name: " << pElem->getName() << "  Value: " << pElem->getValue() << endl;
		return 0;
	}
	
	m_Elements.push_back(pElem);
	return 1;	
}

NodeElem * NodeDOM::findElement (string sName) {
	int		j;
	
	for(j=0; j<m_Elements.size(); j++) {
		if(m_Elements[j]->getName() == sName) {
			return m_Elements[j];
		}	
	}	
	
	return NULL;
}

vector<NodeDOM *> NodeDOM::findChild (string sSearchTag) {
	int		j;
	vector<NodeDOM *> 	listMatches;
	
	for(j=0; j<m_Children.size(); j++) {
		if(m_Children[j]->getTag() == sSearchTag) {
			listMatches.push_back(m_Children[j]);
		}	
	}	
	
	return listMatches;	
}
	
NodeDOM * NodeDOM::findFirstChild (string sSearchTag) {
	int		j;
	
	for(j=0; j<m_Children.size(); j++) {
		if(m_Children[j]->getTag() == sSearchTag) {
			return m_Children[j];
		}	
	}	
	
	return NULL;	
}

void NodeDOM::displayConsole () {
	cout << "ScaleBox Monitor Statistics" << endl;
	
	this->displayConsole_Recurse(0);
}

void NodeDOM::displayConsole_Recurse (int nLevel) {
	int		j;
	
	for(j=0; j<nLevel; j++) {
		cout << "  ";
	}
		
	cout << getTag();
	
	if (getNumElements() > 0) {
		cout << " [";
	}
	
	for(j=0; j<getNumElements(); j++) {
		cout << m_Elements[j]->getName() << "=" << m_Elements[j]->getValue();
		if(j+1<getNumElements()) {
			cout << ",";
		}
	}
	
	if (getNumElements() > 0) {
		cout << "] ";
	}
	
	cout << m_sData << endl;
	
	for(j=0; j<getNumChildren(); j++) {
		m_Children[j]->displayConsole_Recurse(nLevel+1);
	}
}

void		NodeDOM::dumpConsole () {
	this->dumpConsole_Recurse(0);	
}


void NodeDOM::dumpConsole_Recurse (int nLevel) {
	int		j;
	
	for(j=0; j<nLevel; j++) {
		cout << "  ";
	}
	
	printf("0x%X -> ", this);
	
	cout << getTag() << "[";
	
	for(j=0; j<getNumElements(); j++) {
		cout << m_Elements[j]->getName() << "=" << m_Elements[j]->getValue();
		if(j+1<getNumElements()) {
			cout << ",";
		}
	}
	
	cout << "]" << m_sData << endl;
	
	for(j=0; j<getNumChildren(); j++) {
		m_Children[j]->dumpConsole_Recurse(nLevel+1);
	}
}

bool NodeDOM::createString (string & sPopulate) {
	int			j;
	char			byQuote;
	
	byQuote = 34;
	
	sPopulate += "<";
	sPopulate += getTag();
	
	for(j=0; j<getNumElements(); j++) {
		sPopulate += m_Elements[j]->getName();
		sPopulate += "=";
		sPopulate += byQuote;
		sPopulate += m_Elements[j]->getValue();
		sPopulate += byQuote;
	}
	
	sPopulate += ">";
	
	/* Recurse through the children */
	for(j=0; j<getNumChildren(); j++) {
		m_Children[j]->createString(sPopulate);		
	}
	
	sPopulate += "</";
	sPopulate += getTag();
	sPopulate += ">";
	
	return true;	
}


	
	