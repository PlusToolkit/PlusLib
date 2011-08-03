/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

// .NAME vtkUSTemplateCalibrationLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkUSTemplateCalibrationLogic_h
#define __vtkUSTemplateCalibrationLogic_h

#include "vtkUSTemplateCalibrationWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLUSTemplateCalibrationNode.h"
#include "vtkCalibrationManager.h"

class vtkIGTLConnector;

class VTK_EXPORT vtkUSTemplateCalibrationLogic : public vtkSlicerModuleLogic 
{
public:
	//BTX
	enum {  // Events
		//LocatorUpdateEvent      = 50000,
		StatusUpdateEvent       = 50001,
	};
	//ETX

public:

	static vtkUSTemplateCalibrationLogic *New();

	vtkTypeRevisionMacro(vtkUSTemplateCalibrationLogic,vtkObject);
	void PrintSelf(ostream&, vtkIndent);

	vtkGetObjectMacro (USTemplateCalibrationNode, vtkMRMLUSTemplateCalibrationNode); 
	void SetAndObserveUSTemplateCalibrationNode(vtkMRMLUSTemplateCalibrationNode *n) 
	{
		vtkSetAndObserveMRMLNodeMacro( this->USTemplateCalibrationNode, n);
	} 

//	vtkCxxSetObjectMacro(vtkUSTemplateCalibrationLogic,CalibrationManager,vtkCalibrationManager);
//	vtkGetObjectMacro(CalibrationManager,vtkCalibrationManager);

	// The method that creates and runs VTK or ITK pipeline
	void Apply();

	vtkMRMLScalarVolumeNode *AddVolumeToScene(vtkSlicerApplication* app,const char *fileName); 
	vtkMRMLScalarVolumeNode *AddArchetypeVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName);  
	vtkMRMLScalarVolumeNode *AddVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName);  

	void UpdateDisplay(); 

protected:

	vtkUSTemplateCalibrationLogic();
	~vtkUSTemplateCalibrationLogic();

	void operator=(const vtkUSTemplateCalibrationLogic&);
	vtkUSTemplateCalibrationLogic(const vtkUSTemplateCalibrationLogic&);

	static void DataCallback(vtkObject*, unsigned long, void *, void *);
	void UpdateAll();

	vtkCallbackCommand *DataCallbackCommand;
	vtkMRMLUSTemplateCalibrationNode* USTemplateCalibrationNode;

	vtkCalibrationManager* CalibrationManager;

private:


};

#endif



