/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkRfProcessor_h
#define __vtkRfProcessor_h

#include "vtkImageProcessingAlgoExport.h"
#include "PlusVideoFrame.h" // for US_IMAGE_TYPE

class vtkRfToBrightnessConvert;
class vtkUsScanConvert;
class vtkImageData;

/*!
  \class vtkRfProcessor 
  \brief Convenience class to combine multiple algorithms to compute a displayable B-mode frame from RF data
  \ingroup PlusLibImageProcessingAlgo
*/ 
class vtkImageProcessingAlgoExport vtkRfProcessor : public vtkObject
{
public:
  static vtkRfProcessor *New();
  vtkTypeMacro(vtkRfProcessor , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  /*! Set the input RF data
    \param rfFrame frame containing RF data (may be B-mode data but always without scan conversion)
  */
  virtual PlusStatus SetRfFrame(vtkImageData* rfFrame, US_IMAGE_TYPE imageType); 

  /*! Get the B-mode image after brightness conversion, before scan conversion */
  virtual vtkImageData* GetBrightnessConvertedImage(); 

  /*! Get the B-mode image after brightness and scan conversion */
  virtual vtkImageData* GetBrightnessScanConvertedImage(); 

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rfElement); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rfElement); 
  
  /*! Set the scan converter object. The class will keep a reference to this object. */
  virtual void SetScanConverter(vtkUsScanConvert* scanConverter);

  /*! Get the scan converter object */
  vtkGetMacro(ScanConverter, vtkUsScanConvert*);

  /*! Get the rf to brightness converter object */
  vtkGetMacro(RfToBrightnessConverter, vtkRfToBrightnessConvert*);

  static const char* GetRfProcessorTagName();

protected:
  vtkRfProcessor();
  virtual ~vtkRfProcessor(); 

  vtkRfToBrightnessConvert* RfToBrightnessConverter;

  vtkUsScanConvert* ScanConverter;  
  std::vector<vtkUsScanConvert*> AvailableScanConverters;  

  static const char* RF_PROCESSOR_TAG_NAME;
}; 

#endif
