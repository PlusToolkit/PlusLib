/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkMicrochipTracker_h
#define __vtkMicrochipTracker_h

#include "vtkDataCollectionExport.h"

#include "vtkGenericSerialDevice.h"

/*!
\class vtkMicrochipTracker 
\brief Interface for acquiring data from a Microchip MM7150 motion module

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkMicrochipTracker : public vtkGenericSerialDevice
{
public:

  static vtkMicrochipTracker *New();
  vtkTypeMacro( vtkMicrochipTracker,vtkGenericSerialDevice );

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

  vtkMicrochipTracker();
  ~vtkMicrochipTracker();

private:  // Functions.

  vtkMicrochipTracker( const vtkMicrochipTracker& );
  void operator=( const vtkMicrochipTracker& );

  vtkPlusDataSource* OrientationSensorTool;
  // Store the last transform so that we can put the last value in the buffer if there is no update received from the sensor
  vtkMatrix4x4* OrientationSensorToTracker;
  
private:  // Variables.

};

#endif
