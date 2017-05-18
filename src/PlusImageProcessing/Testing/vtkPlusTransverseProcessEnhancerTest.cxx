/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
\file vtkPlusTransverseProcessEnhancerTest.cxx
This is a program meant to test vtkPlusTransverseProcessEnhancer.cxx from the command line.
*/

#include "PlusConfigure.h"
#include "vtkPlusTransverseProcessEnhancer.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>


#include "PlusTrackedFrame.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkImageCast.h"


//----------------------------------------------------------------------------

//Global variables

//Magic numbers, taken from PlusB-bin\PlusLibData\ConfigFiles\ImageProcessing\PlusDeviceSet_Server_TransverseProcessEnhancerConfig.xml
const int numberOfScanLinesDefault = 200;
const int numberOfSamplesPerScanLineDefault = 210;
const double radiusStartMmDefault = 50.0;
const double radiusStopMmDefault = 120.0;
const int thetaStartDegDefault = -24;
const int thetaStopDegDefault = 24;

int argc;
char **argv;

//Variables that will be taken form command line arguments
std::string inputFileName;
std::string configFileName;
std::string outputFileName;
std::string linesImageFileName;
std::string intermediateImageFileName;
std::string processedLinesImageFileName;

int numberOfScanLines = NULL;
int numberOfSamplesPerScanLine = NULL;

double radiusStartMm = NULL;
double radiusStopMm = NULL;
int thetaStartDeg = NULL;
int thetaStopDeg = NULL;

bool convertToLinesImage = NULL;
bool thresholdingEnabled = NULL;
bool gaussianEnabled = NULL;
bool edgeDetectorEnabled = NULL;
bool islandRemovalEnabled = NULL;
bool erosionEnabled = NULL;
bool dilationEnabled = NULL;
bool reconvertBinaryToGreyscale = NULL;
bool returnToFanImage = NULL;

int gaussianStdDev = NULL;
int gaussianKernelSize = NULL;
int thresholdInValue = NULL;
int thresholdOutValue = NULL;
int lowerThreshold = NULL;
int upperThreshold = NULL;
int islandAreaThreshold = NULL;


int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

//miscellaneous global variables 
vtkSmartPointer<vtkPlusTransverseProcessEnhancer> enhancer;
vtkSmartPointer<vtkXMLDataElement> xmlData;


int readCommandLine(){
	/*
	readCommandLine() --> int

	Sets things up so that command line arguments can be read. Estrablishes what the 
	command line arguments are, and checks if the manditory arguments have been 
	given.

	Returns EXIT_FAILURE if an issue occured
	Returns EXIT_SUCCESS if not problems were encountered
	*/

	bool printHelp = false;
	vtksys::CommandLineArguments args;



	//Get command line arguments
	args.Initialize(argc, argv);
	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help");
	args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The filename for the input ultrasound sequence to process.");
	args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &configFileName, "The filename for input config file.");
	args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
	args.AddArgument("--lines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &linesImageFileName, "Optional output files for subsampled input image");
	args.AddArgument("--intermediate-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateImageFileName, "Optional output file");
	args.AddArgument("--processedlines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &processedLinesImageFileName, "Optional output files for processed subsampled image");

	args.AddArgument("--gaussian-standard-deviation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &gaussianStdDev, "Value for Gaussian Standard Deviation.");
	args.AddArgument("--threshold-in-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdInValue, "The in value for the threshold.");
	args.AddArgument("--threshold-out-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdOutValue, "The out value for the threshold.");
	args.AddArgument("--lower-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &lowerThreshold, "The value of the lower bound threshold.");
	args.AddArgument("--upper-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &upperThreshold, "The value of the upper bound threshold.");

	args.AddArgument("--gaussian-kernel-size", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &gaussianKernelSize, "Size of the Gaussian kernel.");
	args.AddArgument("--island-area-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &islandAreaThreshold, "Threshold for island area.");
	args.AddArgument("--number-of-scan-lines", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfScanLines, "The amount of scan lines to be used.");
	args.AddArgument("--samples-per-scan-line", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfSamplesPerScanLine, "The amount of samples in each scan line.");

	args.AddArgument("--convert-to-lines-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &convertToLinesImage, "if lines should be converted to an image.");
	args.AddArgument("--thresholding-enabled", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdingEnabled, "if thresholding should be enabled.");
	args.AddArgument("--gaussian-enabled", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &gaussianEnabled, "if Gaussian Deviation should be enabled.");
	args.AddArgument("--edge-detection", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &edgeDetectorEnabled, "if Edge Detecting should be enabled.");
	args.AddArgument("--island-removal", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &islandRemovalEnabled, "if Island Removal should be enabled.");
	args.AddArgument("--erosion-enabled", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &erosionEnabled, "if Erosion should be enabled.");
	args.AddArgument("--dilation-enabled", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &dilationEnabled, "if Dilation should be enabled.");
	args.AddArgument("--binary-to-greyscale", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &reconvertBinaryToGreyscale, "if Greyscale should be reconverted to Binary");
	args.AddArgument("--return-to-fan-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &returnToFanImage, "if the Fan image should be returned to how it was on input.");

	args.AddArgument("--radius-start", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &radiusStartMm, "Double for the starting point of the radius, measured in mm.");
	args.AddArgument("--radius-stop", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &radiusStopMm, "Double for the stopping point of the radius, measured in mm.");
	args.AddArgument("--theta-start", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thetaStartDeg, "Int for the starting point of theta, measured in degrees.");
	args.AddArgument("--theta-stop", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thetaStopDeg, "Int for the stopping point of theta, measured in degrees.");

	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

	if (!args.Parse())
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);


	//check for various non-optional command line arguments
	int commandCheckStatus = NULL;

	if (inputFileName.empty()){
		std::cerr << "--input-seq-file not found!" << std::endl;
		commandCheckStatus = EXIT_FAILURE;
	}
	if (outputFileName.empty()){
		std::cerr << "--output-seq-file not found!" << std::endl;
		commandCheckStatus = EXIT_FAILURE;
	}
	if (configFileName.empty()){
		std::cerr << "--config-file not found!" << std::endl;
		commandCheckStatus = EXIT_FAILURE;
	}

	if (commandCheckStatus == EXIT_FAILURE){
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int setEnhancerVariables(){
	/*
	setEnhancerVariables() --> int

	Sets the variables of enhancer based on arguments given via the command line.

	Returns EXIT_FAILURE if an issue occured
	returns EXIT_SUCCESS if no problems were encountered
	*/

	if (linesImageFileName.empty() == false){
		enhancer->SetLinesImageFileName(linesImageFileName);
	}
	if (intermediateImageFileName.empty() == false){
		enhancer->SetIntermediateImageFileName(intermediateImageFileName);
	}
	if (processedLinesImageFileName.empty() == false){
		enhancer->SetProcessedLinesImageFileName(processedLinesImageFileName);
	}

	if (numberOfScanLines == NULL){
		LOG_WARNING("--number-of-scan-lines not set. Setting to default value of " << numberOfScanLinesDefault);
		numberOfScanLines = numberOfScanLinesDefault;
	}
	enhancer->SetNumberOfScanLines(numberOfScanLines);

	if (numberOfSamplesPerScanLine == NULL){
		LOG_WARNING("--samples-per-scan-line not set. Setting to default value of " << numberOfSamplesPerScanLineDefault);
		numberOfSamplesPerScanLine = numberOfSamplesPerScanLineDefault;
	}
	enhancer->SetNumberOfSamplesPerScanLine(numberOfSamplesPerScanLine);

	if (radiusStartMm == NULL){
		radiusStartMm = radiusStartMmDefault;
	}
	if (radiusStopMm == NULL){
		radiusStopMm = radiusStopMmDefault;
	}
	if (thetaStartDeg == NULL){
		thetaStartDeg = thetaStartDegDefault;
	}
	if (thetaStopDeg == NULL){
		thetaStopDeg = thetaStopDegDefault;
	}

	if (convertToLinesImage != NULL){
		enhancer->SetConvertToLinesImage(convertToLinesImage);
	}
	if (thresholdingEnabled != NULL){
		enhancer->SetThresholdingEnabled(thresholdingEnabled);
	}
	if (gaussianEnabled != NULL){
		enhancer->SetGaussianEnabled(gaussianEnabled);
	}
	if (edgeDetectorEnabled != NULL){
		enhancer->SetEdgeDetectorEnabled(edgeDetectorEnabled);
	}
	if (islandRemovalEnabled != NULL){
		enhancer->SetIslandRemovalEnabled(islandRemovalEnabled);
	}
	if (erosionEnabled != NULL){
		enhancer->SetErosionEnabled(erosionEnabled);
	}
	if (dilationEnabled != NULL){
		enhancer->SetDilationEnabled(dilationEnabled);
	}
	if (reconvertBinaryToGreyscale != NULL){
		enhancer->SetReconvertBinaryToGreyscale(reconvertBinaryToGreyscale);
	}
	if (returnToFanImage != NULL){
		enhancer->SetReturnToFanImage(returnToFanImage);
	}

	if (gaussianStdDev != NULL){
		enhancer->SetGaussianStdDev(gaussianStdDev);
	}
	if (gaussianKernelSize != NULL){
		enhancer->SetGaussianKernelSize(gaussianKernelSize);
	}
	if (thresholdInValue != NULL){
		enhancer->SetThresholdInValue(thresholdInValue);
	}
	if (thresholdOutValue != NULL){
		enhancer->SetThresholdOutValue(thresholdOutValue);
	}
	if (lowerThreshold != NULL){
		enhancer->SetLowerThreshold(lowerThreshold);
	}
	if (upperThreshold != NULL){
		enhancer->SetUpperThreshold(upperThreshold);
	}
	if (islandAreaThreshold != NULL){
		enhancer->SetIslandAreaThreshold(islandAreaThreshold);
	}

	

	return EXIT_SUCCESS;
}


PlusStatus makeProcessXmlData(){
	/*
	makeProcessXmlData() --> PlusStatus

	Creates the XML data that will be used when testing the ReadConfiguration and
	WriteConfiguration mothods for enhancer.

	The data is saved to the global variable xmlData.
	*/

	//Create xml data for Processor

	xmlData = vtkSmartPointer<vtkXMLDataElement>::New();
	xmlData->SetName("Processor");
	xmlData->SetAttribute("Type", "vtkPlusTransverseProcessEnhancer");
	xmlData->SetIntAttribute("NumberOfScanLines", numberOfScanLines);
	xmlData->SetIntAttribute("NumberOfSamplesPerScanLine", numberOfSamplesPerScanLine);

	//Create xml data for ScanConversion
	XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(scanConversion, xmlData, "ScanConversion");
	scanConversion->SetAttribute("TransducerName", "Ultrasonix_C5-2");
	scanConversion->SetAttribute("TransducerGeometry", "CURVILINEAR");
	scanConversion->SetDoubleAttribute("RadiusStartMm", radiusStartMm);
	scanConversion->SetDoubleAttribute("RadiusStopMm", radiusStopMm);
	scanConversion->SetIntAttribute("ThetaStartDeg", thetaStartDeg);
	scanConversion->SetIntAttribute("ThetaStopDeg", thetaStopDeg);
	scanConversion->SetAttribute("OutputImageSizePixel", "820 616");
	scanConversion->SetAttribute("TransducerCenterPixel", "410 110");
	scanConversion->SetAttribute("OutputImageSpacingMmPerPixel", "0.1526 0.1526");


	return PLUS_SUCCESS;
}


int main(int Argc, char **Argv)
{
	argc = Argc;
	argv = Argv;

	if (readCommandLine() == EXIT_FAILURE){
		return EXIT_FAILURE;
	}
	
	enhancer = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();
	if (setEnhancerVariables() == EXIT_FAILURE){
		return EXIT_FAILURE;
	}

	// Read input and output sequence
	vtkSmartPointer<vtkPlusTrackedFrameList> trackedFrameListIn = vtkSmartPointer<vtkPlusTrackedFrameList>::New();
	trackedFrameListIn->ReadFromSequenceMetafile(inputFileName.c_str());

	vtkSmartPointer<vtkPlusTrackedFrameList> trackedFrameListOut = vtkSmartPointer<vtkPlusTrackedFrameList>::New();
	trackedFrameListOut->ReadFromSequenceMetafile(inputFileName.c_str());

	vtkSmartPointer<vtkImageCast> castToUchar = vtkSmartPointer<vtkImageCast>::New();
	castToUchar->SetOutputScalarTypeToUnsignedChar();

	int numberOfFrames = trackedFrameListIn->GetNumberOfTrackedFrames();
	std::cout << "Number of frames in input: " << numberOfFrames << std::endl;

	if (makeProcessXmlData()==PLUS_FAIL){
		return EXIT_FAILURE;
	}

	//Checks methods of vtkPlusTransverseProcessEnhancer for failure

	//test the abillity to Write to the config file
	LOG_DEBUG("Writing to config file...");
	std::cout << "Writing to Config file." << std::endl;
	if (enhancer->WriteConfiguration(xmlData) == PLUS_FAIL){
		LOG_DEBUG("Unable to write to config file.");
		std::cout << "Failed to write to Config file" << std::endl;
		return EXIT_FAILURE;
	}
	PlusCommon::XML::PrintXML(configFileName, xmlData);
	LOG_DEBUG("Writing to config file finished.");
	std::cout << "Done Writing Config file." << std::endl;


	//test the abillity to read to the config file that was just writen/created
	int configStatus = NULL;

	LOG_DEBUG("Reading config file...");
	std::cout << "Reading Config file." << std::endl;
	vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
	if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, configFileName.c_str()) == PLUS_FAIL){
		configStatus = EXIT_FAILURE;
	}
	if (configStatus != EXIT_FAILURE && enhancer->ReadConfiguration(configRootElement) == PLUS_FAIL){
		configStatus = EXIT_FAILURE;
	}
	if (configStatus == EXIT_FAILURE){
		LOG_ERROR("Unable to read configuration from file " << configFileName.c_str());
		std::cout << "Failed to read Config file" << std::endl;
		return EXIT_FAILURE;
	}
	LOG_DEBUG("Reading config file finished.");
	std::cout << "Done reading Config file." << std::endl;


	//test ProcessFrame method
	LOG_DEBUG("attempting to Process Frames.");
	std::cout << "Processing Frame." << std::endl;
	for (int frameIndex = 0; frameIndex <numberOfFrames; frameIndex++){
		PlusTrackedFrame* inputTrackedFrame = trackedFrameListIn->GetTrackedFrame(frameIndex);
		PlusTrackedFrame* outputTrackedFrame = trackedFrameListOut->GetTrackedFrame(frameIndex);
		if (enhancer->ProcessFrame(inputTrackedFrame, outputTrackedFrame) == PLUS_FAIL){
			LOG_DEBUG("Could not Process Frames. Failed on frame " << frameIndex);
			std::cout << "Failed processing frames. Failed on frame " << frameIndex << std::endl;
			return EXIT_FAILURE;
		}
	}
	LOG_DEBUG("Processed Frames successfully.");
	std::cout << "Done processing frames" << std::endl;

	trackedFrameListOut->SaveToSequenceMetafile(outputFileName.c_str());

	std::cout << "Compleated Test Successfully." << std::endl;
	return EXIT_SUCCESS;
}