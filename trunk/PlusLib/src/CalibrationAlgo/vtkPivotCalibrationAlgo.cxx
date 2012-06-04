/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPivotCalibrationAlgo.h"
#include "vtkTransformRepository.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtkMath.h"
#include "vtksys/SystemTools.hxx"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPivotCalibrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPivotCalibrationAlgo);

//-----------------------------------------------------------------------------

vtkPivotCalibrationAlgo::vtkPivotCalibrationAlgo()
{
	this->PivotPointToMarkerTransformMatrix = NULL;
	this->CalibrationError = -1.0;
  this->Minimizer = NULL;
  this->MarkerToReferenceTransformMatrixArray = NULL;
  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->ObjectPivotPointCoordinateFrame = NULL;

  vtkSmartPointer<vtkAmoebaMinimizer> minimizer = vtkSmartPointer<vtkAmoebaMinimizer>::New();
  this->SetMinimizer(minimizer);

  vtkSmartPointer<vtkDoubleArray> markerToReferenceTransformMatrixArray = vtkSmartPointer<vtkDoubleArray>::New();
  this->SetMarkerToReferenceTransformMatrixArray(markerToReferenceTransformMatrixArray);
  this->MarkerToReferenceTransformMatrixArray->SetNumberOfComponents(16);

  this->PivotPointPosition[0] = 0.0;
  this->PivotPointPosition[1] = 0.0;
  this->PivotPointPosition[2] = 0.0;

  Initialize();
}

//-----------------------------------------------------------------------------

vtkPivotCalibrationAlgo::~vtkPivotCalibrationAlgo()
{
  this->SetPivotPointToMarkerTransformMatrix(NULL);
  this->SetMarkerToReferenceTransformMatrixArray(NULL);
  this->SetMinimizer(NULL);
}

//----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::Initialize()
{
	LOG_TRACE("vtkPivotCalibrationAlgo::Initialize");

	this->MarkerToReferenceTransformMatrixArray->SetNumberOfTuples(0);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::InsertNextCalibrationPoint(vtkSmartPointer<vtkMatrix4x4> aMarkerToReferenceTransformMatrix)
{
	//LOG_TRACE("vtkPivotCalibrationAlgo::InsertNextCalibrationPoint");

  this->MarkerToReferenceTransformMatrixArray->InsertNextTuple(*aMarkerToReferenceTransformMatrix->Element);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::DoPivotCalibration(vtkTransformRepository* aTransformRepository/* = NULL*/)
{
  LOG_TRACE("vtkPivotCalibrationAlgo::DoPivotCalibration");

  // Set up minimizer and run the optimalization
	this->Minimizer->SetFunction(vtkTrackerToolCalibrationFunction,this);
	this->Minimizer->SetFunctionArgDelete(NULL);
	this->Minimizer->SetParameterValue("x",0);
	this->Minimizer->SetParameterScale("x",1000);
	this->Minimizer->SetParameterValue("y",0);
	this->Minimizer->SetParameterScale("y",1000);
	this->Minimizer->SetParameterValue("z",0);
	this->Minimizer->SetParameterScale("z",1000);

	this->Minimizer->Minimize();

	this->CalibrationError = this->Minimizer->GetFunctionValue();

  // Get the result (tooltip to tool transform)
	double x = this->Minimizer->GetParameterValue("x");
	double y = this->Minimizer->GetParameterValue("y");
	double z = this->Minimizer->GetParameterValue("z");

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

  // Compute a tooltip position based on the first acquired position
	double firstMarkerToReferenceTransformElements[16];
  this->MarkerToReferenceTransformMatrixArray->GetTuple(0, firstMarkerToReferenceTransformElements);
  vtkSmartPointer<vtkMatrix4x4> firstMarkerToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  firstMarkerToReferenceTransformMatrix->DeepCopy(firstMarkerToReferenceTransformElements);

  vtkSmartPointer<vtkTransform> pivotPointToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
  pivotPointToReferenceTransform->Identity();
  pivotPointToReferenceTransform->Concatenate(firstMarkerToReferenceTransformMatrix);
  pivotPointToReferenceTransform->Concatenate(this->PivotPointToMarkerTransformMatrix);

  // Set pivot point position
  pivotPointToReferenceTransform->GetPosition(this->PivotPointPosition);

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

std::string vtkPivotCalibrationAlgo::GetPivotPointToMarkerTranslationString()
{
	//LOG_TRACE("vtkPivotCalibrationAlgo::GetPivotPointToMarkerTranslationString");

  if (this->PivotPointToMarkerTransformMatrix == NULL) {
    LOG_ERROR("Tooltip to tool transform is not initialized!");
    return "";
  }

	char pivotPointToMarkerTranslationChars[32];
	sprintf_s(pivotPointToMarkerTranslationChars, 32, "%.2lf X %.2lf X %.2lf", this->PivotPointToMarkerTransformMatrix->GetElement(0,3), this->PivotPointToMarkerTransformMatrix->GetElement(1,3), this->PivotPointToMarkerTransformMatrix->GetElement(2,3));
	std::string pivotPointToMarkerTranslationString(pivotPointToMarkerTranslationChars);

	return pivotPointToMarkerTranslationString;
}

//----------------------------------------------------------------------------

void vtkTrackerToolCalibrationFunction(void *userData)
{
  LOG_TRACE("(vtkPivotCalibrationAlgo)vtkTrackerToolCalibrationFunction");

  vtkPivotCalibrationAlgo *self = (vtkPivotCalibrationAlgo*)userData;

	int i;
	int n = self->MarkerToReferenceTransformMatrixArray->GetNumberOfTuples();

	double x = self->Minimizer->GetParameterValue("x");
	double y = self->Minimizer->GetParameterValue("y");
	double z = self->Minimizer->GetParameterValue("z");
	double nx,ny,nz,sx,sy,sz,sxx,syy,szz;

	double matrix[4][4];

	sx = sy = sz = 0.0;
	sxx = syy = szz = 0.0;

	for (i = 0; i < n; i++)
	{
		self->MarkerToReferenceTransformMatrixArray->GetTuple(i,*matrix);

		nx = matrix[0][0]*x + matrix[0][1]*y + matrix[0][2]*z + matrix[0][3];
		ny = matrix[1][0]*x + matrix[1][1]*y + matrix[1][2]*z + matrix[1][3];
		nz = matrix[2][0]*x + matrix[2][1]*y + matrix[2][2]*z + matrix[2][3];

		sx += nx;
		sy += ny;
		sz += nz;

		sxx += nx*nx;
		syy += ny*ny;
		szz += nz*nz;
	}

	double r;

	if (n > 1)
	{
		r = sqrt((sxx - sx*sx/n)/(n-1) +
			(syy - sy*sy/n)/(n-1) +
			(szz - sz*sz/n)/(n-1));
	}
	else
	{
		r = 0;
	}

	self->Minimizer->SetFunctionValue(r);
}

//-----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkPivotCalibrationAlgo::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  // vtkPivotCalibrationAlgo section
  vtkXMLDataElement* pivotCalibrationElement = aConfig->FindNestedElementWithName("vtkPivotCalibrationAlgo"); 

  if (pivotCalibrationElement == NULL)
  {
    LOG_ERROR("Unable to find vtkPivotCalibrationAlgo element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Object marker coordinate frame name
  const char* objectMarkerCoordinateFrame = pivotCalibrationElement->GetAttribute("ObjectMarkerCoordinateFrame");
  if (objectMarkerCoordinateFrame == NULL)
  {
	  LOG_ERROR("ObjectMarkerCoordinateFrame is not specified in vtkPivotCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetObjectMarkerCoordinateFrame(objectMarkerCoordinateFrame);

  // Reference coordinate frame name
  const char* referenceCoordinateFrame = pivotCalibrationElement->GetAttribute("ReferenceCoordinateFrame");
  if (referenceCoordinateFrame == NULL)
  {
	  LOG_ERROR("ReferenceCoordinateFrame is not specified in vtkPivotCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetReferenceCoordinateFrame(referenceCoordinateFrame);

  // Object pivot point coordinate frame name
  const char* objectPivotPointCoordinateFrame = pivotCalibrationElement->GetAttribute("ObjectPivotPointCoordinateFrame");
  if (objectPivotPointCoordinateFrame == NULL)
  {
	  LOG_ERROR("ObjectPivotPointCoordinateFrame is not specified in vtkPivotCalibrationAlgo element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetObjectPivotPointCoordinateFrame(objectPivotPointCoordinateFrame);

  return PLUS_SUCCESS;
}
