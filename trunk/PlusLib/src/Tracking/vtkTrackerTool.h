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

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

  /*! Get the tracked tool buffer */
  vtkGetObjectMacro(Buffer,vtkTrackerBuffer);

  /*! Get the tracker which owns this tool. */
  vtkGetObjectMacro(Tracker,vtkTracker);

  /*! Get a numeric identifier for this tool i.e. 0, 1, 2. */
  vtkGetMacro(ToolPort,int);

  /*! Get port name. Port name is used to identify the tool among all the tools provided by the tracker device. */
  vtkGetStringMacro(PortName); 

  /*! Get the frame number (some devices has frame numbering, otherwise just increment if new frame received) */
  vtkGetMacro(FrameNumber, unsigned long);
  vtkSetMacro(FrameNumber, unsigned long);

  // TODO: remove it! 
  /*! Set a calibration matrix.  The raw tool transform will be pre-multiplied by this to provide this->Transform. */
  vtkSetObjectMacro(CalibrationMatrix, vtkMatrix4x4); 
  vtkGetObjectMacro(CalibrationMatrix, vtkMatrix4x4); 

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
  /*! Set tool port number */
  vtkSetMacro(ToolPort, int);
  /*! Set port name. Port name is used to identify the tool among all the tools provided by the tracker device. */
  vtkSetStringMacro(PortName); 
  /*! Set tool revision */
  vtkSetStringMacro(ToolRevision);
  /*! Set tool manufacturer */
  vtkSetStringMacro(ToolManufacturer);
  /*! Set tool part number */
  vtkSetStringMacro(ToolPartNumber);
  /*! Set tool serial number */
  vtkSetStringMacro(ToolSerialNumber);
  /*! Set tool name */
  vtkSetStringMacro(ToolName);
  /*! Set tool model */
  vtkSetStringMacro(ToolModel); 

  /*! Set the tool type */
  void SetToolType(TRACKER_TOOL_TYPE type) { ToolType = type; }
  /*! Get the tool type */
  TRACKER_TOOL_TYPE GetToolType() { return this->ToolType; }

  /*! Get tool definition data (model to tool transform, tool registration transform, model file path and filename) */
  vtkGetObjectMacro(ModelToToolTransform, vtkTransform);
  /*! Set tool definition data (model to tool transform, tool registration transform, model file path and filename) */
  vtkSetObjectMacro(ModelToToolTransform, vtkTransform);

  /*! Get name of the file containing the 3D model of this tool */
  vtkGetStringMacro(Tool3DModelFileName); 
  /*! Set name of the file containing the 3D model of this tool */
  vtkSetStringMacro(Tool3DModelFileName); 

  /*! Set calibration matrix name */
  vtkSetStringMacro(CalibrationMatrixName); 
  /*! Get calibration matrix name */
  vtkGetStringMacro(CalibrationMatrixName); 

  /*! Set calibration date */
  vtkSetStringMacro(CalibrationDate); 
  /*! Get calibration date */
  vtkGetStringMacro(CalibrationDate); 

  /*! Set the tool 'SendTo' link for OpenIGTLink broadcasting */
  vtkSetStringMacro(SendToLink);
  /*! Get the tool 'SendTo' link for OpenIGTLink broadcasting */
  vtkGetStringMacro(SendToLink);
  
  /*! Set calibration error */
  vtkSetMacro(CalibrationError, double); 
  /*! Get calibration error */
  vtkGetMacro(CalibrationError, double); 

  /*! Make this tracker into a copy of another tracker. You should lock both of the tracker buffers before doing this. */
  void DeepCopy(vtkTrackerTool *tool);

  /*! Flag to store tool availability */
	vtkSetMacro(Enabled, bool); 
  /*! Flag to store tool availability */
	vtkGetMacro(Enabled, bool);
  /*! Flag to store tool availability */
	vtkBooleanMacro(Enabled, bool); 

protected:
  vtkTrackerTool();
  ~vtkTrackerTool();

  vtkTracker *Tracker;

  int ToolPort;
  char *PortName;

  TRACKER_TOOL_TYPE ToolType;

  vtkMatrix4x4 *CalibrationMatrix;

  unsigned long FrameNumber; 

  int LED1;
  int LED2;
  int LED3;

  bool Enabled; 

  char *ToolRevision;
  char *ToolSerialNumber;
  char *ToolPartNumber;
  char *ToolManufacturer;
  char *ToolName; 
  char *CalibrationMatrixName; 
  char *CalibrationDate; 
  char *SendToLink;
  char *ToolModel;

  char *Tool3DModelFileName;
  vtkTransform *ModelToToolTransform;

  double CalibrationError; 

  vtkTrackerBuffer *Buffer;

private:
  vtkTrackerTool(const vtkTrackerTool&);
  void operator=(const vtkTrackerTool&);  

};

#endif







