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

vtkStandardNewMacro(vtkPhantomLandmarkRegistrationAlgo);

std::string vtkPhantomLandmarkRegistrationAlgo::ConfigurationElementName = "vtkPhantomLandmarkRegistrationAlgo";

//-----------------------------------------------------------------------------

vtkPhantomLandmarkRegistrationAlgo::vtkPhantomLandmarkRegistrationAlgo()
{
  this->RegistrationErrorMm = -1.0;

  this->PhantomToReferenceTransformMatrix = NULL;
  this->DefinedLandmarks_Phantom = NULL;
  this->RecordedLandmarks_Reference = NULL;
  this->PhantomCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->StylusTipCoordinateFrame = NULL;

  this->DefinedLandmarkNames.clear();

  vtkSmartPointer<vtkPoints> definedLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetDefinedLandmarks_Phantom(definedLandmarks);

  vtkSmartPointer<vtkPoints> recordedLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetRecordedLandmarks_Reference(recordedLandmarks);
}

//-----------------------------------------------------------------------------

vtkPhantomLandmarkRegistrationAlgo::~vtkPhantomLandmarkRegistrationAlgo()
{
  this->SetPhantomToReferenceTransformMatrix(NULL);
  this->SetDefinedLandmarks_Phantom(NULL);
  this->SetRecordedLandmarks_Reference(NULL);
  this->PhantomCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->StylusTipCoordinateFrame = NULL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPhantomLandmarkRegistrationAlgo::Register(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkPhantomLandmarkRegistrationAlgo::Register"); 

  // Create input point vectors
  std::vector< itk::Point<double,3> > fixedPoints;
  std::vector< itk::Point<double,3> > movingPoints;

  for (int i=0; i<this->RecordedLandmarks_Reference->GetNumberOfPoints(); ++i)
  {
    // Defined landmarks from xml are in the phantom coordinate system
    double* fixedPointArray = this->DefinedLandmarks_Phantom->GetPoint(i);
    itk::Point<double,3> fixedPoint(fixedPointArray);

    // Recorded landmarks are in the tracker coordinate system
    double* movingPointArray = this->RecordedLandmarks_Reference->GetPoint(i);
    itk::Point<double,3> movingPoint(movingPointArray);

    fixedPoints.push_back(fixedPoint);
    movingPoints.push_back(movingPoint);
  }

  for (int i=0; i<this->RecordedLandmarks_Reference->GetNumberOfPoints(); ++i)
  {
    LOG_DEBUG("Phantom point " << i << ": Defined: " << fixedPoints[i] << "  Recorded: " << movingPoints[i]);
  }

  // Initialize ITK transform
  itk::VersorRigid3DTransform<double>::Pointer transform = itk::VersorRigid3DTransform<double>::New();
  transform->SetIdentity();

  itk::LandmarkBasedTransformInitializer< itk::VersorRigid3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::Pointer initializer = itk::LandmarkBasedTransformInitializer< itk::VersorRigid3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::New();
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
    aTransformRepository->SetTransformError(phantomToReferenceTransformName, this->RegistrationErrorMm);
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

  this->DefinedLandmarks_Phantom->Reset();
  this->RecordedLandmarks_Reference->Reset();
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

      double landmarkPosition[4] = {0,0,0,1};
      if (! landmark->GetVectorAttribute("Position", 3, landmarkPosition))
      {
        LOG_WARNING("Invalid landmark position (landmark #"<<i+1<<")");
        continue;
      }

      this->DefinedLandmarks_Phantom->InsertPoint(i, landmarkPosition);
      this->DefinedLandmarkNames[i] = landmarkName;
    }
  }

  if (this->DefinedLandmarks_Phantom->GetNumberOfPoints() == 0)
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

  for (int i=0; i<this->RecordedLandmarks_Reference->GetNumberOfPoints(); ++i)
  {
    // Defined landmarks from xml are in the phantom coordinate system
    double definedLandmark_Phantom[4] = {0,0,0, 1.0};
    this->DefinedLandmarks_Phantom->GetPoint(i,definedLandmark_Phantom);

    double* definedLandmark_Reference = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(definedLandmark_Phantom);
    sumDistance += sqrt( vtkMath::Distance2BetweenPoints(definedLandmark_Reference, this->RecordedLandmarks_Reference->GetPoint(i)) );
  }

  this->RegistrationErrorMm = sumDistance / this->RecordedLandmarks_Reference->GetNumberOfPoints();

  LOG_DEBUG("Registration error = " << this->RegistrationErrorMm);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
double vtkPhantomLandmarkRegistrationAlgo::GetMinimunDistanceBetweenTwoLandmarksMm()
{
  // Defined landmarks from xml are in the phantom coordinate system
  double referencePointArray_Phantom[4]={0,0,0,1};
  double otherPointArray_Phantom[4]={0,0,0,1};
  double minimumDistanceMm = DBL_MAX;

  for (int i=0; i<this->DefinedLandmarks_Phantom->GetNumberOfPoints(); ++i)
  {
    this->DefinedLandmarks_Phantom->GetPoint(i,referencePointArray_Phantom);
    for(int j=i+1; j<this->DefinedLandmarks_Phantom->GetNumberOfPoints(); ++j)
    {
      this->DefinedLandmarks_Phantom->GetPoint(j,otherPointArray_Phantom);
      // Defined landmarks from xml are in the phantom coordinate system
      double distanceMm = sqrt( vtkMath::Distance2BetweenPoints(referencePointArray_Phantom, otherPointArray_Phantom) );
      if( distanceMm < minimumDistanceMm)
      {
        minimumDistanceMm = distanceMm;
      }
    }
  }
  return minimumDistanceMm;
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetDefinedLandmarksCentroid_Reference(double landmarksAverage_Reference[4])
{
  // Defined landmarks from xml are in the phantom coordinate system
  double landmarksAverage_Phantom[4] = {0,0,0,1};
 GetDefinedLandmarksCentroid_Phantom(landmarksAverage_Phantom);
  // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of landmarksAverage_Reference, too
  double *tempPtr = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(landmarksAverage_Phantom);
  landmarksAverage_Reference[0]=tempPtr[0];
  landmarksAverage_Reference[1]=tempPtr[1];
  landmarksAverage_Reference[2]=tempPtr[2];
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetDefinedLandmarksCentroid_Phantom(double landmarksCentroid_Phantom[4])
{
  // Defined landmarks from xml are in the phantom coordinate system
  double definedLandmarkPoint_Phantom[4] = {0,0,0,1};
  int numberOfLandmarks = this->DefinedLandmarks_Phantom->GetNumberOfPoints();
  for ( int landmarkIndex=0; landmarkIndex<numberOfLandmarks; landmarkIndex++ )
  {
    this->DefinedLandmarks_Phantom->GetPoint(landmarkIndex,definedLandmarkPoint_Phantom);
    landmarksCentroid_Phantom[0]+=definedLandmarkPoint_Phantom[0];
    landmarksCentroid_Phantom[1]+=definedLandmarkPoint_Phantom[1];
    landmarksCentroid_Phantom[2]+=definedLandmarkPoint_Phantom[2];
  }
  landmarksCentroid_Phantom[0]=landmarksCentroid_Phantom[0]/numberOfLandmarks;
  landmarksCentroid_Phantom[1]=landmarksCentroid_Phantom[1]/numberOfLandmarks;
  landmarksCentroid_Phantom[2]=landmarksCentroid_Phantom[2]/numberOfLandmarks;
  return;
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetLandmarkCameraPosition_Reference(int index, double cameraPosition_Reference[4])
{
  this->DefinedLandmarks_Phantom->GetPoint(index,cameraPosition_Reference);
  double centroid_Phantom[4] = {0,0,0,1};
  double viewNormalVector_Reference[4] = {0,0,0,1};

  this->GetDefinedLandmarksCentroid_Phantom(centroid_Phantom);
  viewNormalVector_Reference[0]=cameraPosition_Reference[0]-centroid_Phantom[0];
  viewNormalVector_Reference[1]=cameraPosition_Reference[1]-centroid_Phantom[1];
  viewNormalVector_Reference[2]=cameraPosition_Reference[2]-centroid_Phantom[2];

  vtkMath::Normalize(viewNormalVector_Reference);
  const double CAMERA_DISTANCE_FROM_CENTROID = 500; // in general, 50cm from the centroid gives a good view of the phantom
  centroid_Phantom[0]=centroid_Phantom[0]+CAMERA_DISTANCE_FROM_CENTROID*viewNormalVector_Reference[0];
  centroid_Phantom[1]=centroid_Phantom[1]+CAMERA_DISTANCE_FROM_CENTROID*viewNormalVector_Reference[1];
  centroid_Phantom[2]=centroid_Phantom[2]+CAMERA_DISTANCE_FROM_CENTROID*viewNormalVector_Reference[2];

  // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of cameraPosition_Reference, too
  double *tempPtr = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(centroid_Phantom); 
  cameraPosition_Reference[0]=tempPtr[0];
  cameraPosition_Reference[1]=tempPtr[1];
  cameraPosition_Reference[2]=tempPtr[2];
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::GetDefinedLandmark_Reference(int index, double cameraPosition_Reference[4])
{
  this->DefinedLandmarks_Phantom->GetPoint(index,cameraPosition_Reference);

  // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of cameraPosition_Reference, too
  double *tempPtr = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(cameraPosition_Reference);
  cameraPosition_Reference[0]=tempPtr[0];
  cameraPosition_Reference[1]=tempPtr[1];
  cameraPosition_Reference[2]=tempPtr[2];
}

//-----------------------------------------------------------------------------
void vtkPhantomLandmarkRegistrationAlgo::PrintRecordedLandmarks_Phantom()
{
  // Defined landmarks from xml are in the phantom coordinate system
  double recordedLandmarkPoint_Phantom[4] = {0,0,0,1};
  double *tempPtr;

  vtkSmartPointer<vtkMatrix4x4> referenceToPhantomTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(this->PhantomToReferenceTransformMatrix,referenceToPhantomTransformMatrix);
  int numberOfLandmarks = this->DefinedLandmarks_Phantom->GetNumberOfPoints();
  for ( int landmarkIndex=0; landmarkIndex<numberOfLandmarks; landmarkIndex++ )
  {
    this->RecordedLandmarks_Reference->GetPoint(landmarkIndex,recordedLandmarkPoint_Phantom);
    // store it in a temporary variable because MultiplyDoublePoint would overwrite the 4th component of cameraPosition_Reference, too
    tempPtr=referenceToPhantomTransformMatrix->MultiplyDoublePoint(recordedLandmarkPoint_Phantom);
    LOG_INFO("RecordedLandmark_Phantom "<< landmarkIndex <<" ("<<tempPtr[0]<<", "<<tempPtr[1]<<", "<<tempPtr[2]<<")");
  }
}
