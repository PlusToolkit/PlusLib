/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"

class vtkLogTestObject : public vtkObject
{
public:
  static vtkLogTestObject *New()
  {
    return new vtkLogTestObject;
  }
  void LogMessages()
  {
    vtkErrorMacro("This is a VTK error message");
    vtkWarningMacro("This is a VTK warning message");
    vtkGenericWarningMacro("This is a VTK generic warning message");
    vtkDebugMacro("This is a VTK debug message");
  }
protected:
  vtkLogTestObject() {};
  virtual ~vtkLogTestObject() {}; 
};

int main(int argc, char **argv)
{
	bool printHelp(false);

	int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
	
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( printHelp ) 
	{
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}	

  // Change output dir to test log saving to different folder
  std::ostringstream outputDir; 
  outputDir << vtkPlusConfig::GetInstance()->GetProgramDirectory() << "/OutputTest"; 
  vtkPlusConfig::GetInstance()->SetOutputDirectory( outputDir.str().c_str() ); 

  std::cout << "Verbose level: " << verboseLevel << std::endl;

  LOG_ERROR("This is a test error message");
  LOG_WARNING("This is a test warning message");
  LOG_INFO("This is a test info message");
  
  // Call set output dir with the same dir 
  vtkPlusConfig::GetInstance()->SetOutputDirectory( outputDir.str().c_str() ); 
  
  LOG_DEBUG("This is a test debug message");
  LOG_TRACE("This is a test trace message");

  vtkSmartPointer<vtkLogTestObject> logTester=vtkSmartPointer<vtkLogTestObject>::New();
  logTester->DebugOn();
  logTester->LogMessages();

	return EXIT_SUCCESS; 
 }
