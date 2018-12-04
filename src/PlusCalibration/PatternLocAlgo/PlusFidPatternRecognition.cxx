/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusFidPatternRecognition.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkLine.h"

#include "vtkIGSIOTrackedFrameList.h"
#include "igsioTrackedFrame.h"

static const double DOT_STEPS  = 4.0;
static const double DOT_RADIUS = 6.0;

//-----------------------------------------------------------------------------

PlusFidPatternRecognition::PlusFidPatternRecognition()
{

}

//-----------------------------------------------------------------------------

PlusFidPatternRecognition::~PlusFidPatternRecognition()
{
}

//-----------------------------------------------------------------------------

PlusStatus PlusFidPatternRecognition::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("FidPatternRecognition::ReadConfiguration");

  if (ReadPhantomDefinition(rootConfigElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading phantom definition failed!");
    return PLUS_FAIL;
  }

  //Read the configuration in each of the component of the algorithm
  m_FidSegmentation.ReadConfiguration(rootConfigElement);
  m_FidLineFinder.ReadConfiguration(rootConfigElement);
  m_FidLabeling.ReadConfiguration(rootConfigElement, m_FidLineFinder.GetMinThetaRad(), m_FidLineFinder.GetMaxThetaRad());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus PlusFidPatternRecognition::RecognizePattern(igsioTrackedFrame* trackedFrame, PlusPatternRecognitionResult& patternRecognitionResult, PatternRecognitionError& patternRecognitionError, unsigned int frameIndex)
{
  LOG_TRACE("FidPatternRecognition::RecognizePattern");

  patternRecognitionError = PATTERN_RECOGNITION_ERROR_NO_ERROR;

  if (RecognizePattern(trackedFrame, patternRecognitionError, frameIndex) != PLUS_SUCCESS)
  {
    LOG_ERROR("Recognizing pattern failed!");
    return PLUS_FAIL;
  }

  //Set the results
  patternRecognitionResult.SetIntensity(m_FidLabeling.GetPatternIntensity());
  patternRecognitionResult.SetNumDots(m_FidLabeling.GetDotsVector().size());
  patternRecognitionResult.SetDotsFound(m_FidLabeling.GetDotsFound());
  patternRecognitionResult.SetFoundDotsCoordinateValue(m_FidLabeling.GetFoundDotsCoordinateValue());
  patternRecognitionResult.SetCandidateFidValues(m_FidSegmentation.GetCandidateFidValues());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus PlusFidPatternRecognition::RecognizePattern(igsioTrackedFrame* trackedFrame, PatternRecognitionError& patternRecognitionError, unsigned int frameIndex)
{
  LOG_TRACE("FidPatternRecognition::RecognizePattern");

  patternRecognitionError = PATTERN_RECOGNITION_ERROR_NO_ERROR;

  m_FidSegmentation.Clear();
  m_FidLineFinder.Clear();
  m_FidLabeling.Clear();

  m_FidSegmentation.SetFrameSize(trackedFrame->GetFrameSize());
  m_FidLineFinder.SetFrameSize(trackedFrame->GetFrameSize());
  m_FidLabeling.SetFrameSize(trackedFrame->GetFrameSize());

  if (trackedFrame->GetImageData()->GetVTKScalarPixelType() != VTK_UNSIGNED_CHAR)
  {
    LOG_ERROR("FidPatternRecognition::RecognizePattern only supports 8-bit images");
    patternRecognitionError = PATTERN_RECOGNITION_ERROR_UNKNOWN;
    return PLUS_FAIL;
  }

  int bytes = trackedFrame->GetFrameSize()[0] * trackedFrame->GetFrameSize()[1] * sizeof(PlusFidSegmentation::PixelType);
  PlusFidSegmentation::PixelType* image = reinterpret_cast<PlusFidSegmentation::PixelType*>(trackedFrame->GetImageData()->GetScalarPointer());

  memcpy(m_FidSegmentation.GetWorking(), image, bytes);
  memcpy(m_FidSegmentation.GetUnalteredImage(), image, bytes);

  //Start of the segmentation
  m_FidSegmentation.MorphologicalOperations();
  m_FidSegmentation.Suppress(m_FidSegmentation.GetWorking(), m_FidSegmentation.GetThresholdImagePercent() / 100.00);
  bool tooManyCandidates = false;
  bool clusteringSuccessful = m_FidSegmentation.Cluster(tooManyCandidates);
  if (tooManyCandidates)
  {
    patternRecognitionError = PATTERN_RECOGNITION_ERROR_TOO_MANY_CANDIDATES;
  }
  if (!clusteringSuccessful)
  {
    patternRecognitionError = PATTERN_RECOGNITION_ERROR_UNKNOWN;
  }

  //End of the segmentation

  m_FidSegmentation.SetCandidateFidValues(m_FidSegmentation.GetDotsVector());

  m_FidLineFinder.SetCandidateFidValues(m_FidSegmentation.GetCandidateFidValues());
  m_FidLineFinder.SetDotsVector(m_FidSegmentation.GetDotsVector());
  m_FidLabeling.SetDotsVector(m_FidSegmentation.GetDotsVector());

  m_FidLineFinder.FindLines();

  if (m_FidLineFinder.GetLinesVector().size() > 3)
  {
    m_FidLabeling.SetLinesVector(m_FidLineFinder.GetLinesVector());
    m_FidLabeling.FindPattern();
  }

  if (m_FidSegmentation.GetDebugOutput())
  {
    //Displays the result dots
    m_FidSegmentation.WritePossibleFiducialOverlayImage(m_FidLabeling.GetFoundDotsCoordinateValue(), m_FidSegmentation.GetUnalteredImage(), "foundFiducials", frameIndex);
    m_FidSegmentation.WritePossibleFiducialOverlayImage(m_FidSegmentation.GetCandidateFidValues(), m_FidSegmentation.GetUnalteredImage(), "candidateFiducials", frameIndex);   //Display all candidates dots
  }

  // Set results
  std::vector< std::vector<double> > fiducials = m_FidLabeling.GetFoundDotsCoordinateValue();

  vtkSmartPointer<vtkPoints> fiducialPoints = vtkSmartPointer<vtkPoints>::New();
  fiducialPoints->SetNumberOfPoints(fiducials.size());

  for (unsigned int i = 0; i < fiducials.size(); ++i)
  {
    fiducialPoints->InsertPoint(i, fiducials[i][0], fiducials[i][1], 0.0);
  }
  fiducialPoints->Modified();

  trackedFrame->SetFiducialPointsCoordinatePx(fiducialPoints);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus PlusFidPatternRecognition::RecognizePattern(vtkIGSIOTrackedFrameList* trackedFrameList, PatternRecognitionError& patternRecognitionError, int* numberOfSuccessfullySegmentedImages/*=NULL*/, std::vector<unsigned int>* segmentedFramesIndices)
{
  LOG_TRACE("FidPatternRecognition::RecognizePattern");

  patternRecognitionError = PATTERN_RECOGNITION_ERROR_NO_ERROR;

  // Check if TrackedFrameList is MF oriented BRIGHTNESS image
  if (vtkIGSIOTrackedFrameList::VerifyProperties(trackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to perform calibration - tracked frame list is invalid");
    patternRecognitionError = PATTERN_RECOGNITION_ERROR_UNKNOWN;
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;
  if (numberOfSuccessfullySegmentedImages)
  {
    *numberOfSuccessfullySegmentedImages = 0;
  }

  for (unsigned int currentFrameIndex = 0; currentFrameIndex < trackedFrameList->GetNumberOfTrackedFrames(); currentFrameIndex++)
  {
    igsioTrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(currentFrameIndex);

    // segment only non segmented frames
    if (trackedFrame->GetFiducialPointsCoordinatePx() != NULL)
    {
      continue;
    }

    if (RecognizePattern(trackedFrame, patternRecognitionError, currentFrameIndex) != PLUS_SUCCESS)
    {
      if (patternRecognitionError != PATTERN_RECOGNITION_ERROR_TOO_MANY_CANDIDATES)
      {
        LOG_ERROR("Recognizing pattern failed on frame " << currentFrameIndex);
        status = PLUS_FAIL;
      }
    }

    if (numberOfSuccessfullySegmentedImages)
    {
      // compute the number of successfully segmented images
      if (trackedFrame->GetFiducialPointsCoordinatePx()
          && trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() > 0)
      {
        (*numberOfSuccessfullySegmentedImages)++;
        if (segmentedFramesIndices != NULL)
        {
          segmentedFramesIndices->push_back(currentFrameIndex);
        }
      }
    }
  }

  return status;
}

//-----------------------------------------------------------------------------

void PlusFidPatternRecognition::DrawDots(PlusFidSegmentation::PixelType* image)
{
  LOG_TRACE("FidPatternRecognition::DrawDots");

  for (unsigned int d = 0; d < m_FidLabeling.GetFoundDotsCoordinateValue().size(); d++)
  {
    double row = m_FidLabeling.GetFoundDotsCoordinateValue()[d][1];
    double col = m_FidLabeling.GetFoundDotsCoordinateValue()[d][0];

    for (double t = 0; t < 2 * vtkMath::Pi(); t += vtkMath::Pi() / DOT_STEPS)
    {
      int r = (int)floor(row + cos(t) * DOT_RADIUS);
      int c = (int)floor(col + sin(t) * DOT_RADIUS);

      if (r >= 0 && static_cast<unsigned int>(r) < m_FidSegmentation.GetFrameSize()[1] && c >= 0 && static_cast<unsigned int>(c) < m_FidSegmentation.GetFrameSize()[0])
      {
        image[r * m_FidSegmentation.GetFrameSize()[0] + c] = UCHAR_MAX;
      }
    }

    image[static_cast<int>(floor(row)* m_FidSegmentation.GetFrameSize()[0] + floor(col))] = 0;
  }
}

//-----------------------------------------------------------------------------

void PlusFidPatternRecognition::DrawResults(PlusFidSegmentation::PixelType* image)
{
  std::vector<PlusFidLine> foundLines = m_FidLabeling.GetFoundLinesVector();
  DrawDots(image);

  LOG_TRACE("FidPatternRecognition::DrawLines");

  for (unsigned int l = 0; l < foundLines.size(); l++)
  {
    double origin[2] = { m_FidLabeling.GetDotsVector()[foundLines[l].GetStartPointIndex()].GetX() , m_FidLabeling.GetDotsVector()[foundLines[l].GetStartPointIndex()].GetY()};
    double directionVector[2] = { foundLines[l].GetDirectionVector()[0], foundLines[l].GetDirectionVector()[1]};
    if (directionVector[0] > 0)
    {
      directionVector[0] = -directionVector[0];
      directionVector[1] = -directionVector[1];
    }

    vtkMath::Normalize2D(directionVector);
    double r = origin[1] - 0.2 * foundLines[l].GetLength() * directionVector[1];
    double c = origin[0] - 0.2 * foundLines[l].GetLength() * directionVector[0];

    if (r >= 0 && r < m_FidSegmentation.GetFrameSize()[1] && c >= 0 && c <  m_FidSegmentation.GetFrameSize()[0])
    {
      image[int(r * m_FidSegmentation.GetFrameSize()[0] + c)] = UCHAR_MAX;
    }

    for (int i = 0 ; i < foundLines[l].GetLength() * 1.4 ; i++)
    {
      r += directionVector[1];
      c += directionVector[0];
      if (r >= 0 && r < m_FidSegmentation.GetFrameSize()[1] && c >= 0 && c <  m_FidSegmentation.GetFrameSize()[0])
      {
        image[int(int(r)*m_FidSegmentation.GetFrameSize()[0] + c)] = UCHAR_MAX;
      }
    }
  }
}

//----------------------------------------------------------------------------

void PlusFidPatternRecognition::SetMaxLineLengthToleranceMm(double value)
{
  m_MaxLineLengthToleranceMm = value;
  for (unsigned int i = 0 ; i < m_FidLabeling.GetPatterns().size() ; i++)
  {
    m_FidLabeling.GetPatterns()[i]->SetDistanceToOriginToleranceElementMm(m_FidLabeling.GetPatterns()[i]->GetWires().size() - 1, m_MaxLineLengthToleranceMm);
  }
  for (unsigned int i = 0 ; i < m_FidLineFinder.GetPatterns().size() ; i++)
  {
    m_FidLineFinder.GetPatterns()[i]->SetDistanceToOriginToleranceElementMm(m_FidLineFinder.GetPatterns()[i]->GetWires().size() - 1, m_MaxLineLengthToleranceMm);
  }
}

//----------------------------------------------------------------------------

PlusStatus PlusFidPatternRecognition::ReadPhantomDefinition(vtkXMLDataElement* config)
{
  LOG_TRACE("FidPatternRecognition::ReadPhantomDefinition");

  if (config == NULL)
  {
    LOG_ERROR("Configuration XML data element is NULL");
    return PLUS_FAIL;
  }

  bool nwireFlag = false;

  vtkXMLDataElement* phantomDefinition = config->FindNestedElementWithName("PhantomDefinition");
  if (phantomDefinition == NULL)
  {
    LOG_ERROR("No phantom definition is found in the XML tree!");
    return PLUS_FAIL;
  }
  else
  {
    std::vector<PlusFidPattern*> tempPatterns;

    // Load geometry
    vtkXMLDataElement* geometry = phantomDefinition->FindNestedElementWithName("Geometry");
    if (geometry == NULL)
    {
      LOG_ERROR("Phantom geometry information not found!");
      return PLUS_FAIL;
    }
    else
    {
      // Finding of Patterns and extracting the endpoints
      int numberOfGeometryChildren = geometry->GetNumberOfNestedElements();
      for (int i = 0; i < numberOfGeometryChildren; ++i)
      {
        vtkXMLDataElement* patternElement = geometry->GetNestedElement(i);

        if ((patternElement == NULL) || (STRCASECMP("Pattern", patternElement->GetName())))
        {
          continue;
        }

        PlusNWire* nWire = new PlusNWire();
        PlusCoplanarParallelWires* coplanarParallelWires = new PlusCoplanarParallelWires();

        int numberOfWires = patternElement->GetNumberOfNestedElements();

        if ((numberOfWires != 3) && !(STRCASECMP("NWire", patternElement->GetAttribute("Type"))))
        {
          LOG_WARNING("NWire contains unexpected number of wires - skipped");
          continue;
        }

        for (int j = 0; j < numberOfWires; ++j)
        {
          vtkXMLDataElement* wireElement = patternElement->GetNestedElement(j);

          if (wireElement == NULL)
          {
            LOG_WARNING("Invalid wire description in Pattern - skipped");
            break;
          }

          PlusFidWire wire;

          const char* wireName =  wireElement->GetAttribute("Name");
          if (wireName != NULL)
          {
            wire.SetName(wireName);
          }
          if (! wireElement->GetVectorAttribute("EndPointFront", 3, wire.EndPointFront))
          {
            LOG_WARNING("Wrong wire end point detected - skipped");
            continue;
          }
          if (! wireElement->GetVectorAttribute("EndPointBack", 3, wire.EndPointBack))
          {
            LOG_WARNING("Wrong wire end point detected - skipped");
            continue;
          }

          if (STRCASECMP("CoplanarParallelWires", patternElement->GetAttribute("Type")) == 0)
          {
            coplanarParallelWires->AddWire(wire);
          }
          else if (STRCASECMP("NWire", patternElement->GetAttribute("Type")) == 0)
          {
            nWire->AddWire(wire);
          }
        }

        if (STRCASECMP("CoplanarParallelWires", patternElement->GetAttribute("Type")) == 0)
        {
          tempPatterns.push_back(coplanarParallelWires);

          if (i == 1)
          {
            tempPatterns[i]->AddDistanceToOriginElementMm(0);
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(0);
            tempPatterns[i]->AddDistanceToOriginElementMm(10 * std::sqrt(2.0));
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
            tempPatterns[i]->AddDistanceToOriginElementMm(20 * std::sqrt(2.0));
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
            tempPatterns[i]->AddDistanceToOriginElementMm(30 * std::sqrt(2.0));
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
            tempPatterns[i]->AddDistanceToOriginElementMm(40 * std::sqrt(2.0));
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
          }
          else
          {
            tempPatterns[i]->AddDistanceToOriginElementMm(0);
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(0);
            tempPatterns[i]->AddDistanceToOriginElementMm(10);
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
            tempPatterns[i]->AddDistanceToOriginElementMm(20);
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
            tempPatterns[i]->AddDistanceToOriginElementMm(30);
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
            tempPatterns[i]->AddDistanceToOriginElementMm(40);
            tempPatterns[i]->AddDistanceToOriginToleranceElementMm(2);
          }
        }
        else if (STRCASECMP("NWire", patternElement->GetAttribute("Type")) == 0)
        {
          m_MaxLineLengthToleranceMm = 4;
          tempPatterns.push_back(nWire);

          tempPatterns[i]->AddDistanceToOriginElementMm(0);
          tempPatterns[i]->AddDistanceToOriginToleranceElementMm(0);

          double originToMiddle[3] = {(tempPatterns[i]->GetWires()[1].EndPointBack[0] + tempPatterns[i]->GetWires()[1].EndPointFront[0]) / 2 - tempPatterns[i]->GetWires()[0].EndPointFront[0],
                                      (tempPatterns[i]->GetWires()[1].EndPointBack[1] + tempPatterns[i]->GetWires()[1].EndPointFront[1]) / 2 - tempPatterns[i]->GetWires()[0].EndPointFront[1],
                                      (tempPatterns[i]->GetWires()[1].EndPointBack[2] + tempPatterns[i]->GetWires()[1].EndPointFront[2]) / 2 - tempPatterns[i]->GetWires()[0].EndPointFront[2]
                                     };

          double originToEnd[3] = {tempPatterns[i]->GetWires()[2].EndPointFront[0] - tempPatterns[i]->GetWires()[0].EndPointFront[0],
                                   tempPatterns[i]->GetWires()[2].EndPointFront[1] - tempPatterns[i]->GetWires()[0].EndPointFront[1],
                                   tempPatterns[i]->GetWires()[2].EndPointFront[2] - tempPatterns[i]->GetWires()[0].EndPointFront[2]
                                  };

          vtkMath::Normalize(originToEnd);

          double dot = vtkMath::Dot(originToMiddle, originToEnd);
          const double projectedMiddle[3] = {originToEnd[0]* dot, originToEnd[1]* dot, originToEnd[2]* dot};
          double distMidToOrigin = vtkMath::Norm(projectedMiddle);

          tempPatterns[i]->AddDistanceToOriginElementMm(distMidToOrigin);
          tempPatterns[i]->AddDistanceToOriginToleranceElementMm(15);

          double distEndToOrigin = sqrt((tempPatterns[i]->GetWires()[0].EndPointBack[0] - tempPatterns[i]->GetWires()[2].EndPointBack[0]) * (tempPatterns[i]->GetWires()[0].EndPointBack[0] - tempPatterns[i]->GetWires()[2].EndPointBack[0]) + (tempPatterns[i]->GetWires()[0].EndPointBack[1] - tempPatterns[i]->GetWires()[2].EndPointBack[1]) * (tempPatterns[i]->GetWires()[0].EndPointBack[1] - tempPatterns[i]->GetWires()[2].EndPointBack[1]));
          tempPatterns[i]->AddDistanceToOriginElementMm(distEndToOrigin);
          tempPatterns[i]->AddDistanceToOriginToleranceElementMm(m_MaxLineLengthToleranceMm);

          nwireFlag = true;
        }
      }
    }

    if (nwireFlag)
    {
      // Read input NWires and convert them to vnl vectors to easier processing
      LOG_DEBUG("Endpoints of wires = ");

      // List endpoints, check wire ids and NWire geometry correctness (wire order and locations) and compute intersections
      for (unsigned int k = 0 ; k < tempPatterns.size() ; ++k)
      {
        unsigned int layer = k;

        for (int i = 0; i < 3; ++i)
        {
          vnl_vector<double> endPointFront(3);
          vnl_vector<double> endPointBack(3);

          for (int j = 0; j < 3; ++j)
          {
            endPointFront[j] = tempPatterns[k]->GetWires()[i].EndPointFront[j];
            endPointBack[j] = tempPatterns[k]->GetWires()[i].EndPointBack[j];
          }

          LOG_DEBUG("\t Front endpoint of wire " << i << " on layer " << layer << " = " << endPointFront);
          LOG_DEBUG("\t Back endpoint of wire " << i << " on layer " << layer << " = " << endPointBack);
        }

        /*if (sumLayer != layer * 9 + 6)
        {
        LOG_ERROR("Invalid NWire IDs (" << tempPatterns[k]->Wires[0].id << ", " << tempPatterns[k]->Wires[1].id << ", " << tempPatterns[k]->Wires[2].id << ")!");
        return PLUS_FAIL;
        }*/

        // Check if the middle wire is the diagonal (the other two are parallel to each other and the first and the second, and the second and the third intersect)
        double wire1[3];
        double wire3[3];
        double cross[3];

        vtkMath::Subtract(tempPatterns[k]->GetWires()[0].EndPointFront, tempPatterns[k]->GetWires()[0].EndPointBack, wire1);
        vtkMath::Subtract(tempPatterns[k]->GetWires()[2].EndPointFront, tempPatterns[k]->GetWires()[2].EndPointBack, wire3);
        vtkMath::Cross(wire1, wire3, cross);
        if (vtkMath::Norm(cross) > 0.001)
        {
          LOG_ERROR("The first and third wire of layer " << layer << " are not parallel!");
          return PLUS_FAIL;
        }
        double closestTemp[3];
        double parametricCoord1, parametricCoord2;

        PlusNWire* tempNWire = (PlusNWire*)(tempPatterns[k]);

        // const_cast because vtk classes don't have proper const-ness
        if (vtkLine::DistanceBetweenLines(
              const_cast<double*>(tempNWire->GetWires()[0].EndPointFront),
              const_cast<double*>(tempNWire->GetWires()[0].EndPointBack),
              const_cast<double*>(tempNWire->GetWires()[1].EndPointFront),
              const_cast<double*>(tempNWire->GetWires()[1].EndPointBack),
              tempNWire->IntersectPosW12, closestTemp, parametricCoord1, parametricCoord2) > 0.000001)
        {
          LOG_ERROR("The first and second wire of layer " << layer << " do not intersect each other!");
          return PLUS_FAIL;
        }
        if (vtkLine::DistanceBetweenLines(
              const_cast<double*>(tempNWire->GetWires()[2].EndPointFront),
              const_cast<double*>(tempNWire->GetWires()[2].EndPointBack),
              const_cast<double*>(tempNWire->GetWires()[1].EndPointFront),
              const_cast<double*>(tempNWire->GetWires()[1].EndPointBack),
              tempNWire->IntersectPosW32, closestTemp, parametricCoord1, parametricCoord2) > 0.000001)
        {
          LOG_ERROR("The second and third wire of layer " << layer << " do not intersect each other!");
          return PLUS_FAIL;
        }
      }

      /*// Log the data pipeline if requested.
      int layer;
      for (int i=0, layer = 0; i<tempPatterns.size(); ++i, ++layer)
      {
      LOG_DEBUG("\t Intersection of wire 1 and 2 in layer " << layer << " \t= (" << it->IntersectPosW12[0] << ", " << it->IntersectPosW12[1] << ", " << it->IntersectPosW12[2] << ")");
      LOG_DEBUG("\t Intersection of wire 3 and 2 in layer " << layer << " \t= (" << it->IntersectPosW32[0] << ", " << it->IntersectPosW32[1] << ", " << it->IntersectPosW32[2] << ")");
      }*/
    }

    m_Patterns = tempPatterns;
    m_FidLineFinder.SetPatterns(m_Patterns);
    m_FidLabeling.SetPatterns(m_Patterns);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

void PlusFidPatternRecognition::SetNumberOfMaximumFiducialPointCandidates(int aValue)
{
  int numWires(0);
  for (std::vector<PlusFidPattern*>::iterator it = m_FidLabeling.GetPatterns().begin(); it != m_FidLabeling.GetPatterns().end(); ++it)
  {
    numWires += (*it)->GetWires().size();
  }
  if (aValue < numWires)
  {
    LOG_WARNING("Number of maximum fiducial point candidates is smaller than the number of wires contained in the pattern.");
  }
  this->m_FidSegmentation.SetNumberOfMaximumFiducialPointCandidates(aValue);
}
