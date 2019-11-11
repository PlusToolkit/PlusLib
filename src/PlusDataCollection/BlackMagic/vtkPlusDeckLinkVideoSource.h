/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDeckLinkVideoSource_h
#define __vtkPlusDeckLinkVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// DeckLink includes
#if WIN32
  // Windows includes
  #include <comutil.h>
#endif
#include <DeckLinkAPI.h>

// STL includes
#include <atomic>

/*!
\class vtkPlusDeckLinkVideoSource
\brief Interface to a BlackMagic DeckLink capture card
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusDeckLinkVideoSource : public vtkPlusDevice, public IDeckLinkInputCallback
{
public:
  static vtkPlusDeckLinkVideoSource* New();
  vtkTypeMacro(vtkPlusDeckLinkVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /* Device is a hardware tracker. */
  virtual bool IsTracker() const;
  virtual bool IsVirtual() const;

  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);
  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus InternalStartRecording();
  virtual PlusStatus InternalStopRecording();
  virtual PlusStatus Probe();
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusDeckLinkVideoSource();
  ~vtkPlusDeckLinkVideoSource();

protected:
  // IDeckLinkInputCallback interface
  virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode* newDisplayMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags);
  virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioPacket);

  // IUnknown interface
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv);
  virtual ULONG STDMETHODCALLTYPE AddRef();
  virtual ULONG STDMETHODCALLTYPE Release();

protected:
  std::atomic<ULONG> ReferenceCount;

private:
  vtkPlusDeckLinkVideoSource(const vtkPlusDeckLinkVideoSource&); // Not implemented
  void operator=(const vtkPlusDeckLinkVideoSource&); // Not implemented

  class vtkInternal;
  vtkInternal* Internal;

};

#endif
