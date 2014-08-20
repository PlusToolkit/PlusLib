/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"

#include "vtkPivotDetectionAlgo.h"
#include "vtkTransformRepository.h"
#include "PlusMath.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"

vtkCxxRevisionMacro(vtkPivotDetectionAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPivotDetectionAlgo);

//-----------------------------------------------------------------------------
// Default algorithm parameters
namespace
{
  const int NUMBER_WINDOWS=6;//The number of windows to be detected as pivot in DetectionTime (2 [s])DetectionTime/WindowTime
  const int WINDOW_SIZE=5;//The number of acquisitions in WindowTime (1/3 [s]) AcquisitonRate*WindowTime
  const double ABOVE_PIVOT_THRESHOLD_MM=3.0;//Above the pivot threshold is used to detect stylus pivoting and not static. When a point 10 cm above the stylus tip magnitude change is bigger than AbovePivotThresholdMM, the stylus is pivoting.
  const double PIVOT_THRESHOLD_MM=1.8;// A pivot position will be consider when the stylus tip position magnitude change is below PivotThresholdMM.
  const double MAXIMUM_WINDOW_TIME_SEC=3.0;
  const int EXPECTED_PIVOTS_NUMBER=3;// The default expected number of pivots to be detected 
}

void vtkPivotDetectionAlgo::SetAcquisitionRate(double aquisitionRate)
{
  if(aquisitionRate<=0)
  {
    LOG_ERROR("Specified acquisition rate is not valid");
  }
  this->AcquisitionRate=aquisitionRate;
  this->WindowSize=PlusMath::Round(this->AcquisitionRate*this->WindowTimeSec);
}

void vtkPivotDetectionAlgo::SetWindowTimeSec(double windowTime)
{
  if(this->AcquisitionRate<=0)
  {
    LOG_ERROR("There is no acquisition rate specified");
  }
  if(windowTime>0&& windowTime<MAXIMUM_WINDOW_TIME_SEC)
  {
    this->WindowTimeSec=windowTime;
    this->WindowSize=PlusMath::Round(this->AcquisitionRate*this->WindowTimeSec);
    this->NumberOfWindows=this->DetectionTimeSec/this->WindowTimeSec;
  }
  else
  {
    LOG_WARNING("Specified window time (" << windowTime << " [s]) is not correct, default "<<this->WindowTimeSec<<" [s] is used instead");
  }
}

void vtkPivotDetectionAlgo::SetDetectionTimeSec(double detectionTime)
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

void vtkPivotDetectionAlgo::SetAbovePivotThresholdMM(double abovePivotThreshold)
{
  if(abovePivotThreshold>0)
  {
    this->AbovePivotThresholdMM=abovePivotThreshold;
  }
  else
  {
    LOG_WARNING("Specified pivot threshold (" << abovePivotThreshold << " [mm]) is not correct, default "<<this->AbovePivotThresholdMM<<" [s] is used instead");
  }
}

void vtkPivotDetectionAlgo::SetPivotThresholdMM(double samePivotThreshold)
{
  if(samePivotThreshold>0)
  {
    this->PivotThresholdMM=samePivotThreshold;
  }
  else
  {
    LOG_WARNING("Specified same pivot threshold (" << samePivotThreshold << " [mm]) is not correct, default "<<this->PivotThresholdMM<<" [s] is used instead");
  }
}

void vtkPivotDetectionAlgo::SetExpectedPivotsNumber(int expectedPivotsNumber)
{
  if(expectedPivotsNumber>0)
  {
    this->ExpectedPivotsNumber=expectedPivotsNumber;
  }
  else
  {
    LOG_WARNING("Specified expected number of pivots (" << expectedPivotsNumber << " is not correct, default "<<this->ExpectedPivotsNumber<<" is used instead");
  }
}


//-----------------------------------------------------------------------------
vtkPivotDetectionAlgo::vtkPivotDetectionAlgo()
{
  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  //this->ObjectPivotPointCoordinateFrame = NULL;

  this->PivotPointsReference = NULL;
  vtkSmartPointer<vtkPoints> pivotPointsReference = vtkSmartPointer<vtkPoints>::New();
  this->SetPivotPointsReference(pivotPointsReference);

  this->PivotDetected=false;

  this->AboveStylusTipAverage[0] = 0.0;
  this->AboveStylusTipAverage[1] = 0.0;
  this->AboveStylusTipAverage[2] = 0.0;
  this->AboveStylusTipAverage[3] = 0.0;

  this->PartialInsertedPoints=0;
  this->CurrentStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  this->LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();;

  this->NumberOfWindows=NUMBER_WINDOWS;
  this->WindowSize=WINDOW_SIZE;

  this->AcquisitionRate=0.0;
  this->WindowTimeSec=1/3.0;
  this->DetectionTimeSec=2.0;
  this->AbovePivotThresholdMM = ABOVE_PIVOT_THRESHOLD_MM;
  this->PivotThresholdMM = PIVOT_THRESHOLD_MM;
  this->ExpectedPivotsNumber=EXPECTED_PIVOTS_NUMBER;
}

//-----------------------------------------------------------------------------
vtkPivotDetectionAlgo::~vtkPivotDetectionAlgo()
{
  this->RemoveAllDetectionPoints();
}

//-----------------------------------------------------------------------------
void vtkPivotDetectionAlgo::RemoveAllDetectionPoints()
{
  //for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator it=this->StylusTipToReferenceTransformsList.begin();
  //  it!=this->StylusTipToReferenceTransformsList.end(); ++it)
  //{
  //  (*it)->Delete();
  //}
  this->StylusTipToReferenceTransformsList.clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::GetStylusTipPositionWindowAverage(double* pivotPoint_Reference)
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
    pivotPoint_Reference[0]=stylusPositionSum[0]/this->WindowSize;
    pivotPoint_Reference[1]=stylusPositionSum[1]/this->WindowSize;
    pivotPoint_Reference[2]=stylusPositionSum[2]/this->WindowSize;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPivotDetectionAlgo::EraseLastPoints()
{
  int i=0;
  for (std::list<vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    if(i==this->WindowSize)
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
  while ( markerToReferenceTransformIt!=this->CurrentStylusTipIterator)
  {
    (*markerToReferenceTransformIt)->Delete();
    this->StylusTipToReferenceTransformsList.pop_front();
    markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
  }

  for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    LOG_DEBUG( "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", "<< -(*markerToReferenceTransformIt)->Element[1][3]<<", "<< -(*markerToReferenceTransformIt)->Element[2][3]<<") ");
  }

  StylusTipWindowAverage_Reference_List.pop_front();
}

//----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform)
{
  //Point 10 cm above the stylus tip, if it moves(window change bigger than AbovePivotThresholdMM) while the tip is static (window change smaller than PivotThresholdMM then it is pivot point.
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
    std::vector<double> pivotPointReference (stylusTipWindowAverage, stylusTipWindowAverage+sizeof(stylusTipWindowAverage)/sizeof(stylusTipWindowAverage[0]));
    StylusTipWindowAverage_Reference_List.push_back(pivotPointReference);

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

      if(vtkMath::Norm(stylusTipChange)<this->PivotThresholdMM && vtkMath::Norm(aboveStylusTipChange)>this->AbovePivotThresholdMM)
      {
        LOG_DEBUG("\nDif last points (" <<abs(lastStylusTipWindowAverage[0]-stylusTipWindowAverage[0])<< ", "<<abs(lastStylusTipWindowAverage[1]-stylusTipWindowAverage[1])<< ", "<<abs(lastStylusTipWindowAverage[2]-stylusTipWindowAverage[2])<< ")\n");
        LOG_INFO("Window Pivot ("<< lastStylusTipWindowAverage[0]<< ", "<< lastStylusTipWindowAverage[1]<< ", "<< lastStylusTipWindowAverage[2]<< ") found keep going!!");
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

    if(StylusTipWindowAverage_Reference_List.size()>=this->NumberOfWindows)
    {
      EstimatePivotPointPosition();
    }
  }

  return PLUS_SUCCESS; 
}


bool vtkPivotDetectionAlgo::IsNewPivotPointPosition(double* stylusTipPosition)
{
  double pivotFound[3] = {0,0,0};
  double pivotDifference[3] = {0,0,0};

  for(int id=0; id<this->PivotPointsReference->GetNumberOfPoints();id++)
  {
    this->PivotPointsReference->GetPoint(id, pivotFound);

    pivotDifference[0]=pivotFound[0]-stylusTipPosition[0];
    pivotDifference[1]=pivotFound[1]-stylusTipPosition[1];
    pivotDifference[2]=pivotFound[2]-stylusTipPosition[2];
    if(vtkMath::Norm(pivotDifference)<this->PivotThresholdMM )
    {
      pivotFound[0]=(pivotFound[0]+stylusTipPosition[0])/2.0;
      pivotFound[1]=(pivotFound[1]+stylusTipPosition[1])/2.0;
      pivotFound[2]=(pivotFound[2]+stylusTipPosition[2])/2.0;
      //Only using the first detection time pivot detection and not the average might be more accurate
      this->PivotPointsReference->InsertPoint(id,pivotFound);
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::EstimatePivotPointPosition()
{
  double stylusPositionSum[3] ={0,0,0};int i=0; int j=0;
  for (std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    stylusTipToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++stylusTipToReferenceTransformIt)
  {
    //Only the middle windows
    if(i>=this->WindowSize && i <(this->NumberOfWindows-1)*this->WindowSize)
    {
      stylusPositionSum[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
      stylusPositionSum[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
      stylusPositionSum[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
      j++;
    }

    i++;
  }
  if(j==(this->NumberOfWindows-2)*this->WindowSize)
  {
    stylusPositionSum[0]=stylusPositionSum[0]/((this->NumberOfWindows-2)*this->WindowSize);
    stylusPositionSum[1]=stylusPositionSum[1]/((this->NumberOfWindows-2)*this->WindowSize);
    stylusPositionSum[2]=stylusPositionSum[2]/((this->NumberOfWindows-2)*this->WindowSize);
    if(IsNewPivotPointPosition(stylusPositionSum)==true)
    {
      this->PivotPointsReference->InsertNextPoint(stylusPositionSum);
    }
    RemoveAllDetectionPoints();
    this->PivotDetected=true;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::IsNewPivotPointFound(bool &found)
{
  found=this->PivotDetected;
  if (this->PivotDetected==true)
  {
    this->PivotDetected=false;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::IsPivotDetectionCompleted(bool &completed)
{
  if(this->PivotPointsReference->GetNumberOfPoints()>=this->ExpectedPivotsNumber)
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
std::string vtkPivotDetectionAlgo::GetDetectedPivotsString( double aPrecision/*=3*/ )
{
  if (this->PivotPointsReference->GetNumberOfPoints()>0)
  {
    std::ostrstream s;
    double pivotFound[3] = {0,0,0};
    s << std::fixed << std::setprecision(aPrecision);
    for (int id =0; id<this->PivotPointsReference->GetNumberOfPoints();id++)
    {
      this->PivotPointsReference->GetPoint(id, pivotFound);
      s <<"\nPivot "<< id << " found (" << pivotFound[0]<<", " << pivotFound[1]<<", " << pivotFound[2]<<")";
    }
    s << std::ends;  
    return s.str();
  }
  else
  {
    return "\nNo pivots found";
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(pivotDetectionElement, aConfig, "vtkPivotDetectionAlgo");
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectMarkerCoordinateFrame, pivotDetectionElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, pivotDetectionElement);
  //XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectPivotPointCoordinateFrame, pivotDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(double, AcquisitionRate, pivotDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ExpectedPivotsNumber, pivotDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DetectionTimeSec, pivotDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, WindowTimeSec, pivotDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, AbovePivotThresholdMM, pivotDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, PivotThresholdMM, pivotDetectionElement);
  return PLUS_SUCCESS;
}



