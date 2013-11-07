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

class vtkPolyDataNormals;
class vtkTriangleFilter;
class vtkStripper;
class vtkModifiedBSPTree;
class vtkRfProcessor;
class SpatialModel; 

/*!
  \class vtkUsSimulatorAlgo 
  \brief Class that simulates ultrasound images using an STL model and
         transformations of the intersecting plane as input and
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

  /*! Load stl or vtp model  */ 
  PlusStatus LoadModel(std::string absoluteImagePath);

public:
  // Set model to image transformation matrix
  vtkSetObjectMacro(ModelToImageMatrix, vtkMatrix4x4); 
  // Get model to image transformation matrix
  vtkGetObjectMacro(ModelToImageMatrix, vtkMatrix4x4); 

  /*! Set model file name */
  vtkSetStringMacro(ModelFileName); 
  /*! Get model file name */
  vtkGetStringMacro(ModelFileName);

  /*! Set image frame name */
  vtkSetStringMacro(ImageCoordinateFrame); 
  /*! Get image frame name */
  vtkGetStringMacro(ImageCoordinateFrame);

  /*! Set reference frame name */
  vtkSetStringMacro(ReferenceCoordinateFrame); 
  /*! Get reference frame name */
  vtkGetStringMacro(ReferenceCoordinateFrame);

  /*! Set background value */
  vtkSetMacro(BackgroundValue, int); 
  /*! Get background value */
  vtkGetMacro(BackgroundValue, int);

   /*! Set US frequency*/
  vtkSetMacro(ImagingFrequencyMhz, double); 
  /*! Get US frequency */
  vtkGetMacro(ImagingFrequencyMhz, double);

     /*! Set max intensity in watts per cm squared*/
  vtkSetMacro(IncomingIntensityWattsPerCm2, double); 
  /*! Get max intensity in watts per cm squared */
  vtkGetMacro(IncomingIntensityWattsPerCm2, double);

  /*! Set number of simulated scanlines */
  vtkSetMacro(NumberOfScanlines, int); 

  /*! Set the length of scanlines in pixels */
  vtkSetMacro(NumberOfSamplesPerScanline, int); 

  PlusStatus GetFrameSize(int frameSize[2]);

protected:
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);
  virtual int RequestData(vtkInformation *request,
                        vtkInformationVector** inputVector,
                        vtkInformationVector* outputVector);
  int GetPixelColorFromBeamIntensity(double intensity); 

protected:
  vtkUsSimulatorAlgo();
  ~vtkUsSimulatorAlgo(); 

private:

  /*! Model to image transformation matrix */
  vtkMatrix4x4* ModelToImageMatrix;

  /*! Grayscale value of the background */
  int BackgroundValue;

  /*! Path and name of the STL model file */
  char* ModelFileName;

  /*! Name of the image coordinate frame */
  char* ImageCoordinateFrame;

  /*! Name of the reference coordinate frame */
  char* ReferenceCoordinateFrame;

  /*! Number of scanlines (sensor elements in the transducer) */
  int NumberOfScanlines;

  /*! Number of samples in one scanline */
  int NumberOfSamplesPerScanline;

  /*! Frequency of the ultrasound image to be generated*/
  double ImagingFrequencyMhz; 
  
  /*! incoming Intensity*/
  double IncomingIntensityWattsPerCm2;
  
  
  /*
  struct InputDescriptor
  {
  vtkPolyDataNormals *ModelNormalFilter;
  vtkTriangleFilter *ModelTriangleFilter;
  vtkStripper *ModelStripperFilter;
  vtkModifiedBSPTree *ModelLocalizer;
  MaterialProperties ...;
  }
  std::vector<InputDescriptor> InputDescriptors;
  */
  vtkPolyDataNormals *ModelNormalFilter;
  vtkTriangleFilter *ModelTriangleFilter;
  vtkStripper *ModelStripperFilter;
  vtkModifiedBSPTree *ModelLocalizer;

  vtkRfProcessor *RfProcessor;

  std::vector<double> InsideObjectReflection;
  std::vector<double> OutsideObjectReflection;
  SpatialModel *BackgroundSpatialModel;
  SpatialModel *BoneSpatialModel;

  /*! Color used for CONSTANT_INTENSITY image generation */
  unsigned char InsideObjectColour;
  /*! Color used for CONSTANT_INTENSITY image generation */
  unsigned char OutsideObjectColour;
};

#endif // __vtkUsSimulatorAlgo_h
