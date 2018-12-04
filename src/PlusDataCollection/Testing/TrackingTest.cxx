/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
\file TrackingTest.cxx
\brief This a simple test program to acquire a tracking data and optionally
writes the buffer to a metafile and displays the live transform in a 3D view.
*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "igsioMath.h"
#include "vtkPlusToolAxesActor.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkXMLUtilities.h>
#include <vtksys/CommandLineArguments.hxx>
#include <vtksys/SystemTools.hxx>

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New()
  {return new vtkMyCallback;}

  vtkMyCallback()
  {
    this->TimerId = -1;
    this->StepperTextActor = vtkTextActor::New();
  }

  virtual ~vtkMyCallback()
  {
    this->StepperTextActor->Delete();
    this->StepperTextActor = NULL;
    for (std::map<std::string, vtkPlusToolAxesActor*>::iterator it = this->ToolActors.begin(); it != this->ToolActors.end(); ++it)
    {
      //this->Renderer->RemoveActor(this->ToolActors[i]);
      it->second->Delete();
      it->second = NULL;
    }
  }

  void Init()
  {
    // Create a text actor for tracking information
    vtkTextProperty* textprop = this->StepperTextActor->GetTextProperty();
    textprop->SetColor(1, 0, 0);
    textprop->SetFontFamilyToArial();
    textprop->SetFontSize(15);
    textprop->SetJustificationToLeft();
    textprop->SetVerticalJustificationToTop();
    this->StepperTextActor->VisibilityOn();
    this->StepperTextActor->SetDisplayPosition(20, 65);
    this->Renderer->AddActor(this->StepperTextActor);

    vtkPlusDevice* aDevice = NULL;
    this->DataCollector->GetDevice(aDevice, DeviceId);
    if (aDevice == NULL)
    {
      return;
    }
    for (DataSourceContainerConstIterator it = aDevice->GetToolIteratorBegin(); it != aDevice->GetToolIteratorEnd(); ++it)
    {
      vtkPlusDataSource* tool = it->second;
      AddNewToolActor(tool->GetId());
      SetToolVisible(tool->GetId(), true);
    }

    this->RenderWindowInteractor->AddObserver(vtkCommand::TimerEvent, this);
    // When the interactor stops it stops our timer as well, trigger a restart when the interactor stops
    this->RenderWindowInteractor->AddObserver(vtkCommand::EndInteractionEvent, this);

    this->TimerId = this->RenderWindowInteractor->CreateOneShotTimer(100);
  }

  void AddNewToolActor(const std::string& aToolId)
  {
    vtkPlusToolAxesActor* actor = vtkPlusToolAxesActor::New();
    this->Renderer->AddActor(actor);
    actor->SetVisibility(false);
    this->ToolActors[aToolId] = actor;
    actor->SetName(aToolId);
  }

  void SetToolVisible(const std::string& aToolId, bool visible)
  {
    this->ToolActors[aToolId]->SetVisibility(visible);
  }

  void SetToolToTrackerTransform(const std::string& aToolId, vtkMatrix4x4*  toolToTrackerTransform)
  {
    vtkSmartPointer<vtkTransform> normalizedTransform = vtkSmartPointer<vtkTransform>::New();
    normalizedTransform->SetMatrix(toolToTrackerTransform);
    this->ToolActors[aToolId]->SetUserTransform(normalizedTransform);
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    std::ostringstream ss;
    ss.precision(2);

    igsioTrackedFrame trackedFrame;
    if (this->BroadcastChannel->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get tracked frame!");
      this->TimerId = this->RenderWindowInteractor->CreateOneShotTimer(100);
      return;
    }

    vtkPlusDevice* aDevice = NULL;
    this->DataCollector->GetDevice(aDevice, DeviceId);
    if (aDevice == NULL)
    {
      return;
    }

    std::vector<igsioTransformName> transformNameList;
    trackedFrame.GetFrameTransformNameList(transformNameList);
    for (std::vector<igsioTransformName>::iterator it = transformNameList.begin(); it != transformNameList.end(); ++it)
    {
      igsioTransformName transformName = *it;

      vtkPlusDataSource* tool = NULL;
      if (aDevice->GetTool(transformName.GetTransformName().c_str(), tool) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get tool: " << transformName.From());
        continue;
      }

      std::string strTransformName;
      transformName.GetTransformName(strTransformName);
      // Transform name
      ss << strTransformName << ": ";

      vtkSmartPointer<vtkMatrix4x4> toolToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); // a new transform matrix has to be provided to each SetToolToTrackerTransform call
      if (trackedFrame.GetFrameTransform(transformName, toolToTrackerTransform) != PLUS_SUCCESS)
      {
        ss << "failed to get transform\n";
        SetToolVisible(tool->GetId(), false);
        continue;
      }

      ToolStatus status(TOOL_INVALID);
      trackedFrame.GetFrameTransformStatus(transformName, status);

      if (status != TOOL_OK)
      {
        ss  << "missing or out of view\n";
        SetToolVisible(tool->GetId(), false);
        continue;
      }

      // There is a valid transform
      SetToolToTrackerTransform(tool->GetId(), toolToTrackerTransform);
      SetToolVisible(tool->GetId(), true);
      ss  << std::fixed
          << toolToTrackerTransform->GetElement(0, 0) << "   " << toolToTrackerTransform->GetElement(0, 1) << "   " << toolToTrackerTransform->GetElement(0, 2) << "   " << toolToTrackerTransform->GetElement(0, 3) << " / "
          << toolToTrackerTransform->GetElement(1, 0) << "   " << toolToTrackerTransform->GetElement(1, 1) << "   " << toolToTrackerTransform->GetElement(1, 2) << "   " << toolToTrackerTransform->GetElement(1, 3) << " / "
          << toolToTrackerTransform->GetElement(2, 0) << "   " << toolToTrackerTransform->GetElement(2, 1) << "   " << toolToTrackerTransform->GetElement(2, 2) << "   " << toolToTrackerTransform->GetElement(2, 3) << " / "
          << toolToTrackerTransform->GetElement(3, 0) << "   " << toolToTrackerTransform->GetElement(3, 1) << "   " << toolToTrackerTransform->GetElement(3, 2) << "   " << toolToTrackerTransform->GetElement(3, 3) << "\n";
    }

    this->StepperTextActor->SetInput(ss.str().c_str());
    this->StepperTextActor->Modified();

    this->Renderer->GetRenderWindow()->Render();

    static bool firstUpdate = true;
    if (firstUpdate)
    {
      this->Renderer->ResetCamera();
      firstUpdate = false;
    }

    this->TimerId = this->RenderWindowInteractor->CreateOneShotTimer(100);
  }

  vtkPlusDataCollector* DataCollector;
  vtkPlusChannel* BroadcastChannel;
  std::string DeviceId;
  vtkRenderer* Renderer;
  vtkRenderWindowInteractor* RenderWindowInteractor;
  vtkTextActor* StepperTextActor;
  std::map<std::string, vtkPlusToolAxesActor*> ToolActors;
  int TimerId;
};

int main(int argc, char** argv)
{
  bool printHelp(false);
  std::string inputConfigFileName;
  std::string inputToolSourceId;
  double inputAcqTimeLength(60);
  std::string outputTrackerBufferSequenceFileName;
  bool renderingOff(false);

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--tool-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolSourceId, "Will print the actual transform of this tool (names were defined in the config file, default is the first active tool)");
  args.AddArgument("--acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");
  args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker bufffer sequence metafile (Default: TrackerBufferMetafile)");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    std::cerr << "--config-file is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to parse data collection XML tag.");
    return EXIT_FAILURE;
  }
  std::string deviceId;
  vtkXMLDataElement* dataCollectionElement = configRootElement->FindNestedElementWithName("DataCollection");
  vtkXMLDataElement* deviceElement = dataCollectionElement->FindNestedElementWithName("Device");
  if (deviceElement != NULL)
  {
    deviceId = std::string(deviceElement->GetAttribute("Id"));
  }

  dataCollector->Connect();
  dataCollector->Start();

  vtkPlusDevice* aDevice = NULL;
  dataCollector->GetDevice(aDevice, deviceId);
  if (aDevice == NULL)
  {
    LOG_ERROR("Unable to retrieve device \'" << deviceId << "\'.");
    return EXIT_FAILURE;
  }
  if (aDevice->OutputChannelCount() == 0)
  {
    LOG_ERROR("No channels to retrieve data from. Check config file.");
    return EXIT_FAILURE;
  }
  vtkPlusChannel* aChannel = *(aDevice->GetOutputChannelsStart());

  const double acqStartTime = vtkTimerLog::GetUniversalTime();

  if (!aChannel->GetTrackingEnabled())
  {
    LOG_ERROR("Tracking is not enabled!");
    return EXIT_FAILURE;
  }

  vtkPlusDataSource* tool = NULL;
  if (!inputToolSourceId.empty())
  {
    if (aChannel->GetTool(tool, inputToolSourceId.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get tool with name: " << inputToolSourceId);
      return EXIT_FAILURE;
    }
  }
  else
  {
    if (aChannel->GetToolsStartIterator() == aChannel->GetToolsEndIterator())
    {
      LOG_ERROR("There is no active tool!");
      return EXIT_FAILURE;
    }

    // Use the first active tool
    tool = aChannel->GetToolsStartIterator()->second;
  }

  if (tool == NULL)
  {
    LOG_ERROR("Tool does not exist anymore!");
    return EXIT_FAILURE;
  }

  if (renderingOff)
  {
    // No rendering, just show the output on the console
    LOG_DEBUG("Rendering is disabled");

    StreamBufferItem bufferItem;
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

    while (acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime())
    {

      tool->GetLatestStreamBufferItem(&bufferItem);
      if (bufferItem.GetMatrix(matrix) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get matrix from buffer item!");
        continue;
      }

      std::string transformParameters = igsioMath::GetTransformParametersString(matrix);
      std::string status = igsioCommon::ConvertToolStatusToString(bufferItem.GetStatus());

      std::ostringstream message;
      message << "Tool name: " << tool->GetId() << "Transform:  ";
      for (int r = 0; r < 4; r++)
      {
        for (int c = 0; c < 4; c++)
        {
          message << "  " << std::fixed << std::setprecision(5) << std::setw(8) << std::setfill(' ') << matrix->GetElement(r, c);
        }
        message << "    ";
      }
      message << "  Status: " << status;

      LOG_INFO(message.str());
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
    transformDisplayUpdater->DataCollector = dataCollector;
    transformDisplayUpdater->BroadcastChannel = aChannel;
    transformDisplayUpdater->Renderer = renderer;
    transformDisplayUpdater->DeviceId = deviceId;
    transformDisplayUpdater->RenderWindowInteractor = iren;
    transformDisplayUpdater->Init();

    // Add an origin display actor
    vtkSmartPointer<vtkPlusToolAxesActor> originActor = vtkSmartPointer<vtkPlusToolAxesActor>::New();
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
    vtkSmartPointer<vtkPlusDevice> tracker = vtkSmartPointer<vtkPlusDevice>::New();
    tracker->DeepCopy(*aDevice);
    std::string fullPath = vtkPlusConfig::GetInstance()->GetOutputPath(outputTrackerBufferSequenceFileName);
    LOG_INFO("Write tracker to " << fullPath);
    tracker->WriteToolsToSequenceFile(fullPath.c_str(), true);
  }

  std::cout << "Test completed successfully!" << std::endl;
  return EXIT_SUCCESS;
}