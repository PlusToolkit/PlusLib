/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusLeapMotion_h
#define __vtkPlusLeapMotion_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
\class vtkPlusLeapMotion
\brief Interface for the LeapMotion hand tracker

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkPlusLeapMotion : public vtkPlusDevice
{
public:
  static vtkPlusLeapMotion* New();
  vtkTypeMacro(vtkPlusLeapMotion, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool IsTracker() const { return true; }

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

protected:
  vtkPlusLeapMotion();
  ~vtkPlusLeapMotion();

  PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus InternalUpdate();
  PlusStatus InternalStartRecording();
  PlusStatus InternalStopRecording();

protected:


private:
  vtkPlusLeapMotion(const vtkPlusLeapMotion&);
  void operator=(const vtkPlusLeapMotion&);
};

#endif
