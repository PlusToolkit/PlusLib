/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusPicoScopeDataSource_h
#define __vtkPlusPicoScopeDataSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
\class vtkPlusPicoScopeDataSource
\brief Interface to the Atracsys trackers
This class talks with a Atracsys Tracker over the sTk Passive Tracking SDK.
Requires PLUS_USE_ATRACSYS option in CMake.
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusPicoScopeDataSource : public vtkPlusDevice
{
public:
  static vtkPlusPicoScopeDataSource *New();
  vtkTypeMacro(vtkPlusPicoScopeDataSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /* Device is a hardware oscilloscope. */
  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();

  /*!  */
  PlusStatus InternalUpdate();

  vtkGetMacro(NumberOfSamples, int);
  vtkSetMacro(NumberOfSamples, int);

protected:
  vtkPlusPicoScopeDataSource();
  ~vtkPlusPicoScopeDataSource();

private: // Functions
  vtkPlusPicoScopeDataSource(const vtkPlusPicoScopeDataSource&);
  void operator=(const vtkPlusPicoScopeDataSource&);

  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  class vtkInternal;
  vtkInternal* Internal;

	int NumberOfSamples;

};

#endif
