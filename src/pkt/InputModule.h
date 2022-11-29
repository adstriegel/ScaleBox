/* InputModule.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith
 * 
 **********************************************************
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#ifndef INPUTMODULE_H_
#define INPUTMODULE_H_

#include<string>
using namespace std;

class IOModule;

/** Map a specified name, typically via XML/DOM to an
 * actual class name. In short, the function simply takes
 * care of the new operation mapping and the rest is
 * accomplished by the base and extended XML population
 * functions for the base and child classes.
 * @param sName The proper name (case sensitive) of the module to request
 * @returns A valid pointer to an IOModule (or child class) object
 *   if successful, false otherwise 
 */
IOModule 	*		createInputModuleFromName (string sName);



#endif /*INPUTMODULE_H_*/
