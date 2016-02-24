/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _vtkUSDigitalA2EncodersTracker_h_
#define _vtkUSDigitalA2EncodersTracker_h_

#include "vtkDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkTransformRepository.h"

#include <map>

/*!
\class vtkUSDigitalEncodersTracker 
\brief Interface for multiple US Digital A2, A2T, A4, HBA2, HBA4 or HD25A encoders to generate pose information of a target object. 

This class communicates with multiple US Digital encoders through SEI (Serial Encoder Interface Bus) provided by US Digital.

\ingroup PlusLibDataCollection
*/

class vtkDataCollectionExport vtkUSDigitalEncodersTracker : public vtkPlusDevice
{
public:

  static vtkUSDigitalEncodersTracker *New();
  vtkTypeMacro( vtkUSDigitalEncodersTracker,vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual bool IsTracker() const { return true; }

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

  /*! 
  Probe to see if the tracking system is present on the specified serial port.  
  If the SerialPort is set to -1, then all serial ports will be checked.
  */
  PlusStatus Probe();

  /*!
  Get an update from the multiple USDigital encoders and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  This method is called by the tracker thread.
  */
  PlusStatus InternalUpdate();

  vtkGetMacro(NumberOfEncoders, long);  
  vtkSetMacro(NumberOfEncoders, long);  

  /*! Return whether stepper is alive */
  virtual PlusStatus IsStepperAlive();

protected:
  vtkUSDigitalEncodersTracker();
  ~vtkUSDigitalEncodersTracker();

  /*! 
  Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopRecording();

 
private:  // Variables.
  long                                            NumberOfEncoders; // Number of connected A2 Encoders
  long                                            COMPort;
  long                                            BaudRate;
  
  class                                           vtkEncoderTrackingInfo;
  class                                           vtkUSDigitalEncoderInfo;
  vtkSmartPointer<vtkTransformRepository>         USDigitalEncoderTransformRepository;
  //vtkUSDigitalEncoderInfo
  typedef std::map<long, vtkUSDigitalEncoderInfo> EncoderInfoMapType;
  EncoderInfoMapType                              USDigitalEncoderInfoList;
  
  typedef std::vector<vtkEncoderTrackingInfo>     EncoderTrackingInfoVectorType;        
  EncoderTrackingInfoVectorType                   USDigitalEncoderTrackingInfoList;


private:  // Functions.
  vtkUSDigitalEncodersTracker( const vtkUSDigitalEncodersTracker& );
  void operator=( const vtkUSDigitalEncodersTracker& );  
  PlusStatus ToolTimeStampedUpdateWithvtkEncoderTrackingInfo(vtkEncoderTrackingInfo& encoderTrackingInfo);
};

#endif
