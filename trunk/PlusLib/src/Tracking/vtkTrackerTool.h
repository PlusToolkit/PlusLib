/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

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
ODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
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

class VTK_EXPORT vtkTrackerTool : public vtkObject
{
public:

  static vtkTrackerTool *New();
  vtkTypeMacro(vtkTrackerTool,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Read/write main configuration from/to xml data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

  // Description:
  // Get a reference to the transform associated with this tool.  The
  // transform will automatically update when Update() is called
  // on the tracking system.  You can connect this transform or its
  // matrix to a vtkActor.
  /*vtkGetObjectMacro(Transform,vtkTransform);*/

  // Description:
  // Get a running list of all the transforms received for this
  // tool.  See the vtkTrackerBuffer class for more information.
  vtkGetObjectMacro(Buffer,vtkTrackerBuffer);

  // Description:
  // Get the tracker which owns this tool. 
  vtkGetObjectMacro(Tracker,vtkTracker);

  // Description:
  // Get a numeric identifier for this tool i.e. 0, 1, 2.
  vtkGetMacro(ToolPort,int);

  // Description:
  // Get the frame number (some devices has frame numbering, otherwise 
  // just increment if new frame received)
  vtkGetMacro(FrameNumber, unsigned long);
  vtkSetMacro(FrameNumber, unsigned long);

  // Description:
  // Set a calibration matrix.  The raw tool transform will be pre-multiplied
  // by this to provide this->Transform.  This allows you to define a
  // custom coordinate system for the tool which is different from the
  // manufacturer's tool coordinate system.
  // Warning: the calibration matrix is copied, not referenced.
  vtkSetObjectMacro(CalibrationMatrix, vtkMatrix4x4); 
  vtkGetObjectMacro(CalibrationMatrix, vtkMatrix4x4); 

  // Description:
  // Set the states of the LEDs on the tool.  If the tracking system
  // is not in tracking mode, the state will not be realized until
  // the system enters tracking mode.  The states are 0 (off), 1 (on),
  // and 2 (flashing).  Not all tracking systems support LEDs.
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

  // Description:
  // Get miscellaneous information about the tool.  Most
  // tracking systems only support a subset of these (or none at all).
  vtkGetStringMacro(ToolRevision);
  vtkGetStringMacro(ToolManufacturer);
  vtkGetStringMacro(ToolPartNumber);
  vtkGetStringMacro(ToolSerialNumber);
  vtkGetStringMacro(ToolName); 
  vtkGetStringMacro(ToolModel); 

  //BTX
  // Description:
  // To be used only by the vtkTracker class
  void SetTracker(vtkTracker *tracker);
  vtkSetMacro(ToolPort, int);
  vtkSetStringMacro(ToolRevision);
  vtkSetStringMacro(ToolManufacturer);
  vtkSetStringMacro(ToolPartNumber);
  vtkSetStringMacro(ToolSerialNumber);
  vtkSetStringMacro(ToolName);
  vtkSetStringMacro(ToolModel); 
  //void Update();
  //ETX

  // Description:
  // Set/Get the tool type 
  void SetToolType(TRACKER_TOOL_TYPE type) { ToolType = type; }
  TRACKER_TOOL_TYPE GetToolType() { return this->ToolType; }

  // Description
  // Set/Get Tool definition data (model to tool transform, tool registration transform, model file path and filename)
  vtkGetObjectMacro(ModelToToolTransform, vtkTransform);
  vtkSetObjectMacro(ModelToToolTransform, vtkTransform);

  // Description
  // Set/Get name of the file containing the 3D model of this tool
  vtkGetStringMacro(Tool3DModelFileName); 
  vtkSetStringMacro(Tool3DModelFileName); 

  // Description:
  // Set/get calibration matrix name
  vtkSetStringMacro(CalibrationMatrixName); 
  vtkGetStringMacro(CalibrationMatrixName); 

  // Description:
  // Set/get calibration date
  vtkSetStringMacro(CalibrationDate); 
  vtkGetStringMacro(CalibrationDate); 

  // Description:
  // Set/get the tool 'SendTo' link for OpenIGTLink broadcasting
  vtkSetStringMacro(SendToLink);
  vtkGetStringMacro(SendToLink);
  
  // Description:
  // Set/get calibration error
  vtkSetMacro(CalibrationError, double); 
  vtkGetMacro(CalibrationError, double); 

  // Description:
  // Make this tracker into a copy of another tracker.
  // You should lock both of the tracker buffers before doing this.
  void DeepCopy(vtkTrackerTool *tool);

  	// Description:
	// Set/Get the tool availability 
	vtkSetMacro(Enabled, bool); 
	vtkGetMacro(Enabled, bool);
	vtkBooleanMacro(Enabled, bool); 

protected:
  vtkTrackerTool();
  ~vtkTrackerTool();

  vtkTracker *Tracker;

  int ToolPort;

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







