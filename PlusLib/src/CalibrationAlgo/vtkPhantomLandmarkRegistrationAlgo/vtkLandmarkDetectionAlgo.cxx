/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"

#include "vtkLandmarkDetectionAlgo.h"
#include "vtkTransformRepository.h"
#include "PlusMath.h"

#include "vtkMultiBlockDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkDescriptiveStatistics.h"
#include "vtkTable.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"


vtkCxxRevisionMacro(vtkLandmarkDetectionAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkLandmarkDetectionAlgo);

//-----------------------------------------------------------------------------
// Default algorithm parameters
namespace
{
  const int EXPECTED_LANDMARKS_NUMBER=3;// The default expected number of landmarks to be detected
  const double ABOVE_LANDMARK_THRESHOLD_MM=10.0;//Above the landmark threshold is used to detect stylus pivoting and not static. When a point 100 mm above the stylus tip magnitude change is bigger than AboveLandmarkThresholdMm, the stylus is pivoting.
  const double LANDMARK_THRESHOLD_MM=1.5;// A landmark position will be consider when the stylus tip position magnitude change is below LandmarkThresholdMm.

  const int NUMBER_WINDOWS=5;//The number of windows to be detected as landmark in DetectionTime (1.0 [s]) DetectionTime/WindowTime
  const int WINDOW_SIZE=3;//The number of acquisitions in WindowTime (0.2 [s]) AcquisitonRate*WindowTime
  const double MAXIMUM_WINDOW_TIME_SEC=3.5;//Maximum detection time
  const int NUMBER_WINDOWS_SKIP =2;//The first windows detected as pivoting wont be used for averaging.
  const double MIN_LENGTH_ABOVE=30;//The minimum displacement (sum of bounding box lengths during detection time) to be detected as a the stylus moving 

}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetAcquisitionRate(double aquisitionRate)
{
  if(aquisitionRate<=0)
  {
    LOG_ERROR("Specified acquisition rate is not valid");
  }
  this->AcquisitionRate=aquisitionRate;

  this->WindowTimeSec=WINDOW_SIZE/this->AcquisitionRate; //If not specified window time will span by default WINDOW_SIZE=3 of acquisition points
  this->WindowSize=PlusMath::Round(this->AcquisitionRate*this->WindowTimeSec);
  if(this->WindowSize<1)
  {
    LOG_WARNING("The smallest window size is set to 1");
    this->WindowSize=1;
  }
  LOG_INFO("SET AcquisitionRate = "<< AcquisitionRate << "[fps] WindowTimeSec = " << WindowTimeSec<<"[s] DetectionTimeSec = "<< DetectionTimeSec <<"[s]");
  LOG_INFO("NumberOfWindows = "<< NumberOfWindows<< " WindowSize = "<< WindowSize<< " MinimunDistanceBetweenLandmarksMM = "<< MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << LandmarkThresholdMm <<"[mm]");
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetDetectionTimeSec(double detectionTime)
{
  if(detectionTime>this->WindowTimeSec&&detectionTime<MAXIMUM_WINDOW_TIME_SEC*NUMBER_WINDOWS)
  {
    this->DetectionTimeSec=detectionTime;
    this->NumberOfWindows=this->DetectionTimeSec/this->WindowTimeSec;
  }
  else
  {
    LOG_WARNING("Specified window time (" << detectionTime << " [s]) is not correct, default "<<this->DetectionTimeSec<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetWindowTimeSec(double windowTime)
{
  if(this->AcquisitionRate<=0)
  {
    LOG_ERROR("There is no acquisition rate specified");
  }
  if(windowTime>0&& windowTime<=this->DetectionTimeSec)
  {
    this->WindowTimeSec=windowTime;
    this->WindowSize=PlusMath::Round(this->AcquisitionRate*this->WindowTimeSec);
    if(this->WindowSize<1)
    {
      LOG_WARNING("The smallest window size is set to 1");
      this->WindowSize=1;
    }
    this->NumberOfWindows=this->DetectionTimeSec/this->WindowTimeSec;
  }
  else
  {
    LOG_WARNING("Specified window time (" << windowTime << " [s]) is not correct, default "<<this->WindowTimeSec<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetStylusShaftThresholdMm(double aboveLandmarkThreshold)
{
  if(aboveLandmarkThreshold>0)
  {
    this->StylusShaftThresholdMm=aboveLandmarkThreshold;
  }
  else
  {
    LOG_WARNING("Specified landmark threshold (" << aboveLandmarkThreshold << " [mm]) is not correct, default "<<this->StylusShaftThresholdMm<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetLandmarkThresholdMm(double sameLandmarkThreshold)
{
  if(sameLandmarkThreshold>0)
  {
    this->LandmarkThresholdMm=sameLandmarkThreshold;
  }
  else
  {
    LOG_WARNING("Specified landmark threshold (" << sameLandmarkThreshold << " [mm]) is not correct, default "<<this->LandmarkThresholdMm<<" [s] is used instead");
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetExpectedLandmarksNumber(int expectedLandmarksNumber)
{
  if(expectedLandmarksNumber>0)
  {
    this->ExpectedLandmarksNumber=expectedLandmarksNumber;
  }
  else
  {
    LOG_WARNING("Specified expected number of landmarks (" << expectedLandmarksNumber << " is not correct, default "<<this->ExpectedLandmarksNumber<<" is used instead");
  }
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::vtkLandmarkDetectionAlgo()
{
  this->ReferenceCoordinateFrame = NULL;

  this->LandmarkPointsReference = NULL;
  vtkSmartPointer<vtkPoints> landmarkPointsReference = vtkSmartPointer<vtkPoints>::New();
  this->SetLandmarkPointsReference(landmarkPointsReference);

  this->NewLandmarkFound=false;

  this->AboveStylusTipAverage[0] = 0.0;
  this->AboveStylusTipAverage[1] = 0.0;
  this->AboveStylusTipAverage[2] = 0.0;
  this->AboveStylusTipAverage[3] = 0.0;

  this->PartialInsertedPoints=0;
  this->CurrentStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();

  this->NumberOfWindows=NUMBER_WINDOWS;
  this->WindowSize=WINDOW_SIZE;

  this->AcquisitionRate=20.0;
  this->WindowTimeSec=0.2;
  this->DetectionTimeSec=1.0;
  this->StylusShaftThresholdMm = ABOVE_LANDMARK_THRESHOLD_MM;
  this->LandmarkThresholdMm = LANDMARK_THRESHOLD_MM;
  this->ExpectedLandmarksNumber=EXPECTED_LANDMARKS_NUMBER;
  this->MinimunDistanceBetweenLandmarksMm=15.0;
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::~vtkLandmarkDetectionAlgo()
{
  this->RemoveAllDetectionPoints();
}

//-----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::RemoveAllDetectionPoints()
{
  this->StylusTipToReferenceTransformsList.clear();
}
PlusStatus vtkLandmarkDetectionAlgo::ResetDetection()
{
  this->NewLandmarkFound=false;
  LOG_INFO("Reset");
  this->AboveStylusTipAverage[0] = 0.0;
  this->AboveStylusTipAverage[1] = 0.0;
  this->AboveStylusTipAverage[2] = 0.0;
  this->AboveStylusTipAverage[3] = 0.0;

  this->PartialInsertedPoints=0;
  RemoveAllDetectionPoints();
  this->CurrentStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();
  this->LandmarkPointsReference->Reset();
  this->LandmarkPointsReference->Initialize();
  this->NumberOfWindowsFoundPerLandmark.clear();
  //double landmarkFound[3] = {0,0,0};
  //for(int id=0; id<this->LandmarkPointsReference->GetNumberOfPoints();id++)
  //{
  //  this->LandmarkPointsReference->GetPoint(id, landmarkFound);
  //  LOG_INFO("Landmark not deleted" /*<<NumberOfWindowsFoundPerLandmark[id]*/<< " ("<< landmarkFound[0]<< ", "<< landmarkFound[1]<< ", "<< landmarkFound[2]<< ")")
  //}
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertLandmark(double* stylusTipPosition)
{
  if(IsNewLandmarkPointPosition(stylusTipPosition)==-1)
  {
    NumberOfWindowsFoundPerLandmark.push_back(1.0);
    this->LandmarkPointsReference->InsertNextPoint(stylusTipPosition);
    this->NewLandmarkFound=true;
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::DeleteLastLandmark()
{
  if(this->LandmarkPointsReference->GetNumberOfPoints()>0)
  {
    double landmarkFound[3] = {0,0,0};
    this->LandmarkPointsReference->GetPoint(this->LandmarkPointsReference->GetNumberOfPoints()-1, landmarkFound);
    this->LandmarkPointsReference->GetData()->RemoveTuple(this->LandmarkPointsReference->GetNumberOfPoints()-1);
    this->NumberOfWindowsFoundPerLandmark[this->LandmarkPointsReference->GetNumberOfPoints()]=-1;
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::GetStylusTipPositionWindowAverage(double* landmarkPoint_Reference)
{
  double stylusPositionSum [3] ={0,0,0};
  std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsList.end();
  int i=0;
  do
  {
    i++;
    stylusTipToReferenceTransformIt--;
    stylusPositionSum[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
    stylusPositionSum[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
    stylusPositionSum[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
  }while(i<this->WindowSize/*||*stylusTipToReferenceTransformIt!=*LastStylusTipIterator*/);

  this->CurrentStylusTipIterator=stylusTipToReferenceTransformIt;
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator--;
  if(i==this->WindowSize)
  {
    landmarkPoint_Reference[0]=stylusPositionSum[0]/this->WindowSize;
    landmarkPoint_Reference[1]=stylusPositionSum[1]/this->WindowSize;
    landmarkPoint_Reference[2]=stylusPositionSum[2]/this->WindowSize;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::EraseLastPoints()
{
  int i=0;
  for (std::list<vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    if(i%this->WindowSize==0)
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
  for(int a=0; a<j/this->WindowSize;a++)
  {
    StylusTipWindowAverage_Reference_List.pop_front();
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform)
{
  //Point 10 cm above the stylus tip, if it moves(window change bigger than AboveLandmarkThresholdMm) while the tip is static (window change smaller than LandmarkThresholdMm then it is landmark point.
  float pointAboveStylusTip_Reference[4]={100,0,0,1};
  double stylusTipChange[3]={0,0,0};
  double aboveStylusTipChange[3]={0,0,0};
  stylusTipToReferenceTransform->MultiplyPoint(pointAboveStylusTip_Reference, pointAboveStylusTip_Reference);

  AboveStylusTipAverage[0]+=pointAboveStylusTip_Reference[0];
  AboveStylusTipAverage[1]+=pointAboveStylusTip_Reference[1];
  AboveStylusTipAverage[2]+=pointAboveStylusTip_Reference[2];
  AboveStylusTipAverage[3]+=pointAboveStylusTip_Reference[3];

  this->StylusTipToReferenceTransformsList.push_back(stylusTipToReferenceTransform);
  this->PartialInsertedPoints++;
  if(this->PartialInsertedPoints>=this->WindowSize)
  {
    if(StylusTipWindowAverage_Reference_List.size()<1)
    {
      this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();
    }
    double stylusTipWindowAverage[4]={0,0,0,1};
    if (GetStylusTipPositionWindowAverage( stylusTipWindowAverage)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    std::vector<double> landmarkPointReference (stylusTipWindowAverage, stylusTipWindowAverage+sizeof(stylusTipWindowAverage)/sizeof(stylusTipWindowAverage[0]));
    StylusTipWindowAverage_Reference_List.push_back(landmarkPointReference);

    this->AboveStylusTipAverage[0]=this->AboveStylusTipAverage[0]/PartialInsertedPoints;
    this->AboveStylusTipAverage[1]=this->AboveStylusTipAverage[1]/PartialInsertedPoints;
    this->AboveStylusTipAverage[2]=this->AboveStylusTipAverage[2]/PartialInsertedPoints;
    this->AboveStylusTipAverage[3]=this->AboveStylusTipAverage[3]/PartialInsertedPoints;

    if(StylusTipWindowAverage_Reference_List.size()>1)
    {
      std::list< std::vector<double> >::iterator pointIt=StylusTipWindowAverage_Reference_List.end();
      --pointIt;--pointIt;

      double lastStylusTipWindowAverage[4] = {(*pointIt)[0],(*pointIt)[1],(*pointIt)[2],0};

      stylusTipChange[0]=lastStylusTipWindowAverage[0]-stylusTipWindowAverage[0];
      stylusTipChange[1]=lastStylusTipWindowAverage[1]-stylusTipWindowAverage[1];
      stylusTipChange[2]=lastStylusTipWindowAverage[2]-stylusTipWindowAverage[2];

      aboveStylusTipChange[0]=LastAboveStylusTipAverage[0]-AboveStylusTipAverage[0];
      aboveStylusTipChange[1]=LastAboveStylusTipAverage[1]-AboveStylusTipAverage[1];
      aboveStylusTipChange[2]=LastAboveStylusTipAverage[2]-AboveStylusTipAverage[2];
      this->BoundingBox.AddPoint(AboveStylusTipAverage);
      if(vtkMath::Norm(stylusTipChange)<this->LandmarkThresholdMm /*&& vtkMath::Norm(aboveStylusTipChange)>this->AboveLandmarkThresholdMm*/ )
      {
        LOG_DEBUG("\nDif last points (" <<abs(lastStylusTipWindowAverage[0]-stylusTipWindowAverage[0])<< ", "<<abs(lastStylusTipWindowAverage[1]-stylusTipWindowAverage[1])<< ", "<<abs(lastStylusTipWindowAverage[2]-stylusTipWindowAverage[2])<< ")\n");
        LOG_DEBUG("Window Landmark ("<< lastStylusTipWindowAverage[0]<< ", "<< lastStylusTipWindowAverage[1]<< ", "<< lastStylusTipWindowAverage[2]<< ") found keep going!!");
        int i =0;
        for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
          markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
        {
          if(i%this->WindowSize==0)
          {
            LOG_DEBUG( "\n");
          }
          LOG_DEBUG("\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<")");
          i++;
        }
      }
      else
      {
        LOG_DEBUG("\nDif last points (" <<abs(lastStylusTipWindowAverage[0]-stylusTipWindowAverage[0])<< ", "<<abs(lastStylusTipWindowAverage[1]-stylusTipWindowAverage[1])<< ", "<<abs(lastStylusTipWindowAverage[2]-stylusTipWindowAverage[2])<< ")\n");
        EraseLastPoints();
        while(StylusTipWindowAverage_Reference_List.size()>1)
        {
          StylusTipWindowAverage_Reference_List.pop_front();
        }
        this->BoundingBox.Reset();
        this->BoundingBox.AddPoint(AboveStylusTipAverage);
      }
    }
    this->LastAboveStylusTipAverage[0]=this->AboveStylusTipAverage[0];
    this->LastAboveStylusTipAverage[1]=this->AboveStylusTipAverage[1];
    this->LastAboveStylusTipAverage[2]=this->AboveStylusTipAverage[2];
    this->LastAboveStylusTipAverage[3]=this->AboveStylusTipAverage[3];

    this->AboveStylusTipAverage[0] = 0.0;
    this->AboveStylusTipAverage[1] = 0.0;
    this->AboveStylusTipAverage[2] = 0.0;
    this->AboveStylusTipAverage[3] = 0.0;

    this->PartialInsertedPoints=0;
    double lengths[3];
    this->BoundingBox.GetLengths(lengths);
    if(StylusTipWindowAverage_Reference_List.size()>=this->NumberOfWindows  )
    {
      this->BoundingBox.Reset();
      if((lengths[0]+lengths[1]+lengths[2])>MIN_LENGTH_ABOVE)
      {
        EstimateLandmarkPointPosition();
      }
      else
      {
        this->BoundingBox.AddPoint(this->LastAboveStylusTipAverage);
        EraseLastPoints();
      }
    }
  }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkLandmarkDetectionAlgo::IsNewLandmarkPointPosition(double* stylusTipPosition)
{
  double landmarkFound[3] = {0,0,0};
  double landmarkDifference[3] = {0,0,0};

  for(int id=0; id<this->LandmarkPointsReference->GetNumberOfPoints();id++)
  {
    this->LandmarkPointsReference->GetPoint(id, landmarkFound);

    landmarkDifference[0]=landmarkFound[0]-stylusTipPosition[0];
    landmarkDifference[1]=landmarkFound[1]-stylusTipPosition[1];
    landmarkDifference[2]=landmarkFound[2]-stylusTipPosition[2];
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
    if(vtkMath::Norm(landmarkDifference)<this->MinimunDistanceBetweenLandmarksMm/3)
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
  double stylusPositionMean[3]={0,0,0};
  double stylusPositionStdev[3]={0,0,0};

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
    if(i>=(this->WindowSize*NUMBER_WINDOWS_SKIP) && i <(this->NumberOfWindows)*this->WindowSize)
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

  if(j==(this->NumberOfWindows-NUMBER_WINDOWS_SKIP)*this->WindowSize)
  {
    for ( vtkIdType r = 0; r < outputPrimary->GetNumberOfRows(); ++ r )
    {
        stylusPositionMean[r]=outputPrimary->GetValueByName( r, "Mean" ).ToDouble();
        stylusPositionStdev[r]=outputDerived->GetValueByName( r, "Standard Deviation" ).ToDouble();
    }
    if(IsNewLandmarkPointPosition(stylusPositionMean)==-1)
    {
      NumberOfWindowsFoundPerLandmark.push_back(1.0);
      this->LandmarkPointsReference->InsertNextPoint(stylusPositionMean);
      LOG_DEBUG("\nSTD deviation ( " << stylusPositionStdev[0]<< ", "<< stylusPositionStdev[1]<< ", "<< stylusPositionStdev[2]<< ") " );
      LOG_DEBUG("STD deviation magnitude " << vtkMath::Norm(stylusPositionStdev));
      this->NewLandmarkFound=true;
    }
    RemoveAllDetectionPoints();
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
  if(this->LandmarkPointsReference->GetNumberOfPoints()>=this->ExpectedLandmarksNumber)
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
  if (this->LandmarkPointsReference->GetNumberOfPoints()>0)
  {
    std::ostrstream s;
    double landmarkFound[3] = {0,0,0};
    s << std::fixed << std::setprecision(aPrecision);
    for (int id =0; id<this->LandmarkPointsReference->GetNumberOfPoints();id++)
    {
      this->LandmarkPointsReference->GetPoint(id, landmarkFound);
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

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ExpectedLandmarksNumber, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, WindowTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DetectionTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusShaftThresholdMm, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, LandmarkThresholdMm, PhantomLandmarkLandmarkDetectionElement);

  LOG_INFO("AcquisitionRate = "<< AcquisitionRate << "[fps] WindowTimeSec = " << WindowTimeSec<<"[s] DetectionTimeSec = "<< DetectionTimeSec <<"[s]");
  LOG_INFO("NumberOfWindows = "<< NumberOfWindows<< " WindowSize = "<< WindowSize<< " MinimunDistanceBetweenLandmarksMm = "<< MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << LandmarkThresholdMm <<"[mm]");

  return PLUS_SUCCESS;
}



