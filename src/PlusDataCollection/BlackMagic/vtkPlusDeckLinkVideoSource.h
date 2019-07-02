/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDeckLinkVideoSource_h
#define __vtkPlusDeckLinkVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

#if _WIN32
  #include <combaseapi.h>
#endif

#define _stringify(x) #x
#define STRINGIFY(x) _stringify(x)
#include STRINGIFY(DeckLinkSDK_INCLUDE_FILE)
#undef STRINGIFY
#undef _stringify

/*!
\class vtkPlusDeckLinkVideoSource
\brief Interface to a BlackMagic DeckLink
capture card
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusDeckLinkVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusDeckLinkVideoSource* New();
  vtkTypeMacro(vtkPlusDeckLinkVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /* Device is a hardware tracker. */
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

  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();

  /*! Perform update */
  PlusStatus InternalUpdate();

protected:
  vtkPlusDeckLinkVideoSource();
  ~vtkPlusDeckLinkVideoSource();

private:
  vtkPlusDeckLinkVideoSource(const vtkPlusDeckLinkVideoSource&); // Not implemented
  void operator=(const vtkPlusDeckLinkVideoSource&); // Not implemented

  class vtkInternal;
  vtkInternal* Internal;

};

#endif
