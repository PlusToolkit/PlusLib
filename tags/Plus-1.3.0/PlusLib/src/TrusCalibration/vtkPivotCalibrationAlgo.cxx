/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPivotCalibrationAlgo.h"

#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx" 

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPivotCalibrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPivotCalibrationAlgo);

//-----------------------------------------------------------------------------

vtkPivotCalibrationAlgo::vtkPivotCalibrationAlgo()
{
	this->TooltipToToolTransform = NULL;
	this->CalibrationError = -1.0;
  this->Minimizer = NULL;
  this->CalibrationArray = NULL;

  vtkSmartPointer<vtkAmoebaMinimizer> minimizer = vtkSmartPointer<vtkAmoebaMinimizer>::New();
  this->SetMinimizer(minimizer);

  vtkSmartPointer<vtkDoubleArray> calibrationArray = vtkSmartPointer<vtkDoubleArray>::New();
  this->SetCalibrationArray(calibrationArray);
  this->CalibrationArray->SetNumberOfComponents(16);

  this->TooltipPosition[0] = 0.0;
  this->TooltipPosition[1] = 0.0;
  this->TooltipPosition[2] = 0.0;

  Initialize();
}

//-----------------------------------------------------------------------------

vtkPivotCalibrationAlgo::~vtkPivotCalibrationAlgo()
{
  this->SetTooltipToToolTransform(NULL);
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

PlusStatus vtkPivotCalibrationAlgo::InsertNextCalibrationPoint(vtkSmartPointer<vtkMatrix4x4> aToolToReferenceTransformMatrix)
{
	//LOG_TRACE("vtkPivotCalibrationAlgo::InsertNextCalibrationPoint");

  this->CalibrationArray->InsertNextTuple(*aToolToReferenceTransformMatrix->Element);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::DoTooltipCalibration()
{
  LOG_TRACE("vtkPivotCalibrationAlgo::DoToolTipCalibration");

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

  vtkSmartPointer<vtkTransform> tooltipToToolTransform = vtkSmartPointer<vtkTransform>::New();
  tooltipToToolTransform->SetMatrix(resultMatrix);
	this->SetTooltipToToolTransform(tooltipToToolTransform);

  // Compute a tooltip position based on the first acquired position
	double firstInputMatrixElements[16];
  this->CalibrationArray->GetTuple(0, firstInputMatrixElements);

  vtkSmartPointer<vtkTransform> firstInputTransform = vtkSmartPointer<vtkTransform>::New();
  firstInputTransform->SetMatrix(firstInputMatrixElements);

  vtkSmartPointer<vtkTransform> tooltipToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
  tooltipToReferenceTransform->Identity();
  tooltipToReferenceTransform->Concatenate(firstInputTransform);
  tooltipToReferenceTransform->Concatenate(this->TooltipToToolTransform);

  tooltipToReferenceTransform->GetPosition(this->TooltipPosition);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

std::string vtkPivotCalibrationAlgo::GetTooltipToToolTranslationString()
{
	//LOG_TRACE("vtkPivotCalibrationAlgo::GetTooltipToToolTranslationString");

  if (this->TooltipToToolTransform == NULL) {
    LOG_ERROR("Tooltip to tool transform is not initialized!");
    return "";
  }

	vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
	this->TooltipToToolTransform->GetMatrix(transform);

	char stylustipToStylusTranslationChars[32];
	sprintf_s(stylustipToStylusTranslationChars, 32, "%.2lf X %.2lf X %.2lf", transform->GetElement(0,3), transform->GetElement(1,3), transform->GetElement(2,3));
	std::string stylustipToStylusTranslationString(stylustipToStylusTranslationChars);

	return stylustipToStylusTranslationString;
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

PlusStatus vtkPivotCalibrationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig, TRACKER_TOOL_TYPE aType)
{
	LOG_TRACE("vtkPivotCalibrationAlgo::ReadConfiguration(" << aType << ")");

	// Find tool definition element
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(aType, toolType);
  vtkSmartPointer<vtkXMLDataElement> toolDefinition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType.c_str());
	if (toolDefinition == NULL) {
    LOG_ERROR("No tool definition is found in the XML tree with type: " << aType);
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = toolDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in tool definition!");
		return PLUS_FAIL;
	}

  // Check date - if it is empty, the calibration is considered as invalid (as the calibration transforms in the installed config files are identity matrices with empty dates)
  const char* date = calibration->GetAttribute("Date");
  if ((date == NULL) || (STRCASECMP(date, "") == 0)) {
    LOG_WARNING("Transform cannot be loaded with no date entered!");
    return PLUS_FAIL;
  }

	// Get transform
	double* tooltipToToolTransformVector = new double[16]; 
	if (calibration->GetVectorAttribute("MatrixValue", 16, tooltipToToolTransformVector)) {
    vtkSmartPointer<vtkTransform> tooltipToToolTransform = vtkSmartPointer<vtkTransform>::New();
    tooltipToToolTransform->Identity();
    tooltipToToolTransform->SetMatrix(tooltipToToolTransformVector);
    this->SetTooltipToToolTransform(tooltipToToolTransform);
  }
	delete[] tooltipToToolTransformVector; 

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPivotCalibrationAlgo::WriteConfiguration(vtkXMLDataElement* aConfig, TRACKER_TOOL_TYPE aType)
{
	LOG_TRACE("vtkPivotCalibrationAlgo::WriteConfiguration(" << aType << ")");

	// Find tool definition element
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(aType, toolType);
	vtkSmartPointer<vtkXMLDataElement> toolDefinition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType.c_str());
	if (toolDefinition == NULL) {
    LOG_ERROR("No tool definition is found in the XML tree with type: " << aType);
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = toolDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in tool definition!");
		return PLUS_FAIL;
	}

	// Assemble and save transform
	char tooltipToToolTransformChars[256];
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = this->TooltipToToolTransform->GetMatrix();
	sprintf_s(tooltipToToolTransformChars, 256, "\n\t1 0 0 %.4lf\n\t0 1 0 %.4lf\n\t0 0 1 %.4lf\n\t0 0 0 1", transformMatrix->GetElement(0,3), transformMatrix->GetElement(1,3), transformMatrix->GetElement(2,3));

	calibration->SetAttribute("MatrixValue", tooltipToToolTransformChars);

	// Save matrix name, date and error
	calibration->SetAttribute("MatrixName", "TooltipToTool");
	calibration->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());
	calibration->SetDoubleAttribute("Error", CalibrationError);

	return PLUS_SUCCESS;
}
