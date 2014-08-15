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
vtkCxxRevisionMacro(vtkReadTrackedSignals, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkReadTrackedSignals);

//-----------------------------------------------------------------------------
vtkReadTrackedSignals::vtkReadTrackedSignals()
{
  m_SignalTimeRangeMin=0.0;
  m_SignalTimeRangeMax=-1.0;
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
void vtkReadTrackedSignals::SetProbeToReferenceTransformName(const std::string& transformName)
{
  m_ProbeToReferenceTransformName = transformName;
}

//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::SetCalibrationConfigName(const std::string& calibrationConfigName)
{
  m_CalibrationConfigName = calibrationConfigName;
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
  vtkSmartPointer<vtkTransformRepository> transformRepositoryCalibration = vtkSmartPointer<vtkTransformRepository>::New();
  PlusTransformName transformName;

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(m_CalibrationConfigName.c_str()));
  if (configRootElement == NULL)
  {  
    LOG_ERROR("Unable to read configuration from file " << m_CalibrationConfigName.c_str()); 
    exit(EXIT_FAILURE);
  }

  if ( transformRepositoryCalibration->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkMatrix4x4> referenceToPivotTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> referenceToSylusTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> stylusTipToStylusTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> stylusToStylusTipTransform = vtkSmartPointer<vtkMatrix4x4>::New();   bool valid = false;

  vtkSmartPointer<vtkMatrix4x4> pivotToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> stylusToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();

  transformName.SetTransformName("StylusToStylusTip");
  transformRepositoryCalibration->GetTransform(transformName, stylusToStylusTipTransform, &valid);
  transformName.SetTransformName("StylusTipToStylus");
  transformRepositoryCalibration->GetTransform(transformName, stylusTipToStylusTransform, &valid);

  int pivotFrame=m_TrackerFrames->GetNumberOfTrackedFrames()/2;
  transformRepository->SetTransforms(*(m_TrackerFrames->GetTrackedFrame(pivotFrame)));

  transformName.SetTransformName("ReferenceToStylus");
  transformRepository->GetTransform(transformName, referenceToSylusTransform, &valid);
  vtkMatrix4x4::Multiply4x4(stylusToStylusTipTransform,referenceToSylusTransform,referenceToPivotTransform);

  itk::Point<double, 3> stylusTip; stylusTip[0] = stylusTip[1] = stylusTip[2] = 0.0;
  itk::Point<double, 3> zeroRef; zeroRef[0] = zeroRef[1] = zeroRef[2] = 0.0;

  m_SignalTimestamps.clear();
  m_SignalStylusRef.clear();
  m_SignalStylusTipRef.clear();
  m_SignalZ.clear();
  m_SignalStylusTipSpeed.clear();

  m_SignalValues.clear();

  // Find the mean tracker position
  itk::Point<double, 3> trackerPositionSum;
  trackerPositionSum[0] = trackerPositionSum[1] = trackerPositionSum[2] = 0.0;

  if (transformName.SetTransformName(m_ProbeToReferenceTransformName.c_str())!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot compute tracker position metric, transform name is invalid ("<<m_ProbeToReferenceTransformName<<")");
    return PLUS_FAIL;
  }

  std::deque<itk::Point<double, 3> > trackerPositions;
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

    //double temp[4]={currTrackerPosition[0], currTrackerPosition[1], currTrackerPosition[2], 1};

    //double * currTrackerPositionFromPivot=referenceToPivotTransform->MultiplyDoublePoint(temp);

    //currTrackerPosition[0] = currTrackerPositionFromPivot[0];
    //currTrackerPosition[1] = currTrackerPositionFromPivot[1];
    //currTrackerPosition[2] = currTrackerPositionFromPivot[2];

    m_SignalStylusRef.push_back(currTrackerPosition.EuclideanDistanceTo(zeroRef));


    vtkMatrix4x4::Multiply4x4(probeToReferenceTransform,stylusTipToStylusTransform,pivotToReferenceTransform);
    stylusTip[0] = pivotToReferenceTransform->GetElement(0, 3);
    stylusTip[1] = pivotToReferenceTransform->GetElement(1, 3);
    stylusTip[2] = pivotToReferenceTransform->GetElement(2, 3);

    m_SignalStylusTipRef.push_back(stylusTip.EuclideanDistanceTo(zeroRef));
    m_SignalStylusTipSpeed.push_back(stylusTip.EuclideanDistanceTo(zeroRef)-previousStylusTipPosition);
    previousStylusTipPosition=stylusTip.EuclideanDistanceTo(zeroRef);
    m_SignalZ.push_back(currTrackerPosition[2]);
  }

  //// Calculate the principal axis of motion (using PCA)
  //itk::Point<double,3> principalAxisOfMotion;
  //ComputePrincipalAxis(trackerPositions, principalAxisOfMotion, numberOfValidFrames);

  //// Compute the mean tracker poisition
  //itk::Point<double, 3> meanTrackerPosition;
  //meanTrackerPosition[0] = ( trackerPositionSum[0] / static_cast<double>(numberOfValidFrames) );
  //meanTrackerPosition[1] = ( trackerPositionSum[1] / static_cast<double>(numberOfValidFrames) );
  //meanTrackerPosition[2] = ( trackerPositionSum[2] / static_cast<double>(numberOfValidFrames) );

  //// Project the mean tracker position on the prinicipal axis of motion
  //double meanTrackerPositionProjection = meanTrackerPosition[0] * principalAxisOfMotion[0] 
  //                                     + meanTrackerPosition[1] * principalAxisOfMotion[1] 
  //                                     + meanTrackerPosition[2] * principalAxisOfMotion[2];


  ////  For each tracker position in the recorded tracker sequence, get its translation from reference.
  //for ( int frame = 0; frame < m_SignalTimestamps.size(); ++frame )
  //{
  //  // Project the current tracker position onto the principal axis of motion
  //  double currTrackerPositionProjection = trackerPositions.at(frame).GetElement(0) * principalAxisOfMotion[0]
  //                                       + trackerPositions.at(frame).GetElement(1) * principalAxisOfMotion[1]
  //                                       + trackerPositions.at(frame).GetElement(2) * principalAxisOfMotion[2];

  //  //  Store this translation and corresponding timestamp
  //  m_SignalValues.push_back(currTrackerPositionProjection);
  //}

  return PLUS_SUCCESS;
}

////-----------------------------------------------------------------------------
//void vtkReadTrackedSignals::ComputePrincipalAxis(std::deque<itk::Point<double, 3> > &trackerPositions, itk::Point<double,3> &principalAxisOfMotion, int numValidFrames)
//{
//  // Set the X-values
//  const char m0Name[] = "M0";
//  vtkSmartPointer<vtkDoubleArray> dataset1Arr = vtkSmartPointer<vtkDoubleArray>::New();
//  dataset1Arr->SetNumberOfComponents(1);
//  dataset1Arr->SetName( m0Name );
//  for(int i = 0; i < numValidFrames; ++i)
//  {
//    dataset1Arr->InsertNextValue(trackerPositions[i].GetElement(0));
//  }
//
//  // Set the Y-values
//  const char m1Name[] = "M1";
//  vtkSmartPointer<vtkDoubleArray> dataset2Arr = vtkSmartPointer<vtkDoubleArray>::New();
//  dataset2Arr->SetNumberOfComponents(1);
//  dataset2Arr->SetName( m1Name );
//  for(int i = 0; i < numValidFrames; ++i)
//  {
//    dataset2Arr->InsertNextValue(trackerPositions[i].GetElement(1));
//  }
//
//  // Set the Z-values
//  const char m2Name[] = "M2";
//  vtkSmartPointer<vtkDoubleArray> dataset3Arr = vtkSmartPointer<vtkDoubleArray>::New();
//  dataset3Arr->SetNumberOfComponents(1);
//  dataset3Arr->SetName( m2Name );
//  for(int i = 0; i < numValidFrames; ++i)
//  {
//    dataset3Arr->InsertNextValue(trackerPositions[i].GetElement(2));
//  }
//
//  vtkSmartPointer<vtkTable> datasetTable = vtkSmartPointer<vtkTable>::New();
//  datasetTable->AddColumn(dataset1Arr);
//  datasetTable->AddColumn(dataset2Arr);
//  datasetTable->AddColumn(dataset3Arr);
//
//  vtkSmartPointer<vtkPCAStatistics> pcaStatistics = vtkSmartPointer<vtkPCAStatistics>::New();
//
//  pcaStatistics->SetInputData_vtk5compatible( vtkStatisticsAlgorithm::INPUT_DATA, datasetTable );
// 
//  pcaStatistics->SetColumnStatus("M0", 1 );
//  pcaStatistics->SetColumnStatus("M1", 1 );
//  pcaStatistics->SetColumnStatus("M2", 1 );
//  pcaStatistics->RequestSelectedColumns();
//  pcaStatistics->SetDeriveOption(true);
//  pcaStatistics->Update();
// 
//  // Get the eigenvector corresponding to the largest eigenvalue (i.e. the principal axis). The
//  // eigenvectors are stored with the eigenvector corresponding to the largest eigenvalue stored
//  // first (i.e. in the "zero" position) and the eigenvector corresponding to the smallest eigenvalue
//  // stored last. 
//  vtkSmartPointer<vtkDoubleArray> eigenvector = vtkSmartPointer<vtkDoubleArray>::New();
//  pcaStatistics->GetEigenvector(0, eigenvector);
//
//  for(int i = 0; i < eigenvector->GetNumberOfComponents(); ++i)
//  {
//    principalAxisOfMotion[i] = eigenvector->GetComponent(0,i);
//  }
//}

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
void vtkReadTrackedSignals::GetDetectedTimestamps(std::deque<double> &timestamps)
{
  timestamps=m_SignalTimestamps;
}

//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetDetectedSignalStylusRef(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalStylusRef;
}
//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetDetectedSignalStylusTipRef(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalStylusTipRef;
}
//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetDetectedSignalZ(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalZ;
}
//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetDetectedSignalStylusTipFromPivot(std::deque<double> &signalComponent)
{
  signalComponent=m_SignalStylusTipSpeed;
}



//-----------------------------------------------------------------------------
void vtkReadTrackedSignals::GetDetectedPositions(std::deque<double> &positions)
{
  positions=m_SignalValues;
}
