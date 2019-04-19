/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "igsioCommon.h"
#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusFakeIntuitiveDaVinciTracker.h"

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

vtkStandardNewMacro(vtkPlusFakeIntuitiveDaVinciTracker);

//----------------------------------------------------------------------------
vtkPlusFakeIntuitiveDaVinciTracker::vtkPlusFakeIntuitiveDaVinciTracker()
  : vtkPlusDevice()
  , DaVinci(new FakeIntuitiveDaVinci())
  , LastFrameNumber(0)
  , FrameNumber(0)
{
  this->StartThreadForInternalUpdates = true; // Want a dedicated thread
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->AcquisitionRate = 50;

  this->PSM1Tip = NULL;
  this->PSM2Tip = NULL;


  LOG_DEBUG("vktPlusFakeIntuitiveDaVinciTracker created.");
}

//----------------------------------------------------------------------------
vtkPlusFakeIntuitiveDaVinciTracker::~vtkPlusFakeIntuitiveDaVinciTracker()
{

  this->StopRecording();
  this->Disconnect();

  if (this->DaVinci != nullptr)
  {
    this->DaVinci->stop();
    delete this->DaVinci;
    this->DaVinci = nullptr;
  }

  LOG_DEBUG("vktPlusFakeIntuitiveDaVinciTracker destroyed.");
}

//----------------------------------------------------------------------------
void vtkPlusFakeIntuitiveDaVinciTracker::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::Probe()
{
  LOG_DEBUG("Probing vtkPlusFakeIntuitiveDaVinciTracker.");

  this->DaVinci->stop();
  this->DaVinci->disconnect();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::InternalStartRecording()
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
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::InternalStopRecording()
{
  // Stop the stream and disconnect from the da Vinci.
  this->DaVinci->stop();

  LOG_DEBUG("InternalStartRecording stopped.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::InternalUpdate()
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
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_DEBUG("vtkPlusFakeIntuitiveDaVinciTracker::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // Determine which manipulators we're interested in.
  // Store those names for later, where we can then create the data source pointers.

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
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_DEBUG("vtkPlusFakeIntuitiveDaVinciTracker::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::InternalConnect()
{
  // Before trying to get to the da Vinci, let's see what was read in our XML config file.
  // That is, what manipulators did we say we were interested in?
  LOG_DEBUG("vtkPlusFakeIntuitiveDaVinciTracker::InternalConnect");

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
PlusStatus vtkPlusFakeIntuitiveDaVinciTracker::InternalDisconnect()
{
  LOG_DEBUG("vtkPlusFakeIntuitiveDaVinciTracker::InternalDisconnect");
  this->DaVinci->disconnect();

  this->PSM1Tip = NULL;
  this->PSM2Tip = NULL;

  LOG_DEBUG("Disconnection successful.")
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
FakeIntuitiveDaVinci* vtkPlusFakeIntuitiveDaVinciTracker::GetDaVinci() const
{
  return this->DaVinci;
}

//----------------------------------------------------------------------------
void vtkPlusFakeIntuitiveDaVinciTracker::setVtkMatrixFromISITransform(vtkMatrix4x4& destVtkMatrix, ISI_TRANSFORM* srcIsiMatrix)
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