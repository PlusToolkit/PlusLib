/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualDiscCapture_h
#define __vtkVirtualDiscCapture_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include <string>

class vtkMetaImageSequenceIO;
class vtkTrackedFrameList;

/*!
\class vtkVirtualDiscCapture 
\brief 

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkVirtualDiscCapture : public vtkPlusDevice
{
public:
  static vtkVirtualDiscCapture *New();
  vtkTypeMacro(vtkVirtualDiscCapture, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus InternalUpdate();

  virtual PlusStatus NotifyConfigured();

  virtual bool HasUnsavedData() const;

  /*! Open the output file for writing */
  virtual PlusStatus OpenFile(const char* aFilename = NULL);

  /*! Close the output file */
  virtual PlusStatus CloseFile(const char* aFilename = NULL);

  virtual PlusStatus Reset();

  virtual PlusStatus TakeSnapshot();

  virtual int OutputChannelCount() const;

  /*! Enables capturing frames. It can be used for pausing the recording. */
  vtkGetMacro(EnableCapturing, bool);
  void SetEnableCapturing(bool aValue);

  /*!
    Method that writes output streams to XML
  */
  virtual void InternalWriteOutputChannels(vtkXMLDataElement* rootXMLElement);

  vtkSetMacro(RequestedFrameRate, double);
  vtkGetMacro(RequestedFrameRate, double);

  vtkGetMacro(ActualFrameRate, double);
  vtkGetMacro(TotalFramesRecorded, long int);

  vtkGetMacro(BaseFilename, std::string);
  vtkSetMacro(BaseFilename, std::string);

  vtkGetMacro(EnableFileCompression, bool);
  vtkSetMacro(EnableFileCompression, bool);

  virtual vtkDataCollector* GetDataCollector() { return this->DataCollector; }

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

  virtual std::string GetOutputFileName() { return vtkPlusConfig::GetInstance()->GetOutputPath(CurrentFilename); };

protected:
  virtual PlusStatus ClearRecordedFrames();

  vtkSetMacro(ActualFrameRate, double);
  vtkSetMacro(FrameBufferSize, int);
  vtkGetMacro(FrameBufferSize, int);

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  virtual bool IsFrameBuffered() const;

  /*!
    Copy frames to memory buffer or disk.
    If force flag is true then data is written to disk immediately.
  */
  virtual PlusStatus WriteFrames(bool force = false);

  /*! Read the sequence metafile, re-write it with compression */
  PlusStatus CompressFile();

  vtkVirtualDiscCapture();
  virtual ~vtkVirtualDiscCapture();

  /*! Recorded tracked frame list */
  vtkTrackedFrameList* RecordedFrames;

  /*! Timestamp of last recorded frame (only frames that have more recent timestamp will be added) */
  double LastAlreadyRecordedFrameTimestamp;

  /*! Desired timestamp of the next frame to be recorded */
  double NextFrameToBeRecordedTimestamp;

  /*!
    Requested frame rate (frames per second)
    If the input data source provides data at a higher rate then frames will be skipped.
    If the input data has lower frame rate then requested then all the frames in the input data will be recorded.
  */
  double RequestedFrameRate;

  /*! Actual frame rate (frames per second) */
  double ActualFrameRate;
  
  /*!
    Frame index of the first frame that is recorded in this segment (since pressed the record button).
    It is used when estimating the actual frame rate: frames that are acquired before this frame index (i.e.,
    those that were acquired in a different recording segment) will not be taken into account in the actual
    frame rate computation.
  */
  int FirstFrameIndexInThisSegment;

  /* Time waited in update */
  double TimeWaited;
  double LastUpdateTime;

  /*! File to write */
  std::string CurrentFilename;
  std::string BaseFilename;

  /*! Meta sequence to write to */
  vtkMetaImageSequenceIO* Writer;

  /*! When closing the file, re-read the data from file, and write it compressed */
  bool EnableFileCompression;

  /*! Preparing the header requires image data already collected, this flag makes the header preparation wait until valid data is collected */
  bool IsHeaderPrepared;

  /*! Record the number of frames captured */
  long int TotalFramesRecorded;  // hard drive will probably fill up before a regular int is hit, but still...

  bool EnableCapturing;

  int FrameBufferSize;

  /*! Mutex instance simultaneous access of writer (writer may be accessed from command processing thread and also the internal update thread) */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> WriterAccessMutex;

  vtkPlusLogger::LogLevelType GracePeriodLogLevel;

  PlusStatus GetInputTrackedFrame(TrackedFrame* aFrame);
  PlusStatus GetInputTrackedFrameListSampled(double &lastAlreadyRecordedFrameTimestamp, double &nextFrameToBeRecordedTimestamp, vtkTrackedFrameList* recordedFrames, double requestedFramePeriodSec, double maxProcessingTimeSec);
  PlusStatus GetLatestInputItemTimestamp(double &timestamp);

private:
  vtkVirtualDiscCapture(const vtkVirtualDiscCapture&);  // Not implemented.
  void operator=(const vtkVirtualDiscCapture&);  // Not implemented. 
};

#endif
