#include "PlusConfigure.h"
#include "AtracsysTracker.h"
#include "vtksys/CommandLineArguments.hxx"

// STL includes
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

// global handle to Atracsys API wrapper
AtracsysTracker Tracker;
AtracsysTracker::DEVICE_TYPE DeviceType = AtracsysTracker::UNKNOWN_DEVICE;

// constants
#define NUM_BACKGROUND_FRAMES 100
#define DEFAULT_NUM_DATA_FRAMES 100
#define ATRACSYS_MAX_FIDUCIALS 6

// for convenience
#define ATR_SUCCESS AtracsysTracker::ATRACSYS_RESULT::SUCCESS
typedef AtracsysTracker::ATRACSYS_RESULT ATRACSYS_RESULT;
typedef AtracsysTracker::Fiducial3D Fiducial3D;

// data types
typedef std::vector<Fiducial3D> fiducialFrame; // list of all fiducials in single frame, or all background fids
typedef std::vector<fiducialFrame> fiducialFrameList; // list of frames of fiducials

// struct to hold geometry intrinsics
struct MarkerGeometry
{
  std::string name;
  std::string description;
  std::string destPath;
  int geometryId;
  fiducialFrame fids;
};

// forward declare functions
PlusStatus CollectFiducials(fiducialFrameList& fidFrameList, int numFrames);
PlusStatus ProcessFiducials(fiducialFrameList& fidFrameList, fiducialFrame& backgroundFids);
PlusStatus PerformBackgroundSubtraction(fiducialFrame& backgroundFids, fiducialFrame& dataFids);
PlusStatus ZeroMeanFids(fiducialFrame& dataFids);
PlusStatus WriteGeometryIniFile(const MarkerGeometry geom);

int main(int argc, char** argv)
{
  // Check command line arguments.
  bool printHelp(false);
  bool backgroundSubtraction(false);
  std::string markerName;
  std::string description;
  std::string destinationPath;
  int geometryId = -1;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  int numFrames = DEFAULT_NUM_DATA_FRAMES;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--background-subtraction", vtksys::CommandLineArguments::NO_ARGUMENT, &backgroundSubtraction, "Remove background fiducials from data considered by the creator.");
  args.AddArgument("--marker-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &markerName, "Name of marker.");
  args.AddArgument("--description", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &description, "Decsription of marker (i.e. purpose, color, size, and any other desired metadata).");
  args.AddArgument("--geometryId", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &geometryId, "Id of the geometry we are creating. Must be unique.");
  args.AddArgument("--destination-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &destinationPath, "Where the generated marker geometry ini file will be written to.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace).");
  args.AddArgument("--num-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numFrames, "Number of frames to use in generating marker geometry ini file.");

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

  if (description.empty())
  {
    LOG_ERROR("--description argument is required!");
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (geometryId == -1)
  {
    LOG_ERROR("--geometryId argument is required!");
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
  fiducialFrame backgroundFids; // list of fids visible in background
  if (backgroundSubtraction)
  {
    if (CollectFiducials(backgroundFiducialFrameList, NUM_BACKGROUND_FRAMES) == PLUS_FAIL)
    {
      LOG_ERROR("Failed to collect background noise fiducial frames.");
      return PLUS_FAIL;
    }
    ProcessFiducials(backgroundFiducialFrameList, backgroundFids);

    // set LED red and wait for user to place marker in front of the camera
    Tracker.SetUserLEDState(255, 0, 0, 0);
    LOG_INFO("Background collection successful. Place marker in FOV of camera." << std::endl << "Press <ENTER> to continue.");
    std::cin.get();
  }

  LOG_INFO("Collecting data frames.");
  // set LED blue for collecting frames
  Tracker.SetUserLEDState(0, 0, 255, 0);

  // collect numFrames frames of fiducials
  fiducialFrameList dataFiducialFrameList;
  if (CollectFiducials(dataFiducialFrameList, numFrames) == PLUS_FAIL)
  {
    LOG_ERROR("Failed to collect data fiducial frames.");
    return PLUS_FAIL;
  }
  fiducialFrame dataFids;
  ProcessFiducials(dataFiducialFrameList, dataFids);

  // perform background subtraction and fiducial filtering
  if (PerformBackgroundSubtraction(backgroundFids, dataFids) != PLUS_SUCCESS)
  {
    // problem detected with captured fiducial data, don't generate marker
    return EXIT_FAILURE;
  }

  // create marker
  MarkerGeometry geom;
  geom.name = markerName;
  geom.description = description;
  geom.destPath = destinationPath;
  geom.geometryId = geometryId;
  ZeroMeanFids(dataFids);
  geom.fids = dataFids;

  // if too many fids, return
  if (geom.fids.size() > ATRACSYS_MAX_FIDUCIALS)
  {
    LOG_ERROR("Too many fiducials in frame (there were " << geom.fids.size() << " marker fids visible). Unable to create Atracsys marker with this many fiducials.");
    return EXIT_FAILURE;
  }
  else if (geom.fids.size() < 3)
  {
    LOG_ERROR("Too few fiducials in frame. Ensure marker is fully visible and try again.");
    return EXIT_FAILURE;
  }

  // write marker ini file
  WriteGeometryIniFile(geom);

  // turn LED off & end program
  Tracker.EnableUserLED(false);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
PlusStatus ProcessFiducials(fiducialFrameList& fidFrameList, fiducialFrame& fids)
{
  fiducialFrame frame;
  // make sure backgroundFids is empty
  fids.clear();

  for (fiducialFrameList::size_type frameNum = 0; frameNum < fidFrameList.size(); frameNum++)
  {
    frame = fidFrameList[frameNum];
    // populate backgroundFids with list of fiducials appearing in the background
    std::copy(frame.begin(), frame.end(), std::inserter(fids, fids.end()));
  }

  // remove duplicate points in backgroundFids && resize
  std::sort(fids.begin(), fids.end());
  fiducialFrame::iterator it = std::unique(fids.begin(), fids.end());
  fids.resize(std::distance(fids.begin(), it));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PerformBackgroundSubtraction(fiducialFrame& backgroundFids, fiducialFrame& dataFids)
{
  // subtract the backgroundFids from the dataFids
  fiducialFrame filteredDataFids;
  fiducialFrame::iterator backgroundIt, dataIt;
  for (dataIt = begin(dataFids); dataIt != end(dataFids); dataIt++)
  {
    bool equalityFound = false;
    for (backgroundIt = begin(backgroundFids); backgroundIt != end(backgroundFids); backgroundIt++)
    {
      if (*dataIt == *backgroundIt)
      {
        equalityFound = true;
      }
    }
    if (!equalityFound)
    {
      // element of dataFids is not in background, add to filteredDataFids
      filteredDataFids.push_back(*dataIt);
    }
  }

  // check that no data fiducials have non 1 probability
  for (fiducialFrame::size_type i = 0; i < filteredDataFids.size(); i++)
  {
    if (filteredDataFids[i].probability != 1)
    {
      LOG_ERROR("Fiducial with non 1 probability in data fiducials. Please retry marker creation ensuring that the marker is not moving and is in a good view position for the camera.");
      return PLUS_FAIL;
    }
  }

  // copy filteredDataFids back to dataFids
  dataFids.clear();
  dataFids = filteredDataFids;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus ZeroMeanFids(fiducialFrame& dataFids)
{
  // zero-mean the fiducials
  fiducialFrame zeroMeanFids;
  float cumulativeXmm = 0, cumulativeYmm = 0, cumulativeZmm = 0;
  fiducialFrame::const_iterator it;
  for (it = begin(dataFids); it != end(dataFids); it++)
  {
    cumulativeXmm += it->xMm;
    cumulativeYmm += it->yMm;
    cumulativeZmm += it->zMm;
  }
  float aveXmm = cumulativeXmm /= dataFids.size();
  float aveYmm = cumulativeYmm /= dataFids.size();
  float aveZmm = cumulativeZmm /= dataFids.size();
  for (it = begin(dataFids); it != end(dataFids); it++)
  {
    zeroMeanFids.push_back(
      AtracsysTracker::Fiducial3D(
        it->xMm - aveXmm,
        it->yMm - aveYmm,
        it->zMm - aveZmm,
        it->probability
      )
    );
  }
  dataFids = zeroMeanFids;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus WriteGeometryIniFile(const MarkerGeometry geom)
{
  // create file path
  std::string fileName;
  if (geom.destPath.empty())
  {
    fileName = "./" + geom.name + ".ini";
  }
  else
  {
    fileName = geom.destPath + '/' + geom.name + ".ini";
  }
  LOG_INFO("Writing marker geometry to: " << fileName);
  ofstream file;
  file.open(fileName);

  // write metadata
  file << ";; " << geom.name << std::endl;
  file << ";; " << geom.description << std::endl;
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  file << ";; " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;

  // write geometry
  file << "[geometry]" << std::endl;
  file << "count=" << geom.fids.size() << std::endl;
  file << "id=" << geom.geometryId << std::endl;

  for (fiducialFrame::size_type i = 0; i < geom.fids.size(); i++)
  {
    file << "[fiducial" << i << "]" << std::endl;
    file << "x=" << geom.fids[i].xMm << std::endl;
    file << "y=" << geom.fids[i].yMm << std::endl;
    file << "z=" << geom.fids[i].zMm << std::endl;
  }

  file << "[pivot]" << std::endl;
  file << "x=0.0000" << std::endl;
  file << "y=0.0000" << std::endl;
  file << "z=0.0000" << std::endl;

  file.close();
  return PLUS_SUCCESS;
}
