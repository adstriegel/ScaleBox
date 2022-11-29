/**
 * whirlwind_support.cc
 *
 * Created on: Nov 05, 2014
 * Author: Xueheng Hu
 */

// Include Files
#include "whirlwind_support.h"
#include "whirlwind_gateway.h"

IOModule* mapModuleFromName_Whirlwind (string sName) 
{
    IOModule* pModule;
    pModule = NULL;

    // Check allocation
    //cout << "mapModuleFromName_Whirlwind (arg = " << sName << ")" << endl;

    if(sName == "Whirlwind_Gateway") 
    {
        pModule = new Whirlwind_Gateway();
    }

    //printf("    Result was 0x%X\n", pModule);

    return pModule;
}
