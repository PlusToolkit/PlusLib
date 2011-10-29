/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkTrackerTool.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTrackerBuffer.h"
#include "vtkObjectFactory.h"
#include "vtkXMLUtilities.h"

//----------------------------------------------------------------------------
vtkTrackerTool* vtkTrackerTool::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTrackerTool");
	if(ret)
	{
		return (vtkTrackerTool*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkTrackerTool;
}

//----------------------------------------------------------------------------
vtkTrackerTool::vtkTrackerTool()
{  
	this->Tracker = 0;
	this->CalibrationMatrix = vtkMatrix4x4::New();

	this->LED1 = 0;
	this->LED2 = 0;
	this->LED3 = 0;

	this->ToolType = TRACKER_TOOL_NONE;
	this->ToolRevision = 0;
	this->ToolSerialNumber = 0;
	this->ToolPartNumber = 0;
	this->ToolManufacturer = 0;
	this->ToolName = 0;
	this->ToolModel = 0;
	this->Tool3DModelFileName = 0;
	this->ToolPort = 0;

	this->CalibrationMatrixName = 0; 
	this->CalibrationDate = 0; 
	this->CalibrationError = 0.0; 

	this->SendToLink = 0; 

	// Set tool enabled off by default 
	this->EnabledOff(); 

	this->ModelToToolTransform = vtkTransform::New();
	this->ModelToToolTransform->Identity();
	this->SetToolName("");
	this->SetTool3DModelFileName("");
	this->SetToolModel("");
	this->SetCalibrationDate("");

	this->Buffer = vtkTrackerBuffer::New();
	this->Buffer->SetToolCalibrationMatrix(this->CalibrationMatrix);
	
	this->FrameNumber = 0;
}

//----------------------------------------------------------------------------
vtkTrackerTool::~vtkTrackerTool()
{
	this->CalibrationMatrix->Delete();

	this->SetTool3DModelFileName(NULL); 
	this->SetToolRevision(NULL); 
	this->SetToolSerialNumber(NULL); 
	this->SetToolManufacturer(NULL); 

	this->SetModelToToolTransform(NULL); 
	this->SetToolModel(NULL); 
	this->SetTool3DModelFileName(NULL); 
	
	if ( this->Buffer )
	{
		this->Buffer->Delete(); 
	}
}

//----------------------------------------------------------------------------
void vtkTrackerTool::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkObject::PrintSelf(os,indent);

	os << indent << "Tracker: " << this->Tracker << "\n";
	os << indent << "ToolPort: " << this->ToolPort << "\n";
	os << indent << "LED1: " << this->GetLED1() << "\n"; 
	os << indent << "LED2: " << this->GetLED2() << "\n"; 
	os << indent << "LED3: " << this->GetLED3() << "\n";
	os << indent << "SendTo: " << this->GetSendToLink() << "\n";
	os << indent << "ToolType: " << this->GetToolType() << "\n";
	os << indent << "ToolModel: " << this->GetToolModel() << "\n";
	os << indent << "ModelToToolTransform: " << this->GetModelToToolTransform() << "\n";
	os << indent << "ToolRevision: " << this->GetToolRevision() << "\n";
	os << indent << "ToolManufacturer: " << this->GetToolManufacturer() << "\n";
	os << indent << "ToolPartNumber: " << this->GetToolPartNumber() << "\n";
	os << indent << "ToolSerialNumber: " << this->GetToolSerialNumber() << "\n";
	os << indent << "CalibrationMatrix: " << this->CalibrationMatrix << "\n";
	this->CalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
	os << indent << "Buffer: " << this->Buffer << "\n";
	this->Buffer->PrintSelf(os,indent.GetNextIndent());
}

//----------------------------------------------------------------------------
void vtkTrackerTool::SetLED1(int state)
{
	this->Tracker->SetToolLED(this->ToolPort,1,state);
}

//----------------------------------------------------------------------------
void vtkTrackerTool::SetLED2(int state)
{
	this->Tracker->SetToolLED(this->ToolPort,2,state);
}

//----------------------------------------------------------------------------
void vtkTrackerTool::SetLED3(int state)
{
	this->Tracker->SetToolLED(this->ToolPort,3,state);
}

//----------------------------------------------------------------------------
void vtkTrackerTool::SetTracker(vtkTracker *tracker)
{
	// The Tracker is not reference counted, since that would cause a reference loop
	if (tracker == this->Tracker)
	{
		return;
	}

	if (this->Tracker)
	{
		this->Buffer->SetWorldCalibrationMatrix(NULL);
		this->Tracker = NULL;
	}

	if (tracker)
	{
		this->Tracker = tracker;
		this->Buffer->SetWorldCalibrationMatrix(tracker->GetWorldCalibrationMatrix());
	}
	else
	{
		this->Tracker = NULL;
	}

	this->Modified();
}

//----------------------------------------------------------------------------
void vtkTrackerTool::DeepCopy(vtkTrackerTool *tool)
{
	LOG_TRACE("vtkTrackerTool::DeepCopy"); 

	this->CalibrationMatrix->DeepCopy( tool->GetCalibrationMatrix() ); 

	this->SetLED1( tool->GetLED1() );
	this->SetLED2( tool->GetLED2() );
	this->SetLED3( tool->GetLED3() );

	this->SetSendToLink( tool->GetSendToLink() );

	this->SetToolType( tool->GetToolType() );
	this->SetToolModel( tool->GetToolModel() );
	this->SetTool3DModelFileName( tool->GetTool3DModelFileName() );
	this->SetToolRevision( tool->GetToolRevision() );
	this->SetToolSerialNumber( tool->GetToolSerialNumber() );
	this->SetToolPartNumber( tool->GetToolPartNumber() );
	this->SetToolManufacturer( tool->GetToolManufacturer() );
	this->SetToolName( tool->GetToolName() ); 

	this->ModelToToolTransform->DeepCopy( tool->GetModelToToolTransform() );

	this->SetCalibrationMatrix( tool->GetCalibrationMatrix() ); 
	this->SetCalibrationMatrixName( tool->GetCalibrationMatrixName() ); 
	this->SetCalibrationDate( tool->GetCalibrationDate() ); 
	this->SetCalibrationError( tool->GetCalibrationError() ); 

	this->SetEnabled( tool->GetEnabled() ); 

	this->Buffer->DeepCopy( tool->GetBuffer() );

	this->SetFrameNumber( tool->GetFrameNumber() );
}


//-----------------------------------------------------------------------------
PlusStatus vtkTrackerTool::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkTrackerTool::ReadConfiguration"); 

  // Parameter XMLDataElement is the Tool data element, not the root!
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure tracker tool! (XML data element is NULL)"); 
		return PLUS_FAIL; 
	}

	const char* toolName = config->GetAttribute("Name"); 
	if ( toolName != NULL ) 
	{
		this->SetToolName(toolName); 
	}
	else
	{
		LOG_ERROR("Unable to find tool name! Name attribute is mandatory in tool definition."); 
		return PLUS_FAIL; 
	}

	const char* typeString = config->GetAttribute("Type"); 
	if ( typeString != NULL ) 
	{
		TRACKER_TOOL_TYPE type;
    if (vtkTracker::ConvertStringToToolType(typeString, type) != PLUS_SUCCESS) {
      return PLUS_FAIL;
    }
		this->SetToolType(type);
	}
	else
	{
		LOG_ERROR("Unable to find tool type! Type attribute is mandatory in tool definition."); 
		return PLUS_FAIL; 
	}

	const char* sendToLink = config->GetAttribute("SendTo"); 
	if ( sendToLink != NULL ) 
	{
		this->SetSendToLink(sendToLink); 
	}

	vtkSmartPointer<vtkXMLDataElement> toolCalibrationDataElement = config->FindNestedElementWithName("Calibration"); 
	if ( toolCalibrationDataElement != NULL ) 
	{
		const char* matrixName = toolCalibrationDataElement->GetAttribute("MatrixName"); 
		if ( matrixName != NULL ) 
		{
			this->SetCalibrationMatrixName(matrixName); 
		}

		const char* calibrationDate = toolCalibrationDataElement->GetAttribute("Date"); 
		if ( calibrationDate != NULL ) 
		{
			this->SetCalibrationDate(calibrationDate); 
		}

    if ((calibrationDate != NULL) && (STRCASECMP(calibrationDate, "") != 0)) {
		  double calibrationMatrixValue[16] = {0}; 
		  if ( toolCalibrationDataElement->GetVectorAttribute("MatrixValue", 16, calibrationMatrixValue ) )
		  {
			  this->CalibrationMatrix->DeepCopy(calibrationMatrixValue); 
		  }

      double calibrationError(0); 
		  if ( toolCalibrationDataElement->GetScalarAttribute("Error", calibrationError) )
		  {
			  this->SetCalibrationError(calibrationError); 
		  }
    }
    else
    {
      this->CalibrationMatrix->Identity();

      LOG_INFO("Tool calibration matrix cannot be loaded with no date entered - using identity matrix");
    }
	}

	vtkSmartPointer<vtkXMLDataElement> modelDataElement = config->FindNestedElementWithName("Model"); 
	if ( modelDataElement != NULL ) 
	{
		const char* file = modelDataElement->GetAttribute("File");
    if ( (file != NULL) && (STRCASECMP(file, "") != 0) )
		{
			this->SetTool3DModelFileName(file);
		}
    else
    {
      LOG_WARNING("'" << typeString << "' CAD model has not been defined");
    }

    // ModelToToolTransform stays identity if no model file has been found
		double modelToToolTransformMatrixValue[16] = {0};
		if ( (modelDataElement->GetVectorAttribute("ModelToToolTransform", 16, modelToToolTransformMatrixValue )) && (file != NULL) && (STRCASECMP(file, "") != 0) )
		{
			this->ModelToToolTransform->SetMatrix(modelToToolTransformMatrixValue);
		}
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTrackerTool::WriteConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkTrackerTool::WriteConfiguration"); 

  if ( config == NULL )
  {
    LOG_ERROR("Unable to write calibration result: xml data element is NULL!"); 
    return PLUS_FAIL;
  }

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }
	
  std::string toolType; 
  if ( vtkTracker::ConvertToolTypeToString(this->GetToolType(), toolType) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool type in string format!"); 
    return PLUS_FAIL; 
  }

  // Find tracker tool with type and name
  vtkSmartPointer<vtkXMLDataElement> trackerTool = trackerConfig->FindNestedElementWithNameAndAttribute("Tool", "Type", toolType.c_str() );
	if (trackerTool == NULL) 
  {
    LOG_ERROR("Unable to find tracker tool configuration file for type: " << toolType);
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> tool; 
  vtkSmartPointer<vtkXMLDataElement> calibration = trackerTool->FindNestedElementWithName("Calibration"); 
	if ( calibration == NULL ) 
  {
    // create new element and add to trackerTool 
    calibration = vtkSmartPointer<vtkXMLDataElement>::New(); 
    calibration->SetName("Calibration"); 
    calibration->SetParent(trackerTool); 
    trackerTool->AddNestedElement(calibration); 
  }

  // Set matrix name 
  calibration->SetAttribute("MatrixName", this->GetCalibrationMatrixName()); 

  // Set calibration matrix value
  double matrixValue[16] = {0}; 
  vtkMatrix4x4::DeepCopy(matrixValue, this->GetCalibrationMatrix() ); 
  calibration->SetVectorAttribute("MatrixValue",16, matrixValue); 

  // Set calibration date
  calibration->SetAttribute("Date", this->GetCalibrationDate()); 

  // Set calibration date
  calibration->SetDoubleAttribute("Error", this->GetCalibrationError()); 

  return PLUS_SUCCESS; 
}
