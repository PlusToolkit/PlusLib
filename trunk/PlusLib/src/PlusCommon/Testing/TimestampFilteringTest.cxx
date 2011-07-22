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
  std::string inputBaselineReportFilePath; 

	int verboseLevel = PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--input-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMetafile, "Input sequence metafile.");
  args.AddArgument("--input-baseline-report-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineReportFilePath, "Baseline report file path");
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

  if ( vtksys::SystemTools::FilesDiffer(reportFile.c_str(), inputBaselineReportFilePath.c_str() ) )
  {
    LOG_ERROR("Sync report differ from baseline!"); 
    numberOfErrors++; 
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
