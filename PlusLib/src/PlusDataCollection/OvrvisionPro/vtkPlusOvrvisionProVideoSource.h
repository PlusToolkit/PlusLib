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

  vtkGetStringMacro( LeftEyeDataSourceName );
  vtkGetStringMacro( RightEyeDataSourceName );
  vtkGetVector2Macro( Resolution, int );
  vtkGetMacro( Framerate, int );
  vtkGetMacro( CameraSync, bool );
  vtkGetStringMacro( ProcessingModeName );
  vtkGetStringMacro(Vendor);
  vtkSetMacro(Exposure, int);

protected:
  vtkSetStringMacro( LeftEyeDataSourceName );
  vtkSetStringMacro( RightEyeDataSourceName );
  vtkSetVector2Macro( Resolution, int );
  vtkSetStringMacro( ProcessingModeName );
  vtkSetMacro( Framerate, int );
  vtkSetMacro( CameraSync, bool );
  vtkSetStringMacro(Vendor);
  vtkGetMacro(Exposure, int);

  /// Device-specific connect
  virtual PlusStatus InternalConnect();

  /// Device-specific disconnect
  virtual PlusStatus InternalDisconnect();

  /// Device-specific on-update function
  virtual PlusStatus InternalUpdate();

  bool ConfigureRequestedFormat();
  void ConfigureProcessingMode();

protected:
  vtkPlusOvrvisionProVideoSource();
  ~vtkPlusOvrvisionProVideoSource();

protected:
  // Requested capture format
  OVR::OvrvisionPro OvrvisionProHandle;
  OVR::Camprop RequestedFormat;
  OVR::ROI RegionOfInterest;
  OVR::Camqt ProcessingMode;
  bool CameraSync;

  // Cache variables from the config file
  int Resolution[2];
  int Framerate;
  char* ProcessingModeName;
  bool IsCapturingRGB;
  char* Vendor;
  int Exposure;

  // Record which data source corresponds to which eye
  char* LeftEyeDataSourceName;
  char* RightEyeDataSourceName;

  vtkPlusDataSource* LeftEyeDataSource;
  vtkPlusDataSource* RightEyeDataSource;

private:
  static vtkPlusOvrvisionProVideoSource* ActiveDevice;
  vtkPlusOvrvisionProVideoSource( const vtkPlusOvrvisionProVideoSource& ); // Not implemented.
  void operator=( const vtkPlusOvrvisionProVideoSource& ); // Not implemented.
};

#endif