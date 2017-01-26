/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
\file vtkPlusTransverseProcessEnhancerTest.cxx
\brief TODO Give a one-liner explanation as to the basics of this test
*/

#include "PlusConfigure.h"
#include "vtkPlusTransverseProcessEnhancer.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  ///////////////

  // TODO : write code that tests the functionality of your tool here
  vtkSmartPointer<vtkPlusTransverseProcessEnhancer> enhancer = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();

  return EXIT_SUCCESS;
}