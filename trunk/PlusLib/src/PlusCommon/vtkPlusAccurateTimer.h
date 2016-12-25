/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAccurateTimer_h
#define __vtkPlusAccurateTimer_h

#include "vtkPlusCommonExport.h"

#include "vtkObject.h"

//----------------------------------------------------------------------------
/*!
  \class vtkPlusAccurateTimerCleanup
  \brief Does the cleanup needed for the singleton class
  \ingroup PlusLibCommon
*/

//BTX
class vtkPlusCommonExport vtkPlusAccurateTimerCleanup
{
public:
  /*! Constructor */
  vtkPlusAccurateTimerCleanup();
  /*! Destructor */
  ~vtkPlusAccurateTimerCleanup();
};
//ETX

//----------------------------------------------------------------------------
/*!
  \class vtkPlusAccurateTimer
  \brief This singleton class is used for accurately measuring elapsed time and getting formatted date strings
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusAccurateTimer : public vtkObject
{
public:
  vtkTypeMacro(vtkPlusAccurateTimer, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
    This is a singleton pattern New.  There will only be ONE
    reference to a vtkPlusAccurateTimer object per process.  Clients that
    call this must call Delete on the object so that the reference
    counting will work.   The single instance will be unreferenced when
    the program exits.
  */
  static vtkPlusAccurateTimer* New();

  /*! Return the singleton instance with no reference counting */
  static vtkPlusAccurateTimer* GetInstance();

  /*!
    Supply a user defined instance. Call Delete() on the supplied
    instance after setting it to fix the reference count.
  */
  static void SetInstance(vtkPlusAccurateTimer* instance);

  /*! Wait until specified time in seconds */
  static void Delay(double sec);

  /*!
    Wait until specified time in seconds. Pending events are processed while waiting.
    Certain devices (e.g., VideoForWindows video source) may be blocked on other threads if events are not processed.
    This method delays the execution by at least the specified amount but is not guaranteed to be accurate (uses sleep).
  */
  static void DelayWithEventProcessing(double sec);

  /*!
    Get system time (elapsed time since last reboot)
    \return Internal system time in seconds
  */
  static double GetInternalSystemTime();

  /*!
    Get the elapsed time since class instantiation
    \return System time in seconds
   */
  static double GetSystemTime();

  /*!
   Get the universal (UTC) time
   \return UTC time in seconds
  */
  static double GetUniversalTime();

  /*!
    Get the universal (UTC) time from system time
    \param systemTime system time in seconds
    \return UTC time in seconds
  */
  static double GetUniversalTimeFromSystemTime(double systemTime);

  /*!
    Get the system time from universal (UTC) time
    \param utcTime UTC time in seconds
    \return system time in seconds
  */
  static double GetSystemTimeFromUniversalTime(double utcTime);

  /*!
    Get current date in string
    \return Format: MMDDYY
   */
  static std::string GetDateString();

  /*!
    Get current time in string
    \return Format: HHMMSS
   */
  static std::string GetTimeString();

  /*!
    Get current date with time in string
    \return Format: MMDDYY_HHMMSS
   */
  static std::string GetDateAndTimeString();

  /*!
    Get current date with time ans ms in string
    \return Format: MMDDYY_HHMMSS.MS
   */
  static std::string GetDateAndTimeMSecString();

protected:
  /*! Constructor */
  vtkPlusAccurateTimer();

  /*! Destructor */
  virtual ~vtkPlusAccurateTimer();

private:
  /*! Copy constructor - Not implemented */
  vtkPlusAccurateTimer(const vtkPlusAccurateTimer&);

  /*! Equality operator - Not implemented */
  void operator=(const vtkPlusAccurateTimer&);

  /*! Internal system time at the time of class instantiation, in seconds */
  static double SystemStartTime;

  /*! Universal time (time elapsed since 00:00:00 January 1, 1970, UTC) at the time of class instantiation, in seconds */
  static double UniversalStartTime;

  /*! The singleton instance */
  static vtkPlusAccurateTimer* Instance;

  /*! The singleton cleanup instance */
  static vtkPlusAccurateTimerCleanup Cleanup;

  enum CurrentDateTimeFormat
  {
    DTF_DATE,
    DTF_TIME,
    DTF_DATE_TIME,
    DTF_DATE_TIME_MSEC
  };

  static std::string GetDateAndTimeString(CurrentDateTimeFormat detailsNeeded, double currentTime);
};

#endif