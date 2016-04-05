/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkThorLabsVideoSource_h
#define __vtkThorLabsVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "PlusVideoFrame.h"

class vtkThorLabsVideoSourceInternal;

/*!
  \class vtkThorLabsVideoSource 
  \brief ThorLabs compact spectrometer

  \ingroup PlusLibDataCollection
*/ 
class vtkDataCollectionExport vtkThorLabsVideoSource : public vtkPlusDevice
{
public:
  static vtkThorLabsVideoSource *New();
  vtkTypeMacro(vtkThorLabsVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

  /*!
    Set the name to identify an instrument to connect to.
    Necessary if there are multiple instruments available.
  */
  vtkSetStringMacro(InstrumentName); 
  /*! Get the instrument name */
  vtkGetStringMacro(InstrumentName); 

protected:

  /*! Constructor */
  vtkThorLabsVideoSource();
  /*! Destructor */
  ~vtkThorLabsVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*!
    The internal function which actually grabs one frame.
    It just requests a single frame from the hardware and the object
    will be notified when it is ready.
  */
  virtual PlusStatus InternalUpdate();

  int FrameIndex;

  double IntegrationTimeSec;
  char* InstrumentName;

  vtkThorLabsVideoSourceInternal *Internal;

  /*! Update the buffer format to match the capture settings */
  PlusStatus UpdateFrameBuffer();

private:
  vtkThorLabsVideoSource(const vtkThorLabsVideoSource&);  // Not implemented.
  void operator=(const vtkThorLabsVideoSource&);  // Not implemented.
};

#endif
