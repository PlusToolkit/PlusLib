/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file SingleWallCalibrationAlgoTest.cxx
  \brief 

  \ingroup PerkLabSandboxSingleWallCalibrationAlgorithm
*/

#include "PlusConfigure.h"
#include "vtkMetaImageSequenceIO.h"
#include "vtkSingleWallCalibrationAlgo.h"
#include "vtkSmartPointer.h"
#include "vtkTrackedFrameList.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"

using namespace std; 

int main(int argc, char **argv)
{
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  std::string inputConfigFile;
  std::string trackedFramesFileLocation;

  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFile, "Config file containing the device configuration.");
  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &trackedFramesFileLocation, "The location of the meta file.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
 
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if( trackedFramesFileLocation.empty() )
  {
    LOG_ERROR("Input sequence file mandatory. Re-run with --input-seq-file defined.");
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkXMLDataElement> rootConfig;
  if( !inputConfigFile.empty() )
  {
    LOG_DEBUG("Reading config file...");
    rootConfig = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromFile(inputConfigFile.c_str()) ); 
    LOG_DEBUG("Reading config file finished.");
  }

  vtkSmartPointer<vtkMetaImageSequenceIO> reader = vtkSmartPointer<vtkMetaImageSequenceIO>::New();
  reader->SetFileName( trackedFramesFileLocation.c_str() );
  if( reader->Read() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to read tracked frame list.");
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkSingleWallCalibrationAlgo> aCalibration = vtkSmartPointer<vtkSingleWallCalibrationAlgo>::New();
  if( rootConfig.GetPointer() != NULL )
  {
    aCalibration->ReadConfiguration( rootConfig );
  }
  aCalibration->SetTrackedFrameList(reader->GetTrackedFrameList());
  aCalibration->Calibrate();

  return EXIT_SUCCESS; 
}