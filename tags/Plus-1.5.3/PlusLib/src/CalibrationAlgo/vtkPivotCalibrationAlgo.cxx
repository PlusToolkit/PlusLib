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
  this->CalibrationArray = NULL;
  this->ObjectMarkerCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;
  this->ObjectPivotPointCoordinateFrame = NULL;

  vtkSmartPointer<vtkAmoebaMinimizer> minimizer = vtkSmartPointer<vtkAmoebaMinimizer>::New();
  this->SetMinimizer(minimizer);

  vtkSmartPointer<vtkDoubleArray> calibrationArray = vtkSmartPointer<vtkDoubleArray>::New();
  this->SetCalibrationArray(calibrationArray);
  this->CalibrationArray->SetNumberOfComponents(16);

  this->PivotPointPosition[0] = 0.0;
  this->PivotPointPosition[1] = 0.0;
  this->PivotPointPosition[2] = 0.0;

  Initialize();
}

//-----------------------------------------------------------------------------

vtkPivotCalibrationAlgo::~vtkPivotCalibrationAlgo()
{
  this->SetPivotPointToMarkerTransformMatrix(NULL);
  this->SetCalibrationArray(NULL);
  this->SetMinimizer(NULL);
}

//----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::Initialize()
{
	LOG_TRACE("vtkPivotCalibrationAlgo::Initialize");

	this->CalibrationArray->SetNumberOfTuples(0);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::InsertNextCalibrationPoint(vtkSmartPointer<vtkMatrix4x4> aMarkerToReferenceTransformMatrix)
{
	//LOG_TRACE("vtkPivotCalibrationAlgo::InsertNextCalibrationPoint");

  this->CalibrationArray->InsertNextTuple(*aMarkerToReferenceTransformMatrix->Element);

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

  vtkSmartPointer<vtkMatrix4x4> resultMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	resultMatrix->SetElement(0,3,x);
	resultMatrix->SetElement(1,3,y);
	resultMatrix->SetElement(2,3,z);

	this->SetPivotPointToMarkerTransformMatrix(resultMatrix);

  // Compute a tooltip position based on the first acquired position
	double firstInputMatrixElements[16];
  this->CalibrationArray->GetTuple(0, firstInputMatrixElements);

  vtkSmartPointer<vtkMatrix4x4> firstInputTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  firstInputTransformMatrix->DeepCopy(firstInputMatrixElements);

  vtkSmartPointer<vtkTransform> pivotPointToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
  pivotPointToReferenceTransform->Identity();
  pivotPointToReferenceTransform->Concatenate(firstInputTransformMatrix);
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
	int n = self->CalibrationArray->GetNumberOfTuples();

	double x = self->Minimizer->GetParameterValue("x");
	double y = self->Minimizer->GetParameterValue("y");
	double z = self->Minimizer->GetParameterValue("z");
	double nx,ny,nz,sx,sy,sz,sxx,syy,szz;

	double matrix[4][4];

	sx = sy = sz = 0.0;
	sxx = syy = szz = 0.0;

	for (i = 0; i < n; i++)
	{
		self->CalibrationArray->GetTuple(i,*matrix);

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
