
#include "PktCacheSupport.h"

#include "SourceCacheModule.h"
#include "ChildCacheModule.h"

IOModule * 		mapModuleFromName_PktCache (string sName) {
	IOModule * pModule;
	
	pModule = NULL;

#ifdef PKTCACHE_MODULE_ENABLED
	if(sName == "ChildCacheModule") {
		pModule = new ChildPktCache();
	} else if (sName == "SourceCacheModule") {
		pModule = new SourceCacheModule();
	}
#endif	
	
	return pModule; 	
}

