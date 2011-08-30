#include "FidLineFinder.h"
#include "vtkMath.h"

#include <algorithm>
#include <float.h>

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_cross.h"

static const float DOT_STEPS  = 4.0;
static const float DOT_RADIUS = 6.0;

//-----------------------------------------------------------------------------

FidLineFinder::FidLineFinder()
{
	m_FrameSize[0] = -1;
  m_FrameSize[1] = -1;
	m_ApproximateSpacingMmPerPixel = -1.0;

  for(int i= 0 ; i<6 ; i++)
  {
	  m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[i] = -1.0;
  }

  for(int i= 0 ; i<16 ; i++)
  {
    m_ImageToPhantomTransform[i] = -1.0;
  }

	m_MaxLineLengthErrorPercent = -1.0;
	m_MaxLinePairDistanceErrorPercent = -1.0;

	m_MinLineLenMm = -1.0;
	m_MaxLineLenMm = -1.0;
	m_MaxLineErrorMm = -1.0;

	m_FindLines3PtDistanceMm = -1.0; 

	m_MinTheta = -1.0; 
	m_MaxTheta = -1.0;

	m_Angles = -1.0;
}

//-----------------------------------------------------------------------------

FidLineFinder::~FidLineFinder()
{

}

//-----------------------------------------------------------------------------

void FidLineFinder::UpdateParameters()
{
	// Compute error boundaries based on error percents and the NWire definition (supposing that the NWire is regular - parallel sides)
	// Line length of an N-wire: the maximum distance between its wires' front endpoints
	double maxLineLengthSquared = -1.0;
	double minLineLengthSquared = FLT_MAX;
	std::vector<NWire> nWires = m_NWires;

	for (std::vector<NWire>::iterator it = nWires.begin(); it != nWires.end(); ++it) {
		Wire wire0 = it->wires[0];
		Wire wire1 = it->wires[1];
		Wire wire2 = it->wires[2];

		double distance01Squared = vtkMath::Distance2BetweenPoints(wire0.endPointFront, wire1.endPointFront);
		double distance02Squared = vtkMath::Distance2BetweenPoints(wire0.endPointFront, wire2.endPointFront);
		double distance12Squared = vtkMath::Distance2BetweenPoints(wire1.endPointFront, wire2.endPointFront);
		double lineLengthSquared = std::max( std::max(distance01Squared, distance02Squared), distance12Squared );

		if (maxLineLengthSquared < lineLengthSquared) {
			maxLineLengthSquared = lineLengthSquared;
		}
		if (minLineLengthSquared > lineLengthSquared) {
			minLineLengthSquared = lineLengthSquared;
		}
	}

	m_MaxLineLenMm = sqrt(maxLineLengthSquared) * (1.0 + (m_MaxLineLengthErrorPercent / 100.0));
	m_MinLineLenMm = sqrt(minLineLengthSquared) * (1.0 - (m_MaxLineLengthErrorPercent / 100.0));
	LOG_DEBUG("Line length - computed min: " << sqrt(minLineLengthSquared) << " , max: " << sqrt(maxLineLengthSquared) << ";  allowed min: " << m_MinLineLenMm << ", max: " << m_MaxLineLenMm);
}

//-----------------------------------------------------------------------------

void FidLineFinder::ComputeParameters()
{
	/*double maxAngleY = std::max(fabs(m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[2]),m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[3]);//the maximum of the rotation around the Y axis
	m_MaxLineLengthErrorPercent = 1/cos(maxAngleY) - 1;

	double maxAngleX = std::max(fabs(m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[0]),m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[1]);//the maximum of the rotation around the X axis
	m_MaxLinePairDistanceErrorPercent = 1/cos(maxAngleX) - 1;*/

	std::vector<double> thetaX, thetaY, thetaZ;
	thetaX.push_back(GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg()[0]);
	thetaX.push_back(0);
	thetaX.push_back(GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg()[1]);
	thetaY.push_back(GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg()[2]);
	thetaY.push_back(0);
	thetaY.push_back(GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg()[3]);
	thetaZ.push_back(GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg()[4]);
	thetaZ.push_back(0);
	thetaZ.push_back(GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg()[5]);

	vnl_matrix<double> imageToPhantomTransform(4,4);

	for( int i = 0 ; i<4 ;i++)
	{
		for( int j = 0 ; j<4 ;j++)
		{
			imageToPhantomTransform.put(i,j,GetImageToPhantomTransform()[j+4*i]);
		}
	}
	
	vnl_vector<double> pointA(3), pointB(3), pointC(3);

	for( int i = 0; i<3 ;i++)
	{
		pointA.put(i,m_NWires[0].wires[0].endPointFront[i]);
		pointB.put(i,m_NWires[0].wires[0].endPointBack[i]);
		pointC.put(i,m_NWires[0].wires[1].endPointFront[i]);
	}

	vnl_vector<double> AB(3);
	AB = pointB - pointA;
	vnl_vector<double> AC(3);
	AC = pointC - pointA;

	vnl_vector<double> normalVectorInPhantomCoord(3);
	normalVectorInPhantomCoord = vnl_cross_3d(AB,AC);

	vnl_vector<double> normalVectorInPhantomCoordExtended(4,0);

	for( int i = 0 ;i<normalVectorInPhantomCoord.size() ;i++)
	{
		normalVectorInPhantomCoordExtended.put(i,normalVectorInPhantomCoord.get(i));
	}

	vnl_vector<double> normalImagePlane(3,0);//vector normal to the image plane
	normalImagePlane.put(2,1);

	vnl_vector<double> imageYunitVector(3,0);
	imageYunitVector.put(1,1);//(0,1,0)

	std::vector<double> finalAngleTable;

	double tempThetaX, tempThetaY, tempThetaZ;

	for(int i = 0 ; i<3 ; i++)
	{
		tempThetaX = thetaX[i]*vtkMath::Pi()/180;
		for(int j = 0 ; j<3 ; j++)
		{
			tempThetaY = thetaY[j]*vtkMath::Pi()/180;
			for(int k = 0 ; k<3 ; k++)
			{
				tempThetaZ = thetaZ[k]*vtkMath::Pi()/180;
				vnl_matrix<double> totalRotation(4,4,0);

				totalRotation.put(0,0,cos(tempThetaY)*cos(tempThetaZ));
				totalRotation.put(0,1,-cos(tempThetaX)*sin(tempThetaZ)+sin(tempThetaX)*sin(tempThetaY)*cos(tempThetaZ));
				totalRotation.put(0,2,sin(tempThetaX)*sin(tempThetaZ)+cos(tempThetaX)*sin(tempThetaY)*cos(tempThetaZ));
				totalRotation.put(1,0,cos(tempThetaY)*sin(tempThetaZ));
				totalRotation.put(1,1,cos(tempThetaX)*cos(tempThetaZ)+sin(tempThetaX)*sin(tempThetaY)*sin(tempThetaZ));
				totalRotation.put(1,2,-sin(tempThetaX)*cos(tempThetaZ)+cos(tempThetaX)*sin(tempThetaY)*sin(tempThetaZ));
				totalRotation.put(2,0,-sin(tempThetaY));
				totalRotation.put(2,1,sin(tempThetaX)*cos(tempThetaY));
				totalRotation.put(2,2,cos(tempThetaX)*cos(tempThetaY));
				totalRotation.put(3,3,1);

				vnl_matrix<double> totalTranform(4,4);
				totalTranform = totalRotation*imageToPhantomTransform;

				vnl_vector<double> normalVectorInImageCoordExtended(4);
				normalVectorInImageCoordExtended = totalTranform*normalVectorInPhantomCoordExtended;

				vnl_vector<double> normalVectorInImageCoord(3);
				
				for( int i = 0 ;i<normalVectorInImageCoord.size() ;i++)
				{
					normalVectorInImageCoord.put(i,normalVectorInImageCoordExtended.get(i));
				}

				vnl_vector<double> lineDirectionVector(3);
				lineDirectionVector = vnl_cross_3d(normalVectorInImageCoord,normalImagePlane);

				double dotProductValue = dot_product(lineDirectionVector,imageYunitVector);
				double normOfLineDirectionvector = lineDirectionVector.two_norm();
				double angle = acos(dotProductValue/normOfLineDirectionvector);
				finalAngleTable.push_back(angle);
			}
		}
	}

	m_MaxTheta = (*std::max_element(finalAngleTable.begin(),finalAngleTable.end()));
	m_MinTheta = (*std::min_element(finalAngleTable.begin(),finalAngleTable.end()));
}

//-----------------------------------------------------------------------------

PlusStatus FidLineFinder::ReadConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("FidSegmentation::ReadSegmentationParametersConfiguration"); 
	if ( configData == NULL) 
	{
		LOG_WARNING("Unable to read the SegmentationParameters XML data element!"); 
		return PLUS_FAIL; 
	}

  vtkSmartPointer<vtkXMLDataElement> usCalibration = configData->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL)
  {
    LOG_ERROR("Cannot find USCalibration element in XML tree!");
    return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> segmentationParameters = usCalibration->FindNestedElementWithName("CalibrationController")->FindNestedElementWithName("SegmentationParameters");
	if (segmentationParameters == NULL)
  {
		LOG_ERROR("No Segmentation parameters is found in the XML tree!");
		return PLUS_FAIL;
	}

	// The input image dimensions (in pixels)
	int frameSize[2] = {0}; 
	if ( segmentationParameters->GetVectorAttribute("FrameSize", 2, frameSize) ) 
	{
		m_FrameSize[0] = frameSize[0];
    m_FrameSize[1] = frameSize[1]; 
	}
  else
  {
    LOG_WARNING("Could not read FrameSize from configuration file.");
  }

  double approximateSpacingMmPerPixel(0.0); 
	if ( segmentationParameters->GetScalarAttribute("ApproximateSpacingMmPerPixel", approximateSpacingMmPerPixel) )
	{
		m_ApproximateSpacingMmPerPixel = approximateSpacingMmPerPixel; 
	}
  else
  {
    LOG_WARNING("Could not read ApproximateSpacingMmPerPixel from configuration file.");
  }
	
	//if the tolerance parameters are computed automatically
	int computeSegmentationParametersFromPhantomDefinition(0);
	if(segmentationParameters->GetScalarAttribute("ComputeSegmentationParametersFromPhantomDefinition", computeSegmentationParametersFromPhantomDefinition)
		&& computeSegmentationParametersFromPhantomDefinition!=0 )
	{
		vtkSmartPointer<vtkXMLDataElement> phantomDefinition = segmentationParameters->GetRoot()->FindNestedElementWithName("PhantomDefinition");
		if (phantomDefinition == NULL)
		{
			LOG_ERROR("No phantom definition is found in the XML tree!");
		}
		vtkSmartPointer<vtkXMLDataElement> customTransforms = phantomDefinition->FindNestedElementWithName("CustomTransforms"); 
		if (customTransforms == NULL) 
		{
			LOG_ERROR("Custom transforms are not found in phantom model");
		}
		else
		{
			LOG_ERROR("Unable to read image to phantom transform!"); 
		}

		double* imageNormalVectorInPhantomFrameMaximumRotationAngleDeg = new double[6];
		if ( segmentationParameters->GetVectorAttribute("ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg", 6, imageNormalVectorInPhantomFrameMaximumRotationAngleDeg) )
		{
			for( int i = 0; i<6 ; i++)
			{
				m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[i] = imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[i];
			}
		}
    else
    {
      LOG_WARNING("Could not read ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg from configuration file.");
    }
		delete [] imageNormalVectorInPhantomFrameMaximumRotationAngleDeg;

		double imageToPhantomTransformVector[16]={0}; 
		if (customTransforms->GetVectorAttribute("ImageToPhantomTransform", 16, imageToPhantomTransformVector)) 
		{
			for( int i = 0; i<16 ; i++)
			{
				m_ImageToPhantomTransform[i] = imageToPhantomTransformVector[i];
			}
		}
    else
    {
      LOG_WARNING("Could not read ImageToPhantomTranform from configuration file.");
    }

		//Will be computed automatically in the future
		double maxLineLengthErrorPercent(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLineLengthErrorPercent", maxLineLengthErrorPercent) )
		{
			m_MaxLineLengthErrorPercent = maxLineLengthErrorPercent; 
		}
    else
    {
      LOG_WARNING("Could not read maxLineLengthErrorPercent from configuration file.");
    }

		double findLines3PtDist(0.0); 
		if ( segmentationParameters->GetScalarAttribute("FindLines3PtDistanceMm", findLines3PtDist) )
		{
			m_FindLines3PtDistanceMm = findLines3PtDist; 
		}
    else
    {
      LOG_WARNING("Could not read FindLines3PtDistanceMm from configuration file.");
    }

		double maxLineErrorMm(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLineErrorMm", maxLineErrorMm) )
		{
			m_MaxLineErrorMm = maxLineErrorMm; 
		}
    else
    {
      LOG_WARNING("Could not read maxLineErrorMm from configuration file.");
    }

    ComputeParameters();
	}
	else
	{
		double maxLineLengthErrorPercent(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLineLengthErrorPercent", maxLineLengthErrorPercent) )
		{
			m_MaxLineLengthErrorPercent = maxLineLengthErrorPercent; 
		}
    else
    {
      LOG_WARNING("Could not read maxLineLengthErrorPercent from configuration file.");
    }

		double minThetaDegrees(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MinThetaDegrees", minThetaDegrees) )
		{
			m_MinTheta = minThetaDegrees * vtkMath::Pi() / 180.0; 
		}
    else
    {
      LOG_WARNING("Could not read minThetaDegrees from configuration file.");
    }

		double maxThetaDegrees(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxThetaDegrees", maxThetaDegrees) )
		{
			m_MaxTheta = maxThetaDegrees * vtkMath::Pi() / 180.0; 
		}
    else
    {
      LOG_WARNING("Could not read maxThetaDegrees from configuration file.");
    }

		double findLines3PtDist(0.0); 
		if ( segmentationParameters->GetScalarAttribute("FindLines3PtDistanceMm", findLines3PtDist) )
		{
			m_FindLines3PtDistanceMm = findLines3PtDist; 
		}
    else
    {
      LOG_WARNING("Could not read FindLines3PtDistanceMm from configuration file.");
    }

		double maxLineErrorMm(0.0); 
		if ( segmentationParameters->GetScalarAttribute("MaxLineErrorMm", maxLineErrorMm) )
		{
			m_MaxLineErrorMm = maxLineErrorMm; 
		}
    else
    {
      LOG_WARNING("Could not read maxLineErrorMm from configuration file.");
    }
	}

  UpdateParameters();
  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------

float FidLineFinder::ComputeSlope( Dot *dot1, Dot *dot2 )
{
	float x1 = dot1->GetX() - 1;
	float y1 = m_FrameSize[1] - dot1->GetY();

	float x2 = dot2->GetX() - 1;
	float y2 = m_FrameSize[1] - dot2->GetY();

	float y = (y2 - y1);
	float x = (x2 - x1);

	// :TODO: check if it can be simplified by using atan2 instead of atan+fabsf

	float t;
	if ( fabsf(x) > fabsf(y) )
		t = vtkMath::Pi()/2 + atan( y / x );
	else {
		float tanTheta = x / y;
		if ( tanTheta > 0 )
			t = vtkMath::Pi() - atan( tanTheta );
		else
			t = -atan( tanTheta );
	}
	assert( t >= 0 && t <= vtkMath::Pi() );
	return t;
}

//-----------------------------------------------------------------------------

void FidLineFinder::ComputeLine( Line &line, std::vector<Dot> dots )
{
	int ptnum[3];
	for (int i=0; i<3; i++)
	{
		ptnum[i] = line.GetLinePoint(i);
	}

	float t[3];
	t[0] = ComputeSlope( &dots[ptnum[0]], &dots[ptnum[1]] );
	t[1] = ComputeSlope( &dots[ptnum[0]], &dots[ptnum[2]] );
	t[2] = ComputeSlope( &dots[ptnum[1]], &dots[ptnum[2]] );

	while ( t[1] - t[0] > vtkMath::Pi()/2 )
		t[1] -= vtkMath::Pi();
	while ( t[0] - t[1] > vtkMath::Pi()/2 )
		t[1] += vtkMath::Pi();

	while ( t[2] - t[0] > vtkMath::Pi()/2 )
		t[2] -= vtkMath::Pi();
	while ( t[0] - t[2] > vtkMath::Pi()/2 )
		t[2] += vtkMath::Pi();
		
	float tMean = ( t[0] + t[1] + t[2] ) / 3;

	while ( tMean >= vtkMath::Pi() )
		tMean = tMean - vtkMath::Pi();
	while ( t < 0 )
		tMean = tMean + vtkMath::Pi();
	
	line.SetLineSlope(tMean);
	line.SetLinePosition(0);

	float x[3];
	float y[3];
	float p[3];
	
	for (int i=0; i<3; i++)
	{
		x[i] = dots[ptnum[i]].GetX()  - 1;
		y[i] = m_FrameSize[1] - dots[ptnum[i]].GetY();
		p[i] = x[i] * cos(tMean) + y[i] * sin(tMean);
	}

	float pMean = (p[0] + p[1] + p[2]) / 3;
	line.SetLinePosition(pMean);
	line.SetLineIntensity(dots[ptnum[0]].GetDotIntensity() + dots[ptnum[1]].GetDotIntensity() + 
			dots[ptnum[2]].GetDotIntensity());
	line.SetLineError(fabsf(p[0]-pMean) + fabsf(p[1]-pMean) + fabsf(p[2]-pMean));
	line.SetLineLength(LineLength( line, dots ));
}

//-----------------------------------------------------------------------------

float FidLineFinder::SegmentLength( Dot *d1, Dot *d2 )
{
	float xd = d2->GetX() - d1->GetX();
	float yd = d2->GetY() - d1->GetY();
	return sqrtf( xd*xd + yd*yd );
}

//-----------------------------------------------------------------------------

float FidLineFinder::LineLength( Line &line, std::vector<Dot> dots )
{
	float l1 = SegmentLength( &dots[line.GetLinePoint(0)], &dots[line.GetLinePoint(1)] );
	float l2 = SegmentLength( &dots[line.GetLinePoint(0)], &dots[line.GetLinePoint(2)] );
	float l3 = SegmentLength( &dots[line.GetLinePoint(1)], &dots[line.GetLinePoint(2)] );

	if ( l2 > l1 )
		l1 = l2;
	return l3 > l1 ? l3 : l1;
}

//-----------------------------------------------------------------------------

void FidLineFinder::FindLines2Points()
{
	for ( int b1 = 0; b1 < m_DotsVector.size(); b1++ ) {
		float x1 = m_DotsVector[b1].GetX() - 1;
		float y1 = m_FrameSize[1] - m_DotsVector[b1].GetY();

		for ( int b2 = b1+1; b2 < m_DotsVector.size(); b2++ ) {
			Line twoPointsLine;
			twoPointsLine.GetLinePoints()->resize(2);
			//lines2pt[nlines2pt].GetLinePoints()->resize(2);//TODO: make it more general
			float x2 = m_DotsVector[b2].GetX() - 1;
			float y2 = m_FrameSize[1] - m_DotsVector[b2].GetY();

			float t = ComputeSlope( &m_DotsVector[b1], &m_DotsVector[b2] ); // get degree of slope (0 deg = parallel to -y axis)

			float p1 = x1 * cos(t) + y1 * sin(t);
			float p2 = x2 * cos(t) + y2 * sin(t);
			float p = (p1 + p2) / 2;

			twoPointsLine.SetLineSlope(t); 
			twoPointsLine.SetLinePosition(p);
			twoPointsLine.SetLinePoint(0, b1);
			twoPointsLine.SetLinePoint(1, b2);
			m_TwoPointsLinesVector.push_back(twoPointsLine);
		}
	}
}

//-----------------------------------------------------------------------------

void FidLineFinder::FindLines3Points( )
{
	/* For each point, loop over each 2-point line and try to make a 3-point
	 * line. For the third point use the theta of the line and compute a value
	 * for p. Accept the line if the compute p is within some small distance
	 * of the 2-point line. */

	
	int points[3];
	Line currentTwoPointsLine;
	float dist = m_FindLines3PtDistanceMm / m_ApproximateSpacingMmPerPixel;
	for ( int b3 = 0; b3 < m_DotsVector.size(); b3++ ) 
	{
		float x3 = m_DotsVector[b3].GetX() - 1;
		float y3 = m_FrameSize[1] - m_DotsVector[b3].GetY();

		for ( int l = 0; l < m_TwoPointsLinesVector.size(); l++ ) 
		{
			currentTwoPointsLine = m_TwoPointsLinesVector[l];
			float t = currentTwoPointsLine.GetLineSlope();
			float p = currentTwoPointsLine.GetLinePosition();
			int b1 = currentTwoPointsLine.GetLinePoint(0);
			int b2 = currentTwoPointsLine.GetLinePoint(1);

			if ( b3 != b1 && b3 != b2 ) 
			{
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

					if ( ! std::binary_search(m_LinesVector.begin(), m_LinesVector.end(),line, Line::compareLines) ) 
					{
						ComputeLine( line, m_DotsVector );
						if ( AcceptLine( line ) ) 
						{
							//line.SetLineSlope(0);
							//line.SetLinePosition(0);
							m_LinesVector.push_back(line);
							//UltraSoundFiducialSegmentationTools::sort<Line, Line>( lines, lines.size() );
							// sort the lines so that lines that are already in the list can be quickly found by a binary search
							std::sort (m_LinesVector.begin(), m_LinesVector.end(), Line::compareLines);
							//UltraSoundFiducialSegmentationTools::BinarySearchInsert<Line, Line>( line, lines, nlines );
						}
					}
				}
			}
		}
	}
	
  //std::sort (lines.begin(), lines.end(), Line::lessThan);
}

//-----------------------------------------------------------------------------

void FidLineFinder::Clear()
{
  m_DotsVector.clear();
  m_LinesVector.clear();
	m_TwoPointsLinesVector.clear();
  m_CandidateFidValues.clear();
}

//-----------------------------------------------------------------------------

void FidLineFinder::SortRightToLeft( Line *line )
{
	/* Since we prohibit stepp lines (see MAX_T and MIN_T) we can use the x
	 * values to sort the points. */
	std::vector<std::vector<Dot>::iterator> pointsIterator(3);//TODO Make it general

	for (int i=0; i<3; i++)
	{
		pointsIterator[i] = m_DotsVector.begin() + line->GetLinePoint(i);
	}
	//UltraSoundFiducialSegmentationTools::sort<std::vector<Dot>::iterator, Position>( pointsIterator, line->GetLinePoints()->size() );
	std::sort (pointsIterator.begin(), pointsIterator.end(), Position::lessThan);
	//std::sort(points.begin(),points.end(), Position::lessThan);

	for (int i=0; i<3; i++)
	{
		line->SetLinePoint(i,pointsIterator[i] - m_DotsVector.begin());
	}
}

//-----------------------------------------------------------------------------

bool FidLineFinder::AcceptLine( Line &line )
{
	int maxLineLenPx = floor(m_MaxLineLenMm / m_ApproximateSpacingMmPerPixel + 0.5 );
	int minLineLenPx = floor(m_MinLineLenMm / m_ApproximateSpacingMmPerPixel + 0.5 );
	double maxLineErrorPx = m_MaxLineErrorMm / m_ApproximateSpacingMmPerPixel;

	if ( line.GetLineLength() <= maxLineLenPx && line.GetLineLength() >= minLineLenPx &&
			line.GetLineError() <= maxLineErrorPx && 
			line.GetLineSlope() >= m_MinTheta && line.GetLineSlope() <= m_MaxTheta )
		return true;
	return false;
}

//-----------------------------------------------------------------------------

void FidLineFinder::FindLines( )
{
	// Make pairs of dots into 2-point lines.
	FindLines2Points();

	// Make 2-point lines and dots into 3-point lines.
	FindLines3Points();

	// Sort by intensity.
	//UltraSoundFiducialSegmentationTools::sort<Line, Line>( lines, lines.size() );
	std::sort (m_LinesVector.begin(), m_LinesVector.end(), Line::lessThan);
}

//-----------------------------------------------------------------------------
