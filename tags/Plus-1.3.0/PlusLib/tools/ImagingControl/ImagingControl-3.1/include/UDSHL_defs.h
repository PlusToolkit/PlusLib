#ifndef UDSHL_DEFS_H_INC
#define UDSHL_DEFS_H_INC

#include "libbasedefs.h"

#ifdef UDSHL_GENERATE_DLL_EXPORTS
	#define _UDSHL_EXP_API	__declspec(dllexport)
#else
	#define _UDSHL_EXP_API	__declspec(dllimport)
#endif

#define UDSHL_LIB_VERSION_MAJOR	3
#define UDSHL_LIB_VERSION_MINOR	1


#define _DLL_EXPORT_WARNING_DISABLE warning ( disable : 4251 ) // ... needs to have dll-interface to be used by clients of class ...
#define _DLL_EXPORT_WARNING_ENABLE  warning ( default : 4251 )


#endif // UDSHL_DEFS_H_INC