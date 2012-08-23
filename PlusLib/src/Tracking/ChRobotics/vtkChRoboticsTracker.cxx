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

#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"
 
#include <math.h>

vtkStandardNewMacro(vtkChRoboticsTracker);

//-------------------------------------------------------------------------
// Definitions of constant register address (addresses that must remain the same on all firmware versions)
static const unsigned char UM6_GET_FW_VERSION=170;

// Definitions of some error packets - these are also required to remain constant through all firmware revisions
static const unsigned char UM6_BAD_CHECKSUM=253;
static const unsigned char UM6_UNKNOWN_ADDRESS=254;
static const unsigned char UM6_INVALID_BATCH_SIZE=255;

//-------------------------------------------------------------------------
vtkChRoboticsTracker::vtkChRoboticsTracker() :
  ComPort(5),
  BaudRate(115200)
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
  ReceivePacket( reply );

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
    this->Serial->Read(d);
    lastThreeChars.push_back(d);
  }
  
  LOG_DEBUG("OK!");

  /*
	int BytesReturned;
	bool found_packet;
	int packet_start_index;
	int packet_index;

	RXBufPtr += BytesReturned;

	// If there are enough bytes in the buffer to construct a full packet, then check data.
    // There are RXbufPtr bytes in the buffer at any given time
    while (RXBufPtr >= 7)
    {
        // Search for the packet start sequence
        found_packet = false;
        packet_start_index = 0;
        for (packet_index = 0; packet_index < (RXBufPtr - 2); packet_index++)
        {
            if (RXBuffer[packet_index] == 's' && RXBuffer[packet_index + 1] == 'n' && RXBuffer[packet_index + 2] == 'p')
            {
                found_packet = true;
                packet_start_index = packet_index;

                break;
            }
        }

		// If packet start sequence was not found, then remove all but the last three bytes from the buffer.  This prevents
		// bad data from filling the buffer up.
		if( !found_packet )
		{
			RXBuffer[0] = RXBuffer[RXBufPtr-3];
			RXBuffer[1] = RXBuffer[RXBufPtr-2];
			RXBuffer[2] = RXBuffer[RXBufPtr-1];

			RXBufPtr = 3;
		}

		// If a packet start sequence was found, extract the packet descriptor byte.
		// Make sure that there are enough bytes in the buffer to consitute a full packet
		int indexed_buffer_length = RXBufPtr - packet_start_index;
		if (found_packet && (indexed_buffer_length >= 7))
		{
			unsigned char packet_descriptor = (UInt32)RXBuffer[packet_start_index + 3];
			unsigned char address = (UInt32)RXBuffer[packet_start_index + 4];

			// Check the R/W bit in the packet descriptor.  If it is set, then this packet does not contain data 
			// (the packet is either reporting that the last write operation was succesfull, or it is reporting that
			// a command finished).
			// If the R/W bit is cleared and the batch bit B is cleared, then the packet is 11 bytes long.  Make sure
			// that the buffer contains enough data to hold a complete packet.
			bool HasData;
			bool IsBatch;
			unsigned char BatchLength;

			int packet_length;

			if( ( packet_descriptor & 0x80 ) )
			{
				HasData = true;
			}
			else
			{
				HasData = false;
			}

			if( ( packet_descriptor & 0x40 ) )
			{
				IsBatch = true;
			}
			else
			{
				IsBatch = false;
			}
			
			if( HasData && !IsBatch )
			{
				packet_length = 11;
			}
			else if( HasData && IsBatch )
			{
				// If this is a batch operation, then the packet length is: length = 5 + 4*L + 2, where L is the length of the batch.
				// Make sure that the buffer contains enough data to parse this packet.
				BatchLength = (packet_descriptor >> 2) & 0x0F;
				packet_length = 5 + 4*BatchLength + 2;				
			}
			else if( !HasData )
			{
				packet_length = 7;
			}

			if( indexed_buffer_length < packet_length )
			{
				return;
			}

			SerialPacket^ NewPacket = gcnew SerialPacket();

			// If code reaches this point, then there enough bytes in the RX buffer to form a complete packet.
			NewPacket->Address = address;
			NewPacket->PacketDescriptor = packet_descriptor;

			// Copy data bytes into packet data array
			int data_start_index = packet_start_index + 5;
			for( int i = 0; i < NewPacket->DataLength; i++ )
			{
				NewPacket->SetDataByte( i, RXBuffer[data_start_index + i] );
			}

			// Now record the checksum
			UInt16 Checksum = ((UInt16)RXBuffer[packet_start_index + packet_length - 2] << 8) | ((UInt16)RXBuffer[packet_start_index + packet_length - 1]);

			// Compute the checksum and compare with the one given in the packet.  If different, ignore this packet
			NewPacket->ComputeChecksum();

			if( Checksum == NewPacket->Checksum )
			{
				OnSerialPacketReceived( NewPacket );
			}
			else
			{
				OnSerialPacketError(L"Received packet with bad checksum.  Packet discarded.");
			}

			// At this point, the newest packet has been parsed and copied into the RXPacketBuffer array.
			// Copy all received bytes that weren't part of this packet into the beginning of the
            // buffer.  Then, reset RXbufPtr.
            for (int index = 0; index < (RXBufPtr - (packet_start_index + packet_length)); index++)
            {
                RXBuffer[index] = RXBuffer[(packet_start_index + packet_length) + index];
            }

			RXBufPtr -= (packet_start_index + packet_length);
		}
		else
		{
			return;
		}
	
	}
  */
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