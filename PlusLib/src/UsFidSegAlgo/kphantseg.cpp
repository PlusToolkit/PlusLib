#include "PlusConfigure.h"

#include <climits>
// Thomas Kuiran Chen - ANSI-C++ headers
#include <cmath>
//#include <math.h>
#include <assert.h>

#include <strstream>
#include <iomanip>
#include <string> 
#include "sort.h"
#include "binsearch.h"
#include "segimpl.h"
#include "kphantseg.h"
#include "itkRGBPixel.h"
#include "itkPointSet.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h" 
#include "itkThresholdImageFilter.h"
const int BLACK = 0; 
const int WHITE = 255; 


/* Possible additional criteria:
 *  1. Track the frame-to-frame data?
 *  2. Lines should be roughly of the same length? */

void SegImpl::suppress( pixel *image, float percent_thresh_top, float percent_thresh_bottom )
{
	pixel max = 0;
	for ( uint pos = 0; pos < size; pos ++ ) {
		if ( image[pos] > max )
			max = image[pos];
	}

	pixel min = 255; 
	for ( uint currentPos = 0; currentPos < size; currentPos ++) 
	{ 
		if( image[currentPos] < min)
		{ 
			min = image[currentPos]; 
		} 
	} 

	// Thomas Kuiran Chen
	// NOTE: round/roundf are not ANSI C++ math functions. 
	//       We use floor to calculate the round value here.
	
	pixel thresh_top = min+(pixel)floor( (float)(max-min) * percent_thresh_top + 0.5 );
	pixel thresh_bottom = min+(pixel)floor( (float)(max-min) * percent_thresh_bottom + 0.5 );
	

	typedef unsigned char			PixelType; 
	const unsigned int				Dimension = 2; 
// copy array contents into an image container to be used with thresholding
	typedef itk::Image< PixelType, Dimension > ImageType; 
	ImageType::Pointer imagePointer = ImageType::New(); 
	ImageType::SizeType size; 
	size[0] = cols; 
	size[1] = rows; 

	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0; 

	ImageType::RegionType wholeImage; 
	wholeImage.SetSize(size);
	wholeImage.SetIndex(start); 

	imagePointer->SetRegions(wholeImage); 
	imagePointer->Allocate(); 

	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType iter(imagePointer, imagePointer->GetRequestedRegion() ); 
	iter.GoToBegin(); 

	int count = 0; 	

	while( !iter.IsAtEnd())
	{
	iter.Set(image[count]);
	count++; 
	++iter;

	}

	
// thresholding 
	

	typedef   unsigned char ThresholdPixelType;
	typedef itk::Image< ThresholdPixelType, 2 > ThresholdResultImageType;
	typedef itk::ThresholdImageFilter<ImageType> ThresholdFilterType; 

	ThresholdFilterType::Pointer thresholdFilterTop = ThresholdFilterType::New();
	thresholdFilterTop->SetInput(imagePointer); 
	thresholdFilterTop->SetOutsideValue( BLACK);
	thresholdFilterTop->ThresholdBelow( thresh_top ); // 13
	thresholdFilterTop->Update(); 
	ImageType::Pointer filteredImageTop = thresholdFilterTop->GetOutput(); 

	ThresholdFilterType::Pointer thresholdFilterBottom = ThresholdFilterType::New();
	thresholdFilterBottom->SetInput(imagePointer); 
	thresholdFilterBottom->SetOutsideValue( BLACK);
	thresholdFilterBottom->ThresholdBelow( thresh_bottom ); // 13
	thresholdFilterBottom->Update(); 
	ImageType::Pointer filteredImageBottom = thresholdFilterBottom->GetOutput(); 	

	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType resultingImageIteratorTop(filteredImageTop, filteredImageTop->GetRequestedRegion() ); 
	resultingImageIteratorTop.GoToBegin(); 
	IterType resultingImageIteratorBottom(filteredImageBottom, filteredImageBottom->GetRequestedRegion() ); 
	resultingImageIteratorBottom.GoToBegin(); 

	int counter = 0; 	

	while( !resultingImageIteratorTop.IsAtEnd())
	{
		int lineIndex=counter/cols;
		if (lineIndex<rows/2)
		{
			// top half of the image
			image[counter] = resultingImageIteratorTop.Get();
		}
		else
		{
			// bottom half of the image
			image[counter] = resultingImageIteratorBottom.Get();
		}
		counter++; 
		++resultingImageIteratorTop;
		++resultingImageIteratorBottom;

	}
	
}

/* 
 * Comparison functions used with generic sort.
 */

struct LtDotIntensity
{
	inline static bool lessThan( const Dot &dot1, const Dot &dot2 )
	{
		/* Use > to get descending. */
		return dot1.intensity > dot2.intensity;
	}
};

struct SortedAngle
{
	double angle;
	int pointIndex;
	double coords[2];
};

bool AngleMoreThan( const SortedAngle &pt1, const SortedAngle &pt2 )
{
	/* Use > to get descending. */
	return pt1.angle < pt2.angle;
}

struct LtVectorAngle
{
	inline static bool lessThan( const SortedAngle &pt1, const SortedAngle &pt2 )
	{
		/* Use > to get descending. */
		return pt1.angle > pt2.angle;
	}
};

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

struct LtLineIntensity
{
	inline static bool lessThan( const Line &line1, const Line &line2 )
	{
		/* Use > to get descending. */
		return line1.intensity > line2.intensity;
	}
};

struct LtInt
{
	inline static bool lessThan( int i1, int i2 )
	{
		return i1 < i2;
	}
};

struct LtLinePairIntensity
{
	inline static bool lessThan( const LinePair &pair1, const LinePair &pair2 )
	{
		/* Use > to get descending. */
		return pair1.intensity > pair2.intensity;
	}
};

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


/* Should we accept a dot? */
inline bool SegImpl::accept_dot( const Dot &dot )
{
	int left = horzLow + MIN_WINDOW_DIST;
	int right = horzHigh - MIN_WINDOW_DIST;
	int top = vertLow + MIN_WINDOW_DIST;
	int bot = vertHigh - MIN_WINDOW_DIST;

	if ( dot.r >= top && dot.r < bot && dot.c >= left && dot.c < right )
		return true;
	return false;
}

inline void SegImpl::trypos( pixel *image, int r, int c )
{
	if ( image[r*cols+c] > 0 && ntest < MAX_CLUSTER_VALS && 
			nset < MAX_CLUSTER_VALS )
	{
		test[ntest].r = r, test[ntest].c = c;
		ntest += 1;

		set[nset].r = r, set[nset].c = c;
		vals[nset] = image[r*cols+c];
		nset += 1;
		image[r*cols+c] = 0;
	}
}


Dot * SegImpl::cluster()
{

	Dot dot;

	for ( uint r = vertLow; r < vertHigh; r++ ) {
		for ( uint c = horzLow; c < horzHigh; c++ ) {
			if ( working[r*cols+c] > 0 ) {

				test[0].r = r, test[0].c = c;
				ntest = 1;

				set[0].r = r, set[0].c = c;
				vals[0] = working[r*cols+c];
				nset = 1;
				working[r*cols+c] = 0;

				while ( ntest > 0 ) {
					ntest -= 1;
					int tr = test[ntest].r;
					int tc = test[ntest].c;

					trypos( working, tr-1, tc-1 );
					trypos( working, tr-1, tc );
					trypos( working, tr-1, tc+1 );

					trypos( working, tr, tc-1 );
					trypos( working, tr, tc+1 );

					trypos( working, tr+1, tc-1 );
					trypos( working, tr+1, tc );
					trypos( working, tr+1, tc+1 );
				}

				float dest_r = 0, dest_c = 0, total = 0;
				for ( int p = 0; p < nset; p++ ) {
					float amount = (float)vals[p] / (float)UCHAR_MAX;
					dest_r += set[p].r * amount;
					dest_c += set[p].c * amount;
					total += amount;
				}

				dot.r = dest_r / total;
				dot.c = dest_c / total;
				dot.intensity = total;

				if ( accept_dot( dot ) )
				{
					if (m_SegParams.mUseOriginalImageIntensityForDotIntensityScore)
					{
						// Take into account intensities that are close to the dot center
						const double dotRadius2=3.0*3.0;
						float dest_r = 0, dest_c = 0, total = 0;
						for ( int p = 0; p < nset; p++ ) {
							if ( (set[p].r-dot.r)*(set[p].r-dot.r)+(set[p].c-dot.c)*(set[p].c-dot.c)<=dotRadius2)
							{
								//float amount = (float)vals[p] / (float)UCHAR_MAX;
								float amount = (float)unalteredImage[set[p].r*cols+set[p].c] / (float)UCHAR_MAX;
								dest_r += set[p].r * amount;
								dest_c += set[p].c * amount;
								total += amount;
							}
						}
						dot.intensity = total;
					}

					dots[ndots++] = dot;
				}

				if ( ndots == MAX_DOTS )
					goto done;
			}
		}
	}

done:
	sort<Dot, LtDotIntensity>( dots, ndots );
	return dots; 

	
}

float SegImpl::compute_t( Dot *dot1, Dot *dot2 )
{
	float x1 = dot1->c - 1;
	float y1 = rows - dot1->r;

	float x2 = dot2->c - 1;
	float y2 = rows - dot2->r;

	float y = (y2 - y1);
	float x = (x2 - x1);

	// :TODO: check if it can be simplified by using atan2 instead of atan+fabsf

	float t;
	if ( fabsf(x) > fabsf(y) )
		t = M_PI/2 + atan( y / x );
	else {
		float tanTheta = x / y;
		if ( tanTheta > 0 )
			t = M_PI - atan( tanTheta );
		else
			t = -atan( tanTheta );
	}
	assert( t >= 0 && t <= M_PI );
	return t;
}

float segment_length( Dot *d1, Dot *d2 )
{
	float xd = d2->c - d1->c;
	float yd = d2->r - d1->r;
	return sqrtf( xd*xd + yd*yd );
}

float line_length( const Line &line, Dot *dots )
{
	float l1 = segment_length( &dots[line.b[0]], &dots[line.b[1]] );
	float l2 = segment_length( &dots[line.b[0]], &dots[line.b[2]] );
	float l3 = segment_length( &dots[line.b[1]], &dots[line.b[2]] );

	if ( l2 > l1 )
		l1 = l2;
	return l3 > l1 ? l3 : l1;
}

void SegImpl::compute_line( Line &line, Dot *dots )
{
	int ptnum[3];
	for (int i=0; i<3; i++)
	{
		ptnum[i] = line.b[i];
	}

	float t[3];
	t[0] = compute_t( &dots[ptnum[0]], &dots[ptnum[1]] );
	t[1] = compute_t( &dots[ptnum[0]], &dots[ptnum[2]] );
	t[2] = compute_t( &dots[ptnum[1]], &dots[ptnum[2]] );

	while ( t[1] - t[0] > M_PI/2 )
		t[1] -= M_PI;
	while ( t[0] - t[1] > M_PI/2 )
		t[1] += M_PI;

	while ( t[2] - t[0] > M_PI/2 )
		t[2] -= M_PI;
	while ( t[0] - t[2] > M_PI/2 )
		t[2] += M_PI;
		
	float tMean = ( t[0] + t[1] + t[2] ) / 3;

	while ( tMean >= M_PI )
		tMean = tMean - M_PI;
	while ( t < 0 )
		tMean = tMean + M_PI;
	
	line.t = tMean;
	line.p = 0;

	float x[3];
	float y[3];
	float p[3];
	
	for (int i=0; i<3; i++)
	{
		x[i] = dots[ptnum[i]].c  - 1;
		y[i] = rows - dots[ptnum[i]].r;
		p[i] = x[i] * cos(tMean) + y[i] * sin(tMean);
	}

	float pMean = (p[0] + p[1] + p[2]) / 3;
	line.p = pMean;
	line.intensity = dots[ptnum[0]].intensity + dots[ptnum[1]].intensity + 
			dots[ptnum[2]].intensity;
	line.line_error = fabsf(p[0]-pMean) + fabsf(p[1]-pMean) + fabsf(p[2]-pMean);
	line.length = line_length( line, dots );
}

void SegImpl::find_lines2pt()
{
	for ( int b1 = 0; b1 < ndots; b1++ ) {
		float x1 = dots[b1].c - 1;
		float y1 = rows - dots[b1].r;

		for ( int b2 = b1+1; b2 < ndots; b2++ ) {
			float x2 = dots[b2].c - 1;
			float y2 = rows - dots[b2].r;

			float t = compute_t( &dots[b1], &dots[b2] ); // get degree of slope (0 deg = parallel to -y axis)

			float p1 = x1 * cos(t) + y1 * sin(t);
			float p2 = x2 * cos(t) + y2 * sin(t);
			float p = (p1 + p2) / 2;

			lines2pt[nlines2pt].t = t; 
			lines2pt[nlines2pt].p = p;
			lines2pt[nlines2pt].b[0] = b1; 
			lines2pt[nlines2pt].b[1] = b2;
			nlines2pt += 1;

			if ( nlines2pt == MAX_LINES_2PT )
				return;
		}
	}
}

bool SegImpl::accept_line( const Line &line )
{
	int maxLineLenPx = floor(m_SegParams.mMaxLineLenMm / m_SegParams.mScalingEstimation + 0.5 );
	int minLineLenPx = floor(m_SegParams.mMinLineLenMm / m_SegParams.mScalingEstimation + 0.5 );
	double maxLineErrorPx = m_SegParams.mMaxLineErrorMm / m_SegParams.mScalingEstimation;

	if ( line.length <= maxLineLenPx && line.length >= minLineLenPx &&
			line.line_error <= maxLineErrorPx && 
			line.t >= m_SegParams.mMinTheta && line.t <= m_SegParams.mMaxTheta )
		return true;
	return false;
}

void SegImpl::find_lines3pt( )
{
	/* For each point, loop over each 2-point line and try to make a 3-point
	 * line. For the third point use the theta of the line and compute a value
	 * for p. Accept the line if the compute p is within some small distance
	 * of the 2-point line. */

	Line line;
	int points[3];
	float dist = m_SegParams.mFindLines3PtDist;
	for ( int b3 = 0; b3 < ndots; b3++ ) {
		float x3 = dots[b3].c - 1;
		float y3 = rows - dots[b3].r;

		for ( int l = 0; l < nlines2pt; l++ ) {
			float t = lines2pt[l].t;
			float p = lines2pt[l].p;
			int b1 = lines2pt[l].b[0];
			int b2 = lines2pt[l].b[1];

			if ( b3 != b1 && b3 != b2 ) {
				float pb3 = x3 * cos(t) + y3 * sin(t);
				if ( fabsf( p - pb3 ) <= dist ) {
					lines[nlines].t = 0;
					lines[nlines].p = 0;

					/* To find unique lines, each line must have a uniqe
					 * configuration of three points. */
					points[0] = b1;
					points[1] = b2;
					points[2] = b3;
					sort<int, LtInt>(points, 3);

					for (int i=0; i<3; i++)
					{
						line.b[i] = points[i];
					}					

					if ( ! bs_find<Line, CmpLine>( line, lines, nlines ) ) {
						compute_line( line, dots );
						if ( accept_line( line ) ) {
							bs_insert<Line, CmpLine>( line, lines, nlines );
							if ( nlines == MAX_LINES )
								return;
						}
					}

				}
			}
		}
	}
}

void SegImpl::find_lines( )
{
	/* Make pairs of dots into 2-point lines. */
	find_lines2pt();

	/* Make 2-point lines and dots into 3-point lines. */
	find_lines3pt();

	/* Sort by intensity. */
	sort<Line, LtLineIntensity>( lines, nlines );
}

void SegImpl::find_pairs()
{
	int maxLinePairDistPx = floor(m_SegParams.mMaxLinePairDistMm / m_SegParams.mScalingEstimation + 0.5 );
	int minLinePairDistPx = floor(m_SegParams.mMinLinePairDistMm / m_SegParams.mScalingEstimation + 0.5 );
	double maxLineErrorPx = m_SegParams.mMaxLineErrorMm / m_SegParams.mScalingEstimation;

	for ( int l1 = 0; l1 < nlines; l1++ ) {
		for ( int l2 = l1+1; l2 < nlines; l2++ ) {
			float t1 = lines[l1].t;
			float p1 = lines[l1].p;

			float t2 = lines[l2].t;
			float p2 = lines[l2].p;

			float angle_diff = fabsf( t2 - t1 );
			float line_error = lines[l1].line_error + lines[l2].line_error;

			bool test1 = angle_diff < m_SegParams.mMaxAngleDiff;
			bool test2 = line_error < maxLineErrorPx;
			bool test3 = m_SegParams.mMinTheta <= t1 && t1 <= m_SegParams.mMaxTheta;
			bool test4 = m_SegParams.mMinTheta <= t2 && t2 <= m_SegParams.mMaxTheta;
			// (old value was hardcoded as 65)
			bool test5 = fabsf( p2 - p1 ) < maxLinePairDistPx;  
			// (old value was hardcoded as 25)
			bool test6 = fabsf( p2 - p1 ) > minLinePairDistPx;

			if ( test1 && test2 && test3 && test4 && test5 && test6 ) {
				line_error = line_error / maxLineErrorPx;
				float angle_conf = angle_diff / ( 1 - line_error );

				if ( angle_conf < m_SegParams.mMaxAngleDiff ) {
					float intensity = lines[l1].intensity + lines[l2].intensity;

					pairs[npairs].l1 = l1;
					pairs[npairs].l2 = l2;
					pairs[npairs].intensity = intensity;
					pairs[npairs].line_error = line_error;
					pairs[npairs].angle_diff = angle_diff / m_SegParams.mMaxAngleDiff;
					pairs[npairs].angle_conf = angle_conf / m_SegParams.mMaxAngleDiff;

					npairs += 1;
					if ( npairs == MAX_PAIRS )
						goto done;
				}
			}
		}
	}

done:
	/* Rank the pairs by intensity. */
	sort<LinePair, LtLinePairIntensity>( pairs, npairs );
}

void SegImpl::sort_top_to_bottom( LinePair *pair )
{
	/* check if we need to swap the pairs. */
	if ( lines[pair->l1].p < lines[pair->l2].p ) {
		int swp_tmp = pair->l1;
		pair->l1 = pair->l2;
		pair->l2 = swp_tmp;
	}
}

void SegImpl::sort_right_to_left( Line *line )
{
	/* Since we prohibit stepp lines (see MAX_T and MIN_T) we can use the x
	 * values to sort the points. */
	Dot *points[3];
	for (int i=0; i<3; i++)
	{
		points[i] = &dots[line->b[i]];
	}
	sort<Dot*, LtDotPosition>( points, 3 );
	for (int i=0; i<3; i++)
	{
		line->b[i] = points[i] - dots;
	}
}
void SegImpl::WritePossibleFiducialOverlayImage(Dot *fiducials, pixel *unalteredImage)
{
	typedef itk::RGBPixel< unsigned char >    PixelType;
	typedef itk::Image< PixelType, 2 >   ImageType;

	ImageType::Pointer possibleFiducials = ImageType::New(); 
	
	ImageType::SizeType size;
	size[0] = cols;
	size[1] = rows; 

	ImageType::IndexType start; 
	start[0] = 0; 
	start[1] = 0; 

	ImageType::RegionType wholeImage; 
	wholeImage.SetSize(size);
	wholeImage.SetIndex(start); 

	possibleFiducials->SetRegions(wholeImage); 
	possibleFiducials->Allocate(); 

	ImageType::IndexType pixelLocation={0,0};

	ImageType::PixelType pixelValue; 

	// copy pixel by pixel (we need to do gray->RGB conversion and only a ROI is updated)
	for ( uint r = vertLow; r < vertHigh; r++ ) 
	{
		for ( uint c = horzLow; c < horzHigh; c++ ) 
		{
			pixelValue[0] = 0; //unalteredImage[r*cols+c];
			pixelValue[1] = unalteredImage[r*cols+c];
			pixelValue[2] = unalteredImage[r*cols+c];
			pixelLocation[0]= c;
			pixelLocation[1]= r; 
			possibleFiducials->SetPixel(pixelLocation,pixelValue);
		}
	}

	// Set pixelValue to red (it will be used to mark the centroid of the clusters)
	for(int numDots=0; numDots<ndots; numDots++)
	{
		const int markerPosCount=5;
		const int markerPos[markerPosCount][2]={{0,0}, {+1,0}, {-1,0}, {0,+1}, {0,-1}};

		for (int i=0; i<markerPosCount; i++)
		{
			pixelLocation[0]= fiducials[numDots].c+markerPos[i][0];
			pixelLocation[1]= fiducials[numDots].r+markerPos[i][1]; 
			int clusterMarkerIntensity=fiducials[numDots].intensity*10;
			if (clusterMarkerIntensity>255)
			{
				clusterMarkerIntensity=255;
			}
			pixelValue[0] = clusterMarkerIntensity;
			pixelValue[1] = 0;
			pixelValue[2] = 0;
			possibleFiducials->SetPixel(pixelLocation,pixelValue); 
		}
	}
	/*std::ostrstream possibleFiducialsImageFilename; 
	possibleFiducialsImageFilename << "possibleFiducials" << std::setw(3) << std::setfill('0') << currentFrameIndex << ".bmp" << std::ends; 
	
	const char *test=possibleFiducialsImageFilename.str();
	*/ 

	//std::string possibleFiducialsImageFilename = "possibleFiducials" + currentIndex.str() + ".bmp"; 

	typedef itk::ImageFileWriter< ImageType > WriterType; 
	WriterType::Pointer writeImage = WriterType::New();  
	writeImage->SetFileName(possibleFiducialsImageFilename);  
	// possibleFiducialsImageFilename.rdbuf()->freeze();

	writeImage->SetInput( possibleFiducials );  
		try
	{
		writeImage->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		std::cerr << " Exception! writer did not update" << std::endl; 
		std::cerr << err << std ::endl; 
		//return EXIT_GENERIC_FAILURE;
	}
}


void SegImpl::uscseg( pixel *image, const SegmentationParameters &segParams, SegmentationResults &segResult )
{
	ndots = nlines = npairs = 0;
	ntest = nset = nlines2pt = 0;
	m_SegParams = segParams; 

	memcpy( working, image, bytes );
	memcpy( unalteredImage, image, bytes); 

	morphological_operations();

	suppress( working, m_SegParams.mThresholdImageTop/100.00, m_SegParams.mThresholdImageBottom/100.00 );

	cluster();

	segResult.m_NumDots= ndots; 
	segResult.m_CandidateFidValues=dots;	  
	 
	if(debugOutput) 
	{
		WritePossibleFiducialOverlayImage(segResult.m_CandidateFidValues, unalteredImage); 
	}
	
	find_lines();

	switch (m_SegParams.mFiducialGeometry)
	{
	case SegmentationParameters::TAB2_6_POINT:
		LOG_WARNING("Unsupported phantom geometry - skipped"); 
		break;
	case SegmentationParameters::CALIBRATION_PHANTOM_6_POINT:
		find_double_n_lines(segResult);
		break;
	case SegmentationParameters::TAB2_5_POINT:
		find_u_shape_line_triad(segResult); 
		break;
	default:
		LOG_ERROR("Segmentation error: invalid phantom geometry identifier!"); 
		break;
	}
}


void SegImpl::find_double_n_lines(SegmentationResults &segResult)
{	
	find_pairs();
	if ( npairs > 0 ) {
		LinePair *pair = pairs;
		sort_top_to_bottom( pair );

		Line *line1 = &lines[pair->l1];
		Line *line2 = &lines[pair->l2];

		sort_right_to_left( line1 );
		sort_right_to_left( line2 );
	}
	else if ( npairs < 1 ) 
	{
		LOG_DEBUG("Segmentation was NOT successful! (Number of dots found: " << segResult.m_FoundDotsCoordinateValue.size() << " Number of possible fiducial points: " << segResult.m_NumDots << ")"); 
		segResult.m_DotsFound = false;				
		return;
	}

	segResult.m_DotsFound = true;

	LinePair *pair = pairs;
	Line *line1 = &lines[pair->l1];
	Line *line2 = &lines[pair->l2];

	std::vector<double> dotCoords;
	for (int i=0; i<3; i++)
	{
		dotCoords.push_back(dots[line1->b[i]].c);
		dotCoords.push_back(dots[line1->b[i]].r);
		segResult.m_FoundDotsCoordinateValue.push_back(dotCoords);
		dotCoords.clear();
	}
	for (int i=0; i<3; i++)
	{
		dotCoords.push_back(dots[line2->b[i]].c);
		dotCoords.push_back(dots[line2->b[i]].r);
		segResult.m_FoundDotsCoordinateValue.push_back(dotCoords);
		dotCoords.clear();
	}
	std::vector<std::vector<double>> sortedFiducials = KPhantomSeg::sortInAscendingOrder(segResult.m_FoundDotsCoordinateValue); 
	segResult.m_FoundDotsCoordinateValue = sortedFiducials; 					

	segResult.m_Angles = pair->angle_conf;
	segResult.m_Intensity = pair->intensity;
	segResult.m_NumDots= ndots; 
}


void SegImpl::find_u_shape_line_triad(SegmentationResults &segResult)
{	
	int threePointLineRank = 0; 
	bool fiducialsFound= false;
	
	// needed after loop
	std::vector<std::vector<Dot>> twoFiducialCombos; // pair of U side line endpoint candidate dots
	std::vector<vnl_vector<double>> threePointLine(3); // position of the 3 points
	
	const int EXPECTED_FIDUCIAL_COUNT=5;
	if(ndots<EXPECTED_FIDUCIAL_COUNT) 
	{
		// it is impossible to find all the fiducials, because there are not enough candidates
		return;
	}

	const double MINIMUM_LINE_INTENSITY=1.0;
	if(lines[0].intensity<MINIMUM_LINE_INTENSITY)
	{
		// even the brightest line is too dark, the segmentation failed
		return;
	} 

	while(fiducialsFound == false) 
	{ 


		for(int linePointPosition=0; linePointPosition<3; linePointPosition++)// three for three points
		{
			vnl_vector<double> pt(3);
			pt(0) = dots[lines[threePointLineRank].b[linePointPosition]].c; // first three point line =  line with fiducials with greatest intensity 
			pt(1) = dots[lines[threePointLineRank].b[linePointPosition]].r;
			pt(2) = 1; // position
			threePointLine[linePointPosition]=pt;
		}

		vnl_vector<double> normal(3); // average normal of the line segments connecting the 3 points
		normal(0)=(
			( threePointLine[0](1)-threePointLine[1](1) ) +
			( threePointLine[0](1)-threePointLine[2](1) ) +
			( threePointLine[1](1)-threePointLine[2](1) )
			)/3.0;
		normal(1)=-(
			( threePointLine[0](0)-threePointLine[1](0) ) +
			( threePointLine[0](0)-threePointLine[2](0) ) +
			( threePointLine[1](0)-threePointLine[2](0) )
			)/3.0;
		normal(2)=0; // vector
		normal.normalize();

		int ptAindex=0; // index of one of the endpoints
		int ptBindex=0; // index of the other endpoint
		int d01=(threePointLine[1]-threePointLine[0]).magnitude();
		int d02=(threePointLine[2]-threePointLine[0]).magnitude();
		int d12=(threePointLine[2]-threePointLine[1]).magnitude();
		if (d01>d02 && d01>d12)
		{
			// d01 is the longest distance, min&max are 0 and 1
			ptAindex=0;
			ptBindex=1;
		}
		else if (d02>d01 && d02>d12)
		{
			// d02 is the longest distance, min&max are 0 and 2
			ptAindex=0;
			ptBindex=2;
		}
		else
		{
			// d12 is the longest distance, min&max are 1 and 2
			ptAindex=1;
			ptBindex=2;
		}	

		std::vector<int> potentialAvectors;
		std::vector<int> potentialBvectors;	
		for(int dotPosition=0; dotPosition<ndots; dotPosition++)
		{	
			vnl_vector<double> vectorFromCurrentDot(3);
			vectorFromCurrentDot(0) = (dots[dotPosition].c - threePointLine[ptAindex](0)); 
			vectorFromCurrentDot(1) = (dots[dotPosition].r - threePointLine[ptAindex](1));
			vectorFromCurrentDot(2) = 0;
			if (vectorFromCurrentDot.magnitude()>m_SegParams.mMinUsideLineLength &&
				vectorFromCurrentDot.magnitude()<m_SegParams.mMaxUsideLineLength)
			{
				vectorFromCurrentDot.normalize();
				double angleDiff=acos(dot_product(normal, vectorFromCurrentDot));
				if (fabs(angleDiff)<m_SegParams.mMaxUangleDiff
					|| fabs(M_PI-angleDiff) <m_SegParams.mMaxUangleDiff )
				{
					// this may be sideline of the U
					potentialAvectors.push_back(dotPosition); 
				}
			}

			vectorFromCurrentDot(0) = (dots[dotPosition].c - threePointLine[ptBindex](0)); 
			vectorFromCurrentDot(1) = (dots[dotPosition].r - threePointLine[ptBindex](1));
			vectorFromCurrentDot(2) = 0;
			if (vectorFromCurrentDot.magnitude()>m_SegParams.mMinUsideLineLength &&
				vectorFromCurrentDot.magnitude()<m_SegParams.mMaxUsideLineLength)
			{
				vectorFromCurrentDot.normalize();		
				double angleDiff=acos(dot_product(normal, vectorFromCurrentDot));
				if (fabs(angleDiff)<m_SegParams.mMaxUangleDiff
					|| fabs(M_PI-angleDiff) <m_SegParams.mMaxUangleDiff )
				{
					// this may be sideline of the U
					potentialBvectors.push_back(dotPosition); 
				}
			}
		} 	


		for( int aVecCandidate = 0; aVecCandidate < potentialAvectors.size(); aVecCandidate++)
		{
			int aIndex = potentialAvectors[aVecCandidate]; 		
			vnl_vector<double> aVec(3); // side line A vector
			aVec(0) = (dots[aIndex].c - threePointLine[ptAindex](0)); 
			aVec(1) = (dots[aIndex].r - threePointLine[ptAindex](1)); 
			aVec(2) = 1;
			for(int bVecCandidate = 0; bVecCandidate< potentialBvectors.size(); bVecCandidate++)
			{
				int bIndex = potentialBvectors[bVecCandidate]; 
				vnl_vector<double> bVec(3); // side line B vector
				bVec(0) = (dots[bIndex].c - threePointLine[ptBindex](0)); 
				bVec(1) = (dots[bIndex].r - threePointLine[ptBindex](1)); 
				bVec(2) = 1;

				if( (aVec-bVec).magnitude()<m_SegParams.mMaxUsideLineDiff)
				{
					std::vector<Dot> twoFiducials;
					twoFiducials.push_back(dots[aIndex]);
					twoFiducials.push_back(dots[bIndex]); 
					twoFiducialCombos.push_back(twoFiducials); 
				} 
			}
		}

		if (twoFiducialCombos.size()>=1)
		{
			// we've found a fiducial combo (pair of side lines)
			fiducialsFound = true; 
		}
		else if (threePointLineRank < nlines-1)
		{
			// segmentation failed with this line, but there are other lines to try
			threePointLineRank++;
		}		
		else
		{
			// no more lines left to check, and still haven't found a combo
			return; 
		}

	}// end looping through three point lines

	// Bubble sorting for the fiducial combos, based on average intensity 
	std::vector<Dot> temp;             // holding variable
	int fidCombolength = twoFiducialCombos.size(); 
	int flag = 1;    // set flag to 1 to start first pass
	for( int i = 1; (i <= twoFiducialCombos.size()) && flag; i++)
	{
		flag = 0;
		for (int j=0; j < (twoFiducialCombos.size() -1); j++)
		{
			double averageIntensityCurrent = (twoFiducialCombos[j][0].intensity +  twoFiducialCombos[j][1].intensity)/2;
			double averageIntensityNext = (twoFiducialCombos[j+1][0].intensity +  twoFiducialCombos[j+1][1].intensity)/2;
			if (averageIntensityNext > averageIntensityCurrent)      // ascending order simply changes to <
			{ 
				temp = twoFiducialCombos[j];             // swap elements
				twoFiducialCombos[j] = twoFiducialCombos[j+1];
				twoFiducialCombos[j+1] = temp;
				flag = 1;               // indicates that a swap occurred.
			}
		}
	}

	// sort<std::vector<std::vector<Dot>>, LtLinePairIntensity>( pairs, npairs );

	std::vector<Dot> bestFiducialCombo=twoFiducialCombos[0]; // :TODO: check if the first combo is the best	

	segResult.m_DotsFound=true;

	std::vector<double> coords(2);

	// Side line A top
	coords[0]=bestFiducialCombo[0].c;
	coords[1]=bestFiducialCombo[0].r;
	segResult.m_FoundDotsCoordinateValue.push_back(coords);

	// Baseline	
	coords[0]=threePointLine[0](0);
	coords[1]=threePointLine[0](1);
	segResult.m_FoundDotsCoordinateValue.push_back(coords);
	coords[0]=threePointLine[1](0);
	coords[1]=threePointLine[1](1);
	segResult.m_FoundDotsCoordinateValue.push_back(coords);
	coords[0]=threePointLine[2](0);
	coords[1]=threePointLine[2](1);
	segResult.m_FoundDotsCoordinateValue.push_back(coords);

	// Side line B top
	coords[0]=bestFiducialCombo[1].c;
	coords[1]=bestFiducialCombo[1].r;
	segResult.m_FoundDotsCoordinateValue.push_back(coords);


	// Sort the fiducials based on their location around the center of gravity the fiducial set
	// (ascending order of the arctan of the COG->FidPoint vector)
	std::vector<double> centerPoint(2);
	centerPoint[0]=0.0;
	centerPoint[1]=0.0;
	for(int i =0;i<segResult.m_FoundDotsCoordinateValue.size();i++)
	{
		centerPoint[0] += segResult.m_FoundDotsCoordinateValue[i][0];
		centerPoint[1] +=segResult.m_FoundDotsCoordinateValue [i][1]; 
	}
	int pointCount=segResult.m_FoundDotsCoordinateValue.size();
	centerPoint[0] = centerPoint[0]/pointCount; 
	centerPoint[1] = centerPoint[1]/pointCount;
	std::list<SortedAngle> sortedAngles;
	for(int i =0;i<segResult.m_FoundDotsCoordinateValue.size();i++)
	{		
		SortedAngle sa;
		sa.pointIndex=i;
		sa.coords[0]=segResult.m_FoundDotsCoordinateValue[i][0];
		sa.coords[1]=segResult.m_FoundDotsCoordinateValue[i][1];
		double directionVectorX = sa.coords[0] - centerPoint[0];
		double directionVectorY = sa.coords[1] - centerPoint[1];
		// angle=0 corresponds to -Y direction
		double angle=atan2(directionVectorX, directionVectorY);
		sa.angle=angle;		
		sortedAngles.push_back(sa);
	}
	sortedAngles.sort(AngleMoreThan);
	segResult.m_FoundDotsCoordinateValue.clear();
	for(std::list<SortedAngle>::iterator it=sortedAngles.begin();it!=sortedAngles.end();it++)
	{
		SortedAngle sa=(*it);
		std::vector<double> coord(2);
		coord[0]=sa.coords[0];
		coord[1]=sa.coords[1];
		segResult.m_FoundDotsCoordinateValue.push_back(coord);
	}


	segResult.m_Angles=0; // :TODO: compute a score for this
	segResult.m_Intensity=0; // :TODO: compute a score for this

}




KPhantomSeg::KPhantomSeg(int sizeX, int sizeY,
						 int searchOriginX, int searchOriginY, int searchSizeX, int searchSizeY , bool debugOutput /*=false*/, std::string possibleFiducialsImageFilename)
:
	m_SizeX(sizeX), m_SizeY(sizeY), 
	m_SearchOriginX(searchOriginX), m_SearchOriginY(searchOriginY), 
	m_SearchSizeX(searchSizeX), m_SearchSizeY(searchSizeY), m_PossibleFiducialsImageFilename(possibleFiducialsImageFilename)
{
	m_SegImpl = new SegImpl;

	m_SegImpl->size = sizeX*sizeY;
	m_SegImpl->bytes = sizeX*sizeY*sizeof(pixel);

	m_SegImpl->dilated = new pixel[m_SegImpl->size];
	m_SegImpl->eroded = new pixel[m_SegImpl->size];
	m_SegImpl->working = new pixel[m_SegImpl->size];
	m_SegImpl->unalteredImage = new pixel[m_SegImpl->size]; 

	m_SegImpl->rows = sizeY;
	m_SegImpl->cols = sizeX;

	m_SegImpl->vertLow = searchOriginY;
	m_SegImpl->horzLow = searchOriginX;

	m_SegImpl->vertHigh = searchOriginY + searchSizeY;
	m_SegImpl->horzHigh = searchOriginX + searchSizeX;

	m_SegImpl->debugOutput=debugOutput; 
	m_SegImpl->possibleFiducialsImageFilename=possibleFiducialsImageFilename; 
}

KPhantomSeg::~KPhantomSeg()
{
	delete[] m_SegImpl->dilated;
	delete[] m_SegImpl->eroded;
	delete[] m_SegImpl->working;
	delete[] m_SegImpl->unalteredImage; 
	delete m_SegImpl;
}

std::vector<std::vector<double>> KPhantomSeg::sortInAscendingOrder(std::vector<std::vector<double>> fiducials) 
{
	std::vector<std::vector<double>> sortedFiducials; 
	

	if( fiducials[0][0] < fiducials[1][0] )
	{
		if( fiducials[2][0] > fiducials[1][0] )
		{
			// pattern: X1 < X2 < X3
			std::vector<double> N1 = fiducials[0];
			std::vector<double> N2 = fiducials[1];
			std::vector<double> N3 = fiducials[2];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else if ( fiducials[2][0] < fiducials[1][0] )
		{
			// pattern: X3 < X1 < X2
			std::vector<double> N1 =  fiducials[2];
			std::vector<double> N2 =  fiducials[0];
			std::vector<double> N3 =  fiducials[1];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else
		{
			// pattern: X1 < X3 < X2
			std::vector<double> N1 =  fiducials[0];
			std::vector<double> N2 =  fiducials[2];
			std::vector<double> N3 =  fiducials[1];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}

		
	}
	else	// X1 >= X2
	{
		if( fiducials[2][0] < fiducials[1][0] )
		{
			// pattern: X3 < X2 < X1
			std::vector<double> N1 =  fiducials[2];
			std::vector<double> N2 =  fiducials[1];
			std::vector<double> N3 =  fiducials[0];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else if ( fiducials[2][0] > fiducials[0][0] )
		{
			// pattern: X2 < X1 < X3
			std::vector<double> N1 =  fiducials[1];
			std::vector<double> N2 =  fiducials[0];
			std::vector<double> N3 =  fiducials[2];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else
		{
			// pattern: X2 < X3 < X1
			std::vector<double> N1 =  fiducials[1];
			std::vector<double> N2 =  fiducials[2];
			std::vector<double> N3 =  fiducials[0];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}

		
	}




	if( fiducials[3][0] < fiducials[4][0] )
	{
		if( fiducials[5][0] > fiducials[4][0] )
		{
			// pattern: X1 < X2 < X3
			std::vector<double> N1 =  fiducials[3];
			std::vector<double> N2 =  fiducials[4];
			std::vector<double> N3 =  fiducials[5];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else if ( fiducials[5][0] < fiducials[4][0] )
		{
			// pattern: X3 < X1 < X2
			std::vector<double> N1 =  fiducials[5];
			std::vector<double> N2 =  fiducials[3];
			std::vector<double> N3 =  fiducials[4];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else
		{
			// pattern: X1 < X3 < X2
			std::vector<double> N1 =  fiducials[3];
			std::vector<double> N2 =  fiducials[5];
			std::vector<double> N3 =  fiducials[4];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}

		
	}
	else	// X1 >= X2
	{
		if( fiducials[5][0] < fiducials[4][0] )
		{
			// pattern: X3 < X2 < X1
			std::vector<double> N1 =  fiducials[5];
			std::vector<double> N2 =  fiducials[4];
			std::vector<double> N3 =  fiducials[3];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else if ( fiducials[5][0] > fiducials[3][0] )
		{
			// pattern: X2 < X1 < X3
			std::vector<double> N1 =  fiducials[4];
			std::vector<double> N2 =  fiducials[3];
			std::vector<double> N3 =  fiducials[5];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}
		else
		{
			// pattern: X2 < X3 < X1
			std::vector<double> N1 =  fiducials[4];
			std::vector<double> N2 =  fiducials[5];
			std::vector<double> N3 =  fiducials[3];
			sortedFiducials.push_back( N3 );
			sortedFiducials.push_back( N2 );
			sortedFiducials.push_back( N1 );
		}

		
	}


	return sortedFiducials;

}






void KPhantomSeg::segment( unsigned char *data, const SegmentationParameters &segParams)
{
	m_SegResult.Clear(); 	
	m_SegImpl->uscseg( data, segParams, m_SegResult );
}

void KPhantomSeg::printResults()
{
	m_SegImpl->print_results();
}

void KPhantomSeg::drawResults( unsigned char *image )
{
	m_SegImpl->draw_results( image );
}

void KPhantomSeg::GetSegmentationResults(SegmentationResults &segResults) { 
	segResults=m_SegResult; 
  }


