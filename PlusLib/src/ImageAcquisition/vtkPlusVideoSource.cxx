#include "PlusConfigure.h"

#include "vtkPlusVideoSource.h"
#include "vtkVideoBuffer.h"
#include "vtkCriticalSection.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMutexLock.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"
#include "vtkWindows.h"
#include "vtkDoubleArray.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkMetaImageWriter.h"
#include "vtkPNGWriter.h"
#include "vtkPNMWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkMetaImageReader.h"
#include "vtkPNGReader.h"
#include "vtkPNMReader.h"
#include "vtkTIFFReader.h"
#include "vtkImageData.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"


#include <ctype.h>
#include <time.h>

vtkCxxRevisionMacro(vtkPlusVideoSource, "$Revision: 2.0 $");
vtkStandardNewMacro(vtkPlusVideoSource);

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif 

//----------------------------------------------------------------------------
vtkPlusVideoSource::vtkPlusVideoSource()
{
	this->Connected = 0;
  
  this->SpawnThreadForRecording=0;

	this->Recording = 0;

	this->FrameRate = 30;

	this->FrameCount = 0;
	this->FrameNumber = 0; 

	//this->StartTimeStamp = 0;
	this->FrameTimeStamp = 0;

	this->OutputNeedsInitialization = 1;

	this->NumberOfOutputFrames = 1;

	this->RecordThreader = vtkMultiThreader::New();
	this->RecordThreadId = -1;

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

	if ( this->RecordThreader != NULL )
	{
		this->RecordThreader->Delete();
    this->RecordThreader=NULL;
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

	os << indent << "FrameRate: " << this->FrameRate << "\n";
	os << indent << "FrameCount: " << this->FrameCount << "\n";
	os << indent << "Connected: " << (this->Connected ? "Yes\n" : "No\n");
  os << indent << "SpawnThreadForRecording: " << (this->SpawnThreadForRecording ? "Yes\n" : "No\n");
	os << indent << "Recording: " << (this->Recording ? "On\n" : "Off\n");
	os << indent << "NumberOfOutputFrames: " << this->NumberOfOutputFrames << "\n";
	os << indent << "Buffer:\n";

  this->Buffer->PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::SetFrameSize(int x, int y)
{
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
	return this->Buffer->GetFrameSize();
}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::GetFrameSize(int &x, int &y)
{
	int dim[2];
	this->GetFrameSize(dim);
	x = dim[0];
	y = dim[1];
}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::GetFrameSize(int dim[2])
{
	this->Buffer->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::SetPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  return this->Buffer->SetPixelType(pixelType);
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkPlusVideoSource::GetPixelType()
{
  return this->Buffer->GetPixelType();
}

//----------------------------------------------------------------------------
void vtkPlusVideoSource::SetFrameRate(float rate)
{
	if (this->FrameRate == rate)
	{
		return;
	}

	this->FrameRate = rate;
	this->Modified();
}

//----------------------------------------------------------------------------
int vtkPlusVideoSource::GetFrameBufferSize()
{
	return this->Buffer->GetBufferSize();
}

//----------------------------------------------------------------------------
// set or change the circular buffer size
// you will have to override this if you want the buffers 
// to be device-specific (i.e. something other than vtkDataArray)
PlusStatus vtkPlusVideoSource::SetFrameBufferSize(int bufsize)
{
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
// Sleep until the specified absolute time has arrived.
// You must pass a handle to the current thread.  
// If '0' is returned, then the thread was aborted before or during the wait.
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time)
{
	// loop either until the time has arrived or until the thread is ended
	for (int i = 0;; i++)
	{
		double remaining = time - vtkAccurateTimer::GetSystemTime();

		// check to see if we have reached the specified time
		if (remaining <= 0)
		{
			if (i == 0)
			{
				//vtkGenericWarningMacro("Dropped a video frame.");
				//std::cout << "dropped a video frame" << std::endl; // TODO put back
			}
			return 1;
		}
		// check the ActiveFlag at least every 0.1 seconds
		if (remaining > 0.1)
		{
			remaining = 0.1;
		}

		// check to see if we are being told to quit 
		data->ActiveFlagLock->Lock();
		int activeFlag = *(data->ActiveFlag);
		data->ActiveFlagLock->Unlock();

		if (activeFlag == 0)
		{
			break;
		}

		vtkAccurateTimer::Delay(remaining);
	}

	return 0;
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asyncronously grab frames
void* vtkPlusVideoSource::vtkVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data)
{
	vtkPlusVideoSource *self = (vtkPlusVideoSource *)(data->UserData);

	double startTime = vtkAccurateTimer::GetSystemTime();
	double rate = self->GetFrameRate();
	unsigned long frame = 0;

	do
	{
		if (!self->GetRecording())
		{
			// recording stopped
			LOG_DEBUG("Recording stopped");
			return NULL;
		}
		self->InternalGrab();
		frame++;
	}
	while (vtkThreadSleep(data, startTime + frame/rate));

	return NULL;
}

//----------------------------------------------------------------------------
// Set the source to grab frames continuously.
// You should override this as appropriate for your device.  
PlusStatus vtkPlusVideoSource::StartRecording()
{
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
  
  this->FrameCount = 0;

  if (this->InternalStartRecording()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot start recording, internal StartRecording failed");
    return PLUS_FAIL;
  }

  this->Recording = 1;

  if (SpawnThreadForRecording)
  {
    this->RecordThreadId = 
      this->RecordThreader->SpawnThread((vtkThreadFunctionType)\
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
	if (!this->Recording)
 	{
    LOG_DEBUG("Recording is already inactive");
    return PLUS_SUCCESS;
  }

  this->Recording = 0;
  this->RecordThreadId = -1; // it might be useful to actually wait for the thread completion

  InternalStopRecording();
  
  this->Modified();

  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::Grab()
{
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
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, UsImageConverterCommon::GetVTKScalarPixelType(this->Buffer->GetPixelType()), 1);

	return 1;
}

//----------------------------------------------------------------------------
// The Execute method is fairly complex, so I would not recommend overriding
// it unless you have to.  Override the UnpackRasterLine() method instead.
int vtkPlusVideoSource::RequestData(vtkInformation *vtkNotUsed(request),
								 vtkInformationVector **vtkNotUsed(inputVector),
								 vtkInformationVector *vtkNotUsed(outputVector))
{
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

  this->FrameTimeStamp = this->CurrentVideoBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffset() ); 
  this->TimestampClosestToDesired = this->CurrentVideoBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffset() ); 

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

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize()); 

  imageAcquisitionConfig->SetDoubleAttribute("LocalTimeOffset", this->GetBuffer()->GetLocalTimeOffset() ); 

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

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
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

	int frameRate = 0; 
	if ( imageAcquisitionConfig->GetScalarAttribute("FrameRate", frameRate) )
	{
		this->SetFrameRate(frameRate); 
	}

	int averagedItemsForFiltering = 0; 
	if ( imageAcquisitionConfig->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
	{
		this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering); 
	}
  else
  {
    LOG_WARNING("Unable to find ImageAcquisition NumberOfAveragedItems attribute in configuration file!"); 
  }

	double localTimeOffset = 0; 
	if ( imageAcquisitionConfig->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
	{
		LOG_INFO("Image acqusition local time offset: " << 1000*localTimeOffset << "ms" ); 
		this->GetBuffer()->SetLocalTimeOffset(localTimeOffset); 
	}

  const char* usImageOrientation = imageAcquisitionConfig->GetAttribute("UsImageOrientation"); 
  if ( usImageOrientation != NULL )
  {
      LOG_INFO("Selected US image orientation: " << usImageOrientation ); 
      this->SetUsImageOrientation( UsImageConverterCommon::GetUsImageOrientationFromString(usImageOrientation) ); 
      if ( this->GetUsImageOrientation() == US_IMG_ORIENT_XX )
      {
          LOG_ERROR("Ultrasound image orientation is undefined - please set a proper image orientation!"); 
      }
  }
  else
  {
      LOG_ERROR("Ultrasound image orientation is not defined in the configuration file - set to undefined by default!"); 
      this->SetUsImageOrientation(US_IMG_ORIENT_XX ); 
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSource::GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
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

  std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/VideoBufferTimestamps.txt"); 

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

	std::string plotBufferTimestampScript = gnuplotScriptsFolder + std::string("/PlotBufferTimestamp.gnu"); 
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
