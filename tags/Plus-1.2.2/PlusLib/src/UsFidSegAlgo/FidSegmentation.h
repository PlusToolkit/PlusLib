#ifndef _FIDUCIAL_SEGMENTATION_H
#define _FIDUCIAL_SEGMENTATION_H

#include "FidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
#include "vtkXMLDataElement.h"
#include <string.h>

class FidSegmentation
{
	public:

		enum FiducialGeometryType
		{
			CALIBRATION_PHANTOM_6_POINT, //PerkLab Double-N phantom
			CIRS_PHANTOM_13_POINT //CIRS phantom model 45
		};

		FidSegmentation();
		virtual ~FidSegmentation();

		PlusStatus				  ReadConfiguration( vtkXMLDataElement* segmentationParameters );

		void					      UpdateParameters();
		void				    	  ComputeParameters();
    void                Clear();

    inline PixelType		ErodePoint0( PixelType *image, unsigned int ir, unsigned int ic );
		void					      Erode0( PixelType *dest, PixelType *image );
		inline PixelType		ErodePoint45( PixelType *image, unsigned int ir, unsigned int ic );
		void 					      Erode45( PixelType *dest, PixelType *image );
		inline PixelType		ErodePoint90( PixelType *image, unsigned int ir, unsigned int ic );
		void 					      Erode90( PixelType *dest, PixelType *image );
		inline PixelType		ErodePoint135( PixelType *image, unsigned int ir, unsigned int ic );
		void 					      Erode135( PixelType *dest, PixelType *image );
		void 					      ErodeCircle( PixelType *dest, PixelType *image );
		inline PixelType		DilatePoint0( PixelType *image, unsigned int ir, unsigned int ic );
		void 					      Dilate0( PixelType *dest, PixelType *image );
		inline PixelType		DilatePoint45( PixelType *image, unsigned int ir, unsigned int ic );
		void 					      Dilate45( PixelType *dest, PixelType *image );
		inline PixelType		DilatePoint90( PixelType *image, unsigned int ir, unsigned int ic );
		void					      Dilate90( PixelType *dest, PixelType *image );
		inline PixelType		DilatePoint135( PixelType *image, unsigned int ir, unsigned int ic );
		void 					      Dilate135( PixelType *dest, PixelType *image );
		inline PixelType		DilatePoint( PixelType *image, unsigned int ir, unsigned int ic, Item *shape, int slen );
		void 					      DilateCircle( PixelType *dest, PixelType *image );
		void 					      Subtract( PixelType *image, PixelType *vals );
    		
		void					      WritePossibleFiducialOverlayImage(std::vector<Dot> fiducials, PixelType *unalteredImage); 

		void 					      MorphologicalOperations();	
		void					      Suppress( PixelType *image, float percent_thresh ); 

		inline bool				  AcceptDot( Dot &dot );
		void					      Cluster();

    static void				WritePng(PixelType *modifiedImage, std::string outImageName, int cols, int rows); // addition to write out intermediate files

		bool					    ShapeContains( std::vector<Item> shape, Item newItem );

		inline void				ClusteringAddNeighbors(PixelType *image, int r, int c, std::vector<Position> &m_Test, std::vector<Position> &m_Set, std::vector<PixelType>&m_Vals);
		
		static std::vector<std::vector<double> > SortInAscendingOrder(std::vector<std::vector<double> > fiducials); 

		
		//Accessors and mutators
    void					    SetNWires(std::vector<NWire> value) { m_NWires = value; };
    std::vector<NWire>		GetNWires() { return m_NWires; };

 		std::vector<Item>	GetMorphologicalCircle() { return m_MorphologicalCircle; };
    void					    SetPossibleFiducialsImageFilename(std::string value) { m_PossibleFiducialsImageFilename = value; };

    double				    GetThresholdImage() { return m_ThresholdImage; };

    bool					    GetDebugOutput() { return m_DebugOutput; };

    int						    GetMorphologicalOpeningBarSizePx(); 

    int *					    GetFrameSize() { return m_FrameSize; };
    std::vector<Dot>	GetDotsVector() {return m_DotsVector; };	

    void					    SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };
    std::vector<Dot>	GetCandidateFidValues() { return m_CandidateFidValues; };

    FiducialGeometryType	GetFiducialGeometry() { return m_FiducialGeometry; };

		PixelType *				GetWorking() {return m_Working; };
		PixelType *				GetUnalteredImage() {return m_UnalteredImage; };

	protected:
		int						    m_FrameSize[2];
		int						    m_RegionOfInterest[4];
		bool					    m_UseOriginalImageIntensityForDotIntensityScore;

		double 					  m_ThresholdImage;  // segmentation threshold (in percentage, minimum is 0, maximum is 100 at the top half of the image

		double					  m_MorphologicalOpeningBarSizeMm; 
		double					  m_MorphologicalOpeningCircleRadiusMm; 

		std::string				m_PossibleFiducialsImageFilename;
		
		FiducialGeometryType	m_FiducialGeometry;
		std::vector<NWire>		m_NWires;
		
		std::vector<Item>		m_MorphologicalCircle; 

		double					  m_ScalingEstimation;
    double			      m_ImageScalingTolerancePercent[4];
    double			      m_ImageNormalVectorInPhantomFrameEstimation[3];
    double			      m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[6];
		double					  m_ImageToPhantomTransform[16];

		/* True if the dots are found, false otherwise. */
		bool					    m_DotsFound;

		/* X and Y values of found dots. */
		//vector<vector<double>> m_FoundDotsCoordinateValue;
		std::vector< std::vector<double> >	m_FoundDotsCoordinateValue; 

		double					  m_NumDots; // number of possibel fiducial points
		std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates

		PixelType *				m_Working;
		PixelType *				m_Dilated;
		PixelType *				m_Eroded;
		PixelType *				m_UnalteredImage; 

		std::vector<Dot>	m_DotsVector;

		bool					    m_DebugOutput; 
};

#endif // _FIDUCIAL_SEGMENTATION_H
