/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkAccurateTimer.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtksys/SystemTools.hxx"
#include <sstream>
#include <time.h>

#ifdef _WIN32
#include "WindowsAccurateTimer.h"
WindowsAccurateTimer WindowsAccurateTimer::m_Instance;
#endif

double vtkAccurateTimer::SystemStartTime=0;

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAccurateTimer, "$Revision: 1.0 $");
vtkInstantiatorNewMacro(vtkAccurateTimer);

//----------------------------------------------------------------------------
// The singleton, and the singleton cleanup

vtkAccurateTimer* vtkAccurateTimer::Instance = NULL;
vtkAccurateTimerCleanup vtkAccurateTimer::Cleanup; 

vtkAccurateTimerCleanup::vtkAccurateTimerCleanup(){}

vtkAccurateTimerCleanup::~vtkAccurateTimerCleanup()
{
	// Destroy any remaining output window.
	vtkAccurateTimer::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkAccurateTimer* vtkAccurateTimer::GetInstance()
{
	if (!vtkAccurateTimer::Instance)
	{
		vtkAccurateTimer::Instance = static_cast<vtkAccurateTimer *>(
			vtkObjectFactory::CreateInstance("vtkAccurateTimer"));
		if (!vtkAccurateTimer::Instance)
		{
			vtkAccurateTimer::Instance = new vtkAccurateTimer;
		}
    vtkAccurateTimer::Instance->SystemStartTime = vtkAccurateTimer::Instance->GetInternalSystemTime(); 
	}
	return vtkAccurateTimer::Instance;
}

//----------------------------------------------------------------------------
void vtkAccurateTimer::SetInstance(vtkAccurateTimer* instance)
{
	if (vtkAccurateTimer::Instance == instance)
	{
		return;
	}

	if (vtkAccurateTimer::Instance)
	{
		vtkAccurateTimer::Instance->Delete();
	}

	vtkAccurateTimer::Instance = instance;

	// User will call ->Delete() after setting instance

	if (instance)
	{
		instance->Register(NULL);
	}
}

//----------------------------------------------------------------------------
vtkAccurateTimer* vtkAccurateTimer::New()
{
	vtkAccurateTimer* ret = vtkAccurateTimer::GetInstance();
	ret->Register(NULL);
	return ret;
} 

//----------------------------------------------------------------------------
vtkAccurateTimer::vtkAccurateTimer()
{

}

//----------------------------------------------------------------------------
vtkAccurateTimer::~vtkAccurateTimer()
{

}

//----------------------------------------------------------------------------
void vtkAccurateTimer::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkAccurateTimer::Delay(double sec)
{
#ifdef _WIN32
	WindowsAccurateTimer* timer = WindowsAccurateTimer::Instance();
	timer->Wait( sec * 1000 ); 
#else
	vtksys::SystemTools::Delay( sec * 1000 ); 
#endif
}

//----------------------------------------------------------------------------
double vtkAccurateTimer::GetInternalSystemTime()
{
#ifdef _WIN32
	return WindowsAccurateTimer::GetSystemTime(); 
#else
	return vtkTimerLog::GetUniversalTime();
#endif
}

//----------------------------------------------------------------------------
double vtkAccurateTimer::GetSystemTime()
{
  return (vtkAccurateTimer::GetInternalSystemTime() - vtkAccurateTimer::SystemStartTime); 
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeString(CurrentDateTimeFormat detailsNeeded)
{
  double currentTime=vtkTimerLog::GetUniversalTime();
  time_t timeSec = floor(currentTime);   
  // Obtain the time of day, and convert it to a tm struct.
#ifdef _WIN32    
  struct tm tmstruct;
  struct tm* ptm=&tmstruct;
  localtime_s( ptm, &timeSec );           
#else
  struct tm *ptm = localtime (&timeSec);
#endif

  // Format the date and time, down to a single second.
  char timeStrSec[80];
  switch (detailsNeeded)
  {
    case CURRENT_DATE:
      strftime (timeStrSec, sizeof (timeStrSec), "%m%d%y", ptm);
      break;
    case CURRENT_TIME:
      strftime (timeStrSec, sizeof (timeStrSec), "%H%M%S", ptm);
      break;
    case CURRENT_DATE_TIME:
    case CURRENT_DATE_TIME_MSEC:
      strftime (timeStrSec, sizeof (timeStrSec), "%m%d%y_%H%M%S", ptm);
      break;
    default:
      return "";
  }
  if (detailsNeeded!=CURRENT_DATE_TIME_MSEC)
  {
    return timeStrSec;
  }
  // Get millisecond as well
  long milliseconds = floor((currentTime-floor(currentTime))*1000.0);  
          
  std::ostrstream mSecStream; 
  mSecStream << timeStrSec << "." << std::setw(3) << std::setfill('0') << milliseconds << std::ends;

  return mSecStream.str();
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateString()
{
  return GetDateAndTimeString(CURRENT_DATE);
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetTimeString()
{
  return GetDateAndTimeString(CURRENT_TIME);  
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeString()
{
  return GetDateAndTimeString(CURRENT_DATE_TIME);
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeMSecString()
{
  return GetDateAndTimeString(CURRENT_DATE_TIME_MSEC);
}
