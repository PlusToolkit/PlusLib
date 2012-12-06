// medSAFE

#include "vtkAscension3DGmTracker.h"

#include "ATC3DGm.h"

#define vtkAscension3DGTrackerBase vtkAscension3DGmTracker
#undef ATC_READ_ALL_SENSOR_AT_ONCE 
#include "vtkAscension3DGTrackerBase.cxx"
#undef vtkAscension3DGTrackerBase
