#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkTrackerBuffer.h"
#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtkTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"


int main(int argc, char **argv)
{
  int numberOfErrors(0); 
  VTK_LOG_TO_CONSOLE_ON; 

	bool printHelp(false);
  std::string inputMetafile;
  std::string inputBaselineReportFilePath(""); 
  int inputAveragedItemsForFiltering(20); 
  double inputMaxTimestampDifference(0.080); 
  double inputMinStdevReductionFactor(3.0); 

	int verboseLevel = PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--input-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMetafile, "Input sequence metafile.");
  args.AddArgument("--input-averaged-items-for-filtering", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAveragedItemsForFiltering, "Number of averaged items used for filtering (Default: 20).");
  args.AddArgument("--input-max-timestamp-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMaxTimestampDifference, "The maximum difference between the filtered and nonfiltered timestamps for each frame (Default: 0.08s).");
  args.AddArgument("--input-min-stdev-reduction-factor", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMinStdevReductionFactor, "Minimum factor that the filtering should reduces the standard deviation of the frame periods on filtered data (Default: 3.0 ).");
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
  LOG_INFO("Reading meta file..."); 
  vtkSmartPointer<vtkTrackedFrameList> trackerFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackerFrameList->ReadFromSequenceMetafile(inputMetafile.c_str()); 

  LOG_INFO("Copy buffer to tracker buffer..."); 
  int numberOfFrames = trackerFrameList->GetNumberOfTrackedFrames();
  vtkSmartPointer<vtkTrackerBuffer> trackerBuffer = vtkSmartPointer<vtkTrackerBuffer>::New(); 
  trackerBuffer->SetBufferSize(numberOfFrames); 
  trackerBuffer->SetAveragedItemsForFiltering(inputAveragedItemsForFiltering); 

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

    vtkSmartPointer<vtkMatrix4x4> identityMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

    trackerBuffer->AddTimeStampedItem(identityMatrix, TR_OK, frmnum, unfilteredtimestamp); 
  }

  PlusLogger::PrintProgressbar( 100 ); 
  std::cout << std::endl; 


  // Check filtering results 
  //************************

  // 1. The maximum difference between the filtered and nonfiltered timestamps for each frame shall be below a specified threshold (fabs(timestamp-unfilteredTimestamp) < maxTimestampDifference)
  double maxTimestampDifference(0); 
  for ( BufferItemUidType item = trackerBuffer->GetOldestItemUidInBuffer(); item <= trackerBuffer->GetLatestItemUidInBuffer(); ++item )
  {
    TrackerBufferItem bufferItem; 
    if ( trackerBuffer->GetTrackerBufferItem(item, &bufferItem, false) != ITEM_OK )
    {
      LOG_WARNING("Failed to get buffer item with UID: " << item ); 
      numberOfErrors++; 
      continue; 
    }
    
    double timestampDifference = fabs( bufferItem.GetUnfilteredTimestamp(0) - bufferItem.GetFilteredTimestamp(0) ); 
    if ( timestampDifference > maxTimestampDifference )
    {
       maxTimestampDifference = timestampDifference; 
    }
    if (  timestampDifference > inputMaxTimestampDifference )
    {
      LOG_ERROR("Difference between the filtered and nonfiltered timestamps are higher than the threshold (UID: " << item << ", timestamp diference: " << timestampDifference << ", threshold: " << inputMaxTimestampDifference << ")"); 
      numberOfErrors++; 
    }
  }

  LOG_INFO("Maximum filtered and unfiltered timestamp difference: " << maxTimestampDifference * 1000 << "ms"); 

  //2. The standard deviation of the frame periods in the filtered data should be better than without filtering (stdevFramePeriodsUnfiltered / stdevFramePeriodsFiltered < minStdevReductionFactor)
  vnl_vector<double>unfilteredFramePeriods( trackerBuffer->GetNumberOfItems() - 1 ); 
  vnl_vector<double>filteredFramePeriods( trackerBuffer->GetNumberOfItems() - 1 ); 
  int i = 0; 
  for ( BufferItemUidType item = trackerBuffer->GetOldestItemUidInBuffer(); item < trackerBuffer->GetLatestItemUidInBuffer(); ++item )
  {
    TrackerBufferItem bufferItem_1; 
    if ( trackerBuffer->GetTrackerBufferItem(item, &bufferItem_1, false) != ITEM_OK )
    {
      LOG_WARNING("Failed to get buffer item with UID: " << item ); 
      numberOfErrors++; 
      continue; 
    }

    TrackerBufferItem bufferItem_2; 
    if ( trackerBuffer->GetTrackerBufferItem(item + 1, &bufferItem_2, false) != ITEM_OK )
    {
      LOG_WARNING("Failed to get buffer item with UID: " << item + 1 ); 
      numberOfErrors++; 
      continue; 
    }

    unfilteredFramePeriods.put(i, (bufferItem_2.GetUnfilteredTimestamp(0) - bufferItem_1.GetUnfilteredTimestamp(0)) / (bufferItem_2.GetIndex() - bufferItem_1.GetIndex()) ); 
    filteredFramePeriods.put(i, (bufferItem_2.GetFilteredTimestamp(0) - bufferItem_1.GetFilteredTimestamp(0)) / (bufferItem_2.GetIndex() - bufferItem_1.GetIndex()) ); 
    i++; 
  }

  // Compute frame period means
  double unfilteredFramePeriodsMean = unfilteredFramePeriods.mean(); 
  double filteredFramePeriodsMean = filteredFramePeriods.mean(); 

  // Compute frame period stdev
  vnl_vector<double> diffFromMeanUnfilteredFramePeriods = unfilteredFramePeriods - unfilteredFramePeriodsMean; 
  double unfilteredFramePeriodsStd = sqrt(	diffFromMeanUnfilteredFramePeriods.squared_magnitude() / diffFromMeanUnfilteredFramePeriods.size() );

  LOG_INFO("Unfiltered frame periods mean: " << std::fixed << unfilteredFramePeriodsMean * 1000 << "ms stdev: " << unfilteredFramePeriodsStd * 1000 << "ms"); 

  vnl_vector<double> diffFromMeanFilteredFramePeriods = filteredFramePeriods - filteredFramePeriodsMean; 
  double filteredFramePeriodsStd = sqrt(	diffFromMeanFilteredFramePeriods.squared_magnitude() / diffFromMeanFilteredFramePeriods.size() );

  LOG_INFO("Filtered frame periods mean: " << std::fixed << filteredFramePeriodsMean * 1000 << "ms stdev: " << filteredFramePeriodsStd *1000 << "ms"); 

  LOG_INFO("Filtered data frame period reduction factor: " << std::fixed << unfilteredFramePeriodsStd / filteredFramePeriodsStd ); 

  if ( unfilteredFramePeriodsStd / filteredFramePeriodsStd < inputMinStdevReductionFactor )
  {
    LOG_ERROR("Filtered data frame period reduction factor is smaller than the threshold (factor: " << std::fixed << unfilteredFramePeriodsStd / filteredFramePeriodsStd << ", threshold: " << inputMinStdevReductionFactor << ")"); 
    numberOfErrors++; 
  }

  
  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New(); 
  if ( trackerBuffer->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to get time stamp report table!"); 
    numberOfErrors++; 
  }

  std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/TimestampReport.txt"); 

  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( timestampReportTable, reportFile.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write table to file in gnuplot format!"); 
    return PLUS_FAIL; 
  }

  if ( PlusLogger::Instance()->GetLogLevel() >= PlusLogger::LOG_LEVEL_DEBUG )
  {
    timestampReportTable->Dump(); 
  }

  if ( numberOfErrors != 0 )
  {
    LOG_INFO("Test failed!");
    return EXIT_FAILURE; 
  }

  LOG_INFO("Test completed successfully!");
	return EXIT_SUCCESS; 
 } 
