/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDataCollectorFile_h
#define __vtkDataCollectorFile_h

#include "vtkDataCollector.h"

/*!
  \class vtkDataCollectorFile
  \brief Collects tracked ultrasound data (images synchronized with tracking information) from sequence metafile

  This class collects ultrasound images synchronized with pose tracking information.

  \ingroup PlusLibDataCollection
*/ 
class VTK_EXPORT vtkDataCollectorFile: public vtkDataCollector
{
public:

  static vtkDataCollectorFile *New();
  vtkTypeRevisionMacro(vtkDataCollectorFile, vtkDataCollector);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig ); 

  /*! Disconnect from devices */
  virtual PlusStatus Disconnect(); 

  /*! Connect to devices */
  virtual PlusStatus Connect(); 

  /*! Stop data collection */
  virtual PlusStatus Stop(); 

  /*! Start data collection  */
  virtual PlusStatus Start(); 

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  /*! Get the most recent tracked frame from devices with each tool transforms */
  virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame); 

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aMaxNumberOfFramesToAdd The maximum number of latest frames acquired from the buffers (till most recent timestamp). If -1 get all frames in the time range since frameTimestamp
  */
  virtual PlusStatus GetTrackedFrameList(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd = -1); 

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aSamplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec);

  /*! Get the tracked frame from tracked frame list by timestamp */
  virtual PlusStatus GetTrackedFrameByTime(double aTimestamp, TrackedFrame* aTrackedFrame); 

  /*! Set video and tracker local time offset  */
  virtual void SetLocalTimeOffset(double videoOffset, double trackerOffset); 

public:
  /*! Synchronize the connected devices */
  virtual PlusStatus Synchronize( const char* bufferOutputFolder = NULL, bool acquireDataOnly = false ); 

  /*! Set the Tracking only flag */
  void SetTrackingOnly(bool);

  /*! Set the Video only flag */
  void SetVideoOnly(bool);

  /*! Get frame size */
  virtual void GetFrameSize(int aDim[2]);

  /*! Get frame rate */
  virtual PlusStatus GetFrameRate(double &aFrameRate);

  /*! Callback function for progress bar refreshing */  
  virtual void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb);

protected:
  /*! Compute next timestamp from start time and elapsed time */
  double GetCurrentFrameTimestamp();

  /*! Get index of tracked frame in the loaded tracked frame list that corresponds to a given timestamp */
  PlusStatus GetTrackedFrameIndexForTimestamp(double aTimestamp, int &aIndex);

  /*! Prepares output data. This is called by the superclass */
  virtual int RequestData(vtkInformation *request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

  /*! This method returns the largest data that can be generated. This is called by the superclass */
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

protected:
  /*! Set tracked frame list */
  virtual void SetTrackedFrameBuffer(vtkTrackedFrameList* trackedFrameBuffer); 
  /*! Get tracked frame list */
  vtkGetObjectMacro(TrackedFrameBuffer, vtkTrackedFrameList);

  /*! Set sequence metafile name */
	vtkSetStringMacro(SequenceMetafileName);
  /*! Get sequence metafile name */
	vtkGetStringMacro(SequenceMetafileName);

  /*! Set replay enabled flag */
	vtkSetMacro(ReplayEnabled, bool);
  /*! Get replay enabled flag */
	vtkGetMacro(ReplayEnabled, bool);

protected:
  vtkDataCollectorFile();
  virtual ~vtkDataCollectorFile();

protected:
  /*! Tracked frame list containing the data to be played */
  vtkTrackedFrameList*  TrackedFrameBuffer;

  /*! Name of the used sequence metafile */
	char*                 SequenceMetafileName;

  /*! Acquisition start time */
  double                StartTime; 

  /*! Flag indicating if replaying the simulated data is enabled */
	bool                  ReplayEnabled;

  /*! First timestamp in loaded tracked frame list */
  double                FirstTimestamp;

  /*! Last timestamp in loaded tracked frame list */
  double                LastTimestamp;

  /*! Index of last accessed (returned) tracked frame */
  int                   LastAccessedFrameIndex;

private:
  vtkDataCollectorFile(const vtkDataCollectorFile&);
  void operator=(const vtkDataCollectorFile&);

};

#endif
