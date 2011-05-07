
#include <ctime>
#include <iostream>

#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkDataCollector.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"



void wait( double seconds )
{
  clock_t endwait;
  endwait = clock () + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {}
}



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
	
	vtkDataCollector* dataCollector = vtkDataCollector::New();
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
  
  std::cout << "Start data collector... ";
  dataCollector->Start();
  std::cout << "Done." << std::endl;
  
  for ( int i = 0; i < 10; ++ i )
    {
    std::cout << "Iteration: " << i << std::endl;
    wait( 0.5 );
    
		std::ostringstream ss;
		ss.precision( 2 ); 
		vtkSmartPointer<vtkMatrix4x4> tFrame2Tracker = vtkSmartPointer<vtkMatrix4x4>::New(); 
		if ( dataCollector->GetTracker()->IsTracking())
		{
			double timestamp(0); 
			long flags(0); 
			dataCollector->GetTransformWithTimestamp(tFrame2Tracker, timestamp, flags, dataCollector->GetMainToolNumber()); 

			if (flags & (TR_MISSING | TR_OUT_OF_VIEW )) 
			{
				ss	<< "Tracker out of view..."; 
			}
			else if ( flags & (TR_REQ_TIMEOUT) ) 
			{
				ss	<< "Tracker request timeout..."; 
			}
			else
			{
				ss	<< std::fixed 
					<< tFrame2Tracker->GetElement(0,0) << "   " << tFrame2Tracker->GetElement(0,1) << "   " << tFrame2Tracker->GetElement(0,2) << "   " << tFrame2Tracker->GetElement(0,3) << "\n"
					<< tFrame2Tracker->GetElement(1,0) << "   " << tFrame2Tracker->GetElement(1,1) << "   " << tFrame2Tracker->GetElement(1,2) << "   " << tFrame2Tracker->GetElement(1,3) << "\n"
					<< tFrame2Tracker->GetElement(2,0) << "   " << tFrame2Tracker->GetElement(2,1) << "   " << tFrame2Tracker->GetElement(2,2) << "   " << tFrame2Tracker->GetElement(2,3) << "\n"
					<< tFrame2Tracker->GetElement(3,0) << "   " << tFrame2Tracker->GetElement(3,1) << "   " << tFrame2Tracker->GetElement(3,2) << "   " << tFrame2Tracker->GetElement(3,3) << "\n"; 
			}
		}
		else
		{
			ss << "Unable to connect to tracker...";		
		}
    
    std::cout << ss.str() << std::endl;
    }
  
  std::cout << "Stop data collector... ";
  dataCollector->Stop();
  std::cout << "Done." << std::endl;
  
  std::cout << "Deleting data collector... ";
  dataCollector->Delete();
  std::cout << "Done." << std::endl;
  
  return 0;
}
