#ifndef PKTCACHESUPPORT_H_
#define PKTCACHESUPPORT_H_

// Disable for now
//#define PKTCACHE_MODULE_ENABLED		1

#include <string>
using namespace std;

class IOModule;

// The support header file and source are function linkages available that
// allow for minimal linkages from the core of the ScaleBox framework
// (i.e. include one file and call specific functions)

/** Attempt to resolve an I/O module name from the specified string. The
 * resulting module will then be returned for future processing. The name
 * field refers to the unique class-level type of name (InputLog, InputPurge),
 * not the actual name that might be used to idenify one particular instance
 * of a given type (IL0, IL1, IL-WANSide).  
 * @param	sName		The name of the module (class-wise name)
 * @returns A pointer to a valid IOModule object if successfully resolved,
 *          NULL otherwise 
 */
IOModule * 		mapModuleFromName_PktCache (string sName);

#endif /*PKTCACHESUPPORT_H_*/
