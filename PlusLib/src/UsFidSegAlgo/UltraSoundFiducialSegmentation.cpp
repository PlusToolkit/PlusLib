#include "UltraSoundFiducialSegmentation.h"

const int BLACK = 0; 
const int WHITE = 255; 

#define DOT_STEPS 4.0
#define DOT_RADIUS 6.0

using std::cout;
using std::endl;

//-----------------------------------------------------------------------------

SegmentationResults::SegmentationResults()
	: m_DotsFound( false ), m_Angles(-1), m_Intensity(-1), m_NumDots(0)
{}

//-----------------------------------------------------------------------------

void SegmentationResults::Clear()
{
	m_DotsFound = false;
	m_Angles = -1;
	m_Intensity = -1;
	m_FoundDotsCoordinateValue.clear();
	m_NumDots = 1;
	m_CandidateFidValues = NULL; 
}

//-----------------------------------------------------------------------------

/* Possible additional criteria:
 *  1. Track the frame-to-frame data?
 *  2. Lines should be roughly of the same length? */

void SegImpl::suppress( PixelType *image, float percent_thresh_top, float percent_thresh_bottom )
{
	PixelType max = 0;
	for ( unsigned int pos = 0; pos < size; pos ++ ) {
		if ( image[pos] > max )
			max = image[pos];
	}

	PixelType min = 255; 
	for ( unsigned int currentPos = 0; currentPos < size; currentPos ++) 
	{ 
		if( image[currentPos] < min)
		{ 
			min = image[currentPos]; 
		} 
	} 

	// Thomas Kuiran Chen
	// NOTE: round/roundf are not ANSI C++ math functions. 
	//       We use floor to calculate the round value here.
	
	PixelType thresh_top = min+(PixelType)floor( (float)(max-min) * percent_thresh_top + 0.5 );
	PixelType thresh_bottom = min+(PixelType)floor( (float)(max-min) * percent_thresh_bottom + 0.5 );
	
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

	//thresholding 

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

inline void SegImpl::trypos( PixelType *image, int r, int c )
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

//-----------------------------------------------------------------------------

Dot * SegImpl::cluster()
{

	Dot dot;

	for ( unsigned int r = vertLow; r < vertHigh; r++ ) {
		for ( unsigned int c = horzLow; c < horzHigh; c++ ) {
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
					if (m_SegParams.GetUseOriginalImageIntensityForDotIntensityScore())
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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

float segment_length( Dot *d1, Dot *d2 )
{
	float xd = d2->c - d1->c;
	float yd = d2->r - d1->r;
	return sqrtf( xd*xd + yd*yd );
}

//-----------------------------------------------------------------------------

float line_length( const Line &line, Dot *dots )
{
	float l1 = segment_length( &dots[line.b[0]], &dots[line.b[1]] );
	float l2 = segment_length( &dots[line.b[0]], &dots[line.b[2]] );
	float l3 = segment_length( &dots[line.b[1]], &dots[line.b[2]] );

	if ( l2 > l1 )
		l1 = l2;
	return l3 > l1 ? l3 : l1;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

bool SegImpl::accept_line( const Line &line )
{
	double scalingEstimation = m_SegParams.GetScalingEstimation();

	int maxLineLenPx = floor(m_SegParams.GetMaxLineLenMm() / scalingEstimation + 0.5 );
	int minLineLenPx = floor(m_SegParams.GetMinLineLenMm() / scalingEstimation + 0.5 );
	double maxLineErrorPx = m_SegParams.GetMaxLineErrorMm() / scalingEstimation;

	if ( line.length <= maxLineLenPx && line.length >= minLineLenPx &&
			line.line_error <= maxLineErrorPx && 
			line.t >= m_SegParams.GetMinTheta() && line.t <= m_SegParams.GetMaxTheta() )
		return true;
	return false;
}

//-----------------------------------------------------------------------------

void SegImpl::find_lines3pt( )
{
	/* For each point, loop over each 2-point line and try to make a 3-point
	 * line. For the third point use the theta of the line and compute a value
	 * for p. Accept the line if the compute p is within some small distance
	 * of the 2-point line. */

	Line line;
	int points[3];
	float dist = m_SegParams.GetFindLines3PtDist();
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

//-----------------------------------------------------------------------------

void SegImpl::find_lines( )
{
	/* Make pairs of dots into 2-point lines. */
	find_lines2pt();

	/* Make 2-point lines and dots into 3-point lines. */
	find_lines3pt();

	/* Sort by intensity. */
	sort<Line, LtLineIntensity>( lines, nlines );
}

//-----------------------------------------------------------------------------

void SegImpl::find_pairs()
{
	double scalingEstimation = m_SegParams.GetScalingEstimation();
	double maxAngleDifference = m_SegParams.GetMaxAngleDiff();
	double maxTheta = m_SegParams.GetMaxTheta();
	double minTheta = m_SegParams.GetMinTheta();

	int maxLinePairDistPx = floor(m_SegParams.GetMaxLinePairDistMm() / scalingEstimation + 0.5 );
	int minLinePairDistPx = floor(m_SegParams.GetMinLinePairDistMm() / scalingEstimation + 0.5 );
	double maxLineErrorPx = m_SegParams.GetMaxLineErrorMm() / scalingEstimation;

	for ( int l1 = 0; l1 < nlines; l1++ ) {
		for ( int l2 = l1+1; l2 < nlines; l2++ ) {
			float t1 = lines[l1].t;
			float p1 = lines[l1].p;

			float t2 = lines[l2].t;
			float p2 = lines[l2].p;

			float angle_diff = fabsf( t2 - t1 );
			float line_error = lines[l1].line_error + lines[l2].line_error;

			bool test1 = angle_diff < maxAngleDifference;
			bool test2 = line_error < maxLineErrorPx;
			bool test3 = minTheta <= t1 && t1 <= maxTheta;
			bool test4 = minTheta <= t2 && t2 <= maxTheta;
			// (old value was hardcoded as 65)
			bool test5 = fabsf( p2 - p1 ) < maxLinePairDistPx;  
			// (old value was hardcoded as 25)
			bool test6 = fabsf( p2 - p1 ) > minLinePairDistPx;

			if ( test1 && test2 && test3 && test4 && test5 && test6 ) {
				line_error = line_error / maxLineErrorPx;
				float angle_conf = angle_diff / ( 1 - line_error );

				if ( angle_conf < maxAngleDifference ) {
					float intensity = lines[l1].intensity + lines[l2].intensity;

					pairs[npairs].l1 = l1;
					pairs[npairs].l2 = l2;
					pairs[npairs].intensity = intensity;
					pairs[npairs].line_error = line_error;
					pairs[npairs].angle_diff = angle_diff / maxAngleDifference;
					pairs[npairs].angle_conf = angle_conf / maxAngleDifference;

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

//-----------------------------------------------------------------------------

void SegImpl::sort_top_to_bottom( LinePair *pair )
{
	/* check if we need to swap the pairs. */
	if ( lines[pair->l1].p < lines[pair->l2].p ) {
		int swp_tmp = pair->l1;
		pair->l1 = pair->l2;
		pair->l2 = swp_tmp;
	}
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void SegImpl::WritePossibleFiducialOverlayImage(Dot *fiducials, PixelType *unalteredImage)
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
	for ( unsigned int r = vertLow; r < vertHigh; r++ ) 
	{
		for ( unsigned int c = horzLow; c < horzHigh; c++ ) 
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

//-----------------------------------------------------------------------------

void SegImpl::uscseg( PixelType *image, const SegmentationParameters &segParams, SegmentationResults &segResult )
{
	ndots = nlines = npairs = 0;
	ntest = nset = nlines2pt = 0;
	m_SegParams = segParams; 

	memcpy( working, image, bytes );
	memcpy( unalteredImage, image, bytes); 

	morphological_operations();

	suppress( working, m_SegParams.GetThresholdImageTop()/100.00, m_SegParams.GetThresholdImageBottom()/100.00 );

	cluster();

	segResult.m_NumDots= ndots; 
	segResult.m_CandidateFidValues=dots;	  
	 
	if(debugOutput) 
	{
		WritePossibleFiducialOverlayImage(segResult.m_CandidateFidValues, unalteredImage); 
	}
	
	find_lines();

	switch (m_SegParams.GetFiducialGeometry())
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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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
		double minUsideLineLength = m_SegParams.GetMinUsideLineLength();
		double maxUsideLineLength = m_SegParams.GetMaxUsideLineLength();
		for(int dotPosition=0; dotPosition<ndots; dotPosition++)
		{	
			vnl_vector<double> vectorFromCurrentDot(3);
			vectorFromCurrentDot(0) = (dots[dotPosition].c - threePointLine[ptAindex](0)); 
			vectorFromCurrentDot(1) = (dots[dotPosition].r - threePointLine[ptAindex](1));
			vectorFromCurrentDot(2) = 0;
			if (vectorFromCurrentDot.magnitude()>minUsideLineLength &&
				vectorFromCurrentDot.magnitude()<maxUsideLineLength)
			{
				vectorFromCurrentDot.normalize();
				double maxUangleDiff = m_SegParams.GetMaxUangleDiff();
				double angleDiff=acos(dot_product(normal, vectorFromCurrentDot));
				if (fabs(angleDiff)<maxUangleDiff
					|| fabs(M_PI-angleDiff) <maxUangleDiff )
				{
					// this may be sideline of the U
					potentialAvectors.push_back(dotPosition); 
				}
			}

			vectorFromCurrentDot(0) = (dots[dotPosition].c - threePointLine[ptBindex](0)); 
			vectorFromCurrentDot(1) = (dots[dotPosition].r - threePointLine[ptBindex](1));
			vectorFromCurrentDot(2) = 0;
			if (vectorFromCurrentDot.magnitude()>minUsideLineLength &&
				vectorFromCurrentDot.magnitude()<maxUsideLineLength)
			{
				vectorFromCurrentDot.normalize();		
				double maxUangleDiff = m_SegParams.GetMaxUangleDiff();
				double angleDiff=acos(dot_product(normal, vectorFromCurrentDot));
				if (fabs(angleDiff)<maxUangleDiff
					|| fabs(M_PI-angleDiff) <maxUangleDiff )
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

				if( (aVec-bVec).magnitude()<m_SegParams.GetMaxUsideLineDiff())
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

//-----------------------------------------------------------------------------

KPhantomSeg::KPhantomSeg(int sizeX, int sizeY,
						 int searchOriginX, int searchOriginY, int searchSizeX, int searchSizeY , bool debugOutput /*=false*/, std::string possibleFiducialsImageFilename)
:
	m_SizeX(sizeX), m_SizeY(sizeY), 
	m_SearchOriginX(searchOriginX), m_SearchOriginY(searchOriginY), 
	m_SearchSizeX(searchSizeX), m_SearchSizeY(searchSizeY), m_PossibleFiducialsImageFilename(possibleFiducialsImageFilename)
{
	m_SegImpl = new SegImpl;

	m_SegImpl->size = sizeX*sizeY;
	m_SegImpl->bytes = sizeX*sizeY*sizeof(PixelType);

	m_SegImpl->dilated = new PixelType[m_SegImpl->size];
	m_SegImpl->eroded = new PixelType[m_SegImpl->size];
	m_SegImpl->working = new PixelType[m_SegImpl->size];
	m_SegImpl->unalteredImage = new PixelType[m_SegImpl->size]; 

	m_SegImpl->rows = sizeY;
	m_SegImpl->cols = sizeX;

	m_SegImpl->vertLow = searchOriginY;
	m_SegImpl->horzLow = searchOriginX;

	m_SegImpl->vertHigh = searchOriginY + searchSizeY;
	m_SegImpl->horzHigh = searchOriginX + searchSizeX;

	m_SegImpl->debugOutput=debugOutput; 
	m_SegImpl->possibleFiducialsImageFilename=possibleFiducialsImageFilename; 
}

//-----------------------------------------------------------------------------

KPhantomSeg::~KPhantomSeg()
{
	delete[] m_SegImpl->dilated;
	delete[] m_SegImpl->eroded;
	delete[] m_SegImpl->working;
	delete[] m_SegImpl->unalteredImage; 
	delete m_SegImpl;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void KPhantomSeg::segment( unsigned char *data, const SegmentationParameters &segParams)
{
	m_SegResult.Clear(); 	
	m_SegImpl->uscseg( data, segParams, m_SegResult );
}

//-----------------------------------------------------------------------------

void KPhantomSeg::printResults()
{
	m_SegImpl->print_results();
}

//-----------------------------------------------------------------------------

void KPhantomSeg::drawResults( unsigned char *image )
{
	m_SegImpl->draw_results( image );
}

//-----------------------------------------------------------------------------

void KPhantomSeg::GetSegmentationResults(SegmentationResults &segResults) { 
	segResults=m_SegResult; 
}

//-----------------------------------------------------------------------------

bool shape_contains( std::vector<Item> shape, Item newItem )
{
	for ( unsigned int si = 0; si < shape.size(); si++ ) 
	{
		if ( shape[si] == newItem )
		{
			return true;
		}
	}
	return false;
}

//************************************************************************************************

inline PixelType min( PixelType v1, PixelType v2 )
{
	return v1 < v2 ? v1 : v2;
}

//-----------------------------------------------------------------------------

inline PixelType max( PixelType v1, PixelType v2 )
{
	return v1 > v2 ? v1 : v2;
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::erode_point_0( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = UCHAR_MAX;
	unsigned int p = ir*cols + ic - barSize; // current pixel - bar size (position of the start of the bar)
	unsigned int p_max = ir*cols + ic + barSize;// current pixel +  bar size (position of the end  of the bar)

	//find lowest intensity in bar shaped area in image
	for ( ; p <= p_max; p++ ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

//-----------------------------------------------------------------------------

int SegImpl::GetMorphologicalOpeningBarSizePx()
{
	int barsize = floor(m_SegParams.GetMorphologicalOpeningBarSizeMm() / m_SegParams.GetScalingEstimation() + 0.5 );
	return barsize; 
}

//-----------------------------------------------------------------------------

void SegImpl::erode_0( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		unsigned int ic = horzLow;
		unsigned int p_base = ir*cols;

		PixelType dval = erode_point_0( image, ir, ic ); // find lowest pixel intensity in surroudning region ( postions +/- 8 of current pixel position) 
		dest[p_base+ic] = dval; // p_base+ic = current pixel

		for ( ic++; ic < horzHigh; ic++ ) {
			PixelType new_val = image[p_base + ic + barSize];
			PixelType del_val = image[p_base + ic - 1 - barSize];

			dval = new_val <= dval ? new_val  : // dval = new val if new val is less than or equal to dval
					del_val > dval ? min(dval, new_val) : // if del val is greater than dval, dval= min of dval and new val
					erode_point_0( image, ir, ic ); //else dval = result of erode function

			dest[ir*cols+ic] = dval; // update new "eroded" picture
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::erode_point_45( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	PixelType dval = UCHAR_MAX;
	unsigned int p = (ir+barSize)*cols + ic-barSize;
	unsigned int p_max = (ir-barSize)*cols + ic+barSize;

	for ( ; p >= p_max; p = p - cols + 1 ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::erode_45( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Down the left side. */
	for ( unsigned int sr = vertLow; sr < vertHigh; sr++ ) {
		unsigned int ir = sr;
		unsigned int ic = horzLow;

		PixelType dval = erode_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_45( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}

	/* Accross the bottom */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertHigh-1;

		PixelType dval = erode_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_45( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::erode_point_90( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = UCHAR_MAX;
	unsigned int p = (ir-barSize)*cols + ic;
	unsigned int p_max = (ir+barSize)*cols + ic;

	for ( ; p <= p_max; p += cols ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::erode_90( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ic = horzLow; ic < horzHigh; ic++ ) {
		unsigned int ir = vertLow;

		PixelType dval = erode_point_90( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir++; ir < vertHigh; ir++ ) {
			PixelType new_val = image[(ir + barSize)*cols+ic];
			PixelType del_val = image[(ir - 1 - barSize)*cols+ic];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_90( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::erode_point_135( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = UCHAR_MAX;
	unsigned int p = (ir-barSize)*cols + ic-barSize;
	unsigned int p_max = (ir+barSize)*cols + ic+barSize;

	for ( ; p <= p_max; p = p + cols + 1 ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::erode_135( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Up the left side. */
	for ( unsigned int sr = vertHigh-1; sr >= vertLow; sr-- ) {
		unsigned int ir = sr;

		unsigned int ic = horzLow;
		PixelType dval = erode_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_135( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}

	/* Across the top. */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertLow;

		PixelType dval = erode_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_135( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}
}

//-----------------------------------------------------------------------------

void SegImpl::erode_circle( PixelType *dest, PixelType *image )
{
	std::vector<Item> morphologicalCircle = m_SegParams.GetMorphologicalCircle();
	unsigned int slen = morphologicalCircle.size();

	memset( dest, 0, rows*cols*sizeof(PixelType) );

	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		for ( unsigned int ic = horzLow; ic < horzHigh; ic++ ) {
			PixelType dval = UCHAR_MAX;
			for ( unsigned int sp = 0; sp < slen; sp++ ) {
				unsigned int sr = ir + morphologicalCircle[sp].roff;
				unsigned int sc = ic + morphologicalCircle[sp].coff;

				if ( image[sr*cols+sc] < dval )
					dval = image[sr*cols+sc];

				if ( image[sr*cols+sc] == 0 )
					break;
			}
			dest[ir*cols+ic] = dval;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::dilate_point_0( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = ir*cols + ic - barSize;
	unsigned int p_max = ir*cols + ic + barSize;

	for ( ; p <= p_max; p++ ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::dilate_0( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		unsigned int ic = horzLow;
		unsigned int p_base = ir*cols;

		PixelType dval = dilate_point_0( image, ir, ic );
		dest[ir*cols+ic] = dval;
		for ( ic++; ic < horzHigh; ic++ ) {
			PixelType new_val = image[p_base + ic + barSize];
			PixelType del_val = image[p_base + ic - 1 - barSize];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_0( image, ir, ic ));
			dest[ir*cols+ic] = dval;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::dilate_point_45( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = (ir+barSize)*cols + ic-barSize;
	unsigned int p_max = (ir-barSize)*cols + ic+barSize;

	for ( ; p >= p_max; p = p - cols + 1 ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::dilate_45( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Down the left side. */
	for ( unsigned int sr = vertLow; sr < vertHigh; sr++ ) {
		unsigned int ir = sr;
		unsigned int ic = horzLow;

		PixelType dval = dilate_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_45( image, ir, ic ));
			dest[ir*cols+ic] = dval ;
		}
	}

	/* Accross the bottom */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertHigh-1;

		PixelType dval = dilate_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_45( image, ir, ic ));
			dest[ir*cols+ic] = dval ;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::dilate_point_90( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = (ir-barSize)*cols + ic;
	unsigned int p_max = (ir+barSize)*cols + ic;

	for ( ; p <= p_max; p += cols ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::dilate_90( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ic = horzLow; ic < horzHigh; ic++ ) {
		unsigned int ir = vertLow;
		PixelType dval = dilate_point_90( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir++; ir < vertHigh; ir++ ) {
			PixelType new_val = image[(ir + barSize)*cols+ic];
			PixelType del_val = image[(ir - 1 - barSize)*cols+ic];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_90( image, ir, ic ));

			dest[ir*cols+ic] = dval ;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::dilate_point_135( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = (ir-barSize)*cols + ic-barSize;
	unsigned int p_max = (ir+barSize)*cols + ic+barSize;

	for ( ; p <= p_max; p = p + cols + 1 ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::dilate_135( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Up the left side. */
	for ( unsigned int sr = vertHigh-1; sr >= vertLow; sr-- ) {
		unsigned int ir = sr;
		unsigned int ic = horzLow;
		PixelType dval = dilate_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_135( image, ir, ic ));
			dest[ir*cols+ic] = dval;
		}
	}

	/* Across the top. */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertLow;
		PixelType dval = dilate_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval;
		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) : 
					dilate_point_135( image, ir, ic ));
			dest[ir*cols+ic] = dval;
		}
	}
}

//-----------------------------------------------------------------------------

inline PixelType SegImpl::dilate_point( PixelType *image, unsigned int ir, unsigned int ic, 
                Item *shape, int slen )
{
	PixelType dval = 0;
	for ( int sp = 0; sp < slen; sp++ ) {
		unsigned int sr = ir + shape[sp].roff;
		unsigned int sc = ic + shape[sp].coff;
		if ( image[sr*cols+sc] > dval )
			dval = image[sr*cols+sc];
	}
	return dval;
}

//-----------------------------------------------------------------------------

void SegImpl::dilate_circle( PixelType *dest, PixelType *image )
{
	std::vector<Item> morphologicalCircle = m_SegParams.GetMorphologicalCircle();
	unsigned int slen = morphologicalCircle.size();

	Item *shape = new Item[slen]; 

	for ( unsigned int i = 0; i < slen; i++ )
	{
		shape[i] = morphologicalCircle[i]; 
	}

	/* Which elements stick around when you shift right? */
	int n = 0;

	bool *sr_exist = new bool[slen];	

	memset( sr_exist, 0, slen*sizeof(bool) );
	for ( int si = 0; si < slen; si++ ) 
	{
		if ( shape_contains( morphologicalCircle, Item(morphologicalCircle[si].roff, morphologicalCircle[si].coff+1) ) )
			sr_exist[si] = true, n++;
	}
	//cout << "shift_exist: " << n << endl;

	Item *new_items = new Item[slen]; 
	Item *old_items = new Item[slen];
	
	int n_new_items = 0, n_old_items = 0;
	for ( int si = 0; si < slen; si++ ) {
		if ( sr_exist[si] )
			old_items[n_old_items++] = shape[si];
		else
			new_items[n_new_items++] = shape[si];
	}

	delete [] sr_exist; 

	memset( dest, 0, rows*cols*sizeof(PixelType) );
	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		unsigned int ic = horzLow;

		PixelType dval = dilate_point( image, ir, ic, shape, slen );
		PixelType last = dest[ir*cols+ic] = dval;
		
		for ( ic++; ic < horzHigh; ic++ ) {
			PixelType dval = dilate_point( image, ir, ic, new_items, n_new_items );

			if ( dval < last ) {
				for ( int sp = 0; sp < n_old_items; sp++ ) {
					unsigned int sr = ir + old_items[sp].roff;
					unsigned int sc = ic + old_items[sp].coff;
					if ( image[sr*cols+sc] > dval )
						dval = image[sr*cols+sc];

					if ( image[sr*cols+sc] == last )
						break;
				}
			}
			last = dest[ir*cols+ic] = dval ;
		}
	}
	delete [] new_items; 
	delete [] old_items; 
}

//-----------------------------------------------------------------------------

void SegImpl::subtract( PixelType *image, PixelType *vals )
{
	for ( unsigned int pos = 0; pos < rows*cols; pos++ )
		image[pos] = vals[pos] > image[pos] ? 0 : image[pos] - vals[pos];
}

//-----------------------------------------------------------------------------

void SegImpl::dynamicThresholding( PixelType *pixelArray)
{
	typedef unsigned char			PixelType; 
	const unsigned int				Dimension = 2; 

	// copy array of pixels into an image container
	typedef itk::Image< PixelType, Dimension > ImageType; 
	ImageType::Pointer image = ImageType::New(); 
	ImageType::SizeType size; 
	size[0] = cols; 
	size[1] = rows; 

	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0; 

	ImageType::RegionType wholeImage; 
	wholeImage.SetSize(size);
	wholeImage.SetIndex(start); 

	image->SetRegions(wholeImage); 
	image->Allocate(); 

	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType iter(image, image->GetRequestedRegion() ); 
	iter.GoToBegin(); 

	int count = 0; 	

	while( !iter.IsAtEnd())
	{
		iter.Set(pixelArray[count]);
		count++;
		++iter;
	}

	// mean set up
	// change region size
	typedef   unsigned char  InputPixelType;
	typedef   unsigned char  OutputPixelType;
	
	typedef itk::Image< InputPixelType,  2 >   InputImageType;
	typedef itk::Image< OutputPixelType, 2 >   OutputImageType;
	typedef itk::MeanImageFilter<InputImageType, OutputImageType >  FilterType;

	FilterType::Pointer meanFilter = FilterType::New();

	InputImageType::SizeType indexRadius;
  

	indexRadius[0] = 5; // radius along x
	indexRadius[1] = 8; // radius along y

	meanFilter->SetRadius( indexRadius );
 
	// subtract filter set up
	typedef   unsigned char	SubtractPixelType;
	typedef itk::Image< SubtractPixelType,  2 >   SubtractResultsImageType;
	typedef itk::SubtractImageFilter<ImageType,OutputImageType,SubtractResultsImageType>SubtractionFilterType; 
	
	SubtractionFilterType::Pointer subtractionFilter = SubtractionFilterType::New(); 

	//writer setup
	typedef itk::ImageFileWriter< ImageType > WriterType;
	WriterType::Pointer writeResult = WriterType::New();  // 
	writeResult->SetFileName("dynamicthresholding" + possibleFiducialsImageFilename);

	//thresholding setup
	typedef   unsigned char ThresholdPixelType;
	typedef itk::Image< ThresholdPixelType, 2 > ThresholdResultImageType;
	typedef itk::BinaryThresholdImageFilter<SubtractResultsImageType,ThresholdResultImageType> ThresholdFilterType; 
	ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
	
	//pipeline setup to incorporate mean and subtraction filters if desired, currently only 
	//uses thresholding 

	//meanFilter->SetInput(image);
	//subtractionFilter->SetInput1(image);
	//subtractionFilter->SetInput2(meanFilter->GetOutput()); 
	//subtractionFilter->Update(); 
	
	/* 
	Threshold values based on the concept that after the opening operations
	the image has a black background with potential fiducials appearing less black
	*/  
	thresholdFilter->SetInput(image); 
	thresholdFilter->SetOutsideValue( 10);
	thresholdFilter->SetInsideValue( 255);
	thresholdFilter->SetLowerThreshold( 26); // 13
	thresholdFilter->SetUpperThreshold( 255);

	thresholdFilter->Update(); 
	//uncomment below if you wish to output the image
	// directly after thresholding, redundant if debug
	// mode is on 

	//writeResult->SetInput( thresholdFilter->GetOutput() );
	//writeResult->Update();

	/*set working image pixel values to new thresholded image
	tried using pixel buffer andmemcopy but couldn't figure
	out how to do it*/ 
	
	ImageType::Pointer filteredImage = thresholdFilter->GetOutput(); 

	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType resultingImageIterator(filteredImage, filteredImage->GetRequestedRegion() ); 
	resultingImageIterator.GoToBegin(); 

	int counter = 0; 	

	while( !resultingImageIterator.IsAtEnd())
	{
	working[counter] = resultingImageIterator.Get();
	counter++; 
	++resultingImageIterator;

	}	
}

//-----------------------------------------------------------------------------

void SegImpl::WritePng(PixelType *modifiedImage, std::string outImageName, int cols, int rows) 
{

// output intermediate image
	
	typedef unsigned char          PixelType; // define type for pixel representation
	const unsigned int             Dimension = 2; 

	typedef itk::Image< PixelType, Dimension > ImageType;
	ImageType::Pointer modImage = ImageType::New(); 
	ImageType::SizeType size;
	size[0] = cols;
	size[1] = rows; 

	ImageType::IndexType start; 
	start[0] = 0; 
	start[1] = 0; 

	ImageType::RegionType wholeImage; 
	wholeImage.SetSize(size);
	wholeImage.SetIndex(start); 

	modImage->SetRegions(wholeImage); 
	modImage->Allocate(); 

	typedef itk::ImageFileWriter< ImageType > WriterType; 
	itk::PNGImageIO::Pointer pngImageIO = itk::PNGImageIO::New();
	pngImageIO->SetCompressionLevel(0); 

	WriterType::Pointer writer = WriterType::New();  
	writer->SetImageIO(pngImageIO); 
	writer->SetFileName(outImageName);   
	
	
	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType iter(modImage, modImage->GetRequestedRegion() ); 
	iter.GoToBegin(); 

	int count = 0; 	

	while( !iter.IsAtEnd())
	{
	iter.Set(modifiedImage[count]);
	count++; 
	++iter;
	} 
	
	writer->SetInput( modImage );  // piping output of reader into input of writer
		try
	{
		writer->Update(); // change to writing if want writing feature
	}
	catch (itk::ExceptionObject & err) 
	{		
		std::cerr << " Exception! writer did not update" << std::endl; //ditto 
		std::cerr << err << std ::endl; 
		//return EXIT_GENERIC_FAILURE;
	}
	// end output

}

//-----------------------------------------------------------------------------

void SegImpl::morphological_operations()
{
  // Morphological operations with a stick-like structuring element
	
	if(debugOutput) 
	{
		WritePng(working,"seg01-initial.png", cols, rows); 
	}

	erode_0( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg02-morph-bar-deg0-erode.png", cols, rows); 
	}
	
	dilate_0( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg03-morph-bar-deg0-dilated.png", cols, rows); 
	}
	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg04-morph-bar-deg0-final.png", cols, rows); 
	}

	erode_45( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg05-morph-bar-deg45-erode.png", cols, rows); 
	}

	dilate_45( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg06-morph-bar-deg45-dilated.png", cols, rows); 
	}

	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg07-morph-bar-deg45-final.png", cols, rows); 
	}

	erode_90( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg08-morph-bar-deg90-erode.png", cols, rows); 
	}

	dilate_90( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg09-morph-bar-deg90-dilated.png", cols, rows); 
	}

	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg10-morph-bar-deg90-final.png", cols, rows); 
	}

	erode_135( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg11-morph-bar-deg135-erode.png", cols, rows); 
	}

	dilate_135( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg12-morph-bar-deg135-dilated.png", cols, rows); 
	}

	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg13-morph-bar-deg135-final.png", cols, rows); 
	}

	/* Circle operation. */
	erode_circle( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg14-morph-circle-erode.png", cols, rows); 
	}

	dilate_circle( working, eroded );
	if(debugOutput) 
	{
		WritePng(working,"seg15-morph-circle-final.png", cols, rows); 
	}
	
	//dynamicThresholding(working); 

	/*if(debugOutput) 
	{
		WritePng(working,"afterDT" + possibleFiducialsImageFilename); 
	}
	 */ 
}

//-----------------------------------------------------------------------------

void print_dots( Dot *dots, int ndots )
{
	for ( int d = 0; d < ndots; d++ ) {
		cout << dots[d].c << "\t" << dots[d].r << 
				"\t" << dots[d].intensity << endl;
	}
}

//-----------------------------------------------------------------------------

void print_lines( Line *lines, int nlines )
{
	for ( int l = 0; l < nlines; l++ ) {
		cout << lines[l].t << "\t" << lines[l].p << 
				"\t" << lines[l].b[0] << "\t" << lines[l].b[1] << 
				"\t" << lines[l].b[2] << "\t" << lines[l].length <<
				"\t" << lines[l].intensity << "\t" << lines[l].line_error << endl;
	}
}

//-----------------------------------------------------------------------------

void print_pair( LinePair *pair, Line *lines, Dot *dots )
{
	/* Output. */
	cout << "intensity: " << pair->intensity << endl;
	cout << "line_error: " << pair->line_error << endl;
	cout << "angle_diff: " << pair->angle_diff << endl;
	cout << "angle_conf: " << pair->angle_conf << endl;
	print_lines( lines+pair->l1, 1 );
	print_lines( lines+pair->l2, 1 );
	cout << "-----" << endl;
	for (int i=0; i<3; i++)
	{
		print_dots( dots+lines[pair->l1].b[i], 1 );
	}
	for (int i=0; i<3; i++)
	{
		print_dots( dots+lines[pair->l2].b[i], 1 );
	}
}

//-----------------------------------------------------------------------------

void SegImpl::draw_dots( PixelType *image, Dot *dots, int ndots )
{
	for ( int d = 0; d < ndots; d++ ) {
		Dot *dot = dots+d;
		float row = dot->r;
		float col = dot->c;

		for ( float t = 0; t < 2*M_PI; t += M_PI/DOT_STEPS ) {
			unsigned int r = (int)floor( row + cos(t) * DOT_RADIUS );
			unsigned int c = (int)floor( col + sin(t)* DOT_RADIUS );

			if ( r >= 0 && r < rows && c >= 0 && c <= cols )
				image[r*cols+c] = UCHAR_MAX;
		}

		image[static_cast<int>(floor(row)*cols+floor(col))] = 0; 
	}
}

//-----------------------------------------------------------------------------

void SegImpl::draw_lines( PixelType *image, Line *lines, int nlines )
{
	for ( int l = 0; l < nlines; l++ ) {
		Line *line = lines+l;

		float theta = line->t;
		float p = line->p;

		for (int i=0; i<3; i++)
		{
			draw_dots( image, dots+line->b[i], 1 );
		}		

		if ( theta < M_PI/4 || theta > 3*M_PI/4 ) {
			for ( unsigned int y = 0; y < rows; y++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float x = roundf(( p - y * sin(theta) ) / cos(theta));
				double x = floor( ( p - y * sin(theta) ) / cos(theta) + 0.5 );
				unsigned int r = rows - y - 1;
				unsigned int c = (unsigned int)x;
				if ( c >= 0 && c < cols )
					image[r*cols+c] = UCHAR_MAX;
			}
		}
		else {
			for ( unsigned int x = 0; x < cols; x++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float y = roundf(( p - x * cos(theta) ) / sin(theta));
				double y = floor( ( p - x * cos(theta) ) / sin(theta) + 0.5 );
				unsigned int r = rows - (unsigned int)y - 1;
				unsigned int c = x;
				if ( r >= 0 && r < rows )
					image[r*cols+c] = UCHAR_MAX;
			}
		}
	}
}

//-----------------------------------------------------------------------------

void SegImpl::draw_pair( PixelType *image, LinePair *pair )
{
	/* Drawing on the original. */
	draw_lines( image, lines+pair->l1, 1 );
	draw_lines( image, lines+pair->l2, 1 );
}

//-----------------------------------------------------------------------------

void SegImpl::print_results( )
{
	cout << "===== DOTS =====" << endl;
	print_dots( dots, ndots );

	cout << "===== 3-POINT LINES =====" << endl;
	print_lines( lines, nlines );

	if ( npairs > 0 ) {
		cout << "===== PARALLEL PAIRS =====" << endl;
		for ( int p = 0; p < npairs; p++ ) {
			print_pair( pairs+p, lines, dots );
			cout << endl;
		}
	}
	else {
		cout << "ERROR: could not find any pair of parallel lines!" << endl;
	}
}

//-----------------------------------------------------------------------------

void SegImpl::draw_results( PixelType *image )
{
	if ( npairs > 0 )
		draw_pair( image, pairs );
	else
	{
		cout << "ERROR: could not find the pair of the wires!  See other drawing outputs for more information!" << endl;
		draw_lines( image, lines, nlines );
		draw_dots( image, dots, ndots );
	}

}
