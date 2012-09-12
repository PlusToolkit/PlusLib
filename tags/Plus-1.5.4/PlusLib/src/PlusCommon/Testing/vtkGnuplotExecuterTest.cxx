/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkGnuplotExecuter.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>
#include <iostream>

int main(int argc, char **argv)
{
	bool printHelp(false);
	bool inputHideWindow(false);

	std::string inputGnuplotCommand;
	std::string inputWorkingDirectory;
	std::vector<std::string> inputGnuplotArguments;
	double timeout(120); 

	int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--hide-window", vtksys::CommandLineArguments::NO_ARGUMENT, &inputHideWindow, "Enable gnuplot window hidding (Default: Disabled)");	
	args.AddArgument("--gnuplot-command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGnuplotCommand, "Gnuplot command with path");	
	args.AddArgument("--working-directory", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputWorkingDirectory, "Set working directory (Default: current working directory)");	
	args.AddArgument("--timeout", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &timeout, "Set execution timeout(Default: 120s)");	
	args.AddArgument("--gnuplot-arguments", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputGnuplotArguments, "Gnuplot command line arguments");
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

	if ( inputGnuplotCommand.empty() )
	{
		std::cerr << "gnuplot-command argument is required!" << std::endl;
		exit(EXIT_FAILURE);; 
	}	

	///////////////
	
	vtkSmartPointer<vtkGnuplotExecuter> gnuplot = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
	
	gnuplot->SetGnuplotCommand(inputGnuplotCommand.c_str()); 
	if ( !inputWorkingDirectory.empty() )
	{
		gnuplot->SetWorkingDirectory(inputWorkingDirectory.c_str()); 
	}

	gnuplot->SetHideWindow(inputHideWindow); 

	gnuplot->SetTimeout(timeout); 
	
	for ( unsigned int i = 0; i < inputGnuplotArguments.size(); i++ )
	{
		gnuplot->AddArgument(inputGnuplotArguments[i].c_str() ); 
	}

	if ( gnuplot->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!"); 
    exit(EXIT_FAILURE); 
  }
		
	return EXIT_SUCCESS; 
 }
