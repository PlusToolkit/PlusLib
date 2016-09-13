/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOvrvisionProVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkPlusOvrvisionProVideoSource );

//----------------------------------------------------------------------------
vtkPlusOvrvisionProVideoSource::vtkPlusOvrvisionProVideoSource()
  : RequestedFormat( OVR::OV_CAM20VR_VGA )
  , ProcessingMode( OVR::OV_CAMQT_NONE )
  , CameraSync( false )
  , Framerate( -1 )
  , ProcessingModeName( NULL )
  , LeftEyeDataSourceName( NULL )
  , RightEyeDataSourceName( NULL )
  , LeftEyeDataSource( NULL )
  , RightEyeDataSource( NULL )
{
  this->RequireImageOrientationInConfiguration = true;

  // Poll-based device
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusOvrvisionProVideoSource::~vtkPlusOvrvisionProVideoSource()
{
  if( !this->Connected )
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );

  os << indent << "Resolution: " << Resolution[0] << ", " << Resolution[1] << std::endl;
  os << indent << "Framerate: " << Framerate << std::endl;
  os << indent << "CameraSync: " << CameraSync << std::endl;
  os << indent << "ProcessingMode: " << ProcessingModeName << std::endl;
  os << indent << "LeftEyeDataSourceName: " << LeftEyeDataSourceName << std::endl;
  os << indent << "RightEyeDataSourceName: " << RightEyeDataSourceName << std::endl;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkPlusOvrvisionProVideoSource::InternalConnect" );

  int frameSize[3] = { Resolution[0], Resolution[1], 1 };
  LeftEyeDataSource->SetInputFrameSize( frameSize );
  LeftEyeDataSource->SetNumberOfScalarComponents( 3 );
  RightEyeDataSource->SetInputFrameSize( frameSize );
  RightEyeDataSource->SetNumberOfScalarComponents( 3 );

  LeftFrameRGB = new unsigned char[frameSize[0] * frameSize[1] * frameSize[2] * sizeof( unsigned char ) * 3];
  RightFrameRGB = new unsigned char[frameSize[0] * frameSize[1] * frameSize[2] * sizeof( unsigned char ) * 3];

  if ( !OvrvisionProHandle.Open( 0, RequestedFormat ) ) // We don't need to share it with OpenGL/D3D, but in the future we could access the images in GPU memory
  {
    LOG_ERROR( "Unable to connect to OvrvisionPro device." );
    return PLUS_FAIL;
  }

  OvrvisionProHandle.SetCameraSyncMode( CameraSync );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalDisconnect()
{
  LOG_DEBUG( "vtkPlusOvrvisionProVideoSource::InternalDisconnect" );

  delete[] LeftFrameRGB;
  LeftFrameRGB = NULL;
  delete[] RightFrameRGB;
  RightFrameRGB = NULL;

  if ( OvrvisionProHandle.isOpen() )
  {
    OvrvisionProHandle.Close();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalUpdate()
{
  int numErrors( 0 );

  // Query the SDK for the latest frames
  OvrvisionProHandle.PreStoreCamData( OVR::OV_CAMQT_NONE );
  unsigned char* leftFrameBGRA = OvrvisionProHandle.GetCamImageBGRA( OVR::OV_CAMEYE_LEFT );
  unsigned char* rightFrameBGRA = OvrvisionProHandle.GetCamImageBGRA( OVR::OV_CAMEYE_RIGHT );

  unsigned int frameSize[3];
  LeftEyeDataSource->GetInputFrameSize( frameSize ); // Left and right eye have identical image sizes

  unsigned char* leftTargetPixel = LeftFrameRGB;
  unsigned char* leftSourcePixel = leftFrameBGRA;
  unsigned char* rightTargetPixel = RightFrameRGB;
  unsigned char* rightSourcePixel = rightFrameBGRA;
  const unsigned int targetPixelStride = 3 * sizeof( unsigned char );
  const unsigned int sourcePixelStride = 4 * sizeof( unsigned char );
  for ( unsigned int y = 0; y < frameSize[1]; ++y )
  {
    for ( unsigned int x = 0; x < frameSize[0]; ++x )
    {
      leftTargetPixel[2] = leftSourcePixel[0]; // blue
      leftTargetPixel[1] = leftSourcePixel[1]; // green
      leftTargetPixel[0] = leftSourcePixel[2]; // red

      rightTargetPixel[2] = rightSourcePixel[0]; // blue
      rightTargetPixel[1] = rightSourcePixel[1]; // green
      rightTargetPixel[0] = rightSourcePixel[2]; // red

      leftTargetPixel += targetPixelStride;
      rightTargetPixel += targetPixelStride;
      leftSourcePixel += sourcePixelStride;
      rightSourcePixel += sourcePixelStride;
    }
  }

  // Add them to our local buffers
  if ( LeftEyeDataSource->AddItem( LeftFrameRGB,
                                   LeftEyeDataSource->GetInputImageOrientation(),
                                   LeftEyeDataSource->GetInputFrameSize(),
                                   VTK_UNSIGNED_CHAR,
                                   3,
                                   US_IMG_RGB_COLOR,
                                   0,
                                   this->FrameNumber ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to add left eye image to data source." );
    numErrors++;
  }

  if ( RightEyeDataSource->AddItem( RightFrameRGB,
                                    RightEyeDataSource->GetInputImageOrientation(),
                                    RightEyeDataSource->GetInputFrameSize(),
                                    VTK_UNSIGNED_CHAR,
                                    3,
                                    US_IMG_RGB_COLOR,
                                    0,
                                    this->FrameNumber ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to add right eye image to data source." );
    numErrors++;
  }

  this->FrameNumber++;

  return numErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusOvrvisionProVideoSource::ConfigureRequestedFormat()
{
  RegionOfInterest.offsetX = 0;
  RegionOfInterest.offsetY = 0;
  RegionOfInterest.width = Resolution[0];
  RegionOfInterest.height = Resolution[1];

  switch ( Framerate )
  {
  case 15:
    if ( Resolution[0] == 2560 && Resolution[1] == 1920 )
    {
      RequestedFormat = OVR::OV_CAM5MP_FULL;
      return true;
    }
    if ( Resolution[0] == 1280 && Resolution[1] == 960 )
    {
      RequestedFormat = OVR::OV_CAM20HD_FULL;
      return true;
    }
    return false;
  case 30:
    if ( Resolution[0] == 1920 && Resolution[1] == 1080 )
    {
      RequestedFormat = OVR::OV_CAM5MP_FHD;
      return true;
    }
    if ( Resolution[0] == 640 && Resolution[1] == 480 )
    {
      RequestedFormat = OVR::OV_CAM20VR_VGA;
      return true;
    }
    return false;
  case 45:
    if ( Resolution[0] == 1280 && Resolution[1] == 960 )
    {
      RequestedFormat = OVR::OV_CAMHD_FULL;
      return true;
    }
    return false;
  case 60:
    if ( Resolution[0] == 960 && Resolution[1] == 950 )
    {
      RequestedFormat = OVR::OV_CAMVR_FULL;
      return true;
    }
    if ( Resolution[0] == 1280 && Resolution[1] == 800 )
    {
      RequestedFormat = OVR::OV_CAMVR_WIDE;
      return true;
    }
    return false;
  case 90:
    if ( Resolution[0] == 640 && Resolution[1] == 480 )
    {
      RequestedFormat = OVR::OV_CAMVR_VGA;
      return true;
    }
    return false;
  case 120:
    if ( Resolution[0] == 320 && Resolution[1] == 240 )
    {
      RequestedFormat = OVR::OV_CAMVR_QVGA;
      return true;
    }
    return false;
  default:
    LOG_ERROR( "Unsupported framerate requested." );
  }

  return false;
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::ConfigureProcessingMode()
{
  ProcessingMode = OVR::OV_CAMQT_NONE;
  if ( STRCASECMP( ProcessingModeName, "OV_CAMQT_DMSRMP" ) == 0 )
  {
    ProcessingMode = OVR::OV_CAMQT_DMSRMP;
  }
  else if ( STRCASECMP( ProcessingModeName, "OV_CAMQT_DMS" ) == 0 )
  {
    ProcessingMode = OVR::OV_CAMQT_DMS;
  }
  else
  {
    LOG_WARNING( "Unrecognized processing mode detected." );
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING( deviceConfig, rootConfigElement );

  XML_READ_VECTOR_ATTRIBUTE_REQUIRED( int, 2, Resolution, deviceConfig );
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED( int, Framerate, deviceConfig );
  XML_READ_STRING_ATTRIBUTE_REQUIRED( LeftEyeDataSourceName, deviceConfig );
  XML_READ_STRING_ATTRIBUTE_REQUIRED( RightEyeDataSourceName, deviceConfig );

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL( CameraSync, deviceConfig );
  XML_READ_STRING_ATTRIBUTE_OPTIONAL( ProcessingModeName, deviceConfig );

  if ( !ConfigureRequestedFormat() )
  {
    return PLUS_FAIL;
  }

  if ( ProcessingModeName != NULL )
  {
    ConfigureProcessingMode();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::WriteConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING( deviceConfig, rootConfigElement );

  int resolution[2];
  switch ( RequestedFormat )
  {
  case OVR::OV_CAM5MP_FULL:
    //!2560x1920 @15fps
    resolution[0] = 2560;
    resolution[1] = 1920;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 15 );
    break;
  case OVR::OV_CAM5MP_FHD:
    //!1920x1080 @30fps
    resolution[0] = 1920;
    resolution[1] = 1080;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 30 );
    break;
  case OVR::OV_CAMHD_FULL:
    //!1280x960  @45fps
    resolution[0] = 1280;
    resolution[1] = 960;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 45 );
    break;
  case OVR::OV_CAMVR_FULL:
    //!960x950   @60fps
    resolution[0] = 960;
    resolution[1] = 950;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 60 );
    break;
  case OVR::OV_CAMVR_WIDE:
    //!1280x800  @60fps
    resolution[0] = 1280;
    resolution[1] = 800;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 60 );
    break;
  case OVR::OV_CAMVR_VGA:
    //!640x480   @90fps
    resolution[0] = 640;
    resolution[1] = 480;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 90 );
    break;
  case OVR::OV_CAMVR_QVGA:
    //!320x240   @120fps
    resolution[0] = 320;
    resolution[1] = 240;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 120 );
    break;
  case OVR::OV_CAM20HD_FULL:
    //!1280x960  @15fps
    resolution[0] = 1280;
    resolution[1] = 960;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 15 );
    break;
  case OVR::OV_CAM20VR_VGA:
    //!640x480   @30fps
    resolution[0] = 640;
    resolution[1] = 480;
    deviceConfig->SetVectorAttribute( "Resolution", 2, resolution );
    deviceConfig->SetIntAttribute( "Framerate", 30 );
    break;
  }

  if ( CameraSync )
  {
    deviceConfig->SetAttribute( "CameraSync", "TRUE" );
  }

  if ( ProcessingMode != OVR::OV_CAMQT_NONE )
  {
    switch ( ProcessingMode )
    {
    case OVR::OV_CAMQT_DMS:
      deviceConfig->SetAttribute( "ProcessingModeName", "OV_CAMQT_DMS" );
      break;
    case OVR::OV_CAMQT_DMSRMP:
      deviceConfig->SetAttribute( "ProcessingModeName", "OV_CAMQT_DMSRMP" );
      break;
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::NotifyConfigured()
{
  // OvrvisionSDK requires two data sources, left eye and right eye
  if ( this->GetDataSource( LeftEyeDataSourceName, LeftEyeDataSource ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to locate data source for left eye labelled: " << LeftEyeDataSourceName );
    return PLUS_FAIL;
  }

  if ( LeftEyeDataSource->GetImageType() != US_IMG_RGB_COLOR )
  {
    LOG_ERROR( "Left eye data source must be configured for image type US_IMG_RGB_COLOR. Aborting." );
    return PLUS_FAIL;
  }

  if ( this->GetDataSource( RightEyeDataSourceName, RightEyeDataSource ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to locate data source for right eye labelled: " << RightEyeDataSourceName );
    return PLUS_FAIL;
  }

  if ( RightEyeDataSource->GetImageType() != US_IMG_RGB_COLOR )
  {
    LOG_ERROR( "Right eye data source must be configured for image type US_IMG_RGB_COLOR. Aborting." );
    return PLUS_FAIL;
  }

  if ( this->OutputChannels.size() != 2 )
  {
    LOG_ERROR( "OvrvisionPro device requires exactly 2 output channels. One for each eye." );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
