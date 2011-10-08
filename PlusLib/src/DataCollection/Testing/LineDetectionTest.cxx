#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkTrackedFrameList.h"
#include "PlusVideoFrame.h"
#include <iostream>

//----------------------------------------------------------------------------
PlusStatus LineDetection( PlusVideoFrame* videoFrame ); 

int main(int argc, char **argv)
{

  bool printHelp(false);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;
  std::string inputSequenceMetafile; 


  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
  args.AddArgument("--input-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Input sequence metafile name with path");	

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

  if ( inputSequenceMetafile.empty() )
  {
    std::cerr << "input-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafile); 
      return EXIT_FAILURE;
  }

  for ( int frame = 0; frame < trackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    if ( LineDetection( trackedFrameList->GetTrackedFrame(frame)->GetImageData() ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Line detection failed for frame#" << frame ); 
      return EXIT_FAILURE; 
    }
  }


  return EXIT_SUCCESS; 
}

PlusStatus LineDetection( PlusVideoFrame* videoFrame )
{

  //videoFrame->GetBufferPointer()

  return PLUS_SUCCESS; 
}
