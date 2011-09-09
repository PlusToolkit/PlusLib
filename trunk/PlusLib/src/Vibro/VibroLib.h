// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VIBROLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VIBROLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef VIBROLIB_EXPORTS
#define VIBROLIB_API __declspec(dllexport)
#else
#define VIBROLIB_API __declspec(dllimport)
#endif

// This class is exported from the VibroLib.dll
class VIBROLIB_API CVibroLib {
public:
	CVibroLib(void);
	// TODO: add your methods here.
};

extern VIBROLIB_API int nVibroLib;

VIBROLIB_API int fnVibroLib(void);
