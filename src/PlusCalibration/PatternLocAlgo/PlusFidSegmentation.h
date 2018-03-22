/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_SEGMENTATION_H
#define _FIDUCIAL_SEGMENTATION_H

#include "PlusFidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
#include "vtkXMLDataElement.h"
#include <string.h>

//-----------------------------------------------------------------------------

/*!
  \struct Coordinate2D
  \brief Structure holding a coordinate of a structuring element
  \ingroup PlusLibPatternRecognition
*/
class vtkPlusCalibrationExport PlusCoordinate2D
{
public:
  PlusCoordinate2D() { Y = 0; X = 0; };
  PlusCoordinate2D(int y, int x) { Y = y; X = x; };
  bool operator== (const PlusCoordinate2D& data) const { return (Y == data.Y && X == data.X) ; }

public:
  int Y;
  int X;
};

//-----------------------------------------------------------------------------

/*!
  \class FidSegmentation
  \brief Algorithm for segmenting dots in an image. The dots correspond to the fiducial lines that are orthogonal to the image plane
  \ingroup PlusLibPatternRecognition
*/
class vtkPlusCalibrationExport PlusFidSegmentation
{
public:
  static const double DEFAULT_APPROXIMATE_SPACING_MM_PER_PIXEL;
  static const double DEFAULT_MORPHOLOGICAL_OPENING_CIRCLE_RADIUS_MM;
  static const double DEFAULT_MORPHOLOGICAL_OPENING_BAR_SIZE_MM;
  static const int DEFAULT_CLIP_ORIGIN[2];
  static const int DEFAULT_CLIP_SIZE[2];
  static const double DEFAULT_MAX_LINE_PAIR_DISTANCE_ERROR_PERCENT;
  static const double DEFAULT_ANGLE_TOLERANCE_DEGREES;
  static const double DEFAULT_MAX_ANGLE_DIFFERENCE_DEGREES;
  static const double DEFAULT_MIN_THETA_DEGREES;
  static const double DEFAULT_MAX_THETA_DEGREES;
  static const double DEFAULT_MAX_LINE_SHIFT_MM;
  static const double DEFAULT_THRESHOLD_IMAGE_PERCENT;
  static const double DEFAULT_COLLINEAR_POINTS_MAX_DISTANCE_FROM_LINE_MM;
  static const char* DEFAULT_USE_ORIGINAL_IMAGE_INTENSITY_FOR_DOT_INTENSITY_SCORE;
  static const int DEFAULT_NUMBER_OF_MAXIMUM_FIDUCIAL_POINT_CANDIDATES;

  typedef unsigned char PixelType;

  /*! The different types of phantom the algorithm covers */
  enum FiducialGeometryType
  {
    CALIBRATION_PHANTOM_MULTI_NWIRE, //PerkLab Multi-Nwire phantom
    CIRS_PHANTOM_13_POINT, //CIRS phantom model 45
    CALIBRATION_PHANTOM_6_POINT //PerkLab Double-N phantom, deprecated (superceded by CALIBRATION_PHANTOM_MULTI_NWIRE)
  };

  PlusFidSegmentation();
  virtual ~PlusFidSegmentation();

  /* Read the configuration file */
  PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Set the Frame Size of the image */
  void SetFrameSize(const FrameSizeType& frameSize);
  void SetFiducialGeometry(FiducialGeometryType geometryType);

  /*! Update the parameters, especially the circle need for morphological operations */
  void UpdateParameters();

  /*! Clear the member variables */
  void Clear();

  /*! Check and modify if necessary the region of interest */
  void ValidateRegionOfInterest();

  /*! Morphological operations performed by the algorithm */
  inline PlusFidSegmentation::PixelType ErodePoint0(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Erode0(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType ErodePoint45(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Erode45(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType ErodePoint90(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Erode90(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType ErodePoint135(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Erode135(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  void ErodeCircle(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType DilatePoint0(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Dilate0(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType DilatePoint45(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Dilate45(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType DilatePoint90(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Dilate90(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType DilatePoint135(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic);
  void Dilate135(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  inline PlusFidSegmentation::PixelType DilatePoint(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic, PlusCoordinate2D* shape, int slen);
  void DilateCircle(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image);
  void Subtract(PlusFidSegmentation::PixelType* image, PlusFidSegmentation::PixelType* vals);

  /*!
    Write image with the selected points on it to an image file (possibleFiducialsNNN.bmp)
    \param fiducials position of fiducial points
    \param unalteredImage original image
    \param namePrefix prefix used for image file name generation
    \param frameIndex frame index (used for generating the file name)
  */
  void WritePossibleFiducialOverlayImage(const std::vector< std::vector<double> >& fiducials, PlusFidSegmentation::PixelType* unalteredImage, const char* namePrefix, int frameIndex);

  /*!
    Write image with the selected points on it to an image file (possibleFiducialsNNN.bmp)
    \param fiducials position of fiducial points
    \param unalteredImage original image
    \param namePrefix prefix used for image file name generation
    \param frameIndex frame index (used for generating the file name)
  */
  void WritePossibleFiducialOverlayImage(const std::vector<PlusFidDot>& fiducials, PlusFidSegmentation::PixelType* unalteredImage, const char* namePrefix, int frameIndex);

  /*! Perform the morphological operations on the image */
  void MorphologicalOperations();

  /*! Suppress unwanted parts of the image */
  void Suppress(PlusFidSegmentation::PixelType* image, double percent_thresh);

  /*! Accept a dot as a possible fiducial */
  inline bool AcceptDot(PlusFidDot& dot);

  /*!
    Cluster the dots.
    Returns true on success.
    If tooManyCandidates is true then the maximum limit of clusters are reached and so not all clusters are computed.
  */
  bool Cluster(bool& tooManyCandidates);

  /*! Utility function to write image to file */
  static void WritePng(PlusFidSegmentation::PixelType* modifiedImage, std::string outImageName, int cols, int rows);

  /*! Utility function to set parameters in segmentation XML data element to default values */
  static void SetDefaultSegmentationParameters(vtkXMLDataElement* segmentationElement);

  /*! Check if shape (structuring element) contains the new element (a point) */
  bool ShapeContains(std::vector<PlusCoordinate2D>& shape, PlusCoordinate2D point);

  /*! Add neighbors to the cluster */
  inline void ClusteringAddNeighbors(PlusFidSegmentation::PixelType* image, int r, int c, std::vector<PlusFidDot>& m_Test, std::vector<PlusFidDot>& m_Set, std::vector<PlusFidSegmentation::PixelType>& m_Vals);

  // Accessors and mutators

  /*! Set the possible fiducials Image file name */
  void SetPossibleFiducialsImageFilename(std::string value) { m_PossibleFiducialsImageFilename = value; };

  /*! Get the threshold of the image, this is a percent value */
  double GetThresholdImagePercent() { return m_ThresholdImagePercent; };

  /*! Get the debug output value, if true more debug information are provided but the speed is lower */
  bool GetDebugOutput() { return m_DebugOutput; };

  /*! Set the debug output value, if true more debug information are provided but the speed is lower */
  void SetDebugOutput(bool value) { m_DebugOutput = value; };

  /*! Get the size of the bar for the morphological operations */
  unsigned int GetMorphologicalOpeningBarSizePx();

  /*! Get the size of the frame as an array */
  FrameSizeType GetFrameSize() { return m_FrameSize; };

  /*! Get the vector that contains all the dots that have been segmented */
  std::vector<PlusFidDot>& GetDotsVector() {return m_DotsVector; };

  /*! Get the dots that are considered candidates */
  void SetCandidateFidValues(const std::vector<PlusFidDot>& value) { m_CandidateFidValues = value; };

  /*! Set the dots that are considered candidates */
  std::vector<PlusFidDot>& GetCandidateFidValues() { return m_CandidateFidValues; };

  /*! Set the maximum number of candidates to generate */
  void SetNumberOfMaximumFiducialPointCandidates(int aValue);

  /*! Get the geometry type of the phantom, so far only the 6 points NWires and the CIRS phantom model 45 are supported */
  FiducialGeometryType  GetFiducialGeometry() { return m_FiducialGeometry; };

  /*! Get the working copy of the image */
  PlusFidSegmentation::PixelType* GetWorking() {return m_Working; };

  /*! Get the unaltered copy of the image */
  PlusFidSegmentation::PixelType* GetUnalteredImage() {return m_UnalteredImage; };

  /*! Set the Approximate spacing, this is in Mm per pixel */
  void  SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };

  /*! Set the radius of the opening circle needed for the morphological operations */
  void  SetMorphologicalOpeningCircleRadiusMm(double value) { m_MorphologicalOpeningCircleRadiusMm = value; };

  /*! Set the size in Mm of the opening bar needed for the morphological operations */
  void  SetMorphologicalOpeningBarSizeMm(double value) { m_MorphologicalOpeningBarSizeMm = value; };

  /*! Set the region of interest in the image */
  void  SetRegionOfInterest(unsigned int xMin, unsigned int yMin, unsigned int xMax, unsigned int yMax);

  /*! Validates the region of interest that was set for the image and returns it */
  void  GetRegionOfInterest(unsigned int& xMin, unsigned int& yMin, unsigned int& xMax, unsigned int& yMax);

  /*! Set the threshold of the image, this is a percent value */
  void  SetThresholdImagePercent(double value) { m_ThresholdImagePercent = value; };

  /*! Set to true to use the original image intensity for the dots intensity values */
  void  SetUseOriginalImageIntensityForDotIntensityScore(bool value) { m_UseOriginalImageIntensityForDotIntensityScore = value; };

protected:
  FrameSizeType m_FrameSize;
  std::array<unsigned int, 4> m_RegionOfInterest; // xmin, ymin; xmax, ymax
  bool m_UseOriginalImageIntensityForDotIntensityScore;

  unsigned int m_NumberOfMaximumFiducialPointCandidates;

  /*! Segmentation threshold (in percentage, minimum is 0, maximum is 100) */
  double m_ThresholdImagePercent;

  double m_MorphologicalOpeningBarSizeMm;
  double m_MorphologicalOpeningCircleRadiusMm;

  std::string m_PossibleFiducialsImageFilename;

  FiducialGeometryType m_FiducialGeometry;

  std::vector<PlusCoordinate2D> m_MorphologicalCircle;

  double m_ApproximateSpacingMmPerPixel;
  double m_ImageScalingTolerancePercent[4];
  double m_ImageNormalVectorInPhantomFrameEstimation[3];
  double m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[6];
  double m_ImageToPhantomTransform[16];

  /*! True if the dots are found, false otherwise. */
  bool m_DotsFound;

  /*! X and Y values of found dots. */
  std::vector<std::vector<double>> m_FoundDotsCoordinateValue;

  /*! Number of possible fiducial points */
  double m_NumDots;

  /*! Pointer to the fiducial candidates coordinates */
  std::vector<PlusFidDot> m_CandidateFidValues;

  PlusFidSegmentation::PixelType* m_Working;
  PlusFidSegmentation::PixelType* m_Dilated;
  PlusFidSegmentation::PixelType* m_Eroded;
  PlusFidSegmentation::PixelType* m_UnalteredImage;

  std::vector<PlusFidDot> m_DotsVector;

  bool m_DebugOutput;
};

#endif // _FIDUCIAL_SEGMENTATION_H
