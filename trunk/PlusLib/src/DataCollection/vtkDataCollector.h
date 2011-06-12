// .NAME vtkDataCollector
// .SECTION Description

#ifndef __vtkDataCollector_h
#define __vtkDataCollector_h

#include "vtkMultiThreader.h"
#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h" 
#include "vtkXMLDataElement.h"
#include "vtkImageData.h"
#include "vtkTimerLog.h"
#include "vtksys/SystemTools.hxx"

#include "vtkVideoSource2.h"
#include "vtkVideoBuffer.h"
#include "vtkTracker.h"
#include "vtkTrackerBuffer.h"
#include "vtkTrackedFrameList.h"
#include "vtkDataCollectorSynchronizer.h"

// Acquisition types
enum ACQUISITION_TYPE 
{
	SYNCHRO_VIDEO_NONE=0, 
	SYNCHRO_VIDEO_SAVEDDATASET,
	SYNCHRO_VIDEO_NOISE,
	SYNCHRO_VIDEO_MIL,
	SYNCHRO_VIDEO_WIN32,
	SYNCHRO_VIDEO_LINUX,
	SYNCHRO_VIDEO_SONIX,
	SYNCHRO_VIDEO_ICCAPTURING
}; 

// Tracker types
enum TRACKER_TYPE
{
	TRACKER_NONE=0, 
	TRACKER_SAVEDDATASET,
	TRACKER_AMS, 
	TRACKER_CERTUS, 
	TRACKER_POLARIS, 
	TRACKER_AURORA, 
	TRACKER_FLOCK, 
	TRACKER_MICRON,
	TRACKER_FAKE,
	TRACKER_ASCENSION3DG
}; 

// Synchronization types
enum SYNC_TYPE
{
	SYNC_NONE=0,
	SYNC_CHANGE_DETECTION
};

class VTK_EXPORT vtkDataCollector: public vtkImageAlgorithm
{
public:
	static vtkDataCollector *New();
	vtkTypeRevisionMacro(vtkDataCollector,vtkImageAlgorithm);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Initialize the data collector and connect to devices 
	virtual PlusStatus Initialize(); 

	// Description:
	// Read the configuration file in XML format and set up the devices
	virtual PlusStatus ReadConfiguration( const char* configFileName); 
	virtual PlusStatus ReadConfiguration(); 

	// Description:
	// Disconnect from devices
	virtual PlusStatus Disconnect(); 
	
	// Description:
	// Connect to devices
	virtual PlusStatus Connect(); 

	// Description:
	// Stop data collection
	virtual PlusStatus Stop(); 

	// Description:
	// Start data collection 
	virtual PlusStatus Start(); 

	// Description:
	// Synchronize the connected devices
	virtual PlusStatus Synchronize(const bool saveSyncData = false); 

	// Description:
	// Copy the current state of the tracker buffer 
	virtual PlusStatus CopyTrackerBuffer( vtkTrackerBuffer* trackerBuffer, int toolNumber ); 

	// Description:
	// Copy the current state of the tracker (with each tools and buffers)
	virtual PlusStatus CopyTracker( vtkTracker* tracker); 

	// Description:
	// Dump the current state of the tracker to metafile (with each tools and buffers)
	virtual PlusStatus DumpTrackerToMetafile( vtkTracker* tracker, const char* outputFolder, const char* metaFileName, bool useCompression = false ); 
	
	// Description:
	// Copy the current state of the video buffer 
	virtual PlusStatus CopyVideoBuffer( vtkVideoBuffer* videoBuffer ); 

	// Description:
	// Dump the current state of the video buffer to metafile
	virtual PlusStatus DumpVideoBufferToMetafile( vtkVideoBuffer* videoBuffer, const char* outputFolder, const char* metaFileName, bool useCompression = false ); 

	// Description:
	// Return the most recent frame timestamp in the buffer
	virtual PlusStatus GetMostRecentTimestamp(double &ts); 

	// Description:
	// Return the main tool status at a given time
	virtual long GetMainToolStatus( double time ); 

	// Description:
	// Return the main tool name
	virtual std::string GetMainToolName(); 

	// Description:
	// Get the tracked frame from devices 
	virtual PlusStatus GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, long& flags, double& synchronizedTime, int toolNumber = 0, bool calibratedTransform = false); 

	// Description:
	// Get the tracked frame from devices with each tool transforms
	virtual PlusStatus GetTrackedFrame(vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<long> &flags, double& synchronizedTime, bool calibratedTransform = false); 
	
	// Description:
	// Get the tracked frame from devices with each tool transforms
	virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform = false); 

	// Description:
	// Get the tracked frame from devices by time with each tool transforms
	virtual PlusStatus GetTrackedFrameByTime(const double time, TrackedFrame* trackedFrame, bool calibratedTransform = false); 

	// Description:
	// Get the tracked frame from devices by time with each tool transforms
	virtual PlusStatus GetTrackedFrameByTime(const double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<long> &flags, double& synchronizedTime, bool calibratedTransform = false); 

	// Description:
	// Get transformation with timestamp from tracker 
	virtual PlusStatus GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, long& flags, int toolNumber = 0, bool calibratedTransform = false); 

	// Description:
	// Get transformation by timestamp from tracker 
	virtual PlusStatus GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, long& flags, const double synchronizedTime, int toolNumber = 0, bool calibratedTransform = false); 

	// Description:
	// Get transformations by timestamp range from tracker. The first returned transform is the one after the startTime, except if startTime is -1, then it refers to the oldest one. '-1' for end time means the latest transform. Returns the timestamp of the requested transform (makes sense if endTime is -1)
	virtual double GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<long> &flagsVector, const double startTime, const double endTime, int toolNumber = 0, bool calibratedTransform = false);

	// Description:
	// Get frame data with timestamp 
	virtual PlusStatus GetFrameWithTimestamp(vtkImageData* frame, double& frameTimestamp); 

	// Description:
	// Get frame data by time 
	virtual PlusStatus GetFrameByTime(const double time, vtkImageData* frame, double& frameTimestamp); 

	// Description:
	// Find the next active tracker tool number 
	virtual int GetNextActiveToolNumber(); 
	
	// Description:
	// Find the previous active tracker tool number 
	virtual int GetPreviousActiveToolNumber(); 

	// Description:
	// Set/Get the acquisition type 
	void SetAcquisitionType(ACQUISITION_TYPE type) { AcquisitionType = type; }
	ACQUISITION_TYPE GetAcquisitionType() { return this->AcquisitionType; }

	// Description:
	// Set/Get the tracker type 
	void SetTrackerType(TRACKER_TYPE type) { TrackerType = type; }
	TRACKER_TYPE GetTrackerType() { return this->TrackerType; }

	// Description:
	// Set/Get the synchronization type 
	void SetSyncType(SYNC_TYPE type) { SyncType = type; }
	SYNC_TYPE GetSyncType() { return this->SyncType; }

	// Description:
	// Get the tool transformation matrix
	virtual vtkMatrix4x4* GetToolTransMatrix( unsigned int toolNumber = 0 ) ;
	
	// Description:
	// Get the tool flags (for more details see vtkTracker.h (e.g. TR_MISSING) 
	virtual int GetToolFlags( unsigned int toolNumber/* = 0 */); 

	// Description:
	// Set video and tracker local time offset 
	virtual void SetLocalTimeOffset(double videoOffset, double trackerOffset); 

	// Description:
	// Set/Get the configuration file name
	vtkSetStringMacro(ConfigFileName); 
	vtkGetStringMacro(ConfigFileName); 

	// Description:
	// Set/Get the device set name
	vtkSetStringMacro(DeviceSetName); 
	vtkGetStringMacro(DeviceSetName); 
	
	// Description:
	// Set/Get the device set description 
	vtkSetStringMacro(DeviceSetDescription); 
	vtkGetStringMacro(DeviceSetDescription); 
	
	// Description:
	// Set/Get the video source of ultrasound
	vtkSetObjectMacro(VideoSource,vtkVideoSource2);
	vtkGetObjectMacro(VideoSource,vtkVideoSource2);

	// Description:
	// Set/Get the video source of ultrasound
	vtkSetObjectMacro(Synchronizer,vtkDataCollectorSynchronizer);
	vtkGetObjectMacro(Synchronizer,vtkDataCollectorSynchronizer);

	// Description:
	// Set/Get the tracker 
	vtkSetObjectMacro(Tracker,vtkTracker);
	vtkGetObjectMacro(Tracker,vtkTracker);

	// Description:	
	// Get default tool port number 
	int GetDefaultToolPortNumber(); 

	// Description:	
	// Get/Set the maximum buffer size to dump
	vtkSetMacro(DumpBufferSize, int); 
	vtkGetMacro(DumpBufferSize, int);
	
	// Description:
	// Set/get the Initialized flag 
	vtkSetMacro(Initialized,bool);
	vtkGetMacro(Initialized,bool);
	vtkBooleanMacro(Initialized, bool); 

	// Description:
	// Set/get the Tracking only flag
	vtkGetMacro(TrackingOnly,bool);
	void SetTrackingOnly(bool);

	// Description:
	// Set/get the Video only flag
	vtkGetMacro(VideoOnly,bool);
	void SetVideoOnly(bool);

	// Description:
	// Set/get the cancel sync request flag to cancel the active sync job 
	vtkSetMacro(CancelSyncRequest, bool); 
	vtkGetMacro(CancelSyncRequest, bool); 
	vtkBooleanMacro(CancelSyncRequest, bool); 
	
	int GetNumberOfTools();

	//! Description 
	// Callback function for progress bar refreshing
	typedef void (*ProgressBarUpdatePtr)(int percent);
    void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) { ProgressBarUpdateCallbackFunction = cb; } 
	
protected:
	vtkDataCollector();
	virtual ~vtkDataCollector();

	// This is called by the superclass.
	virtual int RequestData(vtkInformation *request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

	// Description:
	// Read image acqusition properties from xml file 
	virtual PlusStatus ReadImageAcqusitionProperties(vtkXMLDataElement* imageAcqusitionConfig); 

	// Description:
	// Read tracker properties from xml file 
	virtual PlusStatus ReadTrackerProperties(vtkXMLDataElement* trackerConfig); 

	// Description:
	// Read synchronization properties from xml file 
	virtual PlusStatus ReadSynchronizationProperties(vtkXMLDataElement* synchronizationConfig); 

	// Description:
	// Convert vtkImageData to itkImage (TrackedFrame::ImageType)
	virtual void ConvertVtkImageToItkImage(vtkImageData* inFrame, TrackedFrame::ImageType* outFrame); 

	//! Pointer to the progress bar update callback function 
	ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 

	vtkDataCollectorSynchronizer* Synchronizer; 
	
	vtkVideoSource2*	VideoSource; 
	vtkTracker*			Tracker; 

	ACQUISITION_TYPE	AcquisitionType; 
	TRACKER_TYPE		TrackerType; 
	SYNC_TYPE			SyncType; 

	std::vector<vtkMatrix4x4*> ToolTransMatrices; 
	std::vector<int>	ToolFlags; 

	vtkXMLDataElement*	ConfigurationData;
	char*				ConfigFileName; 
	double				ConfigFileVersion; 

	bool Initialized; 
	
	int MostRecentFrameBufferIndex; 
	
	int DumpBufferSize;

	bool TrackingOnly;
	bool VideoOnly;

	bool CancelSyncRequest; 

	char * DeviceSetName; 
	char * DeviceSetDescription; 

	VideoBufferItem* CurrentVideoBufferItem; 

private:
	vtkDataCollector(const vtkDataCollector&);
	void operator=(const vtkDataCollector&);

};

#endif
