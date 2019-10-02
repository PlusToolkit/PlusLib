/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusVirtualDeinterlacer_h
#define __vtkPlusVirtualDeinterlacer_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

// STL includes
#include <memory>

class igsioVideoFrame;
class vtkIGSIOTrackedFrameList;
class vtkImageData;
class vtkPlusChannel;
class vtkPlusDataSource;

/*!
\class vtkPlusVirtualDeinterlacer
\brief

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusVirtualDeinterlacer : public vtkPlusDevice
{
public:
  enum StereoMode
  {
    Stereo_Unknown,
    Stereo_HorizontalInterlace,
    Stereo_VerticalInterlace
  };

  static vtkPlusVirtualDeinterlacer* New();
  vtkTypeMacro(vtkPlusVirtualDeinterlacer, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Answer if device is a tracker */
  virtual bool IsTracker() const { return false; }

  virtual bool IsVirtual() const { return true; }

  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  virtual PlusStatus NotifyConfigured();

  virtual PlusStatus InternalUpdate();

  virtual double GetAcquisitionRate() const;

  vtkGetMacro(SwitchInterlaceOrdering, bool);
  vtkSetMacro(SwitchInterlaceOrdering, bool);

protected:
  void SplitFrameHorizontal(igsioTrackedFrame* frame);
  void SplitFrameVertical(igsioTrackedFrame* frame);

protected:
  vtkPlusVirtualDeinterlacer();
  virtual ~vtkPlusVirtualDeinterlacer();

  StereoMode                                Mode;
  bool                                      Initialized;
  bool                                      SwitchInterlaceOrdering;
  double                                    LastInputTimestamp;
  vtkPlusDataSource*                        InputSource;
  vtkPlusDataSource*                        LeftSource;
  vtkPlusDataSource*                        RightSource;
  vtkImageData*                             LeftImage;
  vtkImageData*                             RightImage;
  vtkIGSIOTrackedFrameList*                 FrameList;

private:
  vtkPlusVirtualDeinterlacer(const vtkPlusVirtualDeinterlacer&);  // Not implemented.
  void operator=(const vtkPlusVirtualDeinterlacer&);  // Not implemented.
};

#endif
