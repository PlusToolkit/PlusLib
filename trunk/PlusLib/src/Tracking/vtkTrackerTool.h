/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// .NAME vtkTrackerTool - interfaces VTK to a handheld 3D positioning tool
// .SECTION Description
// The vtkTrackerTool provides an interface between a tracked object in
// the real world and a virtual object.
// .SECTION see also
// vtkTracker vtkPOLARISTracker vtkFlockTracker

#ifndef __vtkTrackerTool_h
#define __vtkTrackerTool_h

#include "vtkObject.h"
#include "vtkTracker.h"
#include "vtkTransform.h"

class vtkMatrix4x4;
class vtkTransform;
class vtkDoubleArray;
class vtkAmoebaMinimizer;
class vtkTrackerBuffer;

/*!
\class vtkTrackerTool 
\brief Interface to a handheld 3D positioning tool

The vtkTrackerTool provides an interface between a tracked object in
the real world and a virtual object.

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkTrackerTool : public vtkObject
{
public:

  static vtkTrackerTool *New();
  vtkTypeMacro(vtkTrackerTool,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Set tool name. Tool name is used to identify the tool among all the tools provided by the tracker device 
  therefore it must be unique and can be set only once */
  PlusStatus SetToolName(const char* toolName);

  /*! Set port name. Port name is used to identify the tool among all the tools provided by the tracker device 
  therefore it must be unique and can be set only once */
  PlusStatus SetPortName(const char* portName);

  /*! Get the tracked tool buffer */
  vtkGetObjectMacro(Buffer,vtkTrackerBuffer);

  /*! Get the tracker which owns this tool. */
  vtkGetObjectMacro(Tracker,vtkTracker);

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
  /*! Get tool model */
  vtkGetStringMacro(ToolModel);

  /*! Set tracker which owns this tool */
  void SetTracker(vtkTracker *tracker);
  
  /*! Set tool revision */
  vtkSetStringMacro(ToolRevision);
  /*! Set tool manufacturer */
  vtkSetStringMacro(ToolManufacturer);
  /*! Set tool part number */
  vtkSetStringMacro(ToolPartNumber);
  /*! Set tool serial number */
  vtkSetStringMacro(ToolSerialNumber);
  
  /*! Set tool model */
  vtkSetStringMacro(ToolModel); 

  /*! Set the tool 'SendTo' link for OpenIGTLink broadcasting */
  vtkSetStringMacro(SendToLink);
  /*! Get the tool 'SendTo' link for OpenIGTLink broadcasting */
  vtkGetStringMacro(SendToLink);
  
  /*! Make this tracker into a copy of another tracker. You should lock both of the tracker buffers before doing this. */
  void DeepCopy(vtkTrackerTool *tool);

protected:
  vtkTrackerTool();
  ~vtkTrackerTool();

  vtkTracker *Tracker;

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

  char *SendToLink;
  char *ToolModel;

  vtkTrackerBuffer *Buffer;

private:
  vtkTrackerTool(const vtkTrackerTool&);
  void operator=(const vtkTrackerTool&);  

};

#endif







