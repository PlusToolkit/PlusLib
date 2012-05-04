/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "FidLineFinder.h"
#include "vtkMath.h"

#include <algorithm>

#include "PlusMath.h"

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_cross.h"
#include "vnl/algo/vnl_qr.h"
#include "vnl/algo/vnl_svd.h"

static const double DOT_STEPS  = 4.0;
static const double DOT_RADIUS = 6.0;

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

  m_MaxLinePairDistanceErrorPercent = -1.0;

  m_CollinearPointsMaxDistanceFromLineMm = -1.0; 

  m_MinThetaRad = -1.0; 
  m_MaxThetaRad = -1.0;
}

//-----------------------------------------------------------------------------

FidLineFinder::~FidLineFinder()
{

}

//-----------------------------------------------------------------------------

void FidLineFinder::ComputeParameters()
{
  LOG_TRACE("FidLineFinder::ComputeParameters");

  //Computation of MinTheta and MaxTheta from the physical probe rotation range
  std::vector<NWire> nWires;

  for( int i=0 ; i< m_Patterns.size() ; i++)
  {
    if(typeid(m_Patterns.at(i)) == typeid(NWire))//if it is a NWire
    {
      NWire* tempNWire = (NWire*)(&(m_Patterns.at(i)));
      nWires.push_back(*tempNWire);
    }
  }

  double *imageNormalVectorInPhantomFrameMaximumRotationAngleDeg=GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg();
  std::vector<double> thetaXrad, thetaYrad, thetaZrad;
  thetaXrad.push_back(vtkMath::RadiansFromDegrees(imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[0]));
  thetaXrad.push_back(0);
  thetaXrad.push_back(vtkMath::RadiansFromDegrees(imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[1]));
  thetaYrad.push_back(vtkMath::RadiansFromDegrees(imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[2]));
  thetaYrad.push_back(0);
  thetaYrad.push_back(vtkMath::RadiansFromDegrees(imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[3]));
  thetaZrad.push_back(vtkMath::RadiansFromDegrees(imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[4]));
  thetaZrad.push_back(0);
  thetaZrad.push_back(vtkMath::RadiansFromDegrees(imageNormalVectorInPhantomFrameMaximumRotationAngleDeg[5]));

  vnl_matrix<double> imageToPhantomTransform(4,4);

  for( int i = 0 ; i<4 ;i++)
  {
    for( int j = 0 ; j<4 ;j++)
    {
      imageToPhantomTransform.put(i,j,GetImageToPhantomTransform()[j+4*i]);
    }
  }

  vnl_vector<double> pointA(3), pointB(3), pointC(3);

  //3 points from the Nwires
  for( int i = 0; i<3 ;i++)
  {
    pointA.put(i,nWires[0].Wires[0].EndPointFront[i]);
    pointB.put(i,nWires[0].Wires[0].EndPointBack[i]);
    pointC.put(i,nWires[0].Wires[1].EndPointFront[i]);
  }

  //create 2 vectors out of them
  vnl_vector<double> AB(3);
  AB = pointB - pointA;
  vnl_vector<double> AC(3);
  AC = pointC - pointA;

  vnl_vector<double> normalVectorInPhantomCoord(3);
  normalVectorInPhantomCoord = vnl_cross_3d(AB,AC);//Get the vector normal to the wire plane

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

  for(int i = 0 ; i<3 ; i++)
  {    
    for(int j = 0 ; j<3 ; j++)
    {
      for(int k = 0 ; k<3 ; k++)
      {
        vnl_matrix<double> totalRotation(4,4,0);//The overall rotation matrix (after applying rotation around X, Y and Z axis
        double tX=thetaYrad[i];
        double tY=thetaYrad[j];
        double tZ=thetaYrad[k];
        totalRotation.put(0,0,cos(tY)*cos(tZ));
        totalRotation.put(0,1,-cos(tX)*sin(tZ)+sin(tX)*sin(tY)*cos(tZ));
        totalRotation.put(0,2,sin(tX)*sin(tZ)+cos(tX)*sin(tY)*cos(tZ));
        totalRotation.put(1,0,cos(tY)*sin(tZ));
        totalRotation.put(1,1,cos(tX)*cos(tZ)+sin(tX)*sin(tY)*sin(tZ));
        totalRotation.put(1,2,-sin(tX)*cos(tZ)+cos(tX)*sin(tY)*sin(tZ));
        totalRotation.put(2,0,-sin(tY));
        totalRotation.put(2,1,sin(tX)*cos(tY));
        totalRotation.put(2,2,cos(tX)*cos(tY));
        totalRotation.put(3,3,1);

        vnl_matrix<double> totalTranform(4,4);//The overall tranform matrix: Rotation*ImageToPhantom
        totalTranform = totalRotation*imageToPhantomTransform;

        vnl_vector<double> normalVectorInImageCoordExtended(4);//extended because it is a 4 dimension vector (result from a 4x4 matrix)

        //Get the normal vector in image coordinates by applying the total transform to the normal vector in phantom coordinates
        normalVectorInImageCoordExtended = totalTranform*normalVectorInPhantomCoordExtended;

        vnl_vector<double> normalVectorInImageCoord(3);//Make it 3 dimensions, the 4th is a useless value only needed for 4x4 matrix operations

        for( int i = 0 ;i<normalVectorInImageCoord.size() ;i++)
        {
          normalVectorInImageCoord.put(i,normalVectorInImageCoordExtended.get(i));
        }

        vnl_vector<double> lineDirectionVector(3);
        lineDirectionVector = vnl_cross_3d(normalVectorInImageCoord,normalImagePlane);

        double dotProductValue = dot_product(lineDirectionVector,imageYunitVector);
        double normOfLineDirectionvector = lineDirectionVector.two_norm();
        double angle = acos(dotProductValue/normOfLineDirectionvector);//get the angle between the line direction vector and the (0,1,0) vector
        finalAngleTable.push_back(angle);
      }
    }
  }

  //Get the maximum and the minimum angle from the table
  m_MaxThetaRad = (*std::max_element(finalAngleTable.begin(),finalAngleTable.end()));
  m_MinThetaRad = (*std::min_element(finalAngleTable.begin(),finalAngleTable.end()));
}

//-----------------------------------------------------------------------------

PlusStatus FidLineFinder::ReadConfiguration( vtkXMLDataElement* configData )
{
  LOG_TRACE("FidLineFinder::ReadConfiguration");

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
    vtkXMLDataElement* phantomDefinition = segmentationParameters->GetRoot()->FindNestedElementWithName("PhantomDefinition");
    if (phantomDefinition == NULL)
    {
      LOG_ERROR("No phantom definition is found in the XML tree!");
    }
    vtkXMLDataElement* customTransforms = phantomDefinition->FindNestedElementWithName("CustomTransforms"); 
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

    double collinearPointsMaxDistanceFromLineMm(0.0); 
    if ( segmentationParameters->GetScalarAttribute("CollinearPointsMaxDistanceFromLineMm", collinearPointsMaxDistanceFromLineMm) )
    {
      m_CollinearPointsMaxDistanceFromLineMm = collinearPointsMaxDistanceFromLineMm; 
    }
    else
    {
      LOG_WARNING("Could not read CollinearPointsMaxDistanceFromLineMm from configuration file.");
    }

    ComputeParameters();
  }
  else
  {
    double minThetaDegrees(0.0); 
    if ( segmentationParameters->GetScalarAttribute("MinThetaDegrees", minThetaDegrees) )
    {
      m_MinThetaRad = vtkMath::RadiansFromDegrees(minThetaDegrees);
    }
    else
    {
      LOG_WARNING("Could not read minThetaDegrees from configuration file.");
    }

    double maxThetaDegrees(0.0); 
    if ( segmentationParameters->GetScalarAttribute("MaxThetaDegrees", maxThetaDegrees) )
    {
      m_MaxThetaRad = vtkMath::RadiansFromDegrees(maxThetaDegrees); 
    }
    else
    {
      LOG_WARNING("Could not read maxThetaDegrees from configuration file.");
    }

    double collinearPointsMaxDistanceFromLineMm(0.0); 
    if ( segmentationParameters->GetScalarAttribute("CollinearPointsMaxDistanceFromLineMm", collinearPointsMaxDistanceFromLineMm) )
    {
      m_CollinearPointsMaxDistanceFromLineMm = collinearPointsMaxDistanceFromLineMm; 
    }
    else
    {
      LOG_WARNING("Could not read CollinearPointsMaxDistanceFromLineMm from configuration file.");
    }
  }

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------

std::vector<NWire> FidLineFinder::GetNWires()
{
  std::vector<NWire> nWires;

  for( int i=0 ; i< m_Patterns.size() ; i++)
  {
    NWire * tempNWire = (NWire*)((m_Patterns.at(i)));
    if(tempNWire)
    {
      nWires.push_back(*tempNWire);
    }
  }

  return nWires;
}

//-----------------------------------------------------------------------------

void FidLineFinder::SetFrameSize(int frameSize[2])
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

double FidLineFinder::ComputeAngleRad( Dot *dot1, Dot *dot2 )
{
  //LOG_TRACE("FidLineFinder::ComputeSlope");

  double x1 = dot1->GetX();
  double y1 = dot1->GetY();

  double x2 = dot2->GetX();
  double y2 = dot2->GetY();

  double y = (y2 - y1);
  double x = (x2 - x1);

  double angleRad = atan2(y,x);

  return angleRad;
}

//-----------------------------------------------------------------------------

void FidLineFinder::ComputeLine( Line &line )
{
  if (line.GetPoints()->size()<1)
  {
    LOG_WARNING("Cannot compute parameters for an empty line");
    return;
  }

  // Compute line intensity
  std::vector<int> pointNum;
  double lineIntensity = 0;
  for (int i=0; i<line.GetPoints()->size(); i++)
  {
    pointNum.push_back(line.GetPoint(i));
    lineIntensity += m_DotsVector[pointNum[i]].GetDotIntensity();
  }
  line.SetIntensity(lineIntensity);

  //Computing the line start point, end point, and length
  if (line.GetPoints()->size()==1)
  {
    line.SetStartPointIndex(line.GetPoint(0));
    line.SetEndPointIndex(line.GetPoint(0));
    line.SetLength(0);
  }
  else
  {
    line.SetStartPointIndex(line.GetPoint(0));
    // Choose the start point to be the one with the largest X position (closest to the marked side of the probe)
    int largestXposition = m_DotsVector[line.GetPoint(0)].GetX();
    for(int i=0; i<line.GetPoints()->size(); i++)
    {
      double currentXposition=m_DotsVector[line.GetPoint(i)].GetX();
      if (currentXposition>largestXposition)
      {                    
        largestXposition = currentXposition;
        line.SetStartPointIndex(line.GetPoint(i));
      }
    }
    // Search for the end of the line (the point that is farthest from the start point
    int maxDistance = 0;
    for(int i=0; i<line.GetPoints()->size(); i++)
    {
      double currentDistance = m_DotsVector[line.GetStartPointIndex()].GetDistanceFrom(m_DotsVector[line.GetPoint(i)]);
      if(currentDistance > maxDistance)
      {
        maxDistance = currentDistance;
        line.SetEndPointIndex(line.GetPoint(i));
      }
    }
    // Make sure that the startpoint is the farthest point from the endpoint
    maxDistance = 0;    
    for(int i=0; i<line.GetPoints()->size(); i++)
    {
      double currentDistance = m_DotsVector[line.GetEndPointIndex()].GetDistanceFrom(m_DotsVector[line.GetPoint(i)]);
      if(currentDistance > maxDistance)
      {
        maxDistance = currentDistance;
        line.SetStartPointIndex(line.GetPoint(i));
      }
    }
    line.SetLength(maxDistance);//This actually computes the length (not return) and set the endpoint as well
  }

  // Compute the line normal
  if(line.GetPoints()->size() > 2)//separating cases: 2-points lines and n-points lines, way simpler for 2-points lines
  {
    //computing the line equation c + n1*x + n2*y = 0
    std::vector<double> x;
    std::vector<double> y;

    double n1, n2, c;

    vnl_matrix<double> A(line.GetPoints()->size(),3,1);

    for (int i=0; i<line.GetPoints()->size(); i++)
    {
      x.push_back(m_DotsVector[pointNum[i]].GetX());
      y.push_back(m_DotsVector[pointNum[i]].GetY());
      A.put(i,1,x[i]);
      A.put(i,2,y[i]);
    }

    //using QR matrix decomposition
    vnl_qr<double> QR(A);
    vnl_matrix<double> Q = QR.Q();
    vnl_matrix<double> R = QR.R();

    //the B matrix is a subset of the R matrix (because we are in 2D)
    vnl_matrix<double> B(2,2,0);
    B.put(0,0,R(1,1));
    B.put(0,1,R(1,2));
    B.put(1,1,R(2,2));

    //single Value decomposition of B
    vnl_svd<double> SVD(B);
    vnl_matrix<double> V = SVD.V();

    //We get the needed coefficients from V
    n1 = V(0,1);
    n2 = V(1,1);
    c = -(n1*R(0,1)+n2*R(0,2))/R(0,0);

    if(-n2 < 0 && n1 < 0)
    {
      line.SetDirectionVector(0,n2);//the vector (n1,n2) is orthogonal to the line, therefore (-n2,n1) is a direction vector
      line.SetDirectionVector(1,-n1);
    }
    else
    {
      line.SetDirectionVector(0,-n2);//the vector (n1,n2) is orthogonal to the line, therefore (-n2,n1) is a direction vector
      line.SetDirectionVector(1,n1);
    }
  }
  else//for 2-points lines, way simpler and faster
  {
    double xdif = m_DotsVector[line.GetEndPointIndex()].GetX()-m_DotsVector[line.GetStartPointIndex()].GetX();
    double ydif = m_DotsVector[line.GetEndPointIndex()].GetY()-m_DotsVector[line.GetStartPointIndex()].GetY();

    line.SetDirectionVector(0, xdif);
    line.SetDirectionVector(1, ydif);
  }
}

//-----------------------------------------------------------------------------

double FidLineFinder::SegmentLength( Dot *d1, Dot *d2 )
{
  //LOG_TRACE("FidLineFinder::SegmentLength");

  double xd = d2->GetX() - d1->GetX();
  double yd = d2->GetY() - d1->GetY();
  return sqrtf( xd*xd + yd*yd );
}

//-----------------------------------------------------------------------------

double FidLineFinder::ComputeDistancePointLine(Dot dot, Line line)
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

void FidLineFinder::FindLines2Points()
{
  LOG_TRACE("FidLineFinder::FindLines2Points");

  if (m_DotsVector.size() < 2)
  {
    return;
  }

  std::vector<Line> twoPointsLinesVector;

  for( int i=0 ; i<m_Patterns.size() ; i++)
  {
    //the expected length of the line
    int lineLenPx = floor(m_Patterns[i]->DistanceToOriginMm[m_Patterns[i]->Wires.size()-1] / m_ApproximateSpacingMmPerPixel + 0.5 );

    for ( int dot1Index = 0; dot1Index < m_DotsVector.size()-1; dot1Index++ ) 
    {
      for ( int dot2Index = dot1Index+1; dot2Index < m_DotsVector.size(); dot2Index++ ) 
      {
        double length = SegmentLength(&m_DotsVector[dot1Index],&m_DotsVector[dot2Index]);
        bool acceptLength = fabs(length-lineLenPx) < floor(m_Patterns[i]->DistanceToOriginToleranceMm[m_Patterns[i]->Wires.size()-1] / m_ApproximateSpacingMmPerPixel + 0.5 );

        if(acceptLength)//to only add valid two point lines
        {
          double angleRad = ComputeAngleRad(&m_DotsVector[dot1Index], &m_DotsVector[dot2Index]);
          bool acceptAngle = AcceptAngleRad(angleRad);

          if(acceptAngle)
          {
            Line twoPointsLine;
            twoPointsLine.GetPoints()->resize(2);
            twoPointsLine.SetPoint(0, dot1Index);
            twoPointsLine.SetPoint(1, dot2Index);

            bool duplicate = std::binary_search(twoPointsLinesVector.begin(), twoPointsLinesVector.end(),twoPointsLine, Line::compareLines);

            if(!duplicate)
            {
              twoPointsLine.SetStartPointIndex(dot1Index);
              ComputeLine(twoPointsLine);

              twoPointsLinesVector.push_back(twoPointsLine);
              std::sort(twoPointsLinesVector.begin(), twoPointsLinesVector.end(), Line::compareLines);//the lines need to be sorted that way each time for the binary search to be performed
            }
          }
        }
      }
    }
  }
  std::sort(twoPointsLinesVector.begin(), twoPointsLinesVector.end(), Line::lessThan);//sort the lines by intensity finally 

  m_LinesVector.push_back(twoPointsLinesVector);
}

//-----------------------------------------------------------------------------

void FidLineFinder::FindLinesNPoints()
{
  /* For each point, loop over each 2-point line and try to make a 3-point
  * line. For the third point use the theta of the line and compute a value
  * for p. Accept the line if the compute p is within some small distance
  * of the 2-point line. */

  LOG_TRACE("FidLineFinder::FindLines3Points");

  double dist = m_CollinearPointsMaxDistanceFromLineMm / m_ApproximateSpacingMmPerPixel;
  int maxNumberOfPointsPerLine = -1;

  for( int i=0 ; i<m_Patterns.size() ; i++ )
  {
    if(int(m_Patterns[i]->Wires.size()) > maxNumberOfPointsPerLine)
    {
      maxNumberOfPointsPerLine = m_Patterns[i]->Wires.size();
    }
  }

  for( int i=0 ; i<m_Patterns.size() ; i++)
  {
    for( int linesVectorIndex = 3 ; linesVectorIndex <= maxNumberOfPointsPerLine ; linesVectorIndex++ )
    {
      if (linesVectorIndex > m_LinesVector.size())
      {
        continue;
      }

      for ( int l = 0; l < m_LinesVector[linesVectorIndex-1].size(); l++ ) 
      {
        Line currentShorterPointsLine;
        currentShorterPointsLine = m_LinesVector[linesVectorIndex-1][l];//the current max point line we want to expand

        for ( int b3 = 0; b3 < m_DotsVector.size(); b3++ ) 
        {
          std::vector<int> candidatesIndex;
          bool checkDuplicateFlag = false;//assume there is no duplicate

          for( int previousPoints=0 ; previousPoints < currentShorterPointsLine.GetPoints()->size() ; previousPoints++ )
          {
            candidatesIndex.push_back(currentShorterPointsLine.GetPoint(previousPoints));
            if(candidatesIndex[previousPoints] == b3)
            {
              checkDuplicateFlag = true;//the point we want to add is already a point of the line
            }
          }

          if(checkDuplicateFlag)
          {
            continue;
          }

          candidatesIndex.push_back(b3);                 
          double pointToLineDistance = ComputeDistancePointLine(m_DotsVector[b3], currentShorterPointsLine);

          if ( pointToLineDistance <= dist ) 
          {
            Line line;

            // To find unique lines, each line must have a unique configuration of points.
            std::sort(candidatesIndex.begin(),candidatesIndex.end());

            for (int f=0; f<candidatesIndex.size(); f++)
            {
              line.GetPoints()->push_back(candidatesIndex[f]);
            }			
            line.SetStartPointIndex(currentShorterPointsLine.GetStartPointIndex());


            double length = SegmentLength(&m_DotsVector[currentShorterPointsLine.GetStartPointIndex()],&m_DotsVector[b3]); //distance between the origin and the point we try to add

            int lineLenPx = floor(m_Patterns[i]->DistanceToOriginMm[linesVectorIndex-2] / m_ApproximateSpacingMmPerPixel + 0.5 );
            bool acceptLength = fabs(length-lineLenPx) < floor(m_Patterns[i]->DistanceToOriginToleranceMm[linesVectorIndex-2] / m_ApproximateSpacingMmPerPixel + 0.5 );

            if(!acceptLength)
            {
              continue;
            }

            // Create the vector between the origin point to the end point and between the origin point to the new point to check if the new point is between the origin and the end point
            double originToEndPointVector[3] = { m_DotsVector[currentShorterPointsLine.GetEndPointIndex()].GetX()-m_DotsVector[currentShorterPointsLine.GetStartPointIndex()].GetX() ,
              m_DotsVector[currentShorterPointsLine.GetEndPointIndex()].GetY()-m_DotsVector[currentShorterPointsLine.GetStartPointIndex()].GetY() ,
              0 };

            double originToNewPointVector[3] = {m_DotsVector[b3].GetX()-m_DotsVector[currentShorterPointsLine.GetStartPointIndex()].GetX() ,
              m_DotsVector[b3].GetY()-m_DotsVector[currentShorterPointsLine.GetStartPointIndex()].GetY() ,
              0 };

            double dot = vtkMath::Dot(originToEndPointVector,originToNewPointVector);

            // Reject the line if the middle point is outside the original line
            if( dot < 0 )
            {
              continue;
            }                                                

            if(m_LinesVector.size() <= linesVectorIndex)//in case the maxpoint lines has not found any yet (the binary search works on empty vector, not on NULL one obviously)
            { 
              std::vector<Line> emptyLine;
              m_LinesVector.push_back(emptyLine);
            }

            if(!std::binary_search(m_LinesVector[linesVectorIndex].begin(), m_LinesVector[linesVectorIndex].end(),line, Line::compareLines)) 
            {
              ComputeLine(line);
              if(AcceptLine(line))
              {
                m_LinesVector[linesVectorIndex].push_back(line);
                // sort the lines so that lines that are already in the list can be quickly found by a binary search
                std::sort (m_LinesVector[linesVectorIndex].begin(), m_LinesVector[linesVectorIndex].end(), Line::compareLines);
              }
            }
          }
        }
      }
    }
  }
  if(m_LinesVector[m_LinesVector.size()-1].empty())
  {
    m_LinesVector.pop_back();
  }
}

//-----------------------------------------------------------------------------

void FidLineFinder::Clear()
{
  //LOG_TRACE("FidLineFinder::Clear");

  m_DotsVector.clear();
  m_LinesVector.clear();
  m_CandidateFidValues.clear();

  std::vector<Line> emptyLine;
  m_LinesVector.push_back(emptyLine);//initializing the 0 vector of lines (unused)
  m_LinesVector.push_back(emptyLine);//initializing the 1 vector of lines (unused)
}

//-----------------------------------------------------------------------------

bool FidLineFinder::AcceptAngleRad( double angleRad )
{
  //then angle must be in a certain range that is given in half space (in config file) so it needs to check the whole space (if 20 is accepted, so should -160 as it is the same orientation)

  if(angleRad > m_MinThetaRad && angleRad < m_MaxThetaRad)
  {
    return true;
  }
  if(m_MaxThetaRad < vtkMath::Pi()/2 && m_MinThetaRad > -vtkMath::Pi()/2)
  {
    if(angleRad < m_MaxThetaRad-vtkMath::Pi() || angleRad > m_MinThetaRad+vtkMath::Pi())
    {
      return true;
    }
  }
  else if(m_MaxThetaRad > vtkMath::Pi()/2)
  {
    if(angleRad < m_MaxThetaRad-vtkMath::Pi() && angleRad > m_MinThetaRad-vtkMath::Pi())
    {
      return true;
    }
  }
  else if(m_MinThetaRad < -vtkMath::Pi()/2)
  {
    if(angleRad < m_MaxThetaRad+vtkMath::Pi() && angleRad > m_MinThetaRad+vtkMath::Pi())
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------

bool FidLineFinder::AcceptLine( Line &line )
{
  double angleRad = Line::ComputeAngleRad(line);
  bool acceptAngle = AcceptAngleRad(angleRad);
  return acceptAngle;
}

//-----------------------------------------------------------------------------

void FidLineFinder::FindLines( )
{
  LOG_TRACE("FidLineFinder::FindLines");

  // Make pairs of dots into 2-point lines.
  FindLines2Points();

  // Make 2-point lines and dots into 3-point lines.
  FindLinesNPoints();

  // Sort by intensity.
  std::sort (m_LinesVector[m_LinesVector.size()-1].begin(), m_LinesVector[m_LinesVector.size()-1].end(), Line::lessThan);
}

//-----------------------------------------------------------------------------
void FidLineFinder::SetMinThetaDeg(double angleDeg)
{
  m_MinThetaRad=vtkMath::RadiansFromDegrees(angleDeg);
}

//-----------------------------------------------------------------------------
void FidLineFinder::SetMaxThetaDeg(double angleDeg)
{
  m_MaxThetaRad=vtkMath::RadiansFromDegrees(angleDeg);
}
