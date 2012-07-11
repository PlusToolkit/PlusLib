/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkRfProcessor_h
#define __vtkRfProcessor_h

#include "PlusConfigure.h"
#include "PlusVideoFrame.h"

class vtkRfToBrightnessConvert;
class vtkUsScanConvert;
class vtkImageCast;

/*!
  \class vtkRfProcessor 
  \brief Convenience class to combine multiple algorithms to compute a displayable B-mode frame from RF data
  \ingroup RfProcessingAlgo
*/ 
class VTK_EXPORT vtkRfProcessor : public vtkObject
{
public:
  static vtkRfProcessor *New();
  vtkTypeRevisionMacro(vtkRfProcessor , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  /*! Set the input RF data
    \param rfFrame frame containing RF data 
  */
  virtual PlusStatus SetRfFrame(vtkImageData* rfFrame, US_IMAGE_TYPE imageType); 

  /*! Get the B-mode image after brightness conversion, before scan conversion */
  virtual vtkImageData* GetBrightessConvertedImage(); 

  /*! Get the B-mode image after brightness and scan conversion */
  virtual vtkImageData* GetBrightessScanConvertedImage(); 

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

protected:
  vtkRfProcessor();
  virtual ~vtkRfProcessor(); 

  vtkRfToBrightnessConvert* RfToBrightnessConverter;
  vtkUsScanConvert* ScanConverter;
  vtkImageCast* ImageCaster;
}; 

#endif
