#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkTrackerBuffer.h"
#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtkTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMath.h"

double GetTranslationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix); 
double GetRotationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix); 

int main(int argc, char **argv)
{
  int numberOfErrors(0); 
  VTK_LOG_TO_CONSOLE_ON; 

	bool printHelp(false);
  std::string inputMetafile;
  std::string inputBaselineReportFilePath(""); 
  double inputMaxTranslationDifference(0.5); 
  double inputMaxRotationDifference(1.0); 

	int verboseLevel = PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--input-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMetafile, "Input sequence metafile.");
  args.AddArgument("--input-max-rotation-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMaxRotationDifference, "Maximum rotation difference in degrees (Default: 1 deg).");
  args.AddArgument("--input-max-translation-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMaxTranslationDifference, "Maximum translation difference (Default: 0.5 mm).");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
	
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

  if ( inputMetafile.empty() )
  {
    std::cerr << "input-metafile argument required!" << std::endl; 
    std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE); 
  }

	PlusLogger::Instance()->SetLogLevel(verboseLevel);
  PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  // Read buffer 
  LOG_INFO("Reading tracker meta file..."); 
  vtkSmartPointer<vtkTrackedFrameList> trackerFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackerFrameList->ReadFromSequenceMetafile(inputMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile from file: " << inputMetafile ); 
  }

  LOG_INFO("Copy buffer to tracker buffer..."); 
  int numberOfFrames = trackerFrameList->GetNumberOfTrackedFrames();
  vtkSmartPointer<vtkTrackerBuffer> trackerBuffer = vtkSmartPointer<vtkTrackerBuffer>::New(); 
  trackerBuffer->SetBufferSize(numberOfFrames); 

  for ( int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++ )
  {
    PlusLogger::PrintProgressbar( (100.0 * frameNumber) / numberOfFrames ); 

    const char* strUnfilteredTimestamp = trackerFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("UnfilteredTimestamp"); 
    double unfilteredtimestamp(0); 
    if ( strUnfilteredTimestamp != NULL )
    {
      unfilteredtimestamp = atof(strUnfilteredTimestamp); 
    }
    else
    {
      LOG_WARNING("Unable to read UnfilteredTimestamp field of frame #" << frameNumber); 
      numberOfErrors++; 
      continue; 
    }

    const char* strFrameNumber = trackerFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("FrameNumber"); 
    unsigned long frmnum(0); 
    if ( strFrameNumber != NULL )
    {
      frmnum = atol(strFrameNumber);
    }
    else
    {
      LOG_WARNING("Unable to read FrameNumber field of frame #" << frameNumber); 
      numberOfErrors++; 
      continue; 
    }

    double defaultTransform[16]={0}; 
    if ( !trackerFrameList->GetTrackedFrame(frameNumber)->GetDefaultFrameTransform(defaultTransform) )
    {
      LOG_ERROR("Unable to get default frame transform for frame #" << frameNumber); 
      numberOfErrors++; 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> defaultTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    defaultTransformMatrix->DeepCopy(defaultTransform); 

    trackerBuffer->AddTimeStampedItem(defaultTransformMatrix, TR_OK, frmnum, unfilteredtimestamp); 
  }

  PlusLogger::PrintProgressbar( 100 ); 
  std::cout << std::endl; 


  // Check interpolation results 
  //****************************

  double endTime(0); 
  if ( trackerBuffer->GetLatestTimeStamp(endTime) != ITEM_OK )
  {
    LOG_ERROR("Failed to get latest timestamp from tracker buffer!"); 
    exit(EXIT_FAILURE);  
  }

  double startTime(0); 
  if ( trackerBuffer->GetOldestTimeStamp(startTime) != ITEM_OK )
  {
    LOG_ERROR("Failed to get oldest timestamp from tracker buffer!"); 
    exit(EXIT_FAILURE);  
  }

  const double frameRate = trackerBuffer->GetFrameRate(); 

  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> prevmatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double newTime = startTime; 
  while ( newTime < endTime )
  {
    TrackerBufferItem bufferItem;
    if ( trackerBuffer->GetTrackerBufferItemFromTime(newTime, &bufferItem, false) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item from time: " << std::fixed << newTime ); 
      numberOfErrors++; 
      continue; 
    }
    
    long bufferIndex = bufferItem.GetIndex(); 

    if ( bufferItem.GetMatrix(matrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get matrix from buffer!"); 
      numberOfErrors++; 
      continue; 
    }

    if ( abs(newTime - startTime) < 0.0001 )
    {
      // this is the first matrix, we cannot compare it with the previous one. 
      prevmatrix->DeepCopy(matrix); 
      newTime += 1.0 / (frameRate * 5.0); 
      continue; 
    }


    double rotDiff = GetRotationError(matrix, prevmatrix); 
    if ( rotDiff > inputMaxRotationDifference )
    {
      LOG_ERROR("Rotation difference is larger than the max rotation difference (difference=" << std::fixed << rotDiff << ", threshold=" << inputMaxRotationDifference << ", itemIndex=" << bufferIndex << ", timestamp=" << newTime << ")!"); 
      numberOfErrors++; 
    }

    double transDiff = GetTranslationError(matrix, prevmatrix); 
    if ( transDiff > inputMaxTranslationDifference)
    {
      LOG_ERROR("Translation difference is larger than the max translation difference (difference=" << std::fixed << transDiff << ", threshold=" << inputMaxTranslationDifference << ", itemIndex=" << bufferIndex << ", timestamp=" << newTime << ")!"); 
      numberOfErrors++; 
    }

    prevmatrix->DeepCopy(matrix); 
    newTime += 1.0 / (frameRate * 5.0); 
  }

  if ( numberOfErrors != 0 )
  {
    LOG_INFO("Test failed!");
    return EXIT_FAILURE; 
  }

  LOG_INFO("Test completed successfully!");
	return EXIT_SUCCESS; 
 } 

 double GetTranslationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix)
{
	vtkSmartPointer<vtkTransform> baseTransform = vtkSmartPointer<vtkTransform>::New(); 
	baseTransform->SetMatrix(baseTransMatrix); 

	vtkSmartPointer<vtkTransform> currentTransform = vtkSmartPointer<vtkTransform>::New(); 
	currentTransform->SetMatrix(currentTransMatrix); 

	double bx = baseTransform->GetPosition()[0]; 
	double by = baseTransform->GetPosition()[1]; 
	double bz = baseTransform->GetPosition()[2]; 

	double cx = currentTransform->GetPosition()[0]; 
	double cy = currentTransform->GetPosition()[1]; 
	double cz = currentTransform->GetPosition()[2]; 

	// Euclidean distance
	double distance = sqrt( pow(bx-cx,2) + pow(by-cy,2) + pow(bz-cz,2) ); 

	return distance; 
}

double GetRotationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix)
{
	vtkSmartPointer<vtkMatrix4x4> diffTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
	vtkSmartPointer<vtkMatrix4x4> invCurrentTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
	
	vtkMatrix4x4::Invert(currentTransMatrix, invCurrentTransMatrix);  
	
	vtkMatrix4x4::Multiply4x4(baseTransMatrix, invCurrentTransMatrix, diffTransMatrix); 

	vtkSmartPointer<vtkTransform> diffTransform = vtkSmartPointer<vtkTransform>::New(); 
	diffTransform->SetMatrix(diffTransMatrix); 

	double angleDiff_rad= vtkMath::RadiansFromDegrees(diffTransform->GetOrientationWXYZ()[0]);

	double normalizedAngleDiff_rad = atan2( sin(angleDiff_rad), cos(angleDiff_rad) ); // normalize angle to domain -pi, pi 

	return vtkMath::DegreesFromRadians(normalizedAngleDiff_rad);
}
