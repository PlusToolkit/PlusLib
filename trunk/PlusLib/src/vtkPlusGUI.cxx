/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkUSTemplateCalibrationGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkUSTemplateCalibrationGUI );
vtkCxxRevisionMacro ( vtkUSTemplateCalibrationGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkUSTemplateCalibrationGUI::vtkUSTemplateCalibrationGUI ( )
{

	//----------------------------------------------------------------
	// Logic values
	this->Logic = NULL;
	this->USTemplateCalibrationNode = NULL; 
	this->DataCallbackCommand = vtkCallbackCommand::New();
	this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
	this->DataCallbackCommand->SetCallback(vtkUSTemplateCalibrationGUI::DataCallback);

	//----------------------------------------------------------------
	// GUI widgets
	this->LoadVolumeButton = NULL;
	this->TestButton12 = NULL;
	this->TestButton21 = NULL;
	this->TestButton22 = NULL;
	this->USTCalibNodeSelector = vtkSlicerNodeSelectorWidget::New(); 
	this->OutVolumeSelector = vtkSlicerNodeSelectorWidget::New(); 
	this->InVolumeSelector = vtkSlicerNodeSelectorWidget::New(); 

	//----------------------------------------------------------------
	// Locator  (MRML)
	this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkUSTemplateCalibrationGUI::~vtkUSTemplateCalibrationGUI ( )
{

	//----------------------------------------------------------------
	// Remove Callbacks

	if (this->DataCallbackCommand)
	{
		this->DataCallbackCommand->Delete();
	}

	//----------------------------------------------------------------
	// Remove Observers

	this->RemoveGUIObservers();

	//----------------------------------------------------------------
	// Remove GUI widgets

	if (this->LoadVolumeButton)
	{
		this->LoadVolumeButton->SetParent(NULL);
		this->LoadVolumeButton->Delete();
	}

	if (this->TestButton12)
	{
		this->TestButton12->SetParent(NULL);
		this->TestButton12->Delete();
	}

	if (this->TestButton21)
	{
		this->TestButton21->SetParent(NULL);
		this->TestButton21->Delete();
	}

	if (this->TestButton22)
	{
		this->TestButton22->SetParent(NULL);
		this->TestButton22->Delete();
	}

	//----------------------------------------------------------------
	// Unregister Logic class

	this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::Enter()
{
	// Fill in
	//vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

	if (this->TimerFlag == 0)
	{
		this->TimerFlag = 1;
		this->TimerInterval = 100;  // 100 ms
		ProcessTimerEvents();
	}

	//------------------------------------------------------------------

	if ( this->GetApplicationGUI() != NULL )
	{
		vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
		p->RepackMainViewer ( vtkMRMLLayoutNode::SlicerLayoutOneUpSliceView, "Red");       
	}

	vtkMRMLUSTemplateCalibrationNode* n = this->GetUSTemplateCalibrationNode();;

	if (n == NULL)
	{
		// no parameter node selected yet, create new
		this->USTCalibNodeSelector->SetSelectedNew("vtkMRMLUSTemplateCalibrationNode");
		this->USTCalibNodeSelector->ProcessNewNodeCommand("vtkMRMLUSTemplateCalibrationNode", "USTC");
		n = vtkMRMLUSTemplateCalibrationNode::SafeDownCast(this->USTCalibNodeSelector->GetSelected());

		// set an observe new node in Logic
		this->Logic->SetAndObserveUSTemplateCalibrationNode(n);
		vtkSetAndObserveMRMLNodeMacro(this->USTemplateCalibrationNode, n);
		this->UpdateGUI();
	}


}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::Exit ( )
{
	// Fill in
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
	this->vtkObject::PrintSelf ( os, indent );

	os << indent << "USTemplateCalibrationGUI: " << this->GetClassName ( ) << "\n";
	os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::RemoveGUIObservers ( )
{
	//vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

	if (this->LoadVolumeButton)
	{
		this->LoadVolumeButton
			->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
	}

	if (this->TestButton12)
	{
		this->TestButton12
			->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
	}

	if (this->TestButton21)
	{
		this->TestButton21
			->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
	}

	if (this->TestButton22)
	{
		this->TestButton22
			->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
	}


	this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::AddGUIObservers ( )
{
	this->RemoveGUIObservers();

	//vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

	//----------------------------------------------------------------
	// MRML

	vtkIntArray* events = vtkIntArray::New();
	//events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
	//events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
	events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

	if (this->GetMRMLScene() != NULL)
	{
		this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
	}
	events->Delete();

	//----------------------------------------------------------------
	// GUI Observers

	this->TestButton12
		->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
	//this->TestButton11
	//	->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
	
	this->LoadVolumeButton->GetLoadSaveDialog()
      ->AddObserver( vtkKWTopLevel::WithdrawEvent,(vtkCommand *)this->GUICallbackCommand );

	this->TestButton21
		->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
	this->TestButton22
		->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
	this->USTCalibNodeSelector
		->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  
	this->OutVolumeSelector
		->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  
	this->InVolumeSelector
		->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

	this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::RemoveLogicObservers ( )
{
	if (this->GetLogic())
	{
		this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
			(vtkCommand *)this->LogicCallbackCommand);
	}
}




//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::AddLogicObservers ( )
{
	this->RemoveLogicObservers();  

	if (this->GetLogic())
	{
		this->GetLogic()->AddObserver(vtkUSTemplateCalibrationLogic::StatusUpdateEvent,
			(vtkCommand *)this->LogicCallbackCommand);
	}
}

//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::ProcessGUIEvents(vtkObject *caller,
												   unsigned long event, void *callData)
{

	const char *eventName = vtkCommand::GetStringFromEventId(event);

	if (strcmp(eventName, "LeftButtonPressEvent") == 0)
	{
		vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
		HandleMouseEvent(style);
		return;
	}

	vtkKWScaleWithEntry *s = vtkKWScaleWithEntry::SafeDownCast(caller);
	vtkKWMenu *v = vtkKWMenu::SafeDownCast(caller);
	vtkKWPushButton *button = vtkKWPushButton::SafeDownCast(caller);
	vtkSlicerNodeSelectorWidget *selector = vtkSlicerNodeSelectorWidget::SafeDownCast(caller);

	if ( this->LoadVolumeButton && this->LoadVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
    // load planning volume dialog button
    this->LoadVolumeButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "PSOpenPathPlanVol" );
    const char *fileName =
      this->LoadVolumeButton->GetLoadSaveDialog()->GetFileName();    
	if ( fileName ) 
	{
		std::string fileString(fileName);
		for (unsigned int i = 0; i < fileString.length(); i++)
		{
			if (fileString[i] == '\\')
			{
				fileString[i] = '/';
			}
		}
      this->LoadVolumeButton->GetLoadSaveDialog()->
        SaveLastPathToRegistry( "PSOpenPathPlanVol" );
      
        // call the callback function
	    vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetApplication());
		vtkMRMLScalarVolumeNode *volumeNode = this->GetLogic()->AddVolumeToScene(app,fileString.c_str());
		if (volumeNode)
		{

			vtkMRMLScalarVolumeDisplayNode *node = NULL;
			vtkSetAndObserveMRMLNodeMacro(node, volumeNode->GetScalarVolumeDisplayNode());

			this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
			this->GetApplicationLogic()->PropagateVolumeSelection();

		}

		//vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetApplication());
		//vtkMRMLScalarVolumeNode *volumeNode = this->GetLogic()->AddVolumeToScene(app,fileString.c_str());
		//if (volumeNode)
		//{

		//	vtkMRMLScalarVolumeDisplayNode *node = NULL;
		//	vtkSetAndObserveMRMLNodeMacro(node, volumeNode->GetScalarVolumeDisplayNode());

		//	this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
		//	//this->GetApplicationLogic()->GetActiveSlice()->SetSliceNode(
		//	this->GetApplicationLogic()->PropagateVolumeSelection();

		//	this->InVolumeSelector->SetSelected(volumeNode);
		//	const char *strName = this->InVolumeSelector->GetSelected()->GetName();
		//	std::string strPlan = std::string(strName) + "-Plan";
		//	this->InVolumeSelector->GetSelected()->SetName(strPlan.c_str());
		//	this->InVolumeSelector->GetSelected()->SetDescription("Planning image/volume; created by PerkStation module");
		//	this->InVolumeSelector->GetSelected()->Modified();
		//	this->InVolumeSelector->UpdateMenu();

		//}

      } 
    }
	else if (this->TestButton12 == vtkKWPushButton::SafeDownCast(caller)
		&& event == vtkKWPushButton::InvokedEvent)
	{
		this->GetLogic()->UpdateDisplay(); 
		//this->UpdateMRML();
		//this->Logic->Apply();
	}
	else if (this->TestButton21 == vtkKWPushButton::SafeDownCast(caller)
		&& event == vtkKWPushButton::InvokedEvent)
	{
		std::cerr << "TestButton21 is pressed." << std::endl;
	}
	else if (this->TestButton22 == vtkKWPushButton::SafeDownCast(caller)
		&& event == vtkKWPushButton::InvokedEvent)
	{
		std::cerr << "TestButton22 is pressed." << std::endl;
	}
	else if (selector == this->OutVolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
		this->OutVolumeSelector->GetSelected() != NULL) 
	{ 
		this->UpdateMRML();
	}
	else if (selector == this->InVolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
		this->InVolumeSelector->GetSelected() != NULL) 
	{ 
		this->UpdateMRML();
	}
	else if (selector == this->USTCalibNodeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
		this->USTCalibNodeSelector->GetSelected() != NULL) 
	{ 
		vtkMRMLUSTemplateCalibrationNode* n = vtkMRMLUSTemplateCalibrationNode::SafeDownCast(this->USTCalibNodeSelector->GetSelected());
		this->Logic->SetAndObserveUSTemplateCalibrationNode(n);
		vtkSetAndObserveMRMLNodeMacro( this->USTemplateCalibrationNode, n);
		this->UpdateGUI();
	}

} 

void vtkUSTemplateCalibrationGUI::DataCallback(vtkObject *caller, 
											   unsigned long eid, void *clientData, void *callData)
{
	vtkUSTemplateCalibrationGUI *self = reinterpret_cast<vtkUSTemplateCalibrationGUI *>(clientData);
	vtkDebugWithObjectMacro(self, "In vtkUSTemplateCalibrationGUI DataCallback");
	self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::ProcessLogicEvents ( vtkObject *caller,
													  unsigned long event, void *callData )
{

	if (this->GetLogic() == vtkUSTemplateCalibrationLogic::SafeDownCast(caller))
	{
		if (event == vtkUSTemplateCalibrationLogic::StatusUpdateEvent)
		{
			//this->UpdateDeviceStatus();
		}
	}
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::ProcessMRMLEvents ( vtkObject *caller,
													 unsigned long event, void *callData )
{
	// if parameter node has been changed externally, update GUI widgets with new values
	vtkMRMLUSTemplateCalibrationNode* node = vtkMRMLUSTemplateCalibrationNode::SafeDownCast(caller);
	if (node != NULL && this->GetUSTemplateCalibrationNode() == node) 
	{
		this->UpdateGUI();
	}
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::ProcessTimerEvents()
{
	if (this->TimerFlag)
	{

		this->GetLogic()->UpdateDisplay(); 

		// update timer
		vtkKWTkUtilities::CreateTimerHandler(this->GetApplication(), 
			this->TimerInterval,
			this, "ProcessTimerEvents");        
	}
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::BuildGUI ( )
{

	vtkMRMLUSTemplateCalibrationNode* CalibNode = vtkMRMLUSTemplateCalibrationNode::New();
	this->Logic->GetMRMLScene()->RegisterNodeClass(CalibNode);
	CalibNode->Delete();


	// ---
	// MODULE GUI FRAME 
	// create a page
	this->UIPanel->AddPage ( "USTemplateCalibration", "USTemplateCalibration", NULL );

	BuildGUIForHelpFrame();
	BuildGUIForImageAcquisition();
	BuildGUIForCalibration();

}


void vtkUSTemplateCalibrationGUI::BuildGUIForHelpFrame ()
{
	// Define your help text here.
	const char *help = 
		"See "
		"<a>http://www.slicer.org/slicerWiki/index.php/Modules:USTemplateCalibration</a> for details.";
	const char *about =
		"This work is supported by NCIGT, NA-MIC.";

	vtkKWWidget *page = this->UIPanel->GetPageWidget ( "USTemplateCalibration" );
	this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::BuildGUIForImageAcquisition()
{

	vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
	vtkKWWidget *page = this->UIPanel->GetPageWidget ("USTemplateCalibration");

	// -----------------------------------------
	// Image Acquisition frame
	vtkSlicerModuleCollapsibleFrame *imgAcquisitionFrame = vtkSlicerModuleCollapsibleFrame::New();

	imgAcquisitionFrame->SetParent(page);
	imgAcquisitionFrame->Create();
	imgAcquisitionFrame->SetLabelText("Image Acquisition");
	//imgAcquisitionFrame->CollapseFrame();
	app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
		imgAcquisitionFrame->GetWidgetName(), page->GetWidgetName());

	// -----------------------------------------
	// Image Acquisition child frame

	vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
	frame->SetParent(imgAcquisitionFrame->GetFrame());
	frame->Create();
	frame->SetLabelText ("Test child frame");
	this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
		frame->GetWidgetName() );

	// -----------------------------------------
	// Test push button

	//this->TestButton11 = vtkKWPushButton::New ( );
	//this->TestButton11->SetParent ( frame->GetFrame() );
	//this->TestButton11->Create ( );
	//this->TestButton11->SetText ("Load Volume");
	//this->TestButton11->SetWidth (12);

	this->LoadVolumeButton = vtkKWLoadSaveButton::New();
	this->LoadVolumeButton->SetParent(frame->GetFrame());
    this->LoadVolumeButton->Create();
    this->LoadVolumeButton->SetBorderWidth(2);
    this->LoadVolumeButton->SetReliefToRaised();       
    this->LoadVolumeButton->SetHighlightThickness(2);
    this->LoadVolumeButton->SetText("Load Volume");
    this->LoadVolumeButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadVolumeButton->TrimPathFromFileNameOn();
    this->LoadVolumeButton->SetMaximumFileNameLength(256);
    this->LoadVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode

	this->TestButton12 = vtkKWPushButton::New ( );
	this->TestButton12->SetParent ( frame->GetFrame() );
	this->TestButton12->Create ( );
	this->TestButton12->SetText ("Apply");
	this->TestButton12->SetWidth (12);

	this->Script("pack %s %s -side left -padx 2 -pady 2", 
		this->LoadVolumeButton->GetWidgetName(),
		this->TestButton12->GetWidgetName());

	this->USTCalibNodeSelector = vtkSlicerNodeSelectorWidget::New();
	this->USTCalibNodeSelector->SetNodeClass("vtkMRMLUSTemplateCalibrationNode", NULL, NULL, "USTCalibParameters");
	this->USTCalibNodeSelector->SetNewNodeEnabled(1);
	this->USTCalibNodeSelector->NoneEnabledOff();
	this->USTCalibNodeSelector->SetShowHidden(1);
	this->USTCalibNodeSelector->SetParent( frame->GetFrame() );
	this->USTCalibNodeSelector->Create();
	this->USTCalibNodeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
	this->USTCalibNodeSelector->UpdateMenu();

	this->USTCalibNodeSelector->SetBorderWidth(2);
	this->USTCalibNodeSelector->SetLabelText( "Node selector");
	this->USTCalibNodeSelector->SetBalloonHelpString("select a node from the current mrml scene.");
	app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
		this->USTCalibNodeSelector->GetWidgetName());


	this->InVolumeSelector = vtkSlicerNodeSelectorWidget::New();
	this->InVolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, "USTCalibVolumeIn");
	this->InVolumeSelector->SetNewNodeEnabled(1);
	this->USTCalibNodeSelector->NoneEnabledOff();
	this->InVolumeSelector->SetParent( frame->GetFrame() );
	this->InVolumeSelector->Create();
	this->InVolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
	this->InVolumeSelector->UpdateMenu();

	this->InVolumeSelector->SetBorderWidth(2);
	this->InVolumeSelector->SetLabelText( "Input Volume: ");
	this->InVolumeSelector->SetBalloonHelpString("select an output volume from the current mrml scene.");
	app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
		this->InVolumeSelector->GetWidgetName());


	this->OutVolumeSelector = vtkSlicerNodeSelectorWidget::New();
	this->OutVolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, "USTCalibVolumeOut");
	this->OutVolumeSelector->SetNewNodeEnabled(1);
	this->USTCalibNodeSelector->NoneEnabledOff();
	this->OutVolumeSelector->SetParent( frame->GetFrame() );
	this->OutVolumeSelector->Create();
	this->OutVolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
	this->OutVolumeSelector->UpdateMenu();

	this->OutVolumeSelector->SetBorderWidth(2);
	this->OutVolumeSelector->SetLabelText( "Output Volume: ");
	this->OutVolumeSelector->SetBalloonHelpString("select an output volume from the current mrml scene.");
	app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
		this->OutVolumeSelector->GetWidgetName());

	imgAcquisitionFrame->Delete();
	frame->Delete();

}


//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::BuildGUIForCalibration()
{
	vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
	vtkKWWidget *page = this->UIPanel->GetPageWidget ("USTemplateCalibration");

	// -----------------------------------------
	// Calibration frame
	vtkSlicerModuleCollapsibleFrame *calibFrame = vtkSlicerModuleCollapsibleFrame::New();

	calibFrame->SetParent(page);
	calibFrame->Create();
	calibFrame->SetLabelText("US to Template Calibration");
	calibFrame->CollapseFrame();
	app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
		calibFrame->GetWidgetName(), page->GetWidgetName());

	// -----------------------------------------
	// Calibration child frame

	vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
	frame->SetParent(calibFrame->GetFrame());
	frame->Create();
	frame->SetLabelText ("Test child frame");
	this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
		frame->GetWidgetName() );

	// -----------------------------------------
	// Test push button

	this->TestButton21 = vtkKWPushButton::New ( );
	this->TestButton21->SetParent ( frame->GetFrame() );
	this->TestButton21->Create ( );
	this->TestButton21->SetText ("Test 21");
	this->TestButton21->SetWidth (12);

	this->TestButton22 = vtkKWPushButton::New ( );
	this->TestButton22->SetParent ( frame->GetFrame() );
	this->TestButton22->Create ( );
	this->TestButton22->SetText ("Tset 22");
	this->TestButton22->SetWidth (12);

	this->Script("pack %s %s -side left -padx 2 -pady 2", 
		this->TestButton21->GetWidgetName(),
		this->TestButton22->GetWidgetName());


	calibFrame->Delete();
	frame->Delete();
}


//----------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::UpdateAll()
{
	this->UpdateMRML(); 
	this->UpdateGUI(); 
}

//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::UpdateMRML ()
{
	vtkMRMLUSTemplateCalibrationNode* n = this->GetUSTemplateCalibrationNode();
	if (n == NULL)
	{
		// no parameter node selected yet, create new
		this->USTCalibNodeSelector->SetSelectedNew("vtkMRMLUSTemplateCalibrationNode");
		this->USTCalibNodeSelector->ProcessNewNodeCommand("vtkMRMLUSTemplateCalibrationNode", "USTCalibParameters");
		n = vtkMRMLUSTemplateCalibrationNode::SafeDownCast(this->USTCalibNodeSelector->GetSelected());

		// set an observe new node in Logic
		this->Logic->SetAndObserveUSTemplateCalibrationNode(n);
		vtkSetAndObserveMRMLNodeMacro(this->USTemplateCalibrationNode, n);
	}

	// save node parameters for Undo
	this->GetLogic()->GetMRMLScene()->SaveStateForUndo(n);

	if (this->InVolumeSelector->GetSelected() != NULL)
    {
		n->SetInputVolumeRef(this->InVolumeSelector->GetSelected()->GetID());
    }

	if (this->OutVolumeSelector->GetSelected() != NULL)
	{
		n->SetOutputVolumeRef(this->OutVolumeSelector->GetSelected()->GetID());
	}
}

//---------------------------------------------------------------------------
void vtkUSTemplateCalibrationGUI::UpdateGUI ()
{
	//vtkMRMLUSTemplateCalibrationNode* n = this->GetUSTemplateCalibrationNode();
	//if (n != NULL)
	//{
	//	// set GUI widgest from parameter node
	//	//this->ConductanceScale->SetValue(n->GetConductance());

	//	//this->TimeStepScale->SetValue(n->GetTimeStep());

	//	//this->NumberOfIterationsScale->SetValue(n->GetNumberOfIterations());
	//}
}