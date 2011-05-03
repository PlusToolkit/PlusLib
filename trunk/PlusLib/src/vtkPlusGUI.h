/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkUSTemplateCalibrationGUI_h
#define __vtkUSTemplateCalibrationGUI_h

#ifdef WIN32
#include "vtkUSTemplateCalibrationWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkUSTemplateCalibrationLogic.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;

class VTK_EXPORT vtkUSTemplateCalibrationGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkUSTemplateCalibrationGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkUSTemplateCalibrationLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

   // Description: Get/Set MRML node
  vtkGetObjectMacro (USTemplateCalibrationNode, vtkMRMLUSTemplateCalibrationNode); 

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkUSTemplateCalibrationGUI ( );
  virtual ~vtkUSTemplateCalibrationGUI ( );

 private:
  vtkUSTemplateCalibrationGUI ( const vtkUSTemplateCalibrationGUI& ); // Not implemented.
  void operator = ( const vtkUSTemplateCalibrationGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkUSTemplateCalibrationGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  void AddLogicObservers ( );
  void RemoveLogicObservers ( );

  //----------------------------------------------------------------
  // Event Handlers
  //----------------------------------------------------------------

  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForImageAcquisition();
  void BuildGUIForCalibration();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

  void UpdateMRML();

  void UpdateGUI();


 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------
  vtkSlicerNodeSelectorWidget* InVolumeSelector;
  vtkSlicerNodeSelectorWidget* OutVolumeSelector;
  vtkSlicerNodeSelectorWidget* USTCalibNodeSelector;

  vtkKWLoadSaveButton* LoadVolumeButton;
  vtkKWPushButton* TestButton12;
  vtkKWPushButton* TestButton21;
  vtkKWPushButton* TestButton22;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkUSTemplateCalibrationLogic *Logic;
  vtkMRMLUSTemplateCalibrationNode* USTemplateCalibrationNode;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
