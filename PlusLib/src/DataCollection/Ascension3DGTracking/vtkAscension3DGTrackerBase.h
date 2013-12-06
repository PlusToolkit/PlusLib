/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkPlusDevice.h"

class vtkPlusDataBuffer; 

/*!
\class vtkAscension3DGTrackerBase 
\brief Interface for the Ascension 3DG magnetic tracker 

This class talks with Ascension3DG tracker over the ATC trakSTAR device driver

There is a special tool port ("quality"), which provides the quality values for all the tools.
The quality numbers are stored in a 4x4 matrix as described below. If a value is not available then it is set to -1.
  Port 0: matrix row 0, column 3
  Port 1: matrix row 1, column 3
  Port 2: matrix row 2, column 3
  Port 3: matrix row 0, column 0
  Port 4: matrix row 1, column 1
  Port 5: matrix row 2, column 2

\ingroup PlusLibDataCollection
*/
class
  VTK_EXPORT
vtkAscension3DGTrackerBase : public vtkPlusDevice
{
public:

  static vtkAscension3DGTrackerBase *New();
  vtkTypeMacro( vtkAscension3DGTrackerBase,vtkPlusDevice );
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
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  This method is called by the tracker thread.
  */
  PlusStatus InternalUpdate();

  vtkGetMacro(NumberOfSensors, int);

  vtkGetMacro(FilterAcWideNotch, int);
  vtkGetMacro(FilterAcNarrowNotch, int);
  vtkGetMacro(FilterDcAdaptive, double);
  vtkGetMacro(FilterLargeChange, int);
  vtkGetMacro(FilterAlpha, bool);

protected:
  vtkSetMacro(FilterAcWideNotch, int);
  vtkSetMacro(FilterAcNarrowNotch, int);
  vtkSetMacro(FilterDcAdaptive, double);
  vtkSetMacro(FilterLargeChange, int);
  vtkSetMacro(FilterAlpha, bool);

protected:
  vtkAscension3DGTrackerBase();
  ~vtkAscension3DGTrackerBase();

  /*! 
  Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopRecording();

  vtkPlusBuffer* LocalTrackerBuffer; 

private:  // Definitions.
  enum {TRANSMITTER_OFF = -1};

private:  // Functions.
  vtkAscension3DGTrackerBase( const vtkAscension3DGTrackerBase& );
  void operator=( const vtkAscension3DGTrackerBase& );  

  /*! Check return status and log in case of error */
  PlusStatus CheckReturnStatus( int status );


private:  // Variables.
  std::vector< bool > SensorSaturated;
  std::vector< bool > SensorAttached;
  std::vector< bool > SensorInMotion;

  void* AscensionRecordBuffer; 

  bool TransmitterAttached;

  unsigned int FrameNumber;
  int NumberOfSensors; 

  /*! AC wide notch filter status: enabled (1) or disabled (0) */
  int FilterAcWideNotch;

  /*! AC narrow notch filter status: enabled (1) or disabled (0) */
  int FilterAcNarrowNotch;

  /*! DC adaptive filter parameter (0.0 for no filtering, 1.0 for maximum filtering) */
  double FilterDcAdaptive;

  /*! Large change filter status: enabled (1) or disabled (0) */
  int FilterLargeChange;

  /*! Alpha filter status: enabled (true) or disabled (false) */
  bool FilterAlpha;
};
