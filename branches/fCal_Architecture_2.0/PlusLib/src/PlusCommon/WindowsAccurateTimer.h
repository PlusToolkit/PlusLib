// Simple implementation for an accurate wait
// From http://www.codeguru.com/Cpp/W-P/system/timers/comments.php/c5759/?thread=63096

// Attached code works best in multithreaded environment when the number of threads need to wait on the same shared timer object.
// Note that to calculate precise timing it uses high resolution counters. As it has been pointed out in the previous comments,
// that even though this code will work happily on most of the machines, depending on the chip manufacture it may sometimes fail,
// thus one may want to reserve to using only decrement counter as in the original timer example, relying on the fact that timer
// callback gets called every 1msec by the multimedia timers.
// Extra half msec was added purely from my experimental results - to provide higher precision level, you may wish to take it out
// if you are not happy or dont understand why it is there.
// Setting process and thread priority is needed here because NT schedule doesnt deal quite right with the processes that run in
// the background when threads get suspended and resumed. It gives very high precision results when run in the foreground but
// causes delays when run in trhe background unless priority is adjusted. Priority does not cause any problems unless your
// process monopolises all available CPU - timer code by itself consumes pretty much 0%.

//Uncomment TIMING_DEBUG define to provide you with statistical feedback from the timer.

// ------------------------------------------------------
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

//-------------------------------------------------------------------
// this timer class was designed to give high precision < 2% average
// error and it was designed to work best with multi-threaded clients
// waiting on the single instance of this timer
//
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

   // On some Windows XP SP3 machines the vtkTimerLog::GetUniversalTime() 
   // measures time with 15 ms resolution
   // timeBeginPeriod and timeEndPeriod are probably not needed because 
   // timer is already set to high frequency update in the constructor. 
   static inline double GetSystemTime()
   {
	 return 0.001 * timeGetTime(); 
   }

   ///////////////////////////////////////////////////////////////
   // Function name   : Wait
   // Description     : Timer wait method
   //                 : 
   // Return type     : void  : 
   // Argument        : int timeout : 
   ///////////////////////////////////////////////////////////////
   void Wait(int timeout)
   {
//#define TIMING_DEBUG
#ifdef TIMING_DEBUG
      LARGE_INTEGER s[2]; 
      QueryPerformanceCounter(&s[0]);     // timing starts

#endif
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
#ifdef TIMING_DEBUG
      QueryPerformanceCounter(&s[1]);   // wake up call and timing ends
      double diff = ((double)s[1].QuadPart - (double)s[0].QuadPart)/(double)freq.QuadPart;
      double percent = /*Absolute*/(diff-timeout)*100.0/(timeout?timeout:1);

      printf("intended to sleep for: %3ld. slept for: %8.4f. error = %8.4f\n",
             timeout, diff, percent);

      error += percent;
      ++count;
#endif
   }
   inline double        Absolute(double val) { return val > 0 ? val : -val; }
   inline LARGE_INTEGER GetFrequency()       { return freq; };

   ///////////////////////////////////////////////////////////////
   // Function name   : CALLBACK TimerFunc
   // Description     : Media callback timer method
   //                 : 
   // Return type     : static void  : 
   // Argument        : UINT uID : 
   // Argument        : UINT uMsg : 
   // Argument        : DWORD dwUser : 
   // Argument        : DWORD dw1 : 
   // Argument        : DWORD dw2 : 
   ///////////////////////////////////////////////////////////////
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
         if ( now >= it->timeout.QuadPart )
         {
            ResumeThread(it->h);
            it = pThis->waitPool.erase(it);
            continue;
         }
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

   struct WaitData
   { 
      WaitData(HANDLE _h, int _t) : h(_h)
      {
         LARGE_INTEGER now; 
         QueryPerformanceCounter(&now);
         timeout.QuadPart = _t * WindowsAccurateTimer::Instance()->freq.QuadPart + now.QuadPart;
      };

      HANDLE h;
      LARGE_INTEGER timeout;
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
