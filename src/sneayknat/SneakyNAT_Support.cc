/* SneakyNAT_Support.cc
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
 * $Revision: 1.2 $  $Date: 2008/02/15 22:27:08 $
 **********************************************************
 */
 

#include "SneakyNAT.h"
#include "../pkt/IOModule.h"
 
#include <iostream>
using namespace std;
 
IOModule * 		mapModuleFromName_SneakyNAT (string sName) {
	IOModule * pModule;
	
	pModule = NULL;
	
	//cout << "Invoking sandbox mapping" << endl;
	
	if(sName == "SneakyNAT") {
		pModule = new SneakyNATModule();
	}	
		
	return pModule; 		
}

