#include "PlusConfigure.h"

#include "vtkVideoSource2.h"
#include "vtkVideoBuffer.h"
#include "vtkCriticalSection.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMultiThreader.h"
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

vtkCxxRevisionMacro(vtkVideoSource2, "$Revision: 2.0 $");
vtkStandardNewMacro(vtkVideoSource2);

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif 

//----------------------------------------------------------------------------
vtkVideoSource2::vtkVideoSource2()
{
	this->Initialized = 0;

	this->Recording = 0;

	this->FrameRate = 30;

	this->FrameCount = 0;
	this->FrameNumber = 0; 

	//this->StartTimeStamp = 0;
	this->FrameTimeStamp = 0;

	this->OutputNeedsInitialization = 1;

	this->NumberOfOutputFrames = 1;

	for (int i = 0; i < 3; i++)
	{
		this->DataSpacing[i] = 1.0;
		this->DataOrigin[i] = 0.0;
	}

	this->RecordThreader = vtkMultiThreader::New();
	this->RecordThreadId = -1;

	this->CurrentVideoBufferItem = new VideoBufferItem(); 

	this->Buffer = vtkVideoBuffer::New();

	this->UpdateWithDesiredTimestamp = 0;
	this->DesiredTimestamp = -1;
	this->TimestampClosestToDesired = -1;

	this->SetNumberOfInputPorts(0);

	this->PseudoRandomNoiseFrame = NULL; 

    this->UsImageOrientation = US_IMG_ORIENT_XX;  

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
	this->DebugInfoStream.open ("VideoBufferTimestamps.txt", ios::out);
	this->DebugInfoStream << "FrameNumber\tFrameNumberDifference\tUnfilteredTimestamp\tFilteredTimestamp\tUnfilteredTimeDifference\tSamplingPeriod\tEstimatedFramePeriod" << std::endl; 
	this->DebugInfoStream.flush(); 
#endif
}

//----------------------------------------------------------------------------
vtkVideoSource2::~vtkVideoSource2()
{ 
	// we certainly don't want to access a virtual 
	// function after the subclass has destructed!!
	this->vtkVideoSource2::ReleaseSystemResources();

	this->SetFrameBufferSize(0);
	this->Buffer->Delete();
	if ( this->RecordThreader != NULL )
	{
		this->RecordThreader->Delete();
	}

	if ( this->CurrentVideoBufferItem != NULL )
	{
		delete this->CurrentVideoBufferItem; 
		this->CurrentVideoBufferItem = NULL; 
	}

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO
	this->DebugInfoStream.close(); 
#endif

}

//----------------------------------------------------------------------------
void vtkVideoSource2::PrintSelf(ostream& os, vtkIndent indent)
{
	int idx;

	this->Superclass::PrintSelf(os,indent);


	os << indent << "DataSpacing: (" << this->DataSpacing[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataSpacing[idx];
	}
	os << ")\n";

	os << indent << "DataOrigin: (" << this->DataOrigin[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataOrigin[idx];
	}
	os << ")\n";

	os << indent << "FrameRate: " << this->FrameRate << "\n";

	os << indent << "FrameCount: " << this->FrameCount << "\n";

	os << indent << "Recording: " << (this->Recording ? "On\n" : "Off\n");

	os << indent << "NumberOfOutputFrames: " << this->NumberOfOutputFrames << "\n";

	os << indent << "Buffer:\n";
	this->Buffer->PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
void vtkVideoSource2::SetFrameSize(int x, int y)
{
	int* frameSize = this->Buffer->GetFrameSize();

	if (x == frameSize[0] && 
		y == frameSize[1] )
	{
		return;
	}

	if (x < 1 || y < 1) 
	{
		LOG_ERROR("SetFrameSize: Illegal frame size");
		return;
	}

	if (this->Initialized) 
	{
		this->Buffer->SetFrameSize(x,y);
		this->UpdateFrameBuffer();
	}
	else
	{
        this->Buffer->SetFrameSize(x,y);
	}

	this->Modified();
}

//----------------------------------------------------------------------------
int* vtkVideoSource2::GetFrameSize()
{
	return this->Buffer->GetFrameSize();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::GetFrameSize(int &x, int &y)
{
	int *dim = new int[2];
	this->GetFrameSize(dim);
	x = dim[0];
	y = dim[1];
	delete[] dim;
}

//----------------------------------------------------------------------------
void vtkVideoSource2::GetFrameSize(int dim[2])
{
	this->Buffer->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
void vtkVideoSource2::SetFrameRate(float rate)
{
	if (this->FrameRate == rate)
	{
		return;
	}

	this->FrameRate = rate;
	this->Modified();
}

//----------------------------------------------------------------------------
int vtkVideoSource2::GetFrameBufferSize()
{
	return this->Buffer->GetBufferSize();
}

//----------------------------------------------------------------------------
// set or change the circular buffer size
// you will have to override this if you want the buffers 
// to be device-specific (i.e. something other than vtkDataArray)
PlusStatus vtkVideoSource2::SetFrameBufferSize(int bufsize)
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
	this->UpdateFrameBuffer();

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkVideoSource2::UpdateFrameBuffer()
{
	this->Buffer->UpdateBufferFrameFormats(); 
}

//----------------------------------------------------------------------------
// Initialize() should be overridden to initialize the hardware frame grabber
PlusStatus vtkVideoSource2::Initialize()
{
	if (this->Initialized)
	{
		return PLUS_SUCCESS;
	}
	this->Initialized = 1;

	this->UpdateFrameBuffer();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Connect() should be overridden to connect to the hardware 
PlusStatus vtkVideoSource2::Connect()
{
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Disconnect() should be overridden to disconnect from the hardware 
PlusStatus vtkVideoSource2::Disconnect()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// ReleaseSystemResources() should be overridden to release the hardware
void vtkVideoSource2::ReleaseSystemResources()
{
	if (this->Recording)
	{
		this->StopRecording();
	}

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
// Copy pseudo-random noise into the frames.  This function may be called
// asynchronously.
PlusStatus vtkVideoSource2::InternalGrab()
{

	if ( this->PseudoRandomNoiseFrame.IsNull() )
	{
        this->PseudoRandomNoiseFrame = ImageType::New(); 
        ImageType::SizeType size = { this->GetFrameSize()[0], this->GetFrameSize()[1]};
        ImageType::IndexType start = {0,0};
        ImageType::RegionType region;
        region.SetSize(size);
        region.SetIndex(start);
        this->PseudoRandomNoiseFrame->SetRegions(region);
        try 
        {
            this->PseudoRandomNoiseFrame->Allocate();
        }
        catch(itk::ExceptionObject & err)
        {
            LOG_ERROR("Failed to allocate memory for the image: " << err.GetDescription() ); 
            return PLUS_FAIL; 
        }
	}

	static int randsave = 0;
	int randNum;
	int *lptr;

	// get the number of bytes needed to store one frame
	int totalSize = this->GetFrameSize()[0] * this->GetFrameSize()[1] * sizeof(PixelType); 

	randNum = randsave;

	// get the pointer to the first pixel of the current frame
	PixelType* ptr = this->PseudoRandomNoiseFrame->GetBufferPointer();

	// Somebody should check this:
	lptr = (int *)(((((long)ptr) + 3)/4)*4);
	int i = totalSize/4;

	while (--i >= 0)
	{
		randNum = 1664525*randNum + 1013904223;
		*lptr++ = randNum;
	}
	unsigned char *ptr1 = ptr + 4;
	i = (totalSize-4)/16;
	while (--i >= 0)
	{
		randNum = 1664525*randNum + 1013904223;
		*ptr1 = randNum;
		ptr1 += 16;
	}
	randsave = randNum;

	const long frameNumber = this->FrameNumber + 1; 
  const int numberOfBitsPerPixel = this->PseudoRandomNoiseFrame->GetNumberOfComponentsPerPixel() * sizeof(PixelType)*8; 

	PlusStatus status = this->Buffer->AddItem(ptr, this->GetUsImageOrientation(), this->GetFrameSize(), numberOfBitsPerPixel, 0, frameNumber);
	if ( status != PLUS_SUCCESS )
	{
		LOG_WARNING("vtkVideoSource2: Failed to add frame to video buffer."); 
	}
	else
	{
		this->FrameNumber = frameNumber; 
	}
	this->Modified();

	return status;
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
static void *vtkVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data)
{
	vtkVideoSource2 *self = (vtkVideoSource2 *)(data->UserData);

	double startTime = vtkAccurateTimer::GetSystemTime();
	double rate = self->GetFrameRate();
	unsigned long frame = 0;

	do
	{
		self->InternalGrab();
		frame++;
	}
	while (vtkThreadSleep(data, startTime + frame/rate));

	return NULL;
}

//----------------------------------------------------------------------------
// Set the source to grab frames continuously.
// You should override this as appropriate for your device.  
PlusStatus vtkVideoSource2::StartRecording()
{
	if (!this->Recording)
	{
		this->Initialize();
		this->Recording = 1;
		this->FrameCount = 0;
		this->Modified();
		this->RecordThreadId = 
			this->RecordThreader->SpawnThread((vtkThreadFunctionType)\
			&vtkVideoSourceRecordThread,this);
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Stop continuous grabbing.  You will have to override this
// if your class overrides Record()
PlusStatus vtkVideoSource2::StopRecording()
{
	if (this->Recording)
	{
		this->RecordThreader->TerminateThread(this->RecordThreadId);
		this->RecordThreadId = -1;
		this->Recording = 0;
		this->Modified();
	}

  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
// The grab function, which should (of course) be overridden to do
// the appropriate hardware stuff.  This function should never be
// called asynchronously.
PlusStatus vtkVideoSource2::Grab()
{
	// ensure that the hardware is initialized.
	if (this->Initialize()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

	return this->InternalGrab();
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkVideoSource2::RequestInformation(vtkInformation * vtkNotUsed(request),
										vtkInformationVector **vtkNotUsed(inputVector),
										vtkInformationVector *outputVector)
{
    // ensure that the hardware is initialized.
	this->Initialize();

 	// get the info objects
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Set extent
    int extent[6] = {0, this->Buffer->GetFrameSize()[0] - 1, 0, this->Buffer->GetFrameSize()[1] - 1, 0, 0 }; 
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

	// set the spacing
	outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);

	// set the origin.
	outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);

	// set default data type - unsigned char and number of components 1 
	vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 1);

	return 1;
}

//----------------------------------------------------------------------------
// The Execute method is fairly complex, so I would not recommend overriding
// it unless you have to.  Override the UnpackRasterLine() method in
// vtkVideoFrame2 instead.
int vtkVideoSource2::RequestData(vtkInformation *vtkNotUsed(request),
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
			return 0; 
		}	
	}
	else
	{
		// get the most recent frame if we are not updating with the desired timestamp
		ItemStatus itemStatus = this->Buffer->GetLatestVideoBufferItem(this->CurrentVideoBufferItem); 
		if ( itemStatus != ITEM_OK )
		{
			LOG_ERROR("Unable to copy video data to the requested output!"); 
			return 0; 
		}
	}

	this->FrameTimeStamp = this->CurrentVideoBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffset() ); 
	this->TimestampClosestToDesired = this->CurrentVideoBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffset() ); 

    memcpy( outPtr, this->CurrentVideoBufferItem->GetFrame()->GetBufferPointer(), this->CurrentVideoBufferItem->GetFrameSizeInBytes() ); 

	return 1;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVideoSource2::WriteConfiguration(vtkXMLDataElement* config)
{
  LOG_ERROR("Not implemented");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVideoSource2::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkVideoSource2::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure video source! (XML data element is NULL)"); 
		return PLUS_FAIL; 
	}

	int frameSize[2] = {0, 0}; 
	if ( config->GetVectorAttribute("FrameSize", 2, frameSize) )
	{
		this->SetFrameSize(frameSize[0], frameSize[1]); 
	}

	int bufferSize = 0; 
	if ( config->GetScalarAttribute("BufferSize", bufferSize) )
	{
		if ( this->GetBuffer()->SetBufferSize(bufferSize) != PLUS_SUCCESS )
        {
            LOG_ERROR("Failed to set video buffer size!"); 
        }
	}

	int frameRate = 0; 
	if ( config->GetScalarAttribute("FrameRate", frameRate) )
	{
		this->SetFrameRate(frameRate); 
	}

	double smoothingFactor = 0; 
	if ( config->GetScalarAttribute("SmoothingFactor", smoothingFactor) )
	{
		this->GetBuffer()->SetSmoothingFactor(smoothingFactor); 
	}
  else
  {
    LOG_WARNING("Unable to find ImageAcqusition SmoothingFactor attribute in configuration file!"); 
  }

	double localTimeOffset = 0; 
	if ( config->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
	{
		LOG_INFO("Image acqusition local time offset: " << 1000*localTimeOffset << "ms" ); 
		this->GetBuffer()->SetLocalTimeOffset(localTimeOffset); 
	}

    const char* usImageOrientation = config->GetAttribute("UsImageOrientation"); 
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
PlusStatus vtkVideoSource2::GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
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
  LOG_INFO(reportFile); 

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

#endif
}