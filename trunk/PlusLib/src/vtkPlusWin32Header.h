#ifndef __vtkPlusWin32Header_h
#define __vtkPlusWin32Header_h
	#include "PlusConfigure.h"
	#if (defined(_WIN32) || defined(WIN32)) 
		#if defined(PLUS_EXPORTS)
			#  define PLUS_EXPORT __declspec(dllexport)
		# else
			#  define PLUS_EXPORT __declspec(dllimport)
		# endif  /* PLUS_EXPORT */
	#else
		/* unix needs nothing */
		#define PLUS_EXPORT 
	#endif
#endif
