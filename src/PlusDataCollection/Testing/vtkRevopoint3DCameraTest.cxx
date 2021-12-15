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
#include "vtkVersion.h"
#include "vtkPlusRevopoint3DCamera.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkImageViewer.h"
#include "igtlOSUtil.h"

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata);

namespace
{
class vtkMyCallback : public vtkCommand
{
public:
    static vtkMyCallback* New()
    {
        return new vtkMyCallback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        m_Viewer->Render();

        //update the timer so it will trigger again
        m_Interactor->CreateTimer(VTKI_TIMER_UPDATE);
    }

    vtkRenderWindowInteractor* m_Interactor;
    vtkImageViewer* m_Viewer;

private:

    vtkMyCallback()
    {
        m_Interactor = NULL;
        m_Viewer = NULL;
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

    revopoint3DCameraDevice->CreateDefaultOutputChannel(NULL, true);
    vtkPlusDataSource* videoSource = NULL;
    if (revopoint3DCameraDevice->GetFirstActiveOutputVideoSource(videoSource) != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to retrieve the video source.");
        exit(EXIT_FAILURE);
    }
    videoSource->SetInputImageOrientation(US_IMG_ORIENT_UN);

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
        auto viewer = vtkSmartPointer<vtkImageViewer>::New();
        viewer->SetInputConnection(revopoint3DCameraDevice->GetOutputPort());   //set image to the render and window
        viewer->SetColorWindow(500);
        viewer->SetColorLevel(250);
        viewer->SetZSlice(0);

        //Create the interactor that handles the event loop
        vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        iren->SetRenderWindow(viewer->GetRenderWindow());
        viewer->SetupInteractor(iren);

        viewer->Render(); //must be called after iren and viewer are linked or there will be problems

        // Establish timer event and create timer to update the live image
        auto call = vtkSmartPointer<vtkMyCallback>::New();
        call->m_Interactor = iren;
        call->m_Viewer = viewer;
        iren->AddObserver(vtkCommand::TimerEvent, call);
        iren->CreateTimer(VTKI_TIMER_FIRST);

        //iren must be initialized so that it can handle events
        iren->Initialize();
        iren->Start();
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