/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStream_h
#define __vtkPlusStream_h

#include "PlusConfigure.h"
#include "vtkDataObject.h"
#include "vtkPlusDevice.h"
#include "vtkRfProcessor.h"

class vtkPlusDevice;

/*!
  \class vtkPlusChannel 
  \brief  Contains an optional timestamped circular buffer containing the video images and a number of timestamped circular buffer of StreamBufferItems for the transforms. 
          StreamBufferItems are essentially a class that contains both a single video frame and/or a 4x4 matrix.

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusChannel : public vtkDataObject
{
public:
  static vtkPlusChannel *New();
  vtkTypeRevisionMacro(vtkPlusChannel, vtkObject);

  /*!
    Parse the XML, read the details about the stream
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aChannelElement, bool RequireRfElementInDeviceSetConfiguration );
  /*!
    Write the details about the stream to XML
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aChannelElement);

  PlusStatus GetVideoSource( vtkPlusDataSource*& aVideoSource );
  PlusStatus GetVideoSource( vtkPlusDataSource*& aVideoSource ) const;
  void SetVideoSource( vtkPlusDataSource* aSource );
  bool HasVideoSource() const;

  int ToolCount() const { return this->Tools.size(); }
  PlusStatus AddTool(vtkPlusDataSource* aTool );
  PlusStatus RemoveTool(const char* toolName);
  PlusStatus GetTool(vtkPlusDataSource*& aTool, const char* toolName);
  PlusStatus RemoveTools();
  DataSourceContainerIterator GetToolsStartIterator();
  DataSourceContainerIterator GetToolsEndIterator();
  DataSourceContainerConstIterator GetToolsStartConstIterator() const;
  DataSourceContainerConstIterator GetToolsEndConstIterator() const;

  bool GetTrackingDataAvailable();
  bool GetVideoDataAvailable();
  bool GetTrackingEnabled() const;
  bool GetVideoEnabled() const;

  /*! Make a request for the latest image frame */
  vtkImageData* GetBrightnessOutput();

  /*! Return the dimensions of the brightness frame size */
  PlusStatus GetBrightnessFrameSize(int aDim[2]);

  /*! Get the first active tool object */
  PlusStatus GetFirstActiveTool(vtkPlusDataSource*& aTool); 

  /*!
    Get tracked frame containing the transform(s) or the
    image(s) acquired from the device at a specific timestamp
    \param timestamp Timestamp of the requested tracked frame
    \param trackedFrame Target tracked frame
    \param enableImageData Enable returning of image data. Tracking data will be interpolated at the timestamp of the image data.
  */
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame& trackedFrame, bool enableImageData=true);
  virtual PlusStatus GetTrackedFrame(TrackedFrame *trackedFrame);

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestampOfLastFrameAlreadyGot Used for preventing returning the same frame multiple times. In: the timestamp of the timestamp that has been already returned in previous GetTrackedFrameListSampled calls. If no frames have got yet then set it to UNDEFINED_TIMESTAMP. Out: the timestamp of the most recent frame that is returned.
    \param aTimestampOfNextFrameToBeAdded Timestamp of the next frame that should be added. This value is increased by the multiple of aSamplingPeriodSec.
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aSamplingPeriodSec Sampling period time for getting the frames in seconds (timestamps are in seconds too)
    \param maxTimeLimitSec Maximum time spent in the function (in sec)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double &aTimestampOfLastFrameAlreadyGot, double& aTimestampOfNextFrameToBeAdded, vtkTrackedFrameList* aTrackedFrameList, double aSamplingPeriodSec, double maxTimeLimitSec=-1); 

  PlusStatus GetTrackedFrameList( double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd );

  /*! Get the closest tracked frame timestamp to the specified time */
  double GetClosestTrackedFrameTimestampByTime(double time);

  /*! 
    Get the tracked frame from devices by time with each tool transforms
    \param time The closes frame to this timestamp will be retrieved
    \param trackedFrame The output where the tracked frame information will be copied
  */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  PlusStatus Clear();

  virtual void ShallowCopy(const vtkPlusChannel& aChannel);

  PlusStatus GetLatestTimestamp(double& aTimestamp) const;

  vtkSetObjectMacro(OwnerDevice, vtkPlusDevice);
  vtkGetObjectMacro(OwnerDevice, vtkPlusDevice);

  vtkSetStringMacro(ChannelId);
  vtkGetStringMacro(ChannelId);

  vtkGetObjectMacro(RfProcessor, vtkRfProcessor);
  vtkSetObjectMacro(RfProcessor, vtkRfProcessor);

  vtkSetMacro(SaveRfProcessingParameters, bool);

protected:
  /*! Get number of tracked frames between two given timestamps (inclusive) */
  int GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo);

protected:
  DataSourceContainer       Tools;
  vtkPlusDataSource*        VideoSource;
  vtkPlusDevice*            OwnerDevice;
  char *                    ChannelId;

  /*! RF to brightness conversion */
  vtkRfProcessor* RfProcessor;
  vtkImageData* BlankImage;
  StreamBufferItem BrightnessOutputTrackedFrame;
  int BrightnessFrameSize[2];

  /*! If true then RF processing parameters will be saved into the config file */
  bool SaveRfProcessingParameters;

  vtkPlusChannel(void);
  virtual ~vtkPlusChannel(void);

private:
  vtkPlusChannel(const vtkPlusChannel&);
  void operator=(const vtkPlusChannel&);
};

#endif