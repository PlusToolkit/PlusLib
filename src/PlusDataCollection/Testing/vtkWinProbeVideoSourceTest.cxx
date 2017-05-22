/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkPlusWinProbeVideoSourceTest.cxx
  \brief Test basic connection to the WinProbe ultrasound system
  and write 10 frames to the current directory
  \ingroup PlusLibDataCollection
*/

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkPlusWinProbeVideoSource.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPlusDataSource.h"

class vtkMyCallback : public vtkCommand
{
public:
    static vtkMyCallback *New() { return new vtkMyCallback; }

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        m_Viewer->Render();

        //update the timer so it will trigger again
        m_Interactor->CreateTimer(VTKI_TIMER_UPDATE);
    }

    vtkRenderWindowInteractor *m_Interactor;
    vtkImageViewer *m_Viewer;

private:

    vtkMyCallback()
    {
        m_Interactor = NULL;
        m_Viewer = NULL;
    }
};

int main(int argc, char* argv[])
{
    bool printHelp(false);
    bool renderingOff(false);
    std::string inputConfigFileName;
    std::string outputFileName("WinProbeOutputSeq.nrrd");
    int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEBUG;

    vtksys::CommandLineArguments args;
    args.Initialize(argc, argv);
    args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
    args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
    args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
    args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
    args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

    if (!args.Parse())
    {
        std::cerr << "Problem parsing arguments" << std::endl;
        std::cout << "\nvtkPlusWinProbeVideoSourceTest help:" << args.GetHelp() << std::endl;
        exit(EXIT_FAILURE);
    }

    vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
    if (printHelp)
    {
        std::cout << "\n\nvtkPlusWinProbeVideoSourceTest help:" << args.GetHelp() << std::endl;
        exit(EXIT_SUCCESS);
    }


    vtkSmartPointer< vtkPlusWinProbeVideoSource > WinProbeDevice = vtkSmartPointer< vtkPlusWinProbeVideoSource >::New();
    WinProbeDevice->SetDeviceId("VideoDevice");

    // Read config file
    if (STRCASECMP(inputConfigFileName.c_str(), "") != 0)
    {
        LOG_DEBUG("Reading config file...");
        vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();

        if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
        {
            LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
            return EXIT_FAILURE;
        }

        WinProbeDevice->ReadConfiguration(configRootElement);
    }


    if (WinProbeDevice->Connect() != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to connect to WinProbe Probe");
        exit(EXIT_FAILURE);
    }

    WinProbeDevice->StartRecording(); //start recording frame from the video

    if (renderingOff)
    {
        Sleep(500); //allow some time to buffer frames

        vtkPlusChannel* aChannel(NULL);
        if (WinProbeDevice->GetOutputChannelByName(aChannel, "VideoStream") != PLUS_SUCCESS)
        {
            LOG_ERROR("Unable to locate the channel with Id=\"VideoStream\". Check config file.");
            exit(EXIT_FAILURE);
        }

        vtkPlusDataSource* aSource(NULL);
        aChannel->GetVideoSource(aSource);
        aSource->WriteToSequenceFile(outputFileName.c_str());
    }
    else
    {
        vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
        viewer->SetInputConnection(WinProbeDevice->GetOutputPort());   //set image to the render and window
        viewer->SetColorWindow(255);
        viewer->SetColorLevel(127.5);
        viewer->SetZSlice(0);
        viewer->SetSize(128, 512);

        //Create the interactor that handles the event loop
        vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        iren->SetRenderWindow(viewer->GetRenderWindow());
        viewer->SetupInteractor(iren);

        viewer->Render(); //must be called after iren and viewer are linked or there will be problems

        // Establish timer event and create timer to update the live image
        vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
        call->m_Interactor = iren;
        call->m_Viewer = viewer;
        iren->AddObserver(vtkCommand::TimerEvent, call);
        iren->CreateTimer(VTKI_TIMER_FIRST);

        //iren must be initialized so that it can handle events
        iren->Initialize();
        iren->Start();
    }

    WinProbeDevice->StopRecording();
    WinProbeDevice->Disconnect();
    
    return EXIT_SUCCESS;
}

