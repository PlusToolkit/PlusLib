// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VIBROLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VIBROLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

// This class is exported from the VibroLib.dll
class VTK_EXPORT CVibroLib {
public:
	CVibroLib(void);
	// TODO: add your methods here.
};

extern VTK_EXPORT int nVibroLib;

VTK_EXPORT int fnVibroLib(void);
