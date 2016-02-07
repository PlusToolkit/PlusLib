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
    ScreenFieldParameter()
    {
      this->Origin[0] = 0;
      this->Origin[1] = 0;
      this->Origin[2] = 0;
      this->Size[0] = 0;
      this->Size[1] = 0;
      this->Size[2] = 1;
    }

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

  /// Remove any configuration data
  void ClearConfiguration();

  /// Convert a vtkImage data to leptonica pix format
  void vtkImageDataToPix(TrackedFrame& frame, ScreenFieldParameter* parameter);

  /// If a frame has been queried for this input channel, reuse it instead of getting a new one
  PlusStatus FindOrQueryFrame(TrackedFrame& frame, std::map<double, int>& queriedFramesIndexes, ScreenFieldParameter* parameter, 
    std::vector<TrackedFrame*>& queriedFrames);

  /// Language used for detection
  char* Language;

  /// Main entry point for the tesseract API
  tesseract::TessBaseAPI* TesseractAPI;

  vtkTrackedFrameList* TrackedFrames;

  /// Map of channels to fields so that we only have to grab an image once from the each source channel
  ChannelFieldListMap RecognitionFields;

protected:
  vtkVirtualScreenReader();
  virtual ~vtkVirtualScreenReader();

private:
  vtkVirtualScreenReader(const vtkVirtualScreenReader&);
  void operator=(const vtkVirtualScreenReader&);
};

#endif //__vtkVirtualScreenReader_h