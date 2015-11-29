// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the M6502EMULATORDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// M6502EMULATORDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef M6502EMULATORDLL_EXPORTS
#define M6502EMULATORDLL_API __declspec(dllexport)
#else
#define M6502EMULATORDLL_API __declspec(dllimport)
#endif

// This class is exported from the M6502EmulatorDll.dll
class M6502EMULATORDLL_API CM6502EmulatorDll {
public:
	CM6502EmulatorDll(void);
	// TODO: add your methods here.
};

extern M6502EMULATORDLL_API int nM6502EmulatorDll;
extern M6502EMULATORDLL_API int fnM6502EmulatorDll(void);