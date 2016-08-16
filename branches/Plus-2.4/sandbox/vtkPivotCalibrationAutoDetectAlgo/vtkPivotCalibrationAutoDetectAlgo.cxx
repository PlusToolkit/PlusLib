/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"

#include "vtkPivotCalibrationAutoDetectAlgo.h"
#include "vtkTransformRepository.h"
#include "PlusMath.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"

vtkCxxRevisionMacro(vtkPivotCalibrationAutoDetectAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPivotCalibrationAutoDetectAlgo);

const int POINTS_DELTA=15;
const double DELTA_PIVOT=6;

//-----------------------------------------------------------------------------
vtkPivotCalibrationAutoDetectAlgo::vtkPivotCalibrationAutoDetectAlgo()
{
  this->PivotPointToMarkerTransformMatrix = NULL;
  this->CalibrationError = -1.0;
  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->ObjectPivotPointCoordinateFrame = NULL;

  PartialInsertedPoints=0;
  CurrentMarkerIterator=this->MarkerToReferenceTransformMatrixArray.end();
  LastMarkerIterator=this->MarkerToReferenceTransformMatrixArray.begin();;
  //CurrentMarkerIterator=NULL;

  this->PivotPointPosition_Reference[0] = 0.0;
  this->PivotPointPosition_Reference[1] = 0.0;
  this->PivotPointPosition_Reference[2] = 0.0;
  this->PivotPointPosition_Reference[3] = 1.0;
}

//-----------------------------------------------------------------------------
vtkPivotCalibrationAutoDetectAlgo::~vtkPivotCalibrationAutoDetectAlgo()
{
  this->SetPivotPointToMarkerTransformMatrix(NULL);
  this->RemoveAllCalibrationPoints();
}

//-----------------------------------------------------------------------------
void vtkPivotCalibrationAutoDetectAlgo::RemoveAllCalibrationPoints()
{
  for (std::list< vtkMatrix4x4* >::iterator it=this->MarkerToReferenceTransformMatrixArray.begin();
    it!=this->MarkerToReferenceTransformMatrixArray.end(); ++it)
  {
    (*it)->Delete();
  }
  this->MarkerToReferenceTransformMatrixArray.clear();
  this->OutlierIndices.clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkPivotCalibrationAutoDetectAlgo::GetPartialPivotPointPosition(double* pivotPoint_Reference)
{
  std::vector<vnl_vector<double> > aMatrix; 
  std::vector<double> bVector;  
  vnl_vector<double> xVector(6,0); // result vector

  vnl_vector<double> aMatrixRow(6);

  std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.end();
  int i=0;
  do
  {    
    i++;
    markerToReferenceTransformIt--;
    for (int i=0; i<3; i++)
    {
      aMatrixRow(0)=(*markerToReferenceTransformIt)->Element[i][0];
      aMatrixRow(1)=(*markerToReferenceTransformIt)->Element[i][1];
      aMatrixRow(2)=(*markerToReferenceTransformIt)->Element[i][2];
      aMatrixRow(3)= (i==0?-1:0);
      aMatrixRow(4)= (i==1?-1:0);
      aMatrixRow(5)= (i==2?-1:0);
      aMatrix.push_back(aMatrixRow);
    }
    bVector.push_back(-(*markerToReferenceTransformIt)->Element[0][3]);
    bVector.push_back(-(*markerToReferenceTransformIt)->Element[1][3]);
    bVector.push_back(-(*markerToReferenceTransformIt)->Element[2][3]);
  }while(i<POINTS_DELTA||*markerToReferenceTransformIt!=*LastMarkerIterator);

  CurrentMarkerIterator=markerToReferenceTransformIt;
  LastMarkerIterator=this->MarkerToReferenceTransformMatrixArray.end();
  LastMarkerIterator--;
  
  double mean = 0;
  double stdev = 0;
  vnl_vector<unsigned int> notOutliersIndices;
  notOutliersIndices.clear();
  notOutliersIndices.set_size(bVector.size());
  for ( unsigned int i = 0; i < bVector.size(); ++i )
  {
    notOutliersIndices.put(i,i);
  }
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, xVector, &mean, &stdev, &notOutliersIndices) != PLUS_SUCCESS )
  {
    LOG_ERROR("vtkPivotCalibrationAutoDetectAlgo failed: LSQRMinimize error"); 
    return PLUS_FAIL;
  }

  // Note: Outliers are detected and rejected for each row (each coordinate axis). Although most frequently
  // an outlier sample's every component is an outlier, it may be possible that only certain components of an
  // outlier sample are removed, which may be desirable for some cases (e.g., when the point is an outlier because
  // temporarily it was flipped around one axis) and not desirable for others (when the point is completely randomly
  // corrupted), but there would be no measurable difference anyway if the only a few percent of the points are
  // outliers.

  this->OutlierIndices.clear();
  unsigned int processFromRowIndex=0;
  for (int i=0; i<notOutliersIndices.size(); i++)
  {
    unsigned int nextNotOutlierRowIndex=notOutliersIndices[i];
    if (nextNotOutlierRowIndex>processFromRowIndex)
    {
      // samples were missed, so they are outliers
      for (unsigned int outlierRowIndex=processFromRowIndex; outlierRowIndex<nextNotOutlierRowIndex; outlierRowIndex++)
      {
        int sampleIndex=outlierRowIndex/3;  // 3 rows are generated per sample
        this->OutlierIndices.insert(sampleIndex);
      }
    }
    processFromRowIndex=nextNotOutlierRowIndex+1;
  }

  pivotPoint_Reference[0]=xVector[3];
  pivotPoint_Reference[1]=xVector[4];
  pivotPoint_Reference[2]=xVector[5];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPivotCalibrationAutoDetectAlgo::EraseLastPoints()
{
  int i=0;
  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
    markerToReferenceTransformIt!=this->MarkerToReferenceTransformMatrixArray.end(); ++markerToReferenceTransformIt)
  {
    if(i==POINTS_DELTA)
      std::cout << "\n";
    std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
    i++;
  }

  std::cout << "\n Borrar antes de P( " << -(*CurrentMarkerIterator)->Element[0][3]<<", "<< -(*CurrentMarkerIterator)->Element[1][3]<<", "<< -(*CurrentMarkerIterator)->Element[2][3]<<") \n";
  CurrentMarkerIterator++;
  std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
  while ( markerToReferenceTransformIt!=CurrentMarkerIterator)
  {
    (*markerToReferenceTransformIt)->Delete();
    this->MarkerToReferenceTransformMatrixArray.pop_front();
    markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
  }

  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
    markerToReferenceTransformIt!=this->MarkerToReferenceTransformMatrixArray.end(); ++markerToReferenceTransformIt)
  {
    std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", "<< -(*markerToReferenceTransformIt)->Element[1][3]<<", "<< -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
  }

  PivotPointPosition_Reference_List.pop_front();

}
//----------------------------------------------------------------------------
PlusStatus vtkPivotCalibrationAutoDetectAlgo::InsertNextCalibrationPoint(vtkMatrix4x4* aMarkerToReferenceTransformMatrix)
{
  vtkMatrix4x4* markerToReferenceTransformMatrixCopy = vtkMatrix4x4::New();
  markerToReferenceTransformMatrixCopy->DeepCopy(aMarkerToReferenceTransformMatrix);

  if(PartialInsertedPoints>=POINTS_DELTA)
  {
    /*CurrentMarkerIterator=this->MarkerToReferenceTransformMatrixArray.end();*/
    if(PivotPointPosition_Reference_List.size()<1)
      LastMarkerIterator=this->MarkerToReferenceTransformMatrixArray.begin();
    double pivotReference[4];//={0,0,0,1};
    if (GetPartialPivotPointPosition( pivotReference)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    //std::vector<double> pivotPointReference { pivotReference[0],pivotReference[1],pivotReference[2] };
std::vector<double> pivotPointReference (pivotReference, pivotReference+sizeof(pivotReference)/sizeof(pivotReference[0]));
    PivotPointPosition_Reference_List.push_back(pivotPointReference);
    if(PivotPointPosition_Reference_List.size()>1)
    {
      std::list< std::vector<double> >::iterator pointIt=PivotPointPosition_Reference_List.end();
      --pointIt;--pointIt;

      double pivotRefLast[4] = {(*pointIt)[0],(*pointIt)[1],(*pointIt)[2],0};

      if(abs(pivotRefLast[0]-pivotReference[0])<DELTA_PIVOT && abs(pivotRefLast[1]-pivotReference[1])<DELTA_PIVOT && abs(pivotRefLast[2]-pivotReference[2])<DELTA_PIVOT)
      {
        std::cout << "\nDif last points (" <<abs(pivotRefLast[0]-pivotReference[0])<< ", "<<abs(pivotRefLast[1]-pivotReference[1])<< ", "<<abs(pivotRefLast[2]-pivotReference[2])<< ")\n";
        LOG_DEBUG("Pivot found keep going!!");
        int i =0;
        for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
          markerToReferenceTransformIt!=this->MarkerToReferenceTransformMatrixArray.end(); ++markerToReferenceTransformIt)
        {
          if(i%POINTS_DELTA==0)
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

  }

  
  this->MarkerToReferenceTransformMatrixArray.push_back(markerToReferenceTransformMatrixCopy);
  PartialInsertedPoints++;
  return PLUS_SUCCESS; 
}



//----------------------------------------------------------------------------
/*
In homogeneous coordinates:
 PivotPoint_Reference = MarkerToReferenceTransformMatrix * PivotPoint_Marker

MarkerToReferenceTransformMatrix decomosed to rotation matrix and translation vector:
 PivotPoint_Reference = MarkerToReferenceTransformRotationMatrix * PivotPoint_Marker + MarkerToReferenceTransformTranslationVector
rearranged:
 MarkerToReferenceTransformRotationMatrix * PivotPoint_Marker - PivotPoint_Reference = -MarkerToReferenceTransformTranslationVector
in a matrix form:
 [ MarkerToReferenceTransformRotationMatrix | -Identity3x3 ] * [ PivotPoint_Marker    ] = [ -MarkerToReferenceTransformTranslationVector ]
                                                               [ PivotPoint_Reference ]

It's an Ax=b linear problem that can be solved with robust LSQR:
 Ai = [ MarkerToReferenceTransformRotationMatrix | -Identity3x3 ]
 xi = [ PivotPoint_Marker    ]
      [ PivotPoint_Reference ]
 bi = [ -MarkerToReferenceTransformTranslationVector ]
*/
PlusStatus vtkPivotCalibrationAutoDetectAlgo::GetPivotPointPosition(double* pivotPoint_Marker, double* pivotPoint_Reference)
{
  std::vector<vnl_vector<double> > aMatrix; 
  std::vector<double> bVector;  
  vnl_vector<double> xVector(6,0); // result vector

  vnl_vector<double> aMatrixRow(6);
  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
    markerToReferenceTransformIt!=this->MarkerToReferenceTransformMatrixArray.end(); ++markerToReferenceTransformIt)
  {    
    for (int i=0; i<3; i++)
    {
      aMatrixRow(0)=(*markerToReferenceTransformIt)->Element[i][0];
      aMatrixRow(1)=(*markerToReferenceTransformIt)->Element[i][1];
      aMatrixRow(2)=(*markerToReferenceTransformIt)->Element[i][2];
      aMatrixRow(3)= (i==0?-1:0);
      aMatrixRow(4)= (i==1?-1:0);
      aMatrixRow(5)= (i==2?-1:0);
      aMatrix.push_back(aMatrixRow);
    }
    bVector.push_back(-(*markerToReferenceTransformIt)->Element[0][3]);
    bVector.push_back(-(*markerToReferenceTransformIt)->Element[1][3]);
    bVector.push_back(-(*markerToReferenceTransformIt)->Element[2][3]);
  }

  double mean = 0;
  double stdev = 0;
  vnl_vector<unsigned int> notOutliersIndices;
  notOutliersIndices.clear();
  notOutliersIndices.set_size(bVector.size());
  for ( unsigned int i = 0; i < bVector.size(); ++i )
  {
    notOutliersIndices.put(i,i);
  }
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, xVector, &mean, &stdev, &notOutliersIndices) != PLUS_SUCCESS )
  {
    LOG_ERROR("vtkPivotCalibrationAutoDetectAlgo failed: LSQRMinimize error"); 
    return PLUS_FAIL;
  }

  // Note: Outliers are detected and rejected for each row (each coordinate axis). Although most frequently
  // an outlier sample's every component is an outlier, it may be possible that only certain components of an
  // outlier sample are removed, which may be desirable for some cases (e.g., when the point is an outlier because
  // temporarily it was flipped around one axis) and not desirable for others (when the point is completely randomly
  // corrupted), but there would be no measurable difference anyway if the only a few percent of the points are
  // outliers.

  this->OutlierIndices.clear();
  unsigned int processFromRowIndex=0;
  for (int i=0; i<notOutliersIndices.size(); i++)
  {
    unsigned int nextNotOutlierRowIndex=notOutliersIndices[i];
    if (nextNotOutlierRowIndex>processFromRowIndex)
    {
      // samples were missed, so they are outliers
      for (unsigned int outlierRowIndex=processFromRowIndex; outlierRowIndex<nextNotOutlierRowIndex; outlierRowIndex++)
      {
        int sampleIndex=outlierRowIndex/3;  // 3 rows are generated per sample
        this->OutlierIndices.insert(sampleIndex);
      }
    }
    processFromRowIndex=nextNotOutlierRowIndex+1;
  }

  pivotPoint_Marker[0]=xVector[0];
  pivotPoint_Marker[1]=xVector[1];
  pivotPoint_Marker[2]=xVector[2];

  pivotPoint_Reference[0]=xVector[3];
  pivotPoint_Reference[1]=xVector[4];
  pivotPoint_Reference[2]=xVector[5];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPivotCalibrationAutoDetectAlgo::DoPivotCalibration(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  if (this->MarkerToReferenceTransformMatrixArray.empty())
  {
    LOG_ERROR("No points are available for pivot calibration"); 
    return PLUS_FAIL;
  }
  else
  {
    std::cout <<"\nSize of marker list " << this->MarkerToReferenceTransformMatrixArray.size();
    int i =0;
    for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
      markerToReferenceTransformIt!=this->MarkerToReferenceTransformMatrixArray.end(); ++markerToReferenceTransformIt)
    {
      if(i%POINTS_DELTA==0)
        std::cout << "\n";
      std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
      i++;
    }

    std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.end();
    markerToReferenceTransformIt--;
    int extraPoints = this->MarkerToReferenceTransformMatrixArray.size()-PivotPointPosition_Reference_List.size()*POINTS_DELTA;
    std::cout << "\n Delete extrapoints " << extraPoints;
    while ( extraPoints>0)
    {
      std::cout << "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<") ";
      (*markerToReferenceTransformIt)->Delete();
      this->MarkerToReferenceTransformMatrixArray.pop_back();
      markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.end();
      markerToReferenceTransformIt--;
      extraPoints--;
    }
    std::cout <<"\nResize of marker list without extra" << this->MarkerToReferenceTransformMatrixArray.size();
    std::cout <<"\n Size of tip points found " <<PivotPointPosition_Reference_List.size();
    std::list< std::vector<double> >::iterator pointIt=PivotPointPosition_Reference_List.end();
    for (std::list< std::vector<double> >::iterator pointIt=PivotPointPosition_Reference_List.begin(); pointIt!=PivotPointPosition_Reference_List.end(); ++pointIt)
    {
      std::cout << "\n P( " <<(*pointIt)[0]<<", " <<(*pointIt)[1]<<", " <<(*pointIt)[2]<<") ";
    }
  }

  double pivotPoint_Marker[4]={0,0,0,1};
  double pivotPoint_Reference[4]={0,0,0,1};
  if (GetPivotPointPosition(pivotPoint_Marker, pivotPoint_Reference)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }    

  // Get the result (tooltip to tool transform)
  double x = pivotPoint_Marker[0];
  double y = pivotPoint_Marker[1];
  double z = pivotPoint_Marker[2];

  vtkSmartPointer<vtkMatrix4x4> pivotPointToMarkerTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  pivotPointToMarkerTransformMatrix->SetElement(0,3,x);
  pivotPointToMarkerTransformMatrix->SetElement(1,3,y);
  pivotPointToMarkerTransformMatrix->SetElement(2,3,z);

  // Compute tool orientation
  // X axis: from the pivot point to the marker is on the X axis of the tool
  double pivotPointToMarkerTransformX[3]={x,y,z};
  vtkMath::Normalize(pivotPointToMarkerTransformX);
  pivotPointToMarkerTransformMatrix->SetElement(0,0,pivotPointToMarkerTransformX[0]);
  pivotPointToMarkerTransformMatrix->SetElement(1,0,pivotPointToMarkerTransformX[1]);
  pivotPointToMarkerTransformMatrix->SetElement(2,0,pivotPointToMarkerTransformX[2]);

  // Z axis: orthogonal to tool's X axis and the marker's Y axis
  double pivotPointToMarkerTransformZ[3]={0,0,0};
  // Use the unitY vector as pivotPointToMarkerTransformY vector, unless unitY is parallel to pivotPointToMarkerTransformX.
  // If unitY is parallel to pivotPointToMarkerTransformX then use the unitZ vector as pivotPointToMarkerTransformY.

  double unitY[3]={0,1,0};  
  double angle = acos(vtkMath::Dot(pivotPointToMarkerTransformX,unitY));
  // Normalize between -pi/2 .. +pi/2
  if (angle>vtkMath::Pi()/2)
  {
    angle -= vtkMath::Pi();
  }
  else if (angle<-vtkMath::Pi()/2)
  {
    angle += vtkMath::Pi();
  }
  if (fabs(angle)*180.0/vtkMath::Pi()>20.0) 
  {
    // unitY is not parallel to pivotPointToMarkerTransformX
    vtkMath::Cross(pivotPointToMarkerTransformX, unitY, pivotPointToMarkerTransformZ);
    LOG_DEBUG("Use unitY");
  }
  else
  {
    // unitY is parallel to pivotPointToMarkerTransformX
    // use the unitZ instead
    double unitZ[3]={0,0,1};
    vtkMath::Cross(pivotPointToMarkerTransformX, unitZ, pivotPointToMarkerTransformZ);    
    LOG_DEBUG("Use unitZ");
  }
  vtkMath::Normalize(pivotPointToMarkerTransformZ);
  pivotPointToMarkerTransformMatrix->SetElement(0,2,pivotPointToMarkerTransformZ[0]);
  pivotPointToMarkerTransformMatrix->SetElement(1,2,pivotPointToMarkerTransformZ[1]);
  pivotPointToMarkerTransformMatrix->SetElement(2,2,pivotPointToMarkerTransformZ[2]);

  // Y axis: orthogonal to tool's Z axis and X axis
  double pivotPointToMarkerTransformY[3]={0,0,0};
  vtkMath::Cross(pivotPointToMarkerTransformZ, pivotPointToMarkerTransformX, pivotPointToMarkerTransformY);
  vtkMath::Normalize(pivotPointToMarkerTransformY);
  pivotPointToMarkerTransformMatrix->SetElement(0,1,pivotPointToMarkerTransformY[0]);
  pivotPointToMarkerTransformMatrix->SetElement(1,1,pivotPointToMarkerTransformY[1]);
  pivotPointToMarkerTransformMatrix->SetElement(2,1,pivotPointToMarkerTransformY[2]);

  this->SetPivotPointToMarkerTransformMatrix(pivotPointToMarkerTransformMatrix);

  this->PivotPointPosition_Reference[0]=pivotPoint_Reference[0];
  this->PivotPointPosition_Reference[1]=pivotPoint_Reference[1];
  this->PivotPointPosition_Reference[2]=pivotPoint_Reference[2];

  ComputeCalibrationError();

  // Save result
  if (aTransformRepository)
  {
    PlusTransformName pivotPointToMarkerTransformName(this->ObjectPivotPointCoordinateFrame, this->ObjectMarkerCoordinateFrame);
    aTransformRepository->SetTransform(pivotPointToMarkerTransformName, this->PivotPointToMarkerTransformMatrix);
    aTransformRepository->SetTransformPersistent(pivotPointToMarkerTransformName, true);
    aTransformRepository->SetTransformDate(pivotPointToMarkerTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());
    aTransformRepository->SetTransformError(pivotPointToMarkerTransformName, this->CalibrationError);
  }
  else
  {
    LOG_INFO("Transform repository object is NULL, cannot save results into it");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkPivotCalibrationAutoDetectAlgo::GetPivotPointToMarkerTranslationString( double aPrecision/*=3*/ )
{
  if (this->PivotPointToMarkerTransformMatrix == NULL) {
    LOG_ERROR("Tooltip to tool transform is not initialized!");
    return "";
  }

  std::ostrstream s;
  s << std::fixed << std::setprecision(aPrecision)
    << this->PivotPointToMarkerTransformMatrix->GetElement(0,3) 
    << " x " << this->PivotPointToMarkerTransformMatrix->GetElement(1,3)
    << " x " << this->PivotPointToMarkerTransformMatrix->GetElement(2,3)
    << std::ends;  
    
  return s.str();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPivotCalibrationAutoDetectAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(pivotCalibrationElement, aConfig, "vtkPivotCalibrationAutoDetectAlgo");
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectMarkerCoordinateFrame, pivotCalibrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, pivotCalibrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ObjectPivotPointCoordinateFrame, pivotCalibrationElement);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPivotCalibrationAutoDetectAlgo::ComputeCalibrationError()
{
  double* pivotPoint_Reference=this->PivotPointPosition_Reference;
  
  vtkSmartPointer<vtkMatrix4x4> pivotPointToReferenceMatrix=vtkSmartPointer<vtkMatrix4x4>::New();

  // Compute the error for each sample as distance between the mean pivot point position and the pivot point position computed from each sample
  std::vector<double> errorValues;
  double currentPivotPoint_Reference[4]={0,0,0,1};
  unsigned int sampleIndex=0;
  for (std::list< vtkMatrix4x4* >::iterator markerToReferenceTransformIt=this->MarkerToReferenceTransformMatrixArray.begin();
    markerToReferenceTransformIt!=this->MarkerToReferenceTransformMatrixArray.end(); ++markerToReferenceTransformIt, ++sampleIndex)
  {
    if (this->OutlierIndices.find(sampleIndex)!=this->OutlierIndices.end())
    {
      // outlier, so skip from the error computation
      continue;
    }

    vtkMatrix4x4::Multiply4x4((*markerToReferenceTransformIt),this->PivotPointToMarkerTransformMatrix,pivotPointToReferenceMatrix);
    for (int i=0; i<3; i++)
    {
      currentPivotPoint_Reference[i] = pivotPointToReferenceMatrix->Element[i][3];
    }
    double errorValue = sqrt(vtkMath::Distance2BetweenPoints(currentPivotPoint_Reference, pivotPoint_Reference));
    errorValues.push_back(errorValue);
  }

  double mean=0;
  double stdev=0;
  PlusMath::ComputeMeanAndStdev(errorValues, mean, stdev);

  this->CalibrationError = mean;
}

//-----------------------------------------------------------------------------
int vtkPivotCalibrationAutoDetectAlgo::GetNumberOfDetectedOutliers()
{
  return this->OutlierIndices.size();
}
