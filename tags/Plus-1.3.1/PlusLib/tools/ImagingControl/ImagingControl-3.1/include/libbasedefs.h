#ifndef LIBBASEDEFS_H_INC
#define LIBBASEDEFS_H_INC

#define _DSHOWLIB_NAMESPACE DShowLib
namespace DShowLib
{
};

namespace _DSHOWLIB_NAMESPACE	// define for certain code completion tools
{
};


#pragma warning( disable : 4786 ) // too long debug info // may be ignored

#define _DLL_EXPORT_WARNING_DISABLE warning ( disable : 4251 ) // ... needs to have dll-interface to be used by clients of class ...
#define _DLL_EXPORT_WARNING_ENABLE  warning ( default : 4251 )

#define UDSHL_NAME_BASE "TIS_UDSHL08"

#endif // LIBBASEDEFS_H_INC