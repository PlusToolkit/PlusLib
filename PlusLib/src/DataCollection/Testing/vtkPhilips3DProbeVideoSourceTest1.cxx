/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkSonixVideoSourceTest1.cxx
  \brief Test basic connection to the Ultrasonix image acquisition

  If the --rendering-off switch is defined then the connection is established, images are 
  transferred for a few seconds, then the connection is closed (useful for automatic testing).
  If the --rendering-off switch is not defined then the live ultrasound image is displayed
  in a window (useful for quick interactive testing of the image transfer).
  \todo This is a test todo
  \ingroup PlusLibDataCollection
*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkImageData.h"
#include "vtkPhilips3DProbeVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkTrackedFrameList.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include <stdlib.h>

//----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  bool printHelp(false);
  std::string inputConfigFileName;
  std::string inputPhilipsIp;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
  args.AddArgument("--philips-ip", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputPhilipsIp, "IP address of the Philips scanner (overrides the IP address parameter defined in the config file).");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPhilips3DProbeVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkPhilips3DProbeVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  // Read config file
  LOG_DEBUG("Reading config file...");
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }
  
  vtkSmartPointer<vtkPhilips3DProbeVideoSource> philipsDevice = vtkSmartPointer<vtkPhilips3DProbeVideoSource>::New();
  philipsDevice->SetDeviceId("VideoDevice");
  philipsDevice->ReadConfiguration(configRootElement);

  if (!inputPhilipsIp.empty())
  {
    philipsDevice->SetIPAddress(inputPhilipsIp);
  }

  if ( philipsDevice->Connect() != PLUS_SUCCESS ) 
  {
    LOG_ERROR( "Unable to connect to Philips ie33 machine at " << philipsDevice->GetIPAddress() << ":" << philipsDevice->GetPort() ); 
    exit(EXIT_FAILURE); 
  }

  philipsDevice->StartRecording();

  // just run the recording for  a few seconds then exit
  LOG_DEBUG("Rendering disabled. Wait for just a few seconds to acquire data before exiting");
#ifdef _WIN32
  Sleep(3*1000);
#else
  usleep(3 * 1000000);
#endif

  vtkSmartPointer<vtkTrackedFrameList> frameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  vtkPlusChannel* channel = *philipsDevice->GetOutputChannelsStart();

  double oldTimestamp(0.0);
  channel->GetOldestTimestamp(oldTimestamp);
  channel->GetTrackedFrameList(oldTimestamp, frameList, 30);

  philipsDevice->StopRecording(); 
  philipsDevice->Disconnect();

  return EXIT_SUCCESS;
}

