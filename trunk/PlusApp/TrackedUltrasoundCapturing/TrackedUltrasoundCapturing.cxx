#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "TrackedUltrasoundCapturing.h"
#include "vtkDataCollectorSynchronizer.h"

#include "vtkObjectFactory.h"
#include "vtkOutputWindow.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkTrackedFrameList.h"



#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkTextProperty.h"
#include "vtkSmartPointer.h"

#include "vtkImageExport.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkDirectory.h"

#include "vtkTrackerTool.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"


/*
		// Change the main tool to the next active
			case '+':
				{	
					capturing->SetMainToolNumber(capturing->GetNextActiveToolNumber()); 
					LOG_INFO( "Active tool changed to port " << capturing->GetMainToolNumber() << " (" 
						<< capturing->GetDataCollector()->GetTracker()->GetTool(capturing->GetMainToolNumber())->GetToolPartNumber() << ")." );
					break;
				}
				// Change the main tool to the previous active
			case '-':
				{	
					capturing->SetMainToolNumber(capturing->GetPreviousActiveToolNumber()); 
					LOG_INFO( "Active tool changed to port " << capturing->GetMainToolNumber() << " (" 
						<< capturing->GetDataCollector()->GetTracker()->GetTool(capturing->GetMainToolNumber())->GetToolPartNumber() << ")." );
					break;
				}
};
*/

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(TrackedUltrasoundCapturing, "$Revision: 1.0 $");
TrackedUltrasoundCapturing* TrackedUltrasoundCapturing::Instance = 0;

//----------------------------------------------------------------------------
TrackedUltrasoundCapturing* TrackedUltrasoundCapturing::New()
{
	return TrackedUltrasoundCapturing::GetInstance();
}


//----------------------------------------------------------------------------
TrackedUltrasoundCapturing* TrackedUltrasoundCapturing::GetInstance()
{
	if(!TrackedUltrasoundCapturing::Instance)
	{
		// Try the factory first
		TrackedUltrasoundCapturing::Instance = (TrackedUltrasoundCapturing*)vtkObjectFactory::CreateInstance("TrackedUltrasoundCapturing");    
		if(!TrackedUltrasoundCapturing::Instance)
		{
			TrackedUltrasoundCapturing::Instance = new TrackedUltrasoundCapturing;	   
		}
	}
	// return the instance
	return TrackedUltrasoundCapturing::Instance;
}


//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::SetInstance(TrackedUltrasoundCapturing* instance)
{
	if (TrackedUltrasoundCapturing::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (TrackedUltrasoundCapturing::Instance)
	{
		TrackedUltrasoundCapturing::Instance->Delete();
	}

	TrackedUltrasoundCapturing::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}


//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 


//----------------------------------------------------------------------------
TrackedUltrasoundCapturing::TrackedUltrasoundCapturing()
{
	this->RealtimeRenderer = NULL; 
	this->RealtimeImageActor = NULL; 
	this->UpdateRequestCallbackFunction = NULL; 
	this->ProgressBarUpdateCallbackFunction = NULL; 
	this->SynchronizingOff(); 
	this->RecordingOff(); 
	this->DataCollector = NULL; 
	this->RecordingStartTime = 0.0; 
	this->FrameRate = 10; 
	this->OutputFolder = NULL; 
	this->ImageSequenceFileName = NULL; 
	this->InputConfigFileName = NULL;
	this->TrackedFrameContainer = NULL;
}


//----------------------------------------------------------------------------
TrackedUltrasoundCapturing::~TrackedUltrasoundCapturing()
{
	this->SetDataCollector(NULL); 
	this->SetRealtimeRenderer(NULL); 
	this->SetRealtimeImageActor(NULL); 

	if ( this->TrackedFrameContainer != NULL )
	{
		this->TrackedFrameContainer->Clear(); 
		this->TrackedFrameContainer->Delete(); 
		this->TrackedFrameContainer = NULL; 
	}
}

//----------------------------------------------------------------------------
PlusStatus TrackedUltrasoundCapturing::Initialize()
{
	LOG_TRACE("TrackedUltrasoundCapturing::Initialize"); 
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	this->SetDataCollector(dataCollector); 

	if ( this->DataCollector->ReadConfigurationFromFile(this->GetInputConfigFileName()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read configuration file!"); 
		return PLUS_FAIL; 
  }
	if ( this->DataCollector->Initialize() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to initialize DataCollector!"); 
		return PLUS_FAIL; 
  }

	if ( !this->DataCollector->GetInitialized() )
	{
		LOG_ERROR("Unable to initialize DataCollector!"); 
		return PLUS_FAIL; 
	}

  if ( this->TrackedFrameContainer == NULL )
	{
		this->TrackedFrameContainer = vtkTrackedFrameList::New(); 
	}

	vtkSmartPointer<vtkImageActor> realtimeImageActor = vtkSmartPointer<vtkImageActor>::New();
	realtimeImageActor->VisibilityOn(); 
	this->SetRealtimeImageActor(realtimeImageActor); 

	realtimeImageActor->SetInput( this->DataCollector->GetOutput() ); 

	// Set up the realtime renderer
	vtkSmartPointer<vtkRenderer> realtimeRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	realtimeRenderer->SetBackground(0,0,0); 
	this->SetRealtimeRenderer(realtimeRenderer); 

	// Add image actor to the realtime renderer 
	this->GetRealtimeRenderer()->AddActor(this->GetRealtimeImageActor()); 

	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ( dir->Open(this->GetOutputFolder()) == 0 ) 
	{	
		dir->MakeDirectory(this->GetOutputFolder()); 
	}

  return PLUS_SUCCESS; 
}; 

//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::ClearTrackedFrameContainer()
{
	LOG_TRACE("TrackedUltrasoundCapturing::ClearTrackedFrameContainer"); 
	this->TrackedFrameContainer->Clear(); 
}

//----------------------------------------------------------------------------
int TrackedUltrasoundCapturing::GetNumberOfRecordedFrames()
{ 
	//LOG_TRACE("TrackedUltrasoundCapturing::GetNumberOfRecordedFrames"); 
	int numOfFrames(0); 
	if ( this->TrackedFrameContainer )
	{
		numOfFrames = this->TrackedFrameContainer->GetNumberOfTrackedFrames(); 
	}

	return numOfFrames; 
}

//----------------------------------------------------------------------------
double TrackedUltrasoundCapturing::GetLastRecordedFrameTimestamp()
{ 
	LOG_TRACE("TrackedUltrasoundCapturing::GetLastRecordedFrameTimestamp"); 
	double timestamp(0); 
	if ( this->TrackedFrameContainer->GetNumberOfTrackedFrames() > 0 )
	{
		timestamp = this->TrackedFrameContainer->GetTrackedFrameList().back()->Timestamp; 
	}

	return timestamp; 
}

//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::AddTrackedFrame( TrackedFrame* trackedFrame )
{
	LOG_TRACE("TrackedUltrasoundCapturing::AddTrackedFrame");
	if (this->UpdateRequestCallbackFunction != NULL)
	{	
		// Request an update before each image acquisition 
		(*UpdateRequestCallbackFunction)();
	}

	bool isDataUnique(false); 
	if ( this->GetDataCollector()->GetTracker() != NULL )
	{
    // TODO Validate data does not determine uniqueness anymore, it validates speed too - in case it changes anything here...
		isDataUnique = this->TrackedFrameContainer->ValidateData(trackedFrame); 
	}
	else
	{
		// If we don't have tracking device, we don't need to validate status and position
		isDataUnique = this->TrackedFrameContainer->ValidateData(trackedFrame, true, false, false); 
	}

	if ( !isDataUnique )
	{
		LOG_DEBUG("We've already inserted this frame into the sequence."); 
		return; 
	}

	this->TrackedFrameContainer->AddTrackedFrame(trackedFrame); 

	LOG_DEBUG( "Added new tracked frame to container..." ); 
}

//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::StartRecording()
{
	LOG_TRACE("TrackedUltrasoundCapturing::StartRecording");
	LOG_INFO( "Recording started..." ); 
	this->RecordingOn(); 
}

//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::StopRecording()
{
	LOG_TRACE("TrackedUltrasoundCapturing::StopRecording");
	LOG_INFO( "Recording stopped..." ); 
	this->RecordingOff(); 
	this->SetRecordingStartTime(0.0); 

	LOG_DEBUG("Recording stop time: " << std::fixed << this->GetLastRecordedFrameTimestamp()); 
}

//----------------------------------------------------------------------------
PlusStatus TrackedUltrasoundCapturing::UpdateRecording()
{
	LOG_TRACE("TrackedUltrasoundCapturing::UpdateRecording");
	if ( !this->Recording )
	{	
		LOG_DEBUG("No need to update recording: recording stoppped!"); 
		return PLUS_FAIL; 
	}

	double newestTimestamp(0);
  if (this->GetDataCollector()->GetMostRecentTimestamp(newestTimestamp)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot update recording, most recent timestamp is not available");
    return PLUS_FAIL; 
  }
	const double samplingTime = (1.0 / this->GetFrameRate()); 

	if ( this->GetRecordingStartTime() == 0 )
	{
		this->SetRecordingStartTime(newestTimestamp); 
		this->RecordTrackedFrame(newestTimestamp); 
	}

	double lastTimestamp = this->GetLastRecordedFrameTimestamp(); 

  double oldestTimestamp(0); 
  if ( this->DataCollector->GetVideoSource()->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
  {
    LOG_WARNING("Failed to get oldest frame timestamp from video buffer"); 
  }
 
  if ( oldestTimestamp == 0 || lastTimestamp < oldestTimestamp )
  {
    if ( this->DataCollector->GetVideoSource()->GetBuffer()->GetLatestTimeStamp(lastTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Failed to get latest frame timestamp from video buffer"); 
    }
  }
	
  PlusStatus status = PLUS_SUCCESS; 
	while ( status == PLUS_SUCCESS && lastTimestamp + samplingTime <= newestTimestamp )
	{
		status = this->RecordTrackedFrame(lastTimestamp + samplingTime ); 
    if ( status == PLUS_SUCCESS )
    {
		  lastTimestamp = lastTimestamp + samplingTime; 
		  vtksys::SystemTools::Delay(0); 
    }
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TrackedUltrasoundCapturing::RecordTrackedFrame( const double time /*=0*/)
{
	LOG_TRACE("TrackedUltrasoundCapturing::RecordTrackedFrame");

	TrackedFrame trackedFrame; 

  PlusStatus status = PLUS_FAIL; 
	if ( time == 0 )
	{
		status = this->GetDataCollector()->GetTrackedFrame(&trackedFrame); 
	}
	else
	{
		status = this->GetDataCollector()->GetTrackedFrameByTime(time, &trackedFrame); 
	}
  
  if ( status == PLUS_FAIL )
  {
    LOG_ERROR("Failed to get tracked frame!"); 
    return PLUS_FAIL; 
  }

	if ( trackedFrame.Status != TR_OK )
	{
		LOG_WARNING("Unable to record tracked frame: Tracker out of view!"); 
	}
	else
	{		
		this->AddTrackedFrame(&trackedFrame); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::SynchronizeFrameToTracker()
{
	LOG_TRACE("TrackedUltrasoundCapturing::SynchronizeFrameToTracker");
	this->SynchronizingOn(); 
	this->DataCollector->SetProgressBarUpdateCallbackFunction( this->ProgressBarUpdateCallbackFunction ); 
	this->DataCollector->Synchronize(); 
	this->SynchronizingOff(); 
}

//----------------------------------------------------------------------------
void TrackedUltrasoundCapturing::SetLocalTimeOffset(double videoOffset, double trackerOffset)
{
	LOG_TRACE("TrackedUltrasoundCapturing::SetLocalTimeOffset");
	if ( this->DataCollector != NULL )
	{
		this->DataCollector->SetLocalTimeOffset(videoOffset, trackerOffset); 
	}
}



//----------------------------------------------------------------------------
double TrackedUltrasoundCapturing::GetVideoOffsetMs()
{
	LOG_TRACE("TrackedUltrasoundCapturing::GetVideoOffsetMs");
	if (this->DataCollector->GetVideoSource() != NULL) {
		return (1000 * this->DataCollector->GetVideoSource()->GetBuffer()->GetLocalTimeOffset() ); 
	} else {
		return 0.0;
	}
}


//----------------------------------------------------------------------------
PlusStatus TrackedUltrasoundCapturing::SaveData()
{
	LOG_TRACE("TrackedUltrasoundCapturing::SaveData");
	std::ostringstream filename; 
	filename << vtkAccurateTimer::GetDateAndTimeString() << "_" << this->ImageSequenceFileName; 
	if ( this->TrackedFrameContainer->SaveToSequenceMetafile(this->OutputFolder, filename.str().c_str() , vtkTrackedFrameList::SEQ_METAFILE_MHA, false) != PLUS_SUCCESS )
  {
     LOG_ERROR("Failed to save tracked frames to sequence metafile - try to free some memory!"); 
     return PLUS_FAIL; 
  }

  this->TrackedFrameContainer->Clear(); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedUltrasoundCapturing::SaveAsData( const char* filePath )
{
	LOG_TRACE("TrackedUltrasoundCapturing::SaveAsData");
	std::string path = vtksys::SystemTools::GetFilenamePath(filePath); 
	std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(filePath); 
	std::string extension = vtksys::SystemTools::GetFilenameExtension(filePath); 

	vtkTrackedFrameList::SEQ_METAFILE_EXTENSION ext(vtkTrackedFrameList::SEQ_METAFILE_MHA); 
	if ( STRCASECMP(".mhd", extension.c_str() ) == 0 )
	{
		ext = vtkTrackedFrameList::SEQ_METAFILE_MHD; 
	}
	else
	{
		ext = vtkTrackedFrameList::SEQ_METAFILE_MHA; 
	}

	if ( this->TrackedFrameContainer->SaveToSequenceMetafile(path.c_str(), filename.c_str() , ext, false) != PLUS_SUCCESS )
  {
     LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
     return PLUS_FAIL;
  }

	this->TrackedFrameContainer->Clear(); 
  return PLUS_SUCCESS; 
}

/*int main(int argc, char* argv[])
	args.AddArgument("--tool-number", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolNumber, "Set main tool port number from strober - starting from 0 (Default: 0)" );
	args.AddArgument("--reference-tool-number", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolNumber, "Set main tool port number from strober - starting from 0 (Default: 0)" );
	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputOutputFolder, "Path to the output folder ( Default: ./)" );
	args.AddArgument("--output-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageSequenceFileName, "Filename of the output image sequence (Default: SonixRPSequence.mhd)");
	
*/

