/* TestSupport.cc
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
 * $Revision: 1.1 $  $Date: 2008/03/05 22:14:37 $
 **********************************************************
 */
 
#include "../pkt/IOModule.h"
#include "TestTimer.h"
 
#include <iostream>
using namespace std;
 
IOModule * 		mapModuleFromName_Test (string sName) {
	IOModule * pModule;
	
	pModule = NULL;
	
	//cout << "Invoking test mapping" << endl;
	
	if(sName == "TestTimer") {
		pModule = new TestTimerModule();
	}	
		
	return pModule; 		
}

