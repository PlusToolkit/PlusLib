/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkVirtualStreamDiscCapture.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamDiscCapture, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamDiscCapture);

//----------------------------------------------------------------------------
vtkVirtualStreamDiscCapture::vtkVirtualStreamDiscCapture()
: vtkPlusDevice()
, m_RecordedFrames(vtkTrackedFrameList::New())
, m_LastRecordedFrameTimestamp(0.0)
, m_Filename("")
, m_Writer(vtkMetaImageSequenceIO::New())
, m_EnableFileCompression(false)
, m_TotalFramesRecorded(0)
, m_HeaderPrepared(false)
, EnableCapturing(true)
, WriterAccessMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
{
  m_RecordedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP); 
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  // The data capture thread will be used to regularly read the frames and write to disk
  this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkVirtualStreamDiscCapture::~vtkVirtualStreamDiscCapture()
{
  if (m_RecordedFrames != NULL) {
    m_RecordedFrames->Delete();
    m_RecordedFrames = NULL;
  }

  if( m_Writer != NULL )
  {
    m_Writer->Delete();
    m_Writer = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkVirtualStreamDiscCapture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamDiscCapture::ReadConfiguration( vtkXMLDataElement* rootConfig)
{
  if( Superclass::ReadConfiguration(rootConfig) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceElement = this->FindThisDeviceElement(rootConfig);
  if (deviceElement == NULL) 
  {
    LOG_ERROR("Cannot find 3dConnexion element in XML tree!");
    return PLUS_FAIL;
  }

  const char * filename = deviceElement->GetAttribute("Filename");
  if( filename != NULL )
  {
    m_Filename = std::string(filename);
  }

  const char* comp = deviceElement->GetAttribute("EnableFileCompression");
  if( comp != NULL )
  {
    m_EnableFileCompression = STRCASECMP(comp, "true") == 0 ? true : false;
  }

  const char* enableCapturing = deviceElement->GetAttribute("EnableCapturing");
  if( enableCapturing != NULL )
  {
    this->EnableCapturing = STRCASECMP(enableCapturing, "true") == 0 ? true : false;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamDiscCapture::InternalConnect()
{
  bool lowestRateKnown=false;
  double lowestRate=30; // just a usual value (FPS)
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputStream = (*it);
    if( anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
    {
      lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown=true;
    }
  }
  if (lowestRateKnown)
  {
    this->AcquisitionRate = lowestRate;
  }
  else
  {
    LOG_WARNING("vtkVirtualStreamDiscCapture acquisition rate is not known");
  }

  if (this->EnableCapturing)
  {
    // Capturing enabled, so we have to open the file now
    if (OpenFile()!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamDiscCapture::InternalDisconnect()
{ 
  this->EnableCapturing=false;
  PlusStatus status=CloseFile();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamDiscCapture::OpenFile()
{
  if ( m_Filename.empty() )
  {
    LOG_ERROR("vtkVirtualStreamDiscCapture: Cannot open file, filename is not specified");
    return PLUS_FAIL;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

  // Because this virtual device continually appends data to the file, we cannot do live compression
  m_Writer->SetUseCompression(false);
  m_Writer->SetTrackedFrameList(m_RecordedFrames);

  // Save config file next to the tracked frame list
  std::string path = vtksys::SystemTools::GetFilenamePath(m_Filename); 
  if (!path.empty())
  {
    path += "/";
  }
  else
  {
    path = std::string(vtkPlusConfig::GetInstance()->GetOutputDirectory()) + "/";
    std::stringstream ss;
    ss << path << m_Filename;
    m_Filename = ss.str();
  }

  m_Writer->SetFileName(m_Filename.c_str());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamDiscCapture::CloseFile()
{
  // Fix the header to write the correct number of frames
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

  if (!m_HeaderPrepared)
  {
    // nothing has been prepared, so nothing to finalize
    return PLUS_SUCCESS;
  }

  std::ostringstream dimSizeStr; 
  int dimensions[3]={0};
  dimensions[0] = m_Writer->GetDimensions()[0];
  dimensions[1] = m_Writer->GetDimensions()[1];
  dimensions[2] = m_TotalFramesRecorded;
  dimSizeStr << dimensions[0] << " " << dimensions[1] << " " << dimensions[2];
  m_Writer->GetTrackedFrameList()->SetCustomString("DimSize", dimSizeStr.str().c_str());
  m_Writer->UpdateFieldInImageHeader("DimSize");

  m_Writer->FinalizeHeader();

  m_Writer->Close();

  m_Writer->SetTrackedFrameList(NULL);

  std::string path = vtksys::SystemTools::GetFilenamePath(m_Filename); 
  std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(m_Filename); 
  std::string configFileName = path + "/" + filename + "_config.xml";
  PlusCommon::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  if( m_EnableFileCompression )
  {
    if( this->CompressFile() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to compress file.");
      return PLUS_FAIL;
    }
  }

  m_HeaderPrepared = false;
  m_RecordedFrames->Clear();
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkVirtualStreamDiscCapture::InternalUpdate()
{
  if (!this->EnableCapturing)
  {
    // Capturing is disabled
    return PLUS_SUCCESS;
  }

  // If frames exist that haven't been recorded, record them (compare with last recorded timestamp)
  // Allocates frames
  if( this->BuildNewTrackedFrameList() != PLUS_SUCCESS )
  {
    if (!this->EnableCapturing)
    {
      // While this thread was working on getting the frames, capturing was disabled, so cancel the update now
      return PLUS_SUCCESS;
    }
    LOG_WARNING("Unable to build new tracked frame list. Continuing. Device: " << this->GetDeviceId());
    return PLUS_FAIL;
  }

  if( m_RecordedFrames->GetNumberOfTrackedFrames() == 0 )
  {
    return PLUS_SUCCESS;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);
  if (!this->EnableCapturing)
  {
    // While this thread was waiting for the unlock, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  if( !m_HeaderPrepared )
  {
    if( m_Writer->PrepareHeader() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to prepare header.");
      this->Disconnect();
      return PLUS_FAIL;
    }
    m_HeaderPrepared = true;
  }
  
  if( m_Writer->AppendImagesToHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to append image data to header.");
    this->Disconnect();
    return PLUS_FAIL;
  }
  if( m_Writer->AppendImages() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to append images. Stopping recording at timestamp: " << m_LastRecordedFrameTimestamp );
    this->Disconnect();
    return PLUS_FAIL;
  }

  m_TotalFramesRecorded += m_RecordedFrames->GetNumberOfTrackedFrames();
  m_LastRecordedFrameTimestamp = m_RecordedFrames->GetMostRecentTimestamp();
  this->ClearRecordedFrames();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkVirtualStreamDiscCapture::BuildNewTrackedFrameList()
{
  //LOG_TRACE("vtkVirtualStreamDiscCapture::AppendNewFrames");

  // Record
  if ( this->OutputChannels[0]->GetTrackedFrameList(m_LastRecordedFrameTimestamp, m_RecordedFrames, 100) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp << ". Device ID: " << this->GetDeviceId() ); 
    // an error occurred while trying to get the frames
    // to increase the chance of recovery forget about all the previous frames and just try to collect data from now on
    m_LastRecordedFrameTimestamp=vtkAccurateTimer::GetSystemTime();
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVirtualStreamDiscCapture::CompressFile()
{
  vtkSmartPointer<vtkMetaImageSequenceIO> reader = vtkSmartPointer<vtkMetaImageSequenceIO>::New();
  reader->SetFileName(m_Filename.c_str());

  LOG_DEBUG("Read input sequence metafile: " << m_Filename ); 

  if (reader->Read() != PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't read sequence metafile: " <<  m_Filename ); 
    return PLUS_FAIL;
  }	

  // Now write to disc using compression
  reader->SetUseCompression(true);
  reader->SetFileName(m_Filename.c_str());

  if (reader->Write() != PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't write sequence metafile: " <<  reader->GetFileName() ); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVirtualStreamDiscCapture::NotifyConfigured()
{
  if( this->OutputChannels.size() > 0 )
  {
    LOG_WARNING("vtkVirtualStreamDiscCapture is expecting no output channel(s) and there are " << this->OutputChannels.size() << " channels. Output channel information will be dropped.");
    this->OutputChannels.clear();
  }

  if( this->InputChannels.size() == 0 )
  {
    LOG_ERROR("No input channel sent to vtkVirtualStreamDiscCapture. Unable to save anything.");
    return PLUS_FAIL;
  }

  // GetTrackedFrame reads from the OutputChannels
  // For now, place the input stream as an output stream so its data is read
  this->OutputChannels.push_back(this->InputChannels[0]);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkVirtualStreamDiscCapture::SetFilename( const char* filename )
{
  m_Filename = std::string(filename);
  this->m_Writer->SetFileName(m_Filename.c_str());
}

//-----------------------------------------------------------------------------
bool vtkVirtualStreamDiscCapture::HasUnsavedData() const
{
  return m_HeaderPrepared;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamDiscCapture::ClearRecordedFrames()
{
  m_RecordedFrames->Clear();

  return PLUS_SUCCESS;
}

