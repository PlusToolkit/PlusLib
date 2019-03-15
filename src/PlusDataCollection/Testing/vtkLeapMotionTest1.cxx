/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusLeapMotion.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

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

  vtkSmartPointer<vtkPlusLeapMotion> leapMotion = vtkSmartPointer<vtkPlusLeapMotion>::New();

  leapMotion->SetAcquisitionRate(frameRate);

  LOG_INFO("Initialize...");

  if (leapMotion->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect to device");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Start recording...");
  if (leapMotion->StartRecording() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect to device");
    exit(EXIT_FAILURE);
  }

  // TODO : get a transform, see if it's sensical?

  LOG_INFO("Exit successfully");
  return EXIT_SUCCESS;

}
