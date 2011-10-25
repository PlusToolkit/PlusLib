/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "Timer.h"

#include <windows.h>
 
 #ifndef hr_timer
 #include "Timer.h"
 #define hr_timer
 #endif
 
 double Timer::LIToSecs( LARGE_INTEGER & L) {
     return ((double)L.QuadPart /(double)frequency.QuadPart) ;
 }
 
 Timer::Timer(){
     timer.start.QuadPart=0;
     timer.stop.QuadPart=0; 
     QueryPerformanceFrequency( &frequency ) ;
 }
 
 void Timer::startTimer( ) {
     QueryPerformanceCounter(&timer.start) ;
 }
 
 void Timer::stopTimer( ) {
     QueryPerformanceCounter(&timer.stop) ;
 }
 
 double Timer::getElapsedTime() {
     LARGE_INTEGER time;
     time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
     return LIToSecs( time) ;
 }

