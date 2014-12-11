/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkDoubleArray.h"
#include "vtkTable.h"

#include "vtkReadTrackedSignals.h"
#include "vtkTransformRepository.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkReadTrackedSignals);

//-----------------------------------------------------------------------------
vtkReadTrackedSignals::vtkReadTrackedSignals()
{
  m_SignalTimeRangeMin=0.0;
  m_SignalTimeRangeMax=-1.0;

  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->ObjectPivotPointCoordinateFrame = NULL;
}

//-----------------------------------------------------------------------------
vtkReadTrackedSignals::~vtkReadTrackedSignals()
{
}

//----------------------------------------------------------------------------
void vtkReadTrackedSignals::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 

//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::SetTrackerFrames(vtkTrackedFrameList* trackerFrames)
{
  m_TrackerFrames = trackerFrames;
}

//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::SetSignalTimeRange(double rangeMin, double rangeMax)
{
  m_SignalTimeRangeMin=rangeMin;
  m_SignalTimeRangeMax=rangeMax;
}


//-----------------------------------------------------------------------------
PlusStatus vtkReadTrackedSignals::VerifyInputFrames()
{
  if(m_TrackerFrames == NULL)
  {
    LOG_ERROR("Tracker input data verification failed: no tracker data is set");
    return PLUS_FAIL;
  }
  // Make sure tracker frame list is not empty
  if(m_TrackerFrames->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("Tracker input data verification failed: tracker data set is empty");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkReadTrackedSignals::ComputeTrackerPositionMetric()
{
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();

  vtkSmartPointer<vtkMatrix4x4> pivotToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  PlusTransformName transformName(ObjectMarkerCoordinateFrame,ReferenceCoordinateFrame);
  itk::Point<double, 3> stylusTip; stylusTip[0] = stylusTip[1] = stylusTip[2] = 0.0;
  itk::Point<double, 3> zeroRef; zeroRef[0] = zeroRef[1] = zeroRef[2] = 0.0;

  m_SignalTimestamps.clear();
  m_SignalStylusRef.clear();
  m_SignalStylusTipRef.clear();
  m_SignalStylusTipSpeed.clear();

  // Find the mean tracker position
  itk::Point<double, 3> trackerPositionSum;
  trackerPositionSum[0] = trackerPositionSum[1] = trackerPositionSum[2] = 0.0;

  std::deque< itk::Point<double, 3> > trackerPositions;
  int numberOfValidFrames = 0;
  bool signalTimeRangeDefined=(m_SignalTimeRangeMin<=m_SignalTimeRangeMax);
  double previousStylusTipPosition=0.0;
  for(int frame = 0; frame < m_TrackerFrames->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame *trackedFrame = m_TrackerFrames->GetTrackedFrame(frame);

    if (signalTimeRangeDefined && (trackedFrame->GetTimestamp()<m_SignalTimeRangeMin || trackedFrame->GetTimestamp()>m_SignalTimeRangeMax))
    {
      // frame is out of the specified signal range
      continue;
    }

    transformRepository->SetTransforms(*trackedFrame);
    
    vtkSmartPointer<vtkMatrix4x4> probeToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    bool valid = false;
    transformRepository->GetTransform(transformName, probeToReferenceTransform, &valid);
    if (!valid)
    {
      // There is no available transform for this frame; skip that frame
      LOG_INFO("There is no available transform for this frame; skip frame "<<trackedFrame->GetTimestamp()<<" [s]")
      continue;
    }  

    //  Store current tracker position 
    itk::Point<double, 3> currTrackerPosition;
    currTrackerPosition[0] = probeToReferenceTransform->GetElement(0, 3);
    currTrackerPosition[1] = probeToReferenceTransform->GetElement(1, 3);
    currTrackerPosition[2] = probeToReferenceTransform->GetElement(2, 3);
    trackerPositions.push_back(currTrackerPosition);

    // Add current tracker position to the running total
    trackerPositionSum[0] = trackerPositionSum[0] + probeToReferenceTransform->GetElement(0, 3);
    trackerPositionSum[1] = trackerPositionSum[1] + probeToReferenceTransform->GetElement(1, 3);
    trackerPositionSum[2] = trackerPositionSum[2] + probeToReferenceTransform->GetElement(2, 3);
    ++numberOfValidFrames;

    m_SignalTimestamps.push_back(trackedFrame->GetTimestamp()); // These timestamps will be in the desired time range
    m_SignalStylusRef.push_back(currTrackerPosition.EuclideanDistanceTo(zeroRef));

    vtkMatrix4x4::Multiply4x4(probeToReferenceTransform, this->StylusTipToStylusTransform,pivotToReferenceTransform);
    stylusTip[0] = pivotToReferenceTransform->GetElement(0, 3);
    stylusTip[1] = pivotToReferenceTransform->GetElement(1, 3);
    stylusTip[2] = pivotToReferenceTransform->GetElement(2, 3);

    m_SignalStylusTipRef.push_back(stylusTip.EuclideanDistanceTo(zeroRef));
    m_SignalStylusTipSpeed.push_back(stylusTip.EuclideanDistanceTo(zeroRef)-previousStylusTipPosition);
    previousStylusTipPosition=stylusTip.EuclideanDistanceTo(zeroRef);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkReadTrackedSignals::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(pivotCalibrationElement, aConfig, "vtkPivotCalibrationAlgo");
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectMarkerCoordinateFrame, pivotCalibrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, pivotCalibrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectPivotPointCoordinateFrame, pivotCalibrationElement);

  vtkSmartPointer<vtkTransformRepository> transformRepositoryCalibration = vtkSmartPointer<vtkTransformRepository>::New();

  if ( transformRepositoryCalibration->ReadConfiguration(aConfig) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }

  PlusTransformName StylusTipToStylusTransformName(ObjectPivotPointCoordinateFrame, ObjectMarkerCoordinateFrame);
  this->StylusTipToStylusTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  bool valid = false;
  if (transformRepositoryCalibration->GetTransform(StylusTipToStylusTransformName, this->StylusTipToStylusTransform/*, &valid*/) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read StylusTipToStylu Coordinate Definition!"); 
    exit(EXIT_FAILURE);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkReadTrackedSignals::Update()
{
  if (VerifyInputFrames()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (ComputeTrackerPositionMetric()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetTimestamps(std::deque<double> &timestamps)
{
  timestamps=m_SignalTimestamps;
}

//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetSignalStylusRef(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalStylusRef;
}
//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetSignalStylusTipRef(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalStylusTipRef;
}
//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetSignalStylusTipSpeed(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalStylusTipSpeed;
}
