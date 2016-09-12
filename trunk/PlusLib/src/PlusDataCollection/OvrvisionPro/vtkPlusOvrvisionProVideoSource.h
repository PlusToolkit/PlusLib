/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOvrvisionProVideoSource_h
#define __vtkPlusOvrvisionProVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// OvrvisionPro SDK includes
#include <ovrvision_pro.h>

/*!
  \class __vtkPlusOvrvisionProVideoSource_h
  \brief Class for providing video input from the OvrvisionPro stereo camera device
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOvrvisionProVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusOvrvisionProVideoSource* New();
  vtkTypeMacro( vtkPlusOvrvisionProVideoSource, vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual bool IsTracker() const
  {
    return false;
  }

  /// Read configuration from xml data
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );
  /// Write configuration to xml data
  virtual PlusStatus WriteConfiguration( vtkXMLDataElement* config );

  /// Perform any completion tasks once configured
  virtual PlusStatus NotifyConfigured();

  vtkGetMacro( DirectShowFilterID, int );
  vtkGetStringMacro( LeftEyeDataSourceName );
  vtkGetStringMacro( RightEyeDataSourceName );
  vtkGetVector2Macro( Resolution, int );
  vtkGetMacro( Framerate, int );

protected:
  vtkSetMacro( DirectShowFilterID, int );
  vtkSetStringMacro( LeftEyeDataSourceName );
  vtkSetStringMacro( RightEyeDataSourceName );
  vtkSetVector2Macro( Resolution, int );
  vtkSetMacro( Framerate, int );

  /// Device-specific connect
  virtual PlusStatus InternalConnect();

  /// Device-specific disconnect
  virtual PlusStatus InternalDisconnect();

  /// Given a requested resolution and framerate
  bool ConfigureRequestedFormat( int resolution[2], int fps );

protected:
  vtkPlusOvrvisionProVideoSource();
  ~vtkPlusOvrvisionProVideoSource();

  // Callback when the SDK tells us a new frame is available
  static void OnNewFrameAvailable();
  void GrabLatestStereoFrame();

protected:
  OVR::OvrvisionPro OvrvisionProHandle;

  // The filter ID to pass to the SDK
  int DirectShowFilterID;

  // Requested capture format
  OVR::Camprop RequestedFormat;
  OVR::ROI RegionOfInterest;

  // Cache variables from the config file
  int Resolution[2];
  int Framerate;

  // Record which data source corresponds to which eye
  char* LeftEyeDataSourceName;
  char* RightEyeDataSourceName;

  vtkPlusDataSource* LeftEyeDataSource;
  vtkPlusDataSource* RightEyeDataSource;

  // Memory to hold left and right eye images requested from the SDK
  unsigned char* LeftFrameBGRA;
  unsigned char* RightFrameBGRA;

private:
  static vtkPlusOvrvisionProVideoSource* ActiveDevice;
  vtkPlusOvrvisionProVideoSource( const vtkPlusOvrvisionProVideoSource& ); // Not implemented.
  void operator=( const vtkPlusOvrvisionProVideoSource& ); // Not implemented.
};

#endif