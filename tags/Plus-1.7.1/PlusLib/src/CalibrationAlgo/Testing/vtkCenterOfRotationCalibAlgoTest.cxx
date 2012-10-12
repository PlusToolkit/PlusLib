/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkCenterOfRotationCalibAlgoTest.cxx 
  \brief This test computes center of rotation on a recorded data set and 
  compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "FidPatternRecognition.h"
#include "vtkCenterOfRotationCalibAlgo.h"
#include "vtkSpacingCalibAlgo.h"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"

// define tolerance used for comparing double numbers
#ifndef _WIN32
  const double DOUBLE_DIFF = LINUXTOLERANCE;
#else
  const double DOUBLE_DIFF = 0.0001;
#endif

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int numberOfFailures(0); 

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  bool printHelp(false);
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  std::string inputSequenceMetafile(""); 
  std::string inputBaselineFileName(""); 
  std::string inputConfigFileName(""); 

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Input sequence metafile name with path");	
  args.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Input xml baseline file name with path");	
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input xml config file name with path");	
    
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

  if ( inputSequenceMetafile.empty() || inputConfigFileName.empty() || inputBaselineFileName.empty() )
  {
    std::cerr << "input-translation-sequence-metafile, input-baseline-file-name and input-config-file-name are required arguments!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }  

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

	FidPatternRecognition patternRecognition;
  FidPatternRecognition::PatternRecognitionError error;
	patternRecognition.ReadConfiguration(configRootElement);

  LOG_INFO("Read center of rotation data from metafile...");

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafile); 
      return EXIT_FAILURE;
  }

  LOG_INFO("Create tracked frame indices vector..."); 
  std::vector<int> trackedFrameIndices(trackedFrameList->GetNumberOfTrackedFrames(), 0); 
  for ( unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    trackedFrameIndices[i]=i; 
  }

  LOG_INFO("Testing image data segmentation...");
  int numberOfSuccessfullySegmentedImages = 0; 
  patternRecognition.RecognizePattern(trackedFrameList, error, &numberOfSuccessfullySegmentedImages);
  LOG_INFO("Segmentation success rate: " << numberOfSuccessfullySegmentedImages << " out of " << trackedFrameList->GetNumberOfTrackedFrames()
    << " (" << (100.0 * numberOfSuccessfullySegmentedImages ) / trackedFrameList->GetNumberOfTrackedFrames() << "%)");

  LOG_INFO("Testing spacing computation...");
  vtkSmartPointer<vtkSpacingCalibAlgo> spacingCalibAlgo = vtkSmartPointer<vtkSpacingCalibAlgo>::New(); 
  spacingCalibAlgo->SetInputs(trackedFrameList, patternRecognition.GetFidLineFinder()->GetNWires()); 

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
  double spacingErrorMean(0), spacingErrorStdev(0); 
  if ( spacingCalibAlgo->GetError(spacingErrorMean, spacingErrorStdev) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get spacing calibration error!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Spacing calibration error - mean: " << std::fixed << spacingErrorMean << "  stdev: " << spacingErrorStdev); 
  }

  LOG_INFO("Testing center of rotation computation algorithm...");
  vtkSmartPointer<vtkCenterOfRotationCalibAlgo> centerOfRotationCalibAlgo = vtkSmartPointer<vtkCenterOfRotationCalibAlgo>::New(); 
  centerOfRotationCalibAlgo->SetInputs(trackedFrameList, trackedFrameIndices, spacing); 
  
  // Get center of rotation calibration output 
  double centerOfRotationPx[2] = {0}; 
  if ( centerOfRotationCalibAlgo->GetCenterOfRotationPx(centerOfRotationPx) != PLUS_SUCCESS )
  {
    LOG_ERROR("Center of rotation calibration failed!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Center of rotation (px): " << std::fixed << centerOfRotationPx[0] << "  " << centerOfRotationPx[1]); 
  }

  // Get calibration error
  double errorMean(0), errorStdev(0); 
  if ( centerOfRotationCalibAlgo->GetError(errorMean, errorStdev) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get center of rotation calibration error!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Center of rotation calibration error - mean: " << std::fixed << errorMean << "  stdev: " << errorStdev); 
  }
  
  LOG_INFO("Testing report table generation and saving into file..."); 
  vtkTable* reportTable = centerOfRotationCalibAlgo->GetReportTable(); 
  if ( reportTable != NULL )
  {
    if ( vtkPlusLogger::Instance()->GetLogLevel() >= vtkPlusLogger::LOG_LEVEL_DEBUG ) 
      reportTable->Dump(25); 

    vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(reportTable, "./CenterOfRotationCalibrationErrorReport.txt"); 
  }
  else
  {
    LOG_ERROR("Failed to get report table!"); 
    numberOfFailures++; 
  }

  LOG_INFO("Testing HTML report generation..."); 
  vtkSmartPointer<vtkHTMLGenerator> htmlGenerator = vtkSmartPointer<vtkHTMLGenerator>::New(); 
  htmlGenerator->SetTitle("Center of Rotation Calibration Report"); 
  vtkSmartPointer<vtkGnuplotExecuter> gnuplotExecuter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
  gnuplotExecuter->SetHideWindow(true); 
  spacingCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter); 
  centerOfRotationCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter); 
  htmlGenerator->SaveHtmlPage("CenterOfRotationCalibrationErrorReport.html"); 

  std::ostringstream centerOfRotationCalibAlgoStream; 
  centerOfRotationCalibAlgo->PrintSelf(centerOfRotationCalibAlgoStream, vtkIndent(0)); 
  LOG_DEBUG("CenterOfRotationCalibAlgo::PrintSelf: "<< centerOfRotationCalibAlgoStream.str()); 

  //*********************************************************************
  // Save results to file

  const char calibResultSaveFilename[]="CenterOfRotationCalibrationResults.xml";
  LOG_INFO("Save calibration results to XML file: "<<calibResultSaveFilename); 
  std::ofstream outFile; 
  outFile.open(calibResultSaveFilename);	
  outFile << "<CalibrationResults>" << std::endl;
  outFile << "  <CenterOfRotationCalibrationResult " << std::fixed << std::setprecision(8)
    << "CenterOfRotationPx=\""<<centerOfRotationPx[0]<<" "<<centerOfRotationPx[1]<<"\" "
    << "ErrorMean=\""<<errorMean<<"\" "
    << "ErrorStdev=\""<<errorStdev<<"\" "
    << " />" << std::endl;
  outFile << "</CalibrationResults>" << std::endl;  
  outFile.close(); 

  //*********************************************************************
  // Compare result to baseline
  
  LOG_INFO("Comparing result with baseline..."); 

  vtkSmartPointer<vtkXMLDataElement> xmlBaseline = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()));
  vtkXMLDataElement* xmlCenterOfRotationCalibrationBaseline = NULL; 
  if ( xmlBaseline != NULL )
  {
    xmlCenterOfRotationCalibrationBaseline = xmlBaseline->FindNestedElementWithName("CenterOfRotationCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
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
      if ( fabs(baseCenterOfRotationPx[0] - centerOfRotationPx[0]) > DOUBLE_DIFF 
        || fabs(baseCenterOfRotationPx[1] - centerOfRotationPx[1]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Center of rotation result in pixel differ from baseline: current(" << std::fixed << centerOfRotationPx[0] << ", " << centerOfRotationPx[1] 
        << ") base (" << baseCenterOfRotationPx[0] << ", " << baseCenterOfRotationPx[1] << ")."); 
        numberOfFailures++;
      }
    }

    // Compare errorMean 
    double baseErrorMean=0; 
    if ( !xmlCenterOfRotationCalibrationBaseline->GetScalarAttribute("ErrorMean", baseErrorMean) )
    {
      LOG_ERROR("Unable to find ErrorMean XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseErrorMean - errorMean) > DOUBLE_DIFF )
      {
        LOG_ERROR("Center of rotation mean error differ from baseline: current(" << std::fixed << errorMean << ") base (" << baseErrorMean << ")."); 
        numberOfFailures++;
      }
    }

    // Compare errorStdev
    double baseErrorStdev=0; 
    if ( !xmlCenterOfRotationCalibrationBaseline->GetScalarAttribute("ErrorStdev", baseErrorStdev) )
    {
      LOG_ERROR("Unable to find ErrorStdev XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseErrorStdev - errorStdev) > DOUBLE_DIFF )
      {
        LOG_ERROR("Center of rotation stdev of error differ from baseline: current(" << std::fixed << errorStdev << ") base (" << baseErrorStdev << ")."); 
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
