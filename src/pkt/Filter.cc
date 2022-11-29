
#include <iostream>
using namespace std;

#include "Packet.h"
#include "Filter.h"
#include "FilterRule.h"
#include "../xml/NodeDOM.h"
#include "../util/ip-utils.h"

Filter::Filter () {
	
}

Filter::~Filter () {
	
}

void Filter::addRule (FilterRule * pRule) {
	if(pRule != NULL) {
		m_Rules.push_back(pRule);	
	}
}
	
bool Filter::passFilter (Packet * pPacket) {
	int			j;
	
//	cout << " -> passFilter (Filter)" << endl;
//	printf("      0x%X on packet of 0x%X\n", this, pPacket);
	
	// Iterate through the rules and compare	
	for(j=0; j<m_Rules.size(); j++) {
		if(m_Rules[j]->checkFilter(pPacket)) {
//			cout << "      Passed filter!" << endl;
			return true;	
		}
	}
		
	return false;
}

bool	Filter::populateFromDOM (NodeDOM * pNode) {
	NodeDOM *	pRuleNode;
	FilterRule *	pRule;
	int			j;
	
	// Check that the input DOM node is valid
	if(pNode == NULL) {
		return false;
	}
		
	// See how many children there are
	for(j=0; j<pNode->getNumChildren(); j++) {
		// Find all children whose tag is Rule
		pRuleNode = pNode->getChild(j);
		
		if(pRuleNode == NULL) {
			continue;
		}
		
		if(pRuleNode->getTag() == "Rule") {
			pRule = new FilterRule();
			
			if(pRule->populateFromDOM(pRuleNode)) {
				addRule(pRule);
			}
			else
			{
				delete pRule;
				pRule = NULL;
			}
		}
	}
	
	return true;	
}


