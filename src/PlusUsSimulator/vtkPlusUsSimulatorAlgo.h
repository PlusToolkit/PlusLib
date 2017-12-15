/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsSimulatorAlgo_h
#define __vtkPlusUsSimulatorAlgo_h

#include "vtkPlusUsSimulatorExport.h"

#include "vtkImageAlgorithm.h"

#include "PlusSpatialModel.h"
#include "vtkPlusTransformRepository.h"

class vtkPolyDataNormals;
class vtkTriangleFilter;
class vtkStripper;
class vtkModifiedBSPTree;
class vtkPlusRfProcessor;

/*!
  \class vtkPlusUsSimulatorAlgo
  \brief Class that simulates ultrasound images from multiple surface models
  \ingroup PlusLibUsSimulatorAlgo
*/
class vtkPlusUsSimulatorExport vtkPlusUsSimulatorAlgo : public vtkImageAlgorithm
{
public:
  vtkTypeMacro( vtkPlusUsSimulatorAlgo, vtkImageAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );
  static vtkPlusUsSimulatorAlgo* New();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );

public:

  /*! Set transform repository */
  vtkSetObjectMacro( TransformRepository, vtkPlusTransformRepository );
  /*! Get transform repository */
  vtkGetObjectMacro( TransformRepository, vtkPlusTransformRepository );

  /*! Get RF processor */
  vtkGetObjectMacro( RfProcessor, vtkPlusRfProcessor );

  /*! Set image frame name */
  vtkSetStringMacro( ImageCoordinateFrame );
  /*! Get image frame name */
  vtkGetStringMacro( ImageCoordinateFrame );

  /*! Set reference frame name */
  vtkSetStringMacro( ReferenceCoordinateFrame );
  /*! Get reference frame name */
  vtkGetStringMacro( ReferenceCoordinateFrame );

  /*! Set US imaging frequency*/
  vtkSetMacro( FrequencyMhz, double );
  /*! Get US imaging frequency */
  vtkGetMacro( FrequencyMhz, double );

  /*! Set max intensity in mW/cm2 */
  vtkSetMacro( IncomingIntensityMwPerCm2, double );
  /*! Get max intensity in mW/cm2 */
  vtkGetMacro( IncomingIntensityMwPerCm2, double );

  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  vtkGetMacro( BrightnessConversionGamma, double );
  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  vtkSetMacro( BrightnessConversionGamma, double );

  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  vtkGetMacro( BrightnessConversionOffset, double );
  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  vtkSetMacro( BrightnessConversionOffset, double );

  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  vtkGetMacro( BrightnessConversionScale, double );
  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  vtkSetMacro( BrightnessConversionScale, double );

  /*! Set number of simulated scanlines */
  vtkSetMacro( NumberOfScanlines, int );

  /*! Set the length of scanlines in pixels */
  vtkSetMacro( NumberOfSamplesPerScanline, int );

  PlusStatus GetFrameSize( int frameSize[3] );

  vtkSetMacro( NoiseAmplitude, double );
  vtkSetVector3Macro( NoiseFrequency, double );
  vtkSetVector3Macro( NoisePhase, double );

protected:
  virtual int FillOutputPortInformation( int port, vtkInformation* info );
  virtual int RequestData( vtkInformation* request,
                           vtkInformationVector** inputVector,
                           vtkInformationVector* outputVector );

  void ConvertLineModelIntersectionsToSegmentDescriptor( std::deque<PlusSpatialModel::LineIntersectionInfo>& lineIntersectionsWithModels );

protected:
  vtkPlusUsSimulatorAlgo();
  ~vtkPlusUsSimulatorAlgo();

private:

  /*! Transform repository instance */
  vtkPlusTransformRepository* TransformRepository;

  /*! Name of the image coordinate frame */
  char* ImageCoordinateFrame;

  /*! Name of the reference coordinate frame */
  char* ReferenceCoordinateFrame;

  /*! Number of scanlines (sensor elements in the transducer) */
  int NumberOfScanlines;

  /*! Number of samples in one scanline */
  int NumberOfSamplesPerScanline;

  vtkPlusRfProcessor* RfProcessor;

  /*! Frequency of the ultrasound image to be generated*/
  double FrequencyMhz;

  /*! A non-linear function is used for mapping intensities to pixel values:
      pixel = BrightnessConversionOffset + BrightnessConversionScale * intensity ^ BrightnessConversionGamma
  */
  double BrightnessConversionGamma;

  /*! A non-linear function is used for mapping intensities to pixel values. See also BrightnessConversionGamma. */
  double BrightnessConversionOffset;

  /*! A non-linear function is used for mapping intensities to pixel values. See also BrightnessConversionGamma. */
  double BrightnessConversionScale;

  /*! Incoming Intensity in mW/cm2 */
  double IncomingIntensityMwPerCm2;

  std::vector<PlusSpatialModel> SpatialModels;

  /*! Contains material properties of the transducer. Used for computing the sound reflection from the transducer surface. */
  PlusSpatialModel TransducerSpatialModel;

  std::vector<double> InsideObjectReflection;
  std::vector<double> OutsideObjectReflection;

  double NoiseAmplitude;
  double NoiseFrequency[3];
  double NoisePhase[3];
};

#endif // __vtkPlusUsSimulatorAlgo_h
