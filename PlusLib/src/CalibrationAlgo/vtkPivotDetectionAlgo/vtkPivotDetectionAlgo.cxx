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

const int NUMBER_WINDOWS=5;
const int WINDOW_SIZE=10;
const double PIVOT_THRESHOLD=6;

//-----------------------------------------------------------------------------
vtkPivotDetectionAlgo::vtkPivotDetectionAlgo()
{
  this->StylusTipToStylusTransformMatrix = NULL;
  this->CalibrationError = -1.0;
  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->ObjectPivotPointCoordinateFrame = NULL;

  PivotPointsReference = vtkSmartPointer<vtkPoints>::New();

  PartialInsertedPoints=0;
  CurrentStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();;
  //CurrentMarkerIterator=NULL;
  //this->PivotPointPosition_Reference[0] = 0.0;
  //this->PivotPointPosition_Reference[1] = 0.0;
  //this->PivotPointPosition_Reference[2] = 0.0;
  //this->PivotPointPosition_Reference[3] = 1.0;
}

//-----------------------------------------------------------------------------
vtkPivotDetectionAlgo::~vtkPivotDetectionAlgo()
{
  this->SetStylusTipToStylusTransformMatrix(NULL);
  this->RemoveAllCalibrationPoints();
}

//-----------------------------------------------------------------------------
void vtkPivotDetectionAlgo::RemoveAllCalibrationPoints()
{
  for (std::list< vtkMatrix4x4* >::iterator it=this->StylusTipToReferenceTransformsList.begin();
    it!=this->StylusTipToReferenceTransformsList.end(); ++it)
  {
    (*it)->Delete();
  }
  this->StylusTipToReferenceTransformsList.clear();
  this->OutlierIndices.clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::GetPartialStylusPositionAverage(double* pivotPoint_Reference)
{
  double stylusPositionSum [3] ={0,0,0};

  std::list< vtkMatrix4x4* >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsList.end();
  int i=0;
  do
  {    
    i++;
    stylusTipToReferenceTransformIt--;

    stylusPositionSum[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
    stylusPositionSum[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
    stylusPositionSum[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
  }while(i<WINDOW_SIZE/*||*stylusTipToReferenceTransformIt!=*LastStylusTipIterator*/);

  CurrentStylusTipIterator=stylusTipToReferenceTransformIt;
  LastStylusTipIterator=this->StylusTipToReferenceTransformsList.end();
  LastStylusTipIterator--;
  if(i==WINDOW_SIZE)
  {
    pivotPoint_Reference[0]=stylusPositionSum[0]/WINDOW_SIZE;
    pivotPoint_Reference[1]=stylusPositionSum[1]/WINDOW_SIZE;
    pivotPoint_Reference[2]=stylusPositionSum[2]/WINDOW_SIZE;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPivotDetectionAlgo::EraseLastPoints()
{
  int i=0;
  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    if(i==WINDOW_SIZE)
      std::cout << "\n";
    std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
    i++;
  }

  std::cout << "\n Borrar antes de P( " << -(*CurrentStylusTipIterator)->Element[0][3]<<", "<< -(*CurrentStylusTipIterator)->Element[1][3]<<", "<< -(*CurrentStylusTipIterator)->Element[2][3]<<") \n";
  CurrentStylusTipIterator++;
  std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
  CurrentStylusTipIterator--;
  while ( markerToReferenceTransformIt!=CurrentStylusTipIterator)
  {
    (*markerToReferenceTransformIt)->Delete();
    this->StylusTipToReferenceTransformsList.pop_front();
    markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
  }

  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
  {
    std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", "<< -(*markerToReferenceTransformIt)->Element[1][3]<<", "<< -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
  }

  PivotPointPosition_Reference_List.pop_front();
}
//----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::InsertNextCalibrationPoint(vtkMatrix4x4* StylusToReferenceTransform)
{
  vtkMatrix4x4* stylusTipToReferenceTransformMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(StylusToReferenceTransform,StylusTipToStylusTransformMatrix,stylusTipToReferenceTransformMatrix);
  this->StylusTipToReferenceTransformsList.push_back(stylusTipToReferenceTransformMatrix);
  PartialInsertedPoints++;
  if(PartialInsertedPoints>=WINDOW_SIZE)
  {
    if(PivotPointPosition_Reference_List.size()<1)
    {
      LastStylusTipIterator=this->StylusTipToReferenceTransformsList.begin();
    }
    double pivotReference[4]={0,0,0,1};
    if (GetPartialStylusPositionAverage( pivotReference)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    std::vector<double> pivotPointReference (pivotReference, pivotReference+sizeof(pivotReference)/sizeof(pivotReference[0]));
    PivotPointPosition_Reference_List.push_back(pivotPointReference);
    if(PivotPointPosition_Reference_List.size()>1)
    {
      std::list< std::vector<double> >::iterator pointIt=PivotPointPosition_Reference_List.end();
      --pointIt;--pointIt;

      double pivotRefLast[4] = {(*pointIt)[0],(*pointIt)[1],(*pointIt)[2],0};

      if(abs(pivotRefLast[0]-pivotReference[0])<PIVOT_THRESHOLD && abs(pivotRefLast[1]-pivotReference[1])<PIVOT_THRESHOLD && abs(pivotRefLast[2]-pivotReference[2])<PIVOT_THRESHOLD)
      {
        std::cout << "\nDif last points (" <<abs(pivotRefLast[0]-pivotReference[0])<< ", "<<abs(pivotRefLast[1]-pivotReference[1])<< ", "<<abs(pivotRefLast[2]-pivotReference[2])<< ")\n";
        LOG_INFO("Pivot found keep going!!");
        int i =0;
        for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
          markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
        {
          if(i%WINDOW_SIZE==0)
            std::cout << "\n";
          std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
          i++;
        }
      }
      else
      {
        std::cout << "\nDif last points (" <<abs(pivotRefLast[0]-pivotReference[0])<< ", "<<abs(pivotRefLast[1]-pivotReference[1])<< ", "<<abs(pivotRefLast[2]-pivotReference[2])<< ")\n";
        EraseLastPoints();
      }
    }
    PartialInsertedPoints=0;

    if(PivotPointPosition_Reference_List.size()>=NUMBER_WINDOWS)
    {
      EstimatePivotPointPosition();
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
/*
Estimate pivot point position from the stylus tip points. The average of NUMBER_WINDOWS*WINDOW_SIZE consecutive points that remain 
in the same position within the PIVOT_THRESHOLD.
*/
PlusStatus vtkPivotDetectionAlgo::EstimatePivotPointPosition()
{
  double stylusPositionSum[3] ={0,0,0};int i=0;
  for (std::list< vtkMatrix4x4* >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
    stylusTipToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++stylusTipToReferenceTransformIt)
  {
    stylusPositionSum[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
    stylusPositionSum[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
    stylusPositionSum[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
    i++;
  }

  if(i==NUMBER_WINDOWS*WINDOW_SIZE)
  {
    PivotPointsReference->InsertNextPoint(stylusPositionSum[0]/(NUMBER_WINDOWS*WINDOW_SIZE), stylusPositionSum[1]/(NUMBER_WINDOWS*WINDOW_SIZE), stylusPositionSum[2]/(NUMBER_WINDOWS*WINDOW_SIZE));
  }

  //pivotPoint_Stylus[0]=xVector[3];
  //pivotPoint_Stylus[1]=xVector[4];
  //pivotPoint_Stylus[2]=xVector[5];
  return PLUS_SUCCESS;
}

////----------------------------------------------------------------------------
//PlusStatus vtkPivotDetectionAlgo::DoPivotCalibration(vtkTransformRepository* aTransformRepository/* = NULL*/)
//{
//  if (this->StylusTipToReferenceTransformsList.empty())
//  {
//    LOG_ERROR("No points are available for pivot calibration"); 
//    return PLUS_FAIL;
//  }
//  else
//  {
//    std::cout <<"\nSize of marker list " << this->StylusTipToReferenceTransformsList.size();
//    int i =0;
//    for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
//      markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt)
//    {
//      if(i%POINTS_DELTA==0)
//        std::cout << "\n";
//      std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
//      i++;
//    }
//
//    std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.end();
//    markerToReferenceTransformIt--;
//    int extraPoints = this->StylusTipToReferenceTransformsList.size()-PivotPointPosition_Reference_List.size()*POINTS_DELTA;
//    std::cout << "\n Delete extrapoints " << extraPoints;
//    while ( extraPoints>0)
//    {
//      std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
//      (*markerToReferenceTransformIt)->Delete();
//      this->StylusTipToReferenceTransformsList.pop_back();
//      markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.end();
//      markerToReferenceTransformIt--;
//      extraPoints--;
//    }
//    std::cout <<"\nResize of marker list without extra" << this->StylusTipToReferenceTransformsList.size();
//    std::cout <<"\n Size of tip points found " <<PivotPointPosition_Reference_List.size();
//    std::list< std::vector<double> >::iterator pointIt=PivotPointPosition_Reference_List.end();
//    for (std::list< std::vector<double> >::iterator pointIt=PivotPointPosition_Reference_List.begin(); pointIt!=PivotPointPosition_Reference_List.end(); ++pointIt)
//    {
//      std::cout << "\n P( " <<(*pointIt)[0]<<", " <<(*pointIt)[1]<<", " <<(*pointIt)[2]<<") ";
//    }
//  }
//
//  double pivotPoint_Marker[4]={0,0,0,1};
//  double pivotPoint_Reference[4]={0,0,0,1};
//  if (GetPivotPointPosition(pivotPoint_Marker, pivotPoint_Reference)!=PLUS_SUCCESS)
//  {
//    return PLUS_FAIL;
//  }    
//
//  // Get the result (tooltip to tool transform)
//  double x = pivotPoint_Marker[0];
//  double y = pivotPoint_Marker[1];
//  double z = pivotPoint_Marker[2];
//
//  vtkSmartPointer<vtkMatrix4x4> pivotPointToMarkerTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
//  pivotPointToMarkerTransformMatrix->SetElement(0,3,x);
//  pivotPointToMarkerTransformMatrix->SetElement(1,3,y);
//  pivotPointToMarkerTransformMatrix->SetElement(2,3,z);
//
//  // Compute tool orientation
//  // X axis: from the pivot point to the marker is on the X axis of the tool
//  double pivotPointToMarkerTransformX[3]={x,y,z};
//  vtkMath::Normalize(pivotPointToMarkerTransformX);
//  pivotPointToMarkerTransformMatrix->SetElement(0,0,pivotPointToMarkerTransformX[0]);
//  pivotPointToMarkerTransformMatrix->SetElement(1,0,pivotPointToMarkerTransformX[1]);
//  pivotPointToMarkerTransformMatrix->SetElement(2,0,pivotPointToMarkerTransformX[2]);
//
//  // Z axis: orthogonal to tool's X axis and the marker's Y axis
//  double pivotPointToMarkerTransformZ[3]={0,0,0};
//  // Use the unitY vector as pivotPointToMarkerTransformY vector, unless unitY is parallel to pivotPointToMarkerTransformX.
//  // If unitY is parallel to pivotPointToMarkerTransformX then use the unitZ vector as pivotPointToMarkerTransformY.
//
//  double unitY[3]={0,1,0};  
//  double angle = acos(vtkMath::Dot(pivotPointToMarkerTransformX,unitY));
//  // Normalize between -pi/2 .. +pi/2
//  if (angle>vtkMath::Pi()/2)
//  {
//    angle -= vtkMath::Pi();
//  }
//  else if (angle<-vtkMath::Pi()/2)
//  {
//    angle += vtkMath::Pi();
//  }
//  if (fabs(angle)*180.0/vtkMath::Pi()>20.0) 
//  {
//    // unitY is not parallel to pivotPointToMarkerTransformX
//    vtkMath::Cross(pivotPointToMarkerTransformX, unitY, pivotPointToMarkerTransformZ);
//    LOG_DEBUG("Use unitY");
//  }
//  else
//  {
//    // unitY is parallel to pivotPointToMarkerTransformX
//    // use the unitZ instead
//    double unitZ[3]={0,0,1};
//    vtkMath::Cross(pivotPointToMarkerTransformX, unitZ, pivotPointToMarkerTransformZ);    
//    LOG_DEBUG("Use unitZ");
//  }
//  vtkMath::Normalize(pivotPointToMarkerTransformZ);
//  pivotPointToMarkerTransformMatrix->SetElement(0,2,pivotPointToMarkerTransformZ[0]);
//  pivotPointToMarkerTransformMatrix->SetElement(1,2,pivotPointToMarkerTransformZ[1]);
//  pivotPointToMarkerTransformMatrix->SetElement(2,2,pivotPointToMarkerTransformZ[2]);
//
//  // Y axis: orthogonal to tool's Z axis and X axis
//  double pivotPointToMarkerTransformY[3]={0,0,0};
//  vtkMath::Cross(pivotPointToMarkerTransformZ, pivotPointToMarkerTransformX, pivotPointToMarkerTransformY);
//  vtkMath::Normalize(pivotPointToMarkerTransformY);
//  pivotPointToMarkerTransformMatrix->SetElement(0,1,pivotPointToMarkerTransformY[0]);
//  pivotPointToMarkerTransformMatrix->SetElement(1,1,pivotPointToMarkerTransformY[1]);
//  pivotPointToMarkerTransformMatrix->SetElement(2,1,pivotPointToMarkerTransformY[2]);
//
//  this->SetPivotPointToMarkerTransformMatrix(pivotPointToMarkerTransformMatrix);
//
//  this->PivotPointPosition_Reference[0]=pivotPoint_Reference[0];
//  this->PivotPointPosition_Reference[1]=pivotPoint_Reference[1];
//  this->PivotPointPosition_Reference[2]=pivotPoint_Reference[2];
//
//  ComputeCalibrationError();
//
//  // Save result
//  if (aTransformRepository)
//  {
//    PlusTransformName pivotPointToMarkerTransformName(this->ObjectPivotPointCoordinateFrame, this->ObjectMarkerCoordinateFrame);
//    aTransformRepository->SetTransform(pivotPointToMarkerTransformName, this->PivotPointToMarkerTransformMatrix);
//    aTransformRepository->SetTransformPersistent(pivotPointToMarkerTransformName, true);
//    aTransformRepository->SetTransformDate(pivotPointToMarkerTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());
//    aTransformRepository->SetTransformError(pivotPointToMarkerTransformName, this->CalibrationError);
//  }
//  else
//  {
//    LOG_INFO("Transform repository object is NULL, cannot save results into it");
//  }
//
//  return PLUS_SUCCESS;
//}

//-----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::GetPivot(double* pivotPointReference)
{
  if (PivotPointsReference->GetNumberOfPoints()>0)
  {
    PivotPointsReference->GetPoint(0, pivotPointReference);
    return PLUS_SUCCESS;
  }
  else
  {
    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------
std::string vtkPivotDetectionAlgo::GetStylusTipToStylusTranslationString( double aPrecision/*=3*/ )
{
  if (this->StylusTipToStylusTransformMatrix == NULL) {
    LOG_ERROR("Tooltip to tool transform is not initialized!");
    return "";
  }

  std::ostrstream s;
  s << std::fixed << std::setprecision(aPrecision)
    << this->StylusTipToStylusTransformMatrix->GetElement(0,3) 
    << " x " << this->StylusTipToStylusTransformMatrix->GetElement(1,3)
    << " x " << this->StylusTipToStylusTransformMatrix->GetElement(2,3)
    << std::ends;  

  return s.str();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPivotDetectionAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(pivotCalibrationElement, aConfig, "vtkPivotDetectionAlgo");
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectMarkerCoordinateFrame, pivotCalibrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, pivotCalibrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectPivotPointCoordinateFrame, pivotCalibrationElement);
  return PLUS_SUCCESS;
}

////-----------------------------------------------------------------------------
//void vtkPivotDetectionAlgo::ComputeCalibrationError()
//{
//  double* pivotPoint_Reference=this->PivotPointPosition_Reference;
//  
//  vtkSmartPointer<vtkMatrix4x4> pivotPointToReferenceMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
//
//  // Compute the error for each sample as distance between the mean pivot point position and the pivot point position computed from each sample
//  std::vector<double> errorValues;
//  double currentPivotPoint_Reference[4]={0,0,0,1};
//  unsigned int sampleIndex=0;
//  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsList.begin();
//    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsList.end(); ++markerToReferenceTransformIt, ++sampleIndex)
//  {
//    if (this->OutlierIndices.find(sampleIndex)!=this->OutlierIndices.end())
//    {
//      // outlier, so skip from the error computation
//      continue;
//    }
//
//    vtkMatrix4x4::Multiply4x4((*markerToReferenceTransformIt),this->PivotPointToMarkerTransformMatrix,pivotPointToReferenceMatrix);
//    for (int i=0; i<3; i++)
//    {
//      currentPivotPoint_Reference[i] = pivotPointToReferenceMatrix->Element[i][3];
//    }
//    double errorValue = sqrt(vtkMath::Distance2BetweenPoints(currentPivotPoint_Reference, pivotPoint_Reference));
//    errorValues.push_back(errorValue);
//  }
//
//  double mean=0;
//  double stdev=0;
//  PlusMath::ComputeMeanAndStdev(errorValues, mean, stdev);
//
//  this->CalibrationError = mean;
//}

////-----------------------------------------------------------------------------
//int vtkPivotDetectionAlgo::GetNumberOfDetectedOutliers()
//{
//  return this->OutlierIndices.size();
//}



