/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "FidLabeling.h"

#include <algorithm>
#include <float.h>

#include "PlusMath.h"

#include "vtkTriangle.h"
#include "vtkPlane.h"

//-----------------------------------------------------------------------------
FidLabeling::FidLabeling()
{
  m_FrameSize[0]=0;
  m_FrameSize[1]=0;

  m_ApproximateSpacingMmPerPixel = -1.0;
  m_MaxAngleDiff = -1.0;
  m_MinLinePairDistMm = -1.0; 	
  m_MaxLinePairDistMm = -1.0;
  m_MinLinePairAngleRad = -1.0; 	
  m_MaxLinePairAngleRad = -1.0;
  m_MaxLineShiftMm = 10; // TODO: make it adjustable (https://www.assembla.com/spaces/plus/tickets/449)
  m_MaxLinePairDistanceErrorPercent = -1.0;
  m_MinThetaRad = -1.0;
  m_MaxThetaRad = -1.0;
  m_AngleToleranceRad = -1.0;
  m_InclinedLineAngleRad = 0.0;

  m_DotsFound = false;

  m_PatternIntensity = -1.0;
}

//-----------------------------------------------------------------------------

FidLabeling::~FidLabeling()
{
}

//-----------------------------------------------------------------------------

void FidLabeling::UpdateParameters()
{
  LOG_TRACE("FidLabeling::UpdateParameters");

  // Distance between lines (= distance between planes of the N-wires)
  int numOfPatterns = m_Patterns.size();
  double epsilon = 0.001;

  // Compute normal of each pattern and evaluate the other wire endpoints if they are on the computed plane
  std::vector<vtkSmartPointer<vtkPlane>> planes;
  for (int i=0; i<numOfPatterns; ++i) 
  {
    double normal[3]={0,0,0};
    vtkTriangle::ComputeNormal(m_Patterns[i]->Wires[0].EndPointFront, m_Patterns[i]->Wires[0].EndPointBack, m_Patterns[i]->Wires[2].EndPointFront, normal);

    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    plane->SetNormal(normal);
    plane->SetOrigin(m_Patterns[i]->Wires[0].EndPointFront);
    planes.push_back(plane);

    double distance1F = plane->DistanceToPlane(m_Patterns[i]->Wires[1].EndPointFront);
    double distance1B = plane->DistanceToPlane(m_Patterns[i]->Wires[1].EndPointBack);
    double distance2B = plane->DistanceToPlane(m_Patterns[i]->Wires[2].EndPointBack);

    if (distance1F > epsilon || distance1B > epsilon || distance2B > epsilon)
    {
      LOG_ERROR("Pattern number " << i << " is invalid: the endpoints are not on the same plane");
    }
  }
  // Compute distances between each NWire pairs and determine the smallest and the largest distance
  double maxNPlaneDistance = -1.0;
  double minNPlaneDistance = FLT_MAX;
  m_MinLinePairAngleRad = vtkMath::Pi()/2.0; 	
  m_MaxLinePairAngleRad = 0;
  for (int i=numOfPatterns-1; i>0; --i) 
  {
    for (int j=i-1; j>=0; --j) 
    {
      double distance = planes.at(i)->DistanceToPlane(planes.at(j)->GetOrigin());
      if (maxNPlaneDistance < distance) 
      {
        maxNPlaneDistance = distance;
      }
      if (minNPlaneDistance > distance) 
      {
        minNPlaneDistance = distance;
      }
      double angle = acos(vtkMath::Dot(planes.at(i)->GetNormal(),planes.at(j)->GetNormal())
        /vtkMath::Norm(planes.at(i)->GetNormal())
        /vtkMath::Norm(planes.at(j)->GetNormal()));
      // Normalize between -pi/2 .. +pi/2
      if (angle>vtkMath::Pi()/2)
      {
        angle -= vtkMath::Pi();
      }
      else if (angle<-vtkMath::Pi()/2)
      {
        angle += vtkMath::Pi();
      }
      // Return the absolute value (0..+pi/2)
      angle=fabs(angle);
      if (angle < m_MinLinePairAngleRad) 
      {
        m_MinLinePairAngleRad = angle;
      }
      if (angle > m_MaxLinePairAngleRad) 
      {
        m_MaxLinePairAngleRad = angle;
      }
    }
  }

  m_MaxLinePairDistMm = maxNPlaneDistance * (1.0 + (m_MaxLinePairDistanceErrorPercent / 100.0));
  m_MinLinePairDistMm = minNPlaneDistance * (1.0 - (m_MaxLinePairDistanceErrorPercent / 100.0));
  LOG_DEBUG("Line pair distance - computed min: " << minNPlaneDistance << " , max: " << maxNPlaneDistance << ";  allowed min: " << m_MinLinePairDistMm << ", max: " << m_MaxLinePairDistMm);
}

//-----------------------------------------------------------------------------

PlusStatus FidLabeling::ReadConfiguration( vtkXMLDataElement* configData, double minThetaRad, double maxThetaRad )
{
  LOG_TRACE("FidLabeling::ReadConfiguration");

  if ( configData == NULL) 
  {
    LOG_WARNING("Unable to read the SegmentationParameters XML data element!"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* segmentationParameters = configData->FindNestedElementWithName("Segmentation");
  if (segmentationParameters == NULL)
  {
    LOG_ERROR("Cannot find Segmentation element in XML tree!");
    return PLUS_FAIL;
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
    m_MaxAngleDiff = vtkMath::RadiansFromDegrees(maxAngleDifferenceDegrees); 
  }
  else
  {
    LOG_WARNING("Could not read maxAngleDifferenceDegrees from configuration file.");
  }

  double angleToleranceDegrees(0.0); 
  if ( segmentationParameters->GetScalarAttribute("AngleToleranceDegrees", angleToleranceDegrees) )
  {
    m_AngleToleranceRad = vtkMath::RadiansFromDegrees(angleToleranceDegrees); 
  }
  else
  {
    LOG_WARNING("Could not read AngleToleranceDegrees from configuration file.");
  }

  double inclinedLineAngleDegrees(0.0); 
  if ( segmentationParameters->GetScalarAttribute("InclinedLineAngleDegrees", inclinedLineAngleDegrees) )
  {
    m_InclinedLineAngleRad = vtkMath::RadiansFromDegrees(inclinedLineAngleDegrees); 
  }
  else
  {
    LOG_DEBUG("Could not read InclinedLineAngleDegrees from configuration file.");
  }

  UpdateParameters();

  m_MinThetaRad = minThetaRad;
  m_MaxThetaRad = maxThetaRad;

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------

void FidLabeling::Clear()
{
  //LOG_TRACE("FidLabeling::Clear");
  m_DotsVector.clear();
  m_LinesVector.clear();
  m_FoundDotsCoordinateValue.clear();
  m_Results.clear();
  m_FoundLines.clear();

  std::vector<Line> emptyLine;
  m_LinesVector.push_back(emptyLine);//initializing the 0 vector of lines (unused)
  m_LinesVector.push_back(emptyLine);//initializing the 1 vector of lines (unused)
}

//-----------------------------------------------------------------------------

void FidLabeling::SetFrameSize(int frameSize[2])
{
  LOG_TRACE("FidLineFinder::SetFrameSize(" << frameSize[0] << ", " << frameSize[1] << ")");

  if ((m_FrameSize[0] == frameSize[0]) && (m_FrameSize[1] == frameSize[1]))
  {
    return;
  }

  m_FrameSize[0] = frameSize[0];
  m_FrameSize[1] = frameSize[1];

  if (m_FrameSize[0] < 0 || m_FrameSize[1] < 0)
  {
    LOG_ERROR("Dimensions of the frame size are not positive!");
    return;
  }
}

//-----------------------------------------------------------------------------

void FidLabeling::SetAngleToleranceDegrees(double value)
{
  m_AngleToleranceRad = vtkMath::RadiansFromDegrees(value);
}

//-----------------------------------------------------------------------------

bool FidLabeling::SortCompare(std::vector<double> temporaryLine1, std::vector<double> temporaryLine2)
{
  //used for SortPointsByDistanceFromOrigin
  return temporaryLine1[1] < temporaryLine2[1];
}

//-----------------------------------------------------------------------------

Line FidLabeling::SortPointsByDistanceFromStartPoint(Line fiducials) 
{
  std::vector<std::vector<double>> temporaryLine;
  Dot startPointIndex = m_DotsVector[fiducials.GetStartPointIndex()];

  for(int i=0 ; i<fiducials.GetPoints()->size() ; i++)
  {
    std::vector<double> temp;
    Dot point = m_DotsVector[fiducials.GetPoint(i)];
    temp.push_back(fiducials.GetPoint(i));
    temp.push_back(sqrt((startPointIndex.GetX()-point.GetX())*(startPointIndex.GetX()-point.GetX()) + (startPointIndex.GetY()-point.GetY())*(startPointIndex.GetY()-point.GetY())));
    temporaryLine.push_back(temp);
  }

  //sort the indexes by the distance of their respective pioint to the startPointIndex
  std::sort(temporaryLine.begin(),temporaryLine.end(), FidLabeling::SortCompare); 

  Line resultLine = fiducials;

  for(int i=0 ; i<fiducials.GetPoints()->size() ; i++)
  {
    resultLine.SetPoint(i,temporaryLine[i][0]);
  }

  return resultLine;
}

//-----------------------------------------------------------------------------

float FidLabeling::ComputeSlope( Line &line )
{
  //LOG_TRACE("FidLineFinder::ComputeSlope");
  Dot dot1 = m_DotsVector[line.GetStartPointIndex()];
  Dot dot2 = m_DotsVector[line.GetEndPointIndex()];

  float x1 = dot1.GetX();
  float y1 = dot1.GetY();

  float x2 = dot2.GetX();
  float y2 = dot2.GetY();

  float y = (y2 - y1);
  float x = (x2 - x1);

  float t;
  if ( fabsf(x) > fabsf(y) )
  {
    t = vtkMath::Pi()/2 + atan( y / x );
  }
  else 
  {
    float tanTheta = x / y;
    if ( tanTheta > 0 )
    {
      t = vtkMath::Pi() - atan( tanTheta );
    }
    else
    {
      t = -atan( tanTheta );
    }
  }

  assert( t >= 0 && t <= vtkMath::Pi() );
  return t;
}

//-----------------------------------------------------------------------------

float FidLabeling::ComputeDistancePointLine(Dot dot, Line line)
{     
  double x[3], y[3], z[3];

  x[0] = m_DotsVector[line.GetStartPointIndex()].GetX();
  x[1] = m_DotsVector[line.GetStartPointIndex()].GetY();
  x[2] = 0;

  y[0] = m_DotsVector[line.GetEndPointIndex()].GetX();
  y[1] = m_DotsVector[line.GetEndPointIndex()].GetY();
  y[2] = 0;

  z[0] = dot.GetX();
  z[1] = dot.GetY();
  z[2] = 0;

  return PlusMath::ComputeDistanceLinePoint( x, y, z);
}

//-----------------------------------------------------------------------------

float FidLabeling::ComputeShift(Line line1, Line line2)
{
  //middle of the line 1
  double midLine1[2]=
  {
    (m_DotsVector[line1.GetStartPointIndex()].GetX()+m_DotsVector[line1.GetEndPointIndex()].GetX())/2,
    (m_DotsVector[line1.GetStartPointIndex()].GetY()+m_DotsVector[line1.GetEndPointIndex()].GetY())/2
  };
  //middle of the line 2
  double midLine2[2]=
  {
    (m_DotsVector[line2.GetStartPointIndex()].GetX()+m_DotsVector[line2.GetEndPointIndex()].GetX())/2,
    (m_DotsVector[line2.GetStartPointIndex()].GetY()+m_DotsVector[line2.GetEndPointIndex()].GetY())/2
  };
  //vector from one middle to the other
  double midLine1_to_midLine2[3]=
  {
    midLine2[0]-midLine1[0],
    midLine2[1]-midLine1[1],
    0
  };

  double line1vector[3]=
  {
    m_DotsVector[line1.GetEndPointIndex()].GetX()-m_DotsVector[line1.GetStartPointIndex()].GetX(),
    m_DotsVector[line1.GetEndPointIndex()].GetY()-m_DotsVector[line1.GetStartPointIndex()].GetY(),
    0
  };
  vtkMath::Normalize(line1vector);//need to normalize for the dot product to provide significant result

  double midLine1_to_midLine2_projectionToLine1_length = vtkMath::Dot(line1vector,midLine1_to_midLine2);

  return midLine1_to_midLine2_projectionToLine1_length;
}

//-----------------------------------------------------------------------------

void FidLabeling::UpdateNWiresResults(std::vector<Line*> resultLines)
{
  int numberOfLines = m_Patterns.size(); //the number of lines in the pattern
  float intensity = 0;
  std::vector<double> dotCoords;
  std::vector< std::vector<double> > foundDotsCoordinateValues = m_FoundDotsCoordinateValue;

  for (int i=0; i<numberOfLines; ++i)
  {
    SortRightToLeft(resultLines[i]);
  }

  for (int line=0; line<numberOfLines; ++line)
  {
    for(int i=0 ; i<resultLines[line]->GetPoints()->size() ; i++)
    {
      LabelingResults result;
      result.x = m_DotsVector[resultLines[line]->GetPoint(i)].GetX();
      dotCoords.push_back(result.x);
      result.y = m_DotsVector[resultLines[line]->GetPoint(i)].GetY();
      dotCoords.push_back(result.y);
      result.patternId = 0;
      result.wireId = i;
      m_Results.push_back(result);
      foundDotsCoordinateValues.push_back(dotCoords);
      dotCoords.clear();
    }

    intensity += resultLines[line]->GetIntensity();

    m_FoundLines.push_back(*(resultLines[line]));
  }

  m_FoundDotsCoordinateValue = foundDotsCoordinateValues;
  m_PatternIntensity = intensity;
  m_DotsFound = true;
}

//-----------------------------------------------------------------------------

void FidLabeling::UpdateCirsResults(Line resultLine1, Line resultLine2, Line resultLine3)
{
  //resultLine1 is the left line, resultLine2 is the diagonal, resultLine3 is the right line
  float intensity = 0;
  std::vector<double> dotCoords;
  std::vector< std::vector<double> > foundDotsCoordinateValues = m_FoundDotsCoordinateValue;

  for(int i=0 ; i<resultLine1.GetPoints()->size() ; i++)
  {
    LabelingResults result;
    result.x = m_DotsVector[resultLine1.GetPoint(i)].GetX();
    dotCoords.push_back(result.x);
    result.y = m_DotsVector[resultLine1.GetPoint(i)].GetY();
    dotCoords.push_back(result.y);
    result.patternId = 0;
    result.wireId = i;
    m_Results.push_back(result);
    foundDotsCoordinateValues.push_back(dotCoords);
    dotCoords.clear();
  }
  intensity += resultLine1.GetIntensity();

  for(int i=0 ; i<resultLine2.GetPoints()->size() ; i++)
  {
    LabelingResults result;
    result.x = m_DotsVector[resultLine2.GetPoint(i)].GetX();
    dotCoords.push_back(result.x);
    result.y = m_DotsVector[resultLine2.GetPoint(i)].GetY();
    dotCoords.push_back(result.y);
    result.patternId = 1;
    result.wireId = i;
    m_Results.push_back(result);
    foundDotsCoordinateValues.push_back(dotCoords);
    dotCoords.clear();
  }
  intensity += resultLine2.GetIntensity();

  for(int i=0 ; i<resultLine3.GetPoints()->size() ; i++)
  {
    LabelingResults result;
    result.x = m_DotsVector[resultLine3.GetPoint(i)].GetX();
    dotCoords.push_back(result.x);
    result.y = m_DotsVector[resultLine3.GetPoint(i)].GetY();
    dotCoords.push_back(result.y);
    result.patternId = 2;
    result.wireId = i;
    m_Results.push_back(result);
    foundDotsCoordinateValues.push_back(dotCoords);
    dotCoords.clear();
  }
  intensity += resultLine3.GetIntensity();

  m_FoundLines.push_back(resultLine1);
  m_FoundLines.push_back(resultLine2);
  m_FoundLines.push_back(resultLine3);

  m_FoundDotsCoordinateValue = foundDotsCoordinateValues;
  m_PatternIntensity = intensity;
  m_DotsFound = true;
}

//-----------------------------------------------------------------------------

void FidLabeling::FindPattern()
{
  //LOG_TRACE("FidLabeling::FindPattern");

  std::vector<Line> maxPointsLines = m_LinesVector[m_LinesVector.size()-1];
  int numberOfLines = m_Patterns.size();//the number of lines in the pattern
  int numberOfCandidateLines = maxPointsLines.size();
  std::vector<int> lineIndices(numberOfLines);
  std::vector<LabelingResults> results;


  m_DotsFound = false;

  //permutation generator
  for(int i=0 ; i<lineIndices.size() ; i++)
  {
    lineIndices[i] = lineIndices.size()-1-i;
  }
  lineIndices[0]--;

  bool foundPattern = false;
  do
  {
    for (int i=0; i<numberOfLines; i++)
    {
      lineIndices[i]++;

      if (lineIndices[i]<numberOfCandidateLines-i)
      {
        break; //valid permutation
      }

      if (i+1<numberOfLines)
      {
        if (lineIndices[i+1]<numberOfCandidateLines-i-1)
        {
          lineIndices[i]=lineIndices[i+1]+2;
        }
        else if (i+2<numberOfLines)
        {
          lineIndices[i]=lineIndices[i+2]+3;
        }
        else
        {
          return;
        }
      }
      else
      {
        return; //no permutation was valid
      }

      if (lineIndices[i]==numberOfCandidateLines)
      {
        return; //no permutation was valid
      }
    }

    // We have a new permutation in lineIndices.
    // Check if the distance and angle between each possible line pairs within the permutation are within the allowed range.
    // This is a quick filtering to keep only those line combinations for further processing that may form a valid pattern.
    foundPattern=true; // assume that we've found a valid pattern (then set the flag to false if it turns out that one of the values are not within the allowed range)
    for( int i=0 ; i<numberOfLines-1 && foundPattern; i++)
    {
      Line currentLine1 = maxPointsLines[lineIndices[i]];
      for( int j=i+1 ; j<numberOfLines && foundPattern; j++)
      {
        Line currentLine2 = maxPointsLines[lineIndices[j]];

        float angleBetweenLinesRad = Line::ComputeAngleRad(currentLine1, currentLine2);
        if (angleBetweenLinesRad<m_AngleToleranceRad) //The angle between 2 lines is close to 0
        {
          // Parallel lines

          // Check the distance between the lines
          float distance = ComputeDistancePointLine(m_DotsVector[currentLine1.GetStartPointIndex()], currentLine2);
          int maxLinePairDistPx = floor(m_MaxLinePairDistMm / m_ApproximateSpacingMmPerPixel + 0.5 );
          int minLinePairDistPx = floor(m_MinLinePairDistMm / m_ApproximateSpacingMmPerPixel + 0.5 );        
          if((distance > maxLinePairDistPx) || (distance < minLinePairDistPx))
          {
            // The distance between the lines is smaller or larger than the allowed range
            foundPattern=false;
            break;
          }

          // Check the shift (along the direction of the lines)
          float shift = ComputeShift(currentLine1,currentLine2);
          int maxLineShiftDistPx = floor(m_MaxLineShiftMm / m_ApproximateSpacingMmPerPixel + 0.5 );
          //maxLineShiftDistPx = 35;
          if(fabs(shift) > maxLineShiftDistPx)
          {
            // The shift between the is larger than the allowed value
            foundPattern=false;
            break;
          }
        }
        else 
        {
          // Non-parallel lines

          if ( (angleBetweenLinesRad>m_MaxLinePairAngleRad+m_AngleToleranceRad) || (angleBetweenLinesRad<m_MinLinePairAngleRad-m_AngleToleranceRad) )
          {
            // The angle between the patterns are not in the valid range
            foundPattern=false;
            break;
          }

          // If there are common endpoints between the lines then we check if the angle between the lines is correct
          // (Needed e.g., for the CIRS phantom model 45)
          int commonPointIndex = -1; // <0 if there are no common points between the lines, >=0 if there is a common endpoint
          if((currentLine1.GetStartPointIndex() == currentLine2.GetStartPointIndex()) || (currentLine1.GetStartPointIndex() == currentLine2.GetEndPointIndex()))
          {
            commonPointIndex = currentLine1.GetStartPointIndex();
          }
          else if((currentLine1.GetEndPointIndex() == currentLine2.GetStartPointIndex()) || (currentLine1.GetEndPointIndex() == currentLine2.GetEndPointIndex()))
          {
            commonPointIndex = currentLine1.GetEndPointIndex();
          }                    
          if(commonPointIndex != -1)          
          {
            // there is a common point
            if ( (angleBetweenLinesRad>m_InclinedLineAngleRad+m_AngleToleranceRad) || (angleBetweenLinesRad<m_InclinedLineAngleRad-m_AngleToleranceRad) )
            {
              // The angle between the patterns are not in the valid range
              foundPattern=false;
              break;
            }
          }
        }
      }
    }

  } while ((lineIndices[numberOfLines-1]!=numberOfCandidateLines-numberOfLines+2) && (!foundPattern));

  if (foundPattern)//We have the right permutation of lines in lineIndices
  {
    //Update the results, this part is not generic but depends on the Pattern we are looking for
    NWire* nWire = dynamic_cast<NWire*>(m_Patterns.at(0));
    CoplanarParallelWires* coplanarParallelWire = dynamic_cast<CoplanarParallelWires*>(m_Patterns.at(0));
    if (nWire) // NWires
    {
      std::vector<Line*> resultLines(numberOfLines);
      std::vector<double> resultLineMiddlePointYs;

      for (std::vector<int>::iterator it = lineIndices.begin(); it != lineIndices.end(); ++it)
      {
        resultLineMiddlePointYs.push_back( (m_DotsVector[maxPointsLines[*it].GetStartPointIndex()].GetY()+m_DotsVector[maxPointsLines[*it].GetEndPointIndex()].GetY())/2 );
      }

      // Sort result lines according to middlePoint Y's
      // TODO: If the wire pattern is asymmetric then use the pattern geometry to match the lines to the intersection points instead of just sort them by Y value (https://www.assembla.com/spaces/plus/tickets/435)
      std::vector<double>::iterator middlePointYsBeginIt = resultLineMiddlePointYs.begin();
      for (int i=0; i<numberOfLines; ++i)
      {
        std::vector<double>::iterator middlePointYsMinIt = std::min_element(middlePointYsBeginIt, resultLineMiddlePointYs.end());
        int minIndex = (int)std::distance(middlePointYsBeginIt,middlePointYsMinIt);
        resultLines[i] = &maxPointsLines[lineIndices[minIndex]];
        (*middlePointYsMinIt) = DBL_MAX;
      }

      UpdateNWiresResults(resultLines);
    }
    else if (coplanarParallelWire) // CIRS phantom
    {
      Line resultLine1 = maxPointsLines[lineIndices[0]];
      Line resultLine2 = maxPointsLines[lineIndices[1]];
      Line resultLine3 = maxPointsLines[lineIndices[2]];

      bool test1 = (resultLine1.GetStartPointIndex() == resultLine2.GetStartPointIndex()) || (resultLine1.GetStartPointIndex() == resultLine2.GetEndPointIndex());
      bool test2 = (resultLine1.GetEndPointIndex() == resultLine2.GetStartPointIndex()) || (resultLine1.GetEndPointIndex() == resultLine2.GetEndPointIndex());
      bool test3 = (resultLine1.GetStartPointIndex() == resultLine3.GetStartPointIndex()) || (resultLine1.GetStartPointIndex() == resultLine3.GetEndPointIndex());
      bool test4 = (resultLine1.GetEndPointIndex() == resultLine3.GetStartPointIndex()) || (resultLine1.GetEndPointIndex() == resultLine3.GetEndPointIndex());

      if(!test1 && !test2)//if line 1 and 2 have no point in common
      {
        if(m_DotsVector[resultLine1.GetStartPointIndex()].GetX() > m_DotsVector[resultLine2.GetStartPointIndex()].GetX())//Line 1 is on the left on the image
        {
          UpdateCirsResults(resultLine1, resultLine3, resultLine2);
        }
        else
        {
          UpdateCirsResults(resultLine2, resultLine3, resultLine1);
        }
      }
      else if(!test1 && !test3)//if line 1 and 3 have no point in common
      {
        if(m_DotsVector[resultLine1.GetStartPointIndex()].GetX() > m_DotsVector[resultLine3.GetStartPointIndex()].GetX())//Line 1 is on the left on the image
        {
          UpdateCirsResults(resultLine1, resultLine2, resultLine3);
        }
        else
        {
          UpdateCirsResults(resultLine3, resultLine2, resultLine1);
        }
      }
      else//if line 2 and 3 have no point in common
      {
        if(m_DotsVector[resultLine2.GetStartPointIndex()].GetX() > m_DotsVector[resultLine3.GetStartPointIndex()].GetX())//Line 2 is on the left on the image
        {
          UpdateCirsResults(resultLine2, resultLine1, resultLine3);
        }
        else
        {
          UpdateCirsResults(resultLine3, resultLine1, resultLine2);
        }
      }
    }   
  }
}

//-----------------------------------------------------------------------------
void FidLabeling::SortRightToLeft( Line *line )
{
  //LOG_TRACE("FidLabeling::SortRightToLeft");

  std::vector<std::vector<Dot>::iterator> pointsIterator(line->GetPoints()->size());

  for (int i=0; i<line->GetPoints()->size() ; i++)
  {
    pointsIterator[i] = m_DotsVector.begin() + line->GetPoint(i);
  }

  std::sort(pointsIterator.begin(), pointsIterator.end(), Dot::PositionLessThan);

  for (int i=0; i<line->GetPoints()->size(); i++)
  {
    line->SetPoint(i,pointsIterator[i] - m_DotsVector.begin());
  }
}

//-----------------------------------------------------------------------------
void FidLabeling::SetMinThetaDegrees(double value) 
{ 
  m_MinThetaRad = vtkMath::RadiansFromDegrees(value); 
}

void FidLabeling::SetMaxThetaDegrees(double value) 
{ 
  m_MaxThetaRad = vtkMath::RadiansFromDegrees(value); 
}
