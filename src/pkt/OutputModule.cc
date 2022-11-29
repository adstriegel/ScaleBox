

#include "IOModule.h"
#include "OutputModule.h"
#include "OutputWrite.h"


#include "InputPurge.h"
#include "InputLog.h"

IOModule 	*		createOutputModuleFromName (string sName) {
	IOModule *		pModule;
	
	pModule = NULL;
	
	if(sName == "OutputPurge") {
		// Conceptually, the output purge is indistinguishable from the input case
		pModule = new InputPurge();
	} else if(sName == "OutputLog") {
		// Conceptually, the output log is indistinguishable from the input case		
		pModule = new InputLog();
	} else if(sName == "OutputWrite") {
		pModule = new OutputWrite();
	}
	
	return pModule;	
}
