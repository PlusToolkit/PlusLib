/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkGenericSerialDevice_h
#define __vtkGenericSerialDevice_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

class SerialLine;

/*!
\class vtkGenericSerialDevice 
\brief Generic interface for communicating with a serial device

This class communicates with any serial (RS-232) device. It allows sending and receiving data
using OpenIGTLink commands.

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkGenericSerialDevice : public vtkPlusDevice
{
public:

  static vtkGenericSerialDevice *New();
  vtkTypeMacro( vtkGenericSerialDevice,vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! 
  Probe to see if the tracking system is present on the specified serial port.  
  */
  PlusStatus Probe();

  /*!
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  This method is called by the tracker thread.
  */
  PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);  

  virtual bool IsTracker() const { return false; }

  vtkSetMacro(SerialPort, unsigned long);
  vtkSetMacro(BaudRate, unsigned long);
  vtkSetMacro(MaximumReplyDelaySec, double);
  vtkSetMacro(MaximumReplyDurationSec, double);

  /*! Line ending in hex encoded form, separated by spaces (e.g., "13 10") */
  void SetLineEnding(const char* lineEndingHex);
  vtkGetMacro(LineEnding, std::string);

  /*!
    Send text to the serial device. If a non-NULL pointer is passed as textReceived
    then the device waits for a response and returns it in textReceived.
  */
  virtual PlusStatus SendText(const std::string& textToSend, std::string* textReceived=NULL);

  /*!
    Receive a response from the serial device.
  */
  virtual PlusStatus ReceiveResponse(std::string& textReceived);

protected:

  vtkGenericSerialDevice();
  ~vtkGenericSerialDevice();

  /*! 
  Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopRecording();

  /*! Wait until the serial device makes some data available for reading but maximum up to ReplyTImeoutSec */
  virtual bool WaitForResponse();

  /*! Serial (RS232) line connection */
  SerialLine* Serial;

  /*! Mutex instance for sharing the serial line between update thread and command execution thread */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> Mutex;

private:  // Functions.

  vtkGenericSerialDevice( const vtkGenericSerialDevice& );
  void operator=( const vtkGenericSerialDevice& );

private:  // Variables.

  /*! Used COM port number for serial communication (ComPort: 1 => Port name: "COM1")*/
  unsigned long SerialPort; 

  /*! Baud rate for serial communication. */
  unsigned long BaudRate;

  /*! Characters that indicate end of sent or received text data. Each character is encoded in 2-digit hexadecimal, separate by spaces. */
  std::string LineEnding;

  /*! Line ending as simple string. Decoded from LineEnding and stored as a member variable to avoid decoding each time it is used. */
  std::string LineEndingBin;

  /*! Maximum time to wait for the device to start replying */
  double MaximumReplyDelaySec;

  /*! Maximum time to wait for the device to finish replying */
  double MaximumReplyDurationSec;

};

#endif
