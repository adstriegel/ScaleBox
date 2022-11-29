#ifndef OUTPUTMODULE_H_
#define OUTPUTMODULE_H_

/** Map a specified name, typically via XML/DOM to an
 * actual class name. In short, the function simply takes
 * care of the new operation mapping and the rest is
 * accomplished by the base and extended XML population
 * functions for the base and child classes.
 * @param sName The proper name (case sensitive) of the module to request
 * @returns A valid pointer to an IOModule (or child class) object
 *   if successful, false otherwise 
 */
IOModule 	*		createOutputModuleFromName (string sName);


#endif /*OUTPUTMODULE_H_*/
