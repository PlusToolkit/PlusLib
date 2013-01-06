/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkBrachyTracker_h
#define __vtkBrachyTracker_h

#include "BrachyStepper.h" 
#include "PlusConfigure.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDevice.h"
#include "vtkTimestampedCircularBuffer.h"

class TrackedFrame; 

/*!
\class vtkBrachyTracker 
\brief Interface to brachytherapy steppers 

This class talks with CMS and CIVCO brachy steppers

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkBrachyTracker : public vtkPlusDevice
{
public:

  /*! Brachy stepper tool types */
  enum BRACHY_STEPPER_TOOL
  {
    PROBEHOME_TO_PROBE_TRANSFORM=0, 
    TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, 
    RAW_ENCODER_VALUES, 
    NUMBER_OF_BRACHY_TOOLS
  }; 

  /*! Row numbers used for storing stepper encoder positions in a 4x4 matrix */
  enum ENCODER_MATRIX_ROW
  {
    ROW_PROBE_POSITION=0, 
    ROW_PROBE_ROTATION=1, 
    ROW_TEMPLATE_POSITION=2
  }; 

  static vtkBrachyTracker *New();
  vtkTypeMacro(vtkBrachyTracker,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present on the specified serial port. */
  PlusStatus Probe();

  /*! Reset the stepper. After this call, you should call InitializeStepper() */
  PlusStatus ResetStepper(); 

  /*! Initialize the stepper with the factory specified init process. */
  PlusStatus InitializeStepper( std::string &calibMsg );

   /*! Get all transforms from buffer element values of each tool by timestamp. */
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame* aTrackedFrame ); 

  /*! Get stepper encoder values from the buffer by UID */
  PlusStatus GetStepperEncoderValues( BufferItemUidType uid, double &probePosition, double &probeRotation, double &templatePosition, ToolStatus &status ); 

  /*! Get latest stepper encoder values from the buffer */
  PlusStatus GetLatestStepperEncoderValues( double &probePosition, double &probeRotation, double &templatePosition, ToolStatus &status ); 

  /*! Get stepper encoder values in a particular timestamp */
  PlusStatus GetStepperEncoderValues(double timestamp, double &probePosition, double &probeRotation, double &templatePosition, ToolStatus &status ); 

  /*! Get probe home to probe transform from the buffer by UID */
  PlusStatus GetProbeHomeToProbeTransform( BufferItemUidType uid, vtkMatrix4x4* probeHomeToProbeMatrix, ToolStatus &status); 

  /*! Get probe home to probe transform in a particular timestamp */
  PlusStatus GetProbeHomeToProbeTransform( double timestamp, vtkMatrix4x4* probeHomeToProbeMatrix, ToolStatus &status); 

  /*! Get template home to template transform from the buffer by UID */
  PlusStatus GetTemplateHomeToTemplateTransform( BufferItemUidType uid, vtkMatrix4x4* templateHomeToTemplateMatrix, ToolStatus &status); 

  /*! Get template home to template transform in a particular timestamp */
  PlusStatus GetTemplateHomeToTemplateTransform( double timestamp, vtkMatrix4x4* templateHomeToTemplateMatrix, ToolStatus &status);

  /*! Get raw encoder values transform from the buffer */
  PlusStatus GetRawEncoderValuesTransform( BufferItemUidType uid, vtkMatrix4x4* rawEncoderValuesTransform, ToolStatus &status ); 

  /*! Get raw encoder values transform in a particular timestamp */
  PlusStatus GetRawEncoderValuesTransform( double timestamp, vtkMatrix4x4* rawEncoderValuesTransform, ToolStatus &status); 

  /*! Get stepper encoder values from the tracked frame */
  static PlusStatus GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition); 

  /*! Get the a string (perhaps a long one) describing the type and version of the device. */
  vtkGetStringMacro(ModelVersion);

  /*! Get stepper model number */
  vtkGetStringMacro(ModelNumber);

  /*! Get stepper serial number */
  vtkGetStringMacro(ModelSerialNumber);

  /*! Set which serial port to use, COM1 through COM4. */
  vtkSetMacro(SerialPort, unsigned long);

  /*! Get which serial port to use, COM1 through COM4. */
  vtkGetMacro(SerialPort, unsigned long);

  /*! Set the desired baud rate.  Default: 9600.  */
  vtkSetMacro(BaudRate, unsigned long);

  /*! Get the desired baud rate.  Default: 9600.  */
  vtkGetMacro(BaudRate, unsigned long);

  /*! Get stepper calibration algorithm version */
  vtkGetStringMacro(CalibrationAlgorithmVersion);

  /*! Set stepper calibration algorithm version */
  vtkSetStringMacro(CalibrationAlgorithmVersion);

  /*! Get stepper calibration date */
  vtkGetStringMacro(CalibrationDate);

  /*! Set stepper calibration date */
  vtkSetStringMacro(CalibrationDate);

  /*! Set probe translation axis orientation vector[Tx, Ty, 1] */
  vtkSetVector3Macro(ProbeTranslationAxisOrientation, double); 

  /*! Get probe translation axis orientation vector[Tx, Ty, 1] */
  vtkGetVector3Macro(ProbeTranslationAxisOrientation, double); 

  /*! Set probe rotation axis orientation vector[Trx, Try, 1] */
  vtkSetVector3Macro(ProbeRotationAxisOrientation, double); 

  /*! Get probe rotation axis orientation vector[Trx, Try, 1] */
  vtkGetVector3Macro(ProbeRotationAxisOrientation, double); 

  /*! Set template translation axis orientation vector[Tx, Ty, 1] */
  vtkSetVector3Macro(TemplateTranslationAxisOrientation, double); 

  /*! Get template translation axis orientation vector[Tx, Ty, 1] */
  vtkGetVector3Macro(TemplateTranslationAxisOrientation, double); 

  /*! Set probe rotation encoder scale */
  vtkSetMacro(ProbeRotationEncoderScale, double); 

  /*! Get probe rotation encoder scale */
  vtkGetMacro(ProbeRotationEncoderScale, double); 

  /*! Enable/disable stepper calibration compensation */
  vtkSetMacro(CompensationEnabled, bool); 

  /*! Enable/disable stepper calibration compensation */
  vtkGetMacro(CompensationEnabled, bool);

  /*! Enable/disable stepper calibration compensation */
  vtkBooleanMacro(CompensationEnabled, bool); 

  /*! 
  Get brachy stepper type 
  \sa BrachyStepper::BRACHY_STEPPER_TYPE
  */
  BrachyStepper::BRACHY_STEPPER_TYPE GetBrachyStepperType() { return this->BrachyStepperType; }

  /*! Get an update from the tracking system and push the new transforms to the tools. */
  PlusStatus InternalUpdate();

  /*! Read BrachyStepper configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write BrachyStepper configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

protected:
  vtkBrachyTracker();
  ~vtkBrachyTracker();

  std::string GetBrachyToolName(BRACHY_STEPPER_TOOL tool); 

  /*! Set the stepper model version information. */
  vtkSetStringMacro(ModelVersion);

  /*! Set the stepper model number */
  vtkSetStringMacro(ModelNumber);

  /*! Set the stepper serial number */
  vtkSetStringMacro(ModelSerialNumber);

  /*! Start the tracking system.  The tracking system is brought from
  its ground state into full tracking mode.  The device will
  only be reset if communication cannot be established without
  a reset. 
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopRecording();

  /*! Initialize the tracking device */
  PlusStatus InitBrachyTracker();

  BrachyStepper *Device;

  BrachyStepper::BRACHY_STEPPER_TYPE BrachyStepperType; 

  char *ModelVersion;
  char *ModelNumber; 
  char *ModelSerialNumber; 

  unsigned long SerialPort; 
  unsigned long BaudRate;

  //========== Stepper calibration ==================

  /*! Enable/diasable stepper compensation */
  bool CompensationEnabled; 

  /*! Stepper calibration algorithm version */
  char* CalibrationAlgorithmVersion; 

  /*! Stepper calibration data */
  char* CalibrationDate; 

  /*! Probe translation axis orientation vector[Tx, Ty, 1] */
  double ProbeTranslationAxisOrientation[3]; 

  /*! Probe rotation axis orientation vector[Trx, Try, 1] */
  double ProbeRotationAxisOrientation[3]; 

  /*! Template translation axis orientation vector[Tx, Ty, 1] */
  double TemplateTranslationAxisOrientation[3]; 

  /*! Probe rotation encoder scale factor */
  double ProbeRotationEncoderScale; 

private:
  vtkBrachyTracker(const vtkBrachyTracker&);
  void operator=(const vtkBrachyTracker&);  
};

#endif
