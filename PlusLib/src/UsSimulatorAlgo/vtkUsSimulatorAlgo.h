/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorAlgo_h
#define __vtkUsSimulatorAlgo_h

#include "vtkImageAlgorithm.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

#include "SpatialModel.h"
#include "vtkTransformRepository.h"

class vtkPolyDataNormals;
class vtkTriangleFilter;
class vtkStripper;
class vtkModifiedBSPTree;
class vtkRfProcessor;

/*!
  \class vtkUsSimulatorAlgo 
  \brief Class that simulates ultrasound images from multiple surface models
  \ingroup PlusLibUsSimulatorAlgo
*/
class VTK_EXPORT vtkUsSimulatorAlgo : public vtkImageAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkUsSimulatorAlgo,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkUsSimulatorAlgo *New();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

public:

  /*! Set transform repository */
  vtkSetObjectMacro(TransformRepository, vtkTransformRepository); 
  /*! Get transform repository */
  vtkGetObjectMacro(TransformRepository, vtkTransformRepository); 

  /*! Set image frame name */
  vtkSetStringMacro(ImageCoordinateFrame); 
  /*! Get image frame name */
  vtkGetStringMacro(ImageCoordinateFrame);

  /*! Set reference frame name */
  vtkSetStringMacro(ReferenceCoordinateFrame); 
  /*! Get reference frame name */
  vtkGetStringMacro(ReferenceCoordinateFrame);

   /*! Set US frequency*/
  vtkSetMacro(ImagingFrequencyMhz, double); 
  /*! Get US frequency */
  vtkGetMacro(ImagingFrequencyMhz, double);

  /*! Set max intensity in mW/cm2 */
  vtkSetMacro(IncomingIntensityMwPerCm2, double); 
  /*! Get max intensity in mW/cm2 */
  vtkGetMacro(IncomingIntensityMwPerCm2, double);

  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  vtkGetMacro(BrightnessConversionGamma, double);
  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  vtkSetMacro(BrightnessConversionGamma, double);

  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  vtkGetMacro(BrightnessConversionOffset, double);
  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  vtkSetMacro(BrightnessConversionOffset, double);

  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  vtkGetMacro(BrightnessConversionScale, double);
  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  vtkSetMacro(BrightnessConversionScale, double);

  /*! Set number of simulated scanlines */
  vtkSetMacro(NumberOfScanlines, int); 

  /*! Set the length of scanlines in pixels */
  vtkSetMacro(NumberOfSamplesPerScanline, int); 

  PlusStatus GetFrameSize(int frameSize[2]);

protected:
  virtual int FillOutputPortInformation(int port, vtkInformation* info);
  virtual int RequestData(vtkInformation *request,
                        vtkInformationVector** inputVector,
                        vtkInformationVector* outputVector);

  void ConvertLineModelIntersectionsToSegmentDescriptor(std::deque<SpatialModel::LineIntersectionInfo> &lineIntersectionsWithModels);

protected:
  vtkUsSimulatorAlgo();
  ~vtkUsSimulatorAlgo(); 

private:

  /*! Transform repository instance */ 
  vtkTransformRepository* TransformRepository;

  /*! Name of the image coordinate frame */
  char* ImageCoordinateFrame;

  /*! Name of the reference coordinate frame */
  char* ReferenceCoordinateFrame;

  /*! Number of scanlines (sensor elements in the transducer) */
  int NumberOfScanlines;

  /*! Number of samples in one scanline */
  int NumberOfSamplesPerScanline;

  vtkRfProcessor *RfProcessor;

  /*! Frequency of the ultrasound image to be generated*/
  double ImagingFrequencyMhz; 

  /*! A non-linear function is used for mapping intensities to pixel values: pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma */
  double BrightnessConversionGamma;

  /*! A non-linear function is used for mapping intensities to pixel values. See also BrightnessConversionGamma. */
  double BrightnessConversionOffset;

  /*! A non-linear function is used for mapping intensities to pixel values. See also BrightnessConversionGamma. */
  double BrightnessConversionScale;
  
  /*! Incoming Intensity in mW/cm2 */
  double IncomingIntensityMwPerCm2;  
  
  std::vector<SpatialModel> SpatialModels;

  std::vector<double> InsideObjectReflection;
  std::vector<double> OutsideObjectReflection;

  double NoiseAmplitude;
  double NoiseFrequency[3];
  double NoisePhase[3];
};

#endif // __vtkUsSimulatorAlgo_h
