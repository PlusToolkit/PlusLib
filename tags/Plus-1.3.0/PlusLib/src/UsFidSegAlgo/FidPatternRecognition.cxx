/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "FidPatternRecognition.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkLine.h"

static const float DOT_STEPS  = 4.0;
static const float DOT_RADIUS = 6.0;

//-----------------------------------------------------------------------------

FidPatternRecognition::FidPatternRecognition()
{
  m_CurrentFrame = 0;
}

//-----------------------------------------------------------------------------

FidPatternRecognition::~FidPatternRecognition()
{
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
	LOG_TRACE("FidPatternRecognition::ReadConfiguration"); 

  if (ReadPhantomDefinition(rootConfigElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading phantom definition failed!");
    return PLUS_FAIL;
  }

	m_FidSegmentation.ReadConfiguration(rootConfigElement);
	m_FidLineFinder.ReadConfiguration(rootConfigElement);
  m_FidLabeling.ReadConfiguration(rootConfigElement, m_FidLineFinder.GetMinTheta(), m_FidLineFinder.GetMaxTheta(), m_FidLineFinder.GetMaxLineErrorMm());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::RecognizePattern(TrackedFrame* trackedFrame, PatternRecognitionResult &patternRecognitionResult)
{
	LOG_TRACE("FidPatternRecognition::RecognizePattern"); 

  if (RecognizePattern(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Recognizing pattern failed!");
    return PLUS_FAIL;
  }

  patternRecognitionResult.SetAngles(m_FidLabeling.GetAngleConf());
	patternRecognitionResult.SetIntensity(m_FidLabeling.GetLinePairIntensity());
	patternRecognitionResult.SetNumDots(m_FidLabeling.GetDotsVector().size()); 
	patternRecognitionResult.SetDotsFound(m_FidLabeling.GetDotsFound());
	patternRecognitionResult.SetFoundDotsCoordinateValue(m_FidLabeling.GetFoundDotsCoordinateValue());
	patternRecognitionResult.SetCandidateFidValues(m_FidSegmentation.GetCandidateFidValues());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::RecognizePattern(TrackedFrame* trackedFrame)
{
	LOG_TRACE("FidPatternRecognition::RecognizePattern"); 

  m_FidSegmentation.Clear();
  m_FidLineFinder.Clear();
  m_FidLabeling.Clear();
  
  m_CurrentFrame++;

  m_FidSegmentation.SetFrameSize(trackedFrame->GetFrameSize());
  m_FidLineFinder.SetFrameSize(trackedFrame->GetFrameSize());
  m_FidLabeling.SetFrameSize(trackedFrame->GetFrameSize());

  int bytes = trackedFrame->GetFrameSize()[0] * trackedFrame->GetFrameSize()[1] * sizeof(PixelType);
  PixelType* image = reinterpret_cast<PixelType*>(trackedFrame->GetImageData()->GetBufferPointer());

  memcpy( m_FidSegmentation.GetWorking(), image, bytes );
	memcpy( m_FidSegmentation.GetUnalteredImage(), image, bytes);

  m_FidSegmentation.MorphologicalOperations();

  m_FidSegmentation.Suppress( m_FidSegmentation.GetWorking(), m_FidSegmentation.GetThresholdImagePercent()/100.00 );

  m_FidSegmentation.Cluster();

  m_FidLabeling.SetNumDots(m_FidSegmentation.GetDotsVector().size()); 
	m_FidSegmentation.SetCandidateFidValues(m_FidSegmentation.GetDotsVector());	 
  //m_FidSegmentation.SetDebugOutput(true);//for testing purpose only

  m_FidLineFinder.SetCandidateFidValues(m_FidSegmentation.GetCandidateFidValues());
  m_FidLineFinder.SetDotsVector(m_FidSegmentation.GetDotsVector());
  m_FidLabeling.SetDotsVector(m_FidSegmentation.GetDotsVector());

	m_FidLineFinder.FindLines();

  if(m_FidLineFinder.GetLinesVector().size() > 3)
  {
    m_FidLabeling.SetLinesVector(m_FidLineFinder.GetLinesVector());

    m_FidLabeling.FindPattern();
  }

  if(m_FidSegmentation.GetDebugOutput()) 
	{
		m_FidSegmentation.WritePossibleFiducialOverlayImage(m_FidLabeling.GetFoundDotsCoordinateValue(), m_FidSegmentation.GetUnalteredImage(), m_CurrentFrame); 
    //m_FidSegmentation.WritePossibleFiducialOverlayImage(m_FidSegmentation.GetCandidateFidValues(), m_FidSegmentation.GetUnalteredImage(), m_CurrentFrame); 
	}

  // Set results
  std::vector< std::vector<double> > fiducials = m_FidLabeling.GetFoundDotsCoordinateValue();

  vtkSmartPointer<vtkPoints> fiducialPoints = vtkSmartPointer<vtkPoints>::New();
  fiducialPoints->SetNumberOfPoints(fiducials.size());

  for (int i = 0; i<fiducials.size(); ++i)
  {
    fiducialPoints->InsertPoint(i, fiducials[i][0], fiducials[i][1], 0.0);
  }
  fiducialPoints->Modified();

  trackedFrame->SetFiducialPointsCoordinatePx(fiducialPoints);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::RecognizePattern(vtkTrackedFrameList* trackedFrameList, int* numberOfSuccessfullySegmentedImages/*=NULL*/)
{
	LOG_TRACE("FidPatternRecognition::DrawResults"); 

  PlusStatus status = PLUS_SUCCESS;
  if ( numberOfSuccessfullySegmentedImages )
  {
    *numberOfSuccessfullySegmentedImages = 0; 
  }

  for ( int currentFrameIndex = 0; currentFrameIndex < trackedFrameList->GetNumberOfTrackedFrames(); currentFrameIndex++)
  {
    TrackedFrame * trackedFrame = trackedFrameList->GetTrackedFrame(currentFrameIndex); 

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL // segment only non segmented frames
      && RecognizePattern(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Recognizing pattern failed on frame " << currentFrameIndex);
      status = PLUS_FAIL;
    }

    if ( numberOfSuccessfullySegmentedImages )
    {
      // compute the number of successfully segmented images 
      if ( trackedFrame->GetFiducialPointsCoordinatePx()
        && trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() > 0 )
      {
        *numberOfSuccessfullySegmentedImages = *numberOfSuccessfullySegmentedImages + 1;     
      }
    }
  }

  return status;
}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawResults( PixelType *image )
{
	LOG_TRACE("FidPatternRecognition::DrawResults"); 

  if ( m_FidLabeling.GetPairsVector().size() > 0 )
  {
		DrawPair( image, m_FidLabeling.GetPairsVector().begin() );
  }
	else
	{
    std::vector<Line> maxPointsLines = m_FidLabeling.GetLinesVector()[m_FidLabeling.GetLinesVector().size()-1];
		cout << "ERROR: could not find the pair of the wires!  See other drawing outputs for more information!" << endl;
		DrawLines( image, maxPointsLines.begin(), maxPointsLines.size() );
		DrawDots( image, m_FidLabeling.GetDotsVector().begin(), m_FidLabeling.GetDotsVector().size() );
	}

}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawDots( PixelType *image, std::vector<Dot>::iterator dotsIterator, int ndots)
{
	LOG_TRACE("FidPatternRecognition::DrawDots"); 

	for ( int d = 0; d < ndots; d++ ) {
		float row = dotsIterator[d].GetY();
		float col = dotsIterator[d].GetX();

		for ( float t = 0; t < 2*vtkMath::Pi(); t += vtkMath::Pi()/DOT_STEPS ) {
			int r = (int)floor( row + cos(t) * DOT_RADIUS );
			int c = (int)floor( col + sin(t)* DOT_RADIUS );

			if ( r >= 0 && r < m_FidSegmentation.GetFrameSize()[1] && c >= 0 && c <=  m_FidSegmentation.GetFrameSize()[0] )
      {
        image[r* m_FidSegmentation.GetFrameSize()[0]+c] = UCHAR_MAX;
      }
		}

		image[static_cast<int>(floor(row)* m_FidSegmentation.GetFrameSize()[0]+floor(col))] = 0; 
	}
}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawLines( PixelType *image, std::vector<Line>::iterator linesIterator, int nlines )
{
	LOG_TRACE("FidPatternRecognition::DrawLines"); 

	for ( int l = 0; l < nlines; l++ )
	{
    float theta = Line::ComputeAngle(linesIterator[l]);
		float p = 0;//linesIterator[l].GetPosition(); TODO: draw lines in another manner

		for (int i=0; i<3; i++)
		{
			DrawDots( image, m_FidLabeling.GetDotsVector().begin()+linesIterator[l].GetPoint(i), 1 );//watch out, check for iterators problems if errors
		}		

		if ( theta < vtkMath::Pi()/4 || theta > 3*vtkMath::Pi()/4 ) {
			for ( int y = 0; y <  m_FidSegmentation.GetFrameSize()[1]; y++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float x = roundf(( p - y * sin(theta) ) / cos(theta));
				double x = floor( ( p - y * sin(theta) ) / cos(theta) + 0.5 );
				int r =  m_FidSegmentation.GetFrameSize()[1] - y - 1;
				int c = (unsigned int)x;
				if ( c >= 0 && c <  m_FidSegmentation.GetFrameSize()[0] )
					image[r* m_FidSegmentation.GetFrameSize()[0]+c] = UCHAR_MAX;
			}
		}
		else {
			for ( int x = 0; x < m_FidSegmentation.GetFrameSize()[0]; x++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float y = roundf(( p - x * cos(theta) ) / sin(theta));
				double y = floor( ( p - x * cos(theta) ) / sin(theta) + 0.5 );
				int r =  m_FidSegmentation.GetFrameSize()[1] - (unsigned int)y - 1;
				int c = x;
				if ( r >= 0 && r <  m_FidSegmentation.GetFrameSize()[1] )
					image[r* m_FidSegmentation.GetFrameSize()[0]+c] = UCHAR_MAX;
			}
		}
	}
}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawPair( PixelType *image, std::vector<LinePair>::iterator pairIterator )
{
	LOG_TRACE("FidPatternRecognition::DrawPair"); 

  std::vector<Line> maxPointsLines = m_FidLabeling.GetLinesVector()[m_FidLabeling.GetLinesVector().size()-1];

	/* Drawing on the original. */
	DrawLines( image, maxPointsLines.begin()+pairIterator->GetLine1(), 1 );
	DrawLines( image,maxPointsLines.begin()+pairIterator->GetLine2(), 1 );
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::ComputeNWireIntersections()
{
	LOG_TRACE("FidPatternRecognition::ComputeNWireInstersections");

	

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::ReadPhantomDefinition(vtkXMLDataElement* config)
{
	LOG_TRACE("FidPatternRecognition::ReadPhantomDefinition");

	if ( config == NULL )
	{
		LOG_ERROR("Configuration XML data element is NULL"); 
		return PLUS_FAIL;
	}

  bool nwireFlag = false;

	vtkSmartPointer<vtkXMLDataElement> phantomDefinition = config->FindNestedElementWithName("PhantomDefinition");
	if (phantomDefinition == NULL)
  {
		LOG_ERROR("No phantom definition is found in the XML tree!");
		return PLUS_FAIL;
	}
  else
	{
    std::vector<Pattern*> tempPatterns;

		// Load geometry
		vtkSmartPointer<vtkXMLDataElement> geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
		if (geometry == NULL) 
    {
			LOG_ERROR("Phantom geometry information not found!");
			return PLUS_FAIL;
		} 
    else 
    {
			// Finding of Patterns and extracting the endpoints
			int numberOfGeometryChildren = geometry->GetNumberOfNestedElements();
			for (int i=0; i<numberOfGeometryChildren; ++i) 
      {
				vtkSmartPointer<vtkXMLDataElement> patternElement = geometry->GetNestedElement(i);

				if ((patternElement == NULL) || (STRCASECMP("Pattern", patternElement->GetName()))) 
        {
					continue;
				}

			  NWire * nWire = new NWire;
        CoplanarParallelWires * coplanarParallelWires = new CoplanarParallelWires;

			  int numberOfWires = patternElement->GetNumberOfNestedElements();

			  if ((numberOfWires != 3) && !(STRCASECMP("NWire", patternElement->GetAttribute("Type")))) 
        {
				  LOG_WARNING("NWire contains unexpected number of wires - skipped");
				  continue;
			  }

			  for (int j=0; j<numberOfWires; ++j) 
        {
				  vtkSmartPointer<vtkXMLDataElement> wireElement = patternElement->GetNestedElement(j);

				  if (wireElement == NULL) 
          {
					  LOG_WARNING("Invalid Wire description in Pattern - skipped");
					  break;
				  }

				  Wire wire;

				  const char* wireName =  wireElement->GetAttribute("Name"); 
				  if ( wireName != NULL )
				  {
					  wire.name = wireName;
				  }
				  if (! wireElement->GetVectorAttribute("EndPointFront", 3, wire.endPointFront)) 
          {
					  LOG_WARNING("Wrong wire end point detected - skipped");
					  continue;
				  }
				  if (! wireElement->GetVectorAttribute("EndPointBack", 3, wire.endPointBack)) 
          {
					  LOG_WARNING("Wrong wire end point detected - skipped");
					  continue;
				  }
          
          if(STRCASECMP("CoplanarParallelWires", patternElement->GetAttribute("Type")) == 0)
          {
            coplanarParallelWires->wires.push_back(wire);
          }
          else if(STRCASECMP("NWire", patternElement->GetAttribute("Type")) == 0)
          {
            nWire->wires.push_back(wire);
          }
			  }

        if(STRCASECMP("CoplanarParallelWires", patternElement->GetAttribute("Type")) == 0)
        {
          tempPatterns.push_back(coplanarParallelWires);

          if(i == 1)
          {
            tempPatterns[i]->distanceToOriginMm.push_back(0);
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(0);
            tempPatterns[i]->distanceToOriginMm.push_back(10*std::sqrt(2.0));
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
            tempPatterns[i]->distanceToOriginMm.push_back(20*std::sqrt(2.0));
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
            tempPatterns[i]->distanceToOriginMm.push_back(30*std::sqrt(2.0));
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
            tempPatterns[i]->distanceToOriginMm.push_back(40*std::sqrt(2.0));
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
          }
          else
          {
            tempPatterns[i]->distanceToOriginMm.push_back(0);
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(0);
            tempPatterns[i]->distanceToOriginMm.push_back(10);
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
            tempPatterns[i]->distanceToOriginMm.push_back(20);
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
            tempPatterns[i]->distanceToOriginMm.push_back(30);
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
            tempPatterns[i]->distanceToOriginMm.push_back(40);
            tempPatterns[i]->distanceToOriginToleranceMm.push_back(2);
          }
        }
        else if(STRCASECMP("NWire", patternElement->GetAttribute("Type")) == 0)
        {
          tempPatterns.push_back(nWire);

          tempPatterns[i]->distanceToOriginMm.push_back(0);
          tempPatterns[i]->distanceToOriginToleranceMm.push_back(0);
          double midMean[2] = {(tempPatterns[i]->wires[1].endPointBack[0]+tempPatterns[i]->wires[1].endPointFront[0])/2,(tempPatterns[i]->wires[1].endPointBack[1]+tempPatterns[i]->wires[1].endPointFront[1])/2};
          double distMidToOrigin = sqrt((tempPatterns[i]->wires[0].endPointBack[0]-midMean[0])*(tempPatterns[i]->wires[0].endPointBack[0]-midMean[0])+(tempPatterns[i]->wires[0].endPointBack[1]-midMean[1])*(tempPatterns[i]->wires[0].endPointBack[1]-midMean[1]));
          tempPatterns[i]->distanceToOriginMm.push_back(distMidToOrigin);
          tempPatterns[i]->distanceToOriginToleranceMm.push_back(15);
          double distEndToOrigin = sqrt((tempPatterns[i]->wires[0].endPointBack[0]-tempPatterns[i]->wires[2].endPointBack[0])*(tempPatterns[i]->wires[0].endPointBack[0]-tempPatterns[i]->wires[2].endPointBack[0])+(tempPatterns[i]->wires[0].endPointBack[1]-tempPatterns[i]->wires[2].endPointBack[1])*(tempPatterns[i]->wires[0].endPointBack[1]-tempPatterns[i]->wires[2].endPointBack[1]));
          tempPatterns[i]->distanceToOriginMm.push_back(distEndToOrigin);
          tempPatterns[i]->distanceToOriginToleranceMm.push_back(4);

          nwireFlag = true;
        }
		  }
		}

    if(nwireFlag)
    {
      double alphaTopLayerFrontWall = -1.0;
	    double alphaTopLayerBackWall = -1.0;
	    double alphaBottomLayerFrontWall = -1.0;
	    double alphaBottomLayerBackWall = -1.0;

	    // Read input NWires and convert them to vnl vectors to easier processing
	    LOG_DEBUG("Endpoints of wires = ");

	    // List endpoints, check wire ids and NWire geometry correctness (wire order and locations) and compute intersections
	    for (int k=0 ; k<tempPatterns.size() ; ++k) 
      {
		    int layer = k;

		    for (int i=0; i<3; ++i) 
        {
			    vnl_vector<double> endPointFront(3);
			    vnl_vector<double> endPointBack(3);
    		
			    for (int j=0; j<3; ++j) 
          {
				    endPointFront[j] = tempPatterns[k]->wires[i].endPointFront[j];
				    endPointBack[j] = tempPatterns[k]->wires[i].endPointBack[j];
			    }

			    LOG_DEBUG("\t Front endpoint of wire " << i << " on layer " << layer << " = " << endPointFront);
			    LOG_DEBUG("\t Back endpoint of wire " << i << " on layer " << layer << " = " << endPointBack);
		    }

		    /*if (sumLayer != layer * 9 + 6) 
        {
			    LOG_ERROR("Invalid NWire IDs (" << tempPatterns[k]->wires[0].id << ", " << tempPatterns[k]->wires[1].id << ", " << tempPatterns[k]->wires[2].id << ")!");
			    return PLUS_FAIL;
		    }*/

		    // Check if the middle wire is the diagonal (the other two are parallel to each other and the first and the second, and the second and the third intersect)
		    double wire1[3];
		    double wire3[3];
		    double cross[3];

        vtkMath::Subtract(tempPatterns[k]->wires[0].endPointFront, tempPatterns[k]->wires[0].endPointBack, wire1);
		    vtkMath::Subtract(tempPatterns[k]->wires[2].endPointFront, tempPatterns[k]->wires[2].endPointBack, wire3);
		    vtkMath::Cross(wire1, wire3, cross);
		    if (vtkMath::Norm(cross) > 0.001) 
        {
			    LOG_ERROR("The first and third wire of layer " << layer << " are not parallel!");
			    return PLUS_FAIL;
		    }
		    double closestTemp[3];
		    double parametricCoord1, parametricCoord2;

        NWire * tempNWire = (NWire*)(tempPatterns[k]);

		    if (vtkLine::DistanceBetweenLines(tempNWire->wires[0].endPointFront, tempNWire->wires[0].endPointBack, tempNWire->wires[1].endPointFront, tempNWire->wires[1].endPointBack, tempNWire->intersectPosW12, closestTemp, parametricCoord1, parametricCoord2) > 0.000001) 
        {
			    LOG_ERROR("The first and second wire of layer " << layer << " do not intersect each other!");
			    return PLUS_FAIL;
		    }
		    if (vtkLine::DistanceBetweenLines(tempNWire->wires[2].endPointFront, tempNWire->wires[2].endPointBack, tempNWire->wires[1].endPointFront, tempNWire->wires[1].endPointBack, tempNWire->intersectPosW32, closestTemp, parametricCoord1, parametricCoord2) > 0.000001) 
        {
			    LOG_ERROR("The second and third wire of layer " << layer << " do not intersect each other!");
			    return PLUS_FAIL;
		    }
	    }

	    /*// Log the data pipeline if requested.
	    int layer;
	    for (int i=0, layer = 0; i<tempPatterns.size(); ++i, ++layer) 
      {
		    LOG_DEBUG("\t Intersection of wire 1 and 2 in layer " << layer << " \t= (" << it->intersectPosW12[0] << ", " << it->intersectPosW12[1] << ", " << it->intersectPosW12[2] << ")");
		    LOG_DEBUG("\t Intersection of wire 3 and 2 in layer " << layer << " \t= (" << it->intersectPosW32[0] << ", " << it->intersectPosW32[1] << ", " << it->intersectPosW32[2] << ")");
	    }*/
    }

    m_FidLineFinder.SetPatterns(tempPatterns);
    m_FidLabeling.SetPatterns(tempPatterns);
	}

  return PLUS_SUCCESS;
}
