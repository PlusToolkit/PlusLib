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
  : DirectShowFilterID( 0 )
  , RequestedFormat( OVR::OV_CAM20VR_VGA )
{
  this->RequireImageOrientationInConfiguration = true;
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
void vtkPlusOvrvisionProVideoSource::OnNewFrameAvailable()
{
  //aSource->AddItem(frame->pixbuf, aSource->GetInputImageOrientation(), this->FrameSize, VTK_UNSIGNED_CHAR, numberOfScalarComponents, aSource->GetImageType(), 0, this->FrameNumber)
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );

  os << indent << "DirectShowFilterID: " << DirectShowFilterID << std::endl;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkPlusOvrvisionProVideoSource::InternalConnect" );

  if ( !OvrvisionProHandle.Open( DirectShowFilterID, RequestedFormat ) ) // We don't need to share it with OpenGL/D3D, but in the future we could access the images in GPU memory
  {
    LOG_ERROR( "Unable to connect to OvrvisionPro device." );
    return PLUS_FAIL;
  }

  OvrvisionProHandle.SetCallbackImageFunction( &this->OnNewFrameAvailable );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalDisconnect()
{
  LOG_DEBUG( "vtkPlusOvrvisionProVideoSource::InternalDisconnect" );

  if ( OvrvisionProHandle.isOpen() )
  {
    OvrvisionProHandle.Close();
  }

  OvrvisionProHandle.SetCallbackImageFunction( NULL );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusOvrvisionProVideoSource::ConfigureRequestedFormat( int resolution[2], int fps )
{
  switch ( fps )
  {
  case 15:
    if ( resolution[0] == 2560 && resolution[1] == 1920 )
    {
      RequestedFormat = OVR::OV_CAM5MP_FULL;
      return true;
    }
    if ( resolution[0] == 1280 && resolution[1] == 960 )
    {
      RequestedFormat = OVR::OV_CAM20HD_FULL;
      return true;
    }
    return false;
  case 30:
    if ( resolution[0] == 1920 && resolution[1] == 1080 )
    {
      RequestedFormat = OVR::OV_CAM5MP_FHD;
      return true;
    }
    if ( resolution[0] == 640 && resolution[1] == 480 )
    {
      RequestedFormat = OVR::OV_CAM20VR_VGA;
      return true;
    }
    return false;
  case 45:
    if ( resolution[0] == 1280 && resolution[1] == 960 )
    {
      RequestedFormat = OVR::OV_CAMHD_FULL;
      return true;
    }
    return false;
  case 60:
    if ( resolution[0] == 960 && resolution[1] == 950 )
    {
      RequestedFormat = OVR::OV_CAMVR_FULL;
      return true;
    }
    if ( resolution[0] == 1280 && resolution[1] == 800 )
    {
      RequestedFormat = OVR::OV_CAMVR_WIDE;
      return true;
    }
    return false;
  case 90:
    if ( resolution[0] == 640 && resolution[1] == 480 )
    {
      RequestedFormat = OVR::OV_CAMVR_VGA;
      return true;
    }
    return false;
  case 120:
    if ( resolution[0] == 320 && resolution[1] == 240 )
    {
      RequestedFormat = OVR::OV_CAMVR_QVGA;
      return true;
    }
    return false;
  default:
    LOG_ERROR( "Unsupported framerate requested." );
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING( deviceConfig, rootConfigElement );

  XML_READ_SCALAR_ATTRIBUTE_REQUIRED( int, DirectShowFilterID, deviceConfig );

  int resolution[2];
  XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_REQUIRED( int, 2, "Resolution", resolution, deviceConfig );
  int fps;
  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED( int, "Framerate", fps, deviceConfig );

  ConfigureRequestedFormat( resolution, fps );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::WriteConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING( deviceConfig, rootConfigElement );

  deviceConfig->SetIntAttribute( "DirectShowFilterID", DirectShowFilterID );

  int resolution[2];
  int fps;
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

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::NotifyConfigured()
{
  // OvrvisionSDK requires two data sources, left eye and right eye

  return PLUS_SUCCESS;
}
