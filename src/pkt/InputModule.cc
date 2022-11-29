
#include <iostream>
using namespace std;

#include "IOModule.h"
#include "InputModule.h"
#include "Filter.h"

#include "InputLog.h"
#include "InputPurge.h"
#include "InputSignature.h"
#include "InputPassthrough.h"
#include "../stmc/InputIngressVGDM.h"

#include "MultiIFModule.h"
#include "../pktcache/PktCacheSupport.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"


// This may get moved to a custom file for the purposes of avoiding
//  mods to the core code

IOModule 	*		createInputModuleFromName (string sName) {
	IOModule *		pModule;
	
	pModule = NULL;
	
	if(sName == "InputPurge") {
		pModule = new InputPurge();
	} else if(sName == "InputLog") {
		pModule = new InputLog();
	} else if(sName == "InputIngressVGDM") {
		pModule = new InputIngressVGDM();
	} else if(sName == "InputSignature") {
		pModule = new InputSignature();
	} else if(sName == "InputPassthrough") {
		pModule = new InputPassthrough();
	} else if(sName == "PacketTap") {
		pModule = new PacketTap();
	}
	
	return pModule;
}



