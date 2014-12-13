/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPhilips3DProbeVideoSource_h
#define __vtkPhilips3DProbeVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
\class vtkPhilips3DProbeVideoSource
\brief Class for providing VTK video input interface from Philips ie33 3D ultrasound probe
\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkPhilips3DProbeVideoSource : public vtkPlusDevice
{
public:
  vtkTypeMacro(vtkPhilips3DProbeVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   
  static vtkPhilips3DProbeVideoSource* New();

  /*! Read configuration from XML data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to XML data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Respond to the query if this is a tracker or not */
  virtual bool IsTracker() const;

  /*! Perform any completion tasks once configured */
  virtual PlusStatus NotifyConfigured();

protected:
  /*! Constructor */
  vtkPhilips3DProbeVideoSource();
  /*! Destructor */
  virtual ~vtkPhilips3DProbeVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! The internal function which actually does the grab.  */
  PlusStatus InternalUpdate();

private:
  static vtkPhilips3DProbeVideoSource* Instance;
  vtkPhilips3DProbeVideoSource(const vtkPhilips3DProbeVideoSource&);  // Not implemented.
  void operator=(const vtkPhilips3DProbeVideoSource&);  // Not implemented.
};

#endif

