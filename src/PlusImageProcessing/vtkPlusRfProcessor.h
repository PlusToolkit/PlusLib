/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusRfProcessor_h
#define __vtkPlusRfProcessor_h

#include "vtkPlusImageProcessingExport.h"
//#include "igsioVideoFrame.h" // for US_IMAGE_TYPE

class vtkPlusRfToBrightnessConvert;
class vtkPlusUsScanConvert;
class vtkImageData;

/*!
  \class vtkPlusRfProcessor 
  \brief Convenience class to combine multiple algorithms to compute a displayable B-mode frame from RF data
  \ingroup PlusLibImageProcessingAlgo
*/ 
class vtkPlusImageProcessingExport vtkPlusRfProcessor : public vtkObject
{
public:
  static vtkPlusRfProcessor *New();
  vtkTypeMacro(vtkPlusRfProcessor , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

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
  virtual void SetScanConverter(vtkPlusUsScanConvert* scanConverter);

  /*! Get the scan converter object */
  vtkGetMacro(ScanConverter, vtkPlusUsScanConvert*);

  /*! Get the rf to brightness converter object */
  vtkGetMacro(RfToBrightnessConverter, vtkPlusRfToBrightnessConvert*);

  static const char* GetRfProcessorTagName();

protected:
  vtkPlusRfProcessor();
  virtual ~vtkPlusRfProcessor(); 

  vtkPlusRfToBrightnessConvert* RfToBrightnessConverter;

  vtkPlusUsScanConvert* ScanConverter;  
  std::vector<vtkPlusUsScanConvert*> AvailableScanConverters;  

  static const char* RF_PROCESSOR_TAG_NAME;
}; 

#endif
