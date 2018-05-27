#include "PlusConfigure.h"
#include <algorithm>
#include <array>
#include "AtracsysTracker.h"
#include <iostream>
#include <set>
#include <vector>
#include "vtksys/CommandLineArguments.hxx"

// global handle to Atracsys API wrapper
AtracsysTracker Tracker;
AtracsysTracker::DEVICE_TYPE DeviceType = AtracsysTracker::UNKNOWN_DEVICE;

// constants
#define NUM_BACKGROUND_FRAMES 100
#define DEFAULT_NUM_DATA_FRAMES 1000
#define BACKGROUND_RADIUS_MM 5

// for convenience
#define ATR_SUCCESS AtracsysTracker::ATRACSYS_RESULT::SUCCESS
typedef AtracsysTracker::ATRACSYS_RESULT ATRACSYS_RESULT;
typedef AtracsysTracker::Fiducial3D Fiducial3D;

// data types
typedef std::vector<Fiducial3D> fiducialFrame; // list of all fiducials in single frame, or all background fids
typedef std::vector<fiducialFrame> fiducialFrameList; // list of frames of fiducials

// forward declare functions
PlusStatus CollectFiducials(fiducialFrameList& fidFrameList, int numFrames);
PlusStatus ProcessBackgroundFiducials(fiducialFrameList& fidFrameList, fiducialFrame& backgroundFids);
PlusStatus CreateGeometry();
PlusStatus WriteGeometryFile();

int main(int argc, char** argv)
{
  // Check command line arguments.
  bool printHelp(false);
  bool backgroundSubtraction(true);
  std::string markerName;
  std::string description;
  std::string destinationPath;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  int numFrames = DEFAULT_NUM_DATA_FRAMES;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--background-subtraction", vtksys::CommandLineArguments::NO_ARGUMENT, &backgroundSubtraction, "Remove background fiducials from data considered by the creator.");
  args.AddArgument("--marker-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &markerName, "Name of marker.");
  args.AddArgument("--description", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &description, "Decsription of marker (i.e. purpose, color, size, and any other desired metadata).");
  args.AddArgument("--destination-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &destinationPath, "Where the generated marker geometry ini file will be written to.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace).");
  args.AddArgument("--num-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numFrames, "NUmber of frames to use in generating marker geometry ini file.");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (markerName.empty())
  {
    LOG_ERROR("--marker-name argument is required!");
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Logging at level " << vtkPlusLogger::Instance()->GetLogLevel() << " (" << vtkPlusLogger::Instance()->GetLogLevelString() << ") to file: " << vtkPlusLogger::Instance()->GetLogFileName());

  // Connect to tracker
  ATRACSYS_RESULT result = Tracker.Connect();
  if (result != ATR_SUCCESS && result != AtracsysTracker::ATRACSYS_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_ERROR(Tracker.ResultToString(result));
    return PLUS_FAIL;
  }
  else if (result == AtracsysTracker::ATRACSYS_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_WARNING(Tracker.ResultToString(result));
  }

  // get device type
  Tracker.GetDeviceType(DeviceType);

  // if spryTrack, setup for onboard processing and disable extraneous marker info streaming
  if (DeviceType == AtracsysTracker::DEVICE_TYPE::SPRYTRACK_180)
  {
    Tracker.SetSpryTrackProcessingType(AtracsysTracker::SPRYTRACK_IMAGE_PROCESSING_TYPE::PROCESSING_ONBOARD);
  }

  // set LED blue for collecting background
  Tracker.SetUserLEDState(0, 0, 255, 0);

  // collect background frames
  fiducialFrameList backgroundFiducialFrameList;
  fiducialFrame backgroundFiducials; // list of fids visible in background
  if (backgroundSubtraction)
  {
    if (CollectFiducials(backgroundFiducialFrameList, NUM_BACKGROUND_FRAMES) == PLUS_FAIL)
    {
      LOG_ERROR("Failed to collect background noise fiducial frames.");
      return PLUS_FAIL;
    }
    ProcessBackgroundFiducials(backgroundFiducialFrameList, backgroundFiducials);
  }

  // set LED red and wait for user to place marker in front of the camera
  Tracker.SetUserLEDState(255, 0, 0, 0);
  LOG_INFO("Background collection successful. Place marker in FOV of camera." << std::endl << "Press <ENTER> to continue.");
  std::cin.get();
  LOG_INFO("Collecting data frames.");

  // collect numFrames frames of fiducials
  fiducialFrameList dataFiducialFrameList;
  if (backgroundSubtraction)
  {
    if (CollectFiducials(dataFiducialFrameList, numFrames) == PLUS_FAIL)
    {
      LOG_ERROR("Failed to collect background noise fiducial frames.");
      return PLUS_FAIL;
    }
  }

  // process fiducials to see only the moving fids

  // write marker ini file

  // turn LED off & end program
  Tracker.EnableUserLED(false);
  return 0;
}

PlusStatus CollectFiducials(fiducialFrameList& fidFrameList, int numFrames)
{
  int m = 0;
  fiducialFrame fidFrame;
  while (m < numFrames)
  {
    // ensure fiducials vector is empty
    fidFrame.clear();
    ATRACSYS_RESULT result = Tracker.GetFiducialsInFrame(fidFrame);
    if (result == ATR_SUCCESS)
    {
      m++;
      fidFrameList.push_back(fidFrame);
    }
    else if (result == AtracsysTracker::ERROR_NO_FRAME_AVAILABLE)
    {
      continue;
    }
    else
    {
      LOG_ERROR(Tracker.ResultToString(result));
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

PlusStatus ProcessBackgroundFiducials(fiducialFrameList& fidFrameList, fiducialFrame& backgroundFids)
{
  fiducialFrame frame;
  // make sure backgroundFids is empty
  backgroundFids.clear();

  for (int frameNum = 0; frameNum < fidFrameList.size(); frameNum++)
  {
    // remove any fiducials with less than 1 probability value
    frame = fidFrameList[frameNum];
    frame.erase(
      std::remove_if(frame.begin(), frame.end(),
        [](const Fiducial3D & f) { return f.probability < 1.0; }),
      frame.end());

    // populate backgroundFids with list of fiducials appearing in the background
    std::copy(frame.begin(), frame.end(), std::inserter(backgroundFids, backgroundFids.end()));
  }

  // remove duplicate points in backgroundFids && resize
  std::sort(backgroundFids.begin(), backgroundFids.end());
  fiducialFrame::iterator it = std::unique(backgroundFids.begin(), backgroundFids.end());
  backgroundFids.resize(std::distance(backgroundFids.begin(), it));

  return PLUS_SUCCESS;
}

PlusStatus CreateGeometry()
{
  return PLUS_FAIL;
}

PlusStatus WriteGeometryFile()
{
  return PLUS_FAIL;
}