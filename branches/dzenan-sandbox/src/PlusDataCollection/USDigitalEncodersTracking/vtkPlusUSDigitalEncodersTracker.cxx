/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMath.h"
#include "vtkVector.h"

#include "SEIDrv32.h"
#include <sstream>

#include "vtkPlusUSDigitalEncodersTracker.h"

vtkStandardNewMacro( vtkPlusUSDigitalEncodersTracker );

class vtkPlusUSDigitalEncodersTracker::vtkPlusEncoderTrackingInfo
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
  vtkPlusUSDigitalEncodersTracker::vtkPlusEncoderTrackingInfo::vtkPlusEncoderTrackingInfo()
  {
    this->Encoder_PreTMatrix           = vtkSmartPointer<vtkMatrix4x4>::New();
    this->Encoder_TransformationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    this->Encoder_Persistent           = true;
  }

  /*! Destructor */
  virtual vtkPlusUSDigitalEncodersTracker::vtkPlusEncoderTrackingInfo::~vtkPlusEncoderTrackingInfo()
  {
  }
};

class vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncoderInfo
{
public:
  // ---------------------------------------------------------------------------
  // Public member variables ---------------------------------------------------
  vtkPlusUSDigitalEncodersTracker*         External;
  long                                     Encoder_Model;
  long                                     Encoder_SN;
  long                                     Encoder_SN2;
  long                                     Encoder_Version;
  long                                     Encoder_Addr;
  long                                     Encoder_Addr2;
  long                                     Encoder_Mode;
  long                                     Encoder_Resolution;
  bool                                     Encoder_Connected;
  int                                      Encoder_Motion; // 0 : Linear motion , 1: Rotation
  double                                   Encoder_PulseSpacing;
  double                                   Encoder_PulseSpacing2;
  vtkSmartPointer<vtkTransform>            Encoder_LocalTransform;
  vtkVector3d                              Encoder_LocalAxis;
  vtkVector3d                              Encoder_LocalAxis2;
  vtkPlusEncoderTrackingInfo               Encoder_TrackingInfo;

  // ---------------------------------------------------------------------------
  // Public member functions  --------------------------------------------------
  /*! Constructor */
  vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncoderInfo::vtkPlusUSDigitalEncoderInfo()
  {
    this->Encoder_Model                = 0;
    this->Encoder_SN                   = 0;
    this->Encoder_SN2                  = 0;
    this->Encoder_Version              = 0;
    this->Encoder_Addr                 = 0;
    this->Encoder_Addr2                = 0;
    // Supporting Modes:
    /*
    The mode is changed temporarily and will be effective until the encoder is
    reset, powered down, or another mode change command is received. It is
    not stored in the EEPROM. Mode byte as follows:

    |7|  6 |5|  4 |  3 |  2  | 1 | 0 |
    |0|/256|0|incr|size|multi|stb|rev|

    Reverse: rev = 1, the position increases counter clockwise.
             rev = 0, the position increases clockwise.
    Strobe:  stb = 1, the encoder operates in strobe mode: it waits for a strobe
                     request before reading the position; this mode is used to
                     synchronize multiple encoders. After entering this mode, wait
                     at least 2 msec before sending the first strobe command.
             stb = 0, the encoder operates in asynchronous
                     mode: it reads the position within 2 milliseconds and sends
                     the most current position when requested. The data can be
                     from 0 to 2 milliseconds old.
    Multi:   multi = 1, multi-turn mode: a 32 bit counter keeps track of the
                       position (it increases or decreases over multiple turns, i.e. 3 1/
                       2 turns at a resolution of 100 would be 350). This counter is
                       cleared at reset.
             multi = 0, single-turn mode: position is between zero and the max
                       resolution, according to the shaft angle.
                       Note: in older versions (V1.X), this bit indicated a fast mode
                       (3msec update rate) with a 9 bit accuracy.
                       Also, any other command besides position inquires can corrupt
                       the multi-turn position.
    Size: only effective in single-turn mode:
             size = 1: the encoder always sends the position in 2 bytes, even
                       if the resolution is 256 decimal or less.
             size = 0: the position is sent as 1 byte if the resolution is up to
                       256 decimal, or as 2 bytes if above 256 decimal.
                       In multi-turn mode, the position is always 4 bytes and this bit is
                       ignored.
    Incr: only effective in multi-turn mode:
             incr = 1: the encoder sends the position change since the last
                       request, as a 32 bit signed number.
             incr = 0: the encoder sends the position as a 32 bit signed
                       number.
    /256: only available for analog version,
          only effective in multi-turn mode:
             /256 = 1: the encoder position is divided by 256.
             /256 = 0: the encoder position is normal.
    */
    this->Encoder_Mode                 = 4;    // Defaul mode : Strobe (OFF), MultiTurn(On), Size(OFF), Incr (OFF), /256 (OFF)
    this->Encoder_Motion               = 0;
    this->Encoder_PulseSpacing         = 0.0;
    this->Encoder_PulseSpacing2        = 0.0;
    this->Encoder_Connected            = false;
    this->Encoder_Resolution           = 3600; // Default encoder's resolution (3600)
    this->Encoder_LocalTransform       = vtkSmartPointer<vtkTransform>::New();

    /// Do we need this variable
    this->External                     = NULL;
  }

  /*! Destructor */
  virtual vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncoderInfo::~vtkPlusUSDigitalEncoderInfo()
  {
    this->External = NULL;
  }

};

//-------------------------------------------------------------------------
vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncodersTracker()
{
  this->SetDeviceId( "TrackerDevice" );
  this->NumberOfEncoders                    = 0;
  this->COMPort                             = 0;
  this->BaudRate                            = 9600;
  this->USDigitalEncoderTransformRepository = vtkSmartPointer<vtkPlusTransformRepository>::New();
  this->USDigitalEncoderInfoList.clear();
  this->USDigitalEncoderTrackingInfoList.clear();

  //// No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates       = true;
  this->AcquisitionRate                     = 50;
  this->coreXY                              = false;
}

//-------------------------------------------------------------------------
vtkPlusUSDigitalEncodersTracker::~vtkPlusUSDigitalEncodersTracker()
{
  if ( this->Recording )
  {
    this->StopRecording();
  }
  for (auto it = USDigitalEncoderInfoList.begin(); it != USDigitalEncoderInfoList.end(); ++it)
  {
      delete (it->second);
      if (coreXY)
          break; //only one pointer was allocated
  }
}

//-------------------------------------------------------------------------
void vtkPlusUSDigitalEncodersTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalConnect()
{
  LOG_TRACE( "vtkPlusUSDigitalEncodersTracker::Connect" );

  // SEI Initialization.
  // Start the SEI Server Program, and look for devices on the SEI bus with
  // the zero COM port to look on all com ports, and the AUTOASSIGN means
  // that if there are address conflicts on the SEI bus, the device
  // addresses will automatically be reassigned so there are no conflicts
  // Initialization.
  long EncoderStatus = ::InitializeSEI( this->COMPort , REINITIALIZE | AUTOASSIGN | NORESET );
  if (EncoderStatus != 0)
  {
    LOG_ERROR( "Failed to initialize SEI! COMPort=" << this->COMPort );
    return PLUS_FAIL;
  }

  // Do we need to change the baudrate of the RS232C communication?
  /*if ( this->SetBaudRate(this->BaudRate) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set baud rate for SEI!");
    return PLUS_FAIL;
  }*/

  long numberofConnectedEncoders  = ::GetNumberOfDevices();
  EncoderInfoMapType::iterator encoderinfopos;
  for( long deviceID = 0 ; deviceID < numberofConnectedEncoders; ++deviceID )
  {
    // Support multiple US digital A2 encoders.
    long  lModel           = 0;
    long  lSerialNumber    = 0;
    long  lVersion         = 0;
    long  lAddress         = 0;
    long  retVal;

    if ( ::GetDeviceInfo( deviceID, &lModel, &lSerialNumber, &lVersion, &lAddress ) != 0 )
    {
      LOG_ERROR( "Failed to get SEI device info for device number: " << deviceID );
      return PLUS_FAIL;
    }

    encoderinfopos = this->USDigitalEncoderInfoList.find( lSerialNumber );

    if ( encoderinfopos == this->USDigitalEncoderInfoList.end() )
    {
      LOG_WARNING( "Unregistered encoder is detected" );
    }
    else
    {
        if (encoderinfopos->second->Encoder_SN == lSerialNumber)
        {
            encoderinfopos->second->Encoder_Connected = true;
            encoderinfopos->second->Encoder_Model = lModel;
            encoderinfopos->second->Encoder_Version = lVersion;
            encoderinfopos->second->Encoder_Addr = lAddress;
            retVal = ::A2SetMode(encoderinfopos->second->Encoder_Addr, encoderinfopos->second->Encoder_Mode);
            if (retVal != 0)
            {
                LOG_ERROR("Failed to set SEI device mode for device SN: " << lSerialNumber);
                return PLUS_FAIL;
            }
            retVal = ::A2SetPosition(encoderinfopos->second->Encoder_Addr, 0); // Initialize the value of the first encoder
            if (retVal != 0)
            {
                LOG_ERROR("Failed to set initial position for SEI device SN: " << lSerialNumber);
                return PLUS_FAIL;
            }
        }
        else //coreXY second encoder
        {
            encoderinfopos->second->Encoder_Connected = true;
            encoderinfopos->second->Encoder_Model = lModel;
            encoderinfopos->second->Encoder_Version = lVersion;
            encoderinfopos->second->Encoder_Addr2 = lAddress;
            retVal = ::A2SetMode(encoderinfopos->second->Encoder_Addr2, encoderinfopos->second->Encoder_Mode);
            if (retVal != 0)
            {
                LOG_ERROR("Failed to set SEI device mode for device SN: " << lSerialNumber);
                return PLUS_FAIL;
            }
            retVal = ::A2SetPosition(encoderinfopos->second->Encoder_Addr2, 0); // Initialize the value of the second encoder
            if (retVal != 0)
            {
                LOG_ERROR("Failed to set initial position for SEI device SN: " << lSerialNumber);
                return PLUS_FAIL;
            }
        }
    }

  }

  // Remove unconnected encoder info from the encoder info list.
  encoderinfopos = this->USDigitalEncoderInfoList.begin();
  while( encoderinfopos != this->USDigitalEncoderInfoList.end() )
  {
    if( !encoderinfopos->second->Encoder_Connected )
    {
      encoderinfopos = USDigitalEncoderInfoList.erase(encoderinfopos);
    }
    else
    {
      ++encoderinfopos;
    }
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkPlusUSDigitalEncodersTracker::Disconnect" );
  return this->StopRecording();
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::Probe()
{
  LOG_TRACE( "vtkPlusUSDigitalEncodersTracker::Probe" );

  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }

  if ( !this->Connect() )
  {
    LOG_ERROR( "Unable to connect to USDigital Encoders" );
    return PLUS_FAIL;
  }

  this->Disconnect();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalStartRecording()
{
  LOG_TRACE( "vtkPlusUSDigitalEncodersTracker::InternalStartRecording" );
  if ( this->IsRecording() )
  {
    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalStopRecording()
{
  LOG_TRACE( "vtkPlusUSDigitalEncodersTracker::InternalStopRecording" );

  if ( ::IsInitialized() != 1 )
  {
    // Device not yet initialized
    return PLUS_SUCCESS;
  }

  if ( ::CloseSEI() != 0 )
  {
    LOG_ERROR( "Failed to close SEI!" );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalUpdate()
{
  LOG_TRACE( "vtkPlusUSDigitalEncodersTracker::InternalUpdate" );

  if ( ! this->Recording )
  {
    LOG_ERROR( "called Update() when not tracking" );
    return PLUS_FAIL;
  }

  if (coreXY)
  {
    assert(USDigitalEncoderInfoList.size() == 2);
    vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncoderInfo & encoders =
      *this->USDigitalEncoderInfoList.begin()->second;
    if (!encoders.Encoder_Connected)
    {
      LOG_ERROR("USDigital encoder(s) not connected!");
      return PLUS_FAIL;
    }
    
    long Encoder_Value, errorCode;
    vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();

    // Read encoder positions and transform it into XY position in mm
    errorCode = ::A2GetPosition(encoders.Encoder_Addr, &Encoder_Value);
    if (errorCode)
    {
      LOG_ERROR("Unable to read position of first encoder with SN: " << encoders.Encoder_SN);
      return PLUS_FAIL;
    }
    double firstEnc = Encoder_Value * encoders.Encoder_PulseSpacing;

    errorCode = ::A2GetPosition(encoders.Encoder_Addr2, &Encoder_Value);
    if (errorCode)
    {
      LOG_ERROR("Unable to read position of second encoder with SN: " << encoders.Encoder_SN2);
      return PLUS_FAIL;
    }
    double secondEnc = Encoder_Value * encoders.Encoder_PulseSpacing2;

    double firstAxis = firstEnc + secondEnc;
    double secondAxis = firstEnc - secondEnc;

    //now make a transform matrix out of this translation and add it into PLUS system
    vtkVector3d localmovement = encoders.Encoder_LocalAxis;
    vtkMath::MultiplyScalar(localmovement.GetData(), firstAxis);
    tempTransform->Translate(localmovement.GetData());
    localmovement = encoders.Encoder_LocalAxis2;
    vtkMath::MultiplyScalar(localmovement.GetData(), secondAxis);
    tempTransform->Translate(localmovement.GetData());

    vtkMatrix4x4::Multiply4x4(encoders.Encoder_TrackingInfo.Encoder_PreTMatrix,
      tempTransform->GetMatrix(),
      encoders.Encoder_TrackingInfo.Encoder_TransformationMatrix);

    this->USDigitalEncoderTransformRepository->SetTransform(encoders.Encoder_TrackingInfo.Encoder_TransformName,
        encoders.Encoder_TrackingInfo.Encoder_TransformationMatrix);
    if (ToolTimeStampedUpdateWithvtkPlusEncoderTrackingInfo(encoders.Encoder_TrackingInfo) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to find tool on port: " << encoders.Encoder_TrackingInfo.Encoder_PortName);
    }
  }
  else //regular combination of stages
  {
      EncoderInfoMapType::iterator encoderinfopos;
      for (encoderinfopos = this->USDigitalEncoderInfoList.begin();
          encoderinfopos != this->USDigitalEncoderInfoList.end();
          ++encoderinfopos)
      {
          if (encoderinfopos->second->Encoder_Connected)
          {
              long Encoder_Value;
              // Get current encoder values from one connected US digital encoder
              ::A2GetPosition(encoderinfopos->second->Encoder_Addr, &Encoder_Value);

              // Update transformation matrix of the connected US digital encoder
              vtkVector3d localmovement = encoderinfopos->second->Encoder_LocalAxis;
              vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();

              if (encoderinfopos->second->Encoder_Motion == 0)
              {
                  vtkMath::MultiplyScalar(localmovement.GetData(),
                      Encoder_Value * encoderinfopos->second->Encoder_PulseSpacing);

                  tempTransform->Translate(localmovement.GetData());
              }
              else if (encoderinfopos->second->Encoder_Motion == 1)
              {
                  // Check the unit of rotation angle .... (degree or radian)
                  tempTransform->RotateWXYZ(Encoder_Value * encoderinfopos->second->Encoder_PulseSpacing,
                      localmovement.GetData());
              }
              else
              {
                  LOG_ERROR("Un-supported motion");
              }

              vtkMatrix4x4::Multiply4x4(encoderinfopos->second->Encoder_TrackingInfo.Encoder_PreTMatrix,
                  tempTransform->GetMatrix(),
                  encoderinfopos->second->Encoder_TrackingInfo.Encoder_TransformationMatrix);

              this->USDigitalEncoderTransformRepository->SetTransform(encoderinfopos->second->Encoder_TrackingInfo.Encoder_TransformName,
                  encoderinfopos->second->Encoder_TrackingInfo.Encoder_TransformationMatrix);
              if (ToolTimeStampedUpdateWithvtkPlusEncoderTrackingInfo(encoderinfopos->second->Encoder_TrackingInfo) == PLUS_FAIL)
              {
                  LOG_ERROR("Unable to find tool on port: " << encoderinfopos->second->Encoder_TrackingInfo.Encoder_PortName);
                  continue;
              }
          }
      }
  }
  
  for (unsigned int i = 0; i < USDigitalEncoderTrackingInfoList.size(); ++i)
  {
      if (!USDigitalEncoderTrackingInfoList[i].Encoder_Persistent)
      {
          this->USDigitalEncoderTransformRepository->GetTransform(USDigitalEncoderTrackingInfoList[i].Encoder_TransformName,
              USDigitalEncoderTrackingInfoList[i].Encoder_TransformationMatrix);
          if (ToolTimeStampedUpdateWithvtkPlusEncoderTrackingInfo(USDigitalEncoderTrackingInfoList[i]) == PLUS_FAIL)
          {
              LOG_ERROR("Unable to find tool on port: " << USDigitalEncoderTrackingInfoList[i].Encoder_PortName);
              continue;
          }
      }
  }  
  
  return PLUS_SUCCESS;
}

//---------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::ToolTimeStampedUpdateWithvtkPlusEncoderTrackingInfo( vtkPlusEncoderTrackingInfo& encoderTrackingInfo )
{
  if( !encoderTrackingInfo.Encoder_Persistent )
  {
    vtkPlusDataSource* tool = NULL;
    if ( this->GetToolByPortName( encoderTrackingInfo.Encoder_PortName.c_str(), tool ) != PLUS_SUCCESS )
    {
      LOG_ERROR( "Unable to find tool on port: " << encoderTrackingInfo.Encoder_PortName );
      return PLUS_FAIL;
    }

    // Devices has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber        = tool->GetFrameNumber() + 1 ;
    const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();
    this->ToolTimeStampedUpdate( tool->GetSourceId(),
                                 encoderTrackingInfo.Encoder_TransformationMatrix,
                                 TOOL_OK,
                                 frameNumber,
                                 unfilteredTimestamp );
  }
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING( deviceConfig, rootConfigElement );

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL( unsigned long, NumberOfEncoders, deviceConfig );

  XML_FIND_NESTED_ELEMENT_REQUIRED( dataSourcesElement, deviceConfig, "DataSources" );

  this->USDigitalEncoderTransformRepository->Clear();
  this->USDigitalEncoderInfoList.clear();
  this->USDigitalEncoderTrackingInfoList.clear();

  for ( int EncoderIndex = 0; EncoderIndex < dataSourcesElement->GetNumberOfNestedElements(); EncoderIndex++ )
  {
    vtkXMLDataElement* EncoderInfoElement = dataSourcesElement->GetNestedElement( EncoderIndex );
    if ( STRCASECMP( EncoderInfoElement->GetName(), "DataSource" ) != 0 )
    {
      // if this is not a data source element, skip it
      continue;
    }

    vtkPlusEncoderTrackingInfo encodertrackingInfo;

    // ---- Get PortName
    const char* portName = EncoderInfoElement->GetAttribute( "PortName" );
    if ( portName == NULL )
    {
      LOG_ERROR( "Cannot set sensor-specific parameters: tool portname is undefined" );
      continue;
    }

    vtkPlusDataSource* tool = NULL;
    if ( this->GetToolByPortName( portName, tool ) != PLUS_SUCCESS )
    {
      LOG_ERROR( "Cannot set sensor-specific parameters: tool " << portName << " was not found" );
      continue;
    }
    if ( tool == NULL )
    {
      LOG_ERROR( "Cannot set sensor-specific parameters: tool " << portName << " was not found" );
      continue;
    }

    // Reading the coreXY computation mode
    if (STRCASECMP(portName, "coreXY") == 0)
    {
        coreXY = true;
    }

    encodertrackingInfo.Encoder_PortName = portName;

    // ---- Get a name of trasnformation
    const char* fromAttribute = EncoderInfoElement->GetAttribute( "From" );
    const char* toAttribute = EncoderInfoElement->GetAttribute( "To" );

    if ( !fromAttribute || !toAttribute )
    {
      LOG_ERROR( "Failed to read transform of CoordinateDefinitions (nested element index: " << EncoderInfoElement << ") - check 'From' and 'To' attributes in the configuration file!" );
      continue;
    }

    PlusTransformName transformName( fromAttribute, toAttribute );
    if ( !transformName.IsValid() )
    {
      LOG_ERROR( "Invalid transform name (From: '" <<  fromAttribute << "'  To: '" << toAttribute << "')" );
      continue;
    }

    encodertrackingInfo.Encoder_TransformName = transformName;

    bool isPersistent = true;
    if( EncoderInfoElement->GetAttribute( "Persistent" ) ) // if it exists, then it is non-persistent
    {
      if ( STRCASECMP( EncoderInfoElement->GetAttribute( "Persistent" ), "FALSE" ) == 0 )
      {
        isPersistent = false;
      }
    }

    encodertrackingInfo.Encoder_Persistent    = isPersistent;
    if( this->USDigitalEncoderTransformRepository->IsExistingTransform( encodertrackingInfo.Encoder_TransformName ) != PLUS_SUCCESS )
    {
      this->USDigitalEncoderTransformRepository->SetTransform( encodertrackingInfo.Encoder_TransformName,
          encodertrackingInfo.Encoder_TransformationMatrix );
    }

    // ---- Get PreTMatrix:
    double vectorMatrix[16] = {0};
    if ( EncoderInfoElement->GetVectorAttribute( "PreTMatrix", 16, vectorMatrix ) )
    {
      encodertrackingInfo.Encoder_PreTMatrix->DeepCopy( vectorMatrix );
    }
    else
    {
      this->USDigitalEncoderTrackingInfoList.push_back( encodertrackingInfo );
      continue;
    }

    // Reading the serial number of an US Digital Encoder
    const char* sn = EncoderInfoElement->GetAttribute("SN");
    if (sn == NULL)
    {
        this->USDigitalEncoderTrackingInfoList.push_back(encodertrackingInfo);
        //LOG_ERROR("Cannot read the serial number of an US Digital Encoder");
        continue;
    }

    vtkPlusUSDigitalEncoderInfo * encoderinfoPointer = new vtkPlusUSDigitalEncoderInfo;
    vtkPlusUSDigitalEncoderInfo & encoderinfo = *encoderinfoPointer;
    encoderinfo.Encoder_TrackingInfo = encodertrackingInfo;
    encoderinfo.Encoder_SN = atol(sn);

    if (coreXY)
    {
        // Reading second (Y-direction) serial number of an US Digital Encoder
        const char* sn = EncoderInfoElement->GetAttribute("SN2");
        if (sn == NULL)
        {
            this->USDigitalEncoderTrackingInfoList.push_back(encodertrackingInfo);
            //LOG_ERROR("Cannot read the serial number of an US Digital Encoder");
            continue;
        }

        encoderinfo.Encoder_SN2 = atol(sn);
    }


    // Reading the MotionType of an US Digital Encoder
    std::string motiontype = EncoderInfoElement->GetAttribute( "MotionType" );
    if ( motiontype.empty() )
    {
      LOG_ERROR( "Cannot read the MotionType of an US Digital Encoder" );
      continue;
    }

    std::transform( motiontype.begin(), motiontype.end(), motiontype.begin(), ::tolower );
    std::cout << "Motion Type :: " << motiontype << std::endl;

    if( motiontype.find( "linear" ) != std::string::npos )
    {
      std::cout << "Motion Type :: " << motiontype << std::endl;
      encoderinfo.Encoder_Motion = 0;
    }
    else if( motiontype.find( "rotation" ) != std::string::npos )
    {
      std::cout << "Motion Type :: " << motiontype << std::endl;
      encoderinfo.Encoder_Motion = 1;
    }
    else
    {
      LOG_ERROR( "Cannot read the motion type of an US Digital Encoder" );
      continue;
    }

    // Reading the pulse spacing of an US Digital Encoder
    // Linear Motion : mm /pulses
    // Rotation      : rad/pulses
    const char* pulseSpacing = EncoderInfoElement->GetAttribute( "PulseSpacing" );
    if (pulseSpacing == NULL)
    {
      LOG_ERROR( "Cannot read the PulseSpacing of an US Digital Encoder" );
      continue;
    }
    encoderinfo.Encoder_PulseSpacing = atof( pulseSpacing );

    if (coreXY)
    {
        // Reading the pulse spacing of an US Digital Encoder
        // Linear Motion : mm /pulses
        // Rotation      : rad/pulses
        const char* pulseSpacing = EncoderInfoElement->GetAttribute("PulseSpacing2");
        if (sn == NULL)
        {
            LOG_ERROR("Cannot read the second PulseSpacing of an US Digital Encoder in coreXY mode");
            continue;
        }
        encoderinfo.Encoder_PulseSpacing2 = atof(pulseSpacing);
    }

    if (!EncoderInfoElement->GetVectorAttribute("LocalAxis", 3, encoderinfo.Encoder_LocalAxis.GetData()))
    {
        LOG_ERROR("Unable to find 'LocalAxis' attribute of an encoder in the configuration file");
        continue;
    }

    if (coreXY)
    {
        if (!EncoderInfoElement->GetVectorAttribute("LocalAxis2", 3, encoderinfo.Encoder_LocalAxis2.GetData()))
        {
            LOG_ERROR("Unable to find 'LocalAxis2' attribute of an encoder in the configuration file");
            continue;
        }
    }

    // Reading the mode of an US Digital Encoder
    const char* mode = EncoderInfoElement->GetAttribute( "Mode" );
    if ( mode == NULL )
    {
      LOG_ERROR( "Cannot read the Mode of an US Digital Encoder" );
      // Using the defulat mode (multi-turn )
      this->USDigitalEncoderInfoList[encoderinfo.Encoder_SN] = encoderinfoPointer;
      continue;
    }
    encoderinfo.Encoder_Mode = atol( mode );

    // Reading the resolution of an US Digital Encoder
    const char* resolution = EncoderInfoElement->GetAttribute( "Resolution" );
    if ( resolution == NULL )
    {
      LOG_ERROR( "Cannot read the Resolution of an US Digital Encoder" );
      // Using the defulat Resolution (3600 )
      this->USDigitalEncoderInfoList[encoderinfo.Encoder_SN] = encoderinfoPointer;
      continue;
    }
    encoderinfo.Encoder_Resolution = atol( resolution );

    // Build the list of US Digital Encoder Info
    this->USDigitalEncoderInfoList[encoderinfo.Encoder_SN] = encoderinfoPointer;
    if (coreXY) //enter this encoderinfo twice (once for each SN)
    {
        this->USDigitalEncoderInfoList[encoderinfo.Encoder_SN2] = encoderinfoPointer;
        this->NumberOfEncoders = 2;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::WriteConfiguration( vtkXMLDataElement* rootConfigElement )
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

PlusStatus vtkPlusUSDigitalEncodersTracker::IsStepperAlive()
{
  long r = ::IsInitialized();
  if ( r != 1 )
  {
    // Device not yet initialized
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersStrobeMode()
{
  if( ::A2SetStrobe() != 0 )
  {
    LOG_ERROR( "Failed to set US digital A2 Encodrs as Strobe mode." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersSleep()
{
  if( ::A2SetSleep() != 0 )
  {
    LOG_ERROR( "Failed to set US digital A2 Encodrs as Sleep mode." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersWakeup()
{
  if( ::A2SetWakeup() != 0 )
  {
    LOG_ERROR( "Failed to set US digital A2 Encodrs as Wakeup mode." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderOriginWithAddr( long address )
{
  if( ::A2SetOrigin( address ) != 0 )
  {
    LOG_ERROR( "Failed to set US digital A2 Encoder's origin point as current position." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderOriginWithSN( long sn )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return SetUSDigitalA2EncoderOriginWithAddr( address );
  }
  else
  {
    LOG_ERROR( "Failed to set US digital A2 Encoder's origin point as current position." );
    return PLUS_FAIL;
  }
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetAllUSDigitalA2EncoderOrigin()
{
  EncoderInfoMapType::iterator it;
  for( it = this->USDigitalEncoderInfoList.begin(); it != this->USDigitalEncoderInfoList.end(); ++it )
  {
    if( this->SetUSDigitalA2EncoderOriginWithAddr( it->second->Encoder_Addr ) == PLUS_FAIL )
    {
      return PLUS_FAIL;
    }

    if (coreXY)
    {
        if (this->SetUSDigitalA2EncoderOriginWithAddr( it->second->Encoder_Addr2 ) == PLUS_FAIL)
        {
            return PLUS_FAIL;
        }
        //break; //only 2 encoders
    }
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderModeWithAddr( long address, long mode )
{
  if( ::A2SetMode( address, mode ) != 0 )
  {
    LOG_ERROR( "Failed to set the mode of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderModeWithSN( long sn, long mode )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return SetUSDigitalA2EncoderModeWithAddr( address, mode );
  }
  else
  {
    LOG_ERROR( "Failed to set the mode of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
}

PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderModeWithAddr( long address, long* mode )
{
  if( ::A2GetMode( address, mode ) != 0 )
  {
    LOG_ERROR( "Failed to get the mode of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderModeWithSN( long sn, long* mode )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return GetUSDigitalA2EncoderModeWithAddr( address, mode );
  }
  else
  {
    LOG_ERROR( "Failed to get the mode of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderResoultionWithAddr( long address, long res )
{
  if( ::A2SetResolution( address, res ) != 0 )
  {
    LOG_ERROR( "Failed to set the resoultion of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderResoultionWithSN( long sn, long res )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return SetUSDigitalA2EncoderResoultionWithAddr( address, res );
  }
  else
  {
    LOG_ERROR( "Failed to set the resoultion of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
}

PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderResoultionWithAddr( long address, long* res )
{
  if( ::A2GetResolution( address, res ) != 0 )
  {
    LOG_ERROR( "Failed to get the resoultion of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderResoultionWithSN( long sn, long* res )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return GetUSDigitalA2EncoderResoultionWithAddr( address, res );
  }
  else
  {
    LOG_ERROR( "Failed to get the resoultion of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderPositionWithAddr( long address, long pos )
{
  if( ::A2SetPosition( address, pos ) != 0 )
  {
    LOG_ERROR( "Failed to set the position of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderPositionWithSN( long sn, long pos )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return SetUSDigitalA2EncoderPositionWithAddr( address, pos );
  }
  else
  {
    LOG_ERROR( "Failed to set the position of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
}


PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderPositionWithAddr( long address, long* pos )
{
  if( ::A2GetPosition( address, pos ) != 0 )
  {
    LOG_ERROR( "Failed to get the position of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}


PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderPositionWithSN( long sn, long* pos )
{
  long address = this->GetUSDigitalA2EncoderAddressWithSN( sn );
  if( address > -1 )
  {
    return GetUSDigitalA2EncoderPositionWithAddr( address, pos );
  }
  else
  {
    LOG_ERROR( "Failed to get the position of an US digital A2 Encodr." );
    return PLUS_FAIL;
  }
}

long vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderAddressWithSN( long sn )
{
  EncoderInfoMapType::iterator it;
  it = this->USDigitalEncoderInfoList.find( sn );
  if( it == this->USDigitalEncoderInfoList.end() )
  {
    LOG_ERROR( "Cannot find the Address of an US Digital encoder with its SN number." );
    return -1;
  }
  else if (it->second->Encoder_SN == sn)
  {
    return it->second->Encoder_Addr;
  }
  else
  {
    assert(it->second->Encoder_SN2 == sn);
    return it->second->Encoder_Addr2;
  }
}
