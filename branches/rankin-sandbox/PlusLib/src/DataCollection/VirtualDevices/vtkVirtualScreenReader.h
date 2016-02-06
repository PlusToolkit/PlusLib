/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualScreenReader_h
#define __vtkVirtualScreenReader_h

#include "vtkDataCollectionExport.h"
#include "vtkPlusDevice.h"

namespace tesseract
{
  class TessBaseAPI;
}
class vtkPlusChannel;
class vtkTrackedFrameList;
typedef struct Pix PIX;

/*!
\class vtkVirtualScreenReader
\brief 

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkVirtualScreenReader : public vtkPlusDevice
{
  class ScreenFieldParameter
  {
  public:
    std::string LatestParameterValue;
    PIX* ReceivedFrame;
    vtkSmartPointer<vtkImageData> ScreenRegion;
    vtkPlusChannel* SourceChannel;
    std::string ParameterName;
    /// This is only 3d for simplicity in passing to clipping function, OCR is 2d only
    int Origin[3];
    /// This is only 3d for simplicity in passing to clipping function, OCR is 2d only
    int Size[3];
  };

  typedef std::vector<ScreenFieldParameter*> FieldList;
  typedef FieldList::iterator FieldListIterator;
  typedef std::map<vtkPlusChannel*, FieldList > ChannelFieldListMap;
  typedef ChannelFieldListMap::iterator ChannelFieldListMapIterator;

public:
  static vtkVirtualScreenReader *New();
  vtkTypeMacro(vtkVirtualScreenReader, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Prepare the device for recognition */
  virtual PlusStatus InternalConnect();

  /*! Prepare the device for recognition */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write main configuration from xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  /*! Callback after configuration of all devices is complete */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

  vtkSetStringMacro(Language);
  vtkGetStringMacro(Language);

protected:
  virtual PlusStatus InternalUpdate();

  int vtkImageDataToPix(TrackedFrame* frame, ScreenFieldParameter* parameter);

  vtkSmartPointer<TrackedFrame> FindOrQueryFrame(std::map<double, int>& queriedFramesIndexes, ScreenFieldParameter* parameter, 
    std::vector<vtkSmartPointer<TrackedFrame> >& queriedFrames);

  vtkVirtualScreenReader();
  virtual ~vtkVirtualScreenReader();

  /// Language used for detection
  char* Language;

  /// Main entry point for the tesseract API
  tesseract::TessBaseAPI* TesseractAPI;

  vtkTrackedFrameList* TrackedFrames;

  /// Map of channels to fields so that we only have to grab an image once from the each source channel
  ChannelFieldListMap RecognitionFields;

private:
  vtkVirtualScreenReader(const vtkVirtualScreenReader&);
  void operator=(const vtkVirtualScreenReader&);
};

#endif //__vtkVirtualScreenReader_h