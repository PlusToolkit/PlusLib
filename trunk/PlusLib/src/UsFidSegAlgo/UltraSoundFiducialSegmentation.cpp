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
	m_CandidateFidValues.clear(); 
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
inline bool SegImpl::accept_dot( Dot &dot )
{
	int left = horzLow + MIN_WINDOW_DIST;
	int right = horzHigh - MIN_WINDOW_DIST;
	int top = vertLow + MIN_WINDOW_DIST;
	int bot = vertHigh - MIN_WINDOW_DIST;

	if ( dot.GetY() >= top && dot.GetY() < bot && dot.GetX() >= left && dot.GetX() < right )
		return true;
	return false;
}

//-----------------------------------------------------------------------------

inline void SegImpl::trypos( PixelType *image, int r, int c )
{
	if ( image[r*cols+c] > 0 && ntest < MAX_CLUSTER_VALS && 
			nset < MAX_CLUSTER_VALS )
	{
		test[ntest].SetY(r), test[ntest].SetX(c);
		ntest += 1;

		set[nset].SetY(r), set[nset].SetX(c);
		vals[nset] = image[r*cols+c];
		nset += 1;
		image[r*cols+c] = 0;
	}
}

//-----------------------------------------------------------------------------

void SegImpl::cluster()
{

	Dot dot;

	for ( unsigned int r = vertLow; r < vertHigh; r++ ) {
		for ( unsigned int c = horzLow; c < horzHigh; c++ ) {
			if ( working[r*cols+c] > 0 ) {

				test[0].SetY(r), test[0].SetX(c);
				ntest = 1;

				set[0].SetY(r), set[0].SetX(c);
				vals[0] = working[r*cols+c];
				nset = 1;
				working[r*cols+c] = 0;

				while ( ntest > 0 ) {
					ntest -= 1;
					int tr = test[ntest].GetY();
					int tc = test[ntest].GetX();

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
					dest_r += set[p].GetY() * amount;
					dest_c += set[p].GetX() * amount;
					total += amount;
				}

				dot.SetY(dest_r / total);
				dot.SetX(dest_c / total);
				dot.SetDotIntensity(total);

				if ( accept_dot( dot ) )
				{
					if (m_SegParams.GetUseOriginalImageIntensityForDotIntensityScore())
					{
						// Take into account intensities that are close to the dot center
						const double dotRadius2=3.0*3.0;
						float dest_r = 0, dest_c = 0, total = 0;
						for ( int p = 0; p < nset; p++ ) {
							if ( (set[p].GetY()-dot.GetY())*(set[p].GetY()-dot.GetY())+(set[p].GetX()-dot.GetX())*(set[p].GetX()-dot.GetX())<=dotRadius2)
							{
								//float amount = (float)vals[p] / (float)UCHAR_MAX;
								float amount = (float)unalteredImage[set[p].GetY()*cols+set[p].GetX()] / (float)UCHAR_MAX;
								dest_r += set[p].GetY() * amount;
								dest_c += set[p].GetX() * amount;
								total += amount;
							}
						}
						dot.SetDotIntensity(total);
					}

					dots.push_back(dot);
				}

				if ( ndots == MAX_DOTS ) {
					UltraSoundFiducialSegmentationTools::sort<Dot, Dot>( dots, dots.size() );
					return;
				}
			}
		}
	}

	//UltraSoundFiducialSegmentationTools::sort<Dot, Dot>( dots, dots.size() );
	std::sort (dots.begin(), dots.end(), Dot::lessThan);
}

//-----------------------------------------------------------------------------

float SegImpl::compute_t( Dot *dot1, Dot *dot2 )
{
	float x1 = dot1->GetX() - 1;
	float y1 = rows - dot1->GetY();

	float x2 = dot2->GetX() - 1;
	float y2 = rows - dot2->GetY();

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
	float xd = d2->GetX() - d1->GetX();
	float yd = d2->GetY() - d1->GetY();
	return sqrtf( xd*xd + yd*yd );
}

//-----------------------------------------------------------------------------

float line_length( Line &line, std::vector<Dot> dots )
{
	float l1 = segment_length( &dots[line.GetLinePoint(0)], &dots[line.GetLinePoint(1)] );
	float l2 = segment_length( &dots[line.GetLinePoint(0)], &dots[line.GetLinePoint(2)] );
	float l3 = segment_length( &dots[line.GetLinePoint(1)], &dots[line.GetLinePoint(2)] );

	if ( l2 > l1 )
		l1 = l2;
	return l3 > l1 ? l3 : l1;
}

//-----------------------------------------------------------------------------

void SegImpl::compute_line( Line &line, std::vector<Dot> dots )
{
	int ptnum[3];
	for (int i=0; i<3; i++)
	{
		ptnum[i] = line.GetLinePoint(i);
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
	
	line.SetLineSlope(tMean);
	line.SetLinePosition(0);

	float x[3];
	float y[3];
	float p[3];
	
	for (int i=0; i<3; i++)
	{
		x[i] = dots[ptnum[i]].GetX()  - 1;
		y[i] = rows - dots[ptnum[i]].GetY();
		p[i] = x[i] * cos(tMean) + y[i] * sin(tMean);
	}

	float pMean = (p[0] + p[1] + p[2]) / 3;
	line.SetLinePosition(pMean);
	line.SetLineIntensity(dots[ptnum[0]].GetDotIntensity() + dots[ptnum[1]].GetDotIntensity() + 
			dots[ptnum[2]].GetDotIntensity());
	line.SetLineError(fabsf(p[0]-pMean) + fabsf(p[1]-pMean) + fabsf(p[2]-pMean));
	line.SetLineLength(line_length( line, dots ));
}

//-----------------------------------------------------------------------------

void SegImpl::find_lines2pt()
{
	for ( int b1 = 0; b1 < dots.size(); b1++ ) {
		float x1 = dots[b1].GetX() - 1;
		float y1 = rows - dots[b1].GetY();

		for ( int b2 = b1+1; b2 < dots.size(); b2++ ) {
			Line twoPointsLine;
			twoPointsLine.GetLinePoints()->resize(2);
			//lines2pt[nlines2pt].GetLinePoints()->resize(2);//TODO: make it more general
			float x2 = dots[b2].GetX() - 1;
			float y2 = rows - dots[b2].GetY();

			float t = compute_t( &dots[b1], &dots[b2] ); // get degree of slope (0 deg = parallel to -y axis)

			float p1 = x1 * cos(t) + y1 * sin(t);
			float p2 = x2 * cos(t) + y2 * sin(t);
			float p = (p1 + p2) / 2;

			twoPointsLine.SetLineSlope(t); 
			twoPointsLine.SetLinePosition(p);
			twoPointsLine.SetLinePoint(0, b1);
			twoPointsLine.SetLinePoint(1, b2);
			lines2pt.push_back(twoPointsLine);
			nlines2pt += 1;

			if ( nlines2pt == MAX_LINES_2PT )
				return;
		}
	}
}

//-----------------------------------------------------------------------------

bool SegImpl::accept_line( Line &line )
{
	double scalingEstimation = m_SegParams.GetScalingEstimation();

	int maxLineLenPx = floor(m_SegParams.GetMaxLineLenMm() / scalingEstimation + 0.5 );
	int minLineLenPx = floor(m_SegParams.GetMinLineLenMm() / scalingEstimation + 0.5 );
	double maxLineErrorPx = m_SegParams.GetMaxLineErrorMm() / scalingEstimation;

	if ( line.GetLineLength() <= maxLineLenPx && line.GetLineLength() >= minLineLenPx &&
			line.GetLineError() <= maxLineErrorPx && 
			line.GetLineSlope() >= m_SegParams.GetMinTheta() && line.GetLineSlope() <= m_SegParams.GetMaxTheta() )
		return true;
	return false;
}

//-----------------------------------------------------------------------------

SegImpl::SegImpl(int sizeX, int sizeY, int searchOriginX, int searchOriginY, int searchSizeX, int searchSizeY , bool debugOutput /*=false*/, std::string inputPossibleFiducialsImageFilename)
{
	size = sizeX*sizeY;
	bytes = sizeX*sizeY*sizeof(PixelType);

	dilated = new PixelType[size];
	eroded = new PixelType[size];
	working = new PixelType[size];
	unalteredImage = new PixelType[size]; 

	rows = sizeY;
	cols = sizeX;

	vertLow = searchOriginY;
	horzLow = searchOriginX;

	vertHigh = searchOriginY + searchSizeY;
	horzHigh = searchOriginX + searchSizeX;

	debugOutput=debugOutput; 
	possibleFiducialsImageFilename=inputPossibleFiducialsImageFilename; 

	//dots.resize(MAX_DOTS);
	//lines.resize(MAX_LINES);
	//pairs.resize(MAX_PAIRS);
}

//-----------------------------------------------------------------------------

SegImpl::~SegImpl()
{
	delete[] dilated;
	delete[] eroded;
	delete[] working;
	delete[] unalteredImage; 
}

//-----------------------------------------------------------------------------

void SegImpl::find_lines3pt( )
{
	/* For each point, loop over each 2-point line and try to make a 3-point
	 * line. For the third point use the theta of the line and compute a value
	 * for p. Accept the line if the compute p is within some small distance
	 * of the 2-point line. */

	
	int points[3];
	float dist = m_SegParams.GetFindLines3PtDist();
	for ( int b3 = 0; b3 < dots.size(); b3++ ) {
		float x3 = dots[b3].GetX() - 1;
		float y3 = rows - dots[b3].GetY();

		for ( int l = 0; l < lines2pt.size(); l++ ) {
			float t = lines2pt[l].GetLineSlope();
			float p = lines2pt[l].GetLinePosition();
			int b1 = lines2pt[l].GetLinePoint(0);
			int b2 = lines2pt[l].GetLinePoint(1);

			if ( b3 != b1 && b3 != b2 ) {
				float pb3 = x3 * cos(t) + y3 * sin(t);
				if ( fabsf( p - pb3 ) <= dist ) {
					//lines[nlines].SetLineSlope(0);
					//lines[nlines].SetLinePosition(0);
					Line line;
					line.GetLinePoints()->resize(3);//TODO: make the resize more general

					/* To find unique lines, each line must have a uniqe
					 * configuration of three points. */
					points[0] = b1;
					points[1] = b2;
					points[2] = b3;
					std::sort(points,points+3);//WATCH OUT THE SORT FUNCTION HAS BEEN CHANGED TO THE STD ONE !

					for (int i=0; i<3; i++)
					{
						line.SetLinePoint(i,points[i]);
					}					

					if ( ! UltraSoundFiducialSegmentationTools::BinarySearchFind<Line, Line>( line, lines, lines.size() ) ) {
						compute_line( line, dots );
						if ( accept_line( line ) ) 
						{
							//line.SetLineSlope(0);
							//line.SetLinePosition(0);
							lines.push_back(line);
							//UltraSoundFiducialSegmentationTools::sort<Line, Line>( lines, lines.size() );
							std::sort (lines.begin(), lines.end(), Line::lessThan);
							//UltraSoundFiducialSegmentationTools::BinarySearchInsert<Line, Line>( line, lines, nlines );
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
	// Make pairs of dots into 2-point lines.
	find_lines2pt();

	// Make 2-point lines and dots into 3-point lines.
	find_lines3pt();

	// Sort by intensity.
	//UltraSoundFiducialSegmentationTools::sort<Line, Line>( lines, lines.size() );
	std::sort (lines.begin(), lines.end(), Line::lessThan);
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

	for ( int l1 = 0; l1 < lines.size(); l1++ ) {
		for ( int l2 = l1+1; l2 < lines.size(); l2++ ) {
			float t1 = lines[l1].GetLineSlope();
			float p1 = lines[l1].GetLinePosition();

			float t2 = lines[l2].GetLineSlope();
			float p2 = lines[l2].GetLinePosition();

			float angle_diff = fabsf( t2 - t1 );
			float line_error = lines[l1].GetLineError() + lines[l2].GetLineError();

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
					float intensity = lines[l1].GetLineIntensity() + lines[l2].GetLineIntensity();
					LinePair linePair;
					linePair.SetLine1(l1);
					linePair.SetLine2(l2);
					linePair.SetLinePairIntensity(intensity);
					linePair.SetLinePairError(line_error);
					linePair.SetAngleDifference(angle_diff / maxAngleDifference);
					linePair.SetAngleConf(angle_conf / maxAngleDifference);

					pairs.push_back(linePair);
				}
			}
		}
	}

	/* Rank the pairs by intensity. */
	//UltraSoundFiducialSegmentationTools::sort<LinePair, LinePair>( pairs, pairs.size() );
	std::sort (pairs.begin(), pairs.end(), LinePair::lessThan);
}

//-----------------------------------------------------------------------------

void SegImpl::sort_top_to_bottom( LinePair *pair )
{
	/* check if we need to swap the pairs. */
	if ( lines[pair->GetLine1()].GetLinePosition() < lines[pair->GetLine2()].GetLinePosition() ) {
		int swp_tmp = pair->GetLine1();
		pair->SetLine1(pair->GetLine2());
		pair->SetLine2(swp_tmp);
	}
}

//-----------------------------------------------------------------------------

void SegImpl::sort_right_to_left( Line *line )
{
	/* Since we prohibit stepp lines (see MAX_T and MIN_T) we can use the x
	 * values to sort the points. */
	std::vector<std::vector<Dot>::iterator> pointsIterator(3);//TODO Make it general

	for (int i=0; i<3; i++)
	{
		pointsIterator[i] = dots.begin() + line->GetLinePoint(i);
	}
	//UltraSoundFiducialSegmentationTools::sort<std::vector<Dot>::iterator, Position>( pointsIterator, line->GetLinePoints()->size() );
	std::sort (pointsIterator.begin(), pointsIterator.end(), Position::lessThan);
	//std::sort(points.begin(),points.end(), Position::lessThan);

	for (int i=0; i<3; i++)
	{
		line->SetLinePoint(i,pointsIterator[i] - dots.begin());
	}
}

//-----------------------------------------------------------------------------

void SegImpl::WritePossibleFiducialOverlayImage(std::vector<Dot> fiducials, PixelType *unalteredImage)
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
	for(int numDots=0; numDots<dots.size(); numDots++)
	{
		const int markerPosCount=5;
		const int markerPos[markerPosCount][2]={{0,0}, {+1,0}, {-1,0}, {0,+1}, {0,-1}};

		for (int i=0; i<markerPosCount; i++)
		{
			pixelLocation[0]= fiducials[numDots].GetX()+markerPos[i][0];
			pixelLocation[1]= fiducials[numDots].GetY()+markerPos[i][1]; 
			int clusterMarkerIntensity=fiducials[numDots].GetDotIntensity()*10;
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

//-----------------------------------------------------------------------------

void SegmentationParameters::UpdateParameters()
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

//-----------------------------------------------------------------------------

void SegImpl::uscseg( PixelType *image, SegmentationParameters &segParams, SegmentationResults &segResult )
{
	ndots = nlines = npairs = 0;
	ntest = nset = nlines2pt = 0;
	m_SegParams = segParams; 

	memcpy( working, image, bytes );
	memcpy( unalteredImage, image, bytes); 

	morphological_operations();

	suppress( working, m_SegParams.GetThresholdImageTop()/100.00, m_SegParams.GetThresholdImageBottom()/100.00 );

	cluster();

	segResult.SetNumDots(dots.size()); 
	segResult.SetCandidateFidValues(dots);	  
	 
	if(debugOutput) 
	{
		WritePossibleFiducialOverlayImage(segResult.GetCandidateFidValues(), unalteredImage); 
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

bool Dot::lessThan( Dot &dot1, Dot &dot2 )
{
	/* Use > to get descending. */
	return dot1.GetDotIntensity() > dot2.GetDotIntensity();
}

//-----------------------------------------------------------------------------
/*
bool Position::lessThan( Dot *b1, Dot *b2 )
{
	// Use > to get descending.
	return b1->GetX() > b2->GetX();
}
*/
//-----------------------------------------------------------------------------

bool Position::lessThan( std::vector<Dot>::iterator b1, std::vector<Dot>::iterator b2 )
{
	/* Use > to get descending. */
	return b1->GetX() > b2->GetX();
}

//-----------------------------------------------------------------------------

bool LinePair::lessThan( LinePair &pair1, LinePair &pair2 )
{
	/* Use > to get descending. */
	return pair1.GetLinePairIntensity() > pair2.GetLinePairIntensity();
}

//-----------------------------------------------------------------------------

bool Line::lessThan( Line &line1, Line &line2 )
{
	/* Use > to get descending. */
	return line1.GetLineIntensity() > line2.GetLineIntensity();
}

//-----------------------------------------------------------------------------

bool Line::compareLines(const Line &line1, const Line &line2 )
{
	for (int i=0; i<3; i++)
	{
		if ( line1.GetLinePoint(i) < line2.GetLinePoint(i) )
		{
			return true;
		}
		else if ( line1.GetLinePoint(i) > line2.GetLinePoint(i) )
		{
			return false;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------

bool SortedAngle::lessThan( SortedAngle &pt1, SortedAngle &pt2 )
{
	/* Use > to get descending. */
	return pt1.GetAngle() > pt2.GetAngle();
}

//-----------------------------------------------------------------------------

void SegImpl::find_double_n_lines(SegmentationResults &segResult)
{	
	find_pairs();
	if ( pairs.size() > 0 ) {
		LinePair *  pair = &pairs[0];
		sort_top_to_bottom( pair );

		Line *line1 = &lines[pair->GetLine1()];
		Line *line2 = &lines[pair->GetLine2()];

		sort_right_to_left( line1 );
		sort_right_to_left( line2 );
	}
	else if ( pairs.size() < 1 ) 
	{
		LOG_DEBUG("Segmentation was NOT successful! (Number of dots found: " << segResult.GetFoundDotsCoordinateValue().size() << " Number of possible fiducial points: " << segResult.GetNumDots() << ")"); 
		segResult.SetDotsFound(false);				
		return;
	}

	segResult.SetDotsFound(true);

	LinePair *pair = &pairs[0];
	Line *line1 = &lines[pair->GetLine1()];
	Line *line2 = &lines[pair->GetLine2()];

	std::vector<double> dotCoords;
	std::vector< std::vector<double> > foundDotsCoordinateValues = segResult.GetFoundDotsCoordinateValue();
	for (int i=0; i<3; i++)
	{
		dotCoords.push_back(dots[line1->GetLinePoint(i)].GetX());
		dotCoords.push_back(dots[line1->GetLinePoint(i)].GetY());
		foundDotsCoordinateValues.push_back(dotCoords);
		dotCoords.clear();
	}
	for (int i=0; i<3; i++)
	{
		dotCoords.push_back(dots[line2->GetLinePoint(i)].GetX());
		dotCoords.push_back(dots[line2->GetLinePoint(i)].GetY());
		foundDotsCoordinateValues.push_back(dotCoords);
		dotCoords.clear();
	}
	segResult.SetFoundDotsCoordinateValue(foundDotsCoordinateValues);
	std::vector<std::vector<double>> sortedFiducials = KPhantomSeg::sortInAscendingOrder(segResult.GetFoundDotsCoordinateValue()); 
	segResult.SetFoundDotsCoordinateValue(sortedFiducials); 					

	segResult.SetAngles(pair->GetAngleConf());
	segResult.SetIntensity(pair->GetLinePairIntensity());
	segResult.SetNumDots(dots.size()); 
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
	if(dots.size()<EXPECTED_FIDUCIAL_COUNT) 
	{
		// it is impossible to find all the fiducials, because there are not enough candidates
		return;
	}

	const double MINIMUM_LINE_INTENSITY=1.0;
	if(lines[0].GetLineIntensity()<MINIMUM_LINE_INTENSITY)
	{
		// even the brightest line is too dark, the segmentation failed
		return;
	} 

	while(fiducialsFound == false) 
	{
		for(int linePointPosition=0; linePointPosition<3; linePointPosition++)// three for three points
		{
			vnl_vector<double> pt(3);
			pt(0) = dots[lines[threePointLineRank].GetLinePoint(linePointPosition)].GetX(); // first three point line =  line with fiducials with greatest intensity 
			pt(1) = dots[lines[threePointLineRank].GetLinePoint(linePointPosition)].GetY();
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
		for(int dotPosition=0; dotPosition<dots.size(); dotPosition++)
		{	
			vnl_vector<double> vectorFromCurrentDot(3);
			vectorFromCurrentDot(0) = (dots[dotPosition].GetX() - threePointLine[ptAindex](0)); 
			vectorFromCurrentDot(1) = (dots[dotPosition].GetY() - threePointLine[ptAindex](1));
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

			vectorFromCurrentDot(0) = (dots[dotPosition].GetX() - threePointLine[ptBindex](0)); 
			vectorFromCurrentDot(1) = (dots[dotPosition].GetY() - threePointLine[ptBindex](1));
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
			aVec(0) = (dots[aIndex].GetX() - threePointLine[ptAindex](0)); 
			aVec(1) = (dots[aIndex].GetY() - threePointLine[ptAindex](1)); 
			aVec(2) = 1;
			for(int bVecCandidate = 0; bVecCandidate< potentialBvectors.size(); bVecCandidate++)
			{
				int bIndex = potentialBvectors[bVecCandidate]; 
				vnl_vector<double> bVec(3); // side line B vector
				bVec(0) = (dots[bIndex].GetX() - threePointLine[ptBindex](0)); 
				bVec(1) = (dots[bIndex].GetY() - threePointLine[ptBindex](1)); 
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
			double averageIntensityCurrent = (twoFiducialCombos[j][0].GetDotIntensity() +  twoFiducialCombos[j][1].GetDotIntensity())/2;
			double averageIntensityNext = (twoFiducialCombos[j+1][0].GetDotIntensity() +  twoFiducialCombos[j+1][1].GetDotIntensity())/2;
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

	segResult.SetDotsFound(true);

	std::vector<double> coords(2);
	std::vector< std::vector<double> > foundDotsCoordinateValues = segResult.GetFoundDotsCoordinateValue();
	// Side line A top
	coords[0]=bestFiducialCombo[0].GetX();
	coords[1]=bestFiducialCombo[0].GetY();
	foundDotsCoordinateValues.push_back(coords);

	// Baseline	
	coords[0]=threePointLine[0](0);
	coords[1]=threePointLine[0](1);
	foundDotsCoordinateValues.push_back(coords);
	coords[0]=threePointLine[1](0);
	coords[1]=threePointLine[1](1);
	foundDotsCoordinateValues.push_back(coords);
	coords[0]=threePointLine[2](0);
	coords[1]=threePointLine[2](1);
	foundDotsCoordinateValues.push_back(coords);

	// Side line B top
	coords[0]=bestFiducialCombo[1].GetX();
	coords[1]=bestFiducialCombo[1].GetY();
	foundDotsCoordinateValues.push_back(coords);

	// Sort the fiducials based on their location around the center of gravity the fiducial set
	// (ascending order of the arctan of the COG->FidPoint vector)
	std::vector<double> centerPoint(2);
	centerPoint[0]=0.0;
	centerPoint[1]=0.0;
	for(int i =0;i<segResult.GetFoundDotsCoordinateValue().size();i++)
	{
		centerPoint[0] += segResult.GetFoundDotsCoordinateValue()[i][0];
		centerPoint[1] +=segResult.GetFoundDotsCoordinateValue()[i][1]; 
	}
	int pointCount=segResult.GetFoundDotsCoordinateValue().size();
	centerPoint[0] = centerPoint[0]/pointCount; 
	centerPoint[1] = centerPoint[1]/pointCount;
	std::list<SortedAngle> sortedAngles;
	for(int i =0;i<segResult.GetFoundDotsCoordinateValue().size();i++)
	{		
		SortedAngle sa;
		sa.SetPointIndex(i);
		sa.GetCoordinate()[0]=segResult.GetFoundDotsCoordinateValue()[i][0];
		sa.GetCoordinate()[1]=segResult.GetFoundDotsCoordinateValue()[i][1];
		double directionVectorX = sa.GetCoordinate()[0] - centerPoint[0];
		double directionVectorY = sa.GetCoordinate()[1] - centerPoint[1];
		// angle=0 corresponds to -Y direction
		double angle=atan2(directionVectorX, directionVectorY);
		sa.SetAngle(angle);		
		sortedAngles.push_back(sa);
	}
	sortedAngles.sort(UltraSoundFiducialSegmentationTools::AngleMoreThan);
	segResult.GetFoundDotsCoordinateValue().clear();
	for(std::list<SortedAngle>::iterator it=sortedAngles.begin();it!=sortedAngles.end();it++)
	{
		SortedAngle sa=(*it);
		std::vector<double> coord(2);
		coord[0]=sa.GetCoordinate()[0];
		coord[1]=sa.GetCoordinate()[1];
		foundDotsCoordinateValues.push_back(coord);
	}
	
	segResult.SetFoundDotsCoordinateValue(foundDotsCoordinateValues);
	segResult.SetAngles(0); // :TODO: compute a score for this
	segResult.SetIntensity(0); // :TODO: compute a score for this
}

//-----------------------------------------------------------------------------

KPhantomSeg::KPhantomSeg(int sizeX, int sizeY,
						 int searchOriginX, int searchOriginY, int searchSizeX, int searchSizeY , bool debugOutput /*=false*/, std::string possibleFiducialsImageFilename)
:
	m_SizeX(sizeX), m_SizeY(sizeY), 
	m_SearchOriginX(searchOriginX), m_SearchOriginY(searchOriginY), 
	m_SearchSizeX(searchSizeX), m_SearchSizeY(searchSizeY), m_PossibleFiducialsImageFilename(possibleFiducialsImageFilename)

{
	//TODO make SegImpl the class instead of KPhantomSeg (KPhantomSeg's members are not used anywhere)
	m_SegImpl = new SegImpl(sizeX, sizeY, searchOriginX, searchOriginY, searchSizeX, searchSizeY, debugOutput, possibleFiducialsImageFilename);
}

//-----------------------------------------------------------------------------

KPhantomSeg::~KPhantomSeg()
{
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

void KPhantomSeg::segment( unsigned char *data, SegmentationParameters &segParams)
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

void print_dots( std::vector<Dot>::iterator dotsIterator, int ndots )
{
	for ( int d = 0; d < ndots; d++ ) {
		cout << dotsIterator[d].GetX() << "\t" << dotsIterator[d].GetY() << 
				"\t" << dotsIterator[d].GetDotIntensity() << endl;
	}
}

//-----------------------------------------------------------------------------

void print_lines( std::vector<Line>::iterator linesIterator, int nlines )
{
	for ( int l = 0; l < nlines; l++ ) {
		cout << linesIterator[l].GetLineSlope() << "\t" << linesIterator[l].GetLinePosition() << 
				"\t" << linesIterator[l].GetLinePoint(0) << "\t" << linesIterator[l].GetLinePoint(1) << 
				"\t" << linesIterator[l].GetLinePoint(2) << "\t" << linesIterator[l].GetLineLength() <<
				"\t" << linesIterator[l].GetLineIntensity() << "\t" << linesIterator[l].GetLineError() << endl;
	}
}

//-----------------------------------------------------------------------------

void print_pair( std::vector<LinePair>::iterator pairIterator, std::vector<Line> lines, std::vector<Dot> dots )
{
	/* Output. */
	cout << "intensity: " << pairIterator->GetLinePairIntensity() << endl;
	cout << "line_error: " << pairIterator->GetLinePairError() << endl;
	cout << "angle_diff: " << pairIterator->GetAngleDifference() << endl;
	cout << "angle_conf: " << pairIterator->GetAngleConf() << endl;
	print_lines( lines.begin()+pairIterator->GetLine1(), 1 );
	print_lines( lines.begin()+pairIterator->GetLine2(), 1 );
	cout << "-----" << endl;
	for (int i=0; i<3; i++)
	{
		print_dots( dots.begin()+lines[pairIterator->GetLine1()].GetLinePoint(i), 1 );
	}
	for (int i=0; i<3; i++)
	{
		print_dots( dots.begin()+lines[pairIterator->GetLine2()].GetLinePoint(i), 1 );
	}
}

//-----------------------------------------------------------------------------

void SegImpl::draw_dots( PixelType *image, std::vector<Dot>::iterator dotsIterator, int ndots)
{
	for ( int d = 0; d < ndots; d++ ) {
		float row = dotsIterator[d].GetY();
		float col = dotsIterator[d].GetX();

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

void SegImpl::draw_lines( PixelType *image, std::vector<Line>::iterator linesIterator, int nlines )
{
	for ( int l = 0; l < nlines; l++ )
	{
		float theta = linesIterator[l].GetLineSlope();
		float p = linesIterator[l].GetLinePosition();

		for (int i=0; i<3; i++)
		{
			draw_dots( image, dots.begin()+linesIterator[l].GetLinePoint(i), 1 );//watch out, check for iterators problems if errors
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

void SegImpl::draw_pair( PixelType *image, std::vector<LinePair>::iterator pairIterator )
{
	/* Drawing on the original. */
	draw_lines( image, lines.begin()+pairIterator->GetLine1(), 1 );
	draw_lines( image, lines.begin()+pairIterator->GetLine2(), 1 );
}

//-----------------------------------------------------------------------------

void SegImpl::print_results( )
{
	cout << "===== DOTS =====" << endl;
	print_dots( dots.begin(), dots.size() );

	cout << "===== 3-POINT LINES =====" << endl;
	print_lines( lines.begin(), lines.size() );

	if ( npairs > 0 ) {
		cout << "===== PARALLEL PAIRS =====" << endl;
		for ( int p = 0; p < npairs; p++ ) {
			print_pair( pairs.begin()+p, lines, dots );
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
		draw_pair( image, pairs.begin() );
	else
	{
		cout << "ERROR: could not find the pair of the wires!  See other drawing outputs for more information!" << endl;
		draw_lines( image, lines.begin(), lines.size() );
		draw_dots( image, dots.begin(), dots.size() );
	}

}

//-----------------------------------------------------------------------------

bool UltraSoundFiducialSegmentationTools::AngleMoreThan( SortedAngle &pt1, SortedAngle &pt2 )
{
	/* Use > to get descending. */
	return pt1.GetAngle() < pt2.GetAngle();
}

//-----------------------------------------------------------------------------

template<class T, class LessThan> void UltraSoundFiducialSegmentationTools::doSort(std::vector<T> tmpStor, std::vector<T> data, long len)
{
	/*if ( len <= 1 )
		return;

	long mid = len / 2;

	doSort<T, LessThan>( tmpStor, data, mid );
	doSort<T, LessThan>( tmpStor + mid, data + mid, len - mid );
	
	// Merge the data.
	std::vector<T>::iterator endLower = data.begin() + mid;
	std::vector<T>::iterator lower = data.begin();
	std::vector<T>::iterator endUpper = data.begin() + len;
	std::vector<T>::iterator upper = data.begin() + mid;
	std::vector<T>::iterator dest = tmpStor.begin();
	while ( true ) {
		if ( lower == endLower ) {
			// Possibly upper left.
			if ( upper != endUpper )
				tmpStor.insert(dest, upper, endUpper);
				//std::vector<T>::iterator dest(data, upper, endUpper);
			//memcpy( dest, upper, (endUpper - upper) * sizeof(T) );
			break;
		}
		else if ( upper == endUpper ) {
			// Only lower left.
			if ( lower != endLower )
				std::vector<T>::iterator dest(data, lower, endLower);
				//memcpy( dest, lower, (endLower - lower) * sizeof(T) );
			break;
		}
		else {
			// Both upper and lower left.
			if ( LessThan::lessThan( *upper, *lower ) )
				memcpy( dest++, upper++, sizeof(T) );
			else
				memcpy( dest++, lower++, sizeof(T) );
		}
	}

	// Copy back from the tmpStor array.
	memcpy( data, tmpStor, sizeof( T ) * len );*/
}

//-----------------------------------------------------------------------------

template<class T, class LessThan> void UltraSoundFiducialSegmentationTools::sort(std::vector<T> data, long len)
{
	/* Allocate the tmp space needed by merge sort, sort and free. */

	// Thomas Kuiran Chen - retouched for ANSI-C++
	// ANSI-C++ does not allow an initialization of static array
	// data using a variable.  A walk-around is to use the dynamic
	// allocation of the array.
	//T tmpStor[len];
	//std::vector<T> tmpStor;
	//tmpStor.resize(len);
	//doSort<T, LessThan>( tmpStor, data, len );
	std::sort (data.begin(), data.end(), LessThan::lessThan);//TODO use this line instead of this function

}

//-----------------------------------------------------------------------------

template <class T, class Compare> bool UltraSoundFiducialSegmentationTools::BinarySearchFind( const T &item, std::vector<T> data, int dlen )
{
	/*T *lower = data;
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
	}*/
	return std::binary_search(data.begin(),data.end(),item,Compare::compareLines);
}

//-----------------------------------------------------------------------------

template <class T, class Compare> void UltraSoundFiducialSegmentationTools::BinarySearchInsert( T &item,  std::vector<T> data, int &dlen )
{
	/*T *lower = data;
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

	// Shift over data at insert spot if needed.
	if ( dlen > 0 && pos < dlen )
		memmove(data+pos+1, data+pos, sizeof(T)*(dlen-pos));

	// Save the new length.
	dlen = newLen;
	data[pos] = item;*/
}
