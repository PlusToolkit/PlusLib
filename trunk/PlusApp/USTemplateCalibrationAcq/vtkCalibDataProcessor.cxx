#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <assert.h>

// reconstruction output has two scalar components
// 0th - image data
// 1st - whether the pixel was hit
// We just need the first one
#define REMOVE_ALPHA_CHANNEL

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkCalibDataProcessor.h"
#include "vtkTransform.h"
#include "vtkUltrasoundDataCollector.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkTracker.h"
#include "vtkSignalBox.h"
#include "vtkVideoSource2.h"
#include "vtkCalibDataSender.h"

using namespace std;

static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkCalibDataProcessor, "$Revision$");
vtkStandardNewMacro(vtkCalibDataProcessor);

/******************************************************************************
* vtkCalibDataProcessor()
*
*  Constructor
*
*  @Author:Jan Gumprecht
*  @Date:  27.January 2009
*
* ****************************************************************************/
vtkCalibDataProcessor::vtkCalibDataProcessor()
{

	this->USDataCollector = NULL;
	this->DataSender = NULL;

	this->ProcessPeriod = 1.0;

	this->Processing = false;
	this->StartUpTime = vtkTimerLog::GetUniversalTime();

	this->PlayerThreader = vtkMultiThreader::New();
	this->PlayerThreadId = -1;

	this->Verbose = false;
	this->LogStream.ostream::rdbuf(cerr.rdbuf());

#ifdef REMOVE_ALPHA_CHANNEL
	this->ComponentExtractor = vtkImageExtractComponents::New();
#endif

}

/******************************************************************************
* ~vtkCalibDataProcessor()
*
*  Destructor
*
*  @Author:Jan Gumprecht
*  @Date:  27.January 2009
*
* ****************************************************************************/
vtkCalibDataProcessor::~vtkCalibDataProcessor()
{
	this->StopProcessing();

	this->PlayerThreader->Delete();

#ifdef REMOVE_ALPHA_CHANNEL
	if (this->ComponentExtractor)
	{
		this->ComponentExtractor->Delete();
	}
#endif
}
/******************************************************************************
*  PrintSelf(ostream& os, vtkIndent indent)
*
*  Print information about the instance
*
*  @Author:Jan Gumprecht
*  @Date:  27.January 2009
*
* ****************************************************************************/
void vtkCalibDataProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkCalibDataProcessor, USDataCollector, vtkUltrasoundDataCollector);

/******************************************************************************
*  static inline void vtkSleep(double duration)
*
*  Platform-independent sleep function
*  Set the current thread to sleep for a certain amount of time
*
*  @Param: double duration - Time to sleep in ms
*
* ****************************************************************************/
static inline void vtkSleep(double duration)
{
	duration = duration; // avoid warnings
	// sleep according to OS preference
#ifdef _WIN32
	Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || defined(__APPLE__)
	struct timespec sleep_time, dummy;
	sleep_time.tv_sec = (int)duration;
	sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
	nanosleep(&sleep_time,&dummy);
#endif
}

/******************************************************************************
*  static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time)
*
*  Sleep until the specified absolute time has arrived.
*  You must pass a handle to the current thread.
*  If '0' is returned, then the thread was aborted before or during the wait. *
*
*  @Author:Jan Gumprecht
*  @Date:  27.January 2009
*
*  @Param: vtkMultiThreader::ThreadInfo *data *
*  @Param: double time - Time until which to sleep
*  @Param: bool checkActiveFlagImmediately - avoid waiting if enabled
*
* ****************************************************************************/
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time, bool checkActiveFlagImmediately)
{
	if(checkActiveFlagImmediately)
	{
		// check to see if we are being told to quit
		data->ActiveFlagLock->Lock();
		int activeFlag = *(data->ActiveFlag);
		data->ActiveFlagLock->Unlock();

		if (activeFlag == 0)
		{//We are told to quit
			return 0;
		}

		//Go on processing
		return 1;
	}

	// loop either until the time has arrived or until the thread is ended
	for (int i = 0;; i++)
	{
		double remaining = time - vtkTimerLog::GetUniversalTime();

		// check to see if we have reached the specified time
		if (remaining <= 0)
		{
			return 1;
		}
		// check the ActiveFlag at least every 0.1 seconds
		if (remaining > 0.1)
		{
			remaining = 0.1;
		}

		// check to see if we are being told to quit
		data->ActiveFlagLock->Lock();
		int activeFlag = *(data->ActiveFlag);
		data->ActiveFlagLock->Unlock();

		if (activeFlag == 0)
		{
			break;
		}

		vtkSleep(remaining);
	}

	return 0;
}

/******************************************************************************
*  static void *vtkDataProcessorThread(vtkMultiThreader::ThreadInfo *data)
*
*  This function runs in an alternate thread to asyncronously collect data
*
*  @Author:Jan Gumprecht
*  @Date:  27.Januar 2009
*
*  @Param: vtkMultiThreader::ThreadInfo *data
*
* ****************************************************************************/
static void *vtkDataProcessorThread(vtkMultiThreader::ThreadInfo *data)
{
	vtkCalibDataProcessor *self = (vtkCalibDataProcessor *)(data->UserData);
	vtkCalibDataSender* sender = self->GetDataSender();
	int senderBufferSize = sender->GetSendDataBufferSize();
	float processPeriod = self->GetProcessPeriod();
	int lastDataSenderIndex = -2;
	int currentIndex = -1;
	int errors = 0;
	string input;
	double sectionTime;
	double loopTime;
	int numOutputVolumes = self->GetUSDataCollector()->GetNumberOfOutputVolumes();
	int port = 0;

#ifdef  DEBUGPROCESSOR
	self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread started" << endl;
#endif

	do
	{
		if(!self->GetProcessing())
		{
#ifdef DEBUGCOLLECTOR
			self->GetLogStream() << self->GetUpTime() << " |P-WARNING: Data processor stopped processing -> Stop collecting" << endl;
#endif
			cout << "====================================================" << endl << endl
				<< "--- 4D Ultrasound stopped working ---" << endl << endl
				<< "Press 't' and hit 'ENTER' to terminate 4D Ultrasound"<< endl;
		}
		// we are processing
		else
		{
			if(!sender->GetSending())
			{
#ifdef DEBUGPROCESSOR
				self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Data sender stopped sending -> stop processing" << endl;
#endif
				self->StopProcessing();
			}
			else if(sender->IsSendDataBufferFull())
			{
#ifdef DEBUGPROCESSOR
				self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Data sender is too slow -> wait" << endl;
#endif
			}
			// we are ok to go
			else
			{
				loopTime = self->GetUpTime();

				sectionTime = self->GetUpTime();
				if (lastDataSenderIndex >= 0 && lastDataSenderIndex < senderBufferSize)
				{
					self->ResetOldVolume(lastDataSenderIndex);
				}

				sectionTime = self->GetUpTime();

#ifdef  TIMINGPROCESSOR
				self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Volume Reconstruction DONE" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
#endif
				sectionTime = self->GetUpTime();

				vtkImageData *output = self->GetUSDataCollector()->GetOutputFromPort(port);
				//output->Update();

				if (output)
				{
					lastDataSenderIndex = self->ForwardData(output, port);

#ifdef  TIMINGPROCESSOR
					self->GetLogStream() << self->GetUpTime() << " |P-INFO: Volume forwarding DONE" << endl
						<< "          | Last data sender index: " << lastDataSenderIndex
						<< " | S: " << self->GetUpTime() - sectionTime
						<< " | L:" << self->GetUpTime() - loopTime
						<< " | FPS: " << 1 / (self->GetUpTime() - loopTime) << endl;
#endif
				}

				// go to the next output volume
				port = (port + 1) % numOutputVolumes;

				if(lastDataSenderIndex == -1)
				{
					errors++;
				}

				if(errors > 50)
				{
#ifdef ERRORPROCESSOR
					self->GetLogStream() <<  self->GetUpTime() << " |P-ERROR: too many errors ( " << errors << " ) occured terminate data processor" << endl;
#endif
					self->StopProcessing();
					sender->StopSending();
				}

			}
		}
	}
	while(vtkThreadSleep(data, vtkTimerLog::GetUniversalTime() + processPeriod, false));

	return NULL;
}

/******************************************************************************
*  int vtkCalibDataProcessor::StartProcessing(vtkCalibDataSender * sender)
*
*  Starts process thread
*
*  @Author:Jan Gumprecht
*  @Date:  27.Januar 2009
*
*  @Param: vtkCalibDataSender * sender: DataSender to which proccessed data is
*                                  forwarded
*
*  @Return: 0 on success, 1 on warning, -1 on error
*
* ****************************************************************************/
int vtkCalibDataProcessor::StartProcessing(vtkCalibDataSender * sender)
{

	if(this->Processing)
	{
#ifdef DEBUGPROCESSOR
		this->LogStream << this->GetUpTime()  << " |P-WARNING: Data Processor already processes data" << endl;
#endif
		return 1;
	}

	if(sender != NULL)
	{
		this->DataSender = sender;
	}
	else
	{
#ifdef ERRORPROCESSOR
		this->LogStream << this->GetUpTime()  << " |P-ERROR: No data sender provided. Data processing not possible" << endl;
#endif
		return -1;
	}

	if (!this->USDataCollector)
	{
#ifdef DEBUGPROCESSOR
		this->LogStream << this->GetUpTime()  << " |P-ERROR: No USDataCollector provided.  Data processing not possible" << endl;
#endif
		return -1;
	}

	// start real-time reconstruction
	//cout << "Starting real time reconstruction" << endl;
	//this->USDataCollector->StartRealTimeReconstruction();

	//Start process thread
	this->PlayerThreadId =
		this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
		&vtkDataProcessorThread, this);

	//Check if thread successfully started
	if(this->PlayerThreadId != -1)
	{
		this->Processing = true;
		if(Verbose)
		{
			cout << "Start processing data" << endl;
		}
		return 0;
	}
	else
	{
#ifdef ERRORPROCESSOR
		this->LogStream << this->GetUpTime()  << " |P-ERROR: Could not start process thread" << endl;
#endif
		return -1;
	}
}

/******************************************************************************
*  int vtkCalibDataProcessor::StopProcessing()
*
*  Stops the process thread if running
*
*  @Author:Jan Gumprecht
*  @Date:  27.Januar 2009
*
*  @Return: Return 0 on sucess, 1 for warning
*
* ****************************************************************************/
int vtkCalibDataProcessor::StopProcessing()
{
	if(this->Processing)
	{
		//if (this->USDataCollector)
		//  {
		//  this->USDataCollector->StopRealTimeReconstruction();
		//  }

		//Stop thread
		this->PlayerThreader->TerminateThread(this->PlayerThreadId);
		this->PlayerThreadId = -1;
		this->Processing = false;

		if(this->Verbose)
		{
			cout << "Stopped processing" <<endl;
		}
	}
	else
	{
		return 1;
	}

#ifdef  DEBUGPROCESSOR
	this->LogStream << this->GetUpTime()  << " |P-INFO: Data processor stopped processing" << endl;
#endif

	return 0;
}

/******************************************************************************
*  int vtkCalibDataProcessor::ForwardData()
*
*  Prepare and forward reconstructed volume to DataSender
*
*  @Author:Jan Gumprecht
*  @Date:  31.January 2009
*
*  @Return: Success: Index of DataSender's newDataBuffer were forwarded volume
*                    is stored
*           Failure: -1
*
* ****************************************************************************/
int vtkCalibDataProcessor::ForwardData(vtkImageData * image, int port)
{

	vtkImageData * reconstructedVolume = image;

	//Generate and fill volume which will be forwarded----------------------------
	vtkImageData* volumeToForward = vtkImageData::New();

#ifdef DEBUGPROCESSOR
	double copyTime = this->GetUpTime();
	int counter = 0;
#endif

	//Copy volume so that it can be put into the sender buffer
	volumeToForward->DeepCopy(reconstructedVolume);

	//Create and Fill OpenIGTLink Matrix for forwarding---------------------------
	//Need to adjust matrix to OpenIGTLink offset
	vtkMatrix4x4 * matrix = vtkMatrix4x4::New();
	matrix->Identity();

	double xOrigin = volumeToForward->GetOrigin()[0];
	double yOrigin = volumeToForward->GetOrigin()[1];
	double zOrigin = volumeToForward->GetOrigin()[2];

	double xLength = ((volumeToForward->GetDimensions()[0] - 1) * volumeToForward->GetSpacing()[0]) / 2.0;
	double yLength = ((volumeToForward->GetDimensions()[1] - 1) * volumeToForward->GetSpacing()[1]) / 2.0;
	double zLength = ((volumeToForward->GetDimensions()[2] - 1) * volumeToForward->GetSpacing()[2]) / 2.0;

	matrix->Element[0][3] = xLength + xOrigin;
	matrix->Element[1][3] = yLength + yOrigin;
	matrix->Element[2][3] = zLength + zOrigin;

	//Forward data to sender------------------------------------------------------

	// calculate phase from port
	int phase = -1;
	if (this->USDataCollector->GetTriggering())
	{
		phase = this->USDataCollector->CalculatePhaseFromPort(port);
	}
	int retval = this->DataSender->NewData(volumeToForward, matrix, phase);

	if(retval != -1)
	{
#ifdef DEBUGPROCESSOR
		this->LogStream << this->GetUpTime()  << " |P-INFO: Volume forwarded to data sender " << " | Copytime: " << this->GetUpTime() -  copyTime << endl
			<< "         | Pixels: "<< counter << " | Data Sender Index: " << retval << endl
			<< "         | Volume Dimensions: "<< volumeToForward->GetDimensions()[0] << " | "<< volumeToForward->GetDimensions()[1] << " | "<< volumeToForward->GetDimensions()[2] << " | " << endl
			<< "         | SenderIndex: "<< retval << " | "  << endl;
#endif
	}
	else
	{
#ifdef ERRORPROCESSOR
		this->LogStream << this->GetUpTime()  << " |P-ERROR: Could not forward volume to data sender" << " | " << endl;
#endif
	}

	return retval;
}

/******************************************************************************
* double vtkCalibDataProcessor::GetUpTime()
*
*  Returns elapsed Time since program start
*
*  @Author:Jan Gumprecht
*  @Date:  2.February 2009
*
* ****************************************************************************/
double vtkCalibDataProcessor::GetUpTime()
{
	return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
* void vtkCalibDataProcessor::SetLogStream(ofstream &LogStream)
*
*  Redirects Logstream
*
*  @Author:Jan Gumprecht
*  @Date:  4.February 2009
*
* ****************************************************************************/
void vtkCalibDataProcessor::SetLogStream(ofstream &LogStream)
{
	this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
	this->LogStream.precision(6);
	this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
* ofstream& vtkCalibDataProcessor::GetLogStream()
*
*  Returns logstream
*
*  @Author:Jan Gumprecht
*  @Date:  4.February 2009
*
*  @Return: Logstream
*
* ****************************************************************************/
ofstream& vtkCalibDataProcessor::GetLogStream()
{
	return this->LogStream;
}

/******************************************************************************
* void vtkCalibDataProcessor::ResetOldVolume()
*
*  Resets the last reconstructed volume
*
*  @Author:Jan Gumprecht
*  @Date:  4.February 2009
*
* ****************************************************************************/
void vtkCalibDataProcessor::ResetOldVolume(int dataSenderIndex)
{
	this->DataSender->UnlockData(dataSenderIndex, DATAPROCESSOR);
	this->DataSender->TryToDeleteData(dataSenderIndex);
}
