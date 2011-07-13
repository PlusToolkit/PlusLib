#include "PlusConfigure.h"
#include <iostream>
#include <fstream> 
#include <strstream>
#include "UsFidSegResultFile.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h" 
#include "itkImageRegionConstIterator.h" 
#include "itkMetaImageSequenceIO.h"

#include "UltraSoundFiducialSegmentation.h"
#include "vtkCalibrationController.h"

#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtkSmartPointer.h"

#include "itkImageHelper.h"
#include "itkFcsvReader.h"
#include "itkLandmarkSpatialObject.h"


///////////////////////////////////////////////////////////////////
// Image type definition

typedef unsigned char          PixelType; // define type for pixel representation
const unsigned int             imageDimension = 2; 
const unsigned int             imageSequenceDimension = 3; 

typedef itk::Image< PixelType, imageDimension > ImageType;
typedef itk::Image< PixelType, imageSequenceDimension > ImageSequenceType;

typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;

///////////////////////////////////////////////////////////////////
// Other constants

static const int MAX_FIDUCIAL_COUNT = 50; 

static const float FIDUCIAL_POSITION_TOLERANCE = 0.1;  // in pixel
// Acceptance criteria for fiducial candidate (if the distance from the
// real fiducial position is less than segParams.then the fiducial is considered to be
// found).
static const double BASELINE_TO_ALGORITHM_TOLERANCE = 5; 
///////////////////////////////////////////////////////////////////

void SegmentImageSequence( vtkTrackedFrameList* trackedFrameList, std::ofstream &outFile, const std::string &inputTestcaseName, const std::string &inputImageSequenceFileName, vtkCalibrationController* calibrationController) 
{
	double sumFiducialNum = 0;// divide by framenum
	double sumFiducialCandidate = 0;// divide by framenum

	SegmentationParameters segParams = *(calibrationController->GetSegParameters());

	for (int currentFrameIndex=0; currentFrameIndex<trackedFrameList->GetNumberOfTrackedFrames(); currentFrameIndex++)
	{
    LOG_INFO("Frame: "<<currentFrameIndex);

		// Search in the whole image
		int SearchStartAtX=calibrationController->GetSearchStartAtX();
		int SearchStartAtY=calibrationController->GetSearchStartAtY();
		int SearchDimensionX=calibrationController->GetSearchDimensionX();
		int SearchDimensionY=calibrationController->GetSearchDimensionY();

		// Set to false if you don't want images produced after each morphological operation
		bool debugOutput=PlusLogger::Instance()->GetLogLevel()>=PlusLogger::LOG_LEVEL_TRACE; 

		std::ostrstream possibleFiducialsImageFilename; 
		possibleFiducialsImageFilename << inputTestcaseName << std::setw(3) << std::setfill('0') << currentFrameIndex << ".bmp" << std::ends; 
		
		SegmentationResults segResults;

		KPhantomSeg TheSegmentationStruct(trackedFrameList->GetFrameSize()[0],trackedFrameList->GetFrameSize()[1], 
			SearchStartAtX,SearchStartAtY, SearchDimensionX,SearchDimensionY,debugOutput,possibleFiducialsImageFilename.str());
		TheSegmentationStruct.segment(trackedFrameList->GetTrackedFrame(currentFrameIndex)->ImageData->GetBufferPointer(), segParams);
	
		TheSegmentationStruct.GetSegmentationResults(segResults);
		sumFiducialCandidate += segResults.GetNumDots();
		int numFid=0;
		for(int fidPosition = 0; fidPosition<segResults.GetFoundDotsCoordinateValue().size();fidPosition++)
		{ 
			std::vector<double> currentFid = segResults.GetFoundDotsCoordinateValue()[fidPosition]; 
			if (currentFid[0] != 0 || currentFid[1] != 0)
			{
				numFid++; 
			} 
		}
		sumFiducialNum = sumFiducialNum + numFid; 		
		
		possibleFiducialsImageFilename.rdbuf()->freeze(); 
		
		UsFidSegResultFile::WriteSegmentationResults(outFile, segResults, inputTestcaseName, currentFrameIndex, inputImageSequenceFileName);

		if (PlusLogger::Instance()->GetLogLevel()>=PlusLogger::LOG_LEVEL_DEBUG)
		{
			UsFidSegResultFile::WriteSegmentationResults(std::cout, segResults, inputTestcaseName, currentFrameIndex, inputImageSequenceFileName);
		}
	}


	double meanFid = sumFiducialNum/trackedFrameList->GetNumberOfTrackedFrames();
	double meanFidCandidate = sumFiducialCandidate/trackedFrameList->GetNumberOfTrackedFrames();
	UsFidSegResultFile::WriteSegmentationResultsStats(outFile,  meanFid, meanFidCandidate);
}

// return the number of differences
int CompareSegmentationResults(const std::string& inputBaselineFileName, const std::string& outputTestResultsFileName, SegmentationParameters &segParams)
{
	const bool reportWarningsAsFailure=true;
	int numberOfFailures=0;
	
	vtkXMLDataElement* currentRootElem = vtkXMLUtilities::ReadElementFromFile( outputTestResultsFileName.c_str()); 
	vtkXMLDataElement* baselineRootElem = vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str());

  bool writeFidFoundRatioToFile=PlusLogger::Instance()->GetLogLevel()>=PlusLogger::LOG_LEVEL_TRACE; 
	
	// check to make sure we have the right element
	if (baselineRootElem == NULL )
	{
		LOG_ERROR("Reading baseline data file failed: " << inputBaselineFileName);
		numberOfFailures++;
		return numberOfFailures;
	}
	if (currentRootElem == NULL )
	{
		LOG_ERROR("Reading newly generated data file failed: " << outputTestResultsFileName);
		numberOfFailures++;
		return numberOfFailures;
	}

	if (strcmp(baselineRootElem->GetName(), UsFidSegResultFile::TEST_RESULTS_ELEMENT_NAME) != 0)
	{
		LOG_ERROR("Baseline data file is invalid");
		numberOfFailures++;
		return numberOfFailures;
	}
	if (strcmp(currentRootElem->GetName(), UsFidSegResultFile::TEST_RESULTS_ELEMENT_NAME) != 0)
	{
		LOG_ERROR("newly generated data file is invalid");
		numberOfFailures++;
		return numberOfFailures;
	}

	std::ofstream outFileFidFindingResults; 
  if (writeFidFoundRatioToFile)
  {
	  outFileFidFindingResults.open("FiducialsFound.txt");
	  outFileFidFindingResults<< "Baseline to algorithm Tolerance: "<<BASELINE_TO_ALGORITHM_TOLERANCE<<" pixel(s)" <<std::endl;
	  outFileFidFindingResults<< "ThresholdTop: "<< segParams.GetThresholdImageTop() <<std::endl; 
	  outFileFidFindingResults<< "ThresholdBottom: "<< segParams.GetThresholdImageBottom() <<std::endl; 
  }
	for (int nestedElemInd=0; nestedElemInd<currentRootElem->GetNumberOfNestedElements(); nestedElemInd++)
	{
    LOG_DEBUG( "Current Frame: " << nestedElemInd);
		vtkSmartPointer<vtkXMLDataElement> currentElem=currentRootElem->GetNestedElement(nestedElemInd); 
		if (currentElem==NULL)  
		{
			LOG_WARNING("Invalid current data element");
			if (reportWarningsAsFailure) 
			{
				numberOfFailures++;			
			}
			continue;
		}
		if (strcmp(currentElem->GetName(),UsFidSegResultFile::TEST_CASE_ELEMENT_NAME)!=0)
		{ 
			// ignore all non-test-case elements
			continue;	
		}

		if (currentElem->GetAttribute(UsFidSegResultFile::ID_ATTRIBUTE_NAME)==NULL)  
		{
			LOG_WARNING("Current data element doesn't have an id");
			if (reportWarningsAsFailure) 
			{
				numberOfFailures++;
			}
			continue;
		}

		LOG_DEBUG("Comparing "<<currentElem->GetAttribute(UsFidSegResultFile::ID_ATTRIBUTE_NAME));

		vtkSmartPointer<vtkXMLDataElement> baselineElem=baselineRootElem->FindNestedElementWithNameAndId(UsFidSegResultFile::TEST_CASE_ELEMENT_NAME, currentElem->GetId());		

		if (baselineElem==NULL)
		{
			LOG_ERROR("Cannot find corresponding baseline element for current element "<<currentElem->GetId());
			numberOfFailures++;			
			continue;
		}
		
		if (strcmp(baselineElem->GetName(),UsFidSegResultFile::TEST_CASE_ELEMENT_NAME)!=0)
		{ 
			LOG_WARNING("Test case name mismatch");
			if (reportWarningsAsFailure) numberOfFailures++;			
			continue;	
		}
		
		vtkSmartPointer<vtkXMLDataElement> outputElementBaseline=baselineElem->FindNestedElementWithName("Output");
		vtkSmartPointer<vtkXMLDataElement> outputElementCurrent=currentElem->FindNestedElementWithName("Output");
		const int MAX_FIDUCIAL_COORDINATE_COUNT=MAX_FIDUCIAL_COUNT*2;

		int baselineSegmentationSuccess=0;
		int currentSegmentationSuccess=0;
		if (!outputElementBaseline->GetScalarAttribute("SegmentationSuccess", baselineSegmentationSuccess))
		{
			LOG_ERROR("newly generated segmentation angle is missing");
			numberOfFailures++;			
		}
		if (!outputElementCurrent->GetScalarAttribute("SegmentationSuccess", currentSegmentationSuccess))
		{
			LOG_ERROR("newly generated segmentation angle is missing");
			numberOfFailures++;			
		}
		
		if (baselineSegmentationSuccess!=currentSegmentationSuccess)
		{
			LOG_ERROR("SegmentationSuccess mismatch: current="<<currentSegmentationSuccess<<", baseline="<<baselineSegmentationSuccess);
			numberOfFailures++;
		}

		if (!baselineSegmentationSuccess)
		{
			// there is no baseline data, so we cannot do any more comparison
			continue;
		}

		double baselineFiducialPoints[MAX_FIDUCIAL_COORDINATE_COUNT]; // baseline fiducial Points
		memset(baselineFiducialPoints,0,sizeof(baselineFiducialPoints[0] * MAX_FIDUCIAL_COORDINATE_COUNT));
		int baselineFidPointsRead = outputElementBaseline->GetVectorAttribute("SegmentationPoints", MAX_FIDUCIAL_COORDINATE_COUNT, baselineFiducialPoints);

		// Count how many baseline fiducials are detected as fiducial candidates
    vtkSmartPointer<vtkXMLDataElement> fidCandidElement = currentElem->FindNestedElementWithName("FiducialPointCandidates");
    const char *fidCandid = "FiducialPointCandidates";
    if((fidCandidElement != NULL) && (strcmp(fidCandidElement->GetName(),fidCandid) == 0))
    {					
      int foundBaselineFiducials = 0;

      for(int b=0; b+1<baselineFidPointsRead; b+=2)// loop through baseline fiducials
      {
        for(int i=0; i<fidCandidElement->GetNumberOfNestedElements(); i++)// loop through all found potential fiducials
        { 
          double fidCandidates[2]={0,0};
          fidCandidElement->GetNestedElement(i)->GetVectorAttribute("Positon",2,fidCandidates);

          if ( fabs(baselineFiducialPoints[b] - fidCandidates[0])<BASELINE_TO_ALGORITHM_TOLERANCE 
            && fabs(baselineFiducialPoints[b+1] - fidCandidates[1])< BASELINE_TO_ALGORITHM_TOLERANCE)
          {
            LOG_DEBUG("Fiducial candidate ("<<fidCandidates[0]<< ", "<<fidCandidates[1]
              <<") matches the segmented baseline fiducial (" << baselineFiducialPoints[b]<< ", " << baselineFiducialPoints[b+1]<<")" ); 
            foundBaselineFiducials++; 
            break; 
          }

        }
      }

      LOG_DEBUG( "Found fiducials / Fiducial candidates: " << foundBaselineFiducials << " / " << fidCandidElement->GetNumberOfNestedElements()) ; 
      if (writeFidFoundRatioToFile)
      {
	      outFileFidFindingResults << nestedElemInd<< ": " <<foundBaselineFiducials << " / " << fidCandidElement->GetNumberOfNestedElements() <<std::endl; 
      }
    } 

		if (!currentSegmentationSuccess)
		{
			// the current segmentation was not successful, so we cannot do any more comparison
			continue;
		}

		double baselineAngle = 0; 
		double testingElementAngle=0;
		if(!outputElementBaseline->GetScalarAttribute("SegmentationQualityInAngleScore", baselineAngle))
		{
			LOG_ERROR("Cannot access baseline angle");
			numberOfFailures++;
		}
		else if (!outputElementCurrent->GetScalarAttribute("SegmentationQualityInAngleScore", testingElementAngle))
		{
			LOG_ERROR("newly generated segmentation angle is missing");
			numberOfFailures++;			
		}
		else
		{
			if(fabs(baselineAngle-testingElementAngle)>FIDUCIAL_POSITION_TOLERANCE)
			{
				LOG_ERROR("Angle mismatch: current="<<testingElementAngle<<", baseline="<<baselineAngle); 
				numberOfFailures++;
			}
		}

		double baselineIntensity = 0;
		double testingElementIntensity= 0;
		if(!outputElementBaseline->GetScalarAttribute("SegmentationQualityInIntensityScore",baselineIntensity))
		{
			LOG_ERROR("Cannot access baseline scalar");
			numberOfFailures++;
		} 
		else if(!outputElementCurrent->GetScalarAttribute("SegmentationQualityInIntensityScore",testingElementIntensity))
		{
			LOG_ERROR("Newly generated segmentation intensity is missing");
			numberOfFailures++;			
		} 
		else 
		{

			if(fabs(baselineIntensity-testingElementIntensity)>FIDUCIAL_POSITION_TOLERANCE)
			{
				LOG_ERROR("Intensity mismatch: current="<<testingElementIntensity<<", baseline="<<baselineIntensity);
				numberOfFailures++;
			}
		}

		double fiducialPositions[MAX_FIDUCIAL_COORDINATE_COUNT];		
		memset(fiducialPositions, 0, sizeof(fiducialPositions[0])*MAX_FIDUCIAL_COORDINATE_COUNT);		
		int fidCoordinatesRead=outputElementCurrent->GetVectorAttribute("SegmentationPoints", MAX_FIDUCIAL_COORDINATE_COUNT, fiducialPositions);

		if (baselineFidPointsRead!=fidCoordinatesRead)
		{
			LOG_ERROR("Number of current fiducials ("<<fidCoordinatesRead<<") differs from the number of baseline fiducials ("<<baselineFidPointsRead<<")"); 
			numberOfFailures++;
		}
		int fidCount=std::min(baselineFidPointsRead,fidCoordinatesRead);

		if (baselineFidPointsRead < 1)
		{
			LOG_ERROR("Cannot access baseline fiducial points"); 
			numberOfFailures++;
		} 
		else if (fidCoordinatesRead<1)
		{
			LOG_ERROR("Newly generated segmentation points are missing");
			numberOfFailures++;			
		}
		else if (baselineFidPointsRead %2 != 0)
		{
			LOG_ERROR("Unpaired baseline fiducial coordinates");
			numberOfFailures++;
		}
		else if (fidCoordinatesRead%2!=0)
		{
			LOG_ERROR("Unpaired Fiducial Coordinates");
			numberOfFailures++;			
		}
		else if (baselineFidPointsRead >MAX_FIDUCIAL_COORDINATE_COUNT)
		{
			LOG_WARNING("Too many baseline fiducials");
			if (reportWarningsAsFailure) numberOfFailures++;			
		}
		else if (fidCoordinatesRead>MAX_FIDUCIAL_COORDINATE_COUNT)
		{
			LOG_WARNING("Too many Fiducials");
			if (reportWarningsAsFailure) numberOfFailures++;			
		}

		else
		{
			for(int traverseFiducials=0; traverseFiducials<fidCount; ++traverseFiducials)
			{
				if(fabs(fiducialPositions[traverseFiducials]-baselineFiducialPoints[traverseFiducials])>FIDUCIAL_POSITION_TOLERANCE)
				{																	
					LOG_ERROR("Fiducial coordinate ["<<traverseFiducials<<"] mismatch: current="<<fiducialPositions[traverseFiducials]<<", baseline="<<baselineFiducialPoints[traverseFiducials]);
					numberOfFailures++;
				}
			}
		}		
	
	}

	baselineRootElem->Delete();
	currentRootElem->Delete();
  if (writeFidFoundRatioToFile)
  {
	  outFileFidFindingResults.close(); 
  }
	return numberOfFailures;
}

int main(int argc, char **argv)
{
	//TODO: Add to project itkUlteriusImageIOFactory
	//itk::UlteriusImageIOFactory::RegisterOneFactory();

	std::string inputImageSequenceFileName;
	std::string inputBaselineFileName;
	std::string inputTestcaseName;
	std::string inputTestDataDir;
	std::string inputConfigFileName;
	std::string outputTestResultsFileName;
	int thresholdTop = 0; 
	int thresholdBottom = 0; 
	std::string fiducialGeomString;	

	int verboseLevel=PlusLogger::LOG_LEVEL_WARNING;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-test-data-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestDataDir, "Test data directory");
	args.AddArgument("--input-img-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageSequenceFileName, "Filename of the input image sequence. Segmentation will be performed for all frames of the sequence.");
	args.AddArgument("--input-testcase-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestcaseName, "Name of the test case that will be printed to the output");
	args.AddArgument("--input-baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline results (fiducial coordinates, intensity, angle)");

	args.AddArgument("--output-result-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTestResultsFileName, "Name of file storing results of a new segmentation (fiducial coordinates, intensity, angle)");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");		

	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Calibration configuration file name");
		
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

  PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);
  PlusLogger::Instance()->SetLogLevel(verboseLevel);

	if (inputImageSequenceFileName.empty() || inputBaselineFileName.empty() || inputConfigFileName.empty())
	{
		std::cerr << "At lease one of the following parameters is missing: input-img-seq-file-name, input-baseline-file-name, input-config-file-name" << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkSmartPointer<vtkCalibrationController> calibrationController = vtkSmartPointer<vtkCalibrationController>::New();
	calibrationController->ReadConfiguration(inputConfigFileName.c_str());

  LOG_INFO("Read from metafile");
    std::string inputImageSequencePath=inputTestDataDir+"\\"+inputImageSequenceFileName;
    vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
    if ( trackedFrameList->ReadFromSequenceMetafile(inputImageSequencePath.c_str()) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to read sequence metafile: " << inputImageSequencePath); 
        return EXIT_FAILURE;
    }

	std::ofstream outFile; 
	outFile.open(outputTestResultsFileName.c_str());	
	UsFidSegResultFile::WriteSegmentationResultsHeader(outFile);
	UsFidSegResultFile::WriteSegmentationResultsParameters(outFile, *(calibrationController->GetSegParameters()), "");

  LOG_INFO("Segment image sequence");
	SegmentImageSequence(trackedFrameList.GetPointer(), outFile, inputTestcaseName, inputImageSequenceFileName, calibrationController); 

	UsFidSegResultFile::WriteSegmentationResultsFooter(outFile);
	outFile.close();

	LOG_DEBUG("Done!"); 
	
	if (!inputBaselineFileName.empty())
	{		
    LOG_INFO("Compare results");
		if (CompareSegmentationResults(inputBaselineFileName, outputTestResultsFileName, *(calibrationController->GetSegParameters()))!=0)
		{
			LOG_ERROR("Comparison of segmentation data to baseline failed");
			return EXIT_FAILURE;
		}
	}
	
 	return EXIT_SUCCESS;
}
