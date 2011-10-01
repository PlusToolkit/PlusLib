#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "FidPatternRecognition.h"
#include "vtkRotationAxisCalibAlgo.h"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"

const double DOUBLE_DIFF = 0.0001; // used for comparing double numbers


//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int numberOfFailures(0); 

  bool printHelp(false);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  std::string inputRotationSequenceMetafile(""); 
  std::string inputBaselineFileName(""); 
  std::string inputConfigFileName(""); 
  std::string inputGnuplotScriptsFolder(""); 
  std::string inputGnuplotCommand(""); 

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
  args.AddArgument("--input-rotation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationSequenceMetafile, "Input sequence metafile name with path");	
  args.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Input xml baseline file name with path");	
  args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input xml config file name with path");	
  args.AddArgument("--input-gnuplot-scripts-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGnuplotScriptsFolder, "Path to gnuplot scripts folder");	
  args.AddArgument("--input-gnuplot-command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGnuplotCommand, "Path to gnuplot command.");	
  
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  if ( inputRotationSequenceMetafile.empty() || inputConfigFileName.empty() || inputBaselineFileName.empty() )
  {
    std::cerr << "input-translation-sequence-metafile, input-baseline-file-name, and input-config-file-name are required arguments!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);
  
  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);
  vtkPlusConfig::GetInstance()->SetProgramPath(programPath.c_str());

	FidPatternRecognition patternRecognition; 
	patternRecognition.ReadConfiguration(configRootElement);

  LOG_INFO("Read rotation data from metafile...");

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputRotationSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to read sequence metafile: " << inputRotationSequenceMetafile); 
      return EXIT_FAILURE;
  }

  LOG_INFO("Testing image data segmentation...");
  for ( int currentFrameIndex = 0; currentFrameIndex < trackedFrameList->GetNumberOfTrackedFrames(); currentFrameIndex++)
  {
    patternRecognition.RecognizePattern(trackedFrameList->GetTrackedFrame(currentFrameIndex));
  }

  LOG_INFO("Spacing computation...");
  double spacing[2]={0.191451, 0.186871}; // TODO: remove hard coded spacing info, compute it or read it from baseline 

  LOG_INFO("Spacing: " << std::fixed << spacing[0] << "  " << spacing[1] << " mm/px"); 

  LOG_INFO("Testing rotation axis orientation computation algorithm...");
  vtkSmartPointer<vtkRotationAxisCalibAlgo> rotAxisCalibAlgo = vtkSmartPointer<vtkRotationAxisCalibAlgo>::New(); 
  rotAxisCalibAlgo->SetInputs(trackedFrameList, spacing); 
  
  // Get rotation axis calibration output 
  double rotationAxisOrientation[3] = {0}; 
  if ( rotAxisCalibAlgo->GetRotationAxisOrientation(rotationAxisOrientation) != PLUS_SUCCESS )
  {
    LOG_ERROR("Rotation axis calibration failed!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Rotation axis orientation: " << std::fixed << rotationAxisOrientation[0] << "  " << rotationAxisOrientation[1] << "  " << rotationAxisOrientation[2]); 
  }

  // Get center of rotation 
  double *centerofRotationInPx = rotAxisCalibAlgo->GetCenterOfRotationPx(); 
  LOG_INFO("Center of rotation (px): " << centerofRotationInPx[0] << "  " << centerofRotationInPx[1]); 

  // Get calibration error
  double errorMean(0), errorStdev(0); 
  if ( rotAxisCalibAlgo->GetError(errorMean, errorStdev) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get rotation axis calibration error!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Rotation axis calibration error - mean: " << std::fixed << errorMean << "  stdev: " << errorStdev); 
  }
  
  LOG_INFO("Testing report table generation and saving into file..."); 
  vtkTable* reportTable = rotAxisCalibAlgo->GetReportTable(); 
  if ( reportTable != NULL )
  {
    if ( vtkPlusLogger::Instance()->GetLogLevel() >= vtkPlusLogger::LOG_LEVEL_DEBUG ) 
      reportTable->Dump(25); 

    vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(reportTable, "./RotationAxisCalibrationErrorReport.txt"); 
  }
  else
  {
    LOG_ERROR("Failed to get report table!"); 
    numberOfFailures++; 
  }

  vtkTable* centerOfRotationReportTable = rotAxisCalibAlgo->GetCenterOfRotationReportTable(); 
  if ( centerOfRotationReportTable != NULL )
  {
    if ( vtkPlusLogger::Instance()->GetLogLevel() >= vtkPlusLogger::LOG_LEVEL_DEBUG ) 
      centerOfRotationReportTable->Dump(25); 

    vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(centerOfRotationReportTable, "./CenterOfRotationCalibrationErrorReport.txt"); 
  }
  else
  {
    LOG_ERROR("Failed to get center of rotation report table!"); 
    numberOfFailures++; 
  }

  if ( !inputGnuplotCommand.empty() && !inputGnuplotScriptsFolder.empty() )
  {
    LOG_INFO("Testing HTML report generation..."); 
    vtkSmartPointer<vtkHTMLGenerator> htmlGenerator = vtkSmartPointer<vtkHTMLGenerator>::New(); 
    htmlGenerator->SetTitle("Rotation Axis Calibration Report"); 
    vtkSmartPointer<vtkGnuplotExecuter> gnuplotExecuter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
    gnuplotExecuter->SetWorkingDirectory("./"); 
    gnuplotExecuter->SetGnuplotCommand(inputGnuplotCommand.c_str()); 
    gnuplotExecuter->SetHideWindow(true); 
    rotAxisCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 
    htmlGenerator->SaveHtmlPage("RotationAxisCalibrationErrorReport.html"); 
  }

  std::ostringstream rotAxisCalibAlgoStream; 
  rotAxisCalibAlgo->PrintSelf(rotAxisCalibAlgoStream, vtkIndent(0)); 
  LOG_DEBUG("RotationAxisCalibAlgo::PrintSelf: "<< rotAxisCalibAlgoStream.str()); 
  
  //*********************************************************************
  // Compare result to baseline
  
  LOG_INFO("Comparing result with baseline..."); 

  vtkSmartPointer<vtkXMLDataElement> xmlBaseline = vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()); 
  vtkXMLDataElement* xmlRotationAxisCalibrationBaseline = NULL; 
  vtkXMLDataElement* xmlCenterOfRotationCalibrationBaseline = NULL; 
  if ( xmlBaseline != NULL )
  {
    xmlRotationAxisCalibrationBaseline = xmlBaseline->FindNestedElementWithName("RotationAxisCalibrationResult"); 
    xmlCenterOfRotationCalibrationBaseline = xmlBaseline->FindNestedElementWithName("CenterOfRotationCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( xmlRotationAxisCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find RotationAxisCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare rotationAxisOrientation to baseline 
    double baseRotationAxisOrientation[3]={0}; 
    if ( !xmlRotationAxisCalibrationBaseline->GetVectorAttribute( "RotationAxisOrientation", 3, baseRotationAxisOrientation) )
    {
      LOG_ERROR("Unable to find RotationAxisOrientation XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseRotationAxisOrientation[0] - rotationAxisOrientation[0]) > DOUBLE_DIFF 
        || fabs(baseRotationAxisOrientation[1] - rotationAxisOrientation[1]) > DOUBLE_DIFF 
        || fabs(baseRotationAxisOrientation[2] - rotationAxisOrientation[2]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Rotation axis orientation differ from baseline: current(" << rotationAxisOrientation[0] << ", " << rotationAxisOrientation[1] << ", " << rotationAxisOrientation[2] 
        << ") base (" << baseRotationAxisOrientation[0] << ", " << baseRotationAxisOrientation[1] << ", " << baseRotationAxisOrientation[2] <<  ")."); 
        numberOfFailures++;
      }
    }

    // Compare errorMean 
    double baseErrorMean=0; 
    if ( !xmlRotationAxisCalibrationBaseline->GetScalarAttribute("ErrorMean", baseErrorMean) )
    {
      LOG_ERROR("Unable to find ErrorMean XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseErrorMean - errorMean) > DOUBLE_DIFF )
      {
        LOG_ERROR("Rotation axis calibration mean error differ from baseline: current(" << errorMean << ") base (" << baseErrorMean << ")."); 
        numberOfFailures++;
      }
    }

    // Compare errorStdev
    double baseErrorStdev=0; 
    if ( !xmlRotationAxisCalibrationBaseline->GetScalarAttribute("ErrorStdev", baseErrorStdev) )
    {
      LOG_ERROR("Unable to find ErrorStdev XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseErrorStdev - errorStdev) > DOUBLE_DIFF )
      {
        LOG_ERROR("Rotation axis calibration stdev of error differ from baseline: current(" << errorStdev << ") base (" << baseErrorStdev << ")."); 
        numberOfFailures++;
      }
    }
  }

  if ( xmlCenterOfRotationCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find CenterOfRotationCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare CenterOfRotationPx to baseline 
    double baseCenterOfRotationPx[2]={0}; 
    if ( !xmlCenterOfRotationCalibrationBaseline->GetVectorAttribute( "CenterOfRotationPx", 2, baseCenterOfRotationPx) )
    {
      LOG_ERROR("Unable to find CenterOfRotationPx XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseCenterOfRotationPx[0] - centerofRotationInPx[0]) > DOUBLE_DIFF 
        || fabs(baseCenterOfRotationPx[1] - centerofRotationInPx[1]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Center of rotation result in pixel differ from baseline: current(" << centerofRotationInPx[0] << ", " << centerofRotationInPx[1] 
        << ") base (" << baseCenterOfRotationPx[0] << ", " << baseCenterOfRotationPx[1] << ")."); 
        numberOfFailures++;
      }
    }
  }


  if ( numberOfFailures > 0 ) 
  {
    LOG_INFO("Test failed!"); 
    return EXIT_FAILURE; 
  }

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS; 
}
