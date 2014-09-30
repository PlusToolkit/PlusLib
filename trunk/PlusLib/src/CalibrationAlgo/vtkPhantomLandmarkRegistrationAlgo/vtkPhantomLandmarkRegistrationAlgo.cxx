/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPhantomLandmarkRegistrationAlgo.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkTransformRepository.h"
#include "vtkXMLUtilities.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPhantomLandmarkRegistrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPhantomLandmarkRegistrationAlgo);

std::string vtkPhantomLandmarkRegistrationAlgo::ConfigurationElementName = "vtkPhantomLandmarkRegistrationAlgo";

//-----------------------------------------------------------------------------

vtkPhantomLandmarkRegistrationAlgo::vtkPhantomLandmarkRegistrationAlgo()
{
  this->RegistrationError = -1.0;

  this->PhantomToReferenceTransformMatrix = NULL;
  this->DefinedLandmarks = NULL;
  this->RecordedLandmarks = NULL;
  this->PhantomCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->StylusTipCoordinateFrame = NULL;

  this->DefinedLandmarkNames.clear();

  vtkSmartPointer<vtkPoints> definedLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetDefinedLandmarks(definedLandmarks);

  vtkSmartPointer<vtkPoints> recordedLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetRecordedLandmarks(recordedLandmarks);
}

//-----------------------------------------------------------------------------

vtkPhantomLandmarkRegistrationAlgo::~vtkPhantomLandmarkRegistrationAlgo()
{
  this->SetPhantomToReferenceTransformMatrix(NULL);
  this->SetDefinedLandmarks(NULL);
  this->SetRecordedLandmarks(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkPhantomLandmarkRegistrationAlgo::Register(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkPhantomLandmarkRegistrationAlgo::Register"); 

  // Create input point vectors
  std::vector< itk::Point<double,3> > fixedPoints;
  std::vector< itk::Point<double,3> > movingPoints;

  for (int i=0; i<this->RecordedLandmarks->GetNumberOfPoints(); ++i)
  {
    // Defined landmarks from xml are in the phantom coordinate system
    double* fixedPointArray = this->DefinedLandmarks->GetPoint(i);
    itk::Point<double,3> fixedPoint(fixedPointArray);

    // Recorded landmarks are in the tracker coordinate system
    double* movingPointArray = this->RecordedLandmarks->GetPoint(i);
    itk::Point<double,3> movingPoint(movingPointArray);

    fixedPoints.push_back(fixedPoint);
    movingPoints.push_back(movingPoint);
  }

  for (int i=0; i<this->RecordedLandmarks->GetNumberOfPoints(); ++i)
  {
    LOG_DEBUG("Phantom point " << i << ": Defined: " << fixedPoints[i] << "  Recorded: " << movingPoints[i]);
  }

  // Initialize ITK transform
  itk::VersorRigid3DTransform<double>::Pointer transform = itk::VersorRigid3DTransform<double>::New();
  transform->SetIdentity();

  itk::LandmarkBasedTransformInitializer<itk::VersorRigid3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::Pointer initializer = itk::LandmarkBasedTransformInitializer<itk::VersorRigid3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::New();
  initializer->SetTransform(transform);
  initializer->SetFixedLandmarks(fixedPoints);
  initializer->SetMovingLandmarks(movingPoints);
  initializer->InitializeTransform();

  // Get result (do the registration)
  vtkSmartPointer<vtkMatrix4x4> phantomToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  phantomToReferenceTransformMatrix->Identity();

  itk::Matrix<double,3,3> transformMatrix = transform->GetMatrix();
  for (int i=0; i<transformMatrix.RowDimensions; ++i)
  {
    for (int j=0; j<transformMatrix.ColumnDimensions; ++j)
    {
      phantomToReferenceTransformMatrix->SetElement(i, j, transformMatrix[i][j]);
    }
  }
  itk::Vector<double,3> transformOffset = transform->GetOffset();
  for (int j=0; j<transformOffset.GetNumberOfComponents(); ++j)
  {
    phantomToReferenceTransformMatrix->SetElement(j, 3, transformOffset[j]);
  }

  std::ostringstream osPhantomToReferenceTransformMatrix;
  phantomToReferenceTransformMatrix->Print(osPhantomToReferenceTransformMatrix);

  LOG_DEBUG("PhantomToReferenceTransformMatrix:\n" << osPhantomToReferenceTransformMatrix.str().c_str() );

  this->SetPhantomToReferenceTransformMatrix(phantomToReferenceTransformMatrix);

  if (ComputeError() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to compute registration error!");
    return PLUS_FAIL;
  }

  // Save result
  if (aTransformRepository)
  {
    PlusTransformName phantomToReferenceTransformName(this->PhantomCoordinateFrame, this->ReferenceCoordinateFrame);
    aTransformRepository->SetTransform(phantomToReferenceTransformName, this->PhantomToReferenceTransformMatrix);
    aTransformRepository->SetTransformPersistent(phantomToReferenceTransformName, true);
    aTransformRepository->SetTransformDate(phantomToReferenceTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());
    aTransformRepository->SetTransformError(phantomToReferenceTransformName, this->RegistrationError);
  }
  else
  {
    LOG_INFO("Transform repository object is NULL, cannot save results into it");
  }

  return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------

PlusStatus vtkPhantomLandmarkRegistrationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkPhantomLandmarkRegistrationAlgo::ReadConfiguration");

  XML_FIND_NESTED_ELEMENT_REQUIRED(phantomDefinition, aConfig, "PhantomDefinition");

  this->DefinedLandmarks->Reset();
  this->RecordedLandmarks->Reset();
  this->DefinedLandmarkNames.clear();

  // Load geometry
  XML_FIND_NESTED_ELEMENT_REQUIRED(geometry, phantomDefinition, "Geometry");

  // Read landmarks (NWires are not interesting at this point, it is only parsed if segmentation is needed)
  vtkXMLDataElement* landmarks = geometry->FindNestedElementWithName("Landmarks"); 
  if (landmarks == NULL)
  {
    if(geometry->FindNestedElementWithName("Planes") == NULL)
    {
      LOG_ERROR("No Planes or Landmarks found in configuration file found, registration is not possible!");
      return PLUS_FAIL;
    }
    else
    {
      LOG_WARNING("No Landmarks found in configuration file found, perform Landmark Registration");
      return PLUS_FAIL;
    }
  }
  else
  {
    int numberOfLandmarks = landmarks->GetNumberOfNestedElements();
    this->DefinedLandmarkNames.resize(numberOfLandmarks);

    for (int i=0; i<numberOfLandmarks; ++i)
    {
      vtkXMLDataElement* landmark = landmarks->GetNestedElement(i);
      if ((landmark == NULL) || (STRCASECMP("Landmark", landmark->GetName())))
      {
        LOG_WARNING("Invalid landmark definition found");
        continue;
      }

      const char* landmarkName = landmark->GetAttribute("Name");
      if (landmarkName==NULL)
      {
        LOG_WARNING("Invalid landmark name (landmark #"<<i+1<<")");
        continue;
      }

      double landmarkPosition[3];
      if (! landmark->GetVectorAttribute("Position", 3, landmarkPosition))
      {
        LOG_WARNING("Invalid landmark position (landmark #"<<i+1<<")");
        continue;
      }

      this->DefinedLandmarks->InsertPoint(i, landmarkPosition);
      this->DefinedLandmarkNames[i] = landmarkName;
    }
  }

  if (this->DefinedLandmarks->GetNumberOfPoints() == 0)
  {
    LOG_ERROR("No valid landmarks were found!");
    return PLUS_FAIL;
  }

  // vtkPhantomLandmarkRegistrationAlgo section
  XML_FIND_NESTED_ELEMENT_REQUIRED(phantomRegistrationElement, aConfig, vtkPhantomLandmarkRegistrationAlgo::ConfigurationElementName.c_str());
  XML_READ_STRING_ATTRIBUTE_REQUIRED(PhantomCoordinateFrame, phantomRegistrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, phantomRegistrationElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(StylusTipCoordinateFrame, phantomRegistrationElement);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhantomLandmarkRegistrationAlgo::ComputeError()
{
  LOG_TRACE("vtkPhantomLandmarkRegistrationAlgo::ComputeError");

  double sumDistance = 0.0;

  for (int i=0; i<this->RecordedLandmarks->GetNumberOfPoints(); ++i)
  {
    // Defined landmarks from xml are in the phantom coordinate system
    double* landmarkPointArray = this->DefinedLandmarks->GetPoint(i);
    double landmarkPoint[4] = {landmarkPointArray[0], landmarkPointArray[1], landmarkPointArray[2], 1.0};

    double* transformedLandmarkPoint = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(landmarkPoint);

    // Recorded landmarks are in the tracker coordinate system
    double* recordedPointArray = this->RecordedLandmarks->GetPoint(i);
//LOG_INFO("Difference pivot " <<i<<"("<<landmarksAverage[0]<<", "<<landmarksAverage[1]<<", "<<landmarksAverage[2]<<")");
    sumDistance += sqrt( vtkMath::Distance2BetweenPoints(transformedLandmarkPoint, recordedPointArray) );
  }

  this->RegistrationError = sumDistance / this->RecordedLandmarks->GetNumberOfPoints();

  LOG_DEBUG("Registration error = " << this->RegistrationError);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
double vtkPhantomLandmarkRegistrationAlgo::GetMinimunDistanceBetweenTwoLandmarks()
{
  // Defined landmarks from xml are in the phantom coordinate system
  double referencePointArray[3] ;
  this->DefinedLandmarks->GetPoint(0,referencePointArray);
  double minimumDistance = DBL_MAX;
  double otherPointArray[3];

  for (int i=1; i<this->DefinedLandmarks->GetNumberOfPoints(); ++i)
  {
    this->DefinedLandmarks->GetPoint(i,otherPointArray);
    // Defined landmarks from xml are in the phantom coordinate system
    double distance = sqrt( vtkMath::Distance2BetweenPoints(referencePointArray, otherPointArray) );
    if( distance < minimumDistance)
    {
      minimumDistance = distance;
    }
  }
  return minimumDistance;
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetDefinedLandmarksCentroid_Reference(double* landmarksAverage_Reference)
{
  // Defined landmarks from xml are in the phantom coordinate system
  double landmarksAverage[4] = {0,0,0,1};
  int numberOfLandmarks = this->DefinedLandmarks->GetNumberOfPoints();
  for ( int landmarkIndex=0; landmarkIndex<numberOfLandmarks; landmarkIndex++ )
  {
    double definedLandmarkPoint[3] = {0,0,0};
    this->DefinedLandmarks->GetPoint(landmarkIndex,definedLandmarkPoint);
    landmarksAverage[0]+=definedLandmarkPoint[0];
    landmarksAverage[1]+=definedLandmarkPoint[1];
    landmarksAverage[2]+=definedLandmarkPoint[2];
  }
  landmarksAverage[0]=landmarksAverage[0]/numberOfLandmarks;
  landmarksAverage[1]=landmarksAverage[1]/numberOfLandmarks;
  landmarksAverage[2]=landmarksAverage[2]/numberOfLandmarks;
  // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of landmarksAverage_Reference, too
  double *temporalPtr = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(landmarksAverage);
  landmarksAverage_Reference[0]=temporalPtr[0];
  landmarksAverage_Reference[1]=temporalPtr[1];
  landmarksAverage_Reference[2]=temporalPtr[2];
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetDefinedLandmarksAverage(double* landmarksAverage)
{
  // Defined landmarks from xml are in the phantom coordinate system
  double definedLandmarkPoint[3];
  int numberOfLandmarks = this->DefinedLandmarks->GetNumberOfPoints();
  for ( int landmarkIndex=0; landmarkIndex<numberOfLandmarks; landmarkIndex++ )
  {
    this->DefinedLandmarks->GetPoint(landmarkIndex,definedLandmarkPoint);
    landmarksAverage[0]+=definedLandmarkPoint[0];
    landmarksAverage[1]+=definedLandmarkPoint[1];
    landmarksAverage[2]+=definedLandmarkPoint[2];
  }
  landmarksAverage[0]=landmarksAverage[0]/numberOfLandmarks;
  landmarksAverage[1]=landmarksAverage[1]/numberOfLandmarks;
  landmarksAverage[2]=landmarksAverage[2]/numberOfLandmarks;
  return;
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetLandmarkCameraPosition_Reference(int index, double* cameraPosition_Reference)
{
  this->DefinedLandmarks->GetPoint(index,cameraPosition_Reference);
  double centroid[3]={0,0,0};
  double viewNormalVector_Reference[3]={0,0,0};

  this->GetDefinedLandmarksAverage(centroid);
  viewNormalVector_Reference[0]=cameraPosition_Reference[0]-centroid[0];
  viewNormalVector_Reference[1]=cameraPosition_Reference[1]-centroid[1];
  viewNormalVector_Reference[2]=cameraPosition_Reference[2]-centroid[2];

  vtkMath::Normalize(viewNormalVector_Reference);
  const double CAMERA_DISTANCE_FROM_CENTROID = 500; // in general, 50cm from the centroid gives a good view of the phantom
  centroid[0]=centroid[0]+CAMERA_DISTANCE_FROM_CENTROID*viewNormalVector_Reference[0];
  centroid[1]=centroid[1]+CAMERA_DISTANCE_FROM_CENTROID*viewNormalVector_Reference[1];
  centroid[2]=centroid[2]+CAMERA_DISTANCE_FROM_CENTROID*viewNormalVector_Reference[2];

  // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of cameraPosition_Reference, too
  double *tmpPoint = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(centroid); 
  cameraPosition_Reference[0]=tmpPoint[0];
  cameraPosition_Reference[1]=tmpPoint[1];
  cameraPosition_Reference[2]=tmpPoint[2];
  //LOG_INFO("Defined Landmark " <<index<< cameraPosition_Reference);
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetDefinedLandmark_Reference(int index, double* cameraPosition_Reference)
{
  this->DefinedLandmarks->GetPoint(index,cameraPosition_Reference);

  // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of cameraPosition_Reference, too
  double *temporalPoint = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(cameraPosition_Reference);
  cameraPosition_Reference[0]=temporalPoint[0];
  cameraPosition_Reference[1]=temporalPoint[1];
  cameraPosition_Reference[2]=temporalPoint[2];
  //LOG_INFO("Defined Landmark " <<index<< cameraPosition_Reference);
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::PrintRecordedLandmarks_Phantom()
{
  // Defined landmarks from xml are in the phantom coordinate system
  double recordedLandmarkPoint[3];
  double recordedLandmark[4]={0,0,0,1};
  double *temporalPtr;
  int i=0;

  vtkSmartPointer<vtkMatrix4x4> referenceToPhantomTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(this->PhantomToReferenceTransformMatrix,referenceToPhantomTransformMatrix);
  while( i<this->RecordedLandmarks->GetNumberOfPoints())
  {
    this->RecordedLandmarks->GetPoint(i,recordedLandmarkPoint);
    recordedLandmark[0]=recordedLandmarkPoint[0];
    recordedLandmark[1]=recordedLandmarkPoint[1];
    recordedLandmark[2]=recordedLandmarkPoint[2];
    temporalPtr=referenceToPhantomTransformMatrix->MultiplyDoublePoint(recordedLandmark);
    LOG_INFO("Pivot_Phantom "<< i <<" ("<<temporalPtr[0]<<", "<<temporalPtr[1]<<", "<<temporalPtr[2]<<")");
    i++;
  }
}
