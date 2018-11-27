/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusThorLabsVideoSource_h
#define __vtkPlusThorLabsVideoSource_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
//#include "igsioVideoFrame.h"

class vtkPlusThorLabsVideoSourceInternal;

/*!
  \class vtkPlusThorLabsVideoSource 
  \brief ThorLabs compact spectrometer

  \ingroup PlusLibDataCollection
*/ 
class vtkPlusDataCollectionExport vtkPlusThorLabsVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusThorLabsVideoSource *New();
  vtkTypeMacro(vtkPlusThorLabsVideoSource,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

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
  vtkPlusThorLabsVideoSource();
  /*! Destructor */
  ~vtkPlusThorLabsVideoSource();

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

  vtkPlusThorLabsVideoSourceInternal *Internal;

  /*! Update the buffer format to match the capture settings */
  PlusStatus UpdateFrameBuffer();

private:
  vtkPlusThorLabsVideoSource(const vtkPlusThorLabsVideoSource&);  // Not implemented.
  void operator=(const vtkPlusThorLabsVideoSource&);  // Not implemented.
};

#endif
