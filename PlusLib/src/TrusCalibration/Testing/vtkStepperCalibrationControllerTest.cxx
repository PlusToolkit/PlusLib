#include "PlusConfigure.h"
#include "vtkStepperCalibrationController.h" 

#include "vtkCommand.h"
#include "vtkCallbackCommand.h" 
#include "vtksys/CommandLineArguments.hxx"  
#include "vtkTrackedFrameList.h"
#include "vtkXMLUtilities.h"

const double DOUBLE_DIFF = 0.0001; // in mm, used for comparing double numbers

int main (int argc, char* argv[])
{ 
  int numberOfFailures(0); 
  std::string inputProbeTranslationSeqMetafile;
  std::string inputTemplateTranslationSeqMetafile; 
  std::string inputProbeRotationSeqMetafile;
  std::string inputConfigFileName;

  std::string inputBaselineFileName;
  bool inputSaveResultToXml(false); 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--input-probe-translation-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeTranslationSeqMetafile, "Sequence metafile name of probe translation data.");
  cmdargs.AddArgument("--input-template-translation-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTemplateTranslationSeqMetafile, "Sequence metafile name of template translation data.");
  cmdargs.AddArgument("--input-probe-rotation-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeRotationSeqMetafile, "Sequence metafile name of probe rotation data.");

  cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
  cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
  cmdargs.AddArgument("--save-result", vtksys::CommandLineArguments::NO_ARGUMENT, &inputSaveResultToXml, "Enable calibration result saving to XML file");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	


  if ( !cmdargs.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  vtkSmartPointer<vtkStepperCalibrationController> stepperCalibrator = vtkSmartPointer<vtkStepperCalibrationController>::New();  

  LOG_INFO("Read configuration file..."); 

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  stepperCalibrator->ReadConfiguration(configRootElement); 

  LOG_INFO("Initialize stepper calibrator..."); 
  stepperCalibrator->Initialize(); 

  //************************************************************************************

  LOG_INFO("Read frames from metafile for probe rotation axis calibration..."); 
  vtkSmartPointer<vtkTrackedFrameList> probeRotationAxisTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  probeRotationAxisTrackedFrameList->ReadFromSequenceMetafile(inputProbeRotationSeqMetafile.c_str()); 

  LOG_INFO("Add frames for probe rotation axis calibration..."); 
  const int numberOfProbeRotationAxisFrames = probeRotationAxisTrackedFrameList->GetNumberOfTrackedFrames(); 
  int segmentedProbeRotFrameCounter(0); 
  std::string defaultFrameTransformName=probeRotationAxisTrackedFrameList->GetDefaultFrameTransformName();
  for ( int i = 0; i < numberOfProbeRotationAxisFrames; i++ )
  {
    vtkPlusLogger::PrintProgressbar((100.0 * i) / numberOfProbeRotationAxisFrames); 

    if ( stepperCalibrator->AddTrackedFrameData(probeRotationAxisTrackedFrameList->GetTrackedFrame(i), PROBE_ROTATION, defaultFrameTransformName.c_str()) )
    {
      // The segmentation was successful 
      segmentedProbeRotFrameCounter++; 
    }
  }
  vtkPlusLogger::PrintProgressbar(100); 

  LOG_INFO("Segmentation successful rate: " << 100 * (numberOfProbeRotationAxisFrames / segmentedProbeRotFrameCounter) ); 

  probeRotationAxisTrackedFrameList->Clear(); 

  

  //************************************************************************************

  LOG_INFO("Read frames from metafile for probe translation axis calibration..."); 
  vtkSmartPointer<vtkTrackedFrameList> probeTranslationAxisTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  probeTranslationAxisTrackedFrameList->ReadFromSequenceMetafile(inputProbeTranslationSeqMetafile.c_str()); 

  LOG_INFO("Add frames for probe translation axis calibration..."); 
  const int numberOfProbeTranslationAxisFrames = probeTranslationAxisTrackedFrameList->GetNumberOfTrackedFrames(); 
  int segmentedProbeTransFrameCounter(0); 
  std::string defaultFrameTransformNameProbeTrans=probeTranslationAxisTrackedFrameList->GetDefaultFrameTransformName();
  for ( int i = 0; i < numberOfProbeTranslationAxisFrames; i++ )
  {
    vtkPlusLogger::PrintProgressbar((100.0 * i) / numberOfProbeTranslationAxisFrames); 

    if ( stepperCalibrator->AddTrackedFrameData(probeTranslationAxisTrackedFrameList->GetTrackedFrame(i), PROBE_TRANSLATION, defaultFrameTransformNameProbeTrans.c_str()) )
    {
      // The segmentation was successful 
      segmentedProbeTransFrameCounter++; 
    }
  }
  vtkPlusLogger::PrintProgressbar(100); 

  LOG_INFO("Segmentation successful rate: " << 100 * (numberOfProbeTranslationAxisFrames / segmentedProbeTransFrameCounter) ); 

  probeTranslationAxisTrackedFrameList->Clear(); 

  //************************************************************************************

  LOG_INFO("Read frames from metafile for template translation axis calibration..."); 
  vtkSmartPointer<vtkTrackedFrameList> templateTranslationAxisTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  templateTranslationAxisTrackedFrameList->ReadFromSequenceMetafile(inputTemplateTranslationSeqMetafile.c_str()); 

  LOG_INFO("Add frames for template translation axis calibration..."); 
  const int numberOfTemplateTranslationAxisFrames = templateTranslationAxisTrackedFrameList->GetNumberOfTrackedFrames(); 
  int segmentedTemplTransFrameCounter(0); 
  std::string defaultFrameTransformNameTemplTrans=templateTranslationAxisTrackedFrameList->GetDefaultFrameTransformName();
  for ( int i = 0; i < numberOfTemplateTranslationAxisFrames; i++ )
  {
    vtkPlusLogger::PrintProgressbar((100.0 * i) / numberOfTemplateTranslationAxisFrames); 

    if ( stepperCalibrator->AddTrackedFrameData(templateTranslationAxisTrackedFrameList->GetTrackedFrame(i), TEMPLATE_TRANSLATION, defaultFrameTransformNameTemplTrans.c_str()) )
    {
      // The segmentation was successful 
      segmentedTemplTransFrameCounter++; 
    }
  }
  vtkPlusLogger::PrintProgressbar(100); 

  LOG_INFO("Segmentation successful rate: " << 100 * (numberOfTemplateTranslationAxisFrames / segmentedTemplTransFrameCounter) ); 

  templateTranslationAxisTrackedFrameList->Clear(); 

  //************************************************************************************

  LOG_INFO("Testing probe rotation axis calibration..."); 
  if ( !stepperCalibrator->CalibrateProbeRotationAxis() )
  {
    numberOfFailures++; 
  }

  LOG_INFO("Testing probe translation axis calibration..."); 
  if ( !stepperCalibrator->CalibrateProbeTranslationAxis() )
  {
    numberOfFailures++; 
  }

  LOG_INFO("Testing template translation axis calibration..."); 
  if ( !stepperCalibrator->CalibrateTemplateTranslationAxis() )
  {
    numberOfFailures++; 
  }

  if ( inputSaveResultToXml )
  {
    vtkSmartPointer<vtkXMLDataElement> stepperCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
    stepperCalibrationResults->SetName("StepperCalibrationResults"); 

    // ************* Save rotation axis calibration results to xml file ****************
    vtkSmartPointer<vtkXMLDataElement> rotationAxisCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
    rotationAxisCalibrationResults->SetName("RotationAxisCalibrationResult"); 
    rotationAxisCalibrationResults->SetParent(stepperCalibrationResults); 

    // Spacing calulation result 
    double *spacing = stepperCalibrator->GetSpacing(); 
    rotationAxisCalibrationResults->SetVectorAttribute("Spacing", 2, spacing); 

    // Center of rotation result 
    double * centerOfRotationInPixel = stepperCalibrator->GetCenterOfRotationPx(); 
    rotationAxisCalibrationResults->SetVectorAttribute("CenterOfRotationInPx", 2, centerOfRotationInPixel); 

    // Rotation axis calibration result
    double* rotationAxisOrientation = stepperCalibrator->GetProbeRotationAxisOrientation(); 
    rotationAxisCalibrationResults->SetVectorAttribute("RotationAxisOrientation", 3, rotationAxisOrientation); 

    // Rotation encoder calibration result 
    double rotationEncoderOffset = stepperCalibrator->GetProbeRotationEncoderOffset(); 
    rotationAxisCalibrationResults->SetDoubleAttribute("RotationEncoderOffset", rotationEncoderOffset); 

    double rotationEncoderScale = stepperCalibrator->GetProbeRotationEncoderScale(); 
    rotationAxisCalibrationResults->SetDoubleAttribute("RotationEncoderScale", rotationEncoderScale); 

    stepperCalibrationResults->AddNestedElement(rotationAxisCalibrationResults); 

    // ************* Save probe translation axis calibration results to xml file ****************
    vtkSmartPointer<vtkXMLDataElement> translationAxisCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
    translationAxisCalibrationResults->SetName("ProbeTranslationAxisCalibrationResult"); 
    translationAxisCalibrationResults->SetParent(stepperCalibrationResults); 

    double* probeTranslationAxisOrientation = stepperCalibrator->GetProbeTranslationAxisOrientation(); 
    translationAxisCalibrationResults->SetVectorAttribute("ProbeTranslationAxisOrientation", 3, probeTranslationAxisOrientation); 

    stepperCalibrationResults->AddNestedElement(translationAxisCalibrationResults); 

    // ************* Save template translation axis calibration results to xml file ****************
    vtkSmartPointer<vtkXMLDataElement> templateTranslationAxisCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
    templateTranslationAxisCalibrationResults->SetName("TemplateTranslationAxisCalibrationResult"); 
    templateTranslationAxisCalibrationResults->SetParent(stepperCalibrationResults); 

    double* templateTranslationAxisOrientation = stepperCalibrator->GetTemplateTranslationAxisOrientation(); 
    templateTranslationAxisCalibrationResults->SetVectorAttribute("TemplateTranslationAxisOrientation", 3, templateTranslationAxisOrientation); 

    stepperCalibrationResults->AddNestedElement(templateTranslationAxisCalibrationResults); 

    LOG_INFO("Save calibration results to XML file..."); 
    stepperCalibrationResults->PrintXML("StepperCalibrationResult.xml"); 
  }

  //*********************************************************************
  // Compare results to baseline 
  vtkSmartPointer<vtkXMLDataElement> stepperCalibrationResultsBaseline = vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()); 

  // Spacing calibration result
  vtkXMLDataElement* spacingCalibrationBaseline = NULL; 
  if ( stepperCalibrationResultsBaseline != NULL )
  {
    spacingCalibrationBaseline = stepperCalibrationResultsBaseline->FindNestedElementWithName("SpacingCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( spacingCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find SpacingCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare spacing to baseline 
    double *spacing = stepperCalibrator->GetSpacing(); 
    double baseSpacing[2]={0}; 
    if ( !spacingCalibrationBaseline->GetVectorAttribute("Spacing", 2, baseSpacing) )
    {
      LOG_ERROR("Unable to find Spacing XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseSpacing[0] - spacing[0]) > DOUBLE_DIFF 
        || fabs(baseSpacing[1] - spacing[1]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Spacing differ from baseline: current(" << spacing[0] << ", " << spacing[1] 
        << ") base (" << baseSpacing[0] << ", " << baseSpacing[1] << ")."); 
        numberOfFailures++;
      }
    }
  }

  // Rotation axis calibration result
  vtkXMLDataElement* rotationAxisCalibrationBaseline = NULL; 
  if ( stepperCalibrationResultsBaseline != NULL )
  {
    rotationAxisCalibrationBaseline = stepperCalibrationResultsBaseline->FindNestedElementWithName("RotationAxisCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( rotationAxisCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find RotationAxisCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare RotationAxisOrientation to baseline 
    double *rotationAxisOrientation = stepperCalibrator->GetProbeRotationAxisOrientation(); 
    double baseRotationAxisOrientation[3]={0}; 
    if ( !rotationAxisCalibrationBaseline->GetVectorAttribute("RotationAxisOrientation", 3, baseRotationAxisOrientation) )
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
        LOG_ERROR("Probe rotation axis orientation differ from baseline: current(" << rotationAxisOrientation[0] << ", " << rotationAxisOrientation[1] << ", " << rotationAxisOrientation[2] 
        << ") base (" << baseRotationAxisOrientation[0] << ", " << baseRotationAxisOrientation[1] << ", " << baseRotationAxisOrientation[2] <<  ")."); 
        numberOfFailures++;
      }
    }
  }

  // Rotation encoder calibration result
  vtkXMLDataElement* rotationEncoderCalibrationBaseline = NULL; 
  if ( stepperCalibrationResultsBaseline != NULL )
  {
    rotationEncoderCalibrationBaseline = stepperCalibrationResultsBaseline->FindNestedElementWithName("RotationEncoderCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  //TODO: compare center of rotation result to baseline
  /*
  if ( rotationEncoderCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find RotationEncoderCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare RotationEncoderOffset to baseline 
    double rotationEncoderOffset = stepperCalibrator->GetProbeRotationEncoderOffset(); 
    double baseRotationEncoderOffset=0; 
    if ( !rotationEncoderCalibrationBaseline->GetScalarAttribute("RotationEncoderOffset", baseRotationEncoderOffset) )
    {
      LOG_ERROR("Unable to find RotationEncoderOffset XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs( baseRotationEncoderOffset - rotationEncoderOffset ) > DOUBLE_DIFF )
      {
        LOG_ERROR("Probe rotation encoder offset differ from baseline: current(" << rotationEncoderOffset << ") base (" << baseRotationEncoderOffset <<  ")."); 
        numberOfFailures++;
      }
    }

    // Compare RotationEncoderScale to baseline 
    double rotationEncoderScale = stepperCalibrator->GetProbeRotationEncoderScale(); 
    double baseRotationEncoderScale=0; 
    if ( !rotationEncoderCalibrationBaseline->GetScalarAttribute("RotationEncoderScale", baseRotationEncoderScale) )
    {
      LOG_ERROR("Unable to find RotationEncoderScale XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs( baseRotationEncoderScale - rotationEncoderScale) > DOUBLE_DIFF )
      {
        LOG_ERROR("Probe rotation encoder scale differ from baseline: current(" << rotationEncoderScale << ") base (" << baseRotationEncoderScale <<  ")."); 
        numberOfFailures++;
      }
    }
  }
  */

  // Center of rotation calibration result
  vtkXMLDataElement* centerOfRotationCalibrationBaseline = NULL; 
  if ( stepperCalibrationResultsBaseline != NULL )
  {
    centerOfRotationCalibrationBaseline = stepperCalibrationResultsBaseline->FindNestedElementWithName("CenterOfRotationCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  //TODO: compare center of rotation result to baseline
  /*
  if ( centerOfRotationCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find CenterOfRotationCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare CenterOfRotationInPx to baseline 
    double *centerOfRotationInPx = stepperCalibrator->GetCenterOfRotationPx(); 
    double baseCenterOfRotationInPx[2]={0}; 
    if ( !centerOfRotationCalibrationBaseline->GetVectorAttribute("CenterOfRotationPx", 2, baseCenterOfRotationInPx) )
    {
      LOG_ERROR("Unable to find CenterOfRotationPx XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( baseCenterOfRotationInPx[0] != centerOfRotationInPx[0] 
      || baseCenterOfRotationInPx[1] != centerOfRotationInPx[1] )
      {
        LOG_ERROR("Center of rotation differ from baseline: current(" << centerOfRotationInPx[0] << ", " << centerOfRotationInPx[1] 
        << ") base (" << baseCenterOfRotationInPx[0] << ", " << baseCenterOfRotationInPx[1] << ")."); 
        numberOfFailures++;
      }
    }
  }
  */

  // Probe translation axis calibration result
  vtkXMLDataElement* probeTranslationAxisCalibrationBaseline = NULL; 
  if ( stepperCalibrationResultsBaseline != NULL )
  {
    probeTranslationAxisCalibrationBaseline = stepperCalibrationResultsBaseline->FindNestedElementWithName("ProbeTranslationAxisCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( probeTranslationAxisCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find ProbeTranslationAxisCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare ProbeTranslationAxisOrientation to baseline 
    double *probeTranslationAxisOrientation = stepperCalibrator->GetProbeTranslationAxisOrientation(); 
    double baseProbeTranslationAxisOrientation[3]={0}; 
    if ( !probeTranslationAxisCalibrationBaseline->GetVectorAttribute("ProbeTranslationAxisOrientation", 3, baseProbeTranslationAxisOrientation) )
    {
      LOG_ERROR("Unable to find ProbeTranslationAxisOrientation XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseProbeTranslationAxisOrientation[0] - probeTranslationAxisOrientation[0]) > DOUBLE_DIFF 
        || fabs(baseProbeTranslationAxisOrientation[1] - probeTranslationAxisOrientation[1]) > DOUBLE_DIFF 
        || fabs(baseProbeTranslationAxisOrientation[2] - probeTranslationAxisOrientation[2]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Probe translation axis orientation differ from baseline: current(" << probeTranslationAxisOrientation[0] << ", " << probeTranslationAxisOrientation[1] << ", " << probeTranslationAxisOrientation[2] 
        << ") base (" << baseProbeTranslationAxisOrientation[0] << ", " << baseProbeTranslationAxisOrientation[1] << ", " << baseProbeTranslationAxisOrientation[2] <<  ")."); 
        numberOfFailures++;
      }
    }
  }

  // Template translation axis calibration result
  vtkXMLDataElement* templateTranslationAxisCalibrationBaseline = NULL; 
  if ( stepperCalibrationResultsBaseline != NULL )
  {
    templateTranslationAxisCalibrationBaseline = stepperCalibrationResultsBaseline->FindNestedElementWithName("TemplateTranslationAxisCalibrationResult"); 
  }
  else
  {
    LOG_ERROR("Failed to read baseline file!");
    numberOfFailures++;
  }

  if ( templateTranslationAxisCalibrationBaseline == NULL )
  {
    LOG_ERROR("Unable to find TemplateTranslationAxisCalibrationResult XML data element in baseline: " << inputBaselineFileName); 
    numberOfFailures++; 
  }
  else
  {
    // Compare TemplateTranslationAxisOrientation to baseline 
    double *templateTranslationAxisOrientation = stepperCalibrator->GetTemplateTranslationAxisOrientation(); 
    double baseTemplateTranslationAxisOrientation[3]={0}; 
    if ( !templateTranslationAxisCalibrationBaseline->GetVectorAttribute("TemplateTranslationAxisOrientation", 3, baseTemplateTranslationAxisOrientation) )
    {
      LOG_ERROR("Unable to find TemplateTranslationAxisOrientation XML data element in baseline."); 
      numberOfFailures++; 
    }
    else
    {
      if ( fabs(baseTemplateTranslationAxisOrientation[0] - templateTranslationAxisOrientation[0]) > DOUBLE_DIFF 
        || fabs(baseTemplateTranslationAxisOrientation[1] - templateTranslationAxisOrientation[1]) > DOUBLE_DIFF 
        || fabs(baseTemplateTranslationAxisOrientation[2] - templateTranslationAxisOrientation[2]) > DOUBLE_DIFF )
      {
        LOG_ERROR("Template translation axis orientation differ from baseline: current(" << templateTranslationAxisOrientation[0] << ", " << templateTranslationAxisOrientation[1] << ", " << templateTranslationAxisOrientation[2] 
        << ") base (" << baseTemplateTranslationAxisOrientation[0] << ", " << baseTemplateTranslationAxisOrientation[1] << ", " << baseTemplateTranslationAxisOrientation[2] <<  ")."); 
        numberOfFailures++;
      }
    }
  }

  if ( numberOfFailures > 0 )
  {
    LOG_ERROR("Stepper calibration test failed!"); 
    return EXIT_FAILURE; 
  }

  LOG_INFO("vtkStepperCalibrationControllerTest exited successfully!!!"); 
  return EXIT_SUCCESS; 
} 
