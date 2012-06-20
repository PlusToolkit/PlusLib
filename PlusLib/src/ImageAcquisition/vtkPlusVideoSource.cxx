/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusVideoSource.h"
#include "vtkVideoBuffer.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkWindows.h"
#include "vtkMetaImageWriter.h"
#include "vtkMetaImageReader.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkTrackedFrameList.h"

#include <ctype.h>
#include <time.h>

vtkStandardNewMacro(vtkPlusVideoSource);

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif

//----------------------------------------------------------------------------
vtkPlusVideoSource::vtkPlusVideoSource()
{
  this->Connected = 0;
  this->AcquisitionRate = 30;
  this->FrameTimeStamp = 0;
  this->FrameNumber = 0; 
  this->OutputNeedsInitialization = 1;
  this->NumberOfOutputFrames = 1;
  this->SpawnThreadForRecording = false;
  this->RecordingThreadAlive = false; 
  this->StartTimeAbsoluteUTC = 0.0; 
  this->StartTimeRelative = 0.0; 

  this->CurrentVideoBufferItem = new VideoBufferItem();
  this->Buffer = vtkVideoBuffer::New();

  this->UpdateWithDesiredTimestamp = 0;
  this->DesiredTimestamp = -1;
  this->TimestampClosestToDesired = -1;

  this->SetNumberOfInputPorts(0);

  this->UsImageOrientation = US_IMG_ORIENT_XX; 
}

//----------------------------------------------------------------------------
vtkPlusVideoSource::~vtkPlusVideoSource()
{
  // we certainly don't want to access a virtual
  // function after the subclass has destructed!!
  if (this->Connected)
  {
    Disconnect();
  }

  if ( this->CurrentVideoBufferItem != NULL )
  {
    delete this->CurrentVideoBufferItem;
    this->CurrentVideoBufferItem = NULL;
  }

  this->SetFrameBufferSize(0);
  if (this->Buffer!=NULL)
  {
    this->Buffer->Delete();
    this->Buffer=NULL;
  }

}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Connected: " << (this->Connected ? "Yes\n" : "No\n");
  os << indent << "AcquisitionRate: " << this->AcquisitionRate << "\n";
  os << indent << "SpawnThreadForRecording: " << (this->SpawnThreadForRecording ? "Yes\n" : "No\n");
  os << indent << "NumberOfOutputFrames: " << this->NumberOfOutputFrames << "\n";
  os << indent << "Buffer:\n";

  this->Buffer->PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::SetFrameSize(int x, int y)
{
  LOG_TRACE("vtkPlusVideoSource::SetFrameSize(" << x << ", " << y << ")");

  int* frameSize = this->Buffer->GetFrameSize();

  if (x == frameSize[0] &&
    y == frameSize[1] )
  {
    return PLUS_SUCCESS;
  }

  if (x < 1 || y < 1)
  {
    LOG_ERROR("SetFrameSize: Illegal frame size");
    return PLUS_FAIL;
  }

  this->Buffer->SetFrameSize(x,y); 

  this->Modified();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int* vtkPlusVideoSource::GetFrameSize()
{
  LOG_TRACE("vtkPlusVideoSource::GetFrameSize");

  return this->Buffer->GetFrameSize();
}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::GetFrameSize(int &x, int &y)
{
  LOG_TRACE("vtkPlusVideoSource::GetFrameSize");

  int dim[2];
  this->GetFrameSize(dim);
  x = dim[0];
  y = dim[1];
}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::GetFrameSize(int dim[2])
{
  LOG_TRACE("vtkPlusVideoSource::GetFrameSize");

  this->Buffer->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::SetPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  LOG_TRACE("vtkPlusVideoSource::SetPixelType");

  return this->Buffer->SetPixelType(pixelType);
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkPlusVideoSource::GetPixelType()
{
  LOG_TRACE("vtkPlusVideoSource::GetPixelType");

  return this->Buffer->GetPixelType();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::SetAcquisitionRate(double rate)
{
  LOG_TRACE("vtkPlusVideoSource::SetAcquisitionRate(" << rate << ")");

  if (this->AcquisitionRate == rate)
  {
    return PLUS_SUCCESS;
  }

  this->AcquisitionRate = rate;
  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkPlusVideoSource::GetFrameBufferSize()
{
  LOG_TRACE("vtkPlusVideoSource::GetFrameBufferSize");

  return this->Buffer->GetBufferSize();
}

//----------------------------------------------------------------------------
// set or change the circular buffer size
// you will have to override this if you want the buffers
// to be device-specific (i.e. something other than vtkDataArray)
PlusStatus vtkPlusVideoSource::SetFrameBufferSize(int bufsize)
{
  LOG_TRACE("vtkPlusVideoSource::SetFrameBufferSize(" << bufsize << ")");

  if (bufsize < 0)
  {
    LOG_ERROR("SetFrameBufferSize: There must be at least one framebuffer");
    return PLUS_FAIL;
  }

  // update the buffer size
  if ( this->Buffer->SetBufferSize(bufsize) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to set video buffer size!");
      return PLUS_FAIL;
  }
 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::Connect()
{
  LOG_TRACE("vtkPlusVideoSource::Connect");

  if (this->Connected)
  {
    LOG_DEBUG("Already connected to the video source");
    return PLUS_SUCCESS;
  }

  if (this->InternalConnect()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot connect to video source, ConnectInternal failed");
    return PLUS_FAIL;
  }

  this->Connected = 1;
 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::Disconnect()
{
  LOG_TRACE("vtkPlusVideoSource::Disconnect");

  if (!this->Connected)
  {
    LOG_DEBUG("Video source is already disconnected");
    return PLUS_SUCCESS;
  }
  if (this->Recording)
  {
    this->StopRecording();
  }

  this->Connected = 0;

  InternalDisconnect();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asyncronously grab frames
void* vtkPlusVideoSource::vtkVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkPlusVideoSource *self = (vtkPlusVideoSource *)(data->UserData);

  double startTime = vtkAccurateTimer::GetSystemTime();
  double rate = self->GetAcquisitionRate();
  unsigned long frame = 0;
  self->RecordingThreadAlive = true; 

  while ( self->GetRecording() )
  {
    double newtime = vtkAccurateTimer::GetSystemTime(); 
    self->InternalGrab();
    
    double delay = ( newtime + 1.0 / rate - vtkAccurateTimer::GetSystemTime() );
    if ( delay > 0 )
    {
      vtkAccurateTimer::Delay(delay); 
    }
  }

  self->RecordingThreadAlive = false; 
  return NULL;
}

//----------------------------------------------------------------------------
// Set the source to grab frames continuously.
// You should override this as appropriate for your device. 
PlusStatus vtkPlusVideoSource::StartRecording()
{
  LOG_TRACE("vtkPlusVideoSource::StartRecording");

  if (this->Recording)
  {
    LOG_DEBUG("Recording is already active");
    return PLUS_SUCCESS;
  }

  if (!this->Connected)
  {
    if (this->Connect()!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot start recording, connection failed");
      return PLUS_FAIL;
    }
  }
 
  if (this->InternalStartRecording()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot start recording, internal StartRecording failed");
    return PLUS_FAIL;
  }

  this->Recording = 1;

  if (SpawnThreadForRecording)
  {
    this->ThreadId =
      this->Threader->SpawnThread((vtkThreadFunctionType)\
      &vtkVideoSourceRecordThread,this);
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Stop continuous grabbing.  You will have to override this
// if your class overrides Record()
PlusStatus vtkPlusVideoSource::StopRecording()
{
  LOG_TRACE("vtkPlusVideoSource::StopRecording");

  if (!this->Recording)
   {
    LOG_DEBUG("Recording is already inactive");
    return PLUS_SUCCESS;
  }

  this->Recording = 0;
 
  // Let's give a chance to the thread to stop before we kill the connection
  while ( this->RecordingThreadAlive )
  {
    vtkAccurateTimer::Delay(0.1);
  }
  this->ThreadId = -1; 

  InternalStopRecording();
 
  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::Grab()
{
  //LOG_TRACE("vtkPlusVideoSource::Grab");

  if (!this->Connected)
  {
    if (this->Connect()!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot grab a single frame, connection failed");
      return PLUS_FAIL;
    }
  }

  return this->InternalGrab();
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkPlusVideoSource::RequestInformation(vtkInformation * vtkNotUsed(request),
                    vtkInformationVector **vtkNotUsed(inputVector),
                    vtkInformationVector *outputVector)
{
  //LOG_TRACE("vtkPlusVideoSource::RequestInformation");

  if (!this->Connected)
  {
    Connect();
  }

   // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Set extent
  int extent[6] = {0, this->Buffer->GetFrameSize()[0] - 1, 0, this->Buffer->GetFrameSize()[1] - 1, 0, 0 };
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

  // Set the origin and spacing. The video source provides raw pixel output, therefore the spacing is (1,1,1) and the origin is (0,0)
  double spacing[3]={1,1,1};
  outInfo->Set(vtkDataObject::SPACING(),spacing,3);
  double origin[3]={0,0,0};
  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);

  // set default data type - unsigned char and number of components 1
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, PlusVideoFrame::GetVTKScalarPixelType(this->Buffer->GetPixelType()), 1);

  return 1;
}

//----------------------------------------------------------------------------
// The Execute method is fairly complex, so I would not recommend overriding
// it unless you have to.  Override the UnpackRasterLine() method instead.
int vtkPlusVideoSource::RequestData(vtkInformation *vtkNotUsed(request),
                 vtkInformationVector **vtkNotUsed(inputVector),
                 vtkInformationVector *vtkNotUsed(outputVector))
{
  //LOG_TRACE("vtkPlusVideoSource::RequestData");

  // the output data
  vtkImageData *data = this->AllocateOutputData(this->GetOutput());
  unsigned char *outPtr = (unsigned char *)data->GetScalarPointer();
 
  if ( this->Buffer->GetNumberOfItems() < 1 )
  {
    // If the video buffer is empty, we can return immediately
    LOG_DEBUG("Cannot request data from video source, the video buffer is empty!");
    return 1;
  }

  if (this->UpdateWithDesiredTimestamp && this->DesiredTimestamp != -1)
  {
    ItemStatus itemStatus = this->Buffer->GetVideoBufferItemFromTime(this->DesiredTimestamp, this->CurrentVideoBufferItem);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    } 
  }
  else
  {
    // get the most recent frame if we are not updating with the desired timestamp
    ItemStatus itemStatus = this->Buffer->GetLatestVideoBufferItem(this->CurrentVideoBufferItem);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    }
  }

  this->FrameTimeStamp = this->CurrentVideoBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffsetSec() );
  this->TimestampClosestToDesired = this->CurrentVideoBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffsetSec() );

  void* sourcePtr=this->CurrentVideoBufferItem->GetFrame().GetBufferPointer();
  int bytesToCopy=this->CurrentVideoBufferItem->GetFrame().GetFrameSizeInBytes();

  int dimensions[3]={0,0,0};
  data->GetDimensions(dimensions);
  int outputSizeInBytes=dimensions[0]*dimensions[1]*dimensions[2]*data->GetScalarSize()*data->GetNumberOfScalarComponents();

  // the actual output size may be smaller than the output available
  // (e.g., when the rendering window is resized)
  if (bytesToCopy>outputSizeInBytes)
  {
    bytesToCopy=outputSizeInBytes;
  }

  memcpy( outPtr, sourcePtr, bytesToCopy);

  return 1;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkPlusVideoSource::WriteConfiguration");

  if ( config == NULL )
  {
    LOG_ERROR("Unable to write configuration from video source! (XML data element is NULL)");
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
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize());

  imageAcquisitionConfig->SetDoubleAttribute("LocalTimeOffsetSec", this->GetBuffer()->GetLocalTimeOffsetSec() );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkPlusVideoSource::ReadConfiguration");

  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure video source! (XML data element is NULL)");
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
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  int bufferSize = 0;
  if ( imageAcquisitionConfig->GetScalarAttribute("BufferSize", bufferSize) )
  {
    if ( this->GetBuffer()->SetBufferSize(bufferSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set video buffer size!");
    }
  }

  int acquisitionRate = 0;
  if ( imageAcquisitionConfig->GetScalarAttribute("AcquisitionRate", acquisitionRate) )
  {
    this->SetAcquisitionRate(acquisitionRate);
  }

  int averagedItemsForFiltering = 0;
  if ( imageAcquisitionConfig->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
  }
  else
  {
    LOG_WARNING("Unable to find ImageAcquisition AveragedItemsForFiltering attribute in configuration file!");
  }

  double localTimeOffsetSec = 0;
  if ( imageAcquisitionConfig->GetScalarAttribute("LocalTimeOffsetSec", localTimeOffsetSec) )
  {
    LOG_INFO("Image acqusition local time offset: " << 1000*localTimeOffsetSec << "ms" );
    this->GetBuffer()->SetLocalTimeOffsetSec(localTimeOffsetSec);
  }

  const char* usImageOrientation = imageAcquisitionConfig->GetAttribute("UsImageOrientation");
  if ( usImageOrientation != NULL )
  {
    LOG_INFO("Selected US image orientation: " << usImageOrientation );
    this->SetUsImageOrientation( PlusVideoFrame::GetUsImageOrientationFromString(usImageOrientation) );
    if ( this->GetUsImageOrientation() == US_IMG_ORIENT_XX )
    {
      LOG_ERROR("Ultrasound image orientation is undefined - please set a proper image orientation!");
    }
  }
  else
  {
    LOG_ERROR("Ultrasound image orientation is not defined in the configuration file - set to undefined by default!");
    this->SetUsImageOrientation(US_IMG_ORIENT_XX);
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter)
{
  LOG_TRACE("vtkPlusVideoSource::GenerateVideoDataAcquisitionReport");

  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New();
  if ( this->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get timestamp report table from video buffer!");
    return PLUS_FAIL;
  }

  std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/")
    + std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) + std::string(".VideoBufferTimestamps.txt");

  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( timestampReportTable, reportFile.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write table to file in gnuplot format!");
    return PLUS_FAIL;
  }

  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find video data acquisition report file at: " << reportFile);
    return PLUS_FAIL;
  }

  const char* scriptsFolder = vtkPlusConfig::GetInstance()->GetScriptsDirectory();
  std::string plotBufferTimestampScript = scriptsFolder + std::string("/gnuplot/PlotBufferTimestamp.gnu");
  if ( !vtksys::SystemTools::FileExists( plotBufferTimestampScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotBufferTimestampScript);
    return PLUS_FAIL;
  }

  htmlReport->AddText("Video Data Acquisition Analysis", vtkHTMLGenerator::H1);
  plotter->ClearArguments();
  plotter->AddArgument("-e");
  std::ostringstream videoBufferAnalysis;
  videoBufferAnalysis << "f='" << reportFile << "'; o='VideoBufferTimestamps';" << std::ends;
  plotter->AddArgument(videoBufferAnalysis.str().c_str());
 
  plotter->AddArgument(plotBufferTimestampScript.c_str()); 
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!");
    return PLUS_FAIL;
  }
  plotter->ClearArguments();

  htmlReport->AddImage("VideoBufferTimestamps.jpg", "Video Data Acquisition Analysis");

  htmlReport->AddHorizontalLine();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::InternalGrab()
{
  LOG_ERROR("InternalGrab is not implemented");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::SetAcquisitionStartTime( double relative, double absolute )
{
  this->StartTimeRelative = relative; 
  this->StartTimeAbsoluteUTC = absolute; 
  this->Buffer->SetStartTime( this->StartTimeRelative ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::SetBuffer(vtkVideoBuffer *newBuffer)
{
  if (newBuffer==this->Buffer)
  {
    // no action, the buffer has been set already
    return PLUS_SUCCESS;
  }
  if ( newBuffer == NULL )
  {
    // this->Buffer pointer assumed to be valid (non-NULL) pointer throughout the 
    // vtkPlusVideoSource class and its child classes, therefore setting it to NULL
    // is not allowed
    LOG_ERROR( "A valid buffer is required as input for vtkPlusVideoSource::SetBuffer" );
    return PLUS_FAIL;
  }
  this->Buffer->UnRegister(this);
  this->Buffer=newBuffer;
  this->Buffer->Register(this);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::GetTrackedFrame(double timestamp, TrackedFrame *trackedFrame)
{
  if (!trackedFrame)
  {
    return PLUS_FAIL;
  }

  // Get frame UID
  BufferItemUidType frameUID = 0; 
  ItemStatus status = this->Buffer->GetItemUidFromTime(timestamp, frameUID); 
  if ( status != ITEM_OK )
  {
    if ( status == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
        ") - item not available anymore!"); 
    }
    else if ( status == ITEM_NOT_AVAILABLE_YET )
    {
      LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
        ") - item not available yet!"); 
    }
    else
    {
      LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp << ")!"); 
    }

    return PLUS_FAIL; 
  }

  VideoBufferItem currentVideoBufferItem; 
  if ( this->Buffer->GetVideoBufferItem(frameUID, &currentVideoBufferItem) != ITEM_OK )
  {
    LOG_ERROR("Couldn't get video buffer item by frame UID: " << frameUID); 
    return PLUS_FAIL; 
  }

  // Copy frame 
  PlusVideoFrame frame = currentVideoBufferItem.GetFrame(); 
  trackedFrame->SetImageData(frame);

  // Copy all custom fields
  VideoBufferItem::FieldMapType fieldMap = currentVideoBufferItem.GetCustomFrameFieldMap();
  VideoBufferItem::FieldMapType::iterator fieldIterator;
  for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
  {
    trackedFrame->SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
  }

  // Copy frame timestamp 
  trackedFrame->SetTimestamp(
    currentVideoBufferItem.GetTimestamp(this->Buffer->GetLocalTimeOffsetSec()) );

  return PLUS_SUCCESS; 
}
