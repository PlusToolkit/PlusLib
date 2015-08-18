/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStream_h
#define __vtkPlusStream_h

#include "PlusConfigure.h"
#include "vtkDataCollectionExport.h"

#include "StreamBufferItem.h"
#include "vtkDataObject.h"
#include "vtkRfProcessor.h"

class TrackedFrame;
class vtkHTMLGenerator;
class vtkPlusDataSource;
class vtkPlusDevice;
class vtkTrackedFrameList;

typedef std::map<std::string, vtkPlusDataSource*> DataSourceContainer;
typedef DataSourceContainer::iterator DataSourceContainerIterator;
typedef DataSourceContainer::const_iterator DataSourceContainerConstIterator;

/*!
  \class vtkPlusChannel 
  \brief  Contains an optional timestamped circular buffer containing the video images and a number of timestamped circular buffer of StreamBufferItems for the transforms. 
          StreamBufferItems are essentially a class that contains both a single video frame and/or a 4x4 matrix.

  \ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkPlusChannel : public vtkDataObject
{
public:
  typedef std::map< std::string, std::string > CustomAttributeMap;
  typedef CustomAttributeMap::iterator CustomAttributeMapIterator;
  typedef CustomAttributeMap::const_iterator CustomAttributeMapConstIterator;

public:
  static vtkPlusChannel *New();
  vtkTypeMacro(vtkPlusChannel, vtkObject);

  /*!
    Parse the XML, read the details about the stream
  */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aChannelElement, bool RequireImageOrientationInChannelConfiguration );
  /*!
    Write the details about the stream to XML
  */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aChannelElement);

  inline PlusStatus GetVideoSource( vtkPlusDataSource*& aVideoSource ) const
  {
    aVideoSource = this->VideoSource;
    return aVideoSource!=NULL ? PLUS_SUCCESS : PLUS_FAIL;
  }

  void SetVideoSource( vtkPlusDataSource* aSource );
  inline bool HasVideoSource() const { return this->VideoSource != NULL; };
  bool IsVideoSource3D() const;

  int ToolCount() const { return this->Tools.size(); }
  PlusStatus AddTool(vtkPlusDataSource* aTool );
  PlusStatus RemoveTool(const char* toolSourceId);
  PlusStatus GetTool(vtkPlusDataSource*& aTool, const char* toolSourceId);
  PlusStatus RemoveTools();
  inline DataSourceContainerIterator GetToolsStartIterator() { return this->Tools.begin(); };
  inline DataSourceContainerIterator GetToolsEndIterator() { return this->Tools.end(); };
  inline DataSourceContainerConstIterator GetToolsStartConstIterator() const { return this->Tools.begin(); };
  inline DataSourceContainerConstIterator GetToolsEndConstIterator() const { return this->Tools.end(); };

  bool GetTrackingDataAvailable();
  bool GetVideoDataAvailable();
  bool GetTrackingEnabled() const;
  bool GetVideoEnabled() const;

  /*! Make a request for the latest image frame */
  vtkImageData* GetBrightnessOutput();

  /*! Return the dimensions of the brightness frame size */
  PlusStatus GetBrightnessFrameSize(int aDim[3]);
  
  /*!
    Get the timestamp master tool. The timestamp master tool determines the sampling times
    for all the other tools in the output channel if no video data is available.
    Currently the tool that is first added to the channel is used as master tool.
  */
  PlusStatus GetTimestampMasterTool(vtkPlusDataSource*& aTool); 

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

  /*!
    Get all the tracked frame list from devices since time specified
    \param aTimestampOfLastFrameAlreadyGot Used for preventing returning the same frame multiple times.
      In: the timestamp of the timestamp that has been already returned in previous GetTrackedFrameList calls.
      If no frames have got yet then set it to UNDEFINED_TIMESTAMP, the frames will be retrieved from the latest timestamp.
      Out: the timestamp of the most recent frame that is returned.
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aMaxNumberOfFramesToAdd Maximum this number of frames will be added (can be used for limiting the time spent in this method)
  */
  PlusStatus GetTrackedFrameList( double& aTimestampOfLastFrameAlreadyGot, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd );

  /*! Get the closest tracked frame timestamp to the specified time */
  virtual double GetClosestTrackedFrameTimestampByTime(double time);

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  virtual PlusStatus Clear();

  virtual void ShallowCopy(const vtkPlusChannel& aChannel);

  virtual PlusStatus GetLatestTimestamp(double& aTimestamp) const;

  void SetOwnerDevice(vtkPlusDevice* _arg){ this->OwnerDevice = _arg; }
  vtkPlusDevice* GetOwnerDevice() { return this->OwnerDevice; }

  PlusStatus SetCustomAttribute( const std::string& attributeId, const std::string& value );
  PlusStatus GetCustomAttribute( const std::string& attributeId, std::string& output ) const;
  PlusStatus GetCustomAttributeMap( CustomAttributeMap& output ) const;

  vtkSetStringMacro(ChannelId);
  vtkGetStringMacro(ChannelId);

  vtkGetObjectMacro(RfProcessor, vtkRfProcessor);
  vtkSetObjectMacro(RfProcessor, vtkRfProcessor);

  vtkSetMacro(SaveRfProcessingParameters, bool);

  /*! 
    Add generated html report from data acquisition to the existing html report. 
    htmlReport and plotter arguments has to be defined by the caller function 
  */
  virtual PlusStatus GenerateDataAcquisitionReport( vtkHTMLGenerator* htmlReport); 

protected:
  /*! Get number of tracked frames between two given timestamps (inclusive) */
  virtual int GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo);

protected:
  DataSourceContainer       Tools;
  vtkPlusDataSource*        VideoSource;
  vtkPlusDevice*            OwnerDevice;
  char *                    ChannelId;

  /*! RF to brightness conversion */
  vtkRfProcessor* RfProcessor;
  vtkImageData* BlankImage;
  StreamBufferItem BrightnessOutputTrackedFrame;
  int BrightnessFrameSize[3];

  /*! If true then RF processing parameters will be saved into the config file */
  bool SaveRfProcessingParameters;

  /*!
    This tool will be used to provide timestamps if no video data is present
    All the other tools will use the same timestamps and the transforms will be
    interpolated if needed.
  */
  vtkPlusDataSource* TimestampMasterTool;
  
  CustomAttributeMap CustomAttributes;

  vtkPlusChannel(void);
  virtual ~vtkPlusChannel(void);

private:
  vtkPlusChannel(const vtkPlusChannel&);
  void operator=(const vtkPlusChannel&);
};

#endif