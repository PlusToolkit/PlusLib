/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusConfigure.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMath.h"

#include "SEIDrv32.h"
#include <sstream>

#include "vtkUSDigitalEncodersTracker.h"

vtkStandardNewMacro(vtkUSDigitalEncodersTracker);

class vtkUSDigitalEncodersTracker::vtkEncoderTrackingInfo
{
public:
  // ---------------------------------------------------------------------------
  // Public member variables ---------------------------------------------------
  vtkSmartPointer<vtkMatrix4x4>        Encoder_PreTMatrix;
  vtkSmartPointer<vtkMatrix4x4>        Encoder_TransformationMatrix;
  PlusTransformName                    Encoder_TransformName;
  std::string                          Encoder_PortName;
  bool                                 Encoder_Persistent;

  // ---------------------------------------------------------------------------
  // Public member functions  --------------------------------------------------
  /*! Constructor */
  vtkUSDigitalEncodersTracker::vtkEncoderTrackingInfo::vtkEncoderTrackingInfo()
  {
    this->Encoder_PreTMatrix           = vtkSmartPointer<vtkMatrix4x4>::New();
    this->Encoder_TransformationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    this->Encoder_Persistent           = true;
  }

  /*! Destructor */
  virtual vtkUSDigitalEncodersTracker::vtkEncoderTrackingInfo::~vtkEncoderTrackingInfo()
  {
  }
};

class vtkUSDigitalEncodersTracker::vtkUSDigitalEncoderInfo
{
public:
  // ---------------------------------------------------------------------------
  // Public member variables ---------------------------------------------------
  vtkUSDigitalEncodersTracker *        External;
  long                                 Encoder_Model;
  long                                 Encoder_SN;
  long                                 Encoder_Version;
  long                                 Encoder_Addr;
  bool                                 Encoder_Connected;
  int                                  Encoder_Motion; // 0 : Linear motion , 1: Rotation
  double                               Encoder_PulseSpacing; 
  long                                 Encoder_Value;
  vtkSmartPointer<vtkTransform>        Encoder_LocalTransform;
  double                               Encoder_LocalAxis[3];
  vtkEncoderTrackingInfo               Encoder_TrackingInfo;

  // ---------------------------------------------------------------------------
  // Public member functions  --------------------------------------------------
  /*! Constructor */
  vtkUSDigitalEncodersTracker::vtkUSDigitalEncoderInfo::vtkUSDigitalEncoderInfo()
  {
    this->Encoder_Model                = 0;
    this->Encoder_SN                   = 0;
    this->Encoder_Version              = 0;
    this->Encoder_Addr                 = 0;
    this->Encoder_Motion               = 0;
    this->Encoder_PulseSpacing         = 0.0f;
    this->Encoder_Connected            = false;
    this->Encoder_LocalTransform       = vtkSmartPointer<vtkTransform>::New();

    /// Do we need this variable
    this->External                     = NULL;
  }

  /*! Destructor */
  virtual vtkUSDigitalEncodersTracker::vtkUSDigitalEncoderInfo::~vtkUSDigitalEncoderInfo()
  {
    this->External = NULL;
  }

  void vtkUSDigitalEncodersTracker::vtkUSDigitalEncoderInfo::GetLocalAxis(double localaxis[3])
  {
    localaxis[0] = this->Encoder_LocalAxis[0];
    localaxis[1] = this->Encoder_LocalAxis[1];
    localaxis[2] = this->Encoder_LocalAxis[2];
  }

};

//-------------------------------------------------------------------------
vtkUSDigitalEncodersTracker::vtkUSDigitalEncodersTracker()
{
  this->SetDeviceId("TrackerDevice");
  this->NumberOfEncoders                    = 0;
  this->COMPort                             = 0;
  this->BaudRate                            = 9600;
  this->USDigitalEncoderTransformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  this->USDigitalEncoderInfoList.clear();
  this->USDigitalEncoderTrackingInfoList.clear();
  
  //// No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates       = true;
  this->AcquisitionRate                     = 50;
}

//-------------------------------------------------------------------------
vtkUSDigitalEncodersTracker::~vtkUSDigitalEncodersTracker() 
{
  if ( this->Recording )
  {
    this->StopRecording();
  }
}

//-------------------------------------------------------------------------
void vtkUSDigitalEncodersTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::InternalConnect()
{
  LOG_TRACE("vtkUSDigitalEncodersTracker::Connect" );

  // SEI Initialization.
  // Start the SEI Server Program, and look for devices on the SEI bus with
  // the zero COM port to look on all com ports, and the AUTOASSIGN means
  // that if there are address conflicts on the SEI bus, the device
  // addresses will automatically be reassigned so there are no conflicts
  // Initialization.
  if ( this->IsStepperAlive() != PLUS_SUCCESS )
  {
    if ( ::InitializeSEI(this->COMPort , REINITIALIZE | AUTOASSIGN | NORESET ) != 0 )
    {
      LOG_ERROR("Failed to initialize SEI! COMPort="<<this->COMPort); 
      return PLUS_FAIL; 
    }

    // Do we need to change the baudrate of the RS232C communication?
    /*if ( this->SetBaudRate(this->BaudRate) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set baud rate for SEI!"); 
      return PLUS_FAIL; 
    }*/
  }
  else
  {
    LOG_DEBUG("No need to initialize SEI, it's already connected!"); 
  }

  long numberofConnectedEncoders  = ::GetNumberOfDevices();
  EncoderInfoMapType::iterator encoderinfopos;
  for( long deviceID = 0 ; deviceID < numberofConnectedEncoders; ++deviceID )
  {
    // Support multiple US digital A2 encoders.
    long  lModel           = 0;
    long  lSerialNumber    = 0;
    long  lVersion         = 0;
    long  lAddress         = 0;
    
    if ( ::GetDeviceInfo(deviceID, &lModel, &lSerialNumber, &lVersion, &lAddress) != 0 )
    {
      LOG_ERROR("Failed to get SEI device info for device number: " << deviceID); 
      return PLUS_FAIL; 
    }


    encoderinfopos = this->USDigitalEncoderInfoList.find( lSerialNumber ); 

    if ( encoderinfopos == this->USDigitalEncoderInfoList.end() )
    {
      LOG_WARNING("Unregistered encoder is detected");
    }
    else
    {
      encoderinfopos->second.Encoder_Connected  = true;
      encoderinfopos->second.Encoder_Model      = lModel;
      encoderinfopos->second.Encoder_Version    = lVersion;
      encoderinfopos->second.Encoder_Addr       = lAddress;
    }
  }

  // Remove unconnected encoder info from the encoder info list.
  for(encoderinfopos  = this->USDigitalEncoderInfoList.begin();
      encoderinfopos != this->USDigitalEncoderInfoList.end();
      ++encoderinfopos)
  {
    if(!encoderinfopos->second.Encoder_Connected)
    {
      USDigitalEncoderInfoList.erase(encoderinfopos);
    }
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkUSDigitalEncodersTracker::Disconnect" );
  return this->StopRecording();
}

//-------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::Probe()
{
  LOG_TRACE( "vtkUSDigitalEncodersTracker::Probe" );

  if (this->Recording)
  {
    return PLUS_SUCCESS;
  }

  if ( !this->Connect() )
  {
    LOG_ERROR("Unable to connect to USDigital Encoders");
    return PLUS_FAIL;
  }

  this->Disconnect();

  return PLUS_SUCCESS;
} 

//-------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::InternalStartRecording()
{
  LOG_TRACE( "vtkUSDigitalEncodersTracker::InternalStartRecording" ); 
  if ( this->IsRecording() )
  {
    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::InternalStopRecording()
{
  LOG_TRACE( "vtkUSDigitalEncodersTracker::InternalStopRecording" ); 

  if ( ::IsInitialized() != 1 )
  {
    // Device not yet initialized
    return PLUS_SUCCESS; 
  }

  if ( ::CloseSEI() != 0 )
  {
    LOG_ERROR("Failed to close SEI!"); 
    return PLUS_FAIL; 
  }
  
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::InternalUpdate()
{
  LOG_TRACE( "vtkUSDigitalEncodersTracker::InternalUpdate" ); 

  if ( ! this->Recording )
  {
    LOG_ERROR("called Update() when not tracking" );
    return PLUS_FAIL;
  }

  EncoderInfoMapType::iterator encoderinfopos;
  for(encoderinfopos  = this->USDigitalEncoderInfoList.begin();
      encoderinfopos != this->USDigitalEncoderInfoList.end();
      ++encoderinfopos)
  {
    if(encoderinfopos->second.Encoder_Connected)
    {
      // Get current encoder values from one connected US digital encoder
      ::A2GetPosition( encoderinfopos->second.Encoder_Addr,
                       &encoderinfopos->second.Encoder_Value);

      // Update transformation matrix of the connected US digital encoder
      double localmovement[3];
      encoderinfopos->second.GetLocalAxis(localmovement);
      vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();

      if( encoderinfopos->second.Encoder_Motion == 0)
      {
        vtkMath::MultiplyScalar(localmovement, 
                                encoderinfopos->second.Encoder_Value*encoderinfopos->second.Encoder_PulseSpacing);
        
        tempTransform->Translate(localmovement);
      }
      else if ( encoderinfopos->second.Encoder_Motion == 1)
      {
        // Check the unit of rotation angle .... (degree or radian)
        tempTransform->RotateWXYZ(encoderinfopos->second.Encoder_Value*encoderinfopos->second.Encoder_PulseSpacing,
                                  localmovement);
      }
      else
      {
        LOG_ERROR("Un-supported motion");
      }

      vtkMatrix4x4::Multiply4x4(encoderinfopos->second.Encoder_TrackingInfo.Encoder_PreTMatrix,
                                tempTransform->GetMatrix(),
                                encoderinfopos->second.Encoder_TrackingInfo.Encoder_TransformationMatrix);
     
      this->USDigitalEncoderTransformRepository->SetTransform(encoderinfopos->second.Encoder_TrackingInfo.Encoder_TransformName,
                                                              encoderinfopos->second.Encoder_TrackingInfo.Encoder_TransformationMatrix);
      if(ToolTimeStampedUpdateWithvtkEncoderTrackingInfo(encoderinfopos->second.Encoder_TrackingInfo) == PLUS_FAIL)
      {
        LOG_ERROR("Unable to find tool on port: " << encoderinfopos->second.Encoder_TrackingInfo.Encoder_PortName ); 
        continue;
      }
    }
  }

  for(int i=0; i< USDigitalEncoderTrackingInfoList.size(); ++i)
  {
    if(!USDigitalEncoderTrackingInfoList[i].Encoder_Persistent)
    {
      this->USDigitalEncoderTransformRepository->GetTransform(USDigitalEncoderTrackingInfoList[i].Encoder_TransformName,
                                                              USDigitalEncoderTrackingInfoList[i].Encoder_TransformationMatrix);
      if(ToolTimeStampedUpdateWithvtkEncoderTrackingInfo(USDigitalEncoderTrackingInfoList[i]) == PLUS_FAIL)
      {
        LOG_ERROR("Unable to find tool on port: " << encoderinfopos->second.Encoder_TrackingInfo.Encoder_PortName ); 
        continue;
      }
    }
  }

  return PLUS_SUCCESS; 
}

//---------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::ToolTimeStampedUpdateWithvtkEncoderTrackingInfo(vtkEncoderTrackingInfo& encoderTrackingInfo)
{
  if(!encoderTrackingInfo.Encoder_Persistent)
  {
    vtkPlusDataSource* tool = NULL;
    if ( this->GetToolByPortName(encoderTrackingInfo.Encoder_PortName.c_str(), tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to find tool on port: " << encoderTrackingInfo.Encoder_PortName ); 
      return PLUS_FAIL; 
    }

    // Devices has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber        = tool->GetFrameNumber() + 1 ;
    const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
    this->ToolTimeStampedUpdate( tool->GetSourceId(),
                                 encoderTrackingInfo.Encoder_TransformationMatrix,
                                 TOOL_OK,
                                 frameNumber,
                                 unfilteredTimestamp);
  }
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, NumberOfEncoders, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  this->USDigitalEncoderTransformRepository->Clear();
  this->USDigitalEncoderInfoList.clear();
  this->USDigitalEncoderTrackingInfoList.clear();

  for ( int EncoderIndex = 0; EncoderIndex < dataSourcesElement->GetNumberOfNestedElements(); EncoderIndex++ )
  {
    vtkXMLDataElement* EncoderInfoElement = dataSourcesElement->GetNestedElement(EncoderIndex); 
    if ( STRCASECMP(EncoderInfoElement->GetName(), "DataSource") != 0 )
    {
      // if this is not a data source element, skip it
      continue; 
    }

    vtkEncoderTrackingInfo encodertrackingInfo;

    // ---- Get PortName
    const char* portName = EncoderInfoElement->GetAttribute("PortName");
    if ( portName==NULL )
    {
      LOG_ERROR("Cannot set sensor-specific parameters: tool portname is undefined");
      continue;
    }
    vtkPlusDataSource* tool = NULL;
    if ( this->GetToolByPortName(portName, tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Cannot set sensor-specific parameters: tool "<<portName<<" was not found");
      continue;
    }
    if ( tool==NULL )
    {
      LOG_ERROR("Cannot set sensor-specific parameters: tool "<<portName<<" was not found");
      continue;
    }

    encodertrackingInfo.Encoder_PortName = portName;
    
    // ---- Get a name of trasnformation
    const char* fromAttribute = EncoderInfoElement->GetAttribute("From"); 
    const char* toAttribute = EncoderInfoElement->GetAttribute("To"); 

    if ( !fromAttribute || !toAttribute )
    {
      LOG_ERROR("Failed to read transform of CoordinateDefinitions (nested element index: " << EncoderInfoElement << ") - check 'From' and 'To' attributes in the configuration file!"); 
      continue; 
    }

    PlusTransformName transformName(fromAttribute, toAttribute); 
    if ( !transformName.IsValid() )
    {
      LOG_ERROR("Invalid transform name (From: '" <<  fromAttribute << "'  To: '" << toAttribute << "')"); 
      continue;  
    }

    encodertrackingInfo.Encoder_TransformName = transformName;

    bool isPersistent = true;
    if(EncoderInfoElement->GetAttribute("Persistent")) // if it exists, then it is non-persistent
    {
      if (STRCASECMP(EncoderInfoElement->GetAttribute("Persistent"),"FALSE")==0)
      {
        isPersistent = false;
      }
    }

    encodertrackingInfo.Encoder_Persistent    = isPersistent;
    this->USDigitalEncoderTransformRepository->SetTransform(encodertrackingInfo.Encoder_TransformName,
                                                            encodertrackingInfo.Encoder_TransformationMatrix); 

    // ---- Get PreTMatrix:
    double vectorMatrix[16]={0}; 
    if ( EncoderInfoElement->GetVectorAttribute("PreTMatrix", 16, vectorMatrix) )
    {
      encodertrackingInfo.Encoder_PreTMatrix->DeepCopy(vectorMatrix); 
    }
    else
    {
      this->USDigitalEncoderTrackingInfoList.push_back(encodertrackingInfo);
      continue; 
    }

    // Reading the serial number of an US Digital Encoder
    const char* sn = EncoderInfoElement->GetAttribute("SN");
    if ( sn == NULL )
    {
      this->USDigitalEncoderTrackingInfoList.push_back(encodertrackingInfo);
      //LOG_ERROR("Cannot read the serial number of an US Digital Encoder");
      continue;
    }
    
    vtkUSDigitalEncoderInfo encoderinfo;
    encoderinfo.Encoder_TrackingInfo = 	encodertrackingInfo;
    encoderinfo.Encoder_SN = atol(sn);

    // Reading the serial number of an US Digital Encoder
    std::string motiontype = EncoderInfoElement->GetAttribute("MotionType");
    if ( motiontype.empty() )
    {
      LOG_ERROR("Cannot read the serial number of an US Digital Encoder");
      continue;
    }
    
    std::transform(motiontype.begin(), motiontype.end(), motiontype.begin(), ::tolower);
    std::cout << "Motion Type :: " << motiontype << std::endl;

    if(motiontype.find("linear") != std::string::npos)
    {
      std::cout << "Motion Type :: " << motiontype << std::endl;
      encoderinfo.Encoder_Motion = 0;
    }
    else if(motiontype.find("rotation") != std::string::npos)
    {
      std::cout << "Motion Type :: " << motiontype << std::endl;
      encoderinfo.Encoder_Motion = 1;
    }
    else
    {
      LOG_ERROR("Cannot read the motion type of an US Digital Encoder");
      continue;
    }

    // Reading the pulse spacing of an US Digital Encoder
    // Linear Motion : mm /pulses
    // Rotation      : rad/pulses
    const char* pulseSpacing = EncoderInfoElement->GetAttribute("PulseSpacing");
    if ( sn == NULL )
    {
      LOG_ERROR("Cannot read the serial number of an US Digital Encoder");
      continue;
    }
    encoderinfo.Encoder_PulseSpacing = atof(pulseSpacing);

    

    double localAxis[3]={0}; 
    if ( !EncoderInfoElement->GetVectorAttribute("LocalAxis", 3, encoderinfo.Encoder_LocalAxis) )
    {
      LOG_ERROR("Unable to find 'TMatrix' attribute of an encoder in the configuration file");  
      continue; 
    }

    // Build the list of US Digital Encoder Info
    this->USDigitalEncoderInfoList[encoderinfo.Encoder_SN] = encoderinfo;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUSDigitalEncodersTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  /*XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  trackerConfig->SetIntAttribute("FilterAcWideNotch", this->GetFilterAcWideNotch()); 
  trackerConfig->SetIntAttribute("FilterAcNarrowNotch", this->GetFilterAcNarrowNotch()); 
  trackerConfig->SetDoubleAttribute("FilterDcAdaptive", this->GetFilterDcAdaptive()); 
  trackerConfig->SetIntAttribute("FilterLargeChange", this->GetFilterLargeChange()); 
  trackerConfig->SetIntAttribute("FilterAlpha", (this->GetFilterAlpha()?1:0));
*/
  return PLUS_SUCCESS;
}

PlusStatus vtkUSDigitalEncodersTracker::IsStepperAlive()
{
  long r = ::IsInitialized();
  if ( ::IsInitialized() != 1 )
  {
    // Device not yet initialized
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}