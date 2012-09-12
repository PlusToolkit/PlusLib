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
struct Coordinate2D
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
class FidSegmentation
{
  public:

    /*! The different types of phantom the algorithm covers */
    enum FiducialGeometryType
    {
      CALIBRATION_PHANTOM_6_POINT, //PerkLab Double-N phantom
      CIRS_PHANTOM_13_POINT //CIRS phantom model 45
    };

    FidSegmentation();
    virtual ~FidSegmentation();

    /* Read the configuration file */
    PlusStatus ReadConfiguration( vtkXMLDataElement* rootConfigElement );

    /*! Set the Frame Size of the image */
    void SetFrameSize( int frameSize[2] );

    /*! Update the parameters, espcially the circle need for morphological operations */
    void UpdateParameters();

    /*! Clear the member variables */
    void Clear();

    /*! Check and modify if necessary the region of interest */
    void ValidateRegionOfInterest();

    /*! Morphological operations performed by the algorithm */
    inline PixelType    ErodePoint0( PixelType *image, unsigned int ir, unsigned int ic );
    void                Erode0( PixelType *dest, PixelType *image );
    inline PixelType    ErodePoint45( PixelType *image, unsigned int ir, unsigned int ic );
    void                 Erode45( PixelType *dest, PixelType *image );
    inline PixelType    ErodePoint90( PixelType *image, unsigned int ir, unsigned int ic );
    void                 Erode90( PixelType *dest, PixelType *image );
    inline PixelType    ErodePoint135( PixelType *image, unsigned int ir, unsigned int ic );
    void                 Erode135( PixelType *dest, PixelType *image );
    void                 ErodeCircle( PixelType *dest, PixelType *image );
    inline PixelType    DilatePoint0( PixelType *image, unsigned int ir, unsigned int ic );
    void                 Dilate0( PixelType *dest, PixelType *image );
    inline PixelType    DilatePoint45( PixelType *image, unsigned int ir, unsigned int ic );
    void                 Dilate45( PixelType *dest, PixelType *image );
    inline PixelType    DilatePoint90( PixelType *image, unsigned int ir, unsigned int ic );
    void                Dilate90( PixelType *dest, PixelType *image );
    inline PixelType    DilatePoint135( PixelType *image, unsigned int ir, unsigned int ic );
    void                 Dilate135( PixelType *dest, PixelType *image );
    inline PixelType    DilatePoint( PixelType *image, unsigned int ir, unsigned int ic, Coordinate2D *shape, int slen );
    void                 DilateCircle( PixelType *dest, PixelType *image );
    void                 Subtract( PixelType *image, PixelType *vals );
        
    /*! 
      Write image with the selected points on it to an image file (possibleFiducialsNNN.bmp)
      \param fiducials position of fiducial points
      \param unalteredImage original image
      \param frameIndex frame index (used for generating the file name)
    */
    void WritePossibleFiducialOverlayImage(std::vector<std::vector<double> > fiducials, PixelType *unalteredImage, int frameIndex); 

    /*! 
      Write image with the selected points on it to an image file (possibleFiducialsNNN.bmp)
      \param fiducials position of fiducial points
      \param unalteredImage original image
      \param frameIndex frame index (used for generating the file name)
    */
    void WritePossibleFiducialOverlayImage(std::vector<Dot> fiducials, PixelType *unalteredImage, int frameIndex);

    /*! Perform the morphological operations on the image */
    void MorphologicalOperations();  

    /*! Suppress unwanted parts of the image */
    void Suppress( PixelType *image, double percent_thresh ); 

    /*! Accept a dot as a possible fiducial */
    inline bool AcceptDot( Dot &dot );

    /*! Cluster the dots */
    void Cluster();

    /*! Utility function to write image to file */
    static void WritePng(PixelType *modifiedImage, std::string outImageName, int cols, int rows); 

    /*! Check if shape (structuring element) contains the new element (a point) */
    bool ShapeContains( std::vector<Coordinate2D> &shape, Coordinate2D point );

    /*! Add neighbors to the cluster */
    inline void ClusteringAddNeighbors(PixelType *image, int r, int c, std::vector<Dot> &m_Test, std::vector<Dot> &m_Set, std::vector<PixelType>&m_Vals);
    
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
    std::vector<Dot> GetDotsVector() {return m_DotsVector; };  

    /*! Get the dots that are considered candidates */
    void SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };

    /*! Set the dots that are considered candidates */
    std::vector<Dot> GetCandidateFidValues() { return m_CandidateFidValues; };

    /*! Get the geometry type of the phantom, so far only the 6 points NWires and the CIRS phantom model 45 are supported */
    FiducialGeometryType  GetFiducialGeometry() { return m_FiducialGeometry; };

    /*! Get the working copy of the image */
    PixelType* GetWorking() {return m_Working; };

    /*! Get the unaltered copy of the image */
    PixelType* GetUnalteredImage() {return m_UnalteredImage; };

    /*! Set the Approximate spacing, this is in Mm per pixel */
    void  SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };

    /*! Set the radius of the opening circle needed for the morphological operations */
    void  SetMorphologicalOpeningCircleRadiusMm(double value) { m_MorphologicalOpeningCircleRadiusMm = value; };

    /*! Set the size in Mm of the opening bar needed for the morphological operations */
    void  SetMorphologicalOpeningBarSizeMm(double value) { m_MorphologicalOpeningBarSizeMm = value; };

    /*! Set the region of interest in the image */
    void  SetRegionOfInterest(int xMin, int yMin, int xMax, int yMax);

    /*! Set the threshold of the image, this is a percent value */
    void  SetThresholdImagePercent(double value) { m_ThresholdImagePercent = value; };

    /*! Set to true to use the original image intensity for the dots intensity values */
    void  SetUseOriginalImageIntensityForDotIntensityScore(bool value) { m_UseOriginalImageIntensityForDotIntensityScore = value; };

  protected:
    int                   m_FrameSize[2];
    int                   m_RegionOfInterest[4];
    bool                  m_UseOriginalImageIntensityForDotIntensityScore;

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
    std::vector<Dot>      m_CandidateFidValues; 

    PixelType*            m_Working;
    PixelType*            m_Dilated;
    PixelType*            m_Eroded;
    PixelType*            m_UnalteredImage; 

    std::vector<Dot>      m_DotsVector;

    bool                  m_DebugOutput; 
};

#endif // _FIDUCIAL_SEGMENTATION_H
