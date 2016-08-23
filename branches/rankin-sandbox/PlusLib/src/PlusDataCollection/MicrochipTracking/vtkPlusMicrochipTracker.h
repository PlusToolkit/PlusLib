/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusMicrochipTracker_h
#define __vtkPlusMicrochipTracker_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusGenericSerialDevice.h"

/*!
\class vtkPlusMicrochipTracker 
\brief Interface for acquiring data from a Microchip MM7150 motion module

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusMicrochipTracker : public vtkPlusGenericSerialDevice
{
public:

  static vtkPlusMicrochipTracker *New();
  vtkTypeMacro( vtkPlusMicrochipTracker,vtkPlusGenericSerialDevice );

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

  virtual bool IsTracker() const { return true; }

protected:

  /*! Retrieves orientation from the text message received from the sensor */
  PlusStatus ParseMessage(std::string& textReceived, double* rotationQuat);

  vtkPlusMicrochipTracker();
  ~vtkPlusMicrochipTracker();

private:  // Functions.

  vtkPlusMicrochipTracker( const vtkPlusMicrochipTracker& );
  void operator=( const vtkPlusMicrochipTracker& );

  vtkPlusDataSource* OrientationSensorTool;
  // Store the last transform so that we can put the last value in the buffer if there is no update received from the sensor
  vtkMatrix4x4* OrientationSensorToTracker;
  
private:  // Variables.

};

#endif
