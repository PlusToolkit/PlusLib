/*=========================================================================

Program:   AtamaiTracking for VTK
Module:    $RCSfile: vtkTracker.cxx,v $
Creator:   David Gobbi <dgobbi@atamai.com>
Language:  C++
Author:    $Author: pdas $
Date:      $Date: 2007/05/30 18:36:48 $
Version:   $Revision: 1.17 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
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


//----------------------------------------------------------------------------
vtkTracker* vtkTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTracker");
  if(ret)
  {
    return (vtkTracker*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkTracker;
}

//----------------------------------------------------------------------------
vtkTracker::vtkTracker()
{
  this->Tracking = 0;
  this->WorldCalibrationMatrix = vtkMatrix4x4::New();
  this->NumberOfTools = 0;
  this->UpdateTimeStamp = 0;
  this->Tools = 0;
  this->LastUpdateTime = 0;
  this->InternalUpdateRate = 0;
  this->Frequency = 50; 
  this->TrackerCalibratedOff(); 

  // for threaded capture of transformations
  this->Threader = vtkMultiThreader::New();
  this->ThreadId = -1;
  this->UpdateMutex = vtkCriticalSection::New();
  this->RequestUpdateMutex = vtkCriticalSection::New();
}

//----------------------------------------------------------------------------
vtkTracker::~vtkTracker()
{
  this->StopTracking();
  this->Disconnect();

  for (int i = 0; i < this->NumberOfTools; i++)
  { 
    this->Tools[i]->SetTracker(NULL);
    this->Tools[i]->Delete();
  }
  if (this->Tools)
  {
    delete [] this->Tools;
  }

  this->WorldCalibrationMatrix->Delete();

  this->Threader->Delete();
  this->UpdateMutex->Delete();
  this->RequestUpdateMutex->Delete();
}

//----------------------------------------------------------------------------
void vtkTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  os << indent << "WorldCalibrationMatrix: " << this->WorldCalibrationMatrix << "\n";
  this->WorldCalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Tracking: " << this->Tracking << "\n";
  os << indent << "NumberOfTools: " << this->NumberOfTools << "\n";
}

//----------------------------------------------------------------------------
// allocates a vtkTrackerTool object for each of the tools.
void vtkTracker::SetNumberOfTools(int numtools)
{
  int i;

  if (this->NumberOfTools > 0) 
  {
    LOG_ERROR("SetNumberOfTools() can only be called once");
  }
  this->NumberOfTools = numtools;

  this->Tools = new vtkTrackerTool *[numtools];

  for (i = 0; i < numtools; i++) 
  {
    this->Tools[i] = vtkTrackerTool::New();
    this->Tools[i]->SetTracker(this);
    this->Tools[i]->SetToolPort(i);
    // Set default tool names
    std::ostringstream toolname; 
    toolname << "Tool" << i; 
    this->SetToolName(i, toolname.str().c_str()); 

    std::ostringstream toolcalibmatrixname; 
    toolcalibmatrixname << "Tool" << i << "CalibMatrix"; 
    this->Tools[i]->SetCalibrationMatrixName(toolcalibmatrixname.str().c_str()); 
  }
}  

//----------------------------------------------------------------------------
void vtkTracker::SetToolName(int tool, const char* name)
{
  this->Tools[tool]->SetToolName(name); 	
}

//----------------------------------------------------------------------------
void vtkTracker::SetToolEnabled(int tool, bool enabled )
{
  this->Tools[tool]->SetEnabled(enabled); 	
}

//----------------------------------------------------------------------------
vtkTrackerTool *vtkTracker::GetTool(int tool)
{
  if (tool < 0 || tool > this->NumberOfTools) 
  {
    LOG_ERROR("GetTool(" << tool << "): only " << this->NumberOfTools << " are available");
    return NULL;
  }
  return this->Tools[tool];
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
PlusStatus vtkTracker::ToolTimeStampedUpdate(int tool, vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, 
                                             double unfilteredtimestamp) 
{
  vtkTrackerBuffer *buffer = this->Tools[tool]->GetBuffer();
  PlusStatus bufferStatus = buffer->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp);
  this->GetTool(tool)->SetFrameNumber(frameNumber); 

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
void vtkTracker::SetToolLED(int tool, int led, int state)
{
  this->RequestUpdateMutex->Lock();
  this->UpdateMutex->Lock();
  this->RequestUpdateMutex->Unlock();

  this->InternalSetToolLED(tool, led, state);

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
  this->SetNumberOfTools( tracker->GetNumberOfTools() ); 
  this->SetTrackerCalibrated( tracker->GetTrackerCalibrated() ); 

  for ( int i = 0; i < this->NumberOfTools; i++ )
  {
    LOG_DEBUG("Copy the buffer of tracker tool: " << i ); 
    this->Tools[i]->DeepCopy( tracker->GetTool(i) );
  }

  this->WorldCalibrationMatrix->DeepCopy( tracker->GetWorldCalibrationMatrix() ); 
  this->InternalUpdateRate = tracker->GetInternalUpdateRate();
  this->SetFrequency(tracker->GetFrequency()); 
  this->SetTrackerCalibrated(tracker->GetTrackerCalibrated()); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::WriteConfiguration(vtkXMLDataElement* config)
{
  if ( config == NULL )
  {
    LOG_ERROR("Unable to write calibration result: xml data element is NULL!"); 
    return PLUS_FAIL;
  }

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
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

  int firstActiveToolNumber(-1); 
  if ( this->GetFirstActiveTool(firstActiveToolNumber) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get first active tool from tracker!"); 
    return PLUS_FAIL; 
  }
  
  vtkTrackerTool* tool = this->GetTool(firstActiveToolNumber); 

  if ( tool == NULL )
  {
    LOG_ERROR("Unable to get first active tool by tool number - tool is NULL!"); 
    return PLUS_FAIL; 
  }

  config->SetIntAttribute("BufferSize", tool->GetBuffer()->GetBufferSize()); 

  config->SetDoubleAttribute("LocalTimeOffset", tool->GetBuffer()->GetLocalTimeOffset() ); 

  return PLUS_SUCCESS; 
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

  vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  int bufferSize = 0; 
  if ( trackerConfig->GetScalarAttribute("BufferSize", bufferSize) ) 
  {
    for ( int i = 0; i < this->GetNumberOfTools(); i++)
    {
      this->GetTool(i)->GetBuffer()->SetBufferSize( bufferSize ); 
    }
  }

  double frequency = 0; 
  if ( trackerConfig->GetScalarAttribute("Frequency", frequency) ) 
  {
    this->SetFrequency(frequency);  
  }

  int averagedItemsForFiltering = 0; 
  if ( trackerConfig->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    for ( int i = 0; i < this->GetNumberOfTools(); i++)
    {
      this->GetTool(i)->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
    }
  }
  else
  {
    LOG_WARNING("Unable to find Tracker AveragedItemsForFiltering attribute in configuration file!"); 
  }

  double localTimeOffset = 0; 
  if ( trackerConfig->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
  {
    LOG_INFO("Tracker local time offset: " << 1000*localTimeOffset << "ms" ); 
    for ( int i = 0; i < this->GetNumberOfTools(); i++)
    {
      this->GetTool(i)->GetBuffer()->SetLocalTimeOffset(localTimeOffset);
    }
  }

  // Read tool configurations 
  for ( int tool = 0; tool < trackerConfig->GetNumberOfNestedElements(); tool++ )
  {
    vtkSmartPointer<vtkXMLDataElement> toolDataElement = trackerConfig->GetNestedElement(tool); 
    if ( STRCASECMP(toolDataElement->GetName(), "Tool") != 0 )
    {
      // if this is not a Tool element, skip it
      continue; 
    }

    int portNumber(-1); 
    if ( toolDataElement->GetScalarAttribute("PortNumber", portNumber) )
    {
      if ( portNumber < 0 )
      {
        LOG_WARNING("Port number should be larger than 0! Current: " << portNumber); 
      }

      if (portNumber < this->GetNumberOfTools() )
      {
        this->GetTool(portNumber)->ReadConfiguration(toolDataElement); 
      }
      else
      {
        LOG_WARNING("Unable to add tool data element configuration for port: " << portNumber << " - number of tools are: " << this->GetNumberOfTools() ); 
      }
    }
    else
    {
      LOG_ERROR("Unable to find tool port number! PortNumber attribute is mandatory in tool definition."); 
      return PLUS_FAIL; 
    }
  }

  // Set reference tool 

  return PLUS_SUCCESS; 
}


//-----------------------------------------------------------------------------
int vtkTracker::GetToolPortByName( const char* toolName)
{
  if ( toolName != NULL )
  {
    for ( int tool = 0; tool < this->GetNumberOfTools(); tool++ )
    {
      if ( STRCASECMP( toolName, this->GetTool(tool)->GetToolName() ) == 0 )
      {
        return tool;
      }
    }
  }

  return -1; 
}

//----------------------------------------------------------------------------
void vtkTracker::SetStartTime( double startTime)
{
  for ( int i = 0; i < this->GetNumberOfTools(); ++i )
  {
    this->GetTool(i)->GetBuffer()->SetStartTime(startTime); 
  }
}

//----------------------------------------------------------------------------
double vtkTracker::GetStartTime()
{
  double sumStartTime = 0.0;
  for ( int i = 0; i < this->GetNumberOfTools(); ++i )
  {
    sumStartTime += this->GetTool(i)->GetBuffer()->GetStartTime(); 
  }

  return sumStartTime / (double)this->GetNumberOfTools();
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
PlusStatus vtkTracker::GetTrackerToolBufferStringList(double timestamp,
                                                      std::map<std::string, std::string> &toolsBufferMatrices, 
                                                      std::map<std::string, std::string> &toolsCalibrationMatrices, 
                                                      std::map<std::string, std::string> &toolsStatuses,
                                                      bool calibratedTransform /*= false*/)
{
  toolsBufferMatrices.clear();  
  toolsCalibrationMatrices.clear();  
  toolsStatuses.clear(); 

  for ( int tool = 0; tool < this->GetNumberOfTools(); tool++ )
  {
    if ( this->GetTool(tool)->GetEnabled() )
    {
      vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
      TrackerStatus trackerStatus = TR_OK; 

      TrackerBufferItem bufferItem; 
      if ( this->GetTool(tool)->GetBuffer()->GetTrackerBufferItemFromTime(timestamp, &bufferItem, vtkTrackerBuffer::INTERPOLATED, calibratedTransform ) != ITEM_OK )
      {
        double latestTimestamp(0); 
        if ( this->GetTool(tool)->GetBuffer()->GetLatestTimeStamp(latestTimestamp) != ITEM_OK )
        {
          LOG_ERROR("Failed to get latest timestamp!"); 
        }

        double oldestTimestamp(0); 
        if ( this->GetTool(tool)->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
        {
          LOG_ERROR("Failed to get oldest timestamp!"); 
        }

        LOG_ERROR("Failed to get tracker item from buffer by time: " << std::fixed << timestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ")."); 
        return PLUS_FAIL; 
      }

      vtkSmartPointer<vtkMatrix4x4> dMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
      if (bufferItem.GetMatrix(dMatrix)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get dMatrix"); 
        return PLUS_FAIL;
      }

      std::ostringstream strToolTransform; 
      for ( int r = 0; r < 4; ++r )
      {
        for ( int c = 0; c < 4; ++c )
        {
          strToolTransform << dMatrix->GetElement(r,c) << " ";
        }
      }


      vtkMatrix4x4* toolCalibrationMatrix = this->GetTool(tool)->GetCalibrationMatrix(); 
      std::ostringstream strToolCalibMatrix; 
      for ( int r = 0; r < 4; ++r )
      {
        for ( int c = 0; c < 4; ++c )
        {
          strToolCalibMatrix << toolCalibrationMatrix->GetElement(r,c)  << " ";
        }
      }

      toolsBufferMatrices[ this->GetTool(tool)->GetToolName() ] = strToolTransform.str(); 
      toolsCalibrationMatrices[ this->GetTool(tool)->GetCalibrationMatrixName() ] = strToolCalibMatrix.str(); 
      toolsStatuses[ this->GetTool(tool)->GetToolName() ] = vtkTracker::ConvertTrackerStatusToString( bufferItem.GetStatus() ); 
    }
  }

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::GetFirstActiveTool(int &tool)
{
  tool = -1;

  for ( int i = 0; i < this->GetNumberOfTools(); ++i )
  {
    if( this->GetTool(i)->GetEnabled() )
	{
		tool = i; 
		return PLUS_SUCCESS; 
	}
  }

  if ( tool < 0 ) 
  {
	LOG_ERROR("There are no active tools!");
	return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::GenerateTrackingDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }


  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New(); 
  int firstActiveTool = -1;
  if ( GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Cannot get first active tool!");
    return PLUS_FAIL;
  }
  if ( this->GetTool(firstActiveTool)->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to get timestamp report table from default tool buffer!"); 
    return PLUS_FAIL; 
  }

  std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/TrackerBufferTimestamps.txt"); 

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

  std::string plotBufferTimestampScript = gnuplotScriptsFolder + std::string("/PlotBufferTimestamp.gnu"); 
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
PlusStatus vtkTracker::ConvertStringToToolType(const char* typeString, TRACKER_TOOL_TYPE &type)
{
	if (STRCASECMP(typeString, "Reference") == 0)
	{
		type = TRACKER_TOOL_REFERENCE;
	}
	else if (STRCASECMP(typeString, "Probe") == 0)
	{
		type = TRACKER_TOOL_PROBE;
	}
	else if (STRCASECMP(typeString, "Stylus") == 0)
	{
		type = TRACKER_TOOL_STYLUS;
	}
	else if (STRCASECMP(typeString, "Needle") == 0)
	{
		type = TRACKER_TOOL_NEEDLE;
	}
	else if (STRCASECMP(typeString, "General") == 0)
	{
		type = TRACKER_TOOL_GENERAL;
	}
	else
	{
		type = TRACKER_TOOL_NONE;
		LOG_ERROR("Invalid tool type: " << typeString);
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::ConvertToolTypeToString(const TRACKER_TOOL_TYPE type, std::string &typeString)
{
	switch (type)
	{
	case TRACKER_TOOL_REFERENCE:
		typeString = "Reference";
		break;
	case TRACKER_TOOL_PROBE:
		typeString = "Probe";
		break;
	case TRACKER_TOOL_STYLUS:
		typeString = "Stylus";
		break;
	case TRACKER_TOOL_NEEDLE:
		typeString = "Needle";
		break;
	case TRACKER_TOOL_GENERAL:
		typeString = "General";
		break;
	default:
		typeString = "Invalid";
		LOG_ERROR("Invalid tool type!");
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTracker::GetToolPortNumbersByType(TRACKER_TOOL_TYPE type, std::vector<int> &toolNumbersVector)
{
	PlusStatus success = PLUS_FAIL;

	toolNumbersVector.clear();

    for ( int tool = 0; tool < this->GetNumberOfTools(); tool++ )
    {
      if ( this->GetTool(tool)->GetToolType() == type )
      {
        toolNumbersVector.push_back(tool);
		success = PLUS_SUCCESS;
      }
    }

	return success;
}

//-----------------------------------------------------------------------------
int vtkTracker::GetFirstPortNumberByType(TRACKER_TOOL_TYPE type)
{
    for ( int tool = 0; tool < this->GetNumberOfTools(); tool++ )
    {
      if ( this->GetTool(tool)->GetToolType() == type )
      {
        return tool;
      }
    }

	return -1;
}

//-----------------------------------------------------------------------------
int vtkTracker::GetReferenceToolNumber()
{
	return this->GetFirstPortNumberByType(TRACKER_TOOL_REFERENCE);
}
