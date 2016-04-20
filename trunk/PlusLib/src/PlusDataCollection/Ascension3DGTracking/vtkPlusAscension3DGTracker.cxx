// trakSTAR

#include "PlusConfigure.h"

#include "vtkPlusAscension3DGTracker.h"

#include "ATC3DG.h"

#define vtkPlusAscension3DGTrackerBase vtkAscension3DGTracker
#define ATC_READ_ALL_SENSOR_AT_ONCE 
#include "vtkPlusAscension3DGTrackerBase.cxx"
#undef vtkPlusAscension3DGTrackerBase
#undef ATC_READ_ALL_SENSOR_AT_ONCE 
