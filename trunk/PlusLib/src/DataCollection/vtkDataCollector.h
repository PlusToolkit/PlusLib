// .NAME vtkDataCollector
// .SECTION Description

#ifndef __vtkDataCollector_h
#define __vtkDataCollector_h

#include "PlusConfigure.h"
#include "vtkMultiThreader.h"
#include "vtkImageAlgorithm.h" 
#include "vtkXMLDataElement.h"
#include "vtkImageData.h"
#include "vtkTimerLog.h"
#include "vtksys/SystemTools.hxx"

#include "vtkVideoBuffer.h"
#include "vtkTracker.h"
#include "vtkTrackerBuffer.h"
#include "vtkTrackedFrameList.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkPlusVideoSource.h"

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
  TRACKER_BRACHY, 
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
  virtual PlusStatus ReadConfigurationFromFile( const char* configFileName); 
  virtual PlusStatus ReadConfigurationFromFile(); 
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig ); 

  // Description:
  // Save unified configuration to xml file 
  virtual PlusStatus SaveConfigurationToFile(const char* aFile); 

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
  virtual PlusStatus Synchronize( const char* bufferOutputFolder = NULL, bool acquireDataOnly = false ); 

  // Description:
  // Copy the current state of the tracker buffer 
  virtual PlusStatus CopyTrackerBuffer( vtkTrackerBuffer* trackerBuffer, int toolNumber ); 

  // Description:
  // Copy the current state of the tracker (with each tools and buffers)
  virtual PlusStatus CopyTracker( vtkTracker* tracker); 

  // Description:
  // Dump the current state of the tracker to metafile (with each tools and buffers)
  static PlusStatus WriteTrackerToMetafile( vtkTracker* tracker, const char* outputFolder, const char* metaFileName, bool useCompression = false ); 

  // Description:
  // Copy the current state of the video buffer 
  virtual PlusStatus CopyVideoBuffer( vtkVideoBuffer* videoBuffer ); 

  // Description:
  // Dump the current state of the video buffer to metafile
  static PlusStatus WriteVideoBufferToMetafile( vtkVideoBuffer* videoBuffer, const char* outputFolder, const char* metaFileName, bool useCompression = false ); 

  // Description:
  // Return the most recent frame timestamp in the buffer
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  // Description:
  // Return the tool status at a given time
  virtual PlusStatus GetToolStatus( double time, int toolNumber, TrackerStatus &status ); 

  // Description:
  // Get the most recent tracked frame from devices 
  virtual PlusStatus GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, TrackerStatus& status, double& synchronizedTime, int toolNumber = 0, bool calibratedTransform = false); 

  // Description:
  // Get the most recent tracked frame from devices with each tool transforms
  virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform = false); 

  // Description:
  // Get the tracked frame from devices by time with each tool transforms
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame, bool calibratedTransform = false); 

  // Description:
  // Get the tracked frame from devices by time with each tool transforms
  virtual PlusStatus GetTrackedFrameByTime(double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<TrackerStatus> &status, double& synchronizedTime, bool calibratedTransform = false); 

  // Description:
  // Get transformation with timestamp from tracker 
  virtual PlusStatus GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, int toolNumber = 0, bool calibratedTransform = false); 

  // Description:
  // Get transformation by timestamp from tracker 
  virtual PlusStatus GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, TrackerStatus& status, double synchronizedTime, int toolNumber = 0, bool calibratedTransform = false); 

  // Description:
  // Get transformations by timestamp range from tracker. The first returned transform is the one after the startTime, except if startTime is -1, then it refers to the oldest one. '-1' for end time means the latest transform. Returns the timestamp of the requested transform (makes sense if endTime is -1)
  virtual double GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<TrackerStatus> &statusVector, double startTime, double endTime, int toolNumber = 0, bool calibratedTransform = false);

  // Description:
  // Get frame data by time 
  virtual PlusStatus GetFrameByTime(double time, vtkImageData* frame, double& frameTimestamp); 
  virtual PlusStatus GetFrameByTime(double time, PlusVideoFrame& frame, double& frameTimestamp); 

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
  vtkSetObjectMacro(VideoSource,vtkPlusVideoSource);
  vtkGetObjectMacro(VideoSource,vtkPlusVideoSource);

  // Description:
  // Set/Get the video source of ultrasound
  vtkSetObjectMacro(Synchronizer,vtkDataCollectorSynchronizer);
  vtkGetObjectMacro(Synchronizer,vtkDataCollectorSynchronizer);

  // Description:
  // Set/Get the tracker 
  vtkSetObjectMacro(Tracker,vtkTracker);
  vtkGetObjectMacro(Tracker,vtkTracker);

  // Description:
  // Set/get the Initialized flag 
  vtkSetMacro(Initialized,bool);
  vtkGetMacro(Initialized,bool);
  vtkBooleanMacro(Initialized, bool); 

  // Description:
  // Set/get the Connected flag 
  vtkSetMacro(Connected,bool);
  vtkGetMacro(Connected,bool);
  vtkBooleanMacro(Connected, bool); 

  // Description:
  // Set/get the Tracking only flag
  vtkGetMacro(TrackingEnabled,bool);
  void SetTrackingOnly(bool);

  // Description:
  // Set/get the Video only flag
  vtkGetMacro(VideoEnabled,bool);
  void SetVideoOnly(bool);

  // Description:
  // Set/get the cancel sync request flag to cancel the active sync job 
  vtkSetMacro(CancelSyncRequest, bool); 
  vtkGetMacro(CancelSyncRequest, bool); 
  vtkBooleanMacro(CancelSyncRequest, bool); 

  // Description:
  // Set/get startup delay in sec to give some time to the buffers for proper initialization
  vtkSetMacro(StartupDelaySec, double); 
  vtkGetMacro(StartupDelaySec, double);

  // Description:
  // Get configuration data
  vtkGetObjectMacro(ConfigurationData, vtkXMLDataElement); 
  vtkSetObjectMacro(ConfigurationData, vtkXMLDataElement); 

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
  // Read image acquisition properties from xml file 
  virtual PlusStatus ReadImageAcquisitionProperties(vtkXMLDataElement* imageAcquisitionConfig); 

  // Description:
  // Read tracker properties from xml file 
  virtual PlusStatus ReadTrackerProperties(vtkXMLDataElement* trackerConfig); 

  // Description:
  // Read synchronization properties from xml file 
  virtual PlusStatus ReadSynchronizationProperties(vtkXMLDataElement* synchronizationConfig); 

  // Description: 
  // Compute loop times for saved datasets (time intersection of the two buffers)
  // itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
  virtual PlusStatus SetLoopTimes(); 

  //! Pointer to the progress bar update callback function 
  ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 

protected:
  vtkDataCollectorSynchronizer* Synchronizer; 

  vtkPlusVideoSource*	            VideoSource; 
  vtkTracker*		              	Tracker; 

  ACQUISITION_TYPE	            AcquisitionType; 
  TRACKER_TYPE		              TrackerType; 
  SYNC_TYPE			                SyncType; 

  std::vector<vtkMatrix4x4*>    ToolTransMatrices; 
  std::vector<TrackerStatus>	  ToolStatus; 

  vtkXMLDataElement*        	  ConfigurationData;
  char*				                  ConfigFileName; 
  double			              	  ConfigFileVersion; 

  double                        StartupDelaySec; 

  bool                          Initialized; 
  bool                          Connected; 

  bool                          TrackingEnabled;
  bool                          VideoEnabled;

  bool                          CancelSyncRequest; 

  char*                         DeviceSetName; 
  char*                         DeviceSetDescription; 

private:
  vtkDataCollector(const vtkDataCollector&);
  void operator=(const vtkDataCollector&);

};

#endif
