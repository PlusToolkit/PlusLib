/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Test if no thread hang occur when using vtkAccurateTimer
// On Windows also test if the timings are accurate enough

#include "PlusConfigure.h"
#include <time.h>
#include "vtksys/CommandLineArguments.hxx"
#include "vtkMultiThreader.h"
#include "vtkRecursiveCriticalSection.h"

//----------------------------------------------------------------------------
// Global variables for communicating with the threads
double gMaxDelaySec = 0.005;
bool gStopRequested = false;
const unsigned int MAX_DELAY_ERROR_SAMPLES=5e6; // limit the max number of samples to avoid crashes due to memory problems
std::vector<double> gDelayErrorsSec; // access controlled by gCritSec
int gNumberOfDelayErrors = 0; // access controlled by gCritSec
int gNumberOfThreadCompletions = 0; // access controlled by gCritSec
double gMaxDelayErrorSec = 0.010;
vtkSmartPointer<vtkRecursiveCriticalSection> gCritSec = vtkSmartPointer<vtkRecursiveCriticalSection>::New();

//----------------------------------------------------------------------------
// Thread function
void* timerTestThread( vtkMultiThreader::ThreadInfo *data )
{      
  // need to initialize random generation in each thread
  srand((unsigned int)(vtkAccurateTimer::GetSystemTime()-floor(vtkAccurateTimer::GetSystemTime()))*1e6);

  int heartbeatStepCount=1000; // a message will be logged after heartbeatStepCount steps (to indicate if the thread is still alive)
  int stepCount=0;
  while ( !gStopRequested )
  {
    double delaySec=gMaxDelaySec*double(rand())/double(RAND_MAX);
    
    // Measure the delay now
    double timestampBefore=vtkAccurateTimer::GetSystemTime();
    vtkAccurateTimer::Delay(delaySec);
    double timestampAfter=vtkAccurateTimer::GetSystemTime();

    // Analyze and store the results
    double elapsedTimeSec = timestampAfter-timestampBefore;
    double delayErrorSec = (elapsedTimeSec-delaySec);
    double absDelayErrorSec = fabs(delayErrorSec);

    gCritSec->Lock();
    if (gDelayErrorsSec.size()<MAX_DELAY_ERROR_SAMPLES)
    {
      gDelayErrorsSec.push_back(delayErrorSec);
    }
    gCritSec->Unlock();

    if (absDelayErrorSec>gMaxDelayErrorSec)
    {
      gCritSec->Lock();
      gNumberOfDelayErrors++;
      gCritSec->Unlock();
      LOG_ERROR("Delay error = " << delayErrorSec*1000.0 << " ms (intended delay = " << delaySec*1000.0 << ", actual delay = " << elapsedTimeSec*1000.0 << "), thread="<<data->ThreadID);
    }
    else
    {
      // String operations may introduce significant performance overhead,
      // so construct the message only if it will be really logged
      if (vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_DEBUG)
      {
        LOG_DEBUG("Delay error = " << delayErrorSec*1000.0 << " ms (intended delay = " << delaySec*1000.0 << ", actual delay = " << elapsedTimeSec*1000.0 << "), thread="<<data->ThreadID);
      }
    }

    stepCount++;
    if (stepCount % heartbeatStepCount == 0)
    {
      LOG_DEBUG("Thread "<<data->ThreadID<<" reached "<<stepCount<<" delays");
    }
  }

  gCritSec->Lock();
  gNumberOfThreadCompletions++;
  gCritSec->Unlock();

  LOG_INFO("Thread completed: "<<data->ThreadID);

  return 0;
}



int main(int argc, char **argv)
{
	bool printHelp(false);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  int testTimeSec = 10.0;
  int numberOfThreads = 2;
  double averageIntendedDelaySec=0.003;
  
	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
  args.AddArgument("--testTimeSec", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &testTimeSec, "Length of the test run (in seconds)");	
  args.AddArgument("--maxDelayErrorSec", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &gMaxDelayErrorSec, "If the difference between intended and actual delay is larger than this value then it is reported as an error (in seconds)");	
  args.AddArgument("--averageIntendedDelaySec", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &averageIntendedDelaySec, "Controls the average delay time, the delay is chosen from a uniform distribution from the interval [0, averageIntendedDelaySec*2] (in seconds)");	
  args.AddArgument("--numberOfThreads", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfThreads, "Number of test threads that performs the delays");	
	
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( printHelp ) 
	{
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 
	}	

  gMaxDelaySec=averageIntendedDelaySec*2.0; // delay is generated by a uniform distribution => max = mean * 2

  LOG_INFO("Testing the accurate timer: numberOfThreads="<<numberOfThreads    
    <<", averageIntendedDelaySec="<<averageIntendedDelaySec
    <<", maxDelayErrorSec="<<gMaxDelayErrorSec);
	
  if (numberOfThreads>VTK_MAX_THREADS)
  {
    LOG_ERROR("Number of requested threads ("<<numberOfThreads<<") larger than the maximum allowed ("<<VTK_MAX_THREADS<<")");
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkMultiThreader> multithreader = vtkSmartPointer<vtkMultiThreader>::New();

  for (int i=0; i<numberOfThreads; i++)
  {
    // have some time between the startup of the threads to allow initialization of the accurate timer 
    // and also to make the threads a bit less similar to each other in term of timing
    vtkAccurateTimer::Delay(0.2); 

    int threadId = multithreader->SpawnThread( (vtkThreadFunctionType)&timerTestThread, NULL );
    LOG_INFO("Thread started: "<<threadId);
  }

  LOG_INFO("Testing delay timing accuracy... (test duration time = "<<testTimeSec<<"sec)");

#ifdef _WIN32
  Sleep(testTimeSec*1000);
#else
  usleep(testTimeSec * 1000000);
#endif 

  LOG_INFO("Testing completed, stop threads");

  // Signal to the threads that they should quit
  gStopRequested=true;

  // Wait until all the threads quit (max delay + 1 second margin) 
#ifdef _WIN32
  Sleep((gMaxDelaySec+1.0) * 2 * 1000);
#else
  usleep((gMaxDelaySec+1.0) * 2 * 1000000);
#endif 

  // Compute delay error statistics
  gCritSec->Lock();
  double delayErrorSecSum=0;
  for (std::vector<double>::iterator it=gDelayErrorsSec.begin(); it!=gDelayErrorsSec.end(); ++it)
  {
    delayErrorSecSum+=*it;
  }
  double delayErrorSecMean=delayErrorSecSum/double(gDelayErrorsSec.size());
  double delayErrorSecVar=0;
  for (std::vector<double>::iterator it=gDelayErrorsSec.begin(); it!=gDelayErrorsSec.end(); ++it)
  {
    delayErrorSecVar+=(*it-delayErrorSecMean)*(*it-delayErrorSecMean);
  }
  double delayErrorSecStdev=sqrt(delayErrorSecVar/double(gDelayErrorsSec.size()));
  gCritSec->Unlock();

  LOG_INFO("Delay error statistics: mean="<<delayErrorSecMean*1000<<"ms, stdev="<<delayErrorSecStdev*1000<<"ms, number of samples = "<<gDelayErrorsSec.size());

  LOG_INFO("Number of completed threads: "<<gNumberOfThreadCompletions);
  if (gNumberOfThreadCompletions!=numberOfThreads)
  {
    LOG_ERROR("Number of completed threads ("<<gNumberOfThreadCompletions<<") does not match the number of started threads ("<<numberOfThreads<<")");
    return EXIT_FAILURE;
  }

  if (gNumberOfDelayErrors>0)
  {
    LOG_ERROR("There were " << gNumberOfDelayErrors << " timer errors (more than "<<gMaxDelayErrorSec<<" sec difference in intended and actual delay)");
    return EXIT_FAILURE;
  }

	return EXIT_SUCCESS; 
 }
