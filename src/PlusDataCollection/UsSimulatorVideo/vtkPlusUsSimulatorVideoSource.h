/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsSimulatorVideoSource_h
#define __vtkPlusUsSimulatorVideoSource_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusUsDevice.h"
#include "vtkPlusUsSimulatorAlgo.h"

class vtkPlusDataBuffer; 

class vtkPlusDataCollectionExport vtkPlusUsSimulatorVideoSource;

/*!
  \class vtkPlusUsSimulatorVideoSource 
  \brief Class for providing VTK video input interface from simulated ultrasound
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusUsSimulatorVideoSource : public vtkPlusUsDevice
{
public:
  static vtkPlusUsSimulatorVideoSource* New();
  vtkTypeMacro(vtkPlusUsSimulatorVideoSource, vtkPlusUsDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Get ultrasound simulator */
  vtkGetObjectMacro(UsSimulator, vtkPlusUsSimulatorAlgo); 

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

protected:
  /*! Set ultrasound simulator */
  vtkSetObjectMacro(UsSimulator, vtkPlusUsSimulatorAlgo); 

protected:
  /*! Constructor */
  vtkPlusUsSimulatorVideoSource();
  /*! Destructor */
  virtual ~vtkPlusUsSimulatorVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! The internal function which actually does the grab.  */
  virtual PlusStatus InternalUpdate();

protected:
  /*! Ultrasound simulator */
  vtkPlusUsSimulatorAlgo* UsSimulator;

  /* Timestamp of the last tracking item that has been processed already */
  double LastProcessedTrackingDataTimestamp;

  /* Output to different logs depending on the status of the grace period */
  vtkPlusLogger::LogLevelType GracePeriodLogLevel;

private:
  static vtkPlusUsSimulatorVideoSource* Instance;
  vtkPlusUsSimulatorVideoSource(const vtkPlusUsSimulatorVideoSource&);  // Not implemented.
  void operator=(const vtkPlusUsSimulatorVideoSource&);  // Not implemented.
};

#endif
