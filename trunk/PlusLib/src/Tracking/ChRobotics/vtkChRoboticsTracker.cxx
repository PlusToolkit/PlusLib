/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkChRoboticsTracker.h"
#include "SerialLine.h"
#include "ChrSerialPacket.h"

#include <sstream>
#include <deque>

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"

#include "PlusConfigure.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include <math.h>

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
vtkChRoboticsTracker::vtkChRoboticsTracker() :
ComPort(5),
BaudRate(115200),
SelectedFirmwareIndex(-1)
{ 
  this->Serial = new SerialLine(); 

  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;

  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->OrientationSensorTool = NULL;

  this->LastAccelerometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastGyroscopeToTrackerTransform=vtkMatrix4x4::New();
  this->LastMagnetometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastOrientationSensorToTrackerTransform=vtkMatrix4x4::New();
}

//-------------------------------------------------------------------------
vtkChRoboticsTracker::~vtkChRoboticsTracker() 
{
  if ( this->Recording )
  {
    this->StopTracking();
  }

  if (this->Serial->IsHandleAlive())
  {
    this->Serial->Close();
    delete this->Serial; 
    this->Serial = NULL; 
  }

  this->LastAccelerometerToTrackerTransform->Delete();
  this->LastAccelerometerToTrackerTransform=NULL;
  this->LastGyroscopeToTrackerTransform->Delete();
  this->LastGyroscopeToTrackerTransform=NULL;
  this->LastMagnetometerToTrackerTransform->Delete();
  this->LastMagnetometerToTrackerTransform=NULL;
  this->LastOrientationSensorToTrackerTransform->Delete();
  this->LastOrientationSensorToTrackerTransform=NULL;
}

//-------------------------------------------------------------------------
void vtkChRoboticsTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkTracker::PrintSelf( os, indent );

}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::Connect()
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

  // Reset the firmware id, it will be queried from the device
  this->SelectedFirmwareIndex=-1;

  std::string firmwareVersion;
  if (GetFirmwareVersion(firmwareVersion)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to query fimware version of the device");
    this->Serial->Close();
    return PLUS_FAIL;
  }

  this->AccelerometerTool = NULL;
  GetToolByPortName("Accelerometer", this->AccelerometerTool);

  this->GyroscopeTool = NULL;
  GetToolByPortName("Gyroscope", this->GyroscopeTool);

  this->MagnetometerTool = NULL;
  GetToolByPortName("Magnetometer", this->MagnetometerTool);

  this->OrientationSensorTool = NULL;
  GetToolByPortName("OrientationSensor", this->OrientationSensorTool);

  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::Disconnect()
{
  LOG_TRACE( "vtkChRoboticsTracker::Disconnect" ); 
  this->StopTracking();

  this->Serial->Close();

  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->OrientationSensorTool = NULL;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::Probe()
{
  LOG_TRACE( "vtkChRoboticsTracker::Probe" ); 

  return PLUS_SUCCESS; 
} 

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalStartTracking()
{
  LOG_TRACE( "vtkChRoboticsTracker::InternalStartTracking" ); 
  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }  

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalStopTracking()
{
  LOG_TRACE( "vtkChRoboticsTracker::InternalStopTracking" );   

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::InternalUpdate()
{
  LOG_TRACE( "vtkChRoboticsTracker::InternalUpdate" ); 
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::GetFirmwareVersion(std::string &firmwareVersion)
{
  firmwareVersion.empty();

  ChrSerialPacket fwRequest;
  fwRequest.SetHasData(false);
  fwRequest.SetBatchEnable(false);
  fwRequest.SetAddress(UM6_GET_FW_VERSION);  

  if (this->SendPacket( fwRequest )!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  ChrSerialPacket reply;
  while (ReceivePacket( reply )==PLUS_SUCCESS) {};

  return PLUS_SUCCESS;
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
    LOG_ERROR("Received a packet with bad checksum through serial line");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkChRoboticsTracker::ProcessPacket( ChrSerialPacket& packet )
{
  // If no firmware definition has been selected yet, ignore this packet if it isn't reporting the firmware version
  if( this->SelectedFirmwareIndex == -1 )
  {
    if( packet.GetAddress() != UM6_GET_FW_VERSION )
    {
      return PLUS_SUCCESS;
    }
  }

  // Check if the received packet is a firmware definition
  if( packet.GetAddress() == UM6_GET_FW_VERSION )
  {
    std::string packetFirmwareId;
    unsigned char dataLength=packet.GetDataLength();
    for (int i=0; i<dataLength; i++)
    {
      packetFirmwareId.push_back(packet.GetDataByte(i));
    }

    // This is a firmware definition packet.  Search loaded firmware items to determine if we have a definition
    // for the firmware revision given by the sensor.
    /* TODO:
    for( int i = 0; i < this->FirmwareCount; i++ )
    {
    std::string firmwareId = FirmwareArray[i]->GetID();
    if( firmwareId.compare(packetFirmwareId)==0 )
    {
    // found firmware
    this->SelectedFirmwareIndex = i;
    LOG_INFO("CHRobotics device firmware identified: " << firmwareId);
    break;
    }									
    }
    */
    if( SelectedFirmwareIndex == -1 )
    {
      LOG_ERROR("Could not find firmware definition for attached device ("<<packetFirmwareId<<")");
      return PLUS_FAIL;
    }
  }
  else if( packet.GetAddress() == UM6_BAD_CHECKSUM )
  {
    LOG_TRACE("Received BAD_CHECKSUM message from sensor");
    return PLUS_FAIL;
  }
  else if( packet.GetAddress() == UM6_UNKNOWN_ADDRESS )
  {
    LOG_ERROR("Received UNKNOWN_ADDRESS message from sensor.");
    return PLUS_FAIL;
  }
  else if( packet.GetAddress() == UM6_INVALID_BATCH_SIZE )
  {
    LOG_ERROR("Received INVALID_BATCH_SIZE message from sensor.");
    return PLUS_FAIL;
  }
  else if( packet.GetHasData() )
  {
    // Packet has data.  Copy data into local registers.
    unsigned char regIndex = 0;
    /* TODO: */
    unsigned char dataLength=packet.GetDataLength();
    for( int i = 0; i < dataLength; i += 4 )
    {
      int data = (packet.GetDataByte(i) << 24) | (packet.GetDataByte(i+1) << 16) | (packet.GetDataByte(i+2) << 8) | (packet.GetDataByte(i+3));
      //TODO: FirmwareArray[SelectedFirmwareIndex]->SetRegisterContents( packet.GetAddress() + regIndex, data );
      regIndex++;
    }

    // If this packet reported the contents of data registers, update local data registers accordingly
    if( (packet.GetAddress() >= DATA_REGISTER_START_ADDRESS) && (packet.GetAddress() < COMMAND_START_ADDRESS) )
    {
      // Copy new register data into individual items for display in GUI
      //updateItemsSafe( DATA_UPDATE );	

      // If magnetometer data collection (for calibration) is enabled, check to determine if this packet contained raw
      // magnetometer data.  This code is device-specific, and it would be preferable to find another way to do this
      // since the rest of this interface software makes no assumptions about the formatting of the registers aboard
      // the device... Anyway, this code should work for any device with ID UM1*, where * can be anything.
      /* TODO:
      String^ firmware_id = FirmwareArray[SelectedFirmwareIndex]->GetID();
      if( firmware_id->Substring(0,2)  == "UM" )
      {
        // Is data collection enabled?
        if( this->magDataCollectionEnabled )
        {
          // Check to see if this is raw mag. data (register address of first raw mag register is 90.  Second is 91.
          if( packet->IsBatch && (packet->BatchLength == 2) && (packet.GetAddress() == 90) )
          {
            // New raw mag data has arrived.  Write it to the mag logging array.
            Int16 mag_x = (packet->GetDataByte(0) << 8) | (packet->GetDataByte(1));
            Int16 mag_y = (packet->GetDataByte(2) << 8) | packet->GetDataByte(3);
            Int16 mag_z = (packet->GetDataByte(4) << 8) | packet->GetDataByte(5);

            // Make sure we don't read too much data
            if( this->rawMagDataPointer < MAXIMUM_MAG_DATA_POINTS )
            {
              this->rawMagData[this->rawMagDataPointer,0] = (double)mag_x;
              this->rawMagData[this->rawMagDataPointer,1] = (double)mag_y;
              this->rawMagData[this->rawMagDataPointer,2] = (double)mag_z;

              this->rawMagDataPointer++;

              updateMagCounterLabelSafe();
            }
          }
        }
      }
      */
    }
    // If this packet reported the contents of configuration registers, update local configuration registers accordingly
    else if( packet.GetAddress() < DATA_REGISTER_START_ADDRESS )
    {
      //					FirmwareRegister^ current_register = FirmwareArray[SelectedFirmwareIndex]->GetRegister(packet.GetAddress());
      //updateItemsSafe( CONFIG_UPDATE );
      //					this->addStatusTextSafe(L"Received " + current_register->Name + " register contents.", Color::Green);
    }
    // This should never be reached for a properly formatted packet (ie. commands should never contain data)
    else
    {

    }
  }
  // Packet has not data.  If a configuration register address, the packet signifies that a write operation
  // to a configuration register was just completed.
  else if( packet.GetAddress() < DATA_REGISTER_START_ADDRESS )
  {
    /* TODO:
    FirmwareRegister^ current_register = FirmwareArray[SelectedFirmwareIndex]->GetRegister(packet.GetAddress());
    current_register->UserModified = false;
    this->addStatusTextSafe(L"Successfully wrote to " + current_register->Name + " register.", Color::Green);
    this->resetTreeNodeColorSafe( packet.GetAddress() );
    */
  }
  // Packet has no data.  If a command register address, the packet signals that a received command either succeeded or
  // failed.
  else if( packet.GetAddress() >= COMMAND_START_ADDRESS )
  {
    /* TODO:
    String^ command_name = FirmwareArray[SelectedFirmwareIndex]->GetCommandName( packet.GetAddress() );

    // Check to see if command succeeded
    if( packet->CommandFailed == 1 )
    {					
      this->addStatusTextSafe(L"Command failed: " + command_name, Color::Green);
    }
    else
    {
      this->addStatusTextSafe(L"Command complete: " + command_name, Color::Red);
    }
    */
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

  // Get data collection and then Tracker configuration element
  vtkXMLDataElement* dataCollectionConfig = rootConfigElement->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  trackerConfig->SetUnsignedLongAttribute( "SerialPort", this->ComPort ); 
  trackerConfig->SetDoubleAttribute( "BaudRate", this->BaudRate ); 

  return PLUS_SUCCESS;
}