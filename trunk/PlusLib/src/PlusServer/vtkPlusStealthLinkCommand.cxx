/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "StealthLink\vtkStealthLinkTracker.h"

#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusStealthLinkCommand.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h"
#include "vtkVolumeReconstructor.h"
#include "vtkVirtualVolumeReconstructor.h"


#define UNDEFINED_VALUE DBL_MAX

static const int MAX_NUMBER_OF_FRAMES_ADDED_PER_EXECUTE=50;
static const double SKIPPED_PERIOD_REPORTING_THRESHOLD_SEC=0.2; // log a warning if volume reconstruction cannot keep up with the acquisition and skips more than this time period of acquired frames

static const char GET_STEALTHLINK_EXAM_DATA_CMD[]="ExamData";
static const char GET_STEALTHLINK_REGISTRATION_DATA_CMD[]="RegistrationData";

vtkStandardNewMacro( vtkPlusStealthLinkCommand );

//----------------------------------------------------------------------------
vtkPlusStealthLinkCommand::vtkPlusStealthLinkCommand()
: PatientName(NULL)
, PatientId(NULL)
, StealthLinkDeviceId(NULL)
,DicomImagesOutputDirectory(NULL)
{
	this->FrameToExamTransform = vtkSmartPointer<vtkMatrix4x4>::New();
}

//----------------------------------------------------------------------------
vtkPlusStealthLinkCommand::~vtkPlusStealthLinkCommand()
{
  SetPatientName(NULL);
  SetStealthLinkDeviceId(NULL);
  SetPatientId(NULL);
  SetDicomImagesOutputDirectory(NULL);
  FrameToExamTransform->Identity();
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(GET_STEALTHLINK_EXAM_DATA_CMD);
  cmdNames.push_back(GET_STEALTHLINK_REGISTRATION_DATA_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusStealthLinkCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, GET_STEALTHLINK_EXAM_DATA_CMD))
  {
    desc+=GET_STEALTHLINK_EXAM_DATA_CMD;
    desc+=": Acquire the exam data from the StealthLink Server. The exam data contains the image being displayed on the StealthLink Server. The 3D volume will be constructed using these images";
  }
  if (commandName==NULL || STRCASECMP(commandName, GET_STEALTHLINK_REGISTRATION_DATA_CMD))
  {
    desc+=GET_STEALTHLINK_REGISTRATION_DATA_CMD;
    desc+="Acquire the registration data from the StealthLink Server. The data contains the transformation matrix between the image being displayed and the reference frame.";
  }
  return desc;
}
//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::SetNameToGetExam() { SetName(GET_STEALTHLINK_EXAM_DATA_CMD); }
//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::SetNameToGetRegistration() { SetName(GET_STEALTHLINK_REGISTRATION_DATA_CMD); }
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
	
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if(!aConfig->GetAttribute("DicomImagesOutputDirectory"))
  {
    LOG_TRACE("Output file directory for the dicom images is not specified. Default value C:/StealthLinkDicomOutput will be used");
	SetDicomImagesOutputDirectory("C:/StealthLinkDicomOutput");
  }
  else
  {
    SetDicomImagesOutputDirectory(aConfig->GetAttribute("DicomImagesOutputDirectory"));
  }
  if(aConfig->GetAttribute("StealthLinkDeviceId"))
  {
	  SetStealthLinkDeviceId(aConfig->GetAttribute("StealthLinkDeviceId"));
  }
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->StealthLinkDeviceId!=NULL)
  {
    aConfig->SetAttribute("StealthLinkDeviceId",this->StealthLinkDeviceId);     
  }
   if (this->DicomImagesOutputDirectory!=NULL)
  {
    aConfig->SetAttribute("DicomImagesOutputDirectory",this->DicomImagesOutputDirectory);     
  }
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
bool IsMatrixIdentityMatrix(vtkMatrix4x4* matrix)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(i==j)
			{
				if(matrix->GetElement(i,j)!=1)
					return false;
			}
			else
			{
				if(matrix->GetElement(i,j)!=0)
					return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::Execute()
{  
  LOG_DEBUG("vtkPlusStealthLinkCommand::Execute: "<<(this->Name?this->Name:"(undefined)")
    <<", device: "<<(this->StealthLinkDeviceId==NULL?"(undefined)":this->StealthLinkDeviceId) );
  //std::cout << "ID= " << this->StealthLinkDeviceId << "\n";
  if (this->Name==NULL)
  {
    this->ResponseMessage="StealthLink command failed, no command name specified";
    return PLUS_FAIL;
  }

  vtkStealthLinkTracker* stealthLinkDevice = GetStealthLinkDevice();
  if (stealthLinkDevice==NULL)
  {
    this->ResponseMessage=std::string("StealthLink command failed: device ")
      +(this->StealthLinkDeviceId==NULL?"(undefined)":this->StealthLinkDeviceId)+" is not found";
    return PLUS_FAIL;
  }

  if (STRCASECMP(this->Name, GET_STEALTHLINK_EXAM_DATA_CMD)==0)
  {
    LOG_INFO("Acquiring the exam data from StealthLink Server: Device ID: "<<GetStealthLinkDeviceId());
	if(!(stealthLinkDevice->UpdateCurrentExam()))
	{
       return PLUS_FAIL;
    }
	std::string examImageDirectory;
	if(!stealthLinkDevice->GetDicomImage(this->GetDicomImagesOutputDirectory(),examImageDirectory))
	{
		return PLUS_FAIL;
	}

	std::string patientName;
	std::string patientId;
	if(!stealthLinkDevice->GetPatientName(patientName))
	{
		return PLUS_FAIL;
	}
	if(!stealthLinkDevice->GetPatientId(patientId))
	{
		return PLUS_FAIL;
	}
	SetPatientName(patientName.c_str());
	SetPatientId(patientId.c_str());

	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName(examImageDirectory.c_str()); 
	reader->Update();
	float*  iDirectionVector_LPS = reader->GetImageOrientationPatient();  // TODO change ijkToImage
	float*  jDirectionVector_LPS = reader->GetImageOrientationPatient()+3;
	float*  ijkOrigin_LPS = reader->GetImagePositionPatient();
	double* ijkVectorMagnitude_LPS = reader->GetPixelSpacing();

	float   kDirectionVector_LPS[3]={0};
	vtkMath::Cross(iDirectionVector_LPS,jDirectionVector_LPS,kDirectionVector_LPS);

	vtkSmartPointer<vtkMatrix4x4> ijkToLPSTransformationMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); // image to lps
	for(int i=0;i<3;i++)
	{
		ijkToLPSTransformationMatrix->SetElement(0,i,ijkVectorMagnitude_LPS[i] * iDirectionVector_LPS[i]);
		ijkToLPSTransformationMatrix->SetElement(1,i,ijkVectorMagnitude_LPS[i] * jDirectionVector_LPS[i]);
		ijkToLPSTransformationMatrix->SetElement(2,i,ijkVectorMagnitude_LPS[i] * kDirectionVector_LPS[i]);
		ijkToLPSTransformationMatrix->SetElement(i,3,ijkOrigin_LPS[i]);
	}

	stealthLinkDevice->SetImageToLpsTransformationMatrix(ijkToLPSTransformationMatrix);
	vtkImageData* volumeToSend = reader->GetOutput();
	volumeToSend->SetSpacing(1,1,1);
	volumeToSend->SetOrigin(0,0,0);
	return ProcessImageReply(volumeToSend,ijkToLPSTransformationMatrix); 
  }
  else if (STRCASECMP(this->Name, GET_STEALTHLINK_REGISTRATION_DATA_CMD)==0)
  {    
    LOG_INFO("Acquiring the registration data from StealthLink Server: Device ID: "<<GetStealthLinkDeviceId());
	
	//if(stealthLinkDevice->UpdateCurrentRegistration()) TODO - Update is protected now so change the name of the function used here
	//{	
	//	return PLUS_FAIL;
	//}
    this->ResponseMessage="Acquiring the registration data from StealthLink Server completed";
    return PLUS_SUCCESS;
  }
  this->ResponseMessage=std::string("vtkPlusStealthLinkCommand::Execute: failed, unknown command name ")+this->Name;
  return PLUS_FAIL;
} 
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::ProcessImageReply(vtkImageData* volumeToSend,vtkMatrix4x4* imageToReferenceOrientationMatrixWithSpacing)
{
  LOG_DEBUG("Send image to client through OpenIGTLink");
  this->ResponseImageDeviceName=std::string("Stealth_") + GetPatientName();
  SetResponseImage(volumeToSend);
  //SetResponseImageToReferenceTransform(imageToReferenceOrientationMatrixWithSpacing);
  LOG_INFO("Send reconstructed volume to client through OpenIGTLink");
  this->ResponseMessage+=std::string(", image sent as: ")+this->ResponseImageDeviceName;
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
vtkStealthLinkTracker* vtkPlusStealthLinkCommand::GetStealthLinkDevice()
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    return NULL;
  }
  vtkStealthLinkTracker *stealthLinkDevice=NULL;
  if (GetStealthLinkDeviceId()!=NULL)
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device=NULL;
    if (dataCollector->GetDevice(device, GetStealthLinkDeviceId())!=PLUS_SUCCESS)
    {
      LOG_ERROR("No StealthLink device has been found by the name "<<this->GetStealthLinkDeviceId());
      return NULL;
    }
    // device found
    stealthLinkDevice = vtkStealthLinkTracker::SafeDownCast(device);
    if (stealthLinkDevice==NULL)
    {
      // wrong type
      LOG_ERROR("The specified device "<<GetStealthLinkDeviceId()<<" is not StealthLink Device");
      return NULL;
    }
  }
  else
  {
    // No volume reconstruction device id is specified, auto-detect the first one and use that
    for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
    {
	  stealthLinkDevice = vtkStealthLinkTracker::SafeDownCast(*it);
	 // StealthLinkDevice = static_cast<vtkStealthLinkTracker*>(*it);
      if (stealthLinkDevice!=NULL)
      {      
        // found a recording device
		SetStealthLinkDeviceId(stealthLinkDevice->GetDeviceId());
        break;
      }
    }
    if (stealthLinkDevice==NULL)
    {
      LOG_ERROR("No StealthLink Device has been found");
      return NULL;
    }
  }  
  return stealthLinkDevice;
}
