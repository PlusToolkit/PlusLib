/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusPhantomLinearObjectRegistrationAlgo.h"
#include "vtkIGSIOTransformRepository.h"

#include "vtkObjectFactory.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMath.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkSimilarity3DTransform.h"

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusPhantomLinearObjectRegistrationAlgo);

std::string vtkPlusPhantomLinearObjectRegistrationAlgo::ConfigurationElementName = "vtkPlusPhantomLinearObjectRegistrationAlgo";

//-----------------------------------------------------------------------------

vtkPlusPhantomLinearObjectRegistrationAlgo::vtkPlusPhantomLinearObjectRegistrationAlgo()
{
  this->RegistrationError = -1.0;

  this->PhantomToReferenceTransformMatrix = NULL;
  this->RecordedPoints = NULL;
  this->MarkerToReferenceTransformMatrixArray = NULL;
  this->PhantomCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->StylusTipCoordinateFrame = NULL;

  vtkSmartPointer<vtkPoints> acquiredPoints = vtkSmartPointer<vtkPoints>::New();
  this->SetRecordedPoints(acquiredPoints);

  vtkSmartPointer<vtkDoubleArray> markerToReferenceTransformMatrixArray = vtkSmartPointer<vtkDoubleArray>::New();
  this->SetMarkerToReferenceTransformMatrixArray(markerToReferenceTransformMatrixArray);
  this->MarkerToReferenceTransformMatrixArray->SetNumberOfComponents(16);
}

//-----------------------------------------------------------------------------

vtkPlusPhantomLinearObjectRegistrationAlgo::~vtkPlusPhantomLinearObjectRegistrationAlgo()
{
  this->SetPhantomToReferenceTransformMatrix(NULL);
  this->SetDefinedPlanesFromXML(NULL);
  this->SetRecordedPlanes(NULL);
  this->SetRecordedPoints(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusPhantomLinearObjectRegistrationAlgo::LinearObjectRegister(vtkIGSIOTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkPlusPhantomLinearObjectRegistrationAlgo::Register");

  Planes fixedPlanes;
  Planes movingPlanes;
  References fixedReferences;
  References movingReferences;

  for (int i=0; i<this->RecordedPoints->GetNumberOfPoints(); i++)
  {
    //TODO: Matthew's algorithm for converting points into a plane goes here
    ;

    //-----------------------------------------------------------------
  }

  fixedPlanes = this->DefinedPlanes;
  fixedReferences = this->DefinedReferences;

  // Look at matthew's code to see what his algorithnm wants as input
  // hint I think it's planes and an xml
  // Then, call his algorithm!

  //for each plane recorded by the stylus
  int i(0);
  for (std::vector<Plane>::const_iterator it = this->RecordedPlanes.PlanesBegin(); it != this->RecordedPlanes.PlanesEnd(); ++it )
  {
    LOG_DEBUG("Phantom plane " << i << ": Defined: " << it->ToXMLString() << "  Recorded: " << movingPlanes.GetPlane(i).ToXMLString() );
    ++i;
  }

  // Initialize ITK transform
  auto transform = itk::VersorRigid3DTransform<double>::New();
  transform->SetIdentity();

  //TODO: make itk::PlaneBasedTransfromInitializer
  /*auto initializer = itk::LandmarkBasedTransformInitializer<itk::VersorRigid3DTransform<double>, itk::Image<short,3>, itk::Image<short,3> >::New();
  initializer->SetTransform(transform);
  initializer->SetFixedPlanes(fixedPlanes);
  initializer->SetMovingPlanes(movingPlanes);
  initializer->InitializeTransform();*/

  // Get result (do the registration)
  vtkSmartPointer<vtkMatrix4x4> phantomToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  phantomToReferenceTransformMatrix->Identity();

  //TODO: this is where Matthew's algorithm for linear object registration will go--------------------------------------------------------------------
  /*itk::Matrix<double,3,3> transformMatrix = transform->GetMatrix();
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
  }*/

  //------------------------------------------------------------------------------------------------------------------------------------------------

  // Save result
  if (aTransformRepository)
  {
    igsioTransformName phantomToReferenceTransformName(this->PhantomCoordinateFrame, this->ReferenceCoordinateFrame);
    aTransformRepository->SetTransform(phantomToReferenceTransformName, this->PhantomToReferenceTransformMatrix);
    aTransformRepository->SetTransformPersistent(phantomToReferenceTransformName, true);
    aTransformRepository->SetTransformDate(phantomToReferenceTransformName, vtkIGSIOAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());
    aTransformRepository->SetTransformError(phantomToReferenceTransformName, this->RegistrationError);
  }
  else
  {
    LOG_INFO("Transform repository object is NULL, cannot save results into it");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusPhantomLinearObjectRegistrationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkPlusPhantomLinearObjectRegistrationAlgo::ReadConfiguration");

  XML_FIND_NESTED_ELEMENT_REQUIRED(phantomDefinition, aConfig, "PhantomDefinition");

  this->DefinedPlanes.Reset();
  this->RecordedPlanes.Reset();
  this->DefinedPlaneNames.clear();

  // Load geometry
  XML_FIND_NESTED_ELEMENT_REQUIRED(geometry, phantomDefinition, "Geometry");

// Read references (NWires are not interesting at this point, it is only parsed if segmentation is needed)
  vtkXMLDataElement* references = geometry->FindNestedElementWithName("References");
  if (references == NULL)
  {
    if(geometry->FindNestedElementWithName("Landmarks") == NULL)
    {
      LOG_ERROR("No References or Landmarks found in configuration file found, registration is not possible!");
      return PLUS_FAIL;
    }
    else
    {
      LOG_WARNING("No References found in configuration file found, perform Landmark Registration");
      return PLUS_FAIL;
    }
  }
  else
  {
    //find the number of references defined in the config file
    int numberOfReferences = references->GetNumberOfNestedElements();
    this->DefinedReferenceNames.resize(numberOfReferences);

    //for each reference
    for (int i=0; i<numberOfReferences; ++i)
    {
      //define a reference variable
      vtkXMLDataElement* reference = references->GetNestedElement(i);
      if ((reference == NULL) || (STRCASECMP("Reference", reference->GetName())))
      {
        LOG_WARNING("Invalid reference definition found");
        continue;
      }

      //define a varaible to store the name of the reference
      const char* referenceName = reference->GetAttribute("Name");
      if (referenceName==NULL)
      {
        LOG_WARNING("Invalid reference name (reference #"<<i+1<<")");
        continue;
      }

      double referencePosition[3];
      if (! reference->GetVectorAttribute("Position", 3, referencePosition))
      {
        LOG_WARNING("Invalid reference position (reference #"<<i+1<<")");
        continue;
      }

      std::vector<double> referencePositionVector;
      for(int j=0; j<3;j++)
      {
        referencePositionVector.push_back(referencePosition[j]);
      }

      this->DefinedReferences.InsertReference(Reference(referencePositionVector));
      this->DefinedReferenceNames[i] = referenceName;
    }
  }

  if (this->DefinedReferences.GetNumberOfReferences() == 0)
  {
    LOG_ERROR("No valid references were found!");
    return PLUS_FAIL;
  }

  // Read planes (NWires are not interesting at this point, it is only parsed if segmentation is needed)
  vtkXMLDataElement* planes = geometry->FindNestedElementWithName("Planes");
  if (planes == NULL)
  {
    if(geometry->FindNestedElementWithName("Landmarks") == NULL)
    {
      LOG_ERROR("No Planes or Landmarks found in configuration file found, registration is not possible!");
      return PLUS_FAIL;
    }
    else
    {
      LOG_WARNING("No Planes found in configuration file found, perform Landmark Registration");
      return PLUS_FAIL;
    }
  }
  else
  {
    //find the number of planes defined in the config file
    int numberOfPlanes = planes->GetNumberOfNestedElements();
    this->DefinedPlaneNames.resize(numberOfPlanes);

    //for each plane
    for (int i=0; i<numberOfPlanes; ++i)
    {
      //define a plane variable
      vtkXMLDataElement* plane = planes->GetNestedElement(i);

      if ((plane == NULL) || (STRCASECMP("Plane", plane->GetName())))
      {
        LOG_WARNING("Invalid plane definition found!");
        continue;
      }

      //define a varaible to store the name of the plane
      const char* planeName = plane->GetAttribute("Name");
      if (planeName==NULL)
      {
        LOG_WARNING("Invalid plane name (plane #"<<i+1<<")");
        continue;
      }

      //define an array of length 3 of 3-tuples (each plane has 3 points that define it each with 3 rectangular coordinates in the phantom coordinate system)
      std::vector<double> pointsOnPlane[3];

      double pointOnPlane[3];
      //test the validity of each point on the plane then add the plane to the DefinedPlanes
      if (! plane->GetVectorAttribute("BasePoint", 3, pointOnPlane))
      {
        LOG_WARNING("Invalid base point position!");
        continue;
      }
      for(int j=0; j<3; j++)
      {
        pointsOnPlane[0].push_back(pointOnPlane[j]);
      }

      if (! plane->GetVectorAttribute("EndPoint1", 3, pointOnPlane))
      {
        LOG_WARNING("Invalid end point 1 position!");
        continue;
      }
      for(int j=0; j<3; j++)
      {
        pointsOnPlane[1].push_back(pointOnPlane[j]);
      }

      if (! plane->GetVectorAttribute("EndPoint2", 3, pointOnPlane))
      {
        LOG_WARNING("Invalid end point 2 position!");
        continue;
      }
      for(int j=0; j<3; j++)
      {
        pointsOnPlane[2].push_back(pointOnPlane[j]);
      }

      this->DefinedPlanes.InsertPlane(Plane(pointsOnPlane[0], pointsOnPlane[1], pointsOnPlane[2]));
      this->DefinedPlaneNames[i] = planeName;
      //---------------------------------------------------------------------------------------------------------------------------------------------
    }
  }

  if (this->DefinedPlanes.GetNumberOfPlanes() == 0)
  {
    LOG_ERROR("No valid planes were found!");
    return PLUS_FAIL;
  }

  XML_FIND_NESTED_ELEMENT_REQUIRED(phantomRegistrationElement, aConfig, vtkPlusPhantomLinearObjectRegistrationAlgo::ConfigurationElementName.c_str());
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(PhantomCoordinateFrame, phantomRegistrationElement);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, phantomRegistrationElement);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(StylusTipCoordinateFrame, phantomRegistrationElement);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusPhantomLinearObjectRegistrationAlgo::ComputeError()
{
  LOG_TRACE("vtkPlusPhantomLinearObjectRegistrationAlgo::ComputeError");

  //TODO: Matthew's Algorithm for error calculation-------------------------------------------------------------------------------------------------

  /*for (int i=0; i<this->RecordedLandmarks->GetNumberOfPoints(); ++i)
  {
    // Defined landmarks from xml are in the phantom coordinate system
    double* landmarkPointArray = this->DefinedLandmarks->GetPoint(i);
    double landmarkPoint[4] = {landmarkPointArray[0], landmarkPointArray[1], landmarkPointArray[2], 1.0};

    double* transformedLandmarkPoint = this->PhantomToReferenceTransformMatrix->MultiplyDoublePoint(landmarkPoint);

    // Recorded landmarks are in the tracker coordinate system
    double* recordedPointArray = this->RecordedLandmarks->GetPoint(i);

    sumDistance += sqrt( vtkMath::Distance2BetweenPoints(transformedLandmarkPoint, recordedPointArray) );
  }

  this->RegistrationError = sumDistance / this->RecordedLandmarks->GetNumberOfPoints();*/

  //----------------------------------------------------------------------------------------------------------------------------------------

  LOG_DEBUG("Registration error = " << this->RegistrationError);

  return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------

PlusStatus vtkPlusPhantomLinearObjectRegistrationAlgo::InsertNextCalibrationPoint(vtkMatrix4x4* aMarkerToReferenceTransformMatrix)
{
  this->MarkerToReferenceTransformMatrixArray->InsertNextTuple(*aMarkerToReferenceTransformMatrix->Element);

  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------

PlusStatus vtkPlusPhantomLinearObjectRegistrationAlgo::Initialize()
{
  LOG_TRACE("vtkPhantomLinearObjectRegistration::Initialize");

  this->MarkerToReferenceTransformMatrixArray->SetNumberOfTuples(0);

  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------

void vtkPlusPhantomLinearObjectRegistrationAlgo::SetRecordedPlanes(Planes* planes)
{
  if(planes == NULL)
  {
    this->RecordedPlanes.Reset();
    return;
  }

  this->RecordedPlanes = *planes;
}
//-----------------------------------------------------------------------------

void vtkPlusPhantomLinearObjectRegistrationAlgo::SetDefinedPlanesFromXML(Planes* planes)
{
  if(planes == NULL)
  {
    this->DefinedPlanes.Reset();
    return;
  }

  this->DefinedPlanes = *planes;
}
