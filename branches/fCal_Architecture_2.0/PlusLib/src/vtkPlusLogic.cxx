/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerColorLogic.h"

#include "vtkUSTemplateCalibrationLogic.h"

//TODO: (Heffter) Remove it
#include "vtkBMPReader.h"
#include "vtkBMPWriter.h"
#include "vtkImageFlip.h"


static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkUSTemplateCalibrationLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkUSTemplateCalibrationLogic);

//---------------------------------------------------------------------------
vtkUSTemplateCalibrationLogic::vtkUSTemplateCalibrationLogic()
{

	// Timer Handling

	this->DataCallbackCommand = vtkCallbackCommand::New();
	this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
	this->DataCallbackCommand->SetCallback(vtkUSTemplateCalibrationLogic::DataCallback);

	this->USTemplateCalibrationNode = NULL; 

	this->CalibrationManager = NULL;

}


//---------------------------------------------------------------------------
vtkUSTemplateCalibrationLogic::~vtkUSTemplateCalibrationLogic()
{

	if (this->DataCallbackCommand != NULL)
	{
		this->DataCallbackCommand->Delete();
		this->DataCallbackCommand = NULL;
	}

	if ( this->CalibrationManager != NULL) 
	{
		this->CalibrationManager->Delete();
		this->CalibrationManager = NULL;
	}

}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
	this->vtkObject::PrintSelf(os, indent);
	os << indent << "vtkUSTemplateCalibrationLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationLogic::DataCallback(vtkObject *caller, 
												 unsigned long eid, void *clientData, void *callData)
{
	vtkUSTemplateCalibrationLogic *self = reinterpret_cast<vtkUSTemplateCalibrationLogic *>(clientData);
	vtkDebugWithObjectMacro(self, "In vtkUSTemplateCalibrationLogic DataCallback");
	self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationLogic::UpdateAll()
{

}

void vtkUSTemplateCalibrationLogic::Apply()
{

	// check if MRML node is present 
	if (this->USTemplateCalibrationNode == NULL)
	{
		vtkErrorMacro("No input ExampleLoadableModuleNode found");
		return;
	}

	// find input volume
	vtkMRMLScalarVolumeNode *inVolume = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->USTemplateCalibrationNode->GetInputVolumeRef()));
	if (inVolume == NULL)
	{
		vtkErrorMacro("No input volume found");
		return;
	}

	// find output volume
	vtkMRMLScalarVolumeNode *outVolume =  vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->USTemplateCalibrationNode->GetOutputVolumeRef()));
	if (outVolume == NULL)
	{
		vtkErrorMacro("No output volume found with id= " << this->USTemplateCalibrationNode->GetOutputVolumeRef());
		return;
	}

	// copy RASToIJK matrix, and other attributes from input to output
	std::string name (outVolume->GetName());
	std::string id (outVolume->GetID());

	outVolume->CopyOrientation(inVolume);
	outVolume->SetAndObserveTransformNodeID(inVolume->GetTransformNodeID());

	outVolume->SetName(name.c_str());
	//outVolume->SetID(id.c_str());

	// set ouput of the filter to VolumeNode's ImageData
	outVolume->SetAndObserveImageData(inVolume->GetImageData());
	outVolume->SetModifiedSinceRead(1);

} 

vtkMRMLScalarVolumeNode *vtkUSTemplateCalibrationLogic::AddVolumeToScene(vtkSlicerApplication* app, const char *fileName)
{
	if (fileName==0)
	{
		vtkErrorMacro("AddVolumeToScene: invalid filename");
		return 0;
	}
	
	if (this->CalibrationManager == NULL) 
	{
		this->CalibrationManager = vtkCalibrationManager::New();
	}

	vtksys_stl::string volumeNameString = vtksys::SystemTools::GetFilenameName(fileName);
	//vtkMRMLScalarVolumeNode *volumeNode = this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());
	vtkMRMLScalarVolumeNode *volumeNode = this->AddVolume(app, fileName, volumeNameString.c_str());

	//this->SetAutoScaleScalarVolume(volumeNode);
	//this->SetSliceViewFromVolume(app, volumeNode);

	volumeNode->Modified();
	this->Modified();

	return volumeNode;
}

void vtkUSTemplateCalibrationLogic::UpdateDisplay()
{

	if (this->CalibrationManager != NULL) 
	{
		this->CalibrationManager->UpdateFrameData(); 
	}

}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkUSTemplateCalibrationLogic::AddVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName)
{
	vtkMRMLScene* scene = this->GetMRMLScene();

	vtkSmartPointer<vtkMRMLScalarVolumeNode> volumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
	vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> displayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();   

	scene->SaveStateForUndo();
	scene->AddNode(volumeNode);
	scene->AddNode(displayNode);  

	volumeNode->SetScene(scene);
	volumeNode->SetName(fileName);

	displayNode->SetScene(scene);

	volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());


	vtkSmartPointer<vtkBMPReader> reader= vtkSmartPointer<vtkBMPReader>::New();
	reader->SetFileName(fileName); 
	reader->Update(); 


	vtkSmartPointer<vtkImageFlip> flip= vtkSmartPointer<vtkImageFlip>::New();
	flip->SetInput(this->CalibrationManager->GetVideoSource()->GetOutput()); 
	flip->SetFilteredAxis(1); 

	vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>::New();
	img = flip->GetOutput(); 
	
	img->SetDimensions(640, 480, 0); 
	//img->SetExtent(0 ,639, 0, 479, 0, 0); 
	
	//volumeNode->SetAndObserveImageData(img); 
	//volumeNode->SetAndObserveImageData(reader->GetOutput());
	this->CalibrationManager->GetVideoSource()->GetOutput()->Update(); 
	this->CalibrationManager->UpdateFrameData(); 
	volumeNode->SetAndObserveImageData(this->CalibrationManager->GetFrameData());
	

	displayNode->SetImageData(volumeNode->GetImageData()); 
	displayNode->VisibilityOn(); 
	displayNode->SetAutoWindowLevel(false);
	displayNode->SetInterpolate(true);  

	vtkSmartPointer<vtkSlicerColorLogic> colorLogic = vtkSmartPointer<vtkSlicerColorLogic>::New(); 
	displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());

	this->CalibrationManager->UpdateFrameData(); 
	vtkSmartPointer<vtkBMPWriter> w = vtkSmartPointer<vtkBMPWriter>::New();
	w->SetInput(this->CalibrationManager->GetFrameData()); 
	//w->SetInput(this->CalibrationManager->GetVideoSource()->GetOutput()); 
	w->SetFileName("SonixRP_Output.bmp"); 
	w->Update(); 
	
	return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkUSTemplateCalibrationLogic::AddArchetypeVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName)
{
	// Set up storageNode
	vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> storageNode = vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New(); 
	storageNode->SetFileName(fileName);
	// check to see if can read this type of file
	if (storageNode->SupportedFileType(fileName) == 0)
	{
		vtkErrorMacro("AddArchetypeVolume: can't read this kind of file: " << fileName);
		return 0;
	}
	storageNode->SetCenterImage(false);
	storageNode->SetSingleFile(false);
	storageNode->SetUseOrientationFromFile(true);

	// Set up scalarNode
	vtkSmartPointer<vtkMRMLScalarVolumeNode> scalarNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
	scalarNode->SetName(volumeName);
	scalarNode->SetLabelMap(false);

	// Set up displayNode
	vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> displayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();   
	displayNode->SetAutoWindowLevel(false);
	displayNode->SetInterpolate(true);  
	vtkSmartPointer<vtkSlicerColorLogic> colorLogic = vtkSmartPointer<vtkSlicerColorLogic>::New(); 
	displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());

	// Add nodes to scene
	this->GetMRMLScene()->SaveStateForUndo();  
	vtkDebugMacro("LoadArchetypeVolume: adding storage node to the scene");
	storageNode->SetScene(this->GetMRMLScene());
	this->GetMRMLScene()->AddNode(storageNode);
	vtkDebugMacro("LoadArchetypeVolume: adding display node to the scene");
	displayNode->SetScene(this->GetMRMLScene());
	this->GetMRMLScene()->AddNode(displayNode);
	vtkDebugMacro("LoadArchetypeVolume: adding scalar node to the scene");
	scalarNode->SetScene(this->GetMRMLScene());
	this->GetMRMLScene()->AddNode(scalarNode);
	scalarNode->SetAndObserveStorageNodeID(storageNode->GetID());
	scalarNode->SetAndObserveDisplayNodeID(displayNode->GetID());

	// Read the volume into the node
	vtkDebugMacro("AddArchetypeVolume: about to read data into scalar node " << scalarNode->GetName());
	storageNode->AddObserver(vtkCommand::ProgressEvent, this->LogicCallbackCommand);
	if (this->GetDebug())
	{
		storageNode->DebugOn();
	}
	storageNode->ReadData(scalarNode);
	vtkDebugMacro("AddArchetypeVolume: finished reading data into scalarNode");
	storageNode->RemoveObservers(vtkCommand::ProgressEvent, this->LogicCallbackCommand);

	return scalarNode;
}





