// This header is private to the segmentation code.
#include "PlusConfigure.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <math.h>
#include "KPhantSeg.h"
#ifndef _SEGIMPL_H
#define _SEGIMPL_H

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

typedef unsigned int uint;
typedef unsigned char uchar;

typedef unsigned char pixel;

struct Item
{
	Item(){ roff = 0; coff = 0; }; 
	Item( int r, int c) { roff = r; coff = c; }; 
	bool operator== (const Item& data) const { return (roff == data.roff && coff == data.coff) ; }
	int roff;
	int coff;
};

struct Pos
{
	int r;
	int c;
};

struct Dot
{
	float r;
	float c;
	float intensity;
};

struct Line
{
	float t; // slope of the line
	float p; // position of the line (distance of the line from the origin)
	int b[3]; // indices of points that make up the line
	float intensity;
	float line_error;
	float length;
};

struct LinePair
{
	int l1, l2;
	float intensity;
	float line_error;
	float angle_diff;
	float angle_conf;
};

struct Wire
{
	char name[128];
	double endPointFront[3];
	double endPointBack[3];
};

struct NWire
{
	Wire wires[3];
};

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
		mThresholdImageTop( 10.0 ),
		mThresholdImageBottom( 10.0 ),

		mMaxLineLenMm ( -1.0 ), 
		mMinLineLenMm ( -1.0 ),
		mMaxLinePairDistMm ( -1.0 ),
		mMinLinePairDistMm ( -1.0 ),

		mMaxLineLengthErrorPercent( 5.0 ),
		mMaxLinePairDistanceErrorPercent( 10.0 ),
		mMaxLineErrorMm ( 2.0 ),

		mFindLines3PtDist ( 5.3f ),

		mMaxAngleDiff ( 11.0 * M_PI / 180.0 ),
		mMinTheta( 20.0 * M_PI / 180.0 ),
		mMaxTheta( 160.0 * M_PI / 180.0 ),

		mMaxUangleDiff( 10.0 * M_PI / 180.0 ),
		mMaxUsideLineDiff (30), 
		mMinUsideLineLength (280),//320
		mMaxUsideLineLength (300),//350

		mMorphologicalOpeningBarSizeMm(2.0), 
		mMorphologicalOpeningCircleRadiusMm(0.55), 
		mScalingEstimation(0.2), 

		mFiducialGeometry(CALIBRATION_PHANTOM_6_POINT),

		mUseOriginalImageIntensityForDotIntensityScore (false) 
	{
		this->UpdateParameters(); 
	}

	void UpdateParameters()
	{
		// Create morphological circle
		mMorphologicalCircle.clear(); 
		int radiuspx = floor((this->mMorphologicalOpeningCircleRadiusMm / this->mScalingEstimation) + 0.5); 
		for ( int x = -radiuspx; x <= radiuspx; x++ )
		{
			for ( int y = -radiuspx; y <= radiuspx; y++ )
			{
				if ( sqrt( pow(x,2.0) + pow(y,2.0) ) <= radiuspx )
				{
					this->mMorphologicalCircle.push_back( Item(x, y) ); 
				}
			}
		}
	}
		
	bool mUseOriginalImageIntensityForDotIntensityScore;

	double mThresholdImageTop;  // segmentation threshold (in percentage, minimum is 0, maximum is 100 at the top half of the image
	double mThresholdImageBottom;  // segmentation threshold (in percentage, minimum is 0, maximum is 100 at the bottom half of the image

	// line length and line pair distance errors in percent - read from phantom definition
	double mMaxLineLengthErrorPercent;
	double mMaxLinePairDistanceErrorPercent;

	// min and max length of 3pt line (mm) - computed from input error percent and NWire definitions
	double mMinLineLenMm;
	double mMaxLineLenMm;

	// min and max distance between two detected parallel lines - computed from input error percent and NWire definitions
	// NOTE: This parameter should be adjusted w.r.t phantom design specs (e.g., distance between N-wires).
	double mMinLinePairDistMm; 	
	double mMaxLinePairDistMm;


	double mMaxLineErrorMm;

	double mFindLines3PtDist; 
	double mMaxAngleDiff; 
	double mMinTheta; 
	double mMaxTheta; 

	double mMaxUangleDiff; // max difference from 90 deg of angle betwen the side lines of the U and the baseline, in radians
	double mMaxUsideLineDiff; // max difference of the two side line endpoints, in pixels
	double mMinUsideLineLength; // min length of the U side line, in pixels
	double mMaxUsideLineLength; // max length of the U side line, in pixels

	double mMorphologicalOpeningBarSizeMm; 
	double mMorphologicalOpeningCircleRadiusMm; 
	double mScalingEstimation; 
	
	FiducialGeometryType mFiducialGeometry;
	std::vector<NWire> mNWires;
	
	std::vector<Item> mMorphologicalCircle; 
};

class SegmentationResults
{
public:SegmentationResults::SegmentationResults() :
		m_DotsFound( false ),
		m_Angles(-1),
		m_Intensity(-1),
		m_NumDots(0)
	{
	}
	
	void Clear() 
	{
		m_DotsFound = false;
		m_Angles = -1;
		m_Intensity = -1;
		m_FoundDotsCoordinateValue.clear();
		m_NumDots = 1;
		m_CandidateFidValues = NULL; 
	};
	
		/* True if the dots are found, false otherwise. */
	bool m_DotsFound;

			/* X and Y values of found dots. */
	//vector<vector<double>> m_FoundDotsCoordinateValue;
	std::vector< std::vector<double> > m_FoundDotsCoordinateValue; 
		/* The degree to which the lines are parallel and the dots linear.  On the
	 * range 0-1, with 0 being a very good angles score and 1 being the
	 * threshold of acceptability. */
	float m_Angles;

	/* The combined intensity of the six dots. This is the sum of the pixel
	 * values after the morphological operations, with the pixel values on the
	 * range 0-1.  A good intensity score is over 100. A bad one (but still
	 * valid) is below 25. */
	float m_Intensity;
	double m_NumDots; // number of possibel fiducial points
	Dot *m_CandidateFidValues; // pointer to the fiducial candidates coordinates
};

struct SegImpl
{	

	void find_lines3pt();
	void find_lines2pt();
	void compute_line( Line &line, Dot *dots );
	float compute_t( Dot *dot1, Dot *dot2 );

	inline pixel erode_point_0( pixel *image, uint ir, uint ic );
	void erode_0( pixel *dest, pixel *image );
	inline pixel erode_point_45( pixel *image, uint ir, uint ic );
	void erode_45( pixel *dest, pixel *image );
	inline pixel erode_point_90( pixel *image, uint ir, uint ic );
	void erode_90( pixel *dest, pixel *image );
	inline pixel erode_point_135( pixel *image, uint ir, uint ic );
	void erode_135( pixel *dest, pixel *image );
	void erode_circle( pixel *dest, pixel *image );
	inline pixel dilate_point_0( pixel *image, uint ir, uint ic );
	void dilate_0( pixel *dest, pixel *image );
	inline pixel dilate_point_45( pixel *image, uint ir, uint ic );
	void dilate_45( pixel *dest, pixel *image );
	inline pixel dilate_point_90( pixel *image, uint ir, uint ic );
	void dilate_90( pixel *dest, pixel *image );
	inline pixel dilate_point_135( pixel *image, uint ir, uint ic );
	void dilate_135( pixel *dest, pixel *image );
	inline pixel dilate_point( pixel *image, uint ir, uint ic, Item *shape, int slen );
	void dilate_circle( pixel *dest, pixel *image );
	void subtract( pixel *image, pixel *vals );
	void morphological_operations();	
	void setdebugOutput(bool on ); // addition to turn intermediate output on/off
	void setpossibleFiducialsImageFilename(int index); 
	void SegImpl::dynamicThresholding( pixel *image);// addition to test dynamic thresholding
	inline void trypos( pixel *image, int r, int c );
	void suppress( pixel *image, float percent_thresh_top, float percent_thresh_bottom ); // a different threshold can be applied on the top and the bottom of the image
	void WritePossibleFiducialOverlayImage(Dot *fiducials, pixel *unalteredImage); 

	void find_u_shape_line_triad(SegmentationResults &segResult);
	void find_double_n_lines(SegmentationResults &segResult);

	bool accept_line( const Line &line );  

	static void WritePng(pixel *modifiedImage, std::string outImageName, int cols, int rows); // addition to write out intermediate files

	inline bool accept_dot( const Dot &dot );
		Dot * cluster();

	void find_lines();
	void find_pairs();

	void uscseg( pixel *image,  const SegmentationParameters &segParams, SegmentationResults &segResult );

	void draw_dots( pixel *image, Dot *dots, int ndots );
	void draw_lines( pixel *image, Line *lines, int nlines );
	void draw_pair( pixel *image, LinePair *pair );
	void print_results();
	void draw_results( pixel *data );

	void sort_top_to_bottom( LinePair *pair );
	void sort_right_to_left( Line *line );

	int GetMorphologicalOpeningBarSizePx(); 

	SegmentationParameters m_SegParams;

	uint size, bytes;
	uint rows, cols;

	uint vertLow, horzLow;
	uint vertHigh, horzHigh;

	pixel *working;
	pixel *dilated;
	pixel *eroded;
	pixel *unalteredImage; 

	/* Dot, line and pair data. */
	Dot dots[MAX_DOTS];
	Line lines[MAX_LINES];
	LinePair pairs[MAX_PAIRS];
	int ndots, nlines, npairs;

	/* Cluster data. */
	Pos test[MAX_CLUSTER_VALS];
	Pos set[MAX_CLUSTER_VALS];
	pixel vals[MAX_CLUSTER_VALS];
	int ntest, nset;

	/* Line finding. */
	Line lines2pt[MAX_LINES_2PT];
	int nlines2pt;
	std::vector< std::vector<Line> > uShapes;

	bool debugOutput; 
	std::string possibleFiducialsImageFilename;
	
	 
};

#endif
