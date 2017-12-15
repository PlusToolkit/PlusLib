/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.
Authors include:
Danielle Pace (Robarts Research Institute, The University of Western Ontario)
Siddharth Vikal (Queen's University, Kingston, Ontario, Canada)
Adam Rankin (Robarts Research Institute, The University of Western Ontario)
Andras Lasso (Queen's University, Kingston, Ontario, Canada)
=========================================================================*/

#include "PlusConfigure.h"

#include "ImagingModes.h" // Ulterius imaging modes
#include "ulterius_def.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusSonixVideoSource.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtksys/SystemTools.hxx"
#include <ctype.h>
#include <string>
#include <vector>

//----------------------------------------------------------------------------

vtkPlusSonixVideoSource* vtkPlusSonixVideoSource::ActiveSonixDevice = NULL;

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusSonixVideoSource);

//----------------------------------------------------------------------------

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
  #pragma warning ( disable : 4311 )
  #pragma warning ( disable : 4312 )
  #define vtkGetWindowLong GetWindowLongPtr
  #define vtkSetWindowLong SetWindowLongPtr
  #define vtkGWL_USERDATA GWLP_USERDATA
#else // regular Visual studio
  #define vtkGetWindowLong GetWindowLong
  #define vtkSetWindowLong SetWindowLong
  #define vtkGWL_USERDATA GWL_USERDATA
#endif //

static const int CONNECT_RETRY = 5;
static const int CONNECT_RETRY_DELAY_SEC = 1.0;

//----------------------------------------------------------------------------
vtkPlusSonixVideoSource::vtkPlusSonixVideoSource()
  : Superclass()
  , Ult(new ulterius)
  , AcquisitionDataType(udtBPost)
  , ImagingMode(BMode)
  , OutputFormat(-1)
  , CompressionStatus(0)
  , Timeout(-1)
  , ConnectionSetupDelayMs(3000)
  , SharedMemoryStatus(0)
  , RfAcquisitionMode(RF_ACQ_RF_ONLY)
  , ImageGeometryChanged(false)
  , SonixIP(NULL)
  , UlteriusConnected(false)
  , AutoClipEnabled(false)
  , ImageGeometryOutputEnabled(false)
{
  this->SetSonixIP("127.0.0.1");
  this->StartThreadForInternalUpdates = false;

  this->RequireImageOrientationInConfiguration = true;

  // This effectively forces only one vtkPlusSonixVideoSource at a time, but it paves the way
  // for a non-singleton architecture when the SDK supports it
  if (vtkPlusSonixVideoSource::ActiveSonixDevice != NULL)
  {
    LOG_WARNING("There is already an active vtkPlusSonixVideoSource device. Ultrasonix SDK only supports one connection at a time, so the existing device is now deactivated and the newly created class is activated instead.");
  }
  vtkPlusSonixVideoSource::ActiveSonixDevice = this;
}

//----------------------------------------------------------------------------
vtkPlusSonixVideoSource::~vtkPlusSonixVideoSource()
{
  vtkPlusSonixVideoSource::ActiveSonixDevice = NULL;

  this->SetSonixIP(NULL);
  delete this->Ult;
  this->Ult = NULL;
}

//----------------------------------------------------------------------------
std::string vtkPlusSonixVideoSource::GetSdkVersion()
{
  std::ostringstream version;
  version << "UltrasonixSDK-" << PLUS_ULTRASONIX_SDK_MAJOR_VERSION << "." << PLUS_ULTRASONIX_SDK_MINOR_VERSION << "." << PLUS_ULTRASONIX_SDK_PATCH_VERSION;
  return version.str();
}

//----------------------------------------------------------------------------
void vtkPlusSonixVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkPlusSonixVideoSource::vtkPlusSonixVideoSourceNewFrameCallback(void* data, int type, int sz, bool cine, int frmnum)
{
  if (data == NULL || sz == 0)
  {
    LOG_DEBUG("vtkPlusSonixVideoSourceNewFrameCallback: called without data");
    return false;
  }

  if (vtkPlusSonixVideoSource::ActiveSonixDevice != NULL)
  {
    vtkPlusSonixVideoSource::ActiveSonixDevice->AddFrameToBuffer(data, type, sz, cine, frmnum);
  }
  else
  {
    LOG_ERROR("vtkPlusSonixVideoSource data callback but the ActiveSonixDevice is NULL. Disconnect between device and SDK.");
    return false;
  }

  return true;;
}

//----------------------------------------------------------------------------
// the callback when parameters change on a device
bool vtkPlusSonixVideoSource::vtkPlusSonixVideoSourceParamCallback(void* paramId, int ptX, int ptY)
{
  char* paramName = (char*)paramId;

  if (vtkPlusSonixVideoSource::ActiveSonixDevice == NULL)
  {
    LOG_ERROR("vtkPlusSonixVideoSource data callback but the ActiveSonixDevice is NULL. Disconnect between device and SDK.");
    return false;
  }

  if (STRCASECMP(paramName, "b-depth") == 0)
  {
    // we cannot query parameter values here, so just set a flag
    // and get the value when getting the frame
    vtkPlusSonixVideoSource::ActiveSonixDevice->ImageGeometryChanged = true;
    return true;
  }
  else if (STRCASECMP(paramName, "probe id") == 0)
  {
    char probeName[200] = {0};
    if (!vtkPlusSonixVideoSource::ActiveSonixDevice->Ult->getActiveProbe(probeName, 200))
    {
      LOG_ERROR("Unable to retrieve probe name: " << vtkPlusSonixVideoSource::ActiveSonixDevice->GetLastUlteriusError());
      return false;
    }
    std::string probeString(probeName);
    return true;
  }
  else if (STRCASECMP(paramName, "") == 0)
  {
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkPlusSonixVideoSource::AddFrameToBuffer(void* dataPtr, int type, int sz, bool cine, int frmnum)
{
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 4)
  LOG_ERROR("Minimum required Ultasonix SDK version is 5.x");
  return PLUS_FAIL;
#endif

  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  std::vector<vtkPlusDataSource*> sources;
  PlusCommon::VTKScalarPixelType pixelType = VTK_VOID;
  US_IMAGE_TYPE imgType = US_IMG_TYPE_XX;

  if ((uData)type == udtBPost && this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources) == PLUS_SUCCESS)
  {
    pixelType = VTK_UNSIGNED_CHAR;
    imgType = US_IMG_BRIGHTNESS;
  }
  else if ((uData)type == udtRF && this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, sources) == PLUS_SUCCESS)
  {
    pixelType = VTK_SHORT;
    imgType = US_IMG_RF_I_LINE_Q_LINE;
  }
  else
  {
    LOG_ERROR("Received data type \'" << type << "\' is unsupported. Please report this to the PLUS team.");
    return PLUS_FAIL;
  }

  // use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
  // expected data type
  this->FrameNumber = frmnum;

  vtkPlusDataSource* aSource = sources[0];

  unsigned int frameSize[3] = {0, 0, 0};
  aSource->GetInputFrameSize(frameSize);
  int frameBufferBytesPerPixel = aSource->GetNumberOfBytesPerPixel();
  const unsigned int frameSizeInBytes = frameSize[0] * frameSize[1] * frameBufferBytesPerPixel;

  // for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
  int numberOfBytesToSkip = 0;
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 5) || (PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 5 && PLUS_ULTRASONIX_SDK_MINOR_VERSION < 7)
  if ((type == udtBPre) || (type == udtRF)
      || (type == udtMPre) || (type == udtPWRF)
      || (type == udtColorRF)
     )
  {
    numberOfBytesToSkip = 4;
  }
#else
  // It's Ulterius 5.7 or newer.
  // RF images don't have a 4-byte header. It's possible that none of the types have the 4-byte header anymore.
  if ((type == udtBPre)
      || (type == udtMPre) || (type == udtPWRF)
      || (type == udtColorRF)
     )
  {
    numberOfBytesToSkip = 4;
  }
#endif

  if (sz != frameSizeInBytes + numberOfBytesToSkip)
  {
    // This typically occurs when Plus with Ultrasonix SDK 5.7.x is used with Exam software 6.0.7 or later
    LOG_ERROR("Received frame size (" << sz << " bytes) doesn't match the buffer size (" << frameSizeInBytes + numberOfBytesToSkip << " bytes). Make sure the Ultrasonix SDK version used in Plus (" << GetSdkVersion() << ") is compatible with the Exam software running on the ultrasound device.");
    return PLUS_FAIL;
  }

  if (this->ImageGeometryChanged)
  {
    this->ImageGeometryChanged = false;
    int currentDepth = -1;
    if (!vtkPlusSonixVideoSource::ActiveSonixDevice->Ult->getParamValue("b-depth", currentDepth))
    {
      LOG_WARNING("Failed to retrieve b-depth parameter");
    }
    uPoint currentPixelSpacingMicron;
    currentPixelSpacingMicron.x = -1;
    currentPixelSpacingMicron.y = -1;
    if (!vtkPlusSonixVideoSource::ActiveSonixDevice->Ult->getParamValue("microns", currentPixelSpacingMicron))
    {
      LOG_WARNING("Failed to retrieve bb-microns parameter");
    }
    uPoint currentTransducerOriginPixels;
    currentTransducerOriginPixels.x = -1;
    currentTransducerOriginPixels.y = -1;
    if (!vtkPlusSonixVideoSource::ActiveSonixDevice->Ult->getParamValue("origin", currentTransducerOriginPixels))
    {
      LOG_WARNING("Failed to retrieve bb-origin parameter");
    }

    this->ImagingParameters->SetDepthMm(currentDepth);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DEPTH, false);
    //TODO: Trigger parameter changed command

    this->CurrentPixelSpacingMm[0] = 0.001 * currentPixelSpacingMicron.x;
    this->CurrentPixelSpacingMm[1] = 0.001 * currentPixelSpacingMicron.y;
    // Use the mean spacing as Z spacing to minimize out-of-plane distortion
    // when using this information for transforming non-planar objects
    this->CurrentPixelSpacingMm[2] = (this->CurrentPixelSpacingMm[0] + this->CurrentPixelSpacingMm[1]) / 2.0;

    int* clipRectangleOrigin = aSource->GetClipRectangleOrigin();
    this->CurrentTransducerOriginPixels[0] = currentTransducerOriginPixels.x - clipRectangleOrigin[0];
    this->CurrentTransducerOriginPixels[1] = currentTransducerOriginPixels.y - clipRectangleOrigin[1];
    this->CurrentTransducerOriginPixels[2] = 0;
  }

  PlusTrackedFrame::FieldMapType customFields;

  if (this->ImageGeometryOutputEnabled)
  {
    std::ostringstream depthStr;
    depthStr << this->ImagingParameters->GetDepthMm();
    customFields["DepthMm"] = depthStr.str();

    std::ostringstream pixelSpacingStr;
    pixelSpacingStr << this->CurrentPixelSpacingMm[0] << " " << this->CurrentPixelSpacingMm[1];
    customFields["PixelSpacingMm"] = pixelSpacingStr.str();

    std::ostringstream transducerOriginStr;
    transducerOriginStr << this->CurrentTransducerOriginPixels[0] << " " << this->CurrentTransducerOriginPixels[1];
    // TODO : OpenIGTLink v3 will eliminate this short form necessity
    customFields["TransducerOriginPix"] = transducerOriginStr.str(); // "TransducerOriginPixels" would be over the 20-char limit of OpenIGTLink device name
  }

  // get the pointer to actual incoming data on to a local pointer
  unsigned char* deviceDataPtr = static_cast<unsigned char*>(dataPtr);

  PlusStatus status = this->AddVideoItemToVideoSources(sources, deviceDataPtr, aSource->GetInputImageOrientation(), frameSize, pixelType, 1, imgType, numberOfBytesToSkip, this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &customFields);
  this->Modified();

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::InternalConnect()
{
  this->Ult->setMessaging(false); // don't print messages on stdout

  int requestedImagingDataType = 0;
  if (GetRequestedImagingDataTypeFromSources(requestedImagingDataType) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect to sonix video device");
    return PLUS_FAIL;
  }

  switch (requestedImagingDataType)
  {
    case vtkPlusUsImagingParameters::DataTypeBPost:
      LOG_DEBUG("Imaging mode set: BMode");
      this->ImagingMode = BMode;
      this->RfAcquisitionMode = RF_ACQ_B_ONLY;
      this->AcquisitionDataType = udtBPost;
      break;
    case vtkPlusUsImagingParameters::DataTypeRF:
      LOG_DEBUG("Imaging mode set: RfMode");
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
      this->ImagingMode = RfMode;
#else
      this->ImagingMode = 0; // RfMode is always enabled for SDK 6.x
#endif
      this->RfAcquisitionMode = RF_ACQ_RF_ONLY;
      this->AcquisitionDataType = udtRF;
      break;
    case (vtkPlusUsImagingParameters::DataTypeBPost+vtkPlusUsImagingParameters::DataTypeRF):
      LOG_DEBUG("Imaging mode set: BAndRfMode");
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
      this->ImagingMode = RfMode;
#else
      this->ImagingMode = BMode; // RfMode is always enabled for SDK 6.x
#endif
      this->RfAcquisitionMode = RF_ACQ_B_AND_RF;
      this->AcquisitionDataType = udtBPost | udtRF;
      break;
    default:
      LOG_ERROR("Unsupported imaging data types are requested: " << requestedImagingDataType);
      return PLUS_FAIL;
  }

  // Connect to device. Sometimes it just fails so try to make it more robust by retrying
  // the connection a few times.
  int connectionTried = 0;
  bool initializationCompleted = false;
  while (!initializationCompleted)
  {
    if (this->UlteriusConnected)
    {
      // a previous connection setup attempt failed after connection has been made, so
      // disconnect before trying to connect again
      this->Ult->setDataToAcquire(0); // without this Ulterius 5.x may crash
      this->Ult->disconnect();
      vtkPlusAccurateTimer::Delay(0.5); // without this Ulterius 6.x may crash
      this->UlteriusConnected = false;
    }
    if (connectionTried > 0)
    {
      // this is a connection retry attempt
      if (connectionTried >= CONNECT_RETRY)
      {
        LOG_ERROR("Failed to connect to sonix video device");
        return PLUS_FAIL;
      }
      LOG_DEBUG("Failed to connect to sonix video device, retry (" << connectionTried << ")");
      vtkPlusAccurateTimer::Delay(CONNECT_RETRY_DELAY_SEC);
    }
    connectionTried++;

    if (this->SonixIP == NULL)
    {
      LOG_ERROR("Sonix host IP address is undefined");
      continue;
    }

    // Connect to the imaging device
    if (!this->Ult->connect(this->SonixIP))
    {
      LOG_ERROR("Initialize: couldn't connect to Ultrasonix at " << this->SonixIP << " address (error message: " << GetLastUlteriusError() << ")");
      continue;
    }
    this->UlteriusConnected = true;

    // Set the imaging mode
    if (SetImagingModeDevice(this->ImagingMode) != PLUS_SUCCESS) { continue; }

    // We need to wait for a little while before the mode actually gets selected
    vtkPlusAccurateTimer::Delay(0.001 * this->ConnectionSetupDelayMs);

    // Double-check to see if the mode has actually been set
    int actualImagingMode = -1;
    if (GetImagingModeDevice(actualImagingMode) != PLUS_SUCCESS)
    {
      LOG_ERROR("Initialize: Cannot check actual imaging mode");
      continue;
    }
    if (this->ImagingMode != actualImagingMode)
    {
      LOG_ERROR("Initialize: Requested imaging mode could not be selected (requested: " << this->ImagingMode << ", actual: " << actualImagingMode);
      continue;
    }

    // Set up imaging parameters
    // Parameter value <0 means that the parameter should be kept unchanged
    if (this->ImagingParameters->GetFrequencyMhz() >= 0 && this->SetFrequencyDevice(this->ImagingParameters->GetFrequencyMhz()) != PLUS_SUCCESS) { continue; }
    if (this->ImagingParameters->GetDepthMm() >= 0 && this->SetDepthDevice(this->ImagingParameters->GetDepthMm()) != PLUS_SUCCESS) { continue; }
    if (this->ImagingParameters->GetSectorPercent() >= 0 && this->SetSectorDevice(this->ImagingParameters->GetSectorPercent()) != PLUS_SUCCESS) { continue; }
    if (this->ImagingParameters->GetGainPercent() >= 0 && this->SetGainDevice(this->ImagingParameters->GetGainPercent()) != PLUS_SUCCESS) { continue; }
    if (this->ImagingParameters->GetDynRangeDb() >= 0 && this->SetDynRangeDevice(this->ImagingParameters->GetDynRangeDb()) != PLUS_SUCCESS) { continue; }
    if (this->ImagingParameters->GetZoomFactor() >= 0 && this->SetZoomDevice(this->ImagingParameters->GetZoomFactor()) != PLUS_SUCCESS) { continue; }
    if (this->CompressionStatus >= 0 && this->SetCompressionStatus(this->CompressionStatus) != PLUS_SUCCESS) { continue; }
    if (this->ImagingParameters->GetSoundVelocity() > 0 && this->SetSoundVelocityDevice(this->ImagingParameters->GetSoundVelocity()) != PLUS_SUCCESS) { continue; }

    if (this->AcquisitionRate <= 0)
    {
      // AcquisitionRate has not been specified, set it to match the frame rate
      int aFrameRate = 10;
      if (this->Ult->getParamValue("frame rate", aFrameRate))
      {
        this->AcquisitionRate = aFrameRate;
      }
    }

    Ult->setSharedMemoryStatus(this->SharedMemoryStatus);

#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
    // RF acquisition mode is always enabled on Ultrasonix SDK 6.x and above, so we only need to change it if it's an earlier SDK version
    if (this->ImagingMode == RfMode)
    {
      if (this->SetRfAcquisitionModeDevice(this->RfAcquisitionMode) != PLUS_SUCCESS)
      {
        LOG_ERROR("Initialize: Cannot set RF acquisition mode");
        continue;
      }
      RfAcquisitionModeType actualRfAcquisitionMode = RF_UNKNOWN;
      if (GetRfAcquisitionModeDevice(actualRfAcquisitionMode) != PLUS_SUCCESS)
      {
        LOG_ERROR("Initialize: Cannot check actual RF acquisition mode");
        continue;
      }
      if (this->RfAcquisitionMode != actualRfAcquisitionMode)
      {
        LOG_ERROR("Initialize: Requested RF acquisition mode could not be selected (requested: " << this->RfAcquisitionMode << ", actual: " << actualRfAcquisitionMode);
        continue;
      }
    }
#endif

    // Wait for the depth change to take effect before calling ConfigureVideoSource()
    // (it is especially important if auto clipping is enabled because then we need accurate frame size)
    Sleep(1000);

    // Configure video sources
    if (this->WantDataType(udtBPost))
    {
      if (!this->HasDataType(udtBPost))
      {
        // This typically occurs when Plus with Ultrasonix SDK 6.1.0 is used with Exam software 6.0.2
        LOG_ERROR("No B-mode data is available. Make sure the Ultrasonix SDK version used in Plus (" << GetSdkVersion() << ") is compatible with the Exam software running on the ultrasound device.");
        continue;
      }
      if (this->ConfigureVideoSource(udtBPost) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to configure B-mode video source");
        continue;
      }
    }
    if (this->WantDataType(udtRF))
    {
      if (!this->HasDataType(udtRF))
      {
        LOG_ERROR("No Rf-mode data is available");
        continue;
      }
      if (this->ConfigureVideoSource(udtRF) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to configure Rf-mode video source");
        continue;
      }
    }

    // Actually request data, now that its available
    if (!this->Ult->setDataToAcquire(this->AcquisitionDataType))
    {
      LOG_ERROR("Setting AcquisitionDataType failed: couldn't request the data aquisition type " << this->AcquisitionDataType << ", " << GetLastUlteriusError());
      return PLUS_FAIL;
    }

    // Set callback and timeout for receiving new frames
    this->Ult->setCallback(vtkPlusSonixVideoSourceNewFrameCallback);
    if (this->Timeout >= 0 && this->SetTimeout(this->Timeout) != PLUS_SUCCESS)
    {
      continue;
    }

    // Set the param callback so we can observe depth and plane changes
    this->Ult->setParamCallback(vtkPlusSonixVideoSourceParamCallback);

    initializationCompleted = true;
    this->ImageGeometryChanged = true; // trigger an initial update of geometry info
  } // while (!initializationCompleted)

  LOG_DEBUG("Successfully connected to sonix video device");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::InternalDisconnect()
{
  this->UlteriusConnected = false;
  this->Ult->setDataToAcquire(0); // without this Ulterius 5.x may crash
  this->Ult->disconnect();
  vtkPlusAccurateTimer::Delay(0.5); // without this Ulterius 6.x may crash
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::InternalStartRecording()
{
  // Unfreeze
  if (this->Ult->getFreezeState())
  {
    this->Ult->toggleFreeze();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::InternalStopRecording()
{
  // Freeze
  if (!this->Ult->getFreezeState())
  {
    this->Ult->toggleFreeze();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSonixVideoSource::ReadConfiguration");

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootConfigElement);
  if (deviceConfig == NULL)
  {
    LOG_ERROR("Unable to continue configuration of " << this->GetClassName() << ". Could not find corresponding element.");
    return PLUS_FAIL;
  }

  const char* ipAddress = deviceConfig->GetAttribute("IP");
  if (ipAddress != NULL)
  {
    this->SetSonixIP(ipAddress);
    LOG_DEBUG("Sonix Video IP: " << ipAddress);
  }
  else
  {
    LOG_WARNING("Ultrasonix IP address is not defined. Defaulting to " << this->GetSonixIP());
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(AutoClipEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ImageGeometryOutputEnabled, deviceConfig);

  if (Superclass::ReadConfiguration(rootConfigElement) != PLUS_SUCCESS)
  {
    int depth;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "Depth", depth, deviceConfig);
    this->ImagingParameters->SetDepthMm(depth);
    int tgc[8];
    XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 8, "TimeGainCompensation", tgc, deviceConfig);
    std::vector<double> tgcVec(tgc, tgc + 8);
    this->ImagingParameters->SetTimeGainCompensation(tgcVec);
    int sector;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "Sector", sector, deviceConfig);
    this->ImagingParameters->SetSectorPercent(sector);
    int gain;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "Gain", gain, deviceConfig);
    this->ImagingParameters->SetGainPercent(gain);
    int dynRange;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "DynRange", dynRange, deviceConfig);
    this->ImagingParameters->SetDynRangeDb(dynRange);
    int zoom;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "Zoom", zoom, deviceConfig);
    this->ImagingParameters->SetZoomFactor(zoom);
    int freq;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "Frequency", freq, deviceConfig);
    this->ImagingParameters->SetFrequencyMhz(freq);
    int soundVel;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, "SoundVelocity", soundVel, deviceConfig);
    this->ImagingParameters->SetSoundVelocity(soundVel);
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, CompressionStatus, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SharedMemoryStatus, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Timeout, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ConnectionSetupDelayMs, deviceConfig);

  if (this->RequestImagingParameterChange() == PLUS_FAIL)
  {
    LOG_ERROR("Failed to change imaging parameters in the device");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfig);

  if (this->ImagingMode == BMode)
  {
    deviceConfig->SetAttribute("ImagingMode", "BMode");
  }
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6) // RF acquisition mode is not supported on Ultrasonix SDK 6.x and above - see #489 in https://plustoolkit.github.io/legacytickets
  else if (this->ImagingMode == RfMode)
  {
    deviceConfig->SetAttribute("ImagingMode", "RfMode");
  }
#endif
  else
  {
    LOG_ERROR("Saving of unsupported ImagingMode requested!");
  }

  deviceConfig->SetAttribute("IP", this->SonixIP);
  deviceConfig->SetIntAttribute("CompressionStatus", this->CompressionStatus);
  deviceConfig->SetIntAttribute("Timeout", this->Timeout);
  deviceConfig->SetDoubleAttribute("ConnectionSetupDelayMs", this->ConnectionSetupDelayMs);

  XML_WRITE_BOOL_ATTRIBUTE(AutoClipEnabled, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(ImageGeometryOutputEnabled, deviceConfig);

  Superclass::WriteConfiguration(deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusSonixVideoSource::GetLastUlteriusError()
{
  const unsigned int MAX_ULTERIUS_ERROR_MSG_LENGTH = 256;
  char err[MAX_ULTERIUS_ERROR_MSG_LENGTH + 1];
  err[MAX_ULTERIUS_ERROR_MSG_LENGTH] = 0; // make sure the string is null-terminated
  this->Ult->getLastError(err, MAX_ULTERIUS_ERROR_MSG_LENGTH);

  return err;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetParamValueDevice(char* paramId, int paramValue, int& validatedParamValue)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    validatedParamValue = paramValue;
    return PLUS_SUCCESS;
  }
  if (!this->Ult->setParamValue(paramId, paramValue))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetParamValue failed (paramId=" << paramId << ", paramValue=" << paramValue << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  validatedParamValue = paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetParamValueDevice(char* paramId, Plus_uTGC& paramValue, Plus_uTGC& validatedParamValue)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    validatedParamValue = paramValue;
    return PLUS_SUCCESS;
  }
  if (!this->Ult->setParamValue(paramId, paramValue))
  {
    std::stringstream ss;
    ss << paramValue.v1 << ", " << paramValue.v2 << ", " << paramValue.v3 << ", " << paramValue.v4 << ", " << paramValue.v5 << ", " <<
       paramValue.v6 << ", " << paramValue.v7 << ", " << paramValue.v8;
    LOG_ERROR("vtkPlusSonixVideoSource::SetParamValue failed (paramId=" << paramId << ", paramValue=" << ss.str() << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  validatedParamValue = paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetParamValueDevice(char* paramId, int& paramValue, int& validatedParamValue)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Returned the cached value.
    paramValue = validatedParamValue;
    return PLUS_SUCCESS;
  }
  paramValue = -1;
  if (!this->Ult->getParamValue(paramId, paramValue))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::GetParamValue failed (paramId=" << paramId << ", paramValue=" << paramValue << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  validatedParamValue = paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetParamValueDevice(char* paramId, Plus_uTGC& paramValue, Plus_uTGC& validatedParamValue)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Returned the cached value.
    paramValue = validatedParamValue;
    return PLUS_SUCCESS;
  }
  if (!this->Ult->getParamValue(paramId, paramValue))
  {
    std::stringstream ss;
    ss << paramValue.v1 << ", " << paramValue.v2 << ", " << paramValue.v3 << ", " << paramValue.v4 << ", " << paramValue.v5 << ", " <<
       paramValue.v6 << ", " << paramValue.v7 << ", " << paramValue.v8;
    LOG_ERROR("vtkPlusSonixVideoSource::GetParamValue failed (paramId=" << paramId << ", paramValue=" << ss.str() << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  validatedParamValue = paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetFrequencyDevice(int aFrequency)
{
  PlusStatus result = SetParamValueDevice("b-freq", aFrequency, aFrequency);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetFrequencyMhz(aFrequency);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_FREQUENCY, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetFrequencyDevice(int& aFrequency)
{
  PlusStatus result = GetParamValueDevice("b-freq", aFrequency, aFrequency);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetFrequencyMhz(aFrequency);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_FREQUENCY, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetDepthDevice(int aDepth)
{
  PlusStatus result = SetParamValueDevice("b-depth", aDepth, aDepth);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetDepthMm(aDepth);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DEPTH, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetDepthDevice(int& aDepth)
{
  PlusStatus result = GetParamValueDevice("b-depth", aDepth, aDepth);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetDepthMm(aDepth);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DEPTH, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetGainDevice(int aGain)
{
  PlusStatus result = SetParamValueDevice("b-gain", aGain, aGain);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetGainPercent(aGain);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_GAIN, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetGainDevice(int& aGain)
{
  PlusStatus result = GetParamValueDevice("b-gain", aGain, aGain);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetGainPercent(aGain);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_GAIN, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetDynRangeDevice(int aDynRange)
{
  PlusStatus result = SetParamValueDevice("b-dynamic range", aDynRange, aDynRange);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetDynRangeDb(aDynRange);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DYNRANGE, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetDynRangeDevice(int& aDynRange)
{
  PlusStatus result = GetParamValueDevice("b-dynamic range", aDynRange, aDynRange);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetDynRangeDb(aDynRange);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DYNRANGE, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetZoomDevice(int aZoom)
{
  PlusStatus result = SetParamValueDevice("b-initial zoom", aZoom, aZoom);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetZoomFactor(aZoom);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_ZOOM, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetZoomDevice(int& aZoom)
{
  PlusStatus result = GetParamValueDevice("b-initial zoom", aZoom, aZoom);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetZoomFactor(aZoom);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_ZOOM, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetSectorDevice(int aSector)
{
  PlusStatus result = SetParamValueDevice("sector", aSector, aSector);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetSectorPercent(aSector);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_SECTOR, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetSectorDevice(int& aSector)
{
  PlusStatus result = GetParamValueDevice("sector", aSector, aSector);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetSectorPercent(aSector);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_SECTOR, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetTimeGainCompensationDevice(int tgc[8])
{
  Plus_uTGC tgcStruct;
  tgcStruct.v1 = tgc[0];
  tgcStruct.v2 = tgc[1];
  tgcStruct.v3 = tgc[2];
  tgcStruct.v4 = tgc[3];
  tgcStruct.v5 = tgc[4];
  tgcStruct.v6 = tgc[5];
  tgcStruct.v7 = tgc[6];
  tgcStruct.v8 = tgc[7];
  // todo validate this parameter name
  PlusStatus result = this->SetParamValueDevice("b-tgc", tgcStruct, tgcStruct);
  if (result == PLUS_SUCCESS)
  {
    std::vector<int> vec = tgcStruct.toVector();
    std::vector<double> vecDouble(vec.begin(), vec.end());
    this->ImagingParameters->SetTimeGainCompensation(vecDouble);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_TGC, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetTimeGainCompensationDevice(const Plus_uTGC& tgc)
{
  Plus_uTGC local = tgc;
  PlusStatus result = this->SetParamValueDevice("b-tgc", local, local);
  if (result == PLUS_SUCCESS)
  {
    std::vector<int> vec = local.toVector();
    std::vector<double> vecDouble(vec.begin(), vec.end());
    this->ImagingParameters->SetTimeGainCompensation(vecDouble);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_TGC, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetTimeGainCompensationDevice(int tgc[8])
{
  Plus_uTGC tgcStruct;
  PlusStatus result = GetParamValueDevice("b-tgc", tgcStruct, tgcStruct);
  if (result == PLUS_SUCCESS)
  {
    // Cache the value of this request
    std::vector<int> vec = tgcStruct.toVector();
    std::vector<double> vecDouble(vec.begin(), vec.end());
    this->ImagingParameters->SetTimeGainCompensation(vecDouble);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_TGC, false);
    std::copy(tgcStruct.toVector().begin(), tgcStruct.toVector().end(), tgc);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetTimeGainCompensationDevice(Plus_uTGC& tgc)
{
  PlusStatus result = GetParamValueDevice("b-tgc", tgc, tgc);
  if (result == PLUS_SUCCESS)
  {
    // Cache the value of this request
    std::vector<int> vec = tgc.toVector();
    std::vector<double> vecDouble(vec.begin(), vec.end());
    this->ImagingParameters->SetTimeGainCompensation(vecDouble);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_TGC, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetSoundVelocityDevice(float _arg)
{
  int soundVelocity = int(_arg);
  PlusStatus result = this->SetParamValueDevice("soundvelocity", soundVelocity, soundVelocity);
  if (result == PLUS_SUCCESS)
  {
    this->ImagingParameters->SetSoundVelocity(_arg);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetSoundVelocityDevice(float& soundVelocity)
{
  int soundVel;
  PlusStatus result = GetParamValueDevice("soundvelocity", soundVel, soundVel);
  if (result == PLUS_SUCCESS)
  {
    soundVelocity = float(soundVel);
    this->ImagingParameters->SetSoundVelocity(soundVelocity);
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY, false);
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetCompressionStatus(int aCompressionStatus)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->CompressionStatus = aCompressionStatus;
    return PLUS_SUCCESS;
  }
  if (!this->Ult->setCompressionStatus(aCompressionStatus))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetCompressionStatus failed: (compressionStatus=" << aCompressionStatus << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->CompressionStatus = aCompressionStatus;
  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetCompressionStatus(int& aCompressionStatus)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    aCompressionStatus = this->CompressionStatus;
    return PLUS_SUCCESS;
  }
  aCompressionStatus = this->Ult->getCompressionStatus();
  this->CompressionStatus = aCompressionStatus;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetTimeout(int aTimeout)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->Timeout = aTimeout;
    return PLUS_SUCCESS;
  }
  if (aTimeout < 0)
  {
    LOG_ERROR("vtkPlusSonixVideoSource::setTimeout failed: (aTimeout=" << aTimeout << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->Ult->setTimeout(aTimeout);
  this->Timeout = aTimeout;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetAcquisitionDataTypeDevice(int aAcquisitionDataType)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->AcquisitionDataType = aAcquisitionDataType;
    return PLUS_SUCCESS;
  }
  // Check if the desired acquisition type is actually available on desired imaging mode
  if (!this->Ult->isDataAvailable((uData)(aAcquisitionDataType)))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetAcquisitionDataType failed: Requested the data aquisition type not available for selected imaging mode(" << GetLastUlteriusError() << ")");
    return PLUS_FAIL;
  }
  // actually request data, now that its available
  if (!this->Ult->setDataToAcquire(aAcquisitionDataType))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetAcquisitionDataType failed: couldn't request the data aquisition type " << aAcquisitionDataType << ", " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->AcquisitionDataType = aAcquisitionDataType;
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetAcquisitionDataTypeDevice(int& acquisitionDataType)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    acquisitionDataType = this->AcquisitionDataType;
    return PLUS_SUCCESS;
  }
  acquisitionDataType = this->Ult->getDataToAcquire();
  this->AcquisitionDataType = acquisitionDataType;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetImagingModeDevice(int mode)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->ImagingMode = mode;
    return PLUS_SUCCESS;
  }
  if (!this->Ult->selectMode(mode))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetImagingMode failed: couldn't select imaging mode " << mode << ", " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->ImagingMode = mode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetImagingModeDevice(int& mode)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    mode = this->ImagingMode;
    return PLUS_SUCCESS;
  }
  mode = this->Ult->getActiveImagingMode();
  this->ImagingMode = mode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::PrintListOfImagingParametersFromDevice()
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkPlusSonixVideoSource::PrintListOfParameters failed: not connected");
    return PLUS_FAIL;
  }
  uParam prmDescriptor;
  LOG_INFO("Index; Id; Name; Source; Type; Unit");
  for (int paramId = 0; this->Ult->getParam(paramId, prmDescriptor); paramId++)
  {
    LOG_INFO(paramId << "; " << prmDescriptor.id << "; " << prmDescriptor.name << "; " << prmDescriptor.source
             << "; " << prmDescriptor.type << "; " << prmDescriptor.unit);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetDisplayedFrameRateDevice(int& aFrameRate)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkPlusSonixVideoSource::GetDisplayedFrameRate failed: not connected");
    return PLUS_FAIL;
  }

  if (!this->Ult->getParamValue("frame rate", aFrameRate))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::GetDisplayedFrameRate failed: cannot retrieve displayed frame rate.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetRFDecimationDevice(int decimation)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetRFDecimation failed: not connected");
    return PLUS_FAIL;
  }
  if (!this->Ult->setParamValue("rf-rf decimation", decimation))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetRFDecimation failed: cannot set decimation value.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetPPFilterDevice(int filterIndex)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetPPFilter failed: not connected");
    return PLUS_FAIL;
  }
  if (!this->Ult->setParamValue("b-filter type", filterIndex))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetPPFilter failed: cannot set filter value.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetFrameRateLimitDevice(int frLimit)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetFrameRateLimit failed: not connected");
    return PLUS_FAIL;
  }
  if (!this->Ult->setParamValue("max fr", frLimit))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetFrameRateLimit failed: cannot set maximum frame rate limit value.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::SetRfAcquisitionModeDevice(RfAcquisitionModeType mode)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->RfAcquisitionMode = mode;
    return PLUS_SUCCESS;
  }
  if (!this->Ult->setParamValue("rf-mode", mode))
  {
    LOG_ERROR("vtkPlusSonixVideoSource::SetRfAcquisitionMode failed (paramId=rf-mode, paramValue=" << mode << ") " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->RfAcquisitionMode = mode;
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetRfAcquisitionModeDevice(RfAcquisitionModeType& mode)
{
  int iMode = this->RfAcquisitionMode;
  if (this->UlteriusConnected)
  {
    if (!this->Ult->getParamValue("rf-mode", iMode))
    {
      LOG_ERROR("vtkPlusSonixVideoSource::GetRfAcquisitionMode failed. " << GetLastUlteriusError());
      return PLUS_FAIL;
    }
  }

  switch (iMode)
  {
    case 0:
      mode = RF_ACQ_B_ONLY;
      break;
    case 1:
      mode = RF_ACQ_RF_ONLY;
      break;
    case 2:
      mode = RF_ACQ_B_AND_RF;
      break;
    case 3:
      mode = RF_ACQ_CHRF_ONLY;
      break;
    case 4:
      mode = RF_ACQ_B_AND_CHRF;
      break;
    default:
      mode = RF_UNKNOWN;
      LOG_WARNING("Unknown RF acquisition mode type: " << iMode);
      return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 2)
  {
    LOG_WARNING("vtkPlusSonixVideoSource is expecting at most two output channels and their are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.size() == 0)
  {
    LOG_ERROR("No output channels defined for vtkPlusSonixVideoSource. Cannot proceed.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return Superclass::NotifyConfigured();
}

//----------------------------------------------------------------------------
bool vtkPlusSonixVideoSource::HasDataType(uData aValue)
{
  uDataDesc someVal;
  bool success = this->Ult->getDataDescriptor(aValue, someVal);
  return success;
}

//----------------------------------------------------------------------------
bool vtkPlusSonixVideoSource::WantDataType(uData aValue)
{
  return (this->AcquisitionDataType & aValue) > 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::ConfigureVideoSource(uData aValue)
{
  vtkPlusDataSource* aSource(NULL);
  std::vector<vtkPlusDataSource*> sources;
  if ((aValue & udtBPost) > 0)
  {
    this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources);
    if (!sources.empty())
    {
      aSource = sources[0];
    }
  }
  else if ((aValue & udtRF) > 0)
  {
    this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, sources);
    if (!sources.empty())
    {
      aSource = sources[0];
    }
  }
  else
  {
    LOG_ERROR("Unsupported uData requested of Sonix video source.");
    return PLUS_FAIL;
  }

  if (aSource == NULL)
  {
    LOG_ERROR("Unable to locate the video source for uData: " << aValue);
    return PLUS_FAIL;
  }

  if (!this->Ult->isDataAvailable(aValue))
  {
    LOG_ERROR("Data type is not available. " << aValue);
    return PLUS_FAIL;
  }

  // Set frame size and pixel type
  uDataDesc aDataDescriptor;
  if (!this->Ult->getDataDescriptor(aValue, aDataDescriptor))
  {
    LOG_WARNING("Initialize: couldn't retrieve data descriptor (" << GetLastUlteriusError() << ")"); // error is reported at higher level, as it often happens that this call fails but after a few attempts it succeeds
    return PLUS_FAIL;
  }

  switch (aDataDescriptor.ss)
  {
    case 8:
      aSource->SetPixelType(VTK_UNSIGNED_CHAR);
      aSource->SetImageType(US_IMG_BRIGHTNESS);
      aSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);
      break;
    case 16:
      aSource->SetPixelType(VTK_SHORT);
      aSource->SetImageType(US_IMG_RF_I_LINE_Q_LINE);
      // RF data is stored line-by-line, therefore set the storage buffer to FM orientation
      aSource->SetOutputImageOrientation(US_IMG_ORIENT_FM);
      // Swap w/h: in case of RF image acquisition the DataDescriptor.h is the width and the DataDescriptor.w is the height
      std::swap(aDataDescriptor.h, aDataDescriptor.w);
      std::swap(aDataDescriptor.roi.ulx, aDataDescriptor.roi.uly);
      std::swap(aDataDescriptor.roi.urx, aDataDescriptor.roi.ury);
      std::swap(aDataDescriptor.roi.blx, aDataDescriptor.roi.bly);
      std::swap(aDataDescriptor.roi.brx, aDataDescriptor.roi.bry);
      break;
    default:
      LOG_ERROR("Unsupported Ulterius bit depth: " << aDataDescriptor.ss);
      return PLUS_FAIL;
  }

  if (this->AutoClipEnabled)
  {
    int clipRectangleOrigin[3] = {0, 0, 0};
    clipRectangleOrigin[0] = std::min(aDataDescriptor.roi.ulx, aDataDescriptor.roi.blx);
    clipRectangleOrigin[1] = std::min(aDataDescriptor.roi.uly, aDataDescriptor.roi.ury);
    int clipRectangleSize[3] = {0, 0, 1};
    clipRectangleSize[0] = std::max(aDataDescriptor.roi.urx - aDataDescriptor.roi.ulx, aDataDescriptor.roi.brx - aDataDescriptor.roi.blx);
    clipRectangleSize[1] = std::max(aDataDescriptor.roi.bly - aDataDescriptor.roi.uly, aDataDescriptor.roi.bry - aDataDescriptor.roi.ury);
    aSource->SetClipRectangleOrigin(clipRectangleOrigin);
    aSource->SetClipRectangleSize(clipRectangleSize);
  }

  this->SetInputFrameSize(*aSource, aDataDescriptor.w, aDataDescriptor.h, 1);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::GetRequestedImagingDataTypeFromSources(int& requestedImagingDataType)
{
  requestedImagingDataType = 0;
  std::vector<vtkPlusDataSource*> sources;
  if (this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (!sources.empty())
  {
    requestedImagingDataType |= vtkPlusUsImagingParameters::DataTypeBPost;
  }
  if (this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, sources) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (!sources.empty())
  {
    requestedImagingDataType |= vtkPlusUsImagingParameters::DataTypeRF;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<int> vtkPlusSonixVideoSource::Plus_uTGC::toVector()
{
  std::vector<int> result;
  result.push_back(this->v1);
  result.push_back(this->v2);
  result.push_back(this->v3);
  result.push_back(this->v4);
  result.push_back(this->v5);
  result.push_back(this->v6);
  result.push_back(this->v7);
  result.push_back(this->v8);
  return result;
}

//----------------------------------------------------------------------------
void vtkPlusSonixVideoSource::Plus_uTGC::fromVector(const std::vector<int> input)
{
  if (input.size() != 8)
  {
    return;
  }
  this->v1 = input[0];
  this->v2 = input[1];
  this->v3 = input[2];
  this->v4 = input[3];
  this->v5 = input[4];
  this->v6 = input[5];
  this->v7 = input[6];
  this->v8 = input[7];
}

//----------------------------------------------------------------------------
std::string vtkPlusSonixVideoSource::Plus_uTGC::toString(char separator/*=' '*/)
{
  std::stringstream ss;
  ss << v1 << separator << v2 << separator << v3 << separator << v4 << separator << v5 << separator << v6 << separator << v7 << separator << v8;
  return ss.str();
}

//----------------------------------------------------------------------------
void vtkPlusSonixVideoSource::Plus_uTGC::fromString(const std::string& input, char separator/*=' '*/)
{
  std::vector<int> vect;
  std::stringstream ss(input);

  double i;
  while (ss >> i)
  {
    vect.push_back(i);
    if (ss.peek() == separator)
    {
      ss.ignore();
    }
  }
  this->fromVector(vect);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSonixVideoSource::RequestImagingParameterChange()
{
  PlusStatus status = PLUS_SUCCESS;

  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FREQUENCY)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_FREQUENCY) )
  {
    if (this->SetFrequencyDevice(this->ImagingParameters->GetFrequencyMhz()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set frequency imaging parameter" );
      status = PLUS_FAIL;
    }
  }
  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DEPTH)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DEPTH) )
  {
    if (this->SetDepthDevice(this->ImagingParameters->GetDepthMm()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set depth imaging parameter" );
      status = PLUS_FAIL;
    }
  }
  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_SECTOR)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_SECTOR) )
  {
    if (this->SetSectorDevice(this->ImagingParameters->GetSectorPercent()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set sector imaging parameter" );
      status = PLUS_FAIL;
    }
  }
  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_GAIN)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_GAIN) )
  {
    if (this->SetGainDevice(this->ImagingParameters->GetGainPercent()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set gain imaging parameter" );
      status = PLUS_FAIL;
    }
  }
  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DYNRANGE)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DYNRANGE) )
  {
    if (this->SetDynRangeDevice(this->ImagingParameters->GetDynRangeDb()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set dynamic range imaging parameter" );
      status = PLUS_FAIL;
    }
  }
  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_ZOOM)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_ZOOM) )
  {
    if (this->SetZoomDevice(this->ImagingParameters->GetZoomFactor()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set zoom imaging parameter" );
      status = PLUS_FAIL;
    }
  }
  if ( this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY) )
  {
    if (this->SetSoundVelocityDevice(this->ImagingParameters->GetSoundVelocity()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set sound velocity imaging parameter" );
      status = PLUS_FAIL;
    }
  }

  //TODO: Acknowledge parameter change
  //      (if here then need to trust developer, or do it everywhere this function is called)

  return status;
}
