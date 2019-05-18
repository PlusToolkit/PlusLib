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

  this->psm1BaseToWorld = NULL;
  this->psm2BaseToWorld = NULL;
  this->ecmBaseToWorld = NULL;

  this->psm1Transforms = NULL;
  this->psm2Transforms = NULL;
  this->ecmTransforms = NULL;

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
  
  this->DaVinci->UpdateAllJointValues();
  this->DaVinci->PrintAllJointValues();
  this->DaVinci->UpdateAllKinematicsTransforms();
  this->DaVinci->PrintAllKinematicsTransforms();

  vtkSmartPointer<vtkMatrix4x4> tmpVtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  
  if(this->psm1BaseToWorld != NULL)
  {
    ISI_TRANSFORM* isiPsm1BaseToWorld = this->DaVinci->GetPsm1()->GetBaseToWorldTransform();
    ConvertIsiTransformToVtkMatrix(isiPsm1BaseToWorld, *tmpVtkMatrix);
    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->psm1BaseToWorld->GetFrameNumber() + 1 ;
    ToolTimeStampedUpdate(this->psm1BaseToWorld->GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp);
  }

  if (this->psm2BaseToWorld != NULL)
  {
    ISI_TRANSFORM* isiPsm2BaseToWorld = this->DaVinci->GetPsm2()->GetBaseToWorldTransform();
    ConvertIsiTransformToVtkMatrix(isiPsm2BaseToWorld, *tmpVtkMatrix);
    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->psm2BaseToWorld->GetFrameNumber() + 1;
    ToolTimeStampedUpdate(this->psm2BaseToWorld->GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp);
  }

  if (this->ecmBaseToWorld != NULL)
  {
    ISI_TRANSFORM* isiEcmBaseToWorld = this->DaVinci->GetEcm()->GetBaseToWorldTransform();
    ConvertIsiTransformToVtkMatrix(isiEcmBaseToWorld, *tmpVtkMatrix);
    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->ecmBaseToWorld->GetFrameNumber() + 1;
    ToolTimeStampedUpdate(this->ecmBaseToWorld->GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp);
  }

  if (this->psm1Transforms != NULL)
  {
    ISI_TRANSFORM* isiPsm1Transforms = this->DaVinci->GetPsm1()->GetTransforms();
    for (int iii = 0; iii < 7; iii++)
    {
      ConvertIsiTransformToVtkMatrix(isiPsm1Transforms + iii, *tmpVtkMatrix);
      unsigned long frameNumber = this->psm1Transforms[iii].GetFrameNumber() + 1;
      ToolTimeStampedUpdate(this->psm1Transforms[iii].GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp);
    }
  }

  if (this->psm2Transforms != NULL)
  {
    ISI_TRANSFORM* isiPsm2Transforms = this->DaVinci->GetPsm2()->GetTransforms();
    for (int iii = 0; iii < 7; iii++)
    {
      ConvertIsiTransformToVtkMatrix(isiPsm2Transforms + iii, *tmpVtkMatrix);
      unsigned long frameNumber = this->psm2Transforms[iii].GetFrameNumber() + 1;
      ToolTimeStampedUpdate(this->psm2Transforms[iii].GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp);
    }
  }

  if (this->ecmTransforms != NULL)
  {
    ISI_TRANSFORM* isiEcmTransforms = this->DaVinci->GetEcm()->GetTransforms();
    for (int iii = 0; iii < 7; iii++)
    {
      ConvertIsiTransformToVtkMatrix(isiEcmTransforms + iii, *tmpVtkMatrix);
      unsigned long frameNumber = this->ecmTransforms[iii].GetFrameNumber() + 1;
      ToolTimeStampedUpdate(this->ecmTransforms[iii].GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp);
    }
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
  XML_READ_STRING_ATTRIBUTE_WARNING(Psm1DhTable, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_WARNING(Psm2DhTable, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_WARNING(EcmDhTable, deviceConfig);

  PlusStatus status = SetDhTablesFromStrings();

  if (status != PLUS_SUCCESS)
  {
    LOG_ERROR("Check the formatting of the DH tables.");
    return status;
  }
	
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
static bool BothAreSpaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}

//----------------------------------------------------------------------------
void vtkPlusIntuitiveDaVinciTracker::ProcessDhString(std::string& str) const
{
	std::vector<std::string> strTokens;

	// Remove all the new lines from the string
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	// Remove all tabs 
	str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
	// Trim the beginning and end
	str = igsioCommon::Trim(str);

	// Remove all double/triple spaces
	std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
	str.erase(new_end, str.end());
}

static void ConvertTokenVectorToDhTable(std::vector<std::string>& srcTokenVector, ISI_DH_ROW* destIsiDhTable)
{
  for (int iii = 0; iii < 7; iii++)
  {
    try
    {
      destIsiDhTable[iii].type = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 0]);
      destIsiDhTable[iii].l = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 1]);
      destIsiDhTable[iii].sina = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 2]);
      destIsiDhTable[iii].cosa = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 3]);
      destIsiDhTable[iii].d = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 4]);
      destIsiDhTable[iii].sinq = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 5]);
      destIsiDhTable[iii].cosq = (ISI_FLOAT)std::stof(srcTokenVector[7 * iii + 6]);
    }
    catch (...)
    {
      LOG_ERROR("Check input DH table input in config file.");
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntuitiveDaVinciTracker::SetDhTablesFromStrings()
{
  std::vector<std::string> psm1TokenVector, psm2TokenVector, ecmTokenVector;

  const int numDhRows = 7; const int numDhCols = 7;
  int numElem = numDhRows*numDhCols;

  ProcessDhString(Psm1DhTable);
  ProcessDhString(Psm2DhTable);
  ProcessDhString(EcmDhTable);

  psm1TokenVector = igsioCommon::SplitStringIntoTokens(Psm1DhTable, ' ');
  psm2TokenVector = igsioCommon::SplitStringIntoTokens(Psm2DhTable, ' ');
  ecmTokenVector = igsioCommon::SplitStringIntoTokens(EcmDhTable, ' ');

  if((psm1TokenVector.size() != numElem) || 
	   (psm2TokenVector.size() != numElem) ||
	   (ecmTokenVector.size() != numElem))
  {
	  LOG_ERROR("Invalid formatting of DH table string. Must have " << numElem << "elements.");
	  return PLUS_FAIL;
  }

  ISI_DH_ROW isiPsm1DhTable[numDhRows];
  ISI_DH_ROW isiPsm2DhTable[numDhRows];
  ISI_DH_ROW isiEcmDhTable[numDhRows];

  ConvertTokenVectorToDhTable(psm1TokenVector, isiPsm1DhTable);
  ConvertTokenVectorToDhTable(psm2TokenVector, isiPsm2DhTable);
  ConvertTokenVectorToDhTable(ecmTokenVector, isiEcmDhTable);

  this->DaVinci->GetPsm1()->SetDhTable(isiPsm1DhTable);
  LOG_DEBUG("PSM1 DH Table set to: " << this->DaVinci->GetPsm1()->GetDhTableAsString());
  this->DaVinci->GetPsm2()->SetDhTable(isiPsm2DhTable);
  LOG_DEBUG("PSM2 DH Table set to: " << this->DaVinci->GetPsm2()->GetDhTableAsString());
  this->DaVinci->GetEcm()->SetDhTable(isiEcmDhTable);
  LOG_DEBUG("ECM DH Table set to: " << this->DaVinci->GetEcm()->GetDhTableAsString());

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
void vtkPlusIntuitiveDaVinciTracker::ConvertIsiTransformToVtkMatrix(ISI_TRANSFORM* srcIsiMatrix, vtkMatrix4x4& destVtkMatrix)
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