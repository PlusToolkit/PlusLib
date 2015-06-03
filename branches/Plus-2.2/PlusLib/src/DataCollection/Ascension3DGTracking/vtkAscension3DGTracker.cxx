// trakSTAR

#include "vtkAscension3DGTracker.h"

#include "ATC3DG.h"

#define vtkAscension3DGTrackerBase vtkAscension3DGTracker
#define ATC_READ_ALL_SENSOR_AT_ONCE 
#include "vtkAscension3DGTrackerBase.cxx"
#undef vtkAscension3DGTrackerBase
#undef ATC_READ_ALL_SENSOR_AT_ONCE 
