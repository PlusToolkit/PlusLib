/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualVolumeReconstructor_h
#define __vtkVirtualVolumeReconstructor_h

#include "vtkMetaImageSequenceIO.h"
#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"
#include <string>

class vtkVolumeReconstructor;
class vtkTransformRepository;

/*!
\class vtkVirtualVolumeReconstructor 
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualVolumeReconstructor : public vtkPlusDevice
{
public:
  static vtkVirtualVolumeReconstructor *New();
  vtkTypeRevisionMacro(vtkVirtualVolumeReconstructor, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*!
    This method is safe to be called from any thread.
  */
  virtual PlusStatus GetReconstructedVolumeFromFile(const char* inputSeqFilename, vtkImageData* reconstructedVolume, std::string& message);

  /*!
    This method is safe to be called from any thread.
  */
  PlusStatus GetReconstructedVolume(vtkImageData* reconstructedVolume, std::string& message);

  /*!
    Updated the transform repository contents within the volume reconstructor.
    It is advisable to call this before each volume reconstruction starting.
    This method is safe to be called from any thread.
  */
  PlusStatus UpdateTransformRepository(vtkTransformRepository* sharedTransformRepository);

  /*!
    Enables adding frames to the volume. It can be used for pausing the recording.
    This method is safe to be called from any thread.
  */
  vtkGetMacro(EnableReconstruction, bool);
  /*!
    Enables adding frames to the volume. It can be used for pausing the recording.
    This method is safe to be called from any thread.
  */
  void SetEnableReconstruction(bool aValue);  

  /*!
    Clear the volume.
    This method is safe to be called from any thread.
  */
  virtual PlusStatus Reset();

  /*! If specified, the reconstructed volume will be saved into this filename */
  vtkSetStringMacro(OutputVolFilename);
  vtkGetStringMacro(OutputVolFilename);

  /*! If specified, the reconstructed volume will sent to the client through OpenIGTLink, using this device name */
  vtkSetStringMacro(OutputVolDeviceName);
  vtkGetStringMacro(OutputVolDeviceName);

  /*! Set the output volume's origin in the Reference coordinate system*/
  void SetOutputOrigin(double* origin);

  /*! Set the output volume's spacing in the Reference coordinate system's unit (usually mm)*/
  void SetOutputSpacing(double* spacing);

  /*! Set the output volume's extent (xStart, xEnd, yStart, yEnd, zStart, zEnd) in voxels */
  void SetOutputExtent(int* extent);

protected:

    /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus InternalUpdate();

  virtual PlusStatus NotifyConfigured();

  virtual double GetAcquisitionRate() const;  

  virtual int OutputChannelCount() const;

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

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  PlusStatus AddFrames(vtkTrackedFrameList* trackedFrameList);

  vtkSetMacro(ActualFrameRate, double);

  /*!
  * Get the maximum frame rate from the video source. If there is none then the tracker
  * \return Maximum frame rate
  */
  double GetMaximumFrameRate();

  /*! Get the sampling period length (in seconds). Frames are copied from the devices to the data collection buffer once in every sampling period. */
  double GetSamplingPeriodSec();

  vtkVirtualVolumeReconstructor();
  virtual ~vtkVirtualVolumeReconstructor();

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

  /*! Timestamp of last added frame (the tracked frames acquired since this timestamp will be added to the volume on the next Execute) */
  double LastRecordedFrameTimestamp;

  /*! Record the number of frames captured */
  long int TotalFramesRecorded;  // hard drive will probably fill up before a regular int is hit, but still...

  vtkSmartPointer<vtkVolumeReconstructor> VolumeReconstructor;
  vtkSmartPointer<vtkTransformRepository> TransformRepository;

  bool EnableReconstruction;

  char* OutputVolFilename;
  char* OutputVolDeviceName;

  /*! Mutex instance simultaneous access of writer (writer may be accessed from command processing thread and also the internal update thread) */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> VolumeReconstructorAccessMutex;

private:
  vtkVirtualVolumeReconstructor(const vtkVirtualVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkVirtualVolumeReconstructor&);  // Not implemented. 
};

#endif
