#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "FidPatternRecognition.h"
#include "vtkSpacingCalibAlgo.h"
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
  std::vector<std::string> inputSequenceMetafiles; 
  std::string inputBaselineFileName(""); 
  std::string inputConfigFileName(""); 
  std::string inputGnuplotScriptsFolder(""); 
  std::string inputGnuplotCommand(""); 

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
  args.AddArgument("--input-sequence-metafiles", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputSequenceMetafiles, "Input sequence metafile name with path");	
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

  if ( inputSequenceMetafiles.empty() || inputConfigFileName.empty() || inputBaselineFileName.empty() )
  {
    std::cerr << "input-translation-sequence-metafile, input-baseline-file-name and input-config-file-name are required arguments!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

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

	FidPatternRecognition patternRecognition; 
	patternRecognition.ReadConfiguration(configRootElement);

  LOG_INFO("Reading metafiles:");

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  for ( int i = 0; i < inputSequenceMetafiles.size(); ++i )
  {
    LOG_INFO("Reading " << inputSequenceMetafiles[i] << " ..."); 
    vtkSmartPointer<vtkTrackedFrameList> tfList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
    if ( tfList->ReadFromSequenceMetafile(inputSequenceMetafiles[i].c_str()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafiles[i]); 
      return EXIT_FAILURE;
    }

    if ( trackedFrameList->AddTrackedFrameList(tfList) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tracked frame list to container!"); 
      return EXIT_FAILURE; 
    }
  }

  LOG_INFO("Testing image data segmentation...");
  for ( int currentFrameIndex = 0; currentFrameIndex < trackedFrameList->GetNumberOfTrackedFrames(); currentFrameIndex++)
  {
    patternRecognition.RecognizePattern(trackedFrameList->GetTrackedFrame(currentFrameIndex));
  }

  LOG_INFO("Testing spacing computation...");
  vtkSmartPointer<vtkSpacingCalibAlgo> spacingCalibAlgo = vtkSmartPointer<vtkSpacingCalibAlgo>::New(); 
  spacingCalibAlgo->SetInputs(trackedFrameList, patternRecognition.GetFidLabeling()->GetNWires()); 

  double spacing[2]={0};
  if ( spacingCalibAlgo->GetSpacing(spacing) != PLUS_SUCCESS )
  {
    LOG_ERROR("Spacing calibration failed!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Spacing: " << std::fixed << spacing[0] << "  " << spacing[1] << " mm/px"); 
  }

  // Get calibration error
  double errorMean(0), errorStdev(0); 
  if ( spacingCalibAlgo->GetError(errorMean, errorStdev) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get spacing calibration error!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Spacing calibration error - mean: " << std::fixed << errorMean << "  stdev: " << errorStdev); 
  }
  
  LOG_INFO("Testing report table generation and saving into file..."); 
  vtkTable* reportTable = spacingCalibAlgo->GetReportTable(); 
  if ( reportTable != NULL )
  {
    if ( vtkPlusLogger::Instance()->GetLogLevel() >= vtkPlusLogger::LOG_LEVEL_DEBUG ) 
      reportTable->Dump(25); 

    vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(reportTable, "./SpacingCalibrationErrorReport.txt"); 
  }
  else
  {
    LOG_ERROR("Failed to get report table!"); 
    numberOfFailures++; 
  }

  if ( !inputGnuplotCommand.empty() && !inputGnuplotScriptsFolder.empty() )
  {
    LOG_INFO("Testing HTML report generation..."); 
    vtkSmartPointer<vtkHTMLGenerator> htmlGenerator = vtkSmartPointer<vtkHTMLGenerator>::New(); 
    htmlGenerator->SetTitle("Spacing Calibration Test Report"); 
    vtkSmartPointer<vtkGnuplotExecuter> gnuplotExecuter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
    gnuplotExecuter->SetWorkingDirectory("./"); 
    gnuplotExecuter->SetGnuplotCommand(inputGnuplotCommand.c_str()); 
    gnuplotExecuter->SetHideWindow(true); 
    spacingCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 
    htmlGenerator->SaveHtmlPage("SpacingCalibrationErrorReport.html"); 
  }

  std::ostringstream spacingCalibAlgoStream; 
  spacingCalibAlgo->PrintSelf(spacingCalibAlgoStream, vtkIndent(0)); 
  LOG_DEBUG("SpacingCalibAlgo::PrintSelf: "<< spacingCalibAlgoStream.str()); 
  
  //*********************************************************************
  // Compare result to baseline
  
  LOG_INFO("Comparing result with baseline..."); 

  vtkSmartPointer<vtkXMLDataElement> xmlBaseline = vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()); 
  vtkXMLDataElement* xmlSpacingCalibrationBaseline = NULL; 
  if ( xmlBaseline != NULL )
  {
    xmlSpacingCalibrationBaseline = xmlBaseline->FindNestedElementWithName("SpacingCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( xmlSpacingCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find SpacingCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare Spacing to baseline 
    double baseSpacing[2]={0}; 
    if ( !xmlSpacingCalibrationBaseline->GetVectorAttribute( "Spacing", 2, baseSpacing) )
    {
      LOG_ERROR("Unable to find Spacing XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseSpacing[0] - spacing[0]) > DOUBLE_DIFF 
        || fabs(baseSpacing[1] - spacing[1]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Spacing result in pixel differ from baseline: current(" << spacing[0] << ", " << spacing[1] 
        << ") base (" << baseSpacing[0] << ", " << baseSpacing[1] << ")."); 
        numberOfFailures++;
      }
    }

    // Compare errorMean 
    double baseErrorMean=0; 
    if ( !xmlSpacingCalibrationBaseline->GetScalarAttribute("ErrorMean", baseErrorMean) )
    {
      LOG_ERROR("Unable to find ErrorMean XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseErrorMean - errorMean) > DOUBLE_DIFF )
      {
        LOG_ERROR("Spacing mean error differ from baseline: current(" << errorMean << ") base (" << baseErrorMean << ")."); 
        numberOfFailures++;
      }
    }

    // Compare errorStdev
    double baseErrorStdev=0; 
    if ( !xmlSpacingCalibrationBaseline->GetScalarAttribute("ErrorStdev", baseErrorStdev) )
    {
      LOG_ERROR("Unable to find ErrorStdev XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseErrorStdev - errorStdev) > DOUBLE_DIFF )
      {
        LOG_ERROR("Spacing stdev of error differ from baseline: current(" << errorStdev << ") base (" << baseErrorStdev << ")."); 
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