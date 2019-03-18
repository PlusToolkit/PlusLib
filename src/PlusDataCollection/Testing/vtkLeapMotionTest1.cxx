/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusLeapMotion.h"

// VTK includes
#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtksys/CommandLineArguments.hxx>

// STL includes
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
  double frameRate = 60;
  bool printHelp(false);

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--frame-rate", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameRate, "Requested acquisition frame rate (in FPS, default = 30)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << std::endl << std::endl << "vtkLeapMotionTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (printHelp)
  {
    std::cout << std::endl << std::endl << "vtkLeapMotionTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);

  }

  vtkNew<vtkPlusLeapMotion> leapMotion;
  leapMotion->SetDeviceId("LeapMotionDevice");
  leapMotion->SetToolReferenceFrameName("Tracker");

  vtkNew<vtkPlusDataSource> source;
  source->SetId("LeftIndexDistal");
  source->SetType(DATA_SOURCE_TYPE_TOOL);
  vtkNew<vtkPlusChannel> channel;
  channel->SetChannelId("TrackerChannel");
  channel->AddTool(source);
  leapMotion->AddTool(source);
  leapMotion->AddOutputChannel(channel);

  if (leapMotion->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect to device");
    exit(EXIT_FAILURE);
  }

  if (leapMotion->StartRecording() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to start recording");
    exit(EXIT_FAILURE);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  igsioTrackedFrame frame;
  if (channel->GetTrackedFrame(frame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve frame from channel.");
    exit(EXIT_FAILURE);
  }

  vtkNew<vtkMatrix4x4> mat;
  if (frame.GetFrameTransform(igsioTransformName("LeftIndexDistal", "Tracker"), mat) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve left index distal transform.");
    exit(EXIT_FAILURE);
  }

  if (leapMotion->StopRecording() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to stop recording.");
    exit(EXIT_FAILURE);
  }

  if (leapMotion->Disconnect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to disconnect");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;

}
