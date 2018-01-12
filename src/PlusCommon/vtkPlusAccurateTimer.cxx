/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusAccurateTimer.h"
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

double vtkPlusAccurateTimer::SystemStartTime = 0;
double vtkPlusAccurateTimer::UniversalStartTime = 0;

//----------------------------------------------------------------------------
// The singleton, and the singleton cleanup

vtkPlusAccurateTimer* vtkPlusAccurateTimer::Instance = NULL;
vtkPlusAccurateTimerCleanup vtkPlusAccurateTimer::Cleanup;

vtkPlusAccurateTimerCleanup::vtkPlusAccurateTimerCleanup() {}

vtkPlusAccurateTimerCleanup::~vtkPlusAccurateTimerCleanup()
{
  // Destroy any remaining output window.
  vtkPlusAccurateTimer::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkPlusAccurateTimer* vtkPlusAccurateTimer::GetInstance()
{
  if (!vtkPlusAccurateTimer::Instance)
  {
    vtkPlusAccurateTimer::Instance = static_cast<vtkPlusAccurateTimer*>(
                                       vtkObjectFactory::CreateInstance("vtkPlusAccurateTimer"));
    if (!vtkPlusAccurateTimer::Instance)
    {
      vtkPlusAccurateTimer::Instance = new vtkPlusAccurateTimer;
    }
    vtkPlusAccurateTimer::Instance->SystemStartTime = vtkPlusAccurateTimer::Instance->GetInternalSystemTime();
    vtkPlusAccurateTimer::Instance->UniversalStartTime = vtkTimerLog::GetUniversalTime();
    LOG_INFO("System start timestamp: " << std::fixed << std::setprecision(0) << vtkPlusAccurateTimer::Instance->SystemStartTime);
    LOG_DEBUG("AccurateTimer universal start time: " << GetDateAndTimeString(DTF_DATE_TIME_MSEC, vtkPlusAccurateTimer::UniversalStartTime));
  }
  return vtkPlusAccurateTimer::Instance;
}

//----------------------------------------------------------------------------
void vtkPlusAccurateTimer::SetInstance(vtkPlusAccurateTimer* instance)
{
  if (vtkPlusAccurateTimer::Instance == instance)
  {
    return;
  }

  if (vtkPlusAccurateTimer::Instance)
  {
    vtkPlusAccurateTimer::Instance->Delete();
  }

  vtkPlusAccurateTimer::Instance = instance;

  // User will call ->Delete() after setting instance

  if (instance)
  {
    instance->Register(NULL);
  }
}

//----------------------------------------------------------------------------
vtkPlusAccurateTimer* vtkPlusAccurateTimer::New()
{
  vtkPlusAccurateTimer* ret = vtkPlusAccurateTimer::GetInstance();
  ret->Register(NULL);
  return ret;
}

//----------------------------------------------------------------------------
vtkPlusAccurateTimer::vtkPlusAccurateTimer()
{

}

//----------------------------------------------------------------------------
vtkPlusAccurateTimer::~vtkPlusAccurateTimer()
{

}

//----------------------------------------------------------------------------
void vtkPlusAccurateTimer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusAccurateTimer::Delay(double sec)
{
#ifndef PLUS_USE_SIMPLE_TIMER
  // Use accurate timer
#ifdef _WIN32
  WindowsAccurateTimer* timer = WindowsAccurateTimer::Instance();
  timer->Wait(sec * 1000);
#else
  vtksys::SystemTools::Delay(sec * 1000);
#endif
#else // PLUS_USE_SIMPLE_TIMER
  // Use simple timer
  vtksys::SystemTools::Delay(sec * 1000);
  return;
#endif
}

void vtkPlusAccurateTimer::DelayWithEventProcessing(double waitTimeSec)
{
#ifdef _WIN32
  double waitStartTime = vtkPlusAccurateTimer::GetSystemTime();
  const double commandQueuePollIntervalSec = 0.010;
  do
  {
    // Need to process messages because some devices (such as the vtkPlusWin32VideoSource2) require event processing
    MSG Msg;
    while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
    Sleep(commandQueuePollIntervalSec * 1000); // give a chance to other threads to get CPU time now
  }
  while (vtkPlusAccurateTimer::GetSystemTime() - waitStartTime < waitTimeSec);
#else
  usleep(waitTimeSec * 1000000);
#endif
}

//----------------------------------------------------------------------------
double vtkPlusAccurateTimer::GetInternalSystemTime()
{
#ifdef _WIN32
  return WindowsAccurateTimer::GetSystemTime();
#else
  return vtkTimerLog::GetUniversalTime();
#endif
}

//----------------------------------------------------------------------------
double vtkPlusAccurateTimer::GetSystemTime()
{
  return (vtkPlusAccurateTimer::GetInternalSystemTime() - vtkPlusAccurateTimer::SystemStartTime);
}

//----------------------------------------------------------------------------
double vtkPlusAccurateTimer::GetUniversalTime()
{
  return vtkPlusAccurateTimer::UniversalStartTime + (vtkPlusAccurateTimer::GetInternalSystemTime() - vtkPlusAccurateTimer::SystemStartTime);
}

//----------------------------------------------------------------------------
std::string vtkPlusAccurateTimer::GetDateAndTimeString(CurrentDateTimeFormat detailsNeeded, double currentTime)
{
  time_t timeSec = floor(currentTime);
  // Obtain the time of day, and convert it to a tm struct.
#ifdef _WIN32
  struct tm tmstruct;
  struct tm* ptm = &tmstruct;
  localtime_s(ptm, &timeSec);
#else
  struct tm* ptm = localtime(&timeSec);
#endif

  // Format the date and time, down to a single second.
  char timeStrSec[80];
  switch (detailsNeeded)
  {
    case DTF_DATE:
      strftime(timeStrSec, sizeof(timeStrSec), "%m%d%y", ptm);
      break;
    case DTF_TIME:
      strftime(timeStrSec, sizeof(timeStrSec), "%H%M%S", ptm);
      break;
    case DTF_DATE_TIME:
    case DTF_DATE_TIME_MSEC:
      strftime(timeStrSec, sizeof(timeStrSec), "%m%d%y_%H%M%S", ptm);
      break;
    default:
      return "";
  }
  if (detailsNeeded != DTF_DATE_TIME_MSEC)
  {
    return timeStrSec;
  }
  // Get millisecond as well
  long milliseconds = floor((currentTime - floor(currentTime)) * 1000.0);

  std::ostringstream mSecStream;
  mSecStream << timeStrSec << "." << std::setw(3) << std::setfill('0') << milliseconds;

  return mSecStream.str();
}

//----------------------------------------------------------------------------
std::string vtkPlusAccurateTimer::GetDateString()
{
  return GetDateAndTimeString(DTF_DATE, vtkPlusAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
std::string vtkPlusAccurateTimer::GetTimeString()
{
  return GetDateAndTimeString(DTF_TIME, vtkPlusAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
std::string vtkPlusAccurateTimer::GetDateAndTimeString()
{
  return GetDateAndTimeString(DTF_DATE_TIME, vtkPlusAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
std::string vtkPlusAccurateTimer::GetDateAndTimeMSecString()
{
  return GetDateAndTimeString(DTF_DATE_TIME_MSEC, vtkPlusAccurateTimer::GetUniversalTime());
}

//----------------------------------------------------------------------------
double vtkPlusAccurateTimer::GetUniversalTimeFromSystemTime(double systemTime)
{
  return vtkPlusAccurateTimer::UniversalStartTime + systemTime;
}

//----------------------------------------------------------------------------
double vtkPlusAccurateTimer::GetSystemTimeFromUniversalTime(double utcTime)
{
  return utcTime - vtkPlusAccurateTimer::UniversalStartTime;
}
