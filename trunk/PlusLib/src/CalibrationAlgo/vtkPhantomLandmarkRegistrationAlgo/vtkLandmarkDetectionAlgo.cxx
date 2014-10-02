/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"
#include "PlusMath.h"

#include "vtkLandmarkDetectionAlgo.h"

#include "vtkDescriptiveStatistics.h"
#include "vtkDoubleArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkTable.h"
#include "vtkXMLUtilities.h"

vtkCxxRevisionMacro(vtkLandmarkDetectionAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkLandmarkDetectionAlgo);

//-----------------------------------------------------------------------------
// Default algorithm parameters
namespace
{
  const int NUMBER_EXPECTED_LANDMARKS=3;// The default expected number of landmarks to be detected
  const double STYLUSTIP_MAXIMUM_MOTION_THRESHOLD_MM=1.5;// A landmark position will be consider when the stylus tip position magnitude change is below LandmarkThresholdMm.
  const double STYLUS_SHAFT_MINIMUM_DISPLACEMENT_THRESHOLD_MM=30;//The minimum displacement of the stylus shaft(sum of bounding box lengths during detection time) to be detected as a the stylus moving 

  const int NUMBER_WINDOWS=5;//The number of windows to be detected as landmark in DetectionTime (1.0 [s]) DetectionTime/FilterWindowTime
  const int FILTER_WINDOW_SIZE=3;//The number of acquisitions in FilterWindowTime (0.2 [s]) AcquisitonRate*FilterWindowTime
  const double MAXIMUM_WINDOW_TIME_SEC=3.5;//Maximum detection time
  const int NUMBER_WINDOWS_SKIP =2;//The first windows detected as pivoting wont be used for averaging.
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetAcquisitionRate(double aquisitionRateSamplePerSec)
{
  if(aquisitionRateSamplePerSec<=0)
  {
    LOG_ERROR("Specified acquisition rate is not valid");
  }
  this->AcquisitionRate=aquisitionRateSamplePerSec;

  this->FilterWindowTimeSec=FILTER_WINDOW_SIZE/this->AcquisitionRate; //If not specified window time will span by default WINDOW_SIZE=3 of acquisition points
  this->FilterWindowSize=PlusMath::Round(this->AcquisitionRate*this->FilterWindowTimeSec);
  if(this->FilterWindowSize<1)
  {
    LOG_WARNING("The smallest window size is set to 1");
    this->FilterWindowSize=1;
  }
  LOG_INFO("SET AcquisitionRate = "<< AcquisitionRate << "[fps] WindowTimeSec = " << FilterWindowTimeSec<<"[s] DetectionTimeSec = "<< DetectionTimeSec <<"[s]");
  LOG_INFO("NumberOfWindows = "<< NumberOfWindows<< " WindowSize = "<< FilterWindowSize<< " MinimunDistanceBetweenLandmarksMM = "<< MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << StylusTipMaximumMotionThresholdMm <<"[mm]");
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetDetectionTimeSec(double detectionTimeSec)
{
  if(detectionTimeSec>this->FilterWindowTimeSec&&detectionTimeSec<MAXIMUM_WINDOW_TIME_SEC*NUMBER_WINDOWS)
  {
    this->DetectionTimeSec=detectionTimeSec;
    this->NumberOfWindows=this->DetectionTimeSec/this->FilterWindowTimeSec;
  }
  else
  {
    LOG_WARNING("Specified window time (" << detectionTimeSec << " [s]) is not correct, default "<<this->DetectionTimeSec<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetFilterWindowTimeSec(double filterWindowTimeSec)
{
  if(this->AcquisitionRate<=0)
  {
    LOG_ERROR("There is no acquisition rate specified");
  }
  if(filterWindowTimeSec>0&& filterWindowTimeSec<=this->DetectionTimeSec)
  {
    this->FilterWindowTimeSec=filterWindowTimeSec;
    this->FilterWindowSize=PlusMath::Round(this->AcquisitionRate*this->FilterWindowTimeSec);
    if(this->FilterWindowSize<1)
    {
      LOG_WARNING("The smallest window size is set to 1");
      this->FilterWindowSize=1;
    }
    this->NumberOfWindows=this->DetectionTimeSec/this->FilterWindowTimeSec;
  }
  else
  {
    LOG_WARNING("Specified window time (" << filterWindowTimeSec << " [s]) is not correct, default "<<this->FilterWindowTimeSec<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetStylusShaftMinimumDisplacementThresholdMm(double StylusShaftMinimumDisplacementThresholdMm)
{
  if(StylusShaftMinimumDisplacementThresholdMm>0)
  {
    this->StylusShaftMinimumDisplacementThresholdMm=StylusShaftMinimumDisplacementThresholdMm;
  }
  else
  {
    LOG_WARNING("Specified landmark threshold (" << StylusShaftMinimumDisplacementThresholdMm << " [mm]) is not correct, default "<<this->StylusShaftMinimumDisplacementThresholdMm<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetStylusTipMaximumMotionThresholdMm(double stylusTipMaximumMotionThresholdMm)
{
  if(stylusTipMaximumMotionThresholdMm>0)
  {
    this->StylusTipMaximumMotionThresholdMm=stylusTipMaximumMotionThresholdMm;
  }
  else
  {
    LOG_WARNING("Specified landmark threshold (" << stylusTipMaximumMotionThresholdMm << " [mm]) is not correct, default "<<this->StylusTipMaximumMotionThresholdMm<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetNumberOfExpectedLandmarks(int numberOfExpectedLandmarks)
{
  if(numberOfExpectedLandmarks>0)
  {
    this->NumberOfExpectedLandmarks=numberOfExpectedLandmarks;
  }
  else
  {
    LOG_WARNING("Specified expected number of landmarks (" << numberOfExpectedLandmarks << " is not correct, default "<<this->NumberOfExpectedLandmarks<<" is used instead");
  }
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::vtkLandmarkDetectionAlgo()
{
  this->ReferenceCoordinateFrame = NULL;
  this->DetectedLandmarkPoints_Reference = NULL;
  vtkSmartPointer<vtkPoints> detectedLandmarkPoints_Reference = vtkSmartPointer<vtkPoints>::New();
  this->SetDetectedLandmarkPoints_Reference(detectedLandmarkPoints_Reference);

  this->NewLandmarkFound=false;

  this->PartialInsertedPoints=0;
  this->CurrentStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();

  this->NumberOfWindows=NUMBER_WINDOWS;
  this->FilterWindowSize=FILTER_WINDOW_SIZE;

  this->AcquisitionRate=20.0;
  this->FilterWindowTimeSec=0.2;
  this->DetectionTimeSec=1.0;
  this->StylusShaftMinimumDisplacementThresholdMm = STYLUS_SHAFT_MINIMUM_DISPLACEMENT_THRESHOLD_MM;
  this->StylusTipMaximumMotionThresholdMm = STYLUSTIP_MAXIMUM_MOTION_THRESHOLD_MM;
  this->NumberOfExpectedLandmarks=NUMBER_EXPECTED_LANDMARKS;
  this->MinimunDistanceBetweenLandmarksMm=15.0;
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::~vtkLandmarkDetectionAlgo()
{
  this->RemoveAllFilterWindows();
}

//-----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::RemoveAllFilterWindows()
{
  this->StylusTipToReferenceTransformsList.clear();
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ResetDetection()
{
  this->NewLandmarkFound=false;
  LOG_INFO("Reset");

  this->PartialInsertedPoints=0;
  RemoveAllFilterWindows();
  this->CurrentStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();
  this->DetectedLandmarkPoints_Reference->Reset();
  this->DetectedLandmarkPoints_Reference->Initialize();
  this->NumberOfWindowsFoundPerLandmark.clear();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertLandmark_Reference(double* stylusTipPosition_Reference)
{
  if(GetNearExistingLandmarkId(stylusTipPosition_Reference)==-1)
  {
    NumberOfWindowsFoundPerLandmark.push_back(1.0);
    this->DetectedLandmarkPoints_Reference->InsertNextPoint(stylusTipPosition_Reference);
    this->NewLandmarkFound=true;
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::DeleteLastLandmark()
{
  if(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()>0)
  {
    double landmarkFound_Reference[4] = {0,0,0,1};
    this->DetectedLandmarkPoints_Reference->GetPoint(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()-1, landmarkFound_Reference);
    this->DetectedLandmarkPoints_Reference->GetData()->RemoveTuple(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()-1);
    this->NumberOfWindowsFoundPerLandmark[this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()]=-1;
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::AverageFilterStylusTipPositionsWindow(double* stylusTipAverageFiltered_Reference)
{
  double stylusTipPositionSum_Reference [4] = {0,0,0,1};
  std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsList.end();
  int i=0;
  do
  {
    i++;
    stylusTipToReferenceTransformIt--;
    stylusTipPositionSum_Reference[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
    stylusTipPositionSum_Reference[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
    stylusTipPositionSum_Reference[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
  }while(i<this->FilterWindowSize/*||*stylusTipToReferenceTransformIt!=*LastStylusTipIterator*/);

  this->CurrentStylusTipIterator=stylusTipToReferenceTransformIt;
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator--;
  if(i==this->FilterWindowSize)
  {
    stylusTipAverageFiltered_Reference[0]=stylusTipPositionSum_Reference[0]/this->FilterWindowSize;
    stylusTipAverageFiltered_Reference[1]=stylusTipPositionSum_Reference[1]/this->FilterWindowSize;
    stylusTipAverageFiltered_Reference[2]=stylusTipPositionSum_Reference[2]/this->FilterWindowSize;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::KeepLastWindow()
{
  int i=0;
  for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    if(i%this->FilterWindowSize==0)
    {
      LOG_DEBUG( "\n");
    }
    LOG_DEBUG("\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ");
    i++;
  }
  LOG_DEBUG("\n Erase before P( " << -(*this->CurrentStylusTipIterator)->Element[0][3]<<", "<< -(*this->CurrentStylusTipIterator)->Element[1][3]<<", "<< -(*this->CurrentStylusTipIterator)->Element[2][3]<<") \n");

  this->CurrentStylusTipIterator++;
  std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
  this->CurrentStylusTipIterator--;
  int j=0;
  while ( markerToReferenceTransformIt!=this->CurrentStylusTipIterator)
  {
    j++;
    this->StylusTipToReferenceTransformsList.pop_front();
    markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
  }

  for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    LOG_DEBUG( "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", "<< -(*markerToReferenceTransformIt)->Element[1][3]<<", "<< -(*markerToReferenceTransformIt)->Element[2][3]<<") ");
  }
  for(int a=0; a<j/this->FilterWindowSize;a++)
  {
    StylusTipFilteredList_Reference.pop_front();
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform)
{
  //Point 10 cm above the stylus tip, if it moves(window change bigger than AboveLandmarkThresholdMm) while the tip is static (window change smaller than LandmarkThresholdMm then it is landmark point.
  float pointAboveStylusTip_StylusTip[4]={100,0,0,1};
  float pointAboveStylusTip_Reference[4]={0,0,0,1};
  double stylusTipChange_Reference[4] = {0,0,0,1};
  double aboveStylusTipChange[4] = {0,0,0,1};
  stylusTipToReferenceTransform->MultiplyPoint(pointAboveStylusTip_StylusTip, pointAboveStylusTip_Reference);

  this->StylusTipToReferenceTransformsList.push_back(stylusTipToReferenceTransform);
  this->PartialInsertedPoints++;
  if(this->PartialInsertedPoints>=this->FilterWindowSize &&  this->DetectedLandmarkPoints_Reference->GetNumberOfPoints() < this->NumberOfExpectedLandmarks)
  {
    if(StylusTipFilteredList_Reference.size()<1)
    {
      this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();
    }
    double stylusTipFiltered_Reference[4]={0,0,0,1};
    if (AverageFilterStylusTipPositionsWindow( stylusTipFiltered_Reference)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    std::vector<double> stylusTipFilteredVector_Reference (stylusTipFiltered_Reference, stylusTipFiltered_Reference+sizeof(stylusTipFiltered_Reference)/sizeof(stylusTipFiltered_Reference[0]));
    StylusTipFilteredList_Reference.push_back(stylusTipFilteredVector_Reference);

    if(StylusTipFilteredList_Reference.size()>1)
    {
      std::list< std::vector<double> >::iterator pointIt=StylusTipFilteredList_Reference.end();
      --pointIt;--pointIt;

      double lastStylusTipFiltered_Reference[4] = {(*pointIt)[0],(*pointIt)[1],(*pointIt)[2],0};

      stylusTipChange_Reference[0]=lastStylusTipFiltered_Reference[0]-stylusTipFiltered_Reference[0];
      stylusTipChange_Reference[1]=lastStylusTipFiltered_Reference[1]-stylusTipFiltered_Reference[1];
      stylusTipChange_Reference[2]=lastStylusTipFiltered_Reference[2]-stylusTipFiltered_Reference[2];

      this->StylusShaftPathBoundingBox.AddPoint(pointAboveStylusTip_Reference[0],pointAboveStylusTip_Reference[1],pointAboveStylusTip_Reference[2]);
      if(vtkMath::Norm(stylusTipChange_Reference)<this->StylusTipMaximumMotionThresholdMm /*&& vtkMath::Norm(aboveStylusTipChange)>this->AboveLandmarkThresholdMm*/ )
      {
        LOG_DEBUG("\nDif last points (" <<abs(lastStylusTipFiltered_Reference[0]-stylusTipFiltered_Reference[0])<< ", "<<abs(lastStylusTipFiltered_Reference[1]-stylusTipFiltered_Reference[1])<< ", "<<abs(lastStylusTipFiltered_Reference[2]-stylusTipFiltered_Reference[2])<< ")\n");
        LOG_DEBUG("Window Landmark ("<< lastStylusTipFiltered_Reference[0]<< ", "<< lastStylusTipFiltered_Reference[1]<< ", "<< lastStylusTipFiltered_Reference[2]<< ") found keep going!!");
        int i =0;
        for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
          markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
        {
          if(i%this->FilterWindowSize==0)
          {
            LOG_DEBUG( "\n");
          }
          LOG_DEBUG("\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<")");
          i++;
        }
      }
      else
      {
        LOG_DEBUG("\nDif last points (" <<abs(lastStylusTipFiltered_Reference[0]-stylusTipFiltered_Reference[0])<< ", "<<abs(lastStylusTipFiltered_Reference[1]-stylusTipFiltered_Reference[1])<< ", "<<abs(lastStylusTipFiltered_Reference[2]-stylusTipFiltered_Reference[2])<< ")\n");
        KeepLastWindow();
        while(StylusTipFilteredList_Reference.size()>1)
        {
          StylusTipFilteredList_Reference.pop_front();
        }
        this->StylusShaftPathBoundingBox.Reset();
        this->StylusShaftPathBoundingBox.AddPoint(pointAboveStylusTip_Reference[0],pointAboveStylusTip_Reference[1],pointAboveStylusTip_Reference[2]);
      }
    }
    this->LastAboveStylusTip[0]=pointAboveStylusTip_Reference[0];
    this->LastAboveStylusTip[1]=pointAboveStylusTip_Reference[1];
    this->LastAboveStylusTip[2]=pointAboveStylusTip_Reference[2];
    this->LastAboveStylusTip[3]=pointAboveStylusTip_Reference[3];

    this->PartialInsertedPoints=0;
    double lengths[3];
    this->StylusShaftPathBoundingBox.GetLengths(lengths);
    if(StylusTipFilteredList_Reference.size()>=this->NumberOfWindows  )
    {
      this->StylusShaftPathBoundingBox.Reset();
      if((lengths[0]+lengths[1]+lengths[2])>this->StylusShaftMinimumDisplacementThresholdMm)
      {
        EstimateLandmarkPointPosition();
      }
      else
      {
        this->StylusShaftPathBoundingBox.AddPoint(this->LastAboveStylusTip);
        KeepLastWindow();
      }
    }
  }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkLandmarkDetectionAlgo::GetNearExistingLandmarkId(double* stylusTipPosition_Reference)
{
  double detectedLandmark_Reference[4] = {0,0,0,1};
  double landmarkDifference_Reference[4] = {0,0,0,1};

  for(int id=0; id<this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();id++)
  {
    this->DetectedLandmarkPoints_Reference->GetPoint(id, detectedLandmark_Reference);

    landmarkDifference_Reference[0]=detectedLandmark_Reference[0]-stylusTipPosition_Reference[0];
    landmarkDifference_Reference[1]=detectedLandmark_Reference[1]-stylusTipPosition_Reference[1];
    landmarkDifference_Reference[2]=detectedLandmark_Reference[2]-stylusTipPosition_Reference[2];
    ////average if it is really close
    //if(vtkMath::Norm(landmarkDifference)<this->LandmarkThresholdMm/5 )
    //{
    //  NumberOfWindowsFoundPerLandmark[id]=NumberOfWindowsFoundPerLandmark[id]+1.0;
    //  landmarkFound[0]=(landmarkFound[0]*(NumberOfWindowsFoundPerLandmark[id]-1)+stylusTipPosition[0])/NumberOfWindowsFoundPerLandmark[id];
    //  landmarkFound[1]=(landmarkFound[1]*(NumberOfWindowsFoundPerLandmark[id]-1)+stylusTipPosition[1])/NumberOfWindowsFoundPerLandmark[id];
    //  landmarkFound[2]=(landmarkFound[2]*(NumberOfWindowsFoundPerLandmark[id]-1)+stylusTipPosition[2])/NumberOfWindowsFoundPerLandmark[id];
    //  //Only using the first detection time landmark detection and not the average might be more accurate
    //  this->LandmarkPointsReference->InsertPoint(id,landmarkFound);
    //  LOG_INFO("Landmark found "<<NumberOfWindowsFoundPerLandmark[id]<<" times. Average("<< landmarkFound[0]<< ", "<< landmarkFound[1]<< ", "<< landmarkFound[2]<< ") set")
    //    return false;
    //}
    ////if it is relatively close it might be same landmark outlier do not average it 
    //else if(vtkMath::Norm(landmarkDifference)<this->MinimunDistanceBetweenLandmarksMm/3)
    if(vtkMath::Norm(landmarkDifference_Reference)<this->MinimunDistanceBetweenLandmarksMm/3)
    {
      return id;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::EstimateLandmarkPointPosition()
{
  int i=0; int j=0;
  double stylusPositionMean_Reference[4] = {0,0,0,1};
  double stylusPositionStdev_Reference[4] = {0,0,0,1};

  vtkSmartPointer<vtkDoubleArray> datasetArrX = vtkSmartPointer<vtkDoubleArray>::New();
  datasetArrX->SetNumberOfComponents( 1 );
  datasetArrX->SetName( "X" );
  vtkSmartPointer<vtkDoubleArray> datasetArrY = vtkSmartPointer<vtkDoubleArray>::New();
  datasetArrY->SetNumberOfComponents( 1 );
  datasetArrY->SetName( "Y" );
  vtkSmartPointer<vtkDoubleArray> datasetArrZ = vtkSmartPointer<vtkDoubleArray>::New();
  datasetArrZ->SetNumberOfComponents( 1 );
  datasetArrZ->SetName( "Z" );

  for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    stylusTipToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++stylusTipToReferenceTransformIt)
  {
    //Don't use first NUMBER_WINDOWS_SKIP windows
    if(i>=(this->FilterWindowSize*NUMBER_WINDOWS_SKIP) && i <(this->NumberOfWindows)*this->FilterWindowSize)
    {
      datasetArrX->InsertNextValue( (*stylusTipToReferenceTransformIt)->Element[0][3] );
      datasetArrY->InsertNextValue( (*stylusTipToReferenceTransformIt)->Element[1][3] );
      datasetArrZ->InsertNextValue( (*stylusTipToReferenceTransformIt)->Element[2][3] );
      j++;
    }
    i++;
  }

  vtkSmartPointer<vtkTable> simpleTable = vtkSmartPointer<vtkTable>::New();
  simpleTable->AddColumn( datasetArrX );
  simpleTable->AddColumn( datasetArrY );
  simpleTable->AddColumn( datasetArrZ );

  // Pairs of interest
  int nMetrics = 3;
  vtkStdString columns[] =
  {
    "X",
    "Y",
    "Z"
  };

  // Set descriptive statistics algorithm and its input data port
  vtkSmartPointer<vtkDescriptiveStatistics> descriptiveStats = vtkSmartPointer<vtkDescriptiveStatistics>::New();
  descriptiveStats->SetInputData_vtk5compatible( simpleTable );

  // Select Columns of Interest
  for ( int i = 0; i< nMetrics; ++ i )
  {
    descriptiveStats->AddColumn( columns[i] );
  }

  // Test Learn, Derive, Test, and Assess options
  descriptiveStats->SetLearnOption( true );
  descriptiveStats->SetDeriveOption( true );
  descriptiveStats->SetAssessOption( false );
  descriptiveStats->SetTestOption( false );
  descriptiveStats->Update();

  vtkSmartPointer<vtkMultiBlockDataSet> outputMetaDS = vtkMultiBlockDataSet::SafeDownCast( descriptiveStats->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  vtkSmartPointer<vtkTable> outputPrimary = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 0 ) );
  vtkSmartPointer<vtkTable> outputDerived = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 1 ) );

  if(j==(this->NumberOfWindows-NUMBER_WINDOWS_SKIP)*this->FilterWindowSize)
  {
    for ( vtkIdType r = 0; r < outputPrimary->GetNumberOfRows(); ++ r )
    {
        stylusPositionMean_Reference[r]=outputPrimary->GetValueByName( r, "Mean" ).ToDouble();
        stylusPositionStdev_Reference[r]=outputDerived->GetValueByName( r, "Standard Deviation" ).ToDouble();
    }
    if(GetNearExistingLandmarkId(stylusPositionMean_Reference)==-1)
    {
      NumberOfWindowsFoundPerLandmark.push_back(1.0);
      this->DetectedLandmarkPoints_Reference->InsertNextPoint(stylusPositionMean_Reference);
      LOG_DEBUG("\nSTD deviation ( " << stylusPositionStdev_Reference[0]<< ", "<< stylusPositionStdev_Reference[1]<< ", "<< stylusPositionStdev_Reference[2]<< ") " );
      LOG_DEBUG("STD deviation magnitude " << vtkMath::Norm(stylusPositionStdev_Reference));
      this->NewLandmarkFound=true;
    }
    RemoveAllFilterWindows();
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::IsNewLandmarkPointFound(bool &found)
{
  found=this->NewLandmarkFound;
  if (this->NewLandmarkFound==true)
  {
    this->NewLandmarkFound=false;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::IsLandmarkDetectionCompleted(bool &completed)
{
  if(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()>=this->NumberOfExpectedLandmarks)
  {
    completed=true;
  }
  else
  {
    completed=false;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkLandmarkDetectionAlgo::GetDetectedLandmarksString( double aPrecision/*=3*/ )
{
  if (this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()>0)
  {
    std::ostrstream s;
    double landmarkFound[4] = {0,0,0,1};
    s << std::fixed << std::setprecision(aPrecision);
    for (int id =0; id<this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();id++)
    {
      this->DetectedLandmarkPoints_Reference->GetPoint(id, landmarkFound);
      s <<"\nLandmark "<< id+1 << " (" << landmarkFound[0]<<", " << landmarkFound[1]<<", " << landmarkFound[2]<<")";
    }
    s << std::ends;  
    return s.str();
  }
  else
  {
    return "\nNo landmarks found";
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(PhantomLandmarkLandmarkDetectionElement, aConfig, "vtkPhantomLandmarkRegistrationAlgo");
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, PhantomLandmarkLandmarkDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, AcquisitionRate, PhantomLandmarkLandmarkDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, NumberOfExpectedLandmarks, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FilterWindowTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DetectionTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusShaftMinimumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusTipMaximumMotionThresholdMm, PhantomLandmarkLandmarkDetectionElement);

  LOG_INFO("AcquisitionRate = "<< AcquisitionRate << "[fps] WindowTimeSec = " << FilterWindowTimeSec<<"[s] DetectionTimeSec = "<< DetectionTimeSec <<"[s]");
  LOG_INFO("NumberOfWindows = "<< NumberOfWindows<< " WindowSize = "<< FilterWindowSize<< " MinimunDistanceBetweenLandmarksMm = "<< MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << StylusTipMaximumMotionThresholdMm <<"[mm]");

  return PLUS_SUCCESS;
}



