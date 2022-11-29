
#include <iostream>
using namespace std;

#include "FilterRule.h"
#include "FilterLayer.h"
#include "../xml/NodeDOM.h"
#include "../util/ip-utils.h"


FilterRule::FilterRule () {
	m_bExclusion = false;
	m_nTests = 0;
	m_nHits = 0;
}

FilterRule::~FilterRule () {
	
}
		
void FilterRule::setExclude (bool bExclude) {
	m_bExclusion = bExclude;
}

bool FilterRule::shouldExclude () {
	return m_bExclusion;
}

	
void FilterRule::addFilterLayer (FilterLayer * pLayer) {
	if(pLayer == NULL) {
		cerr << "Requested filter layer to add was null, ignoring" << endl;
		cerr << "  FilterRule::addFilterLayer" << endl;
		return;
	}
	
	m_Layers.push_back(pLayer);
}
	
unsigned int	FilterRule::getTests () {
	return m_nTests;	
}

unsigned int	FilterRule::getHits  () {
	return m_nHits;
}
	
void	FilterRule::resetCounters () {
	m_nTests = 0;
	m_nHits = 0;	
}
	
bool	FilterRule::checkFilter (Packet * pPacket) {
	int			j;
	char			bMatch;

	bMatch = 1;
	
	m_nTests++;
	
	if(m_Layers.size() == 0) {
		return false;
	}
	
	//cout << "    -> checkFilter (FilterRule)" << endl;
	//printf("   %X (Filter)\n", this);
	
	for(j=0; j<m_Layers.size(); j++) {
		//printf("  Layer %d @ %X\n", j, m_Layers[j]);
		
		if(m_Layers[j]->matchFilter(pPacket)) {
			
		} else {
			bMatch = false;
			break;
		}	
	}

	if(bMatch) {
		m_nHits++;
	}

	// Are we looking for inclusion or exclusion?
	if(shouldExclude()) {
		if(bMatch) {
			return false;	
		} else {
			return true;
		}	
	} else {
		if(bMatch) {
			return true;
		} else {
			return false;
		}
	}
}

bool	FilterRule::populateFromDOM (NodeDOM * pNode) {
	if(pNode == NULL) {
		return false;
	}
	
	NodeDOM *	pLayerNode;
	FilterLayer *	pLayer;
	int			j;
	
	// Check that the input DOM node is valid
	if(pNode == NULL) {
		return false;
	}

	// See how many children there are
	for(j=0; j<pNode->getNumChildren(); j++) {
		// Find all children whose tag is Rule
		pLayerNode = pNode->getChild(j);

		if(pLayerNode == NULL) {
			continue;
		}

		pLayer = createFilterLayer_FromDOM(pLayerNode);

		if(pLayer != NULL)
		{
			addFilterLayer(pLayer);
		}
	}
	
	// Did we have any filters?
	if(m_Layers.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

	
