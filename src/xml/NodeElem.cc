/* NodeElem.cc
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith
 * 
 **********************************************************
 * $Revision: 1.5 $  $Date: 2008/04/09 15:01:10 $
 **********************************************************
 */

#include <iostream>
using namespace std;

#include "../mem/MemoryPool.h"
#include "NodeElem.h"

NodeElem::NodeElem () : MemPoolObject (MEMPOOL_OBJ_DOMELEM) {
	m_sName = "";
	m_sValue = "";	
}

NodeElem::~NodeElem () {
	
}
			
NodeElem * NodeElem::createNodeElem () {
	NodeElem *	pElem;
	
	pElem = (NodeElem *) g_MemPool.getObject(MEMPOOL_OBJ_DOMELEM);		

	if(pElem == NULL) {
		cerr << "Error: Ran out of memory with node element allocation" << endl;
		return NULL;
	}	

	return pElem;	
}

		
void NodeElem::setName (string sName) {
	m_sName = sName;	
}

string NodeElem::getName () {
	return m_sName;	
}
	
void NodeElem::setValue (string sValue) {
	m_sValue = sValue;	
}

void NodeElem::setValue (int nValue) {
	char	szTemp[17];
	
	sprintf(szTemp, "%d", nValue);	
	
	m_sValue = szTemp;
}


string NodeElem::getValue () {
	return m_sValue;	
}
		
string NodeElem::getXML () {
	// Allow for a 2000 character entry, null char, and the delimiters for XML
	char		szTest[2004];
	string		tempString;
	
	if(m_sName.length() + m_sValue.length() >= 1997) {
		return "";	
	}
	
	// Format the string as --> Name="Value" <-- 
	sprintf(szTest, " %s=%c%s%c", m_sName.c_str(), 34, m_sValue.c_str(), 34); 
	
	tempString = szTest;
	
	return tempString;	
}

void NodeElem::clean () {
	m_sName = "";
}

string	createAttributeString (string sAttrib, string sValue)
{
	string	sXML;

	sXML = sAttrib + "=\"";
	sXML += sValue;
	sXML += "\"";

	return sXML;
}
