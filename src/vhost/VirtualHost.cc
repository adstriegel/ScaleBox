

VirtualHost::VirtualHost () {
	
	
}

VirtualHost::~VirtualHost () {
	
}

bool VirtualHost::addAdapter (Adapter * pAdapter) {
	if(pAdapter == NULL) {
		logFaultHighEvent("Warning: Attempting to add a NULL adapter to list of adapters for the virtual host", "VirtualHost", "addAdapter");
		return false;
	}

	m_Adapters.push_back(pAdapter);
	return true;
}
		
int VirtualHost::getCountAdapters () {
	return m_Adapters.size();
}
		
Adapter * VirtualHost::getAdapter (int nAdapter) {
	if(nAdapter < 0 || nAdapter >= m_Adapters.size()) {
		logFaultMediumEvent("Warning: Attempted to access beyond the bounds of the list of adapters for the virtual host", "VirtualHost", "getAdapter");
		return false;
	}
	
	return m_Adapters[nAdapter];
}

