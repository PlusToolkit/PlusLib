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
#include "PlusCommon.h"

double vtkAccurateTimer::SystemStartTime=0;
double vtkAccurateTimer::UniversalStartTime=0;

//----------------------------------------------------------------------------
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
    vtkAccurateTimer::Instance->UniversalStartTime = vtkTimerLog::GetUniversalTime(); 
    LOG_DEBUG("AccurateTimer universal start time: "<<GetDateAndTimeString(DTF_DATE_TIME_MSEC, vtkAccurateTimer::UniversalStartTime));
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
#ifndef PLUS_USE_SIMPLE_TIMER
  // Use accurate timer
#ifdef _WIN32
  WindowsAccurateTimer* timer = WindowsAccurateTimer::Instance();
  timer->Wait( sec * 1000 ); 
#else
  vtksys::SystemTools::Delay( sec * 1000 );
#endif
#else // PLUS_USE_SIMPLE_TIMER
  // Use simple timer
  vtksys::SystemTools::Delay( sec * 1000 );
  return;
#endif
}

void vtkAccurateTimer::DelayWithEventProcessing(double waitTimeSec)
{
#ifdef _WIN32
  double waitStartTime=vtkAccurateTimer::GetSystemTime();
  const double commandQueuePollIntervalSec=0.010;
  do
  {
    // Need to process messages because some devices (such as the vtkWin32VideoSource2) require event processing
    MSG Msg;
    while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
    Sleep(commandQueuePollIntervalSec*1000); // give a chance to other threads to get CPU time now
  } while (vtkAccurateTimer::GetSystemTime()-waitStartTime<waitTimeSec);
#else
    usleep(waitTimeSec * 1000000);
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
double vtkAccurateTimer::GetUniversalTime()
{
  return vtkAccurateTimer::UniversalStartTime+(vtkAccurateTimer::GetInternalSystemTime() - vtkAccurateTimer::SystemStartTime); 
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeString(CurrentDateTimeFormat detailsNeeded, double currentTime)
{
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
    case DTF_DATE:
      strftime (timeStrSec, sizeof (timeStrSec), "%m%d%y", ptm);
      break;
    case DTF_TIME:
      strftime (timeStrSec, sizeof (timeStrSec), "%H%M%S", ptm);
      break;
    case DTF_DATE_TIME:
    case DTF_DATE_TIME_MSEC:
      strftime (timeStrSec, sizeof (timeStrSec), "%m%d%y_%H%M%S", ptm);
      break;
    default:
      return "";
  }
  if (detailsNeeded!=DTF_DATE_TIME_MSEC)
  {
    return timeStrSec;
  }
  // Get millisecond as well
  long milliseconds = floor((currentTime-floor(currentTime))*1000.0);  
          
  std::ostringstream mSecStream; 
  mSecStream << timeStrSec << "." << std::setw(3) << std::setfill('0') << milliseconds;

  return mSecStream.str();
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateString()
{
  return GetDateAndTimeString(DTF_DATE, vtkAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetTimeString()
{
  return GetDateAndTimeString(DTF_TIME, vtkAccurateTimer::GetUniversalTime());  
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeString()
{
  return GetDateAndTimeString(DTF_DATE_TIME, vtkAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeMSecString()
{
  return GetDateAndTimeString(DTF_DATE_TIME_MSEC, vtkAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
double vtkAccurateTimer::GetUniversalTimeFromSystemTime(double systemTime)
{
  return vtkAccurateTimer::UniversalStartTime+systemTime; 
}

//----------------------------------------------------------------------------
double vtkAccurateTimer::GetSystemTimeFromUniversalTime(double utcTime)
{
  return utcTime-vtkAccurateTimer::UniversalStartTime; 
}
