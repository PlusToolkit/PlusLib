/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusVirtualTextRecognizer_h
#define __vtkPlusVirtualTextRecognizer_h

#include "PlusConfigure.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDevice.h"

namespace tesseract
{
  class TessBaseAPI;
}
//class vtkIGSIOTrackedFrameList;
typedef struct Pix PIX;

/*!
\class vtkPlusVirtualTextRecognizer
\brief

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusVirtualTextRecognizer : public vtkPlusDevice
{
#ifdef PLUS_TEST_TextRecognizer
public:
#endif
  class TextFieldParameter
  {
  public:
    TextFieldParameter()
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
    std::array<int, 3> Origin;
    /// This is only 3d for simplicity in passing to clipping function, OCR is 2d only
    std::array<int, 3> Size;
  };

public:
  typedef std::vector<TextFieldParameter*> FieldList;
  typedef FieldList::iterator FieldListIterator;
  typedef std::map<vtkPlusChannel*, FieldList > ChannelFieldListMap;
  typedef ChannelFieldListMap::iterator ChannelFieldListMapIterator;

public:
  static vtkPlusVirtualTextRecognizer* New();
  vtkTypeMacro(vtkPlusVirtualTextRecognizer, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

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

  virtual bool IsTracker() const {return false;}
  virtual bool IsVirtual() const {return true;}

  vtkSetStdStringMacro(Language);
  vtkGetStdStringMacro(Language);

  vtkSetObjectMacro(OutputChannel, vtkPlusChannel);
  vtkGetObjectMacro(OutputChannel, vtkPlusChannel);

#ifdef PLUS_TEST_TextRecognizer
  ChannelFieldListMap& GetRecognitionFields();
#endif

protected:
  virtual PlusStatus InternalUpdate();

  /// Remove any configuration data
  void ClearConfiguration();

  /// Convert a vtkImage data to leptonica pix format
  void vtkImageDataToPix(igsioTrackedFrame& frame, TextFieldParameter* parameter);

  /// If a frame has been queried for this input channel, reuse it instead of getting a new one
  PlusStatus FindOrQueryFrame(igsioTrackedFrame& frame, std::map<double, int>& queriedFramesIndexes, TextFieldParameter* parameter,
                              std::vector<igsioTrackedFrame*>& queriedFrames);

  /// Language used for detection
  std::string                 Language;

  /// Main entry point for the tesseract API
  tesseract::TessBaseAPI*     TesseractAPI;

  vtkIGSIOTrackedFrameList*    TrackedFrames;

  /// Map of channels to fields so that we only have to grab an image once from the each source channel
  ChannelFieldListMap         RecognitionFields;

  /// Optional output channel to store recognized fields for broadcasting
  vtkPlusChannel*             OutputChannel;

protected:
  vtkPlusVirtualTextRecognizer();
  virtual ~vtkPlusVirtualTextRecognizer();

private:
  vtkPlusVirtualTextRecognizer(const vtkPlusVirtualTextRecognizer&);
  void operator=(const vtkPlusVirtualTextRecognizer&);
};

#endif //__vtkPlusVirtualTextRecognizer_h
