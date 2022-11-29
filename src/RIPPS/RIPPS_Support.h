/* RIPPS_Support.h
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
 * $Revision: 1.4 $  $Date: 2008/04/08 18:08:28 $
 **********************************************************
 */
 
 
#ifndef RIPPS_SUPPORT_H_
#define RIPPS_SUPPORT_H_

// Uncomment to enable
#define RIPPS_MODULE_ENABLED		1

#include "../pkt/IOModule.h"
#include "../mem/MemPoolObject.h"

/** Attempt to resolve an I/O module name from the specified string. The
 * resulting module will then be returned for future processing. The name
 * field refers to the unique class-level type of name (InputLog, InputPurge),
 * not the actual name that might be used to idenify one particular instance
 * of a given type (IL0, IL1, IL-WANSide).  
 * @param	sName		The name of the module (class-wise name)
 * @returns A pointer to a valid IOModule object if successfully resolved,
 *          NULL otherwise 
 */
IOModule * 		mapModuleFromName_RIPPS (string sName);

/** Attempt to allocate a new memory pool managed object from a specific
 * string name.
 * @param sName The generic name of the memory pool object to allocate
 * @returns A pointer to a valid MemPool object if successfully resolved,
 *    NULL otherwise 
 */ 
MemPoolObject *	mapObjectFromName_RIPPS (string sName);

#endif /*RIPPS_SUPPORT_H_*/
