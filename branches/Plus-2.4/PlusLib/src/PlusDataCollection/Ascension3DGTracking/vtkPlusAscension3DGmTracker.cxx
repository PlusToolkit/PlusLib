// medSAFE

#include "PlusConfigure.h"

#include "vtkAscension3DGmTracker.h"

#include "ATC3DGm.h"

#define vtkPlusAscension3DGTrackerBase vtkAscension3DGmTracker
#undef ATC_READ_ALL_SENSOR_AT_ONCE 
#include "vtkPlusAscension3DGTrackerBase.cxx"
#undef vtkPlusAscension3DGTrackerBase
