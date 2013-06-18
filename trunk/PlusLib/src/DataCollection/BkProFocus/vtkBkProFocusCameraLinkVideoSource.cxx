/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusBkProFocusCameraLinkReceiver.h"
#include "PlusConfigure.h"
#include "vtkBkProFocusCameraLinkVideoSource.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "vtkRfProcessor.h"
#include "vtkUsScanConvertCurvilinear.h"
#include "vtkUsScanConvertLinear.h"

// BK Includes
#include "AcquisitionGrabberSapera.h"
#include "AcquisitionInjector.h"
#include "AcquisitionSettings.h"
#include "BmodeViewDataReceiver.h"
#include "CommandAndControl.h"
#include "ParamConnectionSettings.h"
#include "SaperaViewDataReceiver.h"
#include "TcpClient.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkBkProFocusCameraLinkVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkBkProFocusCameraLinkVideoSource);
//----------------------------------------------------------------------------


std::string ParseResponse(std::string str,int item)
{
  int nItems = 0;
  std::string rest = str;
  while(rest.size())
  {	
    int pos = rest.find(",");
    std::string curItem;
    if(pos==-1)
    {	
      curItem = rest;
      rest = "";
    } 
    else
    {
      curItem = rest.substr(0,pos);
      rest = rest.substr(pos+1,rest.length());
    }
    if(nItems++ == item)
    {
      return curItem;
    }
  }
  return "";
}

std::string ParseResponseQuoted(std::string str,int item)
{
  std::string result = ParseResponse(str, item);
  if(result.length()>2)
    return result.substr(1,result.length()-2);
  else
    return "";
}

class vtkBkProFocusCameraLinkVideoSource::vtkInternal
{
public:
  vtkBkProFocusCameraLinkVideoSource *External;
  vtkPlusChannel* Channel;

  ParamConnectionSettings BKparamSettings; // parConnectSettings, for read/write settings from ini file

  AcquisitionInjector BKAcqInjector; // injector
  AcquisitionSettings BKAcqSettings; // settings
  AcquisitionGrabberSapera BKAcqSapera; // sapera
  BmodeViewDataReceiver BKBModeView; // bmodeView; 
  SaperaViewDataReceiver* pBKSaperaView; // saperaView
  PlusBkProFocusCameraLinkReceiver PlusReceiver;

  CmdCtrlSettings BKcmdCtrlSettings; // cmdCtrlSet
  CommandAndControl* pBKcmdCtrl; // cmdctrl

  vtkBkProFocusCameraLinkVideoSource::vtkInternal::vtkInternal(vtkBkProFocusCameraLinkVideoSource* external) 
    : External(external)
    , pBKSaperaView(NULL)
    , pBKcmdCtrl(NULL)
  {
    this->PlusReceiver.SetPlusVideoSource(this->External);
  }

  virtual vtkBkProFocusCameraLinkVideoSource::vtkInternal::~vtkInternal() 
  {    
    this->PlusReceiver.SetPlusVideoSource(NULL);
    this->Channel = NULL;
    delete this->pBKSaperaView;
    this->pBKSaperaView=NULL;
    delete this->pBKcmdCtrl;
    this->pBKcmdCtrl=NULL;
    this->External = NULL;
  }  

  void vtkBkProFocusCameraLinkVideoSource::vtkInternal::InitializeParametersFromOEM()
  {
    //WSAIF wsaif;
    TcpClient *oemClient = (this->pBKcmdCtrl->GetOEMClient());
    std::string value;	  

    // Explanation of the queries/responses is from the BK document 
    //  "Product Specification for Pro Focus OEM Interface"
    // DATA:TRANSDUCER:A "A","8848"; 
    // the first value is the connector used, the second is the transducer type
    value = QueryParameter(oemClient, "TRANSDUCER");
    std::string transducer = ParseResponseQuoted(value,1);
    std::cout << "Transducer: " << transducer << std::endl;
    // DATA:SCAN_PLANE:A "S";
    // reply depends on the transducer type; for 8848, it is either "T" (transverse) or "S"
    // (sagittal). For the abdominal 8820, the response apparently is "" (!)
    value = QueryParameter(oemClient, "SCAN_PLANE");
    std::string scanPlane = ParseResponseQuoted(value, 0);
    std::cout << "Scan plane: " << scanPlane << std::endl;

    value = QueryParameter(oemClient, "B_FRAMERATE");   // DATA:B_FRAMERATE:A 17.8271;
    float frameRate = atof(ParseResponse(value,0).c_str());
    std::cout << "Frame rate: " << frameRate << std::endl;

    std::cout << "Queried value: " << value << std::endl;
    // DATA:B_GEOMETRY_SCANAREA:A 
    //    StartLineX,StartLineY,StartLineAngle,StartDepth,StopLineX,StopLineY,StopLineAngle,StopDepth
    // StartLineX/Y: coordinate of the start line origin in mm
    // StartLineAngle: angle of the start line in radians
    // StartDepth: start depth of the scanning area in m
    // StopLineX/Y: coordinate of the stop line origin in mm
    // StopDepth: stop depth of the scanning area in mm
    value = QueryParameter(oemClient, "B_GEOMETRY_SCANAREA"); 
    float startLineXMm = fabs(atof(ParseResponse(value,0).c_str()))*1000.;
    float stopLineXMm = fabs(atof(ParseResponse(value,4).c_str()))*1000.;
    float startAngleDeg = 
      vtkMath::DegreesFromRadians(atof(ParseResponse(value,2).c_str()));
    // start depth is defined at the distance from the outer surface of the transducer 
    // to the surface of the crystal. stop depth is from the outer surface to the scan depth.
    // start depth has negative depth in this coordinate system (the transducer surface pixels are inside the image)
    float startDepthMm = atof(ParseResponse(value,3).c_str())*1000.;
    float stopAngleDeg = 
      vtkMath::DegreesFromRadians(atof(ParseResponse(value,6).c_str()));
    float stopDepthMm = atof(ParseResponse(value,7).c_str())*1000.;

    // DATA:B_SCANLINES_COUNT:A 517;
    // Number of scanning lines in specified view
    value = QueryParameter(oemClient, "B_SCANLINES_COUNT");
    float scanlinesCount = atof(ParseResponse(value,0).c_str());
    value = QueryParameter(oemClient, "B_RF_LINE_LENGTH");
    float rfLineLength = atof(ParseResponse(value,0).c_str());

    // BK defines angles start at 9:00, not at 12:00
    startAngleDeg = -(startAngleDeg-stopAngleDeg)/2.;
    stopAngleDeg = -startAngleDeg;

    // Update the RfProcessor with scan conversion parameters
    
    if(transducer == "8848")
    {
      if(scanPlane == "S")
      {
        LOG_DEBUG("Linear transducer");
        if ( vtkUsScanConvertLinear::SafeDownCast(this->Channel->GetRfProcessor()->GetScanConverter()) == NULL)
        {
          // The current scan converter is not for a linear transducer, so change it now
          vtkSmartPointer<vtkUsScanConvertLinear> scanConverter = vtkSmartPointer<vtkUsScanConvertLinear>::New();
          this->Channel->GetRfProcessor()->SetScanConverter(scanConverter);
        }
      }
      else if(scanPlane == "T")
      {
        LOG_DEBUG("Curvilinear transducer");
        if ( vtkUsScanConvertCurvilinear::SafeDownCast(this->Channel->GetRfProcessor()->GetScanConverter()) == NULL)
        {
          // The current scan converter is not for a curvilinear transducer, so change it now
          vtkSmartPointer<vtkUsScanConvertCurvilinear> scanConverter = vtkSmartPointer<vtkUsScanConvertCurvilinear>::New();
          this->Channel->GetRfProcessor()->SetScanConverter(scanConverter);
        }
      }
      else
      {
        LOG_WARNING("Unknown transducer scan plane (" << scanPlane << "). Cannot determine transducer geometry.");
      }
    }
    else
    {
      LOG_WARNING("Unknown transducer model (" << transducer << "). Cannot determine transducer geometry.");
    }

    vtkUsScanConvert* scanConverter=this->Channel->GetRfProcessor()->GetScanConverter();
    if (scanConverter != NULL)
    {
      vtkUsScanConvertLinear* scanConverterLinear = vtkUsScanConvertLinear::SafeDownCast(scanConverter);
      vtkUsScanConvertCurvilinear* scanConverterCurvilinear = vtkUsScanConvertCurvilinear::SafeDownCast(scanConverter);
      if (scanConverterLinear != NULL)
      {
        scanConverterLinear->SetTransducerWidthMm(startLineXMm + stopLineXMm);
        scanConverterLinear->SetImagingDepthMm(stopDepthMm);
      }
      else if (scanConverterCurvilinear != NULL)
      {
        // this is a predefined value for 8848 transverse array, which
        // apparently cannot be queried from OEM. It is not clear if ROC is the distance to
        // crystal surface or to the outer surface of the transducer (waiting for the response from BK).
        scanConverterCurvilinear->SetOutputImageStartDepthMm(startDepthMm);
        scanConverterCurvilinear->SetRadiusStartMm(9.74);
        scanConverterCurvilinear->SetRadiusStopMm(stopDepthMm);
        scanConverterCurvilinear->SetThetaStartDeg(startAngleDeg);
        scanConverterCurvilinear->SetThetaStopDeg(stopAngleDeg);        
      }
      else
      {
        LOG_WARNING("Unknown scan converter type: " << scanConverter->GetTransducerGeometry() );
      }

      scanConverter->SetTransducerName((std::string("BK-") + transducer + scanPlane).c_str());      
    }
    else
    {
      LOG_WARNING("Scan converter is not defined in either manually or through the OEM interface");
    }

    /* Not used in reconstruction
    std::cout << "Queried value: " << value << std::endl;
    // DATA:3D_SPACING:A 0.25;
    //  Returns the spacing between the frames; 
    //  Fan-type movers return spacing in degrees
    //  Linear-type movers returm spacing in mm
    value = QueryParameter(oemClient, "3D_SPACING");
    std::cout << "Queried value: " << value << std::endl;
    // DATA:3D_CAPTURE_AREA:A Left,Top,Right,Bottom;
    //  Returns the capture area for the acquisition in screen pixel coordinates
    value = QueryParameter(oemClient, "3D_CAPTURE_AREA");
    std::cout << "Queried value: " << value << std::endl;
    */
  }

  std::string vtkBkProFocusCameraLinkVideoSource::vtkInternal::QueryParameter(TcpClient *oemClient, const char* parameter)
  {
    std::string query;
    char buffer[1024];

    query = std::string("QUERY:")+parameter+":A;";
    oemClient->Write(query.c_str(), strlen(query.c_str()));
    oemClient->Read(&buffer[0], 1024);
    std::string value = std::string(&buffer[0]);
    std::string prefix = std::string("DATA:")+parameter+":A ";
    return value.substr(prefix.length(),value.length()-prefix.length()-1);
  }

  void vtkBkProFocusCameraLinkVideoSource::vtkInternal::RegisterEventCallback(void* owner, void (*func)(void*, char*, size_t))
  {
    if( this->pBKcmdCtrl != NULL )
    {
      this->pBKcmdCtrl->RegisterEventCallback(owner, func);
    }
  }
};

//----------------------------------------------------------------------------
vtkBkProFocusCameraLinkVideoSource::vtkBkProFocusCameraLinkVideoSource()
: SubscribeScanPlane(false)
, CurrentPlane(Transverse)
{
  this->Internal = new vtkInternal(this);

  this->IniFileName=NULL;
  this->ShowSaperaWindow=false;
  this->ShowBModeWindow=false;

  this->ImagingMode=RfMode;
  SetLogFunc(LogInfoMessageCallback);
  SetDbgFunc(LogDebugMessageCallback);

  // No need for StartThreadForInternalUpdates, as we are notified about each new frame through a callback function
}

//----------------------------------------------------------------------------
vtkBkProFocusCameraLinkVideoSource::~vtkBkProFocusCameraLinkVideoSource()
{
  SetIniFileName(NULL);

  delete this->Internal;
  this->Internal=NULL;
}

//----------------------------------------------------------------------------
void vtkBkProFocusCameraLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
void vtkBkProFocusCameraLinkVideoSource::LogInfoMessageCallback(char *msg)
{
  LOG_INFO(msg);
}

//----------------------------------------------------------------------------
void vtkBkProFocusCameraLinkVideoSource::LogDebugMessageCallback(char *msg)
{
  LOG_INFO(msg);
}

//----------------------------------------------------------------------------
void vtkBkProFocusCameraLinkVideoSource::EventCallback(void* owner, char* eventText, size_t eventTextLength)
{
  vtkBkProFocusCameraLinkVideoSource* self = static_cast<vtkBkProFocusCameraLinkVideoSource*>(owner);

  LOG_INFO("full event text: " << eventText);

  if (self->SubscribeScanPlane && !_strnicmp("SCAN_PLANE", &eventText[strlen("CONFIG:DATA:")], strlen("SCAN_PLANE")) )
  {
    char* probeId = &eventText[strlen("CONFIG:DATA:")];
    std::string eventStr(eventText);
    std::string details = eventStr.substr(eventStr.find(' '));
    if( details.find('S') != std::string::npos )
    {
      self->CurrentPlane = Sagittal;
    }
    if( details.find('T') != std::string::npos )
    {
      self->CurrentPlane = Transverse;
    }
    self->Internal->Channel = self->FindChannelByPlane();
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::InternalConnect()
{
  std::string iniFilePath;
  GetFullIniFilePath(iniFilePath);
  if (!this->Internal->BKparamSettings.LoadSettingsFromIniFile(iniFilePath.c_str()))
  {
    LOG_ERROR("Could not load BK parameter settings from file: "<<iniFilePath.c_str());
    return PLUS_FAIL;
  }

  LOG_DEBUG("BK scanner address: " << this->Internal->BKparamSettings.GetScannerAddress());
  LOG_DEBUG("BK scanner OEM port: " << this->Internal->BKparamSettings.GetOemPort());
  LOG_DEBUG("BK scanner toolbox port: " << this->Internal->BKparamSettings.GetToolboxPort());

  this->Internal->BKcmdCtrlSettings.LoadFromIniFile(iniFilePath.c_str());

  if (!this->Internal->BKAcqSettings.LoadIni(iniFilePath.c_str()))
  {
    LOG_ERROR("Failed to load acquisition settings from file: "<<iniFilePath.c_str());
    return PLUS_FAIL;
  }

  this->Internal->pBKcmdCtrl = new CommandAndControl(&this->Internal->BKparamSettings, &this->Internal->BKcmdCtrlSettings);
  this->Internal->BKcmdCtrlSettings = this->Internal->pBKcmdCtrl->GetCmdCtrlSettings();    // Get what has not failed !!!
  this->Internal->BKcmdCtrlSettings.autoUpdate = true;

  this->Internal->InitializeParametersFromOEM();

  int numSamples = 0;
  int numLines = 0;
  if (!this->Internal->pBKcmdCtrl->CalcSaperaBufSize(&numSamples, &numLines))
  {
    LOG_ERROR("Failed to get Sapera framegrabber buffer size for RF data");
    delete this->Internal->pBKcmdCtrl;
    this->Internal->pBKcmdCtrl=NULL;
    return PLUS_FAIL;
  }

  LOG_DEBUG("Sapera buffer size: numSamples="<<numSamples<<", numLines="<<numLines);

  this->Internal->BKAcqSettings.SetLinesPerFrame(numLines);
  this->Internal->BKAcqSettings.SetRFLineLength(numSamples);  
  this->Internal->BKAcqSettings.SetFramesToGrab(0); // continuous

  if( this->SubscribeScanPlane )
  {
    LOG_INFO("Subscribing to scan plane events.");
    this->Internal->pBKcmdCtrl->SubscribeScanPlaneEvents();
    this->Internal->RegisterEventCallback((void*)this, EventCallback);
  }

  if (!this->Internal->BKAcqSapera.Init(this->Internal->BKAcqSettings))
  {
    LOG_ERROR("Failed to initialize framegrabber");
    delete this->Internal->pBKcmdCtrl;
    this->Internal->pBKcmdCtrl=NULL;
    return PLUS_FAIL;
  }

  this->Internal->pBKSaperaView = new SaperaViewDataReceiver(this->Internal->BKAcqSapera.GetBuffer());
  if (this->ShowSaperaWindow)
  {
    // show Sapera viewer
    this->Internal->BKAcqInjector.AddDataReceiver(this->Internal->pBKSaperaView);
  }

  if (this->ShowBModeWindow)
  {
    // show B-mode image  
    this->Internal->BKAcqInjector.AddDataReceiver(&this->Internal->BKBModeView); 
  }

  // send frames to this video source
  this->Internal->BKAcqInjector.AddDataReceiver(&this->Internal->PlusReceiver);

  // Clear buffer on connect because the new frames that we will acquire might have a different size
  for( ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    (*it)->Clear();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::InternalDisconnect()
{
  this->Internal->BKAcqSapera.Destroy();

  this->Internal->BKAcqInjector.RemoveDataReceiver(&this->Internal->PlusReceiver);

  if (this->ShowBModeWindow)
  {
    this->Internal->BKAcqInjector.RemoveDataReceiver(&this->Internal->BKBModeView);
  }

  if (this->ShowSaperaWindow)
  {
    this->Internal->BKAcqInjector.RemoveDataReceiver(this->Internal->pBKSaperaView);
  }

  delete this->Internal->pBKSaperaView;
  this->Internal->pBKSaperaView=NULL;
  delete this->Internal->pBKcmdCtrl;
  this->Internal->pBKcmdCtrl=NULL;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::InternalStartRecording()
{
  if (!this->Internal->BKAcqSapera.StartGrabbing(&this->Internal->BKAcqInjector))
  {
    LOG_ERROR("Failed to start grabbing");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::InternalStopRecording()
{
  /*
  Sleep(500);
  if (!this->Internal->BKAcqSapera.StopGrabbing())
  {
  LOG_ERROR("Failed to start grabbing");
  return PLUS_FAIL;
  }
  */
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkBkProFocusCameraLinkVideoSource::NewFrameCallback(void* pixelDataPtr, const int inputFrameSizeInPix[2], PlusCommon::ITKScalarPixelType pixelType, US_IMAGE_TYPE imageType)
{
  // we may need to overwrite these, so create a copy that will be used internally
  int frameSizeInPix[2] = {
    inputFrameSizeInPix[0],
    inputFrameSizeInPix[1]
  };

  LOG_TRACE("New frame received: " << frameSizeInPix[0] << "x" << frameSizeInPix[1]
    << ", pixel type: " << vtkImageScalarTypeNameMacro(PlusVideoFrame::GetVTKScalarPixelType(pixelType))
    << ", image type: " << PlusVideoFrame::GetStringFromUsImageType(imageType));

  vtkPlusChannel* channel = this->FindChannelByPlane();

  if( channel == NULL )
  {
    LOG_ERROR("No channel returned. Debug!");
    return;
  }

  switch (this->ImagingMode)
  {
  case RfMode:
    {
      if (imageType == US_IMG_RF_REAL || imageType == US_IMG_RF_IQ_LINE || imageType == US_IMG_RF_I_LINE_Q_LINE)
      {
        // RF image is received and RF image is needed => no need for conversion
        break;
      }
      LOG_ERROR("The received frame is discarded, as it cannot be convert from "<<PlusVideoFrame::GetStringFromUsImageType(imageType)<<" to RF");
      return;
    }
  case BMode:
    {
      if (imageType == US_IMG_BRIGHTNESS)
      {
        // B-mode image is received and B-mode image is needed => no need for conversion
        break;
      }
      else if (imageType == US_IMG_RF_REAL || imageType == US_IMG_RF_IQ_LINE || imageType == US_IMG_RF_I_LINE_Q_LINE)
      {
        // convert from RF to Brightness

        // Create a VTK image input for the RF to Brightness converter
        vtkSmartPointer<vtkImageImport> bufferToVtkImage = vtkSmartPointer<vtkImageImport>::New();
        bufferToVtkImage->SetDataScalarType(PlusVideoFrame::GetVTKScalarPixelType(pixelType));
        bufferToVtkImage->SetImportVoidPointer((unsigned char*)pixelDataPtr);
        bufferToVtkImage->SetDataExtent(0, frameSizeInPix[0] - 1, 0, frameSizeInPix[1] - 1, 0,0);
        bufferToVtkImage->SetWholeExtent(0, frameSizeInPix[0] - 1, 0, frameSizeInPix[1] - 1, 0,0);
        bufferToVtkImage->Update();

        channel->GetRfProcessor()->SetRfFrame(bufferToVtkImage->GetOutput(), imageType);        
        channel->SetSaveRfProcessingParameters(true); // RF processing parameters were used, make sure they will be saved into the config file

        // Overwrite the input parameters with the converted image; it will look as if we received a B-mode image
        vtkImageData* convertedBmodeImage = channel->GetRfProcessor()->GetBrightessScanConvertedImage();
        pixelDataPtr = convertedBmodeImage->GetScalarPointer();        
        int *resultExtent = convertedBmodeImage->GetExtent();        
        frameSizeInPix[0] = resultExtent[1] - resultExtent[0] + 1;
        frameSizeInPix[1] = resultExtent[3] - resultExtent[2] + 1;
        pixelType = PlusVideoFrame::GetITKScalarPixelType(convertedBmodeImage->GetScalarType());
        imageType = US_IMG_BRIGHTNESS;
        break;
      }
      LOG_ERROR("The received frame is discarded, as it cannot be convert from " << PlusVideoFrame::GetStringFromUsImageType(imageType) << " to Brightness");
      return;
    }
  default:
    LOG_ERROR("The received frame is discarded, as the requested imaging mode (" << PlusVideoFrame::GetStringFromUsImageType(imageType) << ") is not supported");
    return;
  }

  vtkPlusDataSource* aSource(NULL);
  if( channel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Output channel does not have video source. Unable to record a new frame.");
    return;
  }
  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( aSource->GetBuffer()->GetNumberOfItems() == 0 )
  {
    LOG_DEBUG("Set up BK ProFocus image buffer");
    aSource->GetBuffer()->SetPixelType(pixelType);      
    aSource->GetBuffer()->SetImageType(imageType);
    aSource->GetBuffer()->SetFrameSize( frameSizeInPix[0], frameSizeInPix[1] );
    if (imageType==US_IMG_BRIGHTNESS)
    {
      // Store B-mode images in MF orientation
      aSource->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);
    }
    else
    {
      // RF data is stored line-by-line, therefore set the temporary storage buffer to FM orientation
      aSource->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_FM);
    }
    LOG_INFO("Frame size: " << frameSizeInPix[0] << "x" << frameSizeInPix[1]
    << ", pixel type: " << vtkImageScalarTypeNameMacro(PlusVideoFrame::GetVTKScalarPixelType(pixelType))
      << ", image type: " << PlusVideoFrame::GetStringFromUsImageType(imageType)
      << ", device image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetPortImageOrientation())
      << ", buffer image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetBuffer()->GetImageOrientation()));

  } 

  aSource->GetBuffer()->AddItem(pixelDataPtr, aSource->GetPortImageOrientation(), frameSizeInPix, pixelType, imageType, 0, this->FrameNumber);
  this->Modified();
  this->FrameNumber++;

  // just for testing: PlusVideoFrame::SaveImageToFile( (unsigned char*)pixelDataPtr, frameSizeInPix, numberOfBitsPerPixel, (char *)"test.jpg");
}


//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkBkProFocusCameraLinkVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure BK ProFocus video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* deviceElement = this->FindThisDeviceElement(config);
  if (deviceElement == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* iniFileName = deviceElement->GetAttribute("IniFileName"); 
  if ( iniFileName != NULL) 
  {
    this->SetIniFileName(iniFileName); 
  }

  const char* imagingMode = deviceElement->GetAttribute("ImagingMode"); 
  if ( imagingMode != NULL) 
  {
    if (STRCASECMP(imagingMode, "BMode")==0)
    {
      LOG_DEBUG("Imaging mode set: BMode"); 
      SetImagingMode(BMode); 
    }
    else if (STRCASECMP(imagingMode, "RfMode")==0)
    {
      LOG_DEBUG("Imaging mode set: RfMode"); 
      SetImagingMode(RfMode); 
    }
    else
    {
      LOG_ERROR("Unsupported ImagingMode requested: "<<imagingMode);
    }
  }

  const char* subscribe = deviceElement->GetAttribute("SubscribeScanPlane"); 
  if ( subscribe != NULL ) 
  {
    this->SubscribeScanPlane = (STRCASECMP(subscribe, "true") == 0);
  }

  if( this->SubscribeScanPlane && this->OutputChannels.size() != 2 )
  {
    LOG_ERROR("Scan plane switching requested but there are not exactly two output channels.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceElement = this->FindThisDeviceElement(config);
  if (deviceElement == NULL)
  {
    LOG_ERROR("Cannot find device element in XML tree for device with ID: " << this->GetDeviceId() );
    return PLUS_FAIL;
  }

  deviceElement->SetAttribute("IniFileName", this->IniFileName);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::GetFullIniFilePath(std::string &fullPath)
{
  if (this->IniFileName==NULL)
  {
    LOG_ERROR("Ini file name has not been set");
    return PLUS_FAIL;
  }
  fullPath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory() + std::string("/") + this->IniFileName;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkBkProFocusCameraLinkVideoSource::SetImagingMode(ImagingModeType imagingMode)
{
  this->ImagingMode=imagingMode;
  // always keep the receiver in RF mode and if B-mode image is requested then do the B-mode conversion in this class
  // this->Internal->PlusReceiver.SetImagingMode(imagingMode);
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusCameraLinkVideoSource::NotifyConfigured()
{
  if( !this->SubscribeScanPlane && this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkBkProFocusCameraLinkVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_ERROR("No output channels defined for vtkBkProFocusCameraLinkVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  if( !this->SubscribeScanPlane )
  {
    this->Internal->Channel = this->OutputChannels[0];
  }
  else
  {
    this->Internal->Channel = this->FindChannelByPlane();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusChannel* vtkBkProFocusCameraLinkVideoSource::FindChannelByPlane()
{
  if (this->OutputChannels.size() != 2 )
  {
    return NULL;
  }

  // TODO: post namic, make this more advanced, use in-dev custom channel attributes
  if( this->CurrentPlane == Transverse )
  {
    return this->OutputChannels[0];
  }
  else
  {
    return this->OutputChannels[1];
  }
}
