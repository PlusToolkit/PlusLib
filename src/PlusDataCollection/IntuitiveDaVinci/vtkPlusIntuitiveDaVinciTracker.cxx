/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "igsioCommon.h"
#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIntuitiveDaVinciTracker.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

// OS includes
#include <ctype.h>
#include <float.h>
#include <iomanip>
#include <limits.h>
#include <math.h>
#include <time.h>

// STL
#include <fstream>
#include <iostream>
#include <set>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusIntuitiveDaVinciTracker);

//----------------------------------------------------------------------------
vtkPlusIntuitiveDaVinciTracker::vtkPlusIntuitiveDaVinciTracker()
  : vtkPlusDevice()
  , DaVinci(new IntuitiveDaVinci())
  , LastFrameNumber(0)
  , FrameNumber(0)
{
  this->StartThreadForInternalUpdates = true; // Want a dedicated thread
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->AcquisitionRate = 50;

  this->PSM1Tip = NULL;
  this->PSM2Tip = NULL;

  LOG_DEBUG("vktPlusIntuitiveDaVinciTracker created.");
}

//----------------------------------------------------------------------------
vtkPlusIntuitiveDaVinciTracker::~vtkPlusIntuitiveDaVinciTracker()
{

  this->StopRecording();
  this->Disconnect();

  if (this->DaVinci != nullptr)
  {
    this->DaVinci->stop();
    delete this->DaVinci;
    this->DaVinci = nullptr;
  }

  LOG_DEBUG("vktPlusIntuitiveDaVinciTracker destroyed.");
}

//----------------------------------------------------------------------------
void vtkPlusIntuitiveDaVinciTracker::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::Probe()
{
  LOG_DEBUG("Probing vtkPlusIntuitiveDaVinciTracker.");

  this->DaVinci->stop();
  this->DaVinci->disconnect();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalStartRecording()
{
  if (!this->Connected)
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci has not been initialized");
    return PLUS_FAIL;
  }

  if (!this->DaVinci->isConnected())
  {
    LOG_ERROR("InternalStartRecording failed: da Vinci is not connected");
    return PLUS_FAIL;
  }

  if (!this->DaVinci->start())
  {
    LOG_ERROR("InternalStartRecording: Unable to start streaming.");
    return PLUS_FAIL;
  }

  LOG_DEBUG("InternalStartRecording started.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalStopRecording()
{
  // Stop the stream and disconnect from the da Vinci.
  this->DaVinci->stop();

  LOG_DEBUG("InternalStartRecording stopped.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalUpdate()
{
  
  this->LastFrameNumber++;
  int defaultToolFrameNumber = this->LastFrameNumber;
  const double toolTimestamp = vtkIGSIOAccurateTimer::GetSystemTime(); // unfiltered timestamp
  
  
  if(this->PSM1Tip != NULL)
  {
    vtkSmartPointer<vtkMatrix4x4> PSM1TipToBase = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    
    transform->RotateX(50.0*toolTimestamp);
    transform->RotateY(50.0*toolTimestamp);
    transform->RotateZ(50.0*toolTimestamp);

    transform->GetMatrix(PSM1TipToBase);

    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->PSM1Tip->GetFrameNumber() + 1 ;
    ToolTimeStampedUpdate(this->PSM1Tip->GetId(), PSM1TipToBase, TOOL_OK, frameNumber, toolTimestamp);
  }

  if(this->PSM2Tip != NULL)
  {
    vtkSmartPointer<vtkMatrix4x4> PSM2TipToBase = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    
    transform->RotateX(70.0*toolTimestamp);
    transform->RotateY(70.0*toolTimestamp);
    transform->RotateZ(70.0*toolTimestamp);

    transform->GetMatrix(PSM2TipToBase);

    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->PSM2Tip->GetFrameNumber() + 1 ;
    ToolTimeStampedUpdate(this->PSM2Tip->GetId(), PSM2TipToBase, TOOL_OK, frameNumber, toolTimestamp);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_DEBUG("vtkPlusIntuitiveDaVinciTracker::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  /* Because of how PLUS works when we create a vtkPlusDevice,
  the XML file is already parsed. The parent class reads in the configuration
  file, and finds all the data sources that are tools.

  It then parses the tool tags and adds them (if possible) to this device.

  See vtkPlusDevice.cxx : ReadConfiguration( ... )
  */

  XML_READ_SCALAR_ATTRIBUTE_WARNING(int, AcquisitionRate, deviceConfig); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_DEBUG("vtkPlusIntuitiveDaVinciTracker::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalConnect()
{
  // Before trying to get to the da Vinci, let's see what was read in our XML config file.
  // That is, what manipulators did we say we were interested in?
  LOG_DEBUG("vtkPlusIntuitiveDaVinciTracker::InternalConnect");

  if (this->Connected)
  {
    LOG_DEBUG("Already connected to da Vinci");
    return PLUS_SUCCESS;
  }

  if (this->DaVinci->connect() != ISI_SUCCESS)
  {
    LOG_DEBUG("Failed to connect to da Vinci.");
    return PLUS_FAIL;
  }

  this->PSM1Tip = NULL;
  GetToolByPortName("PSM1Tip", this->PSM1Tip);
  this->PSM2Tip = NULL;
  GetToolByPortName("PSM2Tip", this->PSM2Tip);

  LOG_DEBUG("Connection successful.")
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::InternalDisconnect()
{
  LOG_DEBUG("vtkPlusIntuitiveDaVinciTracker::InternalDisconnect");
  this->DaVinci->disconnect();

  this->PSM1Tip = NULL;
  this->PSM2Tip = NULL;

  LOG_DEBUG("Disconnection successful.")
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
IntuitiveDaVinci* vtkPlusIntuitiveDaVinciTracker::GetDaVinci() const
{
  return this->DaVinci;
}

//----------------------------------------------------------------------------
void vtkPlusIntuitiveDaVinciTracker::setVtkMatrixFromISITransform(vtkMatrix4x4& destVtkMatrix, ISI_TRANSFORM* srcIsiMatrix)
{
  destVtkMatrix.Identity();

  // Let's VERY EXPLCITLY copy over the values.
  destVtkMatrix.SetElement(0, 0, srcIsiMatrix->rot.row0.x);
  destVtkMatrix.SetElement(1, 0, srcIsiMatrix->rot.row0.y);
  destVtkMatrix.SetElement(2, 0, srcIsiMatrix->rot.row0.z);
  destVtkMatrix.SetElement(3, 0, 0);

  destVtkMatrix.SetElement(0, 1, srcIsiMatrix->rot.row1.x);
  destVtkMatrix.SetElement(1, 1, srcIsiMatrix->rot.row1.y);
  destVtkMatrix.SetElement(2, 1, srcIsiMatrix->rot.row1.z);
  destVtkMatrix.SetElement(3, 1, 0);

  destVtkMatrix.SetElement(0, 2, srcIsiMatrix->rot.row2.x);
  destVtkMatrix.SetElement(1, 2, srcIsiMatrix->rot.row2.y);
  destVtkMatrix.SetElement(2, 2, srcIsiMatrix->rot.row2.z);
  destVtkMatrix.SetElement(3, 2, 0);

  destVtkMatrix.SetElement(0, 3, srcIsiMatrix->pos.x);
  destVtkMatrix.SetElement(1, 3, srcIsiMatrix->pos.y);
  destVtkMatrix.SetElement(2, 3, srcIsiMatrix->pos.z);
  destVtkMatrix.SetElement(3, 3, 1);

  return;
}