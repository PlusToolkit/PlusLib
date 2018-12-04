/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGenericSerialDevice_h
#define __vtkPlusGenericSerialDevice_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

class SerialLine;

/*!
\class vtkPlusGenericSerialDevice
\brief Generic interface for communicating with a serial device

This class communicates with any serial (RS-232) device. It allows sending and receiving data
using OpenIGTLink commands.

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusGenericSerialDevice : public vtkPlusDevice
{
public:

  static vtkPlusGenericSerialDevice* New();
  vtkTypeMacro(vtkPlusGenericSerialDevice, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

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

  /*! Specifies what kind of response is expected from the device */
  enum ReplyTermination
  {
    REQUIRE_LINE_ENDING = 0, // only proper LineEnding termination will result in success
    REQUIRE_NOT_EMPTY = 1, // if respnse is not empty, results in success even on timeout
    ANY = 2 // always results in success, but waits for a timeout or LineEnding termination
  };

  /*!
    Send text to the serial device. If a non-NULL pointer is passed as textReceived
    then the device waits for a response and returns it in textReceived.

    If acceptReply is not REQUIRE_NOT_EMPTY, command is considered successful if non-empty
    reply is received within timeout, even if it not terminated by a LineEnding.
    Some devices (e.g. Velmex VXM) has both kinds of commands, e.g. `V` command is not CR terminated:
    http://www.velmex.com/Downloads/Spec_Sheets/VXM%20-%20%20Command%20Summary%20Rev%20B%20814.pdf
  */
  virtual PlusStatus SendText(const std::string& textToSend, std::string* textReceived = NULL,
    ReplyTermination acceptReply = REQUIRE_LINE_ENDING);

  /*!
    Receive a response from the serial device.

    In case that a device has both terminated and unterminated responses,
    acceptReply needs to be specified per command.
  */
  virtual PlusStatus ReceiveResponse(std::string& textReceived, ReplyTermination acceptReply = REQUIRE_LINE_ENDING);

  virtual PlusStatus NotifyConfigured();

  /*! Sets the DTR (data-terminal-ready) line. */
  PlusStatus SetDTR(bool onOff);

  /*! Gets the DTR (data-terminal-ready) line. */
  vtkGetMacro(DTR, bool);

  /*! Sets the RTS (request-to-send) line. */
  PlusStatus SetRTS(bool onOff);

  /*! Gets the RTS (request-to-send) line. */
  vtkGetMacro(RTS, bool);

  /*! Gets the DTR (data-set-ready) line. */
  PlusStatus GetDSR(bool& onOff);

  /*! Gets the RTS (clear-to-send) line. */
  PlusStatus GetCTS(bool& onOff);

protected:
  vtkPlusGenericSerialDevice();
  ~vtkPlusGenericSerialDevice();

  /*! Wait until the serial device makes some data available for reading but maximum up to ReplyTimeoutSec */
  virtual bool WaitForResponse();

private:
  vtkPlusGenericSerialDevice(const vtkPlusGenericSerialDevice&);
  void operator=(const vtkPlusGenericSerialDevice&);

protected:
  /*! Serial (RS232) line connection */
  SerialLine* Serial;

  /*! Used COM port number for serial communication (ComPort: 1 => Port name: "COM1")*/
  unsigned long SerialPort;

  /*! Baud rate for serial communication. */
  unsigned long BaudRate;

  /*! Data Terminal Ready */
  bool DTR;

  /*! Request To Send */
  bool RTS;

  /*! Characters that indicate end of sent or received text data. Each character is encoded in 2-digit hexadecimal, separate by spaces. */
  std::string LineEnding;

  /*! Line ending as simple string. Decoded from LineEnding and stored as a member variable to avoid decoding each time it is used. */
  std::string LineEndingBin;

  /*! Maximum time to wait for the device to start replying */
  double MaximumReplyDelaySec;

  /*! Maximum time to wait for the device to finish replying */
  double MaximumReplyDurationSec;

  long FrameNumber;
  vtkPlusDataSource* FieldDataSource;

  /*! Mutex instance for sharing the serial line between update thread and command execution thread */
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection> Mutex;
};

#endif
