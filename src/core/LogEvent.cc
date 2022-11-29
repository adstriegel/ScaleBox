/* LogEvent.h
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
 */

#include <iostream>
using namespace std;

#include "LogEvent.h"


void		logCriticalEvent 		(string sText) {
	
	
}

void 		logFailureEvent			(string sText) {
	
}

void		logFaultHighEvent  		(string sText) {
	
}

void		logFaultMediumEvent  	(string sText) {
	
	
}


void		logFaultLowEvent  		(string sText) {
	
	
}

void		logCriticalEvent		(string sText, string sClass, string sFunction) {
	cout << "CR: " << sClass << "--" << sFunction << " | " << sText << endl; 
}

void		logFailureEvent			(string sText, string sClass, string sFunction) {
	cout << "FE: " << sClass << "--" << sFunction << " | " << sText << endl; 	
}

void		logFaultHighEvent		(string sText, string sClass, string sFunction) {
	cout << "HF: " << sClass << "--" << sFunction << " | " << sText << endl; 	
}

void		logFaultMediumEvent		(string sText, string sClass, string sFunction) {
	cout << "MF: " << sClass << "--" << sFunction << " | " << sText << endl; 
}

void		logFaultLowEvent		(string sText, string sClass, string sFunction) {
	cout << "LF: " << sClass << "--" << sFunction << " | " << sText << endl; 	
}

void 		logInformEvent 			(string sText, string sClass, string sFunction) {	
	cout << "IE: " << sClass << "--" << sFunction << " | " << sText << endl; 
}


