/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __WINDOWS_ACCURATE_TIMER_H__
#define __WINDOWS_ACCURATE_TIMER_H__

#pragma warning ( disable : 4786 )

#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <deque>

#include "PlusCommon.h"

/*!
  If a waiting thread cannot ber resumed in this many attempts then 
  the thread is just removed from the wait pool and resume will not be attempted
  any more.
*/
static const int MAX_RESUME_ATTEMPTS=100;

/*!
\class WindowsAccurateTimer 
\brief Accurate delay and stopwatch function implementation in Windows

  This timer class was designed to give high precision < 2% average
  error and it was designed to work best with multi-threaded clients
  waiting on the single instance of this timer

  The implementation is based on this code sample:
  From http://www.codeguru.com/Cpp/W-P/system/timers/comments.php/c5759/?thread=63096

  The code works best in multithreaded environment when the number of threads need to wait on the same shared timer object.
  Note that to calculate precise timing it uses high resolution counters. As it has been pointed out in the previous comments,
  that even though this code will work happily on most of the machines, depending on the chip manufacture it may sometimes fail,
  thus one may want to reserve to using only decrement counter as in the original timer example, relying on the fact that timer
  callback gets called every 1msec by the multimedia timers.
  Extra half msec was added purely from my experimental results - to provide higher precision level, you may wish to take it out
  if you are not happy or dont understand why it is there.
  Setting process and thread priority is needed here because NT schedule doesnt deal quite right with the processes that run in
  the background when threads get suspended and resumed. It gives very high precision results when run in the foreground but
  causes delays when run in trhe background unless priority is adjusted. Priority does not cause any problems unless your
  process monopolises all available CPU - timer code by itself consumes pretty much 0%.

\ingroup PlusLibCommon
*/
class WindowsAccurateTimer
{
public:
  static WindowsAccurateTimer* Instance()
  {
    return &instance;
  };
  virtual ~WindowsAccurateTimer()
  {
    timeKillEvent(timer), timer = 0;
    DeleteCriticalSection(&crit);
  }

  /*!
    On some Windows XP SP3 machines the vtkTimerLog::GetUniversalTime() 
    measures time with 15 ms resolution
    timeBeginPeriod and timeEndPeriod are probably not needed because 
    timer is already set to high frequency update in the constructor. 
  */
  static inline double GetSystemTime()
  {
    return 0.001 * timeGetTime(); 
  }

  /*! 
    Wait for a specified amount of time
    \param waiting time in ms
  */
  void Wait(int timeout)
  {
    if ( timeout > 0 )   // anything to wait on ?
    {
      HANDLE tHandle = 0;
      HANDLE pHandle = GetCurrentProcess();
      DuplicateHandle(pHandle, GetCurrentThread(), pHandle,
        &tHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
      // wrap pool data access
      EnterCriticalSection(&WindowsAccurateTimer::Instance()->crit);
      waitPool.push_back(WaitData(tHandle, timeout));
      LeaveCriticalSection(&WindowsAccurateTimer::Instance()->crit);
      SuspendThread(tHandle);        // do the wait
      CloseHandle(tHandle);
    }
  }
  inline double Absolute(double val) { return val > 0 ? val : -val; }
  inline LARGE_INTEGER GetFrequency() { return freq; };

  /*! Media callback timer method */
  static void CALLBACK TimerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
  {
    static LARGE_INTEGER s[2]; 
    static bool init = false;

    if ( !init )
    {
      init = true;
      HANDLE tHandle = 0;
      HANDLE pHandle = GetCurrentProcess();
      DuplicateHandle(pHandle, GetCurrentThread(), pHandle,
        &tHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
      SetThreadPriority(tHandle, THREAD_PRIORITY_TIME_CRITICAL);
      HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _getpid());
      if (!SetPriorityClass(h, REALTIME_PRIORITY_CLASS))
      {
        DWORD err = GetLastError();
      }
      CloseHandle(h);
      CloseHandle(tHandle);
    }

    WindowsAccurateTimer* pThis = (WindowsAccurateTimer*)dwUser;

    QueryPerformanceCounter(&s[1]);
    double diff = (((double)s[1].QuadPart - (double)s[0].QuadPart)/(double)pThis->freq.QuadPart);
    __int64 now = s[1].QuadPart+pThis->halfMsec.QuadPart;

    EnterCriticalSection(&pThis->crit);
    std::deque<WaitData>::iterator it = pThis->waitPool.begin();
    while ( it != pThis->waitPool.end() )
    {
      if ( now < it->timeout.QuadPart )
      {
        // delay time not yet elapsed
        ++it;
        continue;
      }

      // delay time elapsed, resume the thread
      DWORD res=ResumeThread(it->h);
      if (res>0)
      {
        // the thread was successfully waken up, it can be removed from the wait pool
        it = pThis->waitPool.erase(it);
        continue;
      }
      
      // something is wrong with the thread resume
      ++(it->errorCount);

      if (res==DWORD(-1))
      {
        DWORD err=GetLastError();
        LOG_ERROR("Cannot resume thread "<<it->h<<", error: "<<err);
      }
      else if (res<1)
      {
        // The thread has not yet been suspended, so don't try to resume it yet.
        // (keep it in the wait pool, it will be suspended later and we have to resume it to avoid hang)
        LOG_DEBUG("Cannot resume thread "<<it->h<<", it has not yet been suspended");
      }

      if (it->errorCount>MAX_RESUME_ATTEMPTS)
      {
        LOG_ERROR("Maximum thread resume attempts reached, remove item "<<it->h<<"from the wait pool");
        it = pThis->waitPool.erase(it);
        continue;
      }

      // Resume failed this time, but try it again later; now go to the next item in the pool
      ++it;
    }
    LeaveCriticalSection(&pThis->crit);
    s[0] = s[1];
  }

  volatile static double  error;
  volatile static int     count;

private:
  WindowsAccurateTimer()
  {
    error = 0;
    count = 0;
    QueryPerformanceFrequency(&freq);
    halfMsec.QuadPart = freq.QuadPart / 2000;
    freq.QuadPart /= 1000;   // convert to msecs
    InitializeCriticalSection(&crit);
    timer = timeSetEvent(1, 0, TimerFunc, (DWORD)this, TIME_PERIODIC);
  }

  /*!
    \struct WaitData 
    \brief Stores information for a wait object (used by the accurate timer implementation in Windows)
    \ingroup PlusLibCommon
  */
  struct WaitData
  { 
    WaitData(HANDLE _h, int _t) : h(_h)
    {
      LARGE_INTEGER now; 
      QueryPerformanceCounter(&now);
      errorCount=0;
      timeout.QuadPart = _t * WindowsAccurateTimer::Instance()->freq.QuadPart + now.QuadPart;
    };

    HANDLE h;
    LARGE_INTEGER timeout;
    /*! 
      If the thread cannot be resumed then this counter is incremented, 
      after many errors the item may be removed from the wait pool
    */
    int errorCount; 
  };
  friend struct WaitData;

  static WindowsAccurateTimer    instance;

  CRITICAL_SECTION        crit;
  std::deque<WaitData>         waitPool;
  MMRESULT                timer;
  LARGE_INTEGER           freq;
  LARGE_INTEGER           halfMsec;
};

#endif  // __WINDOWS_ACCURATE_TIMER_H__
