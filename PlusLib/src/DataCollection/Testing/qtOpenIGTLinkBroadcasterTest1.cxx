
#include <iostream>

#include <QTest>

#include "vtksys/CommandLineArguments.hxx"



int main( int argc, char** argv )
{
  std::string inputConfigFileName;
	std::string inputVideoBufferMetafile;
	std::string inputTrackerBufferMetafile;
	
  vtksys::CommandLineArguments args;
	args.Initialize( argc, argv );
  
	args.AddArgument( "--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
	                  &inputConfigFileName, "Name of the input configuration file." );
	args.AddArgument( "--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
	                  &inputVideoBufferMetafile, "Video buffer sequence metafile." );
	args.AddArgument( "--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
	                  &inputTrackerBufferMetafile, "Tracker buffer sequence metafile." );
	
	if ( ! args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit( EXIT_FAILURE );
	}
  
	if ( inputConfigFileName.empty() )
	{
		std::cerr << "input-config-file is required" << std::endl;
		exit( EXIT_FAILURE );
	}
  
  
	  // Prepare data collector object.
	
	vtkSmartPointer< vtkDataCollector > dataCollector = vtkSmartPointer< vtkDataCollector >::New();
	  dataCollector->ReadConfiguration( inputConfigFileName.c_str() );
  
  if ( dataCollector->GetAcquisitionType() == ACQUISITION_TYPE::SYNCHRO_VIDEO_SAVEDDATASET )
    {
    if ( inputVideoBufferMetafile.empty() )
      {
      std::cerr << "Video source metafile missing." << std::endl;
      return 1;
      }
     
    vtkSavedDataVideoSource* videoSource = static_cast< vtkSavedDataVideoSource* >( dataCollector->GetVideoSource() );
    videoSource->SetSequenceMetafile( inputVideoBufferMetafile.c_str() );
    }
  
  if ( dataCollector->GetTrackerType() == TRACKER_TYPE::TRACKER_SAVEDDATASET )
    {
    if ( inputTrackerBufferMetafile.empty() )
      {
      std::cerr << "Tracker source metafile missing." << std::endl;
      return 1;
      }
    vtkSavedDataTracker* tracker = static_cast< vtkSavedDataTracker* >( dataCollector->GetTracker() );
    tracker->SetSequenceMetafile( inputTrackerBufferMetafile.c_str() );
    }
  
  std::cout << "Initializing data collector... ";
  dataCollector->Initialize();
  std::cout << "Done." << std::endl;
  
  dataCollector->Start();
  
  for ( int i = 0; i < 10; ++ i )
    {
    
    }
  
  dataCollector->Stop();
  
  return EXIT_SUCCESS;
}
