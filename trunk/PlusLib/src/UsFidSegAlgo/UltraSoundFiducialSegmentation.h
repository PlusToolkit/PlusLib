#ifndef _ULTRASOUND_FIDUCIAL_SEGMENTATION_H
#define _ULTRASOUND_FIDUCIAL_SEGMENTATION_H

#include "PlusConfigure.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <math.h>

#include <climits>
// Thomas Kuiran Chen - ANSI-C++ headers
#include <cmath>
//#include <math.h>
#include <assert.h>

#include <strstream>
#include <iostream>
#include <iomanip>
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

struct Item
{
	Item(){ roff = 0; coff = 0; }; 
	Item( int r, int c) { roff = r; coff = c; }; 
	bool operator== (const Item& data) const { return (roff == data.roff && coff == data.coff) ; }
	int roff;
	int coff;
};

//-----------------------------------------------------------------------------

struct Pos
{
	int r;
	int c;
};

//-----------------------------------------------------------------------------

struct Dot
{
	float r;
	float c;
	float intensity;
};

//-----------------------------------------------------------------------------

struct Line
{
	float t; // slope of the line
	float p; // position of the line (distance of the line from the origin)
	int b[3]; // indices of points that make up the line
	float intensity;
	float line_error;
	float length;
};

//-----------------------------------------------------------------------------

struct LinePair
{
	int l1, l2;
	float intensity;
	float line_error;
	float angle_diff;
	float angle_conf;
};

//-----------------------------------------------------------------------------

struct Wire
{
	char name[128];
	double endPointFront[3];
	double endPointBack[3];
};

//-----------------------------------------------------------------------------

struct NWire
{
	Wire wires[3];
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

		SegmentationParameters::SegmentationParameters() :
			m_ThresholdImageTop( 10.0 ),
			m_ThresholdImageBottom( 10.0 ),

			m_MaxLineLenMm ( -1.0 ), 
			m_MinLineLenMm ( -1.0 ),
			m_MaxLinePairDistMm ( -1.0 ),
			m_MinLinePairDistMm ( -1.0 ),

			m_MaxLineLengthErrorPercent( 5.0 ),
			m_MaxLinePairDistanceErrorPercent( 10.0 ),
			m_MaxLineErrorMm ( 2.0 ),

			m_FindLines3PtDist ( 5.3f ),

			m_MaxAngleDiff ( 11.0 * M_PI / 180.0 ),
			m_MinTheta( 20.0 * M_PI / 180.0 ),
			m_MaxTheta( 160.0 * M_PI / 180.0 ),

			m_MaxUangleDiff( 10.0 * M_PI / 180.0 ),
			m_MaxUsideLineDiff (30), 
			m_MinUsideLineLength (280),//320
			m_MaxUsideLineLength (300),//350

			m_MorphologicalOpeningBarSizeMm(2.0), 
			m_MorphologicalOpeningCircleRadiusMm(0.55), 
			m_ScalingEstimation(0.2), 

			m_FiducialGeometry(CALIBRATION_PHANTOM_6_POINT),

			m_UseOriginalImageIntensityForDotIntensityScore (false) 
		{
			this->UpdateParameters(); 
		}

		void UpdateParameters()
		{
			// Create morphological circle
			m_MorphologicalCircle.clear(); 
			int radiuspx = floor((this->m_MorphologicalOpeningCircleRadiusMm / this->m_ScalingEstimation) + 0.5); 
			for ( int x = -radiuspx; x <= radiuspx; x++ )
			{
				for ( int y = -radiuspx; y <= radiuspx; y++ )
				{
					if ( sqrt( pow(x,2.0) + pow(y,2.0) ) <= radiuspx )
					{
						this->m_MorphologicalCircle.push_back( Item(x, y) ); 
					}
				}
			}
		}

		void SetUseOriginalImageIntensityForDotIntensityScore(bool value) { m_UseOriginalImageIntensityForDotIntensityScore = value; };
		bool GetUseOriginalImageIntensityForDotIntensityScore() { return m_UseOriginalImageIntensityForDotIntensityScore; };

		void SetThresholdImageTop(double value) { m_ThresholdImageTop = value; };
		double GetThresholdImageTop() { return m_ThresholdImageTop; };
		void SetThresholdImageBottom(double value) { m_ThresholdImageBottom = value; };
		double GetThresholdImageBottom() { return m_ThresholdImageBottom; };

		void SetMaxLineLengthErrorPercent(double value) { m_MaxLineLengthErrorPercent = value; };
		double GetMaxLineLengthErrorPercent() { return m_MaxLineLengthErrorPercent; };
		void SetMaxLinePairDistanceErrorPercent(double value) { m_MaxLinePairDistanceErrorPercent = value; };
		double GetMaxLinePairDistanceErrorPercent() { return m_MaxLinePairDistanceErrorPercent; };

		void SetMinLineLenMm(double value) { m_MinLineLenMm = value; };
		double GetMinLineLenMm() { return m_MinLineLenMm; };
		void SetMaxLineLenMm(double value) { m_MaxLineLenMm = value; };
		double GetMaxLineLenMm() { return m_MaxLineLenMm; };

		void SetMinLinePairDistMm(double value) { m_MinLinePairDistMm = value; };
		double GetMinLinePairDistMm() { return m_MinLinePairDistMm; };
		void SetMaxLinePairDistMm(double value) { m_MaxLinePairDistMm = value; };
		double GetMaxLinePairDistMm() { return m_MaxLinePairDistMm; };

		void SetMaxLineErrorMm(double value) { m_MaxLineErrorMm = value; };
		double GetMaxLineErrorMm() { return m_MaxLineErrorMm; };

		void SetFindLines3PtDist(double value) { m_FindLines3PtDist = value; };
		double GetFindLines3PtDist() { return m_FindLines3PtDist; };
		void SetMaxAngleDiff(double value) { m_MaxAngleDiff = value; };
		double GetMaxAngleDiff() { return m_MaxAngleDiff; };
		void SetMinTheta(double value) { m_MinTheta = value; };
		double GetMinTheta() { return m_MinTheta; };
		void SetMaxTheta(double value) { m_MaxTheta = value; };
		double GetMaxTheta() { return m_MaxTheta; };

		void SetMaxUangleDiff(double value) { m_MaxUangleDiff = value; };
		double GetMaxUangleDiff() { return m_MaxUangleDiff; };
		void SetMaxUsideLineDiff(double value) { m_MaxUsideLineDiff = value; };
		double GetMaxUsideLineDiff() { return m_MaxUsideLineDiff; };
		void SetMinUsideLineLength(double value) { m_MinUsideLineLength = value; };
		double GetMinUsideLineLength() { return m_MinUsideLineLength; };
		void SetMaxUsideLineLength(double value) { m_MaxUsideLineLength = value; };
		double GetMaxUsideLineLength() { return m_MaxUsideLineLength; };

		void SetMorphologicalOpeningBarSizeMm(double value) { m_MorphologicalOpeningBarSizeMm = value; };
		double GetMorphologicalOpeningBarSizeMm() { return m_MorphologicalOpeningBarSizeMm; };
		void SetMorphologicalOpeningCircleRadiusMm(double value) { m_MorphologicalOpeningCircleRadiusMm = value; };
		double GetMorphologicalOpeningCircleRadiusMm() { return m_MorphologicalOpeningCircleRadiusMm; };
		void SetScalingEstimation(double value) { m_ScalingEstimation = value; };
		double GetScalingEstimation() { return m_ScalingEstimation; };

		void SetFiducialGeometry(FiducialGeometryType value) { m_FiducialGeometry = value; };
		FiducialGeometryType GetFiducialGeometry() { return m_FiducialGeometry; };
		void SetNWires(std::vector<NWire> value) { m_NWires = value; };
		std::vector<NWire> GetNWires() { return m_NWires; };

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

class SegmentationResults
{
	public:
		SegmentationResults();
		
		void Clear();

		void SetDotsFound(bool value) { m_DotsFound = value; };
		bool GetDotsFound() { return m_DotsFound; };

		void SetFoundDotsCoordinateValue(std::vector< std::vector<double> > value) { m_FoundDotsCoordinateValue = value; };
		std::vector< std::vector<double> > GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };

		void SetAngles(float value) { m_Angles = value; };
		float GetAngles() { return m_Angles; };

		void SetIntensity(float value) { m_Intensity = value; };
		float GetIntensity() { return m_Intensity; };
		void SetNumDots(double value) { m_NumDots = value; };
		double GetNumDots() { return m_NumDots; };
		void SetCandidateFidValues(Dot * value) { m_CandidateFidValues = value; };
		Dot	* GetCandidateFidValues() { return m_CandidateFidValues; };
		
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
		Dot	*	m_CandidateFidValues; // pointer to the fiducial candidates coordinates
};

//-----------------------------------------------------------------------------

struct SegImpl
{	
	//public:
		void find_lines3pt();
		void find_lines2pt();
		void compute_line( Line &line, Dot *dots );
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
		void SegImpl::dynamicThresholding( PixelType *image);// addition to test dynamic thresholding
		inline void trypos( PixelType *image, int r, int c );
		void suppress( PixelType *image, float percent_thresh_top, float percent_thresh_bottom ); // a different threshold can be applied on the top and the bottom of the image
		void WritePossibleFiducialOverlayImage(Dot *fiducials, PixelType *unalteredImage); 

		void find_u_shape_line_triad(SegmentationResults &segResult);
		void find_double_n_lines(SegmentationResults &segResult);

		bool accept_line( const Line &line );  

		static void WritePng(PixelType *modifiedImage, std::string outImageName, int cols, int rows); // addition to write out intermediate files

		inline bool accept_dot( const Dot &dot );
			Dot * cluster();

		void find_lines();
		void find_pairs();

		void uscseg( PixelType *image,  const SegmentationParameters &segParams, SegmentationResults &segResult );

		void draw_dots( PixelType *image, Dot *dots, int ndots );
		void draw_lines( PixelType *image, Line *lines, int nlines );
		void draw_pair( PixelType *image, LinePair *pair );
		void print_results();
		void draw_results( PixelType *data );

		void sort_top_to_bottom( LinePair *pair );
		void sort_right_to_left( Line *line );

		int GetMorphologicalOpeningBarSizePx(); 

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
		Dot dots[MAX_DOTS];
		Line lines[MAX_LINES];
		LinePair pairs[MAX_PAIRS];
		int ndots, nlines, npairs;

		/* Cluster data. */
		Pos test[MAX_CLUSTER_VALS];
		Pos set[MAX_CLUSTER_VALS];
		PixelType vals[MAX_CLUSTER_VALS];
		int ntest, nset;

		/* Line finding. */
		Line lines2pt[MAX_LINES_2PT];
		int nlines2pt;
		std::vector< std::vector<Line> > uShapes;

		bool debugOutput; 
		std::string possibleFiducialsImageFilename;
};

//-----------------------------------------------------------------------------

/* 
 * Comparison functions used with generic sort.
 */

//-----------------------------------------------------------------------------

struct LtDotIntensity
{
	inline static bool lessThan( const Dot &dot1, const Dot &dot2 )
	{
		/* Use > to get descending. */
		return dot1.intensity > dot2.intensity;
	}
};

//-----------------------------------------------------------------------------

struct SortedAngle
{
	double angle;
	int pointIndex;
	double coords[2];
};

//-----------------------------------------------------------------------------

inline bool AngleMoreThan( const SortedAngle &pt1, const SortedAngle &pt2 )
{
	/* Use > to get descending. */
	return pt1.angle < pt2.angle;
};

//-----------------------------------------------------------------------------

struct LtVectorAngle
{
	inline static bool lessThan( const SortedAngle &pt1, const SortedAngle &pt2 )
	{
		/* Use > to get descending. */
		return pt1.angle > pt2.angle;
	}
};

//-----------------------------------------------------------------------------

struct CmpLine
{
	inline static long compare( const Line &line1, const Line &line2 )
	{
		for (int i=0; i<3; i++)
		{
			if ( line1.b[i] < line2.b[i] )
				return -1;
			else if ( line1.b[i] > line2.b[i] )
				return 1;
		}
		return 0;
	}
};

//-----------------------------------------------------------------------------

struct LtLineIntensity
{
	inline static bool lessThan( const Line &line1, const Line &line2 )
	{
		/* Use > to get descending. */
		return line1.intensity > line2.intensity;
	}
};

//-----------------------------------------------------------------------------

struct LtInt
{
	inline static bool lessThan( int i1, int i2 )
	{
		return i1 < i2;
	}
};

//-----------------------------------------------------------------------------

struct LtLinePairIntensity
{
	inline static bool lessThan( const LinePair &pair1, const LinePair &pair2 )
	{
		/* Use > to get descending. */
		return pair1.intensity > pair2.intensity;
	}
};

//-----------------------------------------------------------------------------

/* Used for discovering the correspondences. Sorts points left to right based
 * on their positions in the x axis. */
struct LtDotPosition
{
	inline static bool lessThan( Dot *b1, Dot *b2 )
	{
		/* Use > to get descending. */
		return b1->c > b2->c;
	}
};

//-----------------------------------------------------------------------------

template<class T, class LessThan> void doSort(T *tmpStor, T *data, long len)
{
	if ( len <= 1 )
		return;

	long mid = len / 2;

	doSort<T, LessThan>( tmpStor, data, mid );
	doSort<T, LessThan>( tmpStor + mid, data + mid, len - mid );
	
	/* Merge the data. */
	T *endLower = data + mid, *lower = data;
	T *endUpper = data + len, *upper = data + mid;
	T *dest = tmpStor;
	while ( true ) {
		if ( lower == endLower ) {
			/* Possibly upper left. */
			if ( upper != endUpper )
				memcpy( dest, upper, (endUpper - upper) * sizeof(T) );
			break;
		}
		else if ( upper == endUpper ) {
			/* Only lower left. */
			if ( lower != endLower )
				memcpy( dest, lower, (endLower - lower) * sizeof(T) );
			break;
		}
		else {
			/* Both upper and lower left. */
			if ( LessThan::lessThan( *upper, *lower ) )
				memcpy( dest++, upper++, sizeof(T) );
			else
				memcpy( dest++, lower++, sizeof(T) );
		}
	}

	/* Copy back from the tmpStor array. */
	memcpy( data, tmpStor, sizeof( T ) * len );
}

//-----------------------------------------------------------------------------

template<class T, class LessThan> void sort(T *data, long len)
{
	/* Allocate the tmp space needed by merge sort, sort and free. */

	// Thomas Kuiran Chen - retouched for ANSI-C++
	// ANSI-C++ does not allow an initialization of static array
	// data using a variable.  A walk-around is to use the dynamic
	// allocation of the array.
	//T tmpStor[len];
	T *tmpStor = new T[len];
	doSort<T, LessThan>( tmpStor, data, len );

	// Thomas Kuiran Chen - make sure to clean up the memory.
	delete [] tmpStor;
	tmpStor = NULL;
}

//-----------------------------------------------------------------------------

template <class T, class Compare> bool bs_find( const T &item, T *data, int dlen )
{
	T *lower = data;
	T *upper = data + dlen - 1;
	while ( true ) {
		if ( upper < lower )
			return false;

		T *mid = lower + ((upper-lower)>>1);
		long cmp = Compare::compare( item, *mid );
		if ( cmp < 0 )
			upper = mid - 1;
		else if ( cmp > 0 )
			lower = mid + 1;
		else
			return true;
	}
}

//-----------------------------------------------------------------------------

template <class T, class Compare> void bs_insert( const T &item, T *data, int &dlen )
{
	T *lower = data;
	T *upper = data + dlen - 1;
	while ( true ) {
		if ( upper < lower )
			goto insert;

		T *mid = lower + ((upper-lower)>>1);
		long cmp = Compare::compare( item, *mid );
		if ( cmp < 0 )
			upper = mid - 1;
		else if ( cmp > 0 )
			lower = mid + 1;
		else
			return;
	}
insert:
	int pos = lower - data;
	int newLen = dlen + 1;

	/* Shift over data at insert spot if needed. */
	if ( dlen > 0 && pos < dlen )
		memmove(data+pos+1, data+pos, sizeof(T)*(dlen-pos));

	/* Save the new length. */
	dlen = newLen;
	data[pos] = item;
}

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
		void segment( unsigned char *data,  const SegmentationParameters &segParams );

		/* Debugging routines: printing and drawing the results. */
		void printResults();
		void drawResults( unsigned char *data );
		void GetSegmentationResults(SegmentationResults &segResults); 
		
		static std::vector<std::vector<double>> sortInAscendingOrder(std::vector<std::vector<double>> fiducials);  

		void SetSizeX(int value) { m_SizeX = value; };
		int GetSizeX() { return m_SizeX; };
		void SetSizeY(int value) { m_SizeY = value; };
		int GetSizeY() { return m_SizeY; };

		void SetSearchOriginX(int value) { m_SearchOriginX = value; };
		int GetSearchOriginX() { return m_SearchOriginX; };
		void SetSearchOriginY(int value) { m_SearchOriginY = value; };
		int GetSearchOriginY() { return m_SearchOriginY; };

		void SetSearchSizeX(int value) { m_SearchSizeX = value; };
		int GetSearchSizeX() { return m_SearchSizeX; };
		void SetSearchSizeY(int value) { m_SearchSizeY = value; };
		int GetSearchSizeY() { return m_SearchSizeY; };

		void SetPossibleFiducialsImageFilename(std::string value) { m_PossibleFiducialsImageFilename = value; };
		std::string GetPossibleFiducialsImageFilename() { return m_PossibleFiducialsImageFilename; };

		void SetSegParams(SegmentationParameters value) { m_SegParams = value; };
		SegmentationParameters GetSegParams() { return m_SegParams; };
		void SetSegImpl(SegImpl	* value) { m_SegImpl = value; };
		SegImpl	* GetSegImpl() { return m_SegImpl; };
		void SetSegResult(SegmentationResults value) { m_SegResult = value; };
		SegmentationResults GetSegResult() { return m_SegResult; };

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

		//(3, vector<int>(2,0)); 
		// vector<double> m_FoundDotsYValue; 

		/*
		 * Private Data
		 */  
		
		SegmentationParameters	m_SegParams; 
		SegImpl	*				m_SegImpl;
		SegmentationResults		m_SegResult; 
};

#endif //_ULTRASOUND_FIDUCIAL_SEGMENTATION_H
