/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: ndicapi_thread.c,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.3 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=======================================================================*/

#include "ndicapi_thread.h"
#include <stdlib.h>

/* The interface is modelled after the Windows threading interface,
   but the only real difference from POSIX threads is the "Event"
   type which does not exists in POSIX threads (more information is
   provided below) */

#ifdef _WIN32

HANDLE ndiMutexCreate()
{
  return CreateMutex(0,FALSE,0);
}

void ndiMutexDestroy(HANDLE mutex)
{
  CloseHandle(mutex);
}

void ndiMutexLock(HANDLE mutex)
{
  WaitForSingleObject(mutex,INFINITE);
}

void ndiMutexUnlock(HANDLE mutex)
{
  ReleaseMutex(mutex);
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

pthread_mutex_t *ndiMutexCreate()
{
  pthread_mutex_t *mutex;
  mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,0);
  return mutex;
}

void ndiMutexDestroy(pthread_mutex_t *mutex)
{
  pthread_mutex_destroy(mutex);
  free(mutex);
}

void ndiMutexLock(pthread_mutex_t *mutex)
{
  pthread_mutex_lock(mutex);
}

void ndiMutexUnlock(pthread_mutex_t *mutex)
{
  pthread_mutex_unlock(mutex);
}

#endif

#ifdef _WIN32

HANDLE ndiEventCreate()
{
  return CreateEvent(0,FALSE,FALSE,0);
}

void ndiEventDestroy(HANDLE event)
{
  CloseHandle(event);
}

void ndiEventSignal(HANDLE event)
{
  SetEvent(event);
}

int ndiEventWait(HANDLE event, int milliseconds)
{
  if (milliseconds < 0) {
    WaitForSingleObject(event, INFINITE);
  }
  else {
    if (WaitForSingleObject(event, milliseconds) == WAIT_TIMEOUT) {
      return 1;
    }
  }
  return 0;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

/* There is no equivalent of an 'event' in POSIX threads, so we define
   our own event type consisting of a boolean variable (to say whether
   an event has occurred), a cond, an a mutex for locking the cond
   and the variable.
*/

pl_cond_and_mutex_t *ndiEventCreate()
{
  pl_cond_and_mutex_t *event;
  event = (pl_cond_and_mutex_t *)malloc(sizeof(pl_cond_and_mutex_t));
  event->signalled = 0;
  pthread_cond_init(&event->cond, 0);
  pthread_mutex_init(&event->mutex, 0);
  return event;
}

void ndiEventDestroy(pl_cond_and_mutex_t *event)
{
  pthread_cond_destroy(&event->cond);
  pthread_mutex_destroy(&event->mutex);
  free(event);
}

/* Setting the event is simple: lock, set the variable, signal the cond,
   and unlock.
*/

void ndiEventSignal(pl_cond_and_mutex_t *event)
{
  pthread_mutex_lock(&event->mutex);
  event->signalled = 1;
  pthread_cond_signal(&event->cond);
  pthread_mutex_unlock(&event->mutex);
}

/* Waiting for the event is simple if we don't want a timed wait:
   lock, check the variable, wait until the variable becomes set,
   unset the variable, unlock.
   Note that the event can be received by only one thread.
   
   If a timed wait is needed, then there is a little bit of
   hassle because the pthread_cond_timedwait() wait is until
   an absolute time, so we must get the current time and then
   do a little math as well as a conversion from one time structure
   to another time structure.
*/

int ndiEventWait(pl_cond_and_mutex_t *event, int milliseconds)
{
  int timedout = 0;

  if (milliseconds < 0) { /* do infinite wait */
    pthread_mutex_lock(&event->mutex);
    if (event->signalled == 0) {
      pthread_cond_wait(&event->cond, &event->mutex);
    }
    event->signalled = 0;
    pthread_mutex_unlock(&event->mutex);
  }
  else { /* do timed wait */
    struct timeval tv;
    struct timespec ts;

    pthread_mutex_lock(&event->mutex);
    if (event->signalled == 0) {
      /* all the time stuff is used to check for timeouts */
      gettimeofday(&tv, 0);
      tv.tv_sec += milliseconds/1000; /* msec to sec */ 
      tv.tv_usec += (milliseconds % 1000)*1000; /* msec to usec */
      if (tv.tv_usec >= 1000000) { /* if usec overflow */
        tv.tv_usec -= 1000000;
        tv.tv_sec += 1;
      }
      /* convert timeval to timespec */
      ts.tv_sec = tv.tv_sec;
      ts.tv_nsec = tv.tv_usec * 1000; 

#ifdef PTHREAD_COND_TIMEDWAIT_USES_TIMEVAL
      timedout = (pthread_cond_timedwait(&event->cond, &event->mutex,
                                         &tv) == ETIMEDOUT);
#else 
      timedout = (pthread_cond_timedwait(&event->cond, &event->mutex,
                                         &ts) == ETIMEDOUT);
#endif
    }
    if (!timedout) {
      event->signalled = 0;
    }
    pthread_mutex_unlock(&event->mutex);
  }

  return timedout;
}

#endif

#ifdef _WIN32

HANDLE ndiThreadSplit(void *thread_func(void *userdata), void *userdata)
{
  DWORD thread_id; /* we throw the thread id away */
  return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&thread_func,
                      userdata, 0, &thread_id);
}

void ndiThreadJoin(HANDLE thread)
{
  WaitForSingleObject(thread, INFINITE);
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

pthread_t ndiThreadSplit(void *thread_func(void *userdata), void *userdata)
{
  pthread_t thread;
  if (pthread_create(&thread, 0, thread_func, userdata)) {
    return 0;
  }
  return thread;
}

void ndiThreadJoin(pthread_t thread)
{
  pthread_join(thread, 0);
}

#endif
