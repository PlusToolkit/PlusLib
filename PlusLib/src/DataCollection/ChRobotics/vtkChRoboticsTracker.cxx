/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "ChrSerialPacket.h"
#include "PlusConfigure.h"
#include "PlusConfigure.h"
#include "SerialLine.h"
#include "vtkChRoboticsTracker.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"
#include <deque>
#include <sstream>

#include <math.h>

#ifdef WIN32
  #include <io.h> // for findnext
#else
  #include <dirent.h>
#endif


vtkStandardNewMacro(vtkChRoboticsTracker);

//-------------------------------------------------------------------------
// Definitions of constant register address (addresses that must remain the same on all firmware versions)
static const unsigned char UM6_GET_FW_VERSION=170;

// Definitions of some error packets - these are also required to remain constant through all firmware revisions
static const unsigned char UM6_BAD_CHECKSUM=253;
static const unsigned char UM6_UNKNOWN_ADDRESS=254;
static const unsigned char UM6_INVALID_BATCH_SIZE=255;

static const unsigned char CONFIG_REGISTER_START_ADDRES=0;
static const unsigned char DATA_REGISTER_START_ADDRESS=85;
static const unsigned char COMMAND_START_ADDRESS=170;

//-------------------------------------------------------------------------

static const int MAX_COMMAND_REPLY_WAIT=3000; // number of maximum replies to wait for a command reply

//-------------------------------------------------------------------------
vtkChRoboticsTracker::vtkChRoboticsTracker() :
ComPort(5),
BaudRate(115200)
{ 
  this->Serial = new SerialLine(); 

  this->OrientationSensorTool = NULL;

  this->FirmwareDefinition=vtkXMLDataElement::New();

  this->RequireDeviceImageOrientationInDeviceSetConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = false;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = true;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
}

//-------------------------------------------------------------------------
vtkChRoboticsTracker::~vtkChRoboticsTracker() 
{
  if ( this->Recording )
  {
    this->StopRecording();
  }

  if (this->Serial->IsHandleAlive())
  {
    this->Serial->Close();
    delete this->Serial; 
    this->Serial = NULL; 
  }

  this->FirmwareDefinition->Delete();
  this->FirmwareDefinition=NULL;
}

//-------------------------------------------------------------------------
void vtkChRoboticsTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalConnect()
{
  LOG_TRACE( "vtkChRoboticsTracker::Connect" ); 

  if (this->Serial->IsHandleAlive())
  {
    LOG_ERROR("Already connected to serial port");
    return PLUS_FAIL;
  }

  std::ostringstream strComPort; 
  strComPort << "COM" << this->ComPort; 
  this->Serial->SetPortName(strComPort.str()); 

  this->Serial->SetSerialPortSpeed(this->BaudRate); 

  this->Serial->SetMaxReplyTime(1000);

  if (!this->Serial->Open())
  {
    LOG_ERROR("Cannot open serial port "<<strComPort);
    return PLUS_FAIL;
  }

  if (!this->Serial->IsHandleAlive())  
  {  
    LOG_ERROR("COM port handle is not alive "<<strComPort);
    return PLUS_FAIL; 
  }

  if (LoadFirmwareDescriptionForConnectedDevice()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to load firmware description for the connected device");
    this->Serial->Close();
    return PLUS_FAIL;
  }

  if (UpdateDataItemDescriptors()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to update data item descriptors from firmware description");
    this->Serial->Close();
    return PLUS_FAIL;
  }

  this->OrientationSensorTool = NULL;
  GetToolByPortName("OrientationSensor", this->OrientationSensorTool);

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkChRoboticsTracker::Disconnect" ); 
  this->StopRecording();

  this->Serial->Close();

  this->OrientationSensorTool = NULL;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::Probe()
{
  LOG_TRACE("vtkChRoboticsTracker::Probe"); 

  LOG_ERROR("vtkChRoboticsTracker::Probe is not implemented");

  return PLUS_SUCCESS; 
} 

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalStartRecording()
{
  LOG_TRACE( "vtkChRoboticsTracker::InternalStartRecording" ); 
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalStopRecording()
{
  LOG_TRACE( "vtkChRoboticsTracker::InternalStopRecording" );
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalUpdate()
{
  LOG_TRACE( "vtkChRoboticsTracker::InternalUpdate" ); 

  ChrSerialPacket packet;

  while (this->Serial->GetNumberOfBytesAvailableForReading()>0)
  {    
    if (ReceivePacket(packet)==PLUS_SUCCESS)
    {
      ProcessPacket(packet);
    }
  }

  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

  if (this->OrientationSensorTool!=NULL)
  {
    vtkSmartPointer<vtkMatrix4x4> orientationSensorToTracker=vtkSmartPointer<vtkMatrix4x4>::New();

    // LOG_TRACE("roll="<<this->EulerRoll.GetValue() <<", pitch="<<this->EulerPitch.GetValue() <<", yaw="<<this->EulerYaw.GetValue());

    vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
    transform->RotateX(this->EulerRoll.GetValue());
    transform->RotateY(this->EulerPitch.GetValue());
    transform->RotateZ(this->EulerYaw.GetValue());
    transform->GetMatrix(orientationSensorToTracker);
    
    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->OrientationSensorTool->GetFrameNumber() + 1 ; 
    ToolTimeStampedUpdate( this->OrientationSensorTool->GetSourceId(), orientationSensorToTracker, TOOL_OK, frameNumber, unfilteredTimestamp); 
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::FindFirmwareDefinition(const std::string& requestedFirmwareId, vtkXMLDataElement* foundDefinition)
{
  std::string firmwareFullPath=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory() + std::string("/") + this->FirmwareDirectory;
  LOG_DEBUG("Loading the firmware files from "<<firmwareFullPath);
  
  std::vector<std::string> firmwareFileList;
  GetFileNamesFromDirectory(firmwareFileList, firmwareFullPath);
  if (firmwareFileList.size()==0)
  {
    LOG_ERROR("Failed to load firmware definitions from "<<firmwareFullPath);
    return PLUS_FAIL;
  }
  
  for (std::vector<std::string>::iterator it=firmwareFileList.begin(); it!=firmwareFileList.end(); ++it)
  {
    LOG_DEBUG("Loading firmware from: "<<(*it));
    vtkSmartPointer<vtkXMLDataElement> firmwareElemRoot = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile((*it).c_str())); 
    if (firmwareElemRoot.GetPointer()==NULL)
    {
      LOG_WARNING("Failed to read firmware definition from "<<(*it)<<". The file is ignored.");
      continue;
    }
    const char* foundFirmwareId=firmwareElemRoot->GetAttribute("id");
    if (foundFirmwareId==NULL)
    {
      LOG_WARNING("Id not found in firmware definition file "<<(*it)<<". The file is ignored.");
      continue;
    }      
          
    if (requestedFirmwareId.compare(foundFirmwareId)==0)
    {
      // found firmware description
      foundDefinition->DeepCopy(firmwareElemRoot);      
      return PLUS_SUCCESS;
    }
  }
  
  foundDefinition->RemoveAllAttributes();
  foundDefinition->RemoveAllNestedElements();
  LOG_ERROR("Could not find ChRobotics firmware definition for attached device ("<<requestedFirmwareId<<")");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkChRoboticsTracker::GetFileNamesFromDirectory(std::vector<std::string> &fileNames, const std::string &dir)
{  
#if(WIN32) // Windows
  _finddata_t file; 
  std::string findPath=dir+"\\*.*";
  long currentPosition = _findfirst(findPath.c_str(), &file); //find the first file in directory
  if (currentPosition == -1L)
  {
    return ; //end the procedure if no file is found
  }
  do
  {
    std::string fileName = file.name;
    //ignore . and ..
    if(strcmp(fileName.c_str() , ".") != 0 && strcmp(fileName.c_str() , "..") !=0 )
    {
      //If not subdirectory
      if (!(file.attrib & _A_SUBDIR))
      {
        fileNames.push_back(std::string(dir) + "/" +  fileName);
      }
    }
  } while(_findnext(currentPosition, &file) == 0);
  _findclose(currentPosition); //close search
#else // Linux
  DIR* d;
  struct dirent *ent;
  if ( (d = opendir(dir.c_str())) != NULL)
  {
    while ((ent = readdir(d)) != NULL)
    {
      std::string entry( ent->d_name );
      if (entry != "." && entry != "..")
      {
        fileNames.push_back(dir + "/" + entry);
      }
    }
  }
  closedir(d);
#endif
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::UpdateDataItemDescriptors()
{
  PlusStatus status=PLUS_SUCCESS;
  
  if (FindDataItemDescriptor("Roll (phi)",this->EulerRoll)!=PLUS_SUCCESS) { status=PLUS_FAIL; }
  if (FindDataItemDescriptor("Pitch (theta)",this->EulerPitch)!=PLUS_SUCCESS) { status=PLUS_FAIL; }
  if (FindDataItemDescriptor("Yaw (psi)",this->EulerYaw)!=PLUS_SUCCESS) { status=PLUS_FAIL; }  
  
  return status;
}

//-------------------------------------------------------------------------
void vtkChRoboticsTracker::UpdateDataItemValues(ChrSerialPacket& packet)
{
  this->EulerRoll.ReadValueFromPacket(packet);
  this->EulerPitch.ReadValueFromPacket(packet);
  this->EulerYaw.ReadValueFromPacket(packet); 
}


//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::FindDataItemDescriptor(const std::string itemName, ChrDataItem &foundItem)
{
  if (this->FirmwareDefinition==NULL)
  {
    LOG_ERROR("Firmware definition is not available");
    return PLUS_FAIL;
  }

  const std::string dataGroupElemName="DataGroup";
  const std::string dataItemElemName="DataItem";

  for ( int dataGroupIndex = 0; dataGroupIndex < this->FirmwareDefinition->GetNumberOfNestedElements(); ++dataGroupIndex )
  {
    vtkXMLDataElement* groupElem = this->FirmwareDefinition->GetNestedElement(dataGroupIndex);
    if ( groupElem == NULL )
    {
      continue; 
    }
    if (dataGroupElemName.compare(groupElem->GetName())!=0)
    {
      // not a data group
      continue;
    }
    for ( int dataItemIndex = 0; dataItemIndex < groupElem->GetNumberOfNestedElements(); ++dataItemIndex )
    {
      vtkXMLDataElement* dataItemElem = groupElem->GetNestedElement(dataItemIndex);

      if (dataItemElemName.compare(dataItemElem->GetName())!=0)
      {
        // not a data item
        continue;
      }

      vtkXMLDataElement* dataItemNameElem=dataItemElem->FindNestedElementWithName("Name");
      if ( dataItemNameElem == NULL )
      {
        // name is undefined
        continue; 
      }
      if (itemName.compare(dataItemNameElem->GetCharacterData())!=0)
      {
        // item name does not match the item that we are looking for
        continue;
      }      
      // data item description found
      return foundItem.ReadDescriptionFromXml(dataItemElem);
    } 
  } 

  LOG_ERROR("Data item desctiption not found: "<<itemName);
  return PLUS_FAIL;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::LoadFirmwareDescriptionForConnectedDevice()
{
  LOG_TRACE("vtkChRoboticsTracker::LoadFirmwareDescriptionForConnectedDevice");
  this->FirmwareVersionId.empty();

  ChrSerialPacket fwRequest;
  fwRequest.SetHasData(false);
  fwRequest.SetBatchEnable(false);
  fwRequest.SetAddress(UM6_GET_FW_VERSION);  

  ChrSerialPacket fwReply;

  if (SendCommand(fwRequest, fwReply)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to query ChRobotics device firmware version");
    return PLUS_FAIL;
  }

  // Convert packet data to string
  unsigned char dataLength=fwReply.GetDataLength();
  for (int i=0; i<dataLength; i++)
  {
    this->FirmwareVersionId.push_back(fwReply.GetDataByte(i));
  }

  // Search loaded firmware items to determine if we have a definition
  // for the firmware revision given by the sensor.
  if (FindFirmwareDefinition(this->FirmwareVersionId, this->FirmwareDefinition)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to load firmware definition for ChRobotics device version "<<this->FirmwareVersionId);
    return PLUS_FAIL;
  }

  LOG_INFO("CHRobotics device firmware identified ("<< this->FirmwareVersionId <<") and firmware definition loaded." );
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::SendCommand( ChrSerialPacket& requestPacket, ChrSerialPacket& replyPacket )
{
  if (SendPacket( requestPacket )!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  for (int i=0; i<MAX_COMMAND_REPLY_WAIT; i++)
  {
    ReceivePacket(replyPacket);    
    if (replyPacket.GetAddress()==requestPacket.GetAddress())
    {
      // we've received reply to the command
      return PLUS_SUCCESS;
    }    
  }
  LOG_ERROR("Failed to receive reply to command "<<int(requestPacket.GetAddress()));
  return PLUS_FAIL;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::SendPacket( ChrSerialPacket& packet )
{
  std::vector<unsigned char> data_array;

  packet.ComputeChecksum();
  unsigned char packetLength=packet.GetPacketLength();
  for( int i = 0; i < packetLength; i++ )
  {
    this->Serial->Write(packet.GetPacketByte(i));
  }

  LOG_TRACE("Sent packet: address="<<int(packet.GetAddress())<<", data length="<<int(packet.GetDataLength()));

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::ReceivePacket( ChrSerialPacket& packet )
{
  // search for the packet start sequence "snp"
  std::deque<unsigned char> lastThreeChars;
  unsigned char d=0;
  this->Serial->Read(d);
  lastThreeChars.push_back(d);
  this->Serial->Read(d);
  lastThreeChars.push_back(d);
  this->Serial->Read(d);
  lastThreeChars.push_back(d);
  while (lastThreeChars[0]!='s' || lastThreeChars[1]!='n' || lastThreeChars[2]!='p')
  {
    lastThreeChars.pop_front();
    if (!this->Serial->Read(d))
    {
      LOG_ERROR("Failed to read packet header (start sequence) through serial line");
      return PLUS_FAIL;
    }
    lastThreeChars.push_back(d);
  }

  if (!this->Serial->Read(d))
  {
    LOG_ERROR("Failed to read packet header (packet descriptor) through serial line");
    return PLUS_FAIL;
  }
  packet.SetPacketDescriptor(d);

  if (!this->Serial->Read(d))
  {
    LOG_ERROR("Failed to read packet header (address) through serial line");
    return PLUS_FAIL;
  }
  packet.SetAddress(d);

  // Copy data bytes into packet data array
  int dataLength = packet.GetDataLength();
  for( int i = 0; i < dataLength; i++ )
  {
    if (!this->Serial->Read(d))
    {
      LOG_ERROR("Failed to read packet data["<<i<<"] through serial line");
      return PLUS_FAIL;
    }
    packet.SetDataByte( i, d );
  }

  unsigned char checksum0=0;
  if (!this->Serial->Read(checksum0))
  {
    LOG_ERROR("Failed to read packet checksum[0] through serial line");
    return PLUS_FAIL;
  }
  unsigned char checksum1=0;
  if (!this->Serial->Read(checksum1))
  {
    LOG_ERROR("Failed to read packet checksum[1] through serial line");
    return PLUS_FAIL;
  }

  // Compute the checksum and compare with the one given in the packet.  If different, ignore this packet
  packet.ComputeChecksum();

  if (packet.GetChecksumByte(0)!=checksum0 || packet.GetChecksumByte(1)!=checksum1)
  {
    LOG_ERROR("Received a packet with bad checksum through serial line ("
      <<"address="<<int(packet.GetAddress())<<", checksum: "
      <<int(packet.GetChecksumByte(0))<<"!="<<int(checksum0)<<" and/or "
      <<int(packet.GetChecksumByte(1))<<"!="<<int(checksum1)<<")"
      );
    return PLUS_FAIL;
  }

  LOG_TRACE("Received packet: address="<<int(packet.GetAddress())<<", data length="<<int(packet.GetDataLength()));

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::ProcessPacket( ChrSerialPacket& packet )
{
  // Firmware-independent processing

  if( packet.GetAddress() == UM6_BAD_CHECKSUM )
  {
    LOG_WARNING("Received ChRobotics sensor reply: BAD_CHECKSUM");
    return PLUS_FAIL;
  }
  else if( packet.GetAddress() == UM6_UNKNOWN_ADDRESS )
  {
    LOG_WARNING("Received ChRobotics sensor reply: UNKNOWN_ADDRESS");
    return PLUS_FAIL;
  }
  else if( packet.GetAddress() == UM6_INVALID_BATCH_SIZE )
  {
    LOG_WARNING("Received ChRobotics sensor reply: INVALID_BATCH_SIZE");
    return PLUS_FAIL;
  }

  if( packet.GetHasData() )
  {
    // Packet has data.  

    // If this packet reported the contents of data registers, update local data registers accordingly
    if( (packet.GetAddress() >= DATA_REGISTER_START_ADDRESS) && (packet.GetAddress() < COMMAND_START_ADDRESS) )
    {
      UpdateDataItemValues(packet);   
    }    
    else if( packet.GetAddress() < DATA_REGISTER_START_ADDRESS )
    {
      // this packet reported the contents of configuration registers
      // we could get here the new configuration register value
    }
    else
    {
      // command received
      LOG_WARNING("Improperly formatted packet: commands should never contain data");
    }
  }
  else
  {
    // Packet has no data
    if( packet.GetAddress() < DATA_REGISTER_START_ADDRESS )
    {
      // configuration register address
      // the packet signifies that a write operation to a configuration register was just completed
    }
    else if( packet.GetAddress() >= COMMAND_START_ADDRESS )
    {
      // command register address
      // the packet signals that a received command either succeeded or failed
    }
  }

  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::ReadConfiguration(vtkXMLDataElement* config)
{
  // Read superclass configuration first
  Superclass::ReadConfiguration(config); 

  if ( config == NULL ) 
  {
    LOG_WARNING("Unable to find BrachyTracker XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(config);
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  unsigned long serialPort(0); 
  if ( trackerConfig->GetScalarAttribute("SerialPort", serialPort) ) 
  {
    this->ComPort=serialPort; 
  }

  unsigned long baudRate = 0; 
  if ( trackerConfig->GetScalarAttribute("BaudRate", baudRate) ) 
  {
    this->BaudRate=baudRate; 
  }

  const char* firmwareDirectory = trackerConfig->GetAttribute("FirmwareDirectory"); 
  if ( firmwareDirectory != NULL )
  { 
    this->FirmwareDirectory=firmwareDirectory;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  if ( rootConfigElement == NULL )
  {
    LOG_ERROR("Configuration is invalid");
    return PLUS_FAIL;
  }

  // Write configuration 
  Superclass::WriteConfiguration(rootConfigElement); 

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(rootConfigElement);
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  trackerConfig->SetUnsignedLongAttribute( "SerialPort", this->ComPort ); 
  trackerConfig->SetDoubleAttribute( "BaudRate", this->BaudRate ); 

  return PLUS_SUCCESS;
}
