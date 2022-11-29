#ifndef PKTSUPPORT_H_
#define PKTSUPPORT_H_

/** Attempt to resolve an I/O module name from the specified string. The
 * resulting module will then be returned for future processing. The name
 * field refers to the unique class-level type of name (InputLog, InputPurge),
 * not the actual name that might be used to idenify one particular instance
 * of a given type (IL0, IL1, IL-WANSide).  
 * @param	sName		The name of the module (class-wise name)
 * @returns A pointer to a valid IOModule object if successfully resolved,
 *          NULL otherwise 
 */
IOModule * 		mapModuleFromName_Pkt (string sName);

#endif /*PKTSUPPORT_H_*/
