#include "PlusConfigure.h"
#include "vtkBkProFocusVideoSource.h"
#include "vtkVideoBuffer.h"

#include "PlusBkProFocusReceiver.h"
#include "vtkRfProcessor.h"

// BK Includes
#include "AcquisitionGrabberSapera.h"
#include "AcquisitionInjector.h"
#include "AcquisitionSettings.h"
#include "CommandAndControl.h"
#include "ParamConnectionSettings.h"
#include "BmodeViewDataReceiver.h"
#include "SaperaViewDataReceiver.h"
#include "TcpClient.h"

vtkCxxRevisionMacro(vtkBkProFocusVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkBkProFocusVideoSource);

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
		return result.substr(1,result.length()-1);
	else
		return "";
}

class vtkBkProFocusVideoSource::vtkInternal
{
public:
  vtkBkProFocusVideoSource *External;

  ParamConnectionSettings BKparamSettings; // parConnectSettings, for read/write settings from ini file

  AcquisitionInjector BKAcqInjector; // injector
  AcquisitionSettings BKAcqSettings; // settings
  AcquisitionGrabberSapera BKAcqSapera; // sapera
  BmodeViewDataReceiver BKBModeView; // bmodeView; 
  SaperaViewDataReceiver* pBKSaperaView; // saperaView
  PlusBkProFocusReceiver PlusReceiver;

  CmdCtrlSettings BKcmdCtrlSettings; // cmdCtrlSet
  CommandAndControl* pBKcmdCtrl; // cmdctrl

  vtkBkProFocusVideoSource::vtkInternal::vtkInternal(vtkBkProFocusVideoSource* external) 
  {
    this->External = external;
    this->pBKSaperaView=NULL;
    this->pBKcmdCtrl=NULL;
    this->PlusReceiver.SetPlusVideoSource(this->External);
  }

  virtual vtkBkProFocusVideoSource::vtkInternal::~vtkInternal() 
  {    
    this->PlusReceiver.SetPlusVideoSource(NULL);
    delete this->pBKSaperaView;
    this->pBKSaperaView=NULL;
    delete this->pBKcmdCtrl;
    this->pBKcmdCtrl=NULL;
    this->External = NULL;
  }  

  void vtkBkProFocusVideoSource::vtkInternal::InitializeParametersFromOEM()
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

	  if(transducer == "8848")
	  {
		  if(scanPlane == "S")
		  {
			  this->External->RfProcessor->SetTransducerGeometry(vtkRfProcessor::TRANSDUCER_LINEAR);
		  }
		  else if(scanPlane == "T")
		  {
			  this->External->RfProcessor->SetTransducerGeometry(vtkRfProcessor::TRANSDUCER_CURVILINEAR);
		  }
	  }

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
	  float startLineXMm = atof(ParseResponse(value,0).c_str());
	  float startLineYMm = atof(ParseResponse(value,1).c_str());
	  float startAngleDeg = atof(ParseResponse(value,2).c_str())*180./3.1416;
	  float startDepthMm = atof(ParseResponse(value,3).c_str())*1000.;
	  float stopLineXMm = atof(ParseResponse(value,4).c_str());
	  float stopLineYMm = atof(ParseResponse(value,5).c_str());
	  float stopAngleDeg = atof(ParseResponse(value,6).c_str())*180./3.1416;
	  float stopDepthMm = atof(ParseResponse(value,7).c_str())*1000.;

	  std::cout << "Start line X: " << startLineXMm << " mm" << std::endl;
	  std::cout << "Start line Y: " << startLineYMm << " mm" << std::endl;
	  std::cout << "Start angle: " << startAngleDeg << " deg" << std::endl;
	  std::cout << "Start depth: " << startDepthMm << " mm" << std::endl;
	  std::cout << "Stop line X: " << stopLineXMm << " mm" << std::endl;
	  std::cout << "Stop line Y: " << stopLineYMm << " mm" << std::endl;
	  std::cout << "Stop angle: " << stopAngleDeg << " deg" << std::endl;
	  std::cout << "Stop depth: " << stopDepthMm << " m" << std::endl;

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
	  //for(;;);
  }

  std::string vtkBkProFocusVideoSource::vtkInternal::QueryParameter(TcpClient *oemClient, const char* parameter)
  {
	  std::string query;
	  char buffer[1024];

	  query = std::string("QUERY:")+parameter+":A;";
	  oemClient->Write(query.c_str(), strlen(query.c_str()));
	  oemClient->Read(&buffer[0], 1024);
	  std::string value = std::string(&buffer[0]);
	  std::string prefix = std::string("DATA:")+parameter+":A";
	  return value.substr(prefix.length(),value.length()-prefix.length()-1);
  }
};

//----------------------------------------------------------------------------
vtkBkProFocusVideoSource::vtkBkProFocusVideoSource()
{
  this->Internal = new vtkInternal(this);

  this->SpawnThreadForRecording=0;
  this->IniFileName=NULL;
  this->ShowSaperaWindow=false;
  this->ShowBModeWindow=false;

  this->SetFrameBufferSize(200); 
  this->Buffer->Modified();
  SetLogFunc(LogInfoMessageCallback);
  SetDbgFunc(LogDebugMessageCallback);

  this->TransducerGeometry = NULL;
  this->ImagingDepth = 0;
  this->TransducerWidth = 0;
  this->OutputImageSizePixel[0] = 0;
  this->OutputImageSizePixel[1] = 0;
  this->OutputImageSpacingMmPerPixel[0] = 0;
  this->OutputImageSpacingMmPerPixel[1] = 0;
  this->FrameRate = 0;
}

//----------------------------------------------------------------------------
vtkBkProFocusVideoSource::~vtkBkProFocusVideoSource()
{
  SetIniFileName(NULL);

  delete this->Internal;
  this->Internal=NULL;
}

//----------------------------------------------------------------------------
void vtkBkProFocusVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
void vtkBkProFocusVideoSource::LogInfoMessageCallback(char *msg)
{
  LOG_INFO(msg);
}

//----------------------------------------------------------------------------
void vtkBkProFocusVideoSource::LogDebugMessageCallback(char *msg)
{
  LOG_INFO(msg);
}


//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusVideoSource::InternalConnect()
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
  this->GetBuffer()->Clear();  

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusVideoSource::InternalDisconnect()
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
PlusStatus vtkBkProFocusVideoSource::InternalStartRecording()
{
  if (!this->Internal->BKAcqSapera.StartGrabbing(&this->Internal->BKAcqInjector))
  {
    LOG_ERROR("Failed to start grabbing");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusVideoSource::InternalStopRecording()
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
void vtkBkProFocusVideoSource::NewFrameCallback(void* pixelDataPtr, const int frameSizeInPix[2], PlusCommon::ITKScalarPixelType pixelType, US_IMAGE_TYPE imageType)
{      
  LOG_TRACE("New frame received: "<<frameSizeInPix[0]<<"x"<<frameSizeInPix[1]
    <<", pixel type: "<<vtkImageScalarTypeNameMacro(PlusVideoFrame::GetVTKScalarPixelType(pixelType))
    <<", image type: "<<PlusVideoFrame::GetStringFromUsImageType(imageType));

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( this->GetBuffer()->GetNumberOfItems() == 0 )
  {
    LOG_DEBUG("Set up BK ProFocus image buffer");
    this->GetBuffer()->SetPixelType(pixelType);      
    this->GetBuffer()->SetImageType(imageType);
    this->GetBuffer()->SetFrameSize( frameSizeInPix[0], frameSizeInPix[1] );
    if (imageType==US_IMG_BRIGHTNESS)
    {
      // Store B-mode images in MF orientation
      this->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);
    }
    else
    {
      // RF data is stored line-by-line, therefore set the temporary storage buffer to FM orientation
      this->Buffer->SetImageOrientation(US_IMG_ORIENT_FM);
    }
    LOG_INFO("Frame size: "<<frameSizeInPix[0]<<"x"<<frameSizeInPix[1]
    <<", pixel type: "<<vtkImageScalarTypeNameMacro(PlusVideoFrame::GetVTKScalarPixelType(pixelType))
    <<", image type: "<<PlusVideoFrame::GetStringFromUsImageType(imageType)
    <<", device image orientation: "<<PlusVideoFrame::GetStringFromUsImageOrientation(this->GetDeviceImageOrientation())
    <<", buffer image orientation: "<<PlusVideoFrame::GetStringFromUsImageOrientation(this->Buffer->GetImageOrientation()));

  } 

  this->Buffer->AddItem(pixelDataPtr, this->GetDeviceImageOrientation(), frameSizeInPix, pixelType, imageType, 0, this->FrameNumber);
  this->Modified();
  this->FrameNumber++;
  
  // just for testing: PlusVideoFrame::SaveImageToFile( (unsigned char*)pixelDataPtr, frameSizeInPix, numberOfBitsPerPixel, (char *)"test.jpg");
}


//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkBkProFocusVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure BK ProFocus video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* iniFileName = imageAcquisitionConfig->GetAttribute("IniFileName"); 
  if ( iniFileName != NULL) 
  {
    this->SetIniFileName(iniFileName); 
  }

  const char* imagingMode = imageAcquisitionConfig->GetAttribute("ImagingMode"); 
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


  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetAttribute("IniFileName", this->IniFileName);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusVideoSource::GetFullIniFilePath(std::string &fullPath)
{
  if (this->IniFileName==NULL)
  {
    LOG_ERROR("Ini file name has not been set");
    return PLUS_FAIL;
  }
  fullPath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory() + std::string("/") + this->IniFileName;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkBkProFocusVideoSource::SetImagingMode(ImagingModeType imagingMode)
{
  this->Internal->PlusReceiver.SetImagingMode(imagingMode);
}
