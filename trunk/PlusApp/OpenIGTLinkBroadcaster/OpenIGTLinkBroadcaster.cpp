
#include "MainWidget.h"

#include <QtGui/QApplication>

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkDataCollector.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"



int main( int argc, char *argv[] )
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
		std::cerr << "input-config-file-name is required" << std::endl;
		exit( EXIT_FAILURE );
	}
  
	
	  // Prepare data collector object.
	
	vtkSmartPointer< vtkDataCollector > dataCollector = vtkSmartPointer< vtkDataCollector >::New();
	  dataCollector->ReadConfigurationFromFile( inputConfigFileName.c_str() );
  
	  if ( dataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
    {
    if ( inputVideoBufferMetafile.empty() )
      {
      std::cerr << "Video source metafile missing." << std::endl;
      return 1;
      }
     
    vtkSavedDataVideoSource* videoSource = static_cast< vtkSavedDataVideoSource* >( dataCollector->GetVideoSource() );
    videoSource->SetSequenceMetafile( inputVideoBufferMetafile.c_str() );
    }
  
	  if ( dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
    {
    if ( inputTrackerBufferMetafile.empty() )
      {
      std::cerr << "Tracker source metafile missing." << std::endl;
      return 1;
      }
    vtkSavedDataTracker* tracker = static_cast< vtkSavedDataTracker* >( dataCollector->GetTracker() );
    tracker->SetSequenceMetafile( inputTrackerBufferMetafile.c_str() );
    }
  
	
	QApplication app( argc, argv );
  
  
	MainWidget widget;
	  widget.SetDataCollector( dataCollector );
	  widget.show();
  
  
	return app.exec();
}
