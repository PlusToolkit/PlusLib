#include "PlusConfigure.h"
#include "vtkStepperCalibrationController.h" 

#include "vtkCommand.h"
#include "vtkCallbackCommand.h" 
#include "vtksys/CommandLineArguments.hxx"  
#include "vtkTrackedFrameList.h"


int main (int argc, char* argv[])
{ 
	int numberOfFailures(0); 
	std::string inputProbeTranslationSeqMetafile;
	std::string inputTemplateTranslationSeqMetafile; 
	std::string inputProbeRotationSeqMetafile;
	std::string inputConfigFileName;

	std::string inputBaselineFileName;

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-probe-translation-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeTranslationSeqMetafile, "Sequence metafile name of probe translation data.");
	cmdargs.AddArgument("--input-template-translation-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTemplateTranslationSeqMetafile, "Sequence metafile name of template translation data.");
	cmdargs.AddArgument("--input-probe-rotation-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeRotationSeqMetafile, "Sequence metafile name of probe rotation data.");

	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	VTK_LOG_TO_CONSOLE_ON; 

	vtkSmartPointer<vtkStepperCalibrationController> stepperCalibrator = vtkSmartPointer<vtkStepperCalibrationController>::New();  

	LOG_INFO("Read configuration file..."); 
	stepperCalibrator->ReadConfiguration(inputConfigFileName.c_str()); 
	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	LOG_INFO("Initialize stepper calibrator..."); 
	stepperCalibrator->Initialize(); 

	//************************************************************************************

	LOG_INFO("Read frames from metafile for probe rotation axis calibration..."); 
	vtkSmartPointer<vtkTrackedFrameList> probeRotationAxisTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
	probeRotationAxisTrackedFrameList->ReadFromSequenceMetafile(inputProbeRotationSeqMetafile.c_str()); 

	LOG_INFO("Add frames for probe rotation axis calibration..."); 
	const int numberOfProbeRotationAxisFrames = probeRotationAxisTrackedFrameList->GetNumberOfTrackedFrames(); 
	int segmentedProbeRotFrameCounter(0); 
	for ( int i = 0; i < numberOfProbeRotationAxisFrames; i++ )
	{
		PlusLogger::PrintProgressbar((100.0 * i) / numberOfProbeRotationAxisFrames); 

		if ( stepperCalibrator->AddTrackedFrameData(probeRotationAxisTrackedFrameList->GetTrackedFrame(i), PROBE_ROTATION) )
		{
			// The segmentation was successful 
			segmentedProbeRotFrameCounter++; 
		}
	}
	PlusLogger::PrintProgressbar(100); 

	LOG_INFO("Segmentation successful rate: " << 100 * (numberOfProbeRotationAxisFrames / segmentedProbeRotFrameCounter) ); 

	probeRotationAxisTrackedFrameList->Clear(); 
	
	if ( !stepperCalibrator->CalibrateProbeRotationAxis() )
	{
		numberOfFailures++; 
	}

	//************************************************************************************

	LOG_INFO("Read frames from metafile for probe translation axis calibration..."); 
	vtkSmartPointer<vtkTrackedFrameList> probeTranslationAxisTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
	probeTranslationAxisTrackedFrameList->ReadFromSequenceMetafile(inputProbeTranslationSeqMetafile.c_str()); 

	LOG_INFO("Add frames for probe translation axis calibration..."); 
	const int numberOfProbeTranslationAxisFrames = probeTranslationAxisTrackedFrameList->GetNumberOfTrackedFrames(); 
	int segmentedProbeTransFrameCounter(0); 
	for ( int i = 0; i < numberOfProbeTranslationAxisFrames; i++ )
	{
		PlusLogger::PrintProgressbar((100.0 * i) / numberOfProbeTranslationAxisFrames); 

		if ( stepperCalibrator->AddTrackedFrameData(probeTranslationAxisTrackedFrameList->GetTrackedFrame(i), PROBE_TRANSLATION) )
		{
			// The segmentation was successful 
			segmentedProbeTransFrameCounter++; 
		}
	}
	PlusLogger::PrintProgressbar(100); 

	LOG_INFO("Segmentation successful rate: " << 100 * (numberOfProbeTranslationAxisFrames / segmentedProbeTransFrameCounter) ); 

	probeTranslationAxisTrackedFrameList->Clear(); 
	
	if ( !stepperCalibrator->CalibrateProbeTranslationAxis() )
	{
		numberOfFailures++; 
	}

	//************************************************************************************

	LOG_INFO("Read frames from metafile for template translation axis calibration..."); 
	vtkSmartPointer<vtkTrackedFrameList> templateTranslationAxisTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
	templateTranslationAxisTrackedFrameList->ReadFromSequenceMetafile(inputTemplateTranslationSeqMetafile.c_str()); 

	LOG_INFO("Add frames for template translation axis calibration..."); 
	const int numberOfTemplateTranslationAxisFrames = templateTranslationAxisTrackedFrameList->GetNumberOfTrackedFrames(); 
	int segmentedTemplTransFrameCounter(0); 
	for ( int i = 0; i < numberOfTemplateTranslationAxisFrames; i++ )
	{
		PlusLogger::PrintProgressbar((100.0 * i) / numberOfTemplateTranslationAxisFrames); 

		if ( stepperCalibrator->AddTrackedFrameData(templateTranslationAxisTrackedFrameList->GetTrackedFrame(i), TEMPLATE_TRANSLATION) )
		{
			// The segmentation was successful 
			segmentedTemplTransFrameCounter++; 
		}
	}
	PlusLogger::PrintProgressbar(100); 

	LOG_INFO("Segmentation successful rate: " << 100 * (numberOfTemplateTranslationAxisFrames / segmentedTemplTransFrameCounter) ); 

	templateTranslationAxisTrackedFrameList->Clear(); 

	if ( !stepperCalibrator->CalibrateTemplateTranslationAxis() )
	{
		numberOfFailures++; 
	}

	//************************************************************************************
	

	if ( numberOfFailures > 0 )
	{
		LOG_ERROR("Test failed!"); 
		return EXIT_FAILURE; 
	}
	
    LOG_INFO("vtkStepperCalibrationControllerTest exited successfully!!!"); 
	return EXIT_SUCCESS; 
} 
