#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "FidPatternRecognition.h"
#include "vtkTranslAxisCalibAlgo.h"
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
  std::string inputTranslationSequenceMetafile(""); 
  std::string inputBaselineFileName(""); 
  std::string inputConfigFileName(""); 
  std::string inputStrDataType(""); 
  IMAGE_DATA_TYPE inputDataType(UNKNOWN_DATA); 
  std::string baselineXmlAttributeName(""); 
  std::string baselineXmlElementName(""); 
  std::string inputGnuplotScriptsFolder(""); 
  std::string inputGnuplotCommand(""); 

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
  args.AddArgument("--input-translation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationSequenceMetafile, "Input sequence metafile name with path");	
  args.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Input xml baseline file name with path");	
  args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input xml config file name with path");	
  args.AddArgument("--input-data-type", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputStrDataType, "Input data type ( TEMPLATE_TRANSLATION or PROBE_TRANSLATION)");	
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

  if ( inputTranslationSequenceMetafile.empty() || inputConfigFileName.empty() || inputBaselineFileName.empty() || inputStrDataType.empty() )
  {
    std::cerr << "input-translation-sequence-metafile, input-baseline-file-name, input-data-type and input-config-file-name are required arguments!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 


  if ( STRCASECMP(inputStrDataType.c_str(), "TEMPLATE_TRANSLATION") == 0 )
  {
    LOG_INFO("Selected data type: TEMPLATE_TRANSLATION"); 
    inputDataType = TEMPLATE_TRANSLATION; 
    baselineXmlAttributeName = "TemplateTranslationAxisOrientation"; 
    baselineXmlElementName = "TemplateTranslationAxisCalibrationResult"; 
  }
  else if ( STRCASECMP(inputStrDataType.c_str(), "PROBE_TRANSLATION") == 0 )
  {
    LOG_INFO("Selected data type: PROBE_TRANSLATION"); 
    inputDataType = PROBE_TRANSLATION; 
    baselineXmlAttributeName = "ProbeTranslationAxisOrientation"; 
    baselineXmlElementName = "ProbeTranslationAxisCalibrationResult"; 
  }
  else
  {
    LOG_ERROR("This data type is not supported: " << inputStrDataType); 
    exit(EXIT_FAILURE);
  }

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

  LOG_INFO("Read translation data from metafile...");

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputTranslationSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to read sequence metafile: " << inputTranslationSequenceMetafile); 
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

  LOG_INFO("Testing translation axis orientation computation algorithm...");
  vtkSmartPointer<vtkTranslAxisCalibAlgo> translAxisCalibAlgo = vtkSmartPointer<vtkTranslAxisCalibAlgo>::New(); 
  translAxisCalibAlgo->SetInputs(trackedFrameList, spacing, inputDataType); 
  
  // Get translation axis calibration output 
  double translationAxisOrientation[3] = {0}; 
  if ( translAxisCalibAlgo->GetTranslationAxisOrientation(translationAxisOrientation) != PLUS_SUCCESS )
  {
    LOG_ERROR("Translation axis calibration failed!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Translation axis orientation: " << std::fixed << translationAxisOrientation[0] << "  " << translationAxisOrientation[1] << "  " << translationAxisOrientation[2]); 
  }

  // Get calibration error
  double errorMean(0), errorStdev(0); 
  if ( translAxisCalibAlgo->GetError(errorMean, errorStdev) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get translation axis calibration error!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Translation axis calibration error - mean: " << std::fixed << errorMean << "  stdev: " << errorStdev); 
  }
  
  LOG_INFO("Testing report table generation and saving into file..."); 
  vtkTable* reportTable = translAxisCalibAlgo->GetReportTable(); 
  if ( reportTable != NULL )
  {
    if ( vtkPlusLogger::Instance()->GetLogLevel() >= vtkPlusLogger::LOG_LEVEL_DEBUG ) 
      reportTable->Dump(25); 

    vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(reportTable, "./TranslationAxisCalibrationErrorReport.txt"); 
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
    htmlGenerator->SetTitle("Translation Axis Calibration Report"); 
    vtkSmartPointer<vtkGnuplotExecuter> gnuplotExecuter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
    gnuplotExecuter->SetWorkingDirectory("./"); 
    gnuplotExecuter->SetGnuplotCommand(inputGnuplotCommand.c_str()); 
    gnuplotExecuter->SetHideWindow(true); 
    translAxisCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 
    htmlGenerator->SaveHtmlPage("TranslationAxisCalibrationErrorReport.html"); 
  }

  std::ostringstream translAxisCalibAlgoStream; 
  translAxisCalibAlgo->PrintSelf(translAxisCalibAlgoStream, vtkIndent(0)); 
  LOG_DEBUG("TranslAxisCalibAlgo::PrintSelf: "<< translAxisCalibAlgoStream.str()); 
  
  //*********************************************************************
  // Compare result to baseline
  
  LOG_INFO("Comparing result with baseline..."); 

  vtkSmartPointer<vtkXMLDataElement> xmlBaseline = vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()); 
  vtkXMLDataElement* xmlTranslationAxisCalibrationBaseline = NULL; 
  if ( xmlBaseline != NULL )
  {
    xmlTranslationAxisCalibrationBaseline = xmlBaseline->FindNestedElementWithName(baselineXmlElementName.c_str()); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( xmlTranslationAxisCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find " << baselineXmlElementName.c_str() << " XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare TranslationAxisOrientation to baseline 
    double baseTranslationAxisOrientation[3]={0}; 
    if ( !xmlTranslationAxisCalibrationBaseline->GetVectorAttribute( baselineXmlAttributeName.c_str(), 3, baseTranslationAxisOrientation) )
    {
      LOG_ERROR("Unable to find " << baselineXmlAttributeName.c_str() << " XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseTranslationAxisOrientation[0] - translationAxisOrientation[0]) > DOUBLE_DIFF 
        || fabs(baseTranslationAxisOrientation[1] - translationAxisOrientation[1]) > DOUBLE_DIFF 
        || fabs(baseTranslationAxisOrientation[2] - translationAxisOrientation[2]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Translation axis orientation differ from baseline: current(" << translationAxisOrientation[0] << ", " << translationAxisOrientation[1] << ", " << translationAxisOrientation[2] 
        << ") base (" << baseTranslationAxisOrientation[0] << ", " << baseTranslationAxisOrientation[1] << ", " << baseTranslationAxisOrientation[2] <<  ")."); 
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
