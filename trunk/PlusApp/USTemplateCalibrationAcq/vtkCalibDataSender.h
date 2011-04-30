// .NAME vtkCalibDataSender - send image to the UI
// .SECTION Description
// vtkCalibDataSender is responsible for the creation and configuration
// of connection with the UI, and as requested, sending images.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkDataSender_h
#define __vtkDataSender_h

#include <vtkstd/queue>
#include <vtkstd/map>

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkImageData.h"
//#include "igtlImageMessage.h"
//#include "igtlClientSocket.h"
#include "vtkImageViewer.h"
#include "vtkTextActor.h"

// includes for sleep
#ifdef _WIN32
#include "vtkWindows.h"
#else
#include <time.h>
#endif


class vtkMultiThreader;
class vtkMatrix4x4;
class vtkMutexLock;
//class igtlImageMessage;
//class igtlOSUtil;

struct FrameProperties {
	bool Set;
	int ScalarType;
	int Size[3];
	double Spacing[3];
	int SubVolumeSize[3];
	int SubVolumeOffset[3];
	float Origin[3];
};

struct SenderDataStruct{
	vtkImageData* ImageData;
	vtkMatrix4x4* Matrix;
	int Phase;
	int SenderLock;
	int ProcessorLock;
};

struct StatisticsStruct{
	int fpsCounter;
	double meanFPS;
	double maxFPS;
	double minFPS;
	int volumeCounter;
};

class vtkCalibDataSender : public vtkObject
{
public:
	static vtkCalibDataSender *New();
	vtkTypeRevisionMacro(vtkCalibDataSender, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Time between sending subsequent image messages
	vtkSetMacro(SendPeriod, double);
	vtkGetMacro(SendPeriod, double);

	vtkImageViewer* GetImageViewer() {return ImageViewer; };
	
	// Description:
	// Size of the buffer storing images to be sent over OpenIGTLink connection
	vtkGetMacro(SendDataBufferSize, int);

	// Description:
	// Have we successfully connected to the server?
	vtkGetMacro(Connected, bool);

	// Description:
	// Have we started sending image data?
	vtkGetMacro(Sending, bool);

	// Description:
	// Timestamp when we started the data sender
	vtkSetMacro(StartUpTime, double);
	vtkGetMacro(StartUpTime, double);

	// Description:
	// Get the number of seconds we've been operating for
	double GetUpTime();

	// Description:
	// File used for logging
	void SetLogStream(ofstream &LogStream);
	ofstream& GetLogStream();

	// Description:
	// For verbose output
	vtkSetMacro(Verbose, bool);
	vtkGetMacro(Verbose, bool);

	// Description:
	// Open connection to the server
	int ConnectToImageViewer();

	// Description:
	// Start sending data to the server
	int StartSending();

	// Description:
	// Stop sending data to the server
	int StopSending();

	// Description:
	// Close the connection to the server
	int CloseViewerConnection();

	// Description:
	// Current number of items within the buffer
	int GetCurrentBufferSize(){return SendDataQueue.size();}

	// Description:
	// Get the index at the front of the queue
	int GetHeadOfNewDataBuffer();

	// Description:
	// Is the buffer empty?
	bool IsSendDataBufferEmpty();

	// Description:
	// Is the buffer full?
	bool IsSendDataBufferFull();

	// Description:
	// Get the number of images that we've sent since we started
	vtkGetMacro(NumberSent, int);

	// Description:
	// Add new data to the buffer
	int NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix, int phase);

	//----------------------------
	// The following need to be public for the data sender thread
	// Please don't call them directly!

	// Description:
	// Sets up the OpenIGTLink message for the data stored at the specified index
	// in the buffer
	int PrepareImageMessage(int index, SenderDataStruct& imageMessage);

	// Description:
	// Actually sends the OpenIGTLink message
	int SendMessage(SenderDataStruct& message);

	// Description:
	// Removes the data from the buffer at the specified index, if it is not locked
	int TryToDeleteData(int index);

	// Description:
	// Removes all of the data from the buffer
	void DeleteAllData();

	// Description:
	// Lock/Unlock for synchronization
	int LockIndex(int index, int requester);
	int UnlockData(int index, int lock);
	int ReleaseLock(int requester);

protected:
	vtkCalibDataSender();
	~vtkCalibDataSender();

	vtkImageViewer *ImageViewer; 
	vtkTextActor *TextActor; 

	// rate for data transfer
	double SendPeriod;

	// buffer containing images to be sent
	std::queue<int> SendDataQueue; // Stores index of incoming objects
	int SendDataBufferSize; // Maximum number of items that can be stored at the same time
	int SendDataBufferIndex; // Object which is currently/ was last processed
	std::map<int, struct SenderDataStruct> SendDatabuffer;

	// current status
	bool Sending;
	bool Connected;
	double StartUpTime;
	double LastFrameRateUpdate;
	int UpdateCounter;
	int NumberSent;
	int SaveInsertedTimestampsCounter; 

	//Multithreader to run a thread of collecting and sending data
	vtkMultiThreader *PlayerThreader;
	int PlayerThreadId;
	vtkMutexLock *IndexLock;
	int IndexLockedByDataSender;
	int IndexLockedByDataProcessor;

	// structs
	struct FrameProperties FrameProperties;
	struct StatisticsStruct Statistics;

	// logging
	bool Verbose;
	ofstream LogStream;

	// Description:
	// Add the image data to the buffer at the specified index
	int AddDatatoBuffer(int index, vtkImageData* imageData, vtkMatrix4x4* matrix, int phase);

	// Description:
	// Check to see if the buffer already has data at the specified index
	bool IsIndexAvailable(int index);

	// Description:
	// Calculates and prints current frame rate to console
	void UpdateFrameRate(double sendTime);

	// Description:
	// Increment current index of the buffer
	int IncrementBufferIndex(int increment);

private:
	vtkCalibDataSender(const vtkCalibDataSender&);  // Not implemented.
	void operator=(const vtkCalibDataSender&);  // Not implemented.
};
#endif
