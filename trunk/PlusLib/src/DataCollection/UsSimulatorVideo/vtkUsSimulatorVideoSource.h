/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorVideoSource_h
#define __vtkUsSimulatorVideoSource_h

#include "vtkPlusDevice.h"
#include "vtkUsSimulatorAlgo.h"

class vtkPlusDataBuffer; 

class VTK_EXPORT vtkUsSimulatorVideoSource;

/*!
  \class vtkUsSimulatorVideoSource 
  \brief Class for providing VTK video input interface from simulated ultrasound
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkUsSimulatorVideoSource : public vtkPlusDevice
{
public:
  vtkTypeRevisionMacro(vtkUsSimulatorVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   
  static vtkUsSimulatorVideoSource* New();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Get ultrasound simulator */
  vtkGetObjectMacro(UsSimulator, vtkUsSimulatorAlgo); 

  /*! Set ultrasound simulator */
  vtkSetObjectMacro(Tracker, vtkPlusDevice); 
  /*! Get ultrasound simulator */
  vtkGetObjectMacro(Tracker, vtkPlusDevice); 

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

protected:
  /*! Set ultrasound simulator */
  vtkSetObjectMacro(UsSimulator, vtkUsSimulatorAlgo); 

protected:
  /*! Constructor */
  vtkUsSimulatorVideoSource();
  /*! Destructor */
  virtual ~vtkUsSimulatorVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! The internal function which actually does the grab.  */
  virtual PlusStatus InternalUpdate();

protected:
  /*! Ultrasound simulator */
  vtkUsSimulatorAlgo* UsSimulator;

  /*! Tracker used in the simulator */
  vtkPlusDevice* Tracker;

  /* Timestamp of the last tracking item that has been processed already */
  double LastProcessedTrackingDataTimestamp;

private:
  static vtkUsSimulatorVideoSource* Instance;
  vtkUsSimulatorVideoSource(const vtkUsSimulatorVideoSource&);  // Not implemented.
  void operator=(const vtkUsSimulatorVideoSource&);  // Not implemented.
};

#endif
