#ifndef _ULTRASOUND_FIDUCIAL_SEGMENTATION_H
#define _ULTRASOUND_FIDUCIAL_SEGMENTATION_H

#include "PlusConfigure.h"
#include <string.h>
#include <vector>
#include <math.h>
#include <strstream>
#include <iostream>
#include <iomanip>

#include <climits>
// Thomas Kuiran Chen - ANSI-C++ headers
#include <cmath>
//#include <math.h>
#include <assert.h>

#include "itkRGBPixel.h"
#include "itkPointSet.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h" 
#include "itkThresholdImageFilter.h"
#include "itkMeanImageFilter.h" 
#include "itkSubtractImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkPNGImageIO.h"

// Thomas Kuiran Chen - retouched for ANSI-C++
// NOTE: M_PI is not ANSI C++ defined math constant. 
#define M_PI (float)3.14159265358

#define MAX_DOTS      1024

// We shall be able to store all the lines that are constucted from MAX_DOTS number of points
#define MAX_LINES_2PT (MAX_DOTS*(MAX_DOTS-1)/2)

#define MAX_LINES     512
#define MAX_PAIRS     1024

/* This value is hardcoded in a comment in the interface. */
#define MIN_WINDOW_DIST 8

#define MAX_CLUSTER_VALS 16384

typedef unsigned char PixelType;

//-----------------------------------------------------------------------------

struct Item//TODO find about it and hopefully change it to a class with proper name
{
	Item(){ roff = 0; coff = 0; }; 
	Item( int r, int c) { roff = r; coff = c; }; 
	bool operator== (const Item& data) const { return (roff == data.roff && coff == data.coff) ; }
	int roff;
	int coff;
};

//-----------------------------------------------------------------------------

class Dot
{
	public:
		static bool lessThan( Dot &dot1, Dot &dot2 );//compare the intensity of 2 dots

		void	SetX(float value) { m_X = value; };
		float	GetX() { return m_X; };
		void	SetY(float value) { m_Y = value; };
		float	GetY() { return m_Y; };
		void	SetDotIntensity(float value) { m_DotIntensity = value; };
		float	GetDotIntensity() { return m_DotIntensity; };

	protected:
		float	m_X;
		float	m_Y;
		float	m_DotIntensity;
};

//-----------------------------------------------------------------------------

class Position
{
	public:
		//static bool lessThan( Dot *b1, Dot *b2 );//compare two dots (coordinate wise)
		static bool lessThan( std::vector<Dot>::iterator b1, std::vector<Dot>::iterator b2 );
		
		void	SetX(int value) { m_X = value; };
		int		GetX() { return m_X; };
		void	SetY(int value) { m_Y = value; };
		int		GetY() { return m_Y; };

	protected:
		int		m_X;
		int		m_Y;
};

//-----------------------------------------------------------------------------

class Line
{
	public:
		static bool lessThan( Line &line1, Line &line2 );//compare the intensity of 2 lines
		static bool compareLines( const Line &line1, const Line &line2 );//compare two lines
		
		void				SetLinePoint(int aIndex, int aValue) { m_LinePoints[aIndex] = aValue; };
		std::vector<int>*	GetLinePoints() { return &m_LinePoints; };
		int					GetLinePoint(int aIndex) const{ return m_LinePoints[aIndex]; };
		void				SetLineSlope(float value) { m_LineSlope = value; };
		float				GetLineSlope() { return m_LineSlope; };
		void				SetLinePosition(float value) { m_LinePosition = value; };
		float				GetLinePosition() { return m_LinePosition; };
		void				SetLineIntensity(float value) { m_LineIntensity = value; };
		float				GetLineIntensity() { return m_LineIntensity; };
		void				SetLineError(float value) { m_LineError = value; };
		float				GetLineError() { return m_LineError; };
		void				SetLineLength(float value) { m_LineLength = value; };
		float				GetLineLength() { return m_LineLength; };

	protected:
		std::vector<int>	m_LinePoints; // indices of points that make up the line
		float				m_LineSlope; // slope of the line
		float				m_LinePosition; // position of the line (distance of the line from the origin)
		float				m_LineIntensity;
		float				m_LineError;
		float				m_LineLength;
};

//-----------------------------------------------------------------------------

class LinePair
{
	public:
		static bool lessThan( LinePair &pair1, LinePair &pair2 );//compare the intensity of two pairs of lines

		void	SetLine1(int value) { m_Line1 = value; };
		int		GetLine1() { return m_Line1; };
		void	SetLine2(int value) { m_Line2 = value; };
		int		GetLine2() { return m_Line2; };
		void	SetLinePairIntensity(float value) { m_LinePairIntensity = value; };
		float	GetLinePairIntensity() { return m_LinePairIntensity; };
		void	SetLinePairError(float value) { m_LinePairError = value; };
		float	GetLinePairError() { return m_LinePairError; };
		void	SetAngleDifference(float value) { m_AngleDifference = value; };
		float	GetAngleDifference() { return m_AngleDifference; };
		void	SetAngleConf(float value) { m_AngleConf = value; };
		float	GetAngleConf() { return m_AngleConf; };

	protected:
		int		m_Line1;
		int		m_Line2;
		float	m_LinePairIntensity;
		float	m_LinePairError;
		float	m_AngleDifference;
		float	m_AngleConf;
};


//-----------------------------------------------------------------------------

class SortedAngle
{
	public:
		static bool lessThan( SortedAngle &pt1, SortedAngle &pt2 );//compare two vector angles

		void				SetAngle(double value) { m_Angle = value; };
		double				GetAngle() { return m_Angle; };
		void				SetPointIndex(int value) { m_PointIndex = value; };
		int					GetPointIndex() { return m_PointIndex; };
		void				SetCoordinate(std::vector<double> value) { m_Coordinate = value; };
		std::vector<double>	GetCoordinate() { return m_Coordinate; };

	protected:
		double				m_Angle;
		int					m_PointIndex;
		std::vector<double>	m_Coordinate;
};

//-----------------------------------------------------------------------------

struct Wire
{
	int id;
	char name[128];
	double endPointFront[3];
	double endPointBack[3];
};

//-----------------------------------------------------------------------------

class NWire
{
public:
	Wire wires[3];
	double intersectPosW12[3]; // Use wire.id mod 3 if not first layer
	double intersectPosW32[3];

	Wire *GetWireById(int aId) {
		if ((wires[0].id % 3) == (aId % 3)) { return &wires[0]; }
		else if ((wires[1].id % 3) == (aId % 3)) { return &wires[1]; }
		else if ((wires[2].id % 3) == (aId % 3)) { return &wires[2]; }
		else { return NULL; }
	};
};

//-----------------------------------------------------------------------------

class SegmentationParameters
{
	public:
		enum FiducialGeometryType
		{
			CALIBRATION_PHANTOM_6_POINT, //PerkLab Double-N phantom
			TAB2_5_POINT, // Tissue Ablation Box version 2, with 5 visible fiducials
			TAB2_6_POINT // Tissue Ablation Box version 2, with 2 horizontal 3 point lines 
		};

		SegmentationParameters::SegmentationParameters();

		void					UpdateParameters();
		
		void					SetUseOriginalImageIntensityForDotIntensityScore(bool value) { m_UseOriginalImageIntensityForDotIntensityScore = value; };
		bool					GetUseOriginalImageIntensityForDotIntensityScore() { return m_UseOriginalImageIntensityForDotIntensityScore; };

		void					SetThresholdImageTop(double value) { m_ThresholdImageTop = value; };
		double					GetThresholdImageTop() { return m_ThresholdImageTop; };
		void					SetThresholdImageBottom(double value) { m_ThresholdImageBottom = value; };
		double					GetThresholdImageBottom() { return m_ThresholdImageBottom; };

		void					SetMaxLineLengthErrorPercent(double value) { m_MaxLineLengthErrorPercent = value; };
		double					GetMaxLineLengthErrorPercent() { return m_MaxLineLengthErrorPercent; };
		void					SetMaxLinePairDistanceErrorPercent(double value) { m_MaxLinePairDistanceErrorPercent = value; };
		double					GetMaxLinePairDistanceErrorPercent() { return m_MaxLinePairDistanceErrorPercent; };

		void					SetMinLineLenMm(double value) { m_MinLineLenMm = value; };
		double					GetMinLineLenMm() { return m_MinLineLenMm; };
		void					SetMaxLineLenMm(double value) { m_MaxLineLenMm = value; };
		double					GetMaxLineLenMm() { return m_MaxLineLenMm; };

		void					SetMinLinePairDistMm(double value) { m_MinLinePairDistMm = value; };
		double					GetMinLinePairDistMm() { return m_MinLinePairDistMm; };
		void					SetMaxLinePairDistMm(double value) { m_MaxLinePairDistMm = value; };
		double					GetMaxLinePairDistMm() { return m_MaxLinePairDistMm; };

		void					SetMaxLineErrorMm(double value) { m_MaxLineErrorMm = value; };
		double					GetMaxLineErrorMm() { return m_MaxLineErrorMm; };

		void					SetFindLines3PtDist(double value) { m_FindLines3PtDist = value; };
		double					GetFindLines3PtDist() { return m_FindLines3PtDist; };
		void					SetMaxAngleDiff(double value) { m_MaxAngleDiff = value; };
		double					GetMaxAngleDiff() { return m_MaxAngleDiff; };
		void					SetMinTheta(double value) { m_MinTheta = value; };
		double					GetMinTheta() { return m_MinTheta; };
		void					SetMaxTheta(double value) { m_MaxTheta = value; };
		double					GetMaxTheta() { return m_MaxTheta; };

		void					SetMaxUangleDiff(double value) { m_MaxUangleDiff = value; };
		double					GetMaxUangleDiff() { return m_MaxUangleDiff; };
		void					SetMaxUsideLineDiff(double value) { m_MaxUsideLineDiff = value; };
		double					GetMaxUsideLineDiff() { return m_MaxUsideLineDiff; };
		void					SetMinUsideLineLength(double value) { m_MinUsideLineLength = value; };
		double					GetMinUsideLineLength() { return m_MinUsideLineLength; };
		void					SetMaxUsideLineLength(double value) { m_MaxUsideLineLength = value; };
		double					GetMaxUsideLineLength() { return m_MaxUsideLineLength; };

		void					SetMorphologicalOpeningBarSizeMm(double value) { m_MorphologicalOpeningBarSizeMm = value; };
		double					GetMorphologicalOpeningBarSizeMm() { return m_MorphologicalOpeningBarSizeMm; };
		void					SetMorphologicalOpeningCircleRadiusMm(double value) { m_MorphologicalOpeningCircleRadiusMm = value; };
		double					GetMorphologicalOpeningCircleRadiusMm() { return m_MorphologicalOpeningCircleRadiusMm; };
		void					SetScalingEstimation(double value) { m_ScalingEstimation = value; };
		double					GetScalingEstimation() { return m_ScalingEstimation; };

		void					SetFiducialGeometry(FiducialGeometryType value) { m_FiducialGeometry = value; };
		FiducialGeometryType	GetFiducialGeometry() { return m_FiducialGeometry; };
		void					SetNWires(std::vector<NWire> value) { m_NWires = value; };
		std::vector<NWire>		GetNWires() { return m_NWires; };

		void SetMorphologicalCircle(std::vector<Item> value) { m_MorphologicalCircle = value; };
		std::vector<Item> GetMorphologicalCircle() { return m_MorphologicalCircle; };

	protected:
			
		bool					m_UseOriginalImageIntensityForDotIntensityScore;

		double 					m_ThresholdImageTop;  // segmentation threshold (in percentage, minimum is 0, maximum is 100 at the top half of the image
		double 					m_ThresholdImageBottom;  // segmentation threshold (in percentage, minimum is 0, maximum is 100 at the bottom half of the image

		// line length and line pair distance errors in percent - read from phantom definition
		double 					m_MaxLineLengthErrorPercent;
		double 					m_MaxLinePairDistanceErrorPercent;

		// min and max length of 3pt line (mm) - computed from input error percent and NWire definitions
		double 					m_MinLineLenMm;
		double 					m_MaxLineLenMm;

		// min and max distance between two detected parallel lines - computed from input error percent and NWire definitions
		// NOTE: This parameter should be adjusted w.r.t phantom design specs (e.g., distance between N-wires).
		double 					m_MinLinePairDistMm; 	
		double 					m_MaxLinePairDistMm;

		double 					m_MaxLineErrorMm;

		double 					m_FindLines3PtDist; 
		double 					m_MaxAngleDiff; 
		double 					m_MinTheta; 
		double 					m_MaxTheta; 

		double					m_MaxUangleDiff; // max difference from 90 deg of angle betwen the side lines of the U and the baseline, in radians
		double					m_MaxUsideLineDiff; // max difference of the two side line endpoints, in pixels
		double					m_MinUsideLineLength; // min length of the U side line, in pixels
		double					m_MaxUsideLineLength; // max length of the U side line, in pixels

		double					m_MorphologicalOpeningBarSizeMm; 
		double					m_MorphologicalOpeningCircleRadiusMm; 
		double					m_ScalingEstimation; 
		
		FiducialGeometryType	m_FiducialGeometry;
		std::vector<NWire>		m_NWires;
		
		std::vector<Item>		m_MorphologicalCircle; 
};

//-----------------------------------------------------------------------------

#define WIRE1 3
#define WIRE2 4
#define WIRE3 5
#define WIRE4 0
#define WIRE5 1
#define WIRE6 2

class SegmentationResults
{
	public:
		SegmentationResults();
		
		void Clear();

		void								SetDotsFound(bool value) { m_DotsFound = value; };
		bool								GetDotsFound() { return m_DotsFound; };

		void								SetFoundDotsCoordinateValue(std::vector< std::vector<double> > value) { m_FoundDotsCoordinateValue = value; };
		std::vector< std::vector<double> >	GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };

		void								SetAngles(float value) { m_Angles = value; };
		float								GetAngles() { return m_Angles; };

		void								SetIntensity(float value) { m_Intensity = value; };
		float								GetIntensity() { return m_Intensity; };
		void								SetNumDots(double value) { m_NumDots = value; };
		double								GetNumDots() { return m_NumDots; };
		void								SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };
		std::vector<Dot>								GetCandidateFidValues() { return m_CandidateFidValues; };
		
	protected:
		/* True if the dots are found, false otherwise. */
		bool m_DotsFound;

		/* X and Y values of found dots. */
		//vector<vector<double>> m_FoundDotsCoordinateValue;
		std::vector< std::vector<double> > m_FoundDotsCoordinateValue; 
		/* The degree to which the lines are parallel and the dots linear.  On the
		 * range 0-1, with 0 being a very good angles score and 1 being the
		 * threshold of acceptability. */
		float	m_Angles;

		/* The combined intensity of the six dots. This is the sum of the pixel
		 * values after the morphological operations, with the pixel values on the
		 * range 0-1.  A good intensity score is over 100. A bad one (but still
		 * valid) is below 25. */
		float	m_Intensity;
		double	m_NumDots; // number of possibel fiducial points
		std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates
};

//-----------------------------------------------------------------------------

class SegImpl
{	
	public:
		SegImpl(int sizeX, int sizeY, int searchOriginX, int searchOriginY, int searchSizeX, int searchSizeY , bool debugOutput /*=false*/, std::string);
		~SegImpl();

		void find_lines3pt();
		void find_lines2pt();
		void compute_line( Line &line, std::vector<Dot> dots );
		float compute_t( Dot *dot1, Dot *dot2 );

		inline PixelType erode_point_0( PixelType *image, unsigned int ir, unsigned int ic );
		void erode_0( PixelType *dest, PixelType *image );
		inline PixelType erode_point_45( PixelType *image, unsigned int ir, unsigned int ic );
		void erode_45( PixelType *dest, PixelType *image );
		inline PixelType erode_point_90( PixelType *image, unsigned int ir, unsigned int ic );
		void erode_90( PixelType *dest, PixelType *image );
		inline PixelType erode_point_135( PixelType *image, unsigned int ir, unsigned int ic );
		void erode_135( PixelType *dest, PixelType *image );
		void erode_circle( PixelType *dest, PixelType *image );
		inline PixelType dilate_point_0( PixelType *image, unsigned int ir, unsigned int ic );
		void dilate_0( PixelType *dest, PixelType *image );
		inline PixelType dilate_point_45( PixelType *image, unsigned int ir, unsigned int ic );
		void dilate_45( PixelType *dest, PixelType *image );
		inline PixelType dilate_point_90( PixelType *image, unsigned int ir, unsigned int ic );
		void dilate_90( PixelType *dest, PixelType *image );
		inline PixelType dilate_point_135( PixelType *image, unsigned int ir, unsigned int ic );
		void dilate_135( PixelType *dest, PixelType *image );
		inline PixelType dilate_point( PixelType *image, unsigned int ir, unsigned int ic, Item *shape, int slen );
		void dilate_circle( PixelType *dest, PixelType *image );
		void subtract( PixelType *image, PixelType *vals );
		void morphological_operations();	
		void setdebugOutput(bool on ); // addition to turn intermediate output on/off
		void setpossibleFiducialsImageFilename(int index); 
		inline void trypos( PixelType *image, int r, int c );
		void suppress( PixelType *image, float percent_thresh_top, float percent_thresh_bottom ); // a different threshold can be applied on the top and the bottom of the image
		void WritePossibleFiducialOverlayImage(std::vector<Dot> fiducials, PixelType *unalteredImage); 

		void find_u_shape_line_triad(SegmentationResults &segResult);
		void find_double_n_lines(SegmentationResults &segResult);

		bool accept_line( Line &line );  

		static void WritePng(PixelType *modifiedImage, std::string outImageName, int cols, int rows); // addition to write out intermediate files

		inline bool accept_dot( Dot &dot );
		void cluster();

		void find_lines();
		void find_pairs();

		void uscseg( PixelType *image,  SegmentationParameters &segParams, SegmentationResults &segResult );

		void draw_dots( PixelType *image,  std::vector<Dot>::iterator dotsIterator, int ndots);
		void draw_lines( PixelType *image, std::vector<Line>::iterator linesIterator, int nlines );
		void draw_pair( PixelType *image, std::vector<LinePair>::iterator pairIterator  );
		void print_results();
		void draw_results( PixelType *data );

		void sort_top_to_bottom( LinePair *pair );
		void sort_right_to_left( Line *line );

		int GetMorphologicalOpeningBarSizePx(); 

	public: //TODO protected
		SegmentationParameters m_SegParams;

		unsigned int size, bytes;
		unsigned int rows, cols;

		unsigned int vertLow, horzLow;
		unsigned int vertHigh, horzHigh;

		PixelType *working;
		PixelType *dilated;
		PixelType *eroded;
		PixelType *unalteredImage; 

		/* Dot, line and pair data. */
		std::vector<Dot> m_DotsVector;
		std::vector<Line> m_LinesVector;
		std::vector<LinePair> m_PairsVector;

		/* Cluster data. */
		Position test[MAX_CLUSTER_VALS];
		Position set[MAX_CLUSTER_VALS];
		PixelType vals[MAX_CLUSTER_VALS];
		int ntest, nset;

		/* Line finding. */
		std::vector<Line> m_TwoPointsLinesVector;
		std::vector< std::vector<Line> > uShapes;

		bool m_DebugOutput; 
		std::string possibleFiducialsImageFilename;
};

//-----------------------------------------------------------------------------

class UltraSoundFiducialSegmentationTools
{
	public:
		UltraSoundFiducialSegmentationTools(){}
		static bool AngleMoreThan( SortedAngle &pt1,  SortedAngle &pt2 );
		
		//Search Methods
		template<class T, class LessThan> static void doSort(std::vector<T> tmpStor, std::vector<T> data, long len);
		template<class T, class LessThan> static void sort(std::vector<T> data, long len);
			
		//Binary search methods
		template <class T, class Compare> static bool BinarySearchFind(const T &item, std::vector<T> data, int dlen );
		template <class T, class Compare> static void BinarySearchInsert( T &item, std::vector<T> data, int &dlen );
		
};

//-----------------------------------------------------------------------------

class KPhantomSeg
{
	public:

		/* Constructor: initialize the segmentation structure with image
		 * dimenstions.  It can then be used to segment a sequence of images. See
		 * below for descriptions of parameters.
		 * DebugOutput: if it is set to true, then intermediate results 
		   are written into files.
		 */
		KPhantomSeg(int sizeX, int sizeY, 
				int searchOriginX, int searchOriginY, 
				int searchSizeX, int searchSizeY, bool debugOutput=false, std::string possibleFiducialsImageFilename="");
		
		~KPhantomSeg();
		
		/* Run the segmentation, store the results in the 
		 * output parameters described below. 
		 *
		 * data: One byte per pixel, on range 0-255. The origin of the image
		 * (0,0) is the top left corner. Increasing X moves to the right and
		 * increasng Y moves down. The returned dot locations are in this
		 * coordinate system. The data is in row major order. The offset of a
		 * pixel is computed using the formula ( posY * sizeX + posX ). */
		void segment( unsigned char *data,  SegmentationParameters &segParams );

		/* Debugging routines: printing and drawing the results. */
		void printResults();
		void drawResults( unsigned char *data );
		void GetSegmentationResults(SegmentationResults &segResults); 
		
		static std::vector<std::vector<double> > sortInAscendingOrder(std::vector<std::vector<double> > fiducials);  

		void					SetSizeX(int value) { m_SizeX = value; };
		int						GetSizeX() { return m_SizeX; };
		void					SetSizeY(int value) { m_SizeY = value; };
		int						GetSizeY() { return m_SizeY; };

		void					SetSearchOriginX(int value) { m_SearchOriginX = value; };
		int						GetSearchOriginX() { return m_SearchOriginX; };
		void					SetSearchOriginY(int value) { m_SearchOriginY = value; };
		int						GetSearchOriginY() { return m_SearchOriginY; };

		void					SetSearchSizeX(int value) { m_SearchSizeX = value; };
		int						GetSearchSizeX() { return m_SearchSizeX; };
		void					SetSearchSizeY(int value) { m_SearchSizeY = value; };
		int						GetSearchSizeY() { return m_SearchSizeY; };

		void					SetPossibleFiducialsImageFilename(std::string value) { m_PossibleFiducialsImageFilename = value; };
		std::string				GetPossibleFiducialsImageFilename() { return m_PossibleFiducialsImageFilename; };

		void					SetSegParams(SegmentationParameters value) { m_SegParams = value; };
		SegmentationParameters	GetSegParams() { return m_SegParams; };
		void					SetSegImpl(SegImpl	* value) { m_SegImpl = value; };
		SegImpl	*				GetSegImpl() { return m_SegImpl; };
		void					SetSegResult(SegmentationResults value) { m_SegResult = value; };
		SegmentationResults		GetSegResult() { return m_SegResult; };

	protected:
		/*
		 * Input
		 */

		/* Image size. 
		 *
		 * NOTE: hardcoding these values in morph.cpp (known there as cols and
		 * rows) will yeild a 15-20% speed increase.
		 * */
		int m_SizeX;
		int m_SizeY;

		/* Search origin and size parameters define the region within the image
		 * where we should search for the dots. Outside of this region the image
		 * data must be null. Around all sides of this region there must be at
		 * least 8 pixels of this null space (searchOrigin >= 8 and searchSize <=
		 * imageSize-16). */
		int m_SearchOriginX;
		int m_SearchOriginY;

		int m_SearchSizeX;
		int m_SearchSizeY;

		std::string m_PossibleFiducialsImageFilename; 

		/*
		 * Output
		 */

		/*
		 * Private Data
		 */  
		
		SegmentationParameters	m_SegParams; 
		SegImpl	*				m_SegImpl;
		SegmentationResults		m_SegResult; 
};

#endif //_ULTRASOUND_FIDUCIAL_SEGMENTATION_H
