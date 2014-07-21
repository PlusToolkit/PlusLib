/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkPlusDeviceFactory.h"

int main(int argc, char **argv)
{
  bool printShortVersionInfo=false;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );
  args.AddArgument( "--short", vtksys::CommandLineArguments::NO_ARGUMENT, &printShortVersionInfo, "Print short version information (by default pring detailed information)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }

  // don't print |INFO|
  vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_ERROR);

  if (!printShortVersionInfo)
  {
    vtkSmartPointer<vtkPlusDeviceFactory> deviceFactory = vtkSmartPointer<vtkPlusDeviceFactory>::New(); 
    std::ostringstream supportedDevices; 
    deviceFactory->PrintAvailableDevices(supportedDevices, vtkIndent());
    std::cout << supportedDevices.str();
  }

  return EXIT_SUCCESS; 
}