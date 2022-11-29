/*
 * TWiCE_Support.cc
 *
 *  Created on: Jul 11, 2011
 *      Author: striegel
 */

#include "TWiCE_Support.h"

#include "TWiCE_Gateway.h"

IOModule * 		mapModuleFromName_TWiCE (string sName) {
	IOModule * pModule;

	pModule = NULL;

	if(sName == "TWiCEGateway") {
		pModule = new TWiCE_Gateway();
	}

	return pModule;
}

