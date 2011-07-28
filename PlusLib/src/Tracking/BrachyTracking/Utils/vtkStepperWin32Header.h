
// .NAME vtkStepperWin32Header - manage Windows system differences
// .SECTION Description
// The vtkStepperWin32Header captures some system differences between Unix
// and Windows operating systems.

#ifndef __vtkStepperWin32Header_h
#define __vtkStepperWin32Header_h

#if defined(WIN32)
	#include <windows.h>
	#if defined(vtkAMSStepper_EXPORTS)
		#define VTK_AMSSTEPPER_EXPORT __declspec( dllexport )
	#else
		#define VTK_AMSSTEPPER_EXPORT __declspec( dllimport )
	#endif
#else
	#define VTK_AMSSTEPPER_EXPORT
#endif

#endif

