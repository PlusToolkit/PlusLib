/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_SEGMENTATION_H
#define _FIDUCIAL_SEGMENTATION_H

#include "FidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
#include "vtkXMLDataElement.h"
#include <string.h>

//-----------------------------------------------------------------------------

/*!
  \struct Coordinate2D
  \brief Structure holding a coordinate of a structuring element
  \ingroup PlusLibPatternRecognition
*/ 
struct vtkCalibrationAlgoExport Coordinate2D
{
  Coordinate2D(){ Y = 0; X = 0; }; 
  Coordinate2D( int y, int x) { Y = y; X = x; }; 
  bool operator== (const Coordinate2D& data) const { return (Y == data.Y && X == data.X) ; }
  int Y;
  int X;
}; 

//-----------------------------------------------------------------------------

/*!
  \class FidSegmentation
  \brief Algorithm for segmenting dots in an image. The dots correspond to the fiducial lines that are orthogonal to the image plane
  \ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport FidSegmentation
{
  public:
    static const int DEFAULT_NUMBER_OF_MAXIMUM_FIDUCIAL_POINT_CANDIDATES;

    typedef unsigned char PixelType;

    /*! The different types of phantom the algorithm covers */
    enum FiducialGeometryType
    {
      CALIBRATION_PHANTOM_MULTI_NWIRE, //PerkLab Multi-Nwire phantom      
      CIRS_PHANTOM_13_POINT, //CIRS phantom model 45
      CALIBRATION_PHANTOM_6_POINT //PerkLab Double-N phantom, deprecated (superceded by CALIBRATION_PHANTOM_MULTI_NWIRE)
    };

    FidSegmentation();
    virtual ~FidSegmentation();

    /* Read the configuration file */
    PlusStatus ReadConfiguration( vtkXMLDataElement* rootConfigElement );

    /*! Set the Frame Size of the image */
    void SetFrameSize( int frameSize[2] );

    void SetFiducialGeometry( FiducialGeometryType geometryType );

    /*! Update the parameters, espcially the circle need for morphological operations */
    void UpdateParameters();

    /*! Clear the member variables */
    void Clear();

    /*! Check and modify if necessary the region of interest */
    void ValidateRegionOfInterest();

    /*! Morphological operations performed by the algorithm */
    inline FidSegmentation::PixelType    ErodePoint0( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                Erode0( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    ErodePoint45( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                 Erode45( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    ErodePoint90( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                 Erode90( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    ErodePoint135( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                 Erode135( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    void                 ErodeCircle( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    DilatePoint0( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                 Dilate0( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    DilatePoint45( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                 Dilate45( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    DilatePoint90( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                Dilate90( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    DilatePoint135( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic );
    void                 Dilate135( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    inline FidSegmentation::PixelType    DilatePoint( FidSegmentation::PixelType *image, unsigned int ir, unsigned int ic, Coordinate2D *shape, int slen );
    void                 DilateCircle( FidSegmentation::PixelType *dest, FidSegmentation::PixelType *image );
    void                 Subtract( FidSegmentation::PixelType *image, FidSegmentation::PixelType *vals );
        
    /*! 
      Write image with the selected points on it to an image file (possibleFiducialsNNN.bmp)
      \param fiducials position of fiducial points
      \param unalteredImage original image
      \param namePrefix prefix used for image file name generation
      \param frameIndex frame index (used for generating the file name)
    */
    void WritePossibleFiducialOverlayImage(const std::vector< std::vector<double> >& fiducials, FidSegmentation::PixelType *unalteredImage, const char* namePrefix, int frameIndex); 

    /*! 
      Write image with the selected points on it to an image file (possibleFiducialsNNN.bmp)
      \param fiducials position of fiducial points
      \param unalteredImage original image
      \param namePrefix prefix used for image file name generation
      \param frameIndex frame index (used for generating the file name)
    */
    void WritePossibleFiducialOverlayImage(const std::vector<FidDot>& fiducials, FidSegmentation::PixelType *unalteredImage, const char* namePrefix, int frameIndex);

    /*! Perform the morphological operations on the image */
    void MorphologicalOperations();  

    /*! Suppress unwanted parts of the image */
    void Suppress( FidSegmentation::PixelType *image, double percent_thresh ); 

    /*! Accept a dot as a possible fiducial */
    inline bool AcceptDot( FidDot &dot );

    /*!
      Cluster the dots.
      Returns true on success.
      If tooManyCandidates is true then the maximum limit of clusters are reached and so not all clusters are computed.
    */
    bool Cluster(bool& tooManyCandidates);

    /*! Utility function to write image to file */
    static void WritePng(FidSegmentation::PixelType *modifiedImage, std::string outImageName, int cols, int rows); 

    /*! Check if shape (structuring element) contains the new element (a point) */
    bool ShapeContains( std::vector<Coordinate2D>& shape, Coordinate2D point );

    /*! Add neighbors to the cluster */
    inline void ClusteringAddNeighbors(FidSegmentation::PixelType *image, int r, int c, std::vector<FidDot> &m_Test, std::vector<FidDot> &m_Set, std::vector<FidSegmentation::PixelType>&m_Vals);
    
    // Accessors and mutators

    /*! Set the possible fiducials Image file name */
    void SetPossibleFiducialsImageFilename(std::string value) { m_PossibleFiducialsImageFilename = value; };

    /*! Get the threshold of the image, this is a percent value */
    double GetThresholdImagePercent() { return m_ThresholdImagePercent; };

    /*! Get the debug output value, if true more debug information are provided but the speed is lower */
    bool GetDebugOutput() { return m_DebugOutput; };

    /*! Set the debug output value, if true more debug information are provided but the speed is lower */
    void SetDebugOutput(bool value) { m_DebugOutput = value; };

    /*! Get the size of the bar for the morphological oprations */
    int GetMorphologicalOpeningBarSizePx(); 

    /*! Get the size of the frame as an array */
    int* GetFrameSize() { return m_FrameSize; };

    /*! Get the vector that contains all the dots that have been segmented */
    std::vector<FidDot>& GetDotsVector() {return m_DotsVector; };  

    /*! Get the dots that are considered candidates */
    void SetCandidateFidValues(const std::vector<FidDot>& value) { m_CandidateFidValues = value; };

    /*! Set the dots that are considered candidates */
    std::vector<FidDot>& GetCandidateFidValues() { return m_CandidateFidValues; };

    /*! Set the maximum number of candidates to generate */
    void SetNumberOfMaximumFiducialPointCandidates( int aValue );

    /*! Get the geometry type of the phantom, so far only the 6 points NWires and the CIRS phantom model 45 are supported */
    FiducialGeometryType  GetFiducialGeometry() { return m_FiducialGeometry; };

    /*! Get the working copy of the image */
    FidSegmentation::PixelType* GetWorking() {return m_Working; };

    /*! Get the unaltered copy of the image */
    FidSegmentation::PixelType* GetUnalteredImage() {return m_UnalteredImage; };

    /*! Set the Approximate spacing, this is in Mm per pixel */
    void  SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };

    /*! Set the radius of the opening circle needed for the morphological operations */
    void  SetMorphologicalOpeningCircleRadiusMm(double value) { m_MorphologicalOpeningCircleRadiusMm = value; };

    /*! Set the size in Mm of the opening bar needed for the morphological operations */
    void  SetMorphologicalOpeningBarSizeMm(double value) { m_MorphologicalOpeningBarSizeMm = value; };

    /*! Set the region of interest in the image */
    void  SetRegionOfInterest(int xMin, int yMin, int xMax, int yMax);

    /*! Validates the region of interest that was set for the image and returns it */
    void  GetRegionOfInterest(int &xMin, int &yMin, int &xMax, int &yMax);

    /*! Set the threshold of the image, this is a percent value */
    void  SetThresholdImagePercent(double value) { m_ThresholdImagePercent = value; };

    /*! Set to true to use the original image intensity for the dots intensity values */
    void  SetUseOriginalImageIntensityForDotIntensityScore(bool value) { m_UseOriginalImageIntensityForDotIntensityScore = value; };

  protected:
    int                   m_FrameSize[2];
    int                   m_RegionOfInterest[4]; // xmin, ymin; xmax, ymax
    bool                  m_UseOriginalImageIntensityForDotIntensityScore;

    int                   m_NumberOfMaximumFiducialPointCandidates;

    /*! Segmentation threshold (in percentage, minimum is 0, maximum is 100) */
    double                m_ThresholdImagePercent;  

    double                m_MorphologicalOpeningBarSizeMm; 
    double                m_MorphologicalOpeningCircleRadiusMm; 

    std::string           m_PossibleFiducialsImageFilename;
    
    FiducialGeometryType  m_FiducialGeometry;
    
    std::vector<Coordinate2D> m_MorphologicalCircle; 

    double                m_ApproximateSpacingMmPerPixel;
    double                m_ImageScalingTolerancePercent[4];
    double                m_ImageNormalVectorInPhantomFrameEstimation[3];
    double                m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[6];
    double                m_ImageToPhantomTransform[16];

    /*! True if the dots are found, false otherwise. */
    bool                  m_DotsFound;

    /*! X and Y values of found dots. */
    std::vector< std::vector<double> >  m_FoundDotsCoordinateValue; 

    /*! Number of possibel fiducial points */
    double                m_NumDots; 
    /*! Pointer to the fiducial candidates coordinates */
    std::vector<FidDot>      m_CandidateFidValues; 

    FidSegmentation::PixelType*            m_Working;
    FidSegmentation::PixelType*            m_Dilated;
    FidSegmentation::PixelType*            m_Eroded;
    FidSegmentation::PixelType*            m_UnalteredImage; 

    std::vector<FidDot>      m_DotsVector;

    bool                  m_DebugOutput; 
};

#endif // _FIDUCIAL_SEGMENTATION_H
