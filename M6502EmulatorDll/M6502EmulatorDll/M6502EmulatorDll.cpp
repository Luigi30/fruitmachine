// M6502EmulatorDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
extern "C" {
#include "M6502EmulatorDll.h"
}
#include "api.h"


// This is an example of an exported variable
M6502EMULATORDLL_API int nM6502EmulatorDll=0;

// This is an example of an exported function.
M6502EMULATORDLL_API int fnM6502EmulatorDll(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see M6502EmulatorDll.h for the class definition
CM6502EmulatorDll::CM6502EmulatorDll()
{
    return;
}
