/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.

  Developed by MACBIOIDI & IACTEC group
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkPlusDataSource.h"
#include "vtkSmartPointer.h"
#include "vtkPlusRevopoint3DCamera.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkPointData.h"
#include "vtkActor.h"
#include "igtlOSUtil.h"

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata);

namespace
{
  class vtkMyImageViewerCallback : public vtkCommand
  {
  public:
    static vtkMyImageViewerCallback* New()
    {
      return new vtkMyImageViewerCallback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
      if (m_Channel->GetVideoDataAvailable())
      {
        m_Viewer->SetInputData(m_Channel->GetBrightnessOutput());
      }

      //update the timer so it will trigger again
      m_Viewer->Render();
      m_Interactor->CreateTimer(VTKI_TIMER_UPDATE);
    }

    vtkRenderWindowInteractor* m_Interactor{nullptr};
    vtkImageViewer* m_Viewer{nullptr};
    vtkPlusChannel* m_Channel{nullptr};
  };
  class vtkMyMeshViewerCallback : public vtkCommand
  {
  public:
    static vtkMyMeshViewerCallback* New()
    {
      return new vtkMyMeshViewerCallback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
      static bool firstDisplay{true};

      if (m_Channel->GetVideoDataAvailable())
      {
        m_Mapper->SetInputData(ConvertToPCL(m_Channel->GetBrightnessOutput()));
        m_Mapper->Modified();

        if (firstDisplay)
        {
          m_Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
          firstDisplay = false;
        }
      }

      m_Interactor->Render();
    }

    vtkRenderWindowInteractor* m_Interactor{nullptr};
    vtkPlusChannel* m_Channel{nullptr};
    vtkPolyDataMapper* m_Mapper{nullptr};

  private:
    vtkSmartPointer<vtkPolyData> ConvertToPCL(vtkImageData* imageData)
    {
      if (imageData->GetNumberOfScalarComponents() != 3)
      {
        return nullptr;
      }

      int* dims = imageData->GetDimensions();
      vtkNew<vtkPoints> points;

      for (int x = 0; x < dims[0]; x++)
      {
        for (int y = 0; y < dims[1]; y++)
        {
          for (int z = 0; z < dims[2]; z++)
          {
            auto* coords =
              static_cast<float*>(imageData->GetScalarPointer(x, y, z));

            if (coords[2] == 0)
            {
              continue;
            }

            points->InsertNextPoint(coords[0], coords[1], coords[2]);
          }
        }
      }

      vtkNew<vtkPolyData> poly;
      poly->SetPoints(points);

      vtkNew<vtkVertexGlyphFilter> vertexGenerator;
      vertexGenerator->SetInputData(poly);
      vertexGenerator->Update();

      return vertexGenerator->GetOutput();
    }
  };
}

int main(int argc, char** argv)
{

  bool printHelp(false);
  std::string inputConfigFileName = "Testing/PlusDeviceSet_DataCollectionOnly_Revopoint3DCamera.xml";

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  bool renderingOff(false);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (printHelp)
  {
    std::cout << "\n\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkNew<vtkPlusRevopoint3DCamera> revopoint3DCameraDevice;
  revopoint3DCameraDevice->SetDeviceId("VideoDevice");

  if (!vtksys::SystemTools::FileExists(inputConfigFileName))
  {
    LOG_ERROR("Bad configuration file: " << inputConfigFileName);
    exit(EXIT_FAILURE);
  }

  LOG_DEBUG("Reading config file: " << inputConfigFileName);
  auto* configRead = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());

  if (!configRead)
  {
    LOG_ERROR("Failed to read configuration file");
    exit(EXIT_FAILURE);
  }

  LOG_TRACE("Config file: " << *configRead);
  if (revopoint3DCameraDevice->ReadConfiguration(configRead) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read configuration");
    exit(EXIT_FAILURE);
  }

  if (revopoint3DCameraDevice->NotifyConfigured() != PLUS_SUCCESS)
  {
    LOG_ERROR("Invalid configuration");
    exit(EXIT_FAILURE);
  }

  vtkPlusChannel* depthChannel{nullptr};
  revopoint3DCameraDevice->GetOutputChannelByName(depthChannel, "VideoStreamDEPTH");


  vtkPlusChannel* pclChannel{nullptr};
  revopoint3DCameraDevice->GetOutputChannelByName(pclChannel, "VideoStreamPCL");

  vtkIndent indent;
  revopoint3DCameraDevice->PrintSelf(std::cout, indent);

  // Add an observer to warning and error events for redirecting it to the stdout
  vtkNew<vtkCallbackCommand> callbackCommand;
  callbackCommand->SetCallback(PrintLogsCallback);
  revopoint3DCameraDevice->AddObserver("WarningEvent", callbackCommand);
  revopoint3DCameraDevice->AddObserver("ErrorEvent", callbackCommand);

  if (revopoint3DCameraDevice->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect");
    exit(EXIT_FAILURE);
  }

  if (revopoint3DCameraDevice->StartRecording() != PLUS_SUCCESS)
  {
    LOG_INFO("Failed to start recording");
    exit(EXIT_FAILURE);
  }

  if (!renderingOff)
  {
    vtkNew<vtkImageViewer> imageViewer;
    imageViewer->SetColorWindow(255);
    imageViewer->SetColorLevel(127.5);
    imageViewer->SetZSlice(0);

    // Create the interactor that handles the event loop
    vtkNew<vtkRenderWindowInteractor> imageInteractor;
    imageInteractor->SetRenderWindow(imageViewer->GetRenderWindow());
    imageViewer->SetupInteractor(imageInteractor);

    imageViewer->Render(); //must be called after iren and viewer are linked or there will be problems

    // Establish timer event and create timer to update the live image
    vtkNew<vtkMyImageViewerCallback> call;
    call->m_Interactor = imageInteractor;
    call->m_Viewer = imageViewer;
    imageInteractor->AddObserver(vtkCommand::TimerEvent, call);
    imageInteractor->CreateTimer(VTKI_TIMER_FIRST);

    // Display depth image
    if (depthChannel)
    {
      call->m_Channel = depthChannel;
      imageInteractor->Initialize();
      imageInteractor->Start();
    }

    imageInteractor->GetRenderWindow()->Finalize();

    // Display points cloud
    if (pclChannel)
    {
      vtkNew<vtkRenderer> pclRenderer;
      vtkNew<vtkRenderWindow> pclRenderWindow;
      pclRenderWindow->SetSize(640, 480);
      pclRenderWindow->AddRenderer(pclRenderer);
      vtkNew<vtkRenderWindowInteractor> pclInteractor;
      pclInteractor->SetRenderWindow(pclRenderWindow);

      vtkNew<vtkPolyDataMapper> mapper;
      mapper->SetInputData(vtkSmartPointer<vtkPolyData>::New());
      vtkNew<vtkActor> actor;
      actor->SetMapper(mapper);
      pclRenderer->SetBackground(0., 0., 0.);
      pclRenderer->AddActor(actor);

      pclRenderWindow->Render();
      pclInteractor->Initialize();

      vtkNew<vtkMyMeshViewerCallback> call2;
      call2->m_Interactor = pclInteractor;
      call2->m_Mapper = mapper;
      call2->m_Channel = pclChannel;

      pclInteractor->AddObserver(vtkCommand::TimerEvent, call2);
      pclInteractor->CreateRepeatingTimer(100);

      pclInteractor->Start();
      pclInteractor->GetRenderWindow()->Finalize();
    }
  }
  else
  {
    igtl::Sleep(2000);
  }

  if (revopoint3DCameraDevice->StopRecording() != PLUS_SUCCESS)
  {
    LOG_INFO("Failed to stop recording");
    exit(EXIT_FAILURE);
  }

  if (revopoint3DCameraDevice->Disconnect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to disconnect");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Exit successfully");
  exit(EXIT_SUCCESS);
}


// Callback function for error and warning redirects
void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata)
{
  if (eid == vtkCommand::GetEventIdFromString("WarningEvent"))
  {
    LOG_WARNING((const char*)calldata);
  }
  else if (eid == vtkCommand::GetEventIdFromString("ErrorEvent"))
  {
    LOG_ERROR((const char*)calldata);
  }
}