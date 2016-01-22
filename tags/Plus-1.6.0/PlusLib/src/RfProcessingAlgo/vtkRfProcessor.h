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
class vtkUsScanConvertLinear;
class vtkUsScanConvertCurvilinear;
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

  enum TransducerGeometryType
  {
    TRANSDUCER_UNKNOWN,
    TRANSDUCER_LINEAR,
    TRANSDUCER_CURVILINEAR
  };


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

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

  /*! Set the transducer geometry (linear or curvilinear) */
  vtkSetMacro(TransducerGeometry, TransducerGeometryType); 

  /*! Set the transducer name */
  vtkSetStringMacro(TransducerName); 

  /*! Initialize the corresponding values in the converter classes. For all transducer types. */
  void SetStartDepthMm(double); 

  /*! Initialize the corresponding values in the converter classes. For all transducer types. */
  void SetStopDepthMm(double); 

  /*! Initialize the corresponding values in the converter classes. For curvilinear transducer type only. */
  void SetStartAngleDeg(double); 

  /*! Initialize the corresponding values in the converter classes. For curvilinear transducer type only. */
  void SetStopAngleDeg(double); 

  /*! Initialize the corresponding values in the converter classes. For curvilinear transducer type only. */
  void SetRadiusOfCurvatureMm(double);

  /*! Initialize the corresponding values in the converter classes. For linear transducer type only. */
  void SetTransducerWidthMm(double);

protected:
  vtkRfProcessor();
  virtual ~vtkRfProcessor(); 

  vtkRfToBrightnessConvert* RfToBrightnessConverter;
  vtkUsScanConvertLinear* ScanConverterLinear;  
  vtkUsScanConvertCurvilinear* ScanConverterCurvilinear;  
  vtkImageCast* ImageCaster;

  /*! Transducer name */
  char* TransducerName;
  /*! Type of the transducer (linear or curvilinear) */
  TransducerGeometryType TransducerGeometry;
}; 

#endif