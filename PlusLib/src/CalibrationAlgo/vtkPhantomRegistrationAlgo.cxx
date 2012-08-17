/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPhantomRegistrationAlgo.h"
#include "vtkTransformRepository.h"

#include "vtkObjectFactory.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx" 
#include "vtkMath.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkSimilarity3DTransform.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPhantomRegistrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPhantomRegistrationAlgo);

std::string vtkPhantomRegistrationAlgo::ConfigurationElementName = "vtkPhantomRegistrationAlgo";

//-----------------------------------------------------------------------------

vtkPhantomRegistrationAlgo::vtkPhantomRegistrationAlgo()
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

  vtkSmartPointer<vtkPoints> acquiredLandmarks = vtkSmartPointer<vtkPoints>::New();
  this->SetRecordedLandmarks(acquiredLandmarks);
}

//-----------------------------------------------------------------------------

vtkPhantomRegistrationAlgo::~vtkPhantomRegistrationAlgo()
{
  this->SetPhantomToReferenceTransformMatrix(NULL);
  this->SetDefinedLandmarks(NULL);
  this->SetRecordedLandmarks(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkPhantomRegistrationAlgo::Register(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkPhantomRegistrationAlgo::Register"); 

  // Create input point vectors
  //int numberOfLandmarks = this->DefinedLandmarks->GetNumberOfPoints();
  std::vector<itk::Point<double,3> > fixedPoints;
  std::vector<itk::Point<double,3> > movingPoints;

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
  itk::Similarity3DTransform<double>::Pointer transform = itk::Similarity3DTransform<double>::New();
  transform->SetIdentity();

  itk::LandmarkBasedTransformInitializer<itk::Similarity3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::Pointer initializer = itk::LandmarkBasedTransformInitializer<itk::Similarity3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::New();
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

PlusStatus vtkPhantomRegistrationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkPhantomRegistrationAlgo::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Invalid configuration! Problably device set is not connected.");
    return PLUS_FAIL;
  }

  // Find phantom definition element
  vtkXMLDataElement* phantomDefinition = aConfig->FindNestedElementWithName("PhantomDefinition");
  if (phantomDefinition == NULL) {
    LOG_ERROR("No phantom definition is found in the XML tree!");
    return PLUS_FAIL;
  }

  this->DefinedLandmarks->Reset();
  this->RecordedLandmarks->Reset();
  this->DefinedLandmarkNames.clear();

  // Load geometry
  vtkXMLDataElement* geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
  if (geometry == NULL)
  {
    LOG_ERROR("Phantom geometry information not found!");
    return PLUS_FAIL;
  }

  // Read landmarks (NWires are not interesting at this point, it is only parsed if segmentation is needed)
  vtkXMLDataElement* landmarks = geometry->FindNestedElementWithName("Landmarks"); 
  if (landmarks == NULL)
  {
    LOG_ERROR("Landmarks not found, registration is not possible!");
    return PLUS_FAIL;
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
        LOG_WARNING("Invalid landmark definition found!");
        continue;
      }

      const char* landmarkName = landmark->GetAttribute("Name");
      std::string landmarkNameString(landmarkName);
      double landmarkPosition[3];

      if (! landmark->GetVectorAttribute("Position", 3, landmarkPosition))
      {
        LOG_WARNING("Invalid landmark position!");
        continue;
      }

      this->DefinedLandmarks->InsertPoint(i, landmarkPosition);
      this->DefinedLandmarkNames[i] = landmarkNameString;
    }

    if (this->DefinedLandmarks->GetNumberOfPoints() != numberOfLandmarks)
    {
      LOG_WARNING("Some invalid landmarks were found!");
    }
  }

  if (this->DefinedLandmarks->GetNumberOfPoints() == 0)
  {
    LOG_ERROR("No valid landmarks were found!");
    return PLUS_FAIL;
  }

  // vtkPhantomRegistrationAlgo section
  vtkXMLDataElement* phantomRegistrationElement = aConfig->FindNestedElementWithName(vtkPhantomRegistrationAlgo::ConfigurationElementName.c_str()); 

  if (phantomRegistrationElement == NULL)
  {
    LOG_ERROR("Unable to find " << vtkPhantomRegistrationAlgo::ConfigurationElementName << " element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Phantom coordinate frame name
  const char* phantomCoordinateFrame = phantomRegistrationElement->GetAttribute("PhantomCoordinateFrame");
  if (phantomCoordinateFrame == NULL)
  {
    LOG_ERROR("PhantomCoordinateFrame is not specified in " << vtkPhantomRegistrationAlgo::ConfigurationElementName << " element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetPhantomCoordinateFrame(phantomCoordinateFrame);

  // Reference coordinate frame name
  const char* referenceCoordinateFrame = phantomRegistrationElement->GetAttribute("ReferenceCoordinateFrame");
  if (referenceCoordinateFrame == NULL)
  {
    LOG_ERROR("ReferenceCoordinateFrame is not specified in " << vtkPhantomRegistrationAlgo::ConfigurationElementName << " element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetReferenceCoordinateFrame(referenceCoordinateFrame);

  // Object pivot point coordinate frame name
  const char* stylusTipCoordinateFrame = phantomRegistrationElement->GetAttribute("StylusTipCoordinateFrame");
  if (stylusTipCoordinateFrame == NULL)
  {
    LOG_ERROR("StylusTipCoordinateFrame is not specified in " << vtkPhantomRegistrationAlgo::ConfigurationElementName << " element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetStylusTipCoordinateFrame(stylusTipCoordinateFrame);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPhantomRegistrationAlgo::ComputeError()
{
  LOG_TRACE("vtkPhantomRegistrationAlgo::ComputeError");

  double sumDistance = 0.0;

  for (int i=0; i<this->RecordedLandmarks->GetNumberOfPoints(); ++i)
  {
    // Defined landmarks from xml are in the phantom coordinate system
    double* landmarkPointArray = this->DefinedLandmarks->GetPoint(i);
    double landmarkPoint[4] = {landmarkPointArray[0], landmarkPointArray[1], landmarkPointArray[2], 1.0};

    double* transformedLandmarkPoint = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(landmarkPoint);

    // Recorded landmarks are in the tracker coordinate system
    double* recordedPointArray = this->RecordedLandmarks->GetPoint(i);

    sumDistance += sqrt( vtkMath::Distance2BetweenPoints(transformedLandmarkPoint, recordedPointArray) );
  }

  this->RegistrationError = sumDistance / this->RecordedLandmarks->GetNumberOfPoints();

  LOG_DEBUG("Registration error = " << this->RegistrationError);

  return PLUS_SUCCESS;
}
