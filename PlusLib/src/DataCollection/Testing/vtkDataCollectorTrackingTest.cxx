/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file This a simple test program to acquire a tracking data and optionally writes the buffer to a metafile
  and display the live transform in a 3D view.
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkDataCollector.h"
#include "vtkTrackerBuffer.h"
#include "vtkXMLUtilities.h"
#include "vtkTimerLog.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"


#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkToolAxesActor.h" 
#include "vtkOrientationMarkerWidget.h" 
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkCamera.h"

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {return new vtkMyCallback;}

  vtkMyCallback()
  {
    this->TimerId=-1;
    this->StepperTextActor = vtkTextActor::New(); 
  }

  virtual ~vtkMyCallback()
  {
    this->StepperTextActor->Delete();
    this->StepperTextActor=NULL;
    for (int i=0; i<this->ToolActors.size(); i++)
    {
      //this->Renderer->RemoveActor(this->ToolActors[i]);
      this->ToolActors[i]->Delete();
      this->ToolActors[i]=NULL;
    }
  }

  void Init()
  {        

    // Create a text actor for tracking information
    vtkTextProperty* textprop = this->StepperTextActor->GetTextProperty();
    textprop->SetColor(1,0,0);
    textprop->SetFontFamilyToArial();
    textprop->SetFontSize(15);
    textprop->SetJustificationToLeft();
    textprop->SetVerticalJustificationToTop();
    this->StepperTextActor->VisibilityOn(); 
    this->StepperTextActor->SetDisplayPosition(20,65); 
    this->Renderer->AddActor(this->StepperTextActor); 

    for (int toolNumber=0; toolNumber<this->DataCollector->GetNumberOfTools(); toolNumber++)
    {
      AddNewToolActor();
      vtkTrackerTool* tool=this->DataCollector->GetTracker()->GetTool(toolNumber);
      this->ToolActors[toolNumber]->SetName(tool->GetToolName());
      if (tool->GetEnabled())
      {
        SetToolVisible(toolNumber,true);        
      }
    }

    this->Iren->AddObserver(vtkCommand::TimerEvent, this);
    // When the interactor stops it stops our timer as well, trigger a restart when the interactor stops    
    this->Iren->AddObserver(vtkCommand::EndInteractionEvent, this);

    this->TimerId=this->Iren->CreateOneShotTimer(100);
  }

  void AddNewToolActor()
  {
    vtkToolAxesActor* actor=vtkToolAxesActor::New();
    this->Renderer->AddActor(actor);
    actor->SetVisibility(false);
    this->ToolActors.push_back(actor);
  }

  void SetToolVisible(int toolNumber, bool visible)
  {
    this->ToolActors[toolNumber]->SetVisibility(visible);
  }

  void SetToolToTrackerTransform(int toolNumber, vtkMatrix4x4*  toolToTrackerTransform)
  {
    // Get position and orientation from toolToTrackerTransform
    vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
    transform->SetMatrix(toolToTrackerTransform);    
    double *pos=transform->GetPosition();
    double *orient=transform->GetOrientationWXYZ();

    // Set position and orientation in the axes actor (ignore scale)
    vtkSmartPointer<vtkTransform> normalizedTransform=vtkSmartPointer<vtkTransform>::New();
    normalizedTransform->Translate(pos);
    normalizedTransform->RotateWXYZ(orient[0],orient[1],orient[2],orient[3]);

    this->ToolActors[toolNumber]->SetUserTransform(normalizedTransform);
  }
  

  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {       
    std::ostringstream ss;
    ss.precision( 2 ); 

    double toolToTrackerTransformTimestamp=0;
    TrackerStatus trackerStatus=TR_OK;
    for (int toolNumber=0; toolNumber<this->DataCollector->GetNumberOfTools(); toolNumber++)
    {      
      vtkTrackerTool* tool=this->DataCollector->GetTracker()->GetTool(toolNumber);
      if (!tool->GetEnabled())
      {
        SetToolVisible(toolNumber,false);        
        continue;  
      }

      // Tool enabled
      ss << tool->GetToolName() << ": ";

      vtkSmartPointer<vtkMatrix4x4> toolToTrackerTransform=vtkSmartPointer<vtkMatrix4x4>::New(); // a new transform matrix has to be provided to each SetToolToTrackerTransform call
      if (this->DataCollector->GetTransformWithTimestamp(toolToTrackerTransform, toolToTrackerTransformTimestamp, trackerStatus, toolNumber)!=PLUS_SUCCESS)
      {
        ss << "failed to get transform\n";
        SetToolVisible(toolNumber,false);        
        continue;
      }

      if ( trackerStatus!=TR_OK )
      {
        ss	<< "missing or out of view\n"; 
        SetToolVisible(toolNumber,false);        
        continue;
      }

      // There is a valid transform
      SetToolToTrackerTransform(toolNumber, toolToTrackerTransform);
      SetToolVisible(toolNumber,true);
      ss	<< std::fixed 
        << toolToTrackerTransform->GetElement(0,0) << "   " << toolToTrackerTransform->GetElement(0,1) << "   " << toolToTrackerTransform->GetElement(0,2) << "   " << toolToTrackerTransform->GetElement(0,3) << " / "
        << toolToTrackerTransform->GetElement(1,0) << "   " << toolToTrackerTransform->GetElement(1,1) << "   " << toolToTrackerTransform->GetElement(1,2) << "   " << toolToTrackerTransform->GetElement(1,3) << " / "
        << toolToTrackerTransform->GetElement(2,0) << "   " << toolToTrackerTransform->GetElement(2,1) << "   " << toolToTrackerTransform->GetElement(2,2) << "   " << toolToTrackerTransform->GetElement(2,3) << " / "
        << toolToTrackerTransform->GetElement(3,0) << "   " << toolToTrackerTransform->GetElement(3,1) << "   " << toolToTrackerTransform->GetElement(3,2) << "   " << toolToTrackerTransform->GetElement(3,3) << "\n"; 

    }

    this->StepperTextActor->SetInput(ss.str().c_str());
    this->StepperTextActor->Modified(); 

    this->Renderer->GetRenderWindow()->Render();

    static bool firstUpdate=true;
    if (firstUpdate)
    {
      this->Renderer->ResetCamera();
      firstUpdate=false;
    }

    this->TimerId=this->Iren->CreateOneShotTimer(100); 
  }

  vtkDataCollector* DataCollector; 
  vtkRenderer *Renderer;
  vtkRenderWindowInteractor *Iren;
  vtkTextActor *StepperTextActor; 
  std::vector<vtkToolAxesActor*> ToolActors;
  int TimerId;
};

int main(int argc, char **argv)
{
	std::string inputConfigFileName;
  std::string inputToolName; 
	double inputAcqTimeLength(60);
	std::string outputTrackerBufferSequenceFileName; 
	std::string outputFolder("./");
  bool renderingOff(false);

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--input-tool-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolName, "Will print the actual transform of this tool (names were defined in the config file, default is the first active tool)");	
  args.AddArgument("--input-acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker bufffer sequence metafile (Default: TrackerBufferMetafile)");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (inputConfigFileName.empty())
	{
		std::cerr << "input-config-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName.c_str()<< std::endl;
		exit(EXIT_FAILURE);
  }

	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  dataCollector->ReadConfiguration( configRootElement );
	dataCollector->Connect(); 
	dataCollector->Start();

	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

  if ( !dataCollector->GetTrackingEnabled() )
  {
    LOG_ERROR("Tracking is not enabled!"); 
    return EXIT_FAILURE; 
  }

  int portNumber = dataCollector->GetTracker()->GetToolPortByName(inputToolName.c_str());
  if ( portNumber < 0 )
  {
    if ( dataCollector->GetTracker()->GetFirstActiveTool(portNumber) != PLUS_SUCCESS )
    {
      LOG_ERROR("There is no active tool!"); 
      return EXIT_FAILURE; 
    }
  }

  vtkTrackerTool* tool = dataCollector->GetTracker()->GetTool(portNumber); 

  if ( tool == NULL )
  {
    LOG_ERROR("Tool does not exist anymore!"); 
    return EXIT_FAILURE;
  }

  if (renderingOff)
  {
    // No rendering, just show the output on the console

    LOG_DEBUG("Rendering is disabled");

    TrackerBufferItem bufferItem; 
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

    while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
    {

      tool->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem); 
      if ( bufferItem.GetMatrix(matrix) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get matrix from buffer item!"); 
        continue; 
      }

      std::string transformParameters = PlusMath::GetTransformParametersString(matrix); 
      std::string status = vtkTracker::ConvertTrackerStatusToString(bufferItem.GetStatus()); 

      std::ostringstream message;
      message << "Tool name: " << tool->GetToolName() << "Transform:  "; 
      for ( int r = 0; r < 4; r++ )
      {
        for ( int c = 0; c < 4; c++ ) 
        {
          message << "  " << std::fixed << std::setprecision(5) << std::setw(8) << std::setfill(' ') << matrix->GetElement(r,c); 
        }
        message << "    "; 
      }
      message << "  Status: " << status; 


      LOG_INFO( message.str() ); 
      vtksys::SystemTools::Delay(200); 
    }

  }
  else
  {
    // Start live rendering

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New(); 
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New(); 
    renWin->AddRenderer(renderer);  

    //Create the interactor that handles the event loop
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    // Switch interactor style to trackball
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    iren->SetInteractorStyle(style); 

    // Must be called after iren and renderer are linked or there will be problems
    renderer->Render();	

    // iren must be initialized so that it can handle events
    iren->Initialize();

    // Set up transform display actors
    vtkSmartPointer<vtkMyCallback> transformDisplayUpdater = vtkSmartPointer<vtkMyCallback>::New();
    transformDisplayUpdater->DataCollector=dataCollector; 
    transformDisplayUpdater->Renderer=renderer;
    transformDisplayUpdater->Iren=iren;
    transformDisplayUpdater->Init();

    // Add an origin display actor
    vtkSmartPointer<vtkToolAxesActor> originActor=vtkSmartPointer<vtkToolAxesActor>::New();
    originActor->SetName("origin");
    renderer->AddActor(originActor);
   
    // Set projection to parallel to enable estimate distances
    renderer->GetActiveCamera()->ParallelProjectionOn();

    iren->Start();

  }

  dataCollector->Disconnect();

  if (!outputTrackerBufferSequenceFileName.empty())
  {
    LOG_INFO("Copy tracker..."); 
    vtkSmartPointer<vtkTracker> tracker = vtkSmartPointer<vtkTracker>::New(); 
    dataCollector->CopyTracker(tracker); 
    LOG_INFO("Write tracker to " << outputTrackerBufferSequenceFileName);
    dataCollector->WriteTrackerToMetafile( tracker, outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), true); 
  }

	std::cout << "Test completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

}

