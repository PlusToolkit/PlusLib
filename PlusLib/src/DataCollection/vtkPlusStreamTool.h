/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// .NAME vtkPlusStreamTool - interfaces VTK to a handheld 3D positioning tool
// .SECTION Description
// The vtkPlusStreamTool provides an interface between a tracked object in
// the real world and a virtual object.
// .SECTION see also
// vtkTracker vtkPOLARISTracker vtkFlockTracker

#ifndef __vtkPlusStreamTool_h
#define __vtkPlusStreamTool_h

#include "vtkObject.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStreamBuffer.h"

/*!
\class vtkPlusStreamTool 
\brief Interface to a handheld 3D positioning tool

The vtkPlusStreamTool provides an interface between a tracked object in
the real world and a virtual object.

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkPlusStreamTool : public vtkObject
{
public:

  static vtkPlusStreamTool *New();
  vtkTypeMacro(vtkPlusStreamTool,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* toolElement, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration = false); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* toolElement); 
  virtual PlusStatus WriteCompactConfiguration(vtkXMLDataElement* toolElement); 

  /*! Set tool name. Tool name is used to identify the tool among all the tools provided by the tracker device 
  therefore it must be unique and can be set only once */
  PlusStatus SetToolName(const char* toolName);

  /*! Set port name. Port name is used to identify the tool among all the tools provided by the tracker device 
  therefore it must be unique and can be set only once */
  PlusStatus SetPortName(const char* portName);

  /*! Get the tracked tool buffer */
  virtual vtkSmartPointer<vtkPlusStreamBuffer> GetBuffer() const { return this->Buffer; }

  /*! Get the tracker which owns this tool. */
  vtkGetObjectMacro(Device,vtkPlusDevice);

  /*! Get port name. Port name is used to identify the tool among all the tools provided by the tracker device. */
  vtkGetStringMacro(PortName); 

  /*! Get the frame number (some devices has frame numbering, otherwise just increment if new frame received) */
  vtkGetMacro(FrameNumber, unsigned long);
  vtkSetMacro(FrameNumber, unsigned long);

  /*! 
  Set the states of the LEDs on the tool.  If the tracking system is not in tracking mode, the state will not be realized until
  the system enters tracking mode.  The states are 0 (off), 1 (on), and 2 (flashing).  Not all tracking systems support LEDs.
  */
  void SetLED1(int state);
  void SetLED2(int state);
  void SetLED3(int state);
  void LED2Off() { this->SetLED2(TR_LED_OFF); };
  void LED3Off() { this->SetLED3(TR_LED_OFF); };
  void LED1Off() { this->SetLED1(TR_LED_OFF); };
  void LED1On() { this->SetLED1(TR_LED_ON); };
  void LED2On() { this->SetLED2(TR_LED_ON); };
  void LED3On() { this->SetLED3(TR_LED_ON); };
  void LED1Flash() { this->SetLED1(TR_LED_FLASH); };
  void LED2Flash() { this->SetLED2(TR_LED_FLASH); };
  void LED3Flash() { this->SetLED3(TR_LED_FLASH); };
  vtkGetMacro(LED1,int);
  vtkGetMacro(LED2,int);
  vtkGetMacro(LED3,int);

  /*! Get tool revision */
  vtkGetStringMacro(ToolRevision);
  /*! Get tool manufacturer */
  vtkGetStringMacro(ToolManufacturer);
  /*! Get tool part number */
  vtkGetStringMacro(ToolPartNumber);
  /*! Get tool serial number */
  vtkGetStringMacro(ToolSerialNumber);
  /*! Get tool name */
  vtkGetStringMacro(ToolName); 

  /*! Set tracker which owns this tool */
  void SetDevice(vtkPlusDevice *device);
  
  /*! Set tool revision */
  vtkSetStringMacro(ToolRevision);
  /*! Set tool manufacturer */
  vtkSetStringMacro(ToolManufacturer);
  /*! Set tool part number */
  vtkSetStringMacro(ToolPartNumber);
  /*! Set tool serial number */
  vtkSetStringMacro(ToolSerialNumber);
    
  /*! Make this tracker into a copy of another tracker. You should lock both of the tracker buffers before doing this. */
  void DeepCopy(vtkPlusStreamTool *tool);

protected:
  vtkPlusStreamTool();
  ~vtkPlusStreamTool();

  vtkPlusDevice *Device;

  char *PortName;

  unsigned long FrameNumber; 

  int LED1;
  int LED2;
  int LED3;

  char *ToolRevision;
  char *ToolSerialNumber;
  char *ToolPartNumber;
  char *ToolManufacturer;
  char *ToolName; 

  vtkSmartPointer<vtkPlusStreamBuffer> Buffer;

private:
  vtkPlusStreamTool(const vtkPlusStreamTool&);
  void operator=(const vtkPlusStreamTool&);  

};

#endif







