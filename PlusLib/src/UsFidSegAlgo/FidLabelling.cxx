#include "FidLabelling.h"

#include <algorithm>
#include <float.h>

#include "vtkTriangle.h"
#include "vtkPlane.h"

//-----------------------------------------------------------------------------

FidLabelling::FidLabelling()
{
  m_FrameSize[0] = -1;
  m_FrameSize[1] = -1;

	m_ApproximateSpacingMmPerPixel = -1.0;
	m_MaxAngleDiff = -1.0;
	m_MinLinePairDistMm = -1.0; 	
	m_MaxLinePairDistMm = -1.0;
	m_MaxLinePairDistanceErrorPercent = -1.0;
	m_NumDots = -1.0;
	m_MaxLineErrorMm = -1.0;
	m_MinTheta = -1.0;
	m_MaxTheta = -1.0;
	
	m_DotsFound = false;

	m_AngleConf = -1.0;
	m_LinePairIntensity = -1.0;
}

//-----------------------------------------------------------------------------

FidLabelling::~FidLabelling()
{

}

//-----------------------------------------------------------------------------

void FidLabelling::UpdateParameters()
{
	std::vector<NWire> nWires = m_NWires;

	// Distance between lines (= distance between planes of the N-wires)
	double maxNPlaneDistance = -1.0;
	double minNPlaneDistance = FLT_MAX;
	int numOfNWires = nWires.size();
	double epsilon = 0.001;

	// Compute normal of each NWire and evaluate the other wire endpoints if they are on the computed plane
	std::vector<vtkSmartPointer<vtkPlane>> planes;
	for (int i=0; i<numOfNWires; ++i) {
		double normal[3];
		vtkTriangle::ComputeNormal(nWires.at(i).wires[0].endPointFront, nWires.at(i).wires[0].endPointBack, nWires.at(i).wires[2].endPointFront, normal);

		vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
		plane->SetNormal(normal);
		plane->SetOrigin(nWires.at(i).wires[0].endPointFront);
		planes.push_back(plane);

		double distance1F = plane->DistanceToPlane(nWires.at(i).wires[1].endPointFront);
		double distance1B = plane->DistanceToPlane(nWires.at(i).wires[1].endPointBack);
		double distance2B = plane->DistanceToPlane(nWires.at(i).wires[2].endPointBack);

		if (distance1F > epsilon || distance1B > epsilon || distance2B > epsilon) {
			LOG_ERROR("NWire number " << i << " is invalid: the endpoints are not on the same plane");
		}
	}

	// Compute distances between each NWire pairs and determine the smallest and the largest distance
	for (int i=numOfNWires-1; i>0; --i) {
		for (int j=i-1; j>=0; --j) {
			double distance = planes.at(i)->DistanceToPlane(planes.at(j)->GetOrigin());

			if (maxNPlaneDistance < distance) {
				maxNPlaneDistance = distance;
			}
			if (minNPlaneDistance > distance) {
				minNPlaneDistance = distance;
			}
		}
	}

	m_MaxLinePairDistMm = maxNPlaneDistance * (1.0 + (m_MaxLinePairDistanceErrorPercent / 100.0));
	m_MinLinePairDistMm = minNPlaneDistance * (1.0 - (m_MaxLinePairDistanceErrorPercent / 100.0));
	LOG_DEBUG("Line pair distance - computed min: " << minNPlaneDistance << " , max: " << maxNPlaneDistance << ";  allowed min: " << m_MinLinePairDistMm << ", max: " << m_MaxLinePairDistMm);
}

//-----------------------------------------------------------------------------

void FidLabelling::ComputeParameters()
{
  // TODO: to be completed (currently the parameters are read from the config xml)
}

//-----------------------------------------------------------------------------

PlusStatus FidLabelling::ReadConfiguration( vtkXMLDataElement* configData, double minTheta, double maxTheta, double maxLineErrorMm )
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
    LOG_WARNING("Automatic computation of the MaxLinePairDistanceErrorPercent and MaxAngleDifferenceDegrees parameters are not yet supported, use the values that are in the config file");
	}
	
	double maxLinePairDistanceErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLinePairDistanceErrorPercent", maxLinePairDistanceErrorPercent) )
	{
		m_MaxLinePairDistanceErrorPercent = maxLinePairDistanceErrorPercent; 
	}
  else
  {
    LOG_WARNING("Could not read maxLinePairDistanceErrorPercent from configuration file.");
  }

	double maxAngleDifferenceDegrees(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxAngleDifferenceDegrees", maxAngleDifferenceDegrees) )
	{
    m_MaxAngleDiff = maxAngleDifferenceDegrees * vtkMath::Pi() / 180.0; 
	}
  else
  {
    LOG_WARNING("Could not read maxAngleDifferenceDegrees from configuration file.");
  }

  UpdateParameters();

	m_MinTheta = minTheta;
	m_MaxTheta = maxTheta;
	m_MaxLineErrorMm = maxLineErrorMm;

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------

void FidLabelling::Clear()
{
  m_DotsVector.clear();
  m_LinesVector.clear();
  m_PairsVector.clear();
  m_FoundDotsCoordinateValue.clear();
}

//-----------------------------------------------------------------------------

std::vector<std::vector<double>> FidLabelling::SortInAscendingOrder(std::vector<std::vector<double>> fiducials) 
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

void FidLabelling::FindDoubleNLines()
{	
	FindPairs();

  if ( m_PairsVector.size() < 1 ) 
	{
		LOG_DEBUG("Segmentation was NOT successful! (Number of dots found: " << GetFoundDotsCoordinateValue().size() << " Number of possible fiducial points: " << GetNumDots() << ")"); 
		SetDotsFound(false);				
		return;
	}

	SetDotsFound(true);

  LinePair *  pair = &m_PairsVector[0];
  SortTopToBottom( pair );

  Line *line1 = &m_LinesVector[pair->GetLine1()];
  Line *line2 = &m_LinesVector[pair->GetLine2()];

  SortRightToLeft( line1 );
  SortRightToLeft( line2 );

	std::vector<double> dotCoords;
	std::vector< std::vector<double> > foundDotsCoordinateValues = m_FoundDotsCoordinateValue;
	for (int i=0; i<3; i++)
	{
		dotCoords.push_back(m_DotsVector[line1->GetLinePoint(i)].GetX());
		dotCoords.push_back(m_DotsVector[line1->GetLinePoint(i)].GetY());
		foundDotsCoordinateValues.push_back(dotCoords);
		dotCoords.clear();
	}
	for (int i=0; i<3; i++)
	{
		dotCoords.push_back(m_DotsVector[line2->GetLinePoint(i)].GetX());
		dotCoords.push_back(m_DotsVector[line2->GetLinePoint(i)].GetY());
		foundDotsCoordinateValues.push_back(dotCoords);
		dotCoords.clear();
	}
	m_FoundDotsCoordinateValue = foundDotsCoordinateValues;
	std::vector<std::vector<double>> sortedFiducials = SortInAscendingOrder(m_FoundDotsCoordinateValue); 
	m_FoundDotsCoordinateValue = sortedFiducials; 					

	m_AngleConf = pair->GetAngleConf();
	m_LinePairIntensity = pair->GetLinePairIntensity();
	m_NumDots = m_DotsVector.size(); 
}

//-----------------------------------------------------------------------------

void FidLabelling::FindPairs()
{
	//These are for extra checks not needed in case of parallel lines as they are already checked when accepting the line
	double maxTheta = m_MaxTheta;
	double minTheta = m_MinTheta;

	int maxLinePairDistPx = floor(m_MaxLinePairDistMm / m_ApproximateSpacingMmPerPixel + 0.5 );
	int minLinePairDistPx = floor(m_MinLinePairDistMm / m_ApproximateSpacingMmPerPixel + 0.5 );
	double maxLineErrorPx = m_MaxLineErrorMm / m_ApproximateSpacingMmPerPixel;

	Line currentLine1, currentLine2;

	for ( unsigned int l1 = 0; l1 < m_LinesVector.size(); l1++ ) 
	{
		currentLine1 = m_LinesVector[l1];
		for ( unsigned int l2 = l1+1; l2 < m_LinesVector.size(); l2++ ) 
		{
			currentLine2 = m_LinesVector[l2];
			float t1 = currentLine1.GetLineSlope();
			float p1 = currentLine1.GetLinePosition();

			float t2 = currentLine2.GetLineSlope();
			float p2 = currentLine2.GetLinePosition();

			float angle_diff = fabsf( t2 - t1 );
			float line_error = currentLine1.GetLineError() + currentLine2.GetLineError();

			bool test1 = angle_diff < m_MaxAngleDiff;
			bool test2 = line_error < maxLineErrorPx;
			bool test3 = minTheta <= t1 && t1 <= maxTheta;
			bool test4 = minTheta <= t2 && t2 <= maxTheta;		
			bool test5 = fabsf( p2 - p1 ) < maxLinePairDistPx;// (old value was hardcoded as 65)
			bool test6 = fabsf( p2 - p1 ) > minLinePairDistPx;// (old value was hardcoded as 25)

			if ( test1 && test2 && /*test3 && test4 &&*/ test5 && test6 ) 
			{
				line_error = line_error / maxLineErrorPx;
				float angle_conf = angle_diff / ( 1 - line_error );

				if ( angle_conf < m_MaxAngleDiff ) 
				{
					float intensity = currentLine1.GetLineIntensity() + currentLine2.GetLineIntensity();
					LinePair linePair;
					linePair.SetLine1(l1);
					linePair.SetLine2(l2);
					linePair.SetLinePairIntensity(intensity);
					linePair.SetLinePairError(line_error);
					linePair.SetAngleDifference(angle_diff / m_MaxAngleDiff);
					linePair.SetAngleConf(angle_conf / m_MaxAngleDiff);

					m_PairsVector.push_back(linePair);
				}
			}
		}
	}

	/* Rank the pairs by intensity. */
	//UltraSoundFiducialSegmentationTools::sort<LinePair, LinePair>( pairs, pairs.size() );
	std::sort (m_PairsVector.begin(), m_PairsVector.end(), LinePair::lessThan);
}

//-----------------------------------------------------------------------------

void FidLabelling::SortTopToBottom( LinePair *pair )
{
	/* check if we need to swap the pairs. */
	if ( m_LinesVector[pair->GetLine1()].GetLinePosition() < m_LinesVector[pair->GetLine2()].GetLinePosition() ) {
		int swp_tmp = pair->GetLine1();
		pair->SetLine1(pair->GetLine2());
		pair->SetLine2(swp_tmp);
	}
}

//-----------------------------------------------------------------------------

void FidLabelling::SortRightToLeft( Line *line )
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
