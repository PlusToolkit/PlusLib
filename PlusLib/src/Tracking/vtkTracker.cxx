/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sstream>
#include "vtkCharArray.h"
#include "vtkCriticalSection.h"
#include "vtkDoubleArray.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkObjectFactory.h"
#include "vtkSocketCommunicator.h" // VTK_PARALLEL support has to be enabled
#include "vtkTracker.h"
#include "vtkTransform.h"
#include "vtkTimerLog.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtksys/SystemTools.hxx"
#include "vtkAccurateTimer.h"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkTrackedFrameList.h"

vtkStandardNewMacro(vtkTracker);

//----------------------------------------------------------------------------
vtkTracker::vtkTracker()
{
  this->Tracking = 0;
    this->LastUpdateTime = 0;
  this->InternalUpdateRate = 0;
  this->Frequency = 50; 
  this->ToolReferenceFrameName = NULL; 

  // for threaded capture of transformations
  this->Threader = vtkMultiThreader::New();
  this->ThreadId = -1;
  this->UpdateMutex = vtkCriticalSection::New();
  this->RequestUpdateMutex = vtkCriticalSection::New();

  this->SetToolReferenceFrameName("Tracker"); 
}

//----------------------------------------------------------------------------
vtkTracker::~vtkTracker()
{
  this->StopTracking();
  this->Disconnect();

  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
  {
    it->second->SetTracker(NULL); 
    it->second->Delete(); 
  }

  this->Threader->Delete();
  this->UpdateMutex->Delete();
  this->RequestUpdateMutex->Delete();
}

//----------------------------------------------------------------------------
void vtkTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);
  os << indent << "Tracking: " << this->Tracking << "\n";
}

//----------------------------------------------------------------------------
ToolIteratorType vtkTracker::GetToolIteratorBegin()
{
  return this->ToolContainer.begin(); 
}

//----------------------------------------------------------------------------
ToolIteratorType vtkTracker::GetToolIteratorEnd()
{
  return this->ToolContainer.end();
}

//----------------------------------------------------------------------------
int vtkTracker::GetNumberOfTools()
{
  return this->ToolContainer.size(); 
}


//----------------------------------------------------------------------------
PlusStatus vtkTracker::AddTool( vtkTrackerTool* tool )
{
  if ( tool == NULL )
  {
    LOG_ERROR("Failed to add tool to tracker, tool is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( tool->GetToolName() == NULL || tool->GetPortName() == NULL )
  {
    LOG_ERROR("Failed to add tool to tracker, tool Name and PortName must be defined!"); 
    return PLUS_FAIL; 
  }

  if ( this->ToolContainer.find( tool->GetToolName() ) == this->GetToolIteratorEnd() )
  {
    // Check tool port names, it should be unique too
    for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {
      if ( STRCASECMP( tool->GetPortName(), it->second->GetPortName() ) == 0 )
      {
        LOG_ERROR("Failed to add '" << tool->GetToolName() << "' tool to container: tool with name '" << it->second->GetToolName() 
          << "' is already defined on port '" << tool->GetPortName() << "'!"); 
        return PLUS_FAIL; 
      }
    }

    tool->Register(this); 
    tool->SetTracker(this); 
    this->ToolContainer[tool->GetToolName()] = tool; 
  }
  else
  {
    LOG_ERROR("Tool with name '" << tool->GetToolName() << "' is already in the tool conatainer!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::GetFirstActiveTool(vtkTrackerTool* &aTool)
{
  if ( this->GetToolIteratorBegin() == this->GetToolIteratorEnd() )
  {
    LOG_ERROR("Failed to get first active tool - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  aTool = this->GetToolIteratorBegin()->second; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::GetTool(const char* aToolName, vtkTrackerTool* &aTool)
{
  if ( aToolName == NULL )
  {
    LOG_ERROR("Failed to get tool, tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  ToolIteratorType tool = this->ToolContainer.find(aToolName); 
  if ( tool == this->GetToolIteratorEnd() )
  {
    std::ostringstream availableTools; 
    for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
    {
      availableTools << it->first <<";"; 
    }
    LOG_ERROR("Unable to find tool '"<< aToolName <<"' in the list, please check the configuration file first (available tools: " << availableTools.str() << ")." ); 
    return PLUS_FAIL; 
  }

  aTool = tool->second; 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// this thread is run whenever the tracker is tracking
static void *vtkTrackerThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkTracker *self = (vtkTracker *)(data->UserData);

  double currtime[10];

  // loop until cancelled
  for (int i = 0;; i++)
  {
    if (!self->IsTracking())
    {
      LOG_DEBUG("Stopped tracking");
      return NULL;
    }

    double newtime = vtkAccurateTimer::GetSystemTime(); 

    // get current tracking rate over last 10 updates
    double difftime = newtime - currtime[i%10];
    currtime[i%10] = newtime;
    if (i > 10 && difftime != 0)
    {
      self->InternalUpdateRate = (10.0/difftime);
    }    

    self->UpdateMutex->Lock();
    if (self->IsTracking())
    {
      self->InternalUpdate();
      self->UpdateTime.Modified();
    }
    self->UpdateMutex->Unlock();

    if (!self->IsTracking())
    {
      LOG_DEBUG("Stopped tracking");
      return NULL;
    }

    // check to see if main thread wants to lock the UpdateMutex
    self->RequestUpdateMutex->Lock();
    self->RequestUpdateMutex->Unlock();

    // check to see if we are being told to quit 
    data->ActiveFlagLock->Lock();
    int activeFlag = *(data->ActiveFlag);
    data->ActiveFlagLock->Unlock();

    if (!activeFlag)
    {
      LOG_DEBUG("Stopped tracking");
      return NULL;
    }

    double delay = ( newtime + 1.0 / self->GetFrequency() - vtkAccurateTimer::GetSystemTime() );
    if ( delay > 0 )
    {
      vtkAccurateTimer::Delay(delay); 
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::Probe()
{
  PlusLockGuard<vtkCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  // Client

  if (this->InternalStartTracking() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->InternalStopTracking() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::StartTracking()
{
  // start the tracking thread
  if ( this->Tracking )
  {
    LOG_ERROR("Cannot start the tracking thread - tracking still running!");
    return PLUS_FAIL;
  }

  if ( this->InternalStartTracking() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start tracking!"); 
    return PLUS_FAIL; 
  } 

  // this will block the tracking thread until we're ready
  this->UpdateMutex->Lock();

  this->Tracking = 1;

  // start the tracking thread
  this->ThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)\
    &vtkTrackerThread,this);
  this->LastUpdateTime = this->UpdateTime.GetMTime();

  // allow the tracking thread to proceed
  this->UpdateMutex->Unlock();

  // wait until the first update has occurred before returning
  int timechanged = 0;

  while (!timechanged)
  {
    this->RequestUpdateMutex->Lock();
    this->UpdateMutex->Lock();
    this->RequestUpdateMutex->Unlock();
    timechanged = (this->LastUpdateTime != this->UpdateTime.GetMTime());
    this->UpdateMutex->Unlock();
    vtkAccurateTimer::Delay(0.1); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::StopTracking()
{

  if (!this->Tracking)
  {
    // tracking already stopped, nothing to do
    return PLUS_SUCCESS;
  }

  this->UpdateMutex->Lock();
  // after lock we can be sure that InternalUpdate is not running, so we can safely stop the thread
  this->ThreadId = -1;
  this->Tracking = 0;
  this->UpdateMutex->Unlock();

  // Let's give a chance to the thread to stop before we kill the tracker connection
  // TODO: we should wait until the thread is actually stopped, not by a fixed amount
  vtkAccurateTimer::Delay(0.5);

  if ( this->InternalStopTracking() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to stop tracking thread!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::ToolTimeStampedUpdate(const char* aToolName, vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, 
                                             double unfilteredtimestamp) 
{
  if ( aToolName == NULL )
  {
    LOG_ERROR("Failed to update tool - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool* tool = NULL; 
  if ( this->GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tool - unable to find tool!" << aToolName ); 
    return PLUS_FAIL; 
  }

  vtkTrackerBuffer *buffer = tool->GetBuffer();
  PlusStatus bufferStatus = buffer->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp);
  tool->SetFrameNumber(frameNumber); 

  return bufferStatus; 
}

//----------------------------------------------------------------------------
void vtkTracker::Beep(int n)
{
  this->RequestUpdateMutex->Lock();
  this->UpdateMutex->Lock();
  this->RequestUpdateMutex->Unlock();

  this->InternalBeep(n);

  this->UpdateMutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkTracker::SetToolLED(const char* portName, int led, int state)
{
  this->RequestUpdateMutex->Lock();
  this->UpdateMutex->Lock();
  this->RequestUpdateMutex->Unlock();

  this->InternalSetToolLED(portName, led, state);

  this->UpdateMutex->Unlock();
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::Connect()
{
  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::Disconnect()
{
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkTracker::DeepCopy(vtkTracker *tracker)
{
  LOG_TRACE("vtkTracker::DeepCopy"); 
  for ( ToolIteratorType it = tracker->ToolContainer.begin(); it != tracker->ToolContainer.end(); ++it )
  {
    LOG_DEBUG("Copy the buffer of tracker tool: " << it->first ); 
    if ( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Copy of tool '" << it->first << "' failed - unabale to add tool to the container!"); 
      continue; 
    }
    
    vtkTrackerTool* tool = NULL; 
    if ( this->GetTool(it->first.c_str(), tool ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Copy of tool '" << it->first << "' failed - unabale to get tool from container!"); 
      continue;   
    }

    tool->DeepCopy( it->second ); 
  }

  this->InternalUpdateRate = tracker->GetInternalUpdateRate();
  this->SetFrequency(tracker->GetFrequency()); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::WriteConfiguration(vtkXMLDataElement* config)
{
  if ( config == NULL )
  {
    LOG_ERROR("Unable to write configuration: xml data element is NULL!"); 
    return PLUS_FAIL;
  }

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  if ( trackerConfig == NULL )
  {
    LOG_ERROR("Unable to find Tracker xml data element!"); 
    return PLUS_FAIL; 
  }

  if ( !this->ToolContainer.empty() )  
  {
    vtkTrackerTool* tool = this->GetToolIteratorBegin()->second; 

    config->SetIntAttribute("BufferSize", tool->GetBuffer()->GetBufferSize()); 

    config->SetDoubleAttribute("LocalTimeOffset", tool->GetBuffer()->GetLocalTimeOffset() ); 
  }

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
void vtkTracker::SetToolsBufferSize( int aBufferSize )
{
  LOG_TRACE("vtkTracker::SetToolsBufferSize to " << aBufferSize ); 
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetBufferSize( aBufferSize ); 
  }
}

//-----------------------------------------------------------------------------
void vtkTracker::SetToolsLocalTimeOffset( double aLocalTimeOffset )
{
  LOG_INFO("Tools local time offset: " << 1000*aLocalTimeOffset << "ms" ); 
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetLocalTimeOffset(aLocalTimeOffset); 
  }
}


//-----------------------------------------------------------------------------
PlusStatus vtkTracker::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkTracker::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure tracker! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  // Read tool configurations 
  for ( int tool = 0; tool < trackerConfig->GetNumberOfNestedElements(); tool++ )
  {
    vtkXMLDataElement* toolDataElement = trackerConfig->GetNestedElement(tool); 
    if ( STRCASECMP(toolDataElement->GetName(), "Tool") != 0 )
    {
      // if this is not a Tool element, skip it
      continue; 
    }

    vtkSmartPointer<vtkTrackerTool> trackerTool = vtkSmartPointer<vtkTrackerTool>::New(); 
    if ( trackerTool->ReadConfiguration(toolDataElement) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tool to tracker - failed to read tool configuration"); 
      continue; 
    }

    if ( this->AddTool(trackerTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tool '" << trackerTool->GetToolName() << "' to tracker on port " << trackerTool->GetPortName() );
      continue; 
    }
  }

  int bufferSize = 0; 
  if ( trackerConfig->GetScalarAttribute("BufferSize", bufferSize) ) 
  {
    this->SetToolsBufferSize(bufferSize); 
  }

  double frequency = 0; 
  if ( trackerConfig->GetScalarAttribute("Frequency", frequency) ) 
  {
    this->SetFrequency(frequency);  
  }

  int averagedItemsForFiltering = 0; 
  if ( trackerConfig->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {
      it->second->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering); 
    }
  }
  else
  {
    LOG_WARNING("Unable to find Tracker AveragedItemsForFiltering attribute in configuration file!"); 
  }

  double localTimeOffset = 0; 
  if ( trackerConfig->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
  {
    this->SetToolsLocalTimeOffset(localTimeOffset); 
  }
  
  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::GetToolByPortName( const char* portName, vtkTrackerTool* &aTool)
{
  if ( portName == NULL )
  {
    LOG_ERROR("Failed to get tool - port name is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if ( STRCASECMP( portName, it->second->GetPortName() ) == 0 )
    {
      aTool = it->second; 
      return PLUS_SUCCESS; 
    }
  }

  return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
void vtkTracker::SetStartTime( double startTime)
{
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetStartTime(startTime); 
  }
}

//----------------------------------------------------------------------------
double vtkTracker::GetStartTime()
{
  double sumStartTime = 0.0;
  double numberOfTools(0); 
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetBuffer()->GetStartTime(); 
    numberOfTools++; 
  }

  return sumStartTime / numberOfTools;
}

//----------------------------------------------------------------------------
std::string vtkTracker::ConvertTrackerStatusToString(TrackerStatus status)
{
  std::ostringstream flagFieldValue; 
  if ( status == TR_OK )
  {
    flagFieldValue << "OK "; 
  }
  else if ( status == TR_MISSING != 0 )
  {
    flagFieldValue << "TR_MISSING "; 
  }
  else if ( status == TR_OUT_OF_VIEW != 0 )
  {
    flagFieldValue << "TR_OUT_OF_VIEW "; 
  }
  else if ( status == TR_OUT_OF_VOLUME != 0 )
  {
    flagFieldValue << "TR_OUT_OF_VOLUME "; 
  }
  else if ( status == TR_REQ_TIMEOUT != 0 )
  {
    flagFieldValue << "TR_REQ_TIMEOUT "; 
  }
  else
  { 
    LOG_WARNING("Unknown tracker status received - set TR_MISSING by default!"); 
    flagFieldValue << "TR_MISSING "; 
  }

  return flagFieldValue.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::GetAllTransforms(double timestamp, TrackedFrame* aTrackedFrame )
{
  int numberOfErrors(0); 

  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    PlusTransformName toolTransformName(it->second->GetToolName(), this->ToolReferenceFrameName ); 
    if ( ! toolTransformName.IsValid() )
    {
      LOG_ERROR("Tool transform name is invalid!"); 
      numberOfErrors++; 
      continue; 
    }
    
    TrackerBufferItem bufferItem; 
    if ( it->second->GetBuffer()->GetTrackerBufferItemFromTime(timestamp, &bufferItem, vtkTrackerBuffer::INTERPOLATED ) != ITEM_OK )
    {
      double latestTimestamp(0); 
      if ( it->second->GetBuffer()->GetLatestTimeStamp(latestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get latest timestamp!"); 
      }

      double oldestTimestamp(0); 
      if ( it->second->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get oldest timestamp!"); 
      }

      LOG_ERROR("Failed to get tracker item from buffer by time: " << std::fixed << timestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ")."); 
      numberOfErrors++; 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> dMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(dMatrix)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get matrix from buffer item for tool " << it->second->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame->SetCustomFrameTransform(toolTransformName, dMatrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform for tool " << it->second->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame->SetCustomFrameTransformStatus(toolTransformName, vtkTracker::ConvertTrackerStatusToString(bufferItem.GetStatus()) ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform status for tool " << it->second->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::GenerateTrackingDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  if ( this->ToolContainer.empty() )
  {
    LOG_ERROR("Failed to generate tracking data acqusition report - no tools available!"); 
    return PLUS_FAIL;
  }

  // Use the first tool in the container to generate the report
  vtkTrackerTool* tool = this->GetToolIteratorBegin()->second;  

  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New(); 
 
  if ( tool->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to get timestamp report table from tool '"<< tool->GetToolName() << "' buffer!"); 
    return PLUS_FAIL; 
  }

  std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/")
    + std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) 
    + std::string(".TrackerBufferTimestamps.txt"); 

  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( timestampReportTable, reportFile.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write table to file in gnuplot format!"); 
    return PLUS_FAIL; 
  } 

  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find tracking data acquisition report file at: " << reportFile); 
    return PLUS_FAIL; 
  }

  const char* scriptsFolder = vtkPlusConfig::GetInstance()->GetScriptsDirectory();
  std::string plotBufferTimestampScript = scriptsFolder + std::string("/gnuplot/PlotBufferTimestamp.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotBufferTimestampScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotBufferTimestampScript); 
    return PLUS_FAIL; 
  }

  htmlReport->AddText("Tracking Data Acquisition Analysis", vtkHTMLGenerator::H1); 
  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream trackerBufferAnalysis; 
  trackerBufferAnalysis << "f='" << reportFile << "'; o='TrackerBufferTimestamps';" << std::ends; 
  plotter->AddArgument(trackerBufferAnalysis.str().c_str()); 
  plotter->AddArgument(plotBufferTimestampScript.c_str());  
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!"); 
    return PLUS_FAIL; 
  } 
  htmlReport->AddImage("TrackerBufferTimestamps.jpg", "Tracking Data Acquisition Analysis"); 

  htmlReport->AddHorizontalLine(); 

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
void vtkTracker::ClearAllBuffers()
{
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->Clear(); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::CopyBuffer( vtkTrackerBuffer* aTrackerBuffer, const char* aToolName )
{
  LOG_TRACE("vtkTracker::CopyBuffer"); 

  if ( aTrackerBuffer == NULL )
  {
    LOG_ERROR("Unable to copy tracker buffer to a NULL buffer!"); 
    return PLUS_FAIL; 
  }

  if ( aToolName == NULL )
  {
    LOG_ERROR("Unable to copy tracker buffer - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool * tool = NULL; 
  if ( GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool with name: " << aToolName ); 
    return PLUS_FAIL; 
  }

  aTrackerBuffer->DeepCopy(tool->GetBuffer()); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTracker::WriteToMetafile( const char* outputFolder, const char* metaFileName, bool useCompression /*= false*/ )
{
  LOG_TRACE("vtkTracker::WriteToMetafile: " << outputFolder << "/" << metaFileName); 

  if ( this->GetNumberOfTools() == 0 )
  {
    LOG_ERROR("Failed to write tracker to metafile - there are no active tools!"); 
    return PLUS_FAIL; 
  }

  // Get the number of items from buffers and use the lowest
  int numberOfItems(-1); 
  for ( ToolIteratorType it = this->ToolContainer.begin(); it != this->ToolContainer.end(); ++it)
  {
    if ( numberOfItems < 0 || numberOfItems > it->second->GetBuffer()->GetNumberOfItems() )
    {
      numberOfItems = it->second->GetBuffer()->GetNumberOfItems(); 
    }
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  PlusStatus status=PLUS_SUCCESS;

  // Get the first tool
  vtkTrackerTool* firstActiveTool = this->ToolContainer.begin()->second; 

  // Set default transform name
  PlusTransformName defaultFrameTransformName(firstActiveTool->GetToolName(), this->ToolReferenceFrameName ); 
  trackedFrameList->SetDefaultFrameTransformName(defaultFrameTransformName); 
  for ( int i = 0 ; i < numberOfItems; i++ ) 
  {
    //Create fake image 
    typedef itk::Image<unsigned char, 2> ImageType;
    ImageType::Pointer frame = ImageType::New(); 
    ImageType::SizeType size = {1, 1};
    ImageType::IndexType start = {0,0};
    ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    frame->SetRegions(region);

    try
    {
      frame->Allocate();
    }
    catch (itk::ExceptionObject & err) 
    {		
      LOG_ERROR("Unable to allocate memory for image: " << err.GetDescription() );
      status=PLUS_FAIL;
      continue; 
    }	

    TrackedFrame trackedFrame; 
    trackedFrame.GetImageData()->SetITKImageBase(frame);

    TrackerBufferItem bufferItem; 
    BufferItemUidType uid = firstActiveTool->GetBuffer()->GetOldestItemUidInBuffer() + i; 

    if ( firstActiveTool->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << uid ); 
      continue; 
    }

    const double frameTimestamp = bufferItem.GetFilteredTimestamp(firstActiveTool->GetBuffer()->GetLocalTimeOffset()); 

    // Add main tool timestamp
    std::ostringstream timestampFieldValue; 
    timestampFieldValue << std::fixed << frameTimestamp; 
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

    // Add main tool unfiltered timestamp
    std::ostringstream unfilteredtimestampFieldValue; 
    unfilteredtimestampFieldValue << std::fixed << bufferItem.GetUnfilteredTimestamp(firstActiveTool->GetBuffer()->GetLocalTimeOffset()); 
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

    // Add main tool frameNumber
    std::ostringstream frameNumberFieldValue; 
    frameNumberFieldValue << std::fixed << bufferItem.GetIndex(); 
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 


    // Add transforms
    for ( ToolIteratorType it = this->ToolContainer.begin(); it != this->ToolContainer.end(); ++it)
    {
      TrackerBufferItem toolBufferItem; 
      if ( it->second->GetBuffer()->GetTrackerBufferItemFromTime( frameTimestamp, &toolBufferItem, vtkTrackerBuffer::EXACT_TIME ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item from time: " << std::fixed << frameTimestamp ); 
        continue; 
      }

      vtkSmartPointer<vtkMatrix4x4> toolMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
      if (toolBufferItem.GetMatrix(toolMatrix)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get toolMatrix"); 
        return PLUS_FAIL; 
      }
  
      PlusTransformName toolToTrackerTransform(it->second->GetToolName(), this->ToolReferenceFrameName ); 
      trackedFrame.SetCustomFrameTransform(toolToTrackerTransform, toolMatrix ); 

      // Add tool status
      trackedFrame.SetCustomFrameTransformStatus(toolToTrackerTransform, vtkTracker::ConvertTrackerStatusToString(toolBufferItem.GetStatus()) ); 
    }

    // Add tracked frame to the list
    trackedFrameList->AddTrackedFrame(&trackedFrame); 
  }

  // Save tracked frames to metafile
  if ( trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    return PLUS_FAIL;
  }

  return status;
}
