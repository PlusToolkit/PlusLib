/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualDiscCapture_h
#define __vtkVirtualDiscCapture_h

#include "vtkMetaImageSequenceIO.h"
#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"
#include <string>

/*!
\class vtkVirtualDiscCapture 
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualDiscCapture : public vtkPlusDevice
{
public:
  static vtkVirtualDiscCapture *New();
  vtkTypeRevisionMacro(vtkVirtualDiscCapture, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus InternalUpdate();

  virtual PlusStatus NotifyConfigured();

  virtual bool HasUnsavedData() const;

  virtual double GetAcquisitionRate() const;

  /*! Open the output file for writing */
  virtual PlusStatus OpenFile();

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

  void SetRequestedFrameRate(double aValue);
  vtkGetMacro(RequestedFrameRate, double);

  vtkGetMacro(ActualFrameRate, double);
  vtkGetMacro(TotalFramesRecorded, long int);

  virtual vtkDataCollector* GetDataCollector() { return this->DataCollector; }

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

protected:
  virtual PlusStatus ClearRecordedFrames();

  vtkSetMacro(ActualFrameRate, double);
  vtkSetMacro(FrameBufferSize, int);
  vtkGetMacro(FrameBufferSize, int);

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  virtual bool IsFrameBuffered() const;

  virtual PlusStatus WriteFrames(bool force = false);

  /*!
  * Get the maximum frame rate from the video source. If there is none then the tracker
  * \return Maximum frame rate
  */
  double GetMaximumFrameRate();

  /*! Get the sampling period length (in seconds). Frames are copied from the devices to the data collection buffer once in every sampling period. */
  double GetSamplingPeriodSec();

  /*! Read the sequence metafile, re-write it with compression */
  PlusStatus CompressFile();

  vtkVirtualDiscCapture();
  virtual ~vtkVirtualDiscCapture();

  /*! Recorded tracked frame list */
  vtkTrackedFrameList* m_RecordedFrames;

  /*! Timestamp of last recorded frame (only frames that have more recent timestamp will be added) */
  double m_LastAlreadyRecordedFrameTimestamp;

  /*! Desired timestamp of the next frame to be recorded */
  double m_NextFrameToBeRecordedTimestamp;

  /*! Frame rate of the sampling */
  const int m_SamplingFrameRate;

  /*! Requested frame rate (frames per second) */
  double RequestedFrameRate;

  /*! Actual frame rate (frames per second) */
  double ActualFrameRate;
  
  /*!
    Frame index of the first frame that is recorded in this segment (since pressed the record button).
    It is used when estimating the actual frame rate: frames that are acquired before this frame index (i.e.,
    those that were acquired in a different recording segment) will not be taken into account in the actual
    frame rate computation.
  */
  int m_FirstFrameIndexInThisSegment;

  /* Time waited in update */
  double m_TimeWaited;
  double m_LastUpdateTime;

  /*! File to write */
  std::string m_BaseFilename;

  /*! Meta sequence to write to */
  vtkMetaImageSequenceIO* m_Writer;

  /*! When closing the file, re-read the data from file, and write it compressed */
  bool m_EnableFileCompression;

  /*! Preparing the header requires image data already collected, this flag makes the header preparation wait until valid data is collected */
  bool m_HeaderPrepared;

  /*! Record the number of frames captured */
  long int TotalFramesRecorded;  // hard drive will probably fill up before a regular int is hit, but still...

  bool EnableCapturing;

  int FrameBufferSize;

  /*! Mutex instance simultaneous access of writer (writer may be accessed from command processing thread and also the internal update thread) */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> WriterAccessMutex;

private:
  vtkVirtualDiscCapture(const vtkVirtualDiscCapture&);  // Not implemented.
  void operator=(const vtkVirtualDiscCapture&);  // Not implemented. 
};

#endif
