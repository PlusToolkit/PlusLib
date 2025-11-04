#include "PlusConfigure.h"
#include "AtracsysTracker.h"
#include "vtksys/CommandLineArguments.hxx"

// STL includes
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------
using namespace Atracsys;

// global handle to Atracsys API wrapper
Tracker AtrTracker;
Tracker::DEVICE_TYPE DeviceType = Atracsys::Tracker::DEVICE_TYPE::UNKNOWN_DEVICE;

// constants
#define NUM_BACKGROUND_FRAMES 100
#define DEFAULT_NUM_DATA_FRAMES 100
#define ATRACSYS_MAX_FIDUCIALS 6

// for convenience
typedef Atracsys::Tracker::RESULT ATR_RESULT;
#define ATR_SUCCESS ATR_RESULT::SUCCESS

// data types
typedef std::vector<Fiducials> FiducialsSequence;
typedef std::vector<Marker> MarkerSequence;

// struct to hold geometry intrinsics
struct CustomGeometry
{
  std::string name;
  std::string description;
  std::string destPath;
  int geometryId;
  Fiducials fids;
};

// forward declarations
PlusStatus CollectFiducialSequence(FiducialsSequence& fidFrameList, int numFrames);
PlusStatus ProcessBackgroundFiducials(FiducialsSequence& fidFrameList, Fiducials& backgroundFids);
PlusStatus CaptureInitialGeometry(Fiducials& backgroundFids, Fiducials& fids);
void TransformMarkerCoordinateSystem(Fiducials& fids);
std::string WriteGeometryToString(const CustomGeometry& geom);
PlusStatus CollectMarkerSequence(CustomGeometry& geom, MarkerSequence& markerFrameList, int numFrames);
PlusStatus RefineMarkerGeometry(CustomGeometry& geom, MarkerSequence& markerFrameList);
PlusStatus WriteGeometryToIniFile(const CustomGeometry& geom);

// function to remove excessive precision for geometry coordinates writing
double remDecimals(double a, double prec)
{
  return (std::abs(a) > prec) ? a : 0.0;
};

//----------------------------------------------------------------------------
// 3D point/vector struct to help in processing fiducial data
struct vec3
{
  double x{ 0 };
  double y{ 0 };
  double z{ 0 };

  vec3() = default;
  vec3(const vec3& x1) = default;
  vec3(const Fiducial& f) : x{ f.xMm }, y{ f.yMm }, z{ f.zMm } {};
  vec3(double _x, double _y, double _z) : x{ _x }, y{ _y }, z{ _z } {};

  vec3& operator=(const vec3& other) = default;
  vec3& operator=(vec3&& other) = default;
  vec3 operator-() const
  {
    return { -this->x, -this->y, -this->z };
  }

  vec3& operator+=(const vec3& other)
  {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    return *this;
  }

  vec3& operator-=(const vec3& other)
  {
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    return *this;
  }

  vec3& operator/=(const double& value)
  {
    this->x /= value;
    this->y /= value;
    this->z /= value;
    return *this;
  }

  vec3& operator*=(const double& value)
  {
    this->x *= value;
    this->y *= value;
    this->z *= value;
    return *this;
  }

  vec3 operator+ (const vec3& x2) const
  {
    vec3 result{ *this };
    result += x2;
    return result;
  }

  vec3 operator- (const vec3& x1) const
  {
    vec3 result{ *this };
    result -= x1;
    return result;
  }

  vec3 operator* (const double value) const
  {
    return { this->x * value, this->y * value, this->z * value };
  }

  friend vec3 operator* (const double value, const vec3& x1)
  {
    return x1 * value;
  }

  vec3 operator/ (const double value) const
  {
    vec3 result{ *this };
    result /= value;
    return result;
  }

  double operator* (const vec3& x1) const
  {
    return x1.x * this->x + x1.y * this->y + x1.z * this->z;
  }

  double norm() const
  {
    return std::sqrt(this->normSqr());
  }

  double normSqr() const
  {
    return *this * *this;
  }

  vec3& normalize()
  {
    // check if norm() is not 0
    if (this->norm())
      *this /= this->norm();
    return *this;
  }

  vec3 cross(const vec3& x2) const
  {
    return { this->y * x2.z - this->z * x2.y, this->z * x2.x - this->x * x2.z, this->x * x2.y - this->y * x2.x };
  }
};

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Check command line arguments.
  bool printHelp(false);
  std::string markerName;
  std::string description;
  std::string destinationPath;
  int geometryId = -1;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  int numFrames = DEFAULT_NUM_DATA_FRAMES;
  int intTime = -1;
  int imgThresh = -1;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--marker-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &markerName, "Name of marker.");
  args.AddArgument("--description", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &description, "Decsription of marker (i.e. purpose, color, size, and any other desired metadata).");
  args.AddArgument("--geometryId", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &geometryId, "Id of the geometry we are creating. Must be unique.");
  args.AddArgument("--destination-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &destinationPath, "Where the generated marker geometry ini file will be written to.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace).");
  args.AddArgument("--num-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numFrames, "Number of frames to use in generating marker geometry ini file.");
  args.AddArgument("--int-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intTime, "Integration time for the camera.");
  args.AddArgument("--img-thresh", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &imgThresh, "Image compression threshold.");

  std::this_thread::sleep_for(std::chrono::milliseconds(4000));

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
  ATR_RESULT result = AtrTracker.Connect();
  if (result != ATR_SUCCESS && result != ATR_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_ERROR(AtrTracker.ResultToString(result));
    AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
    return PLUS_FAIL;
  }
  else if (result == ATR_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_WARNING(AtrTracker.ResultToString(result));
    AtrTracker.SetUserLEDState(255, 153, 0, 0); // orange LED
  }

  // If spryTrack, setup for onboard processing and disable extraneous marker info streaming
  AtrTracker.GetDeviceType(DeviceType);
  if (DeviceType == Tracker::DEVICE_TYPE::SPRYTRACK_180)
  {
    AtrTracker.SetSpryTrackProcessingType(Tracker::SPRYTRACK_IMAGE_PROCESSING_TYPE::PROCESSING_ONBOARD);
  }
  // Setting other options
  if (intTime > 0)
  {
    if (AtrTracker.SetOption("Image Integration Time", std::to_string(intTime)) != Atracsys::Tracker::SUCCESS)
    {
      AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
      return PLUS_FAIL;
    }
  }
  if (imgThresh > 0)
  {
    if (AtrTracker.SetOption("Image Compression Threshold", std::to_string(imgThresh)) != Atracsys::Tracker::SUCCESS)
    {
      AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
      return PLUS_FAIL;
    }
  }

  // Collect background frames
  LOG_INFO("\nBackground acquisition. Do NOT place the marker in front of the camera.");
  LOG_INFO("Press <ENTER> to continue.");
  std::cin.get();

  FiducialsSequence backgroundfidsFrameList;
  Fiducials backgroundFids; // list of fiducials visible in background
  AtrTracker.SetUserLEDState(0, 0, 255, 0); // blue LED
  if (CollectFiducialSequence(backgroundfidsFrameList, NUM_BACKGROUND_FRAMES) != PLUS_SUCCESS)
  {
    AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
    LOG_ERROR("Failed to collect background noise fiducial frames.");
    return PLUS_FAIL;
  }
  std::cout << "\n";
  ProcessBackgroundFiducials(backgroundfidsFrameList, backgroundFids);
  LOG_INFO("Background collection successful (" << backgroundFids.size() << " stray reflections).\n");

  // Create initial marker geometry
  AtrTracker.SetUserLEDState(0, 255, 0, 0); // green LED
  LOG_INFO("Place marker facing the camera in the first half of the working volume\nand rotate slowly the marker until geometry file is generated (left LED off).");
  LOG_INFO("Press <ENTER> to continue.");
  std::cin.get();

  AtrTracker.SetUserLEDState(255, 153, 0, 0); // orange LED
  LOG_INFO("Trying to get a good initial guess of the geometry...");
  Fiducials foregroundFids;
  if (CaptureInitialGeometry(backgroundFids, foregroundFids) != PLUS_SUCCESS)
  {
    AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
    LOG_ERROR("Failed to collect initial foreground fiducials.");
    return PLUS_FAIL;
  }

  AtrTracker.SetUserLEDState(0, 0, 255, 0); // blue LED
  LOG_INFO("Initial guess for marker geometry obtained.");

  CustomGeometry geom;
  geom.name = markerName;
  geom.description = description;
  geom.destPath = destinationPath;
  geom.geometryId = geometryId;
  geom.fids = foregroundFids;

  // Refining initial marker geometry
  LOG_INFO("Collecting marker frames to refine geometry...");
  MarkerSequence markerFrameList;
  if (CollectMarkerSequence(geom, markerFrameList, numFrames) != PLUS_SUCCESS)
  {
    AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
    LOG_ERROR("Failed to collect marker frames with initial geometry guess.");
    return PLUS_FAIL;
  }
  std::cout << "\n";

  LOG_INFO("Refining geometry...");
  if (RefineMarkerGeometry(geom, markerFrameList) != PLUS_SUCCESS)
  {
    AtrTracker.SetUserLEDState(255, 0, 0, 0); // red LED
    LOG_ERROR("Failed to refine geometry.");
    return PLUS_FAIL;
  }

  // Write marker ini file
  WriteGeometryToIniFile(geom);

  // turn LED off & end program
  AtrTracker.EnableUserLED(false);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
void ProgressBar(int i, int N, int barWidth = 70)
{
  std::cout << "[";
  int pos = barWidth * i / N;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << int(100 * i / N) << " %\r";
  std::cout.flush();
}

//----------------------------------------------------------------------------
PlusStatus CollectFiducialSequence(FiducialsSequence& fidFrameList, int numFrames)
{
  int m = 0;
  ProgressBar(m, numFrames);
  Fiducials fid3dFrame;
  std::map<std::string, std::string> events; // unused
  uint64_t ts = 0; // unused
  while (m < numFrames)
  {
    ATR_RESULT result = AtrTracker.GetFiducialsInFrame(fid3dFrame, events, ts);
    if (result == ATR_SUCCESS)
    {
      ProgressBar(++m, numFrames);
      fidFrameList.push_back(fid3dFrame);
    }
    else if (result == ATR_RESULT::ERROR_NO_FRAME_AVAILABLE)
    {
      continue;
    }
    else
    {
      LOG_ERROR(AtrTracker.ResultToString(result));
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus ProcessBackgroundFiducials(FiducialsSequence& fidFrameList, Fiducials& fids)
{
  Fiducials frame;
  // make sure fids is empty
  fids.clear();

  for (FiducialsSequence::size_type frameNum = 0; frameNum < fidFrameList.size(); frameNum++)
  {
    frame = fidFrameList[frameNum];
    // populate fids with list of fiducials appearing in the background
    std::copy(frame.begin(), frame.end(), std::inserter(fids, fids.end()));
  }

  // remove duplicate points in backgroundFids && resize
  std::sort(fids.begin(), fids.end());
  Fiducials::iterator it = std::unique(fids.begin(), fids.end());
  fids.resize(std::distance(fids.begin(), it));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus CaptureInitialGeometry(Fiducials& backFids, Fiducials& fids)
{
  Fiducials fid3dFrame;
  std::map<std::string, std::string> events; // unused
  uint64_t ts = 0; // unused
  while (true)
  {
    ATR_RESULT result = AtrTracker.GetFiducialsInFrame(fid3dFrame, events, ts);
    if (result == ATR_SUCCESS)
    {
      int deter = 0;
      // Remove those also part of the background
      fid3dFrame.erase(
        std::remove_if(fid3dFrame.begin(), fid3dFrame.end(),
          [&backFids](const Fiducial& item) {
            return std::find(backFids.begin(), backFids.end(), item) != backFids.end();
          }),
        fid3dFrame.end()
      );

      // Skip frame if foreground fiducial number too small or too large
      if (fid3dFrame.size() < 3 || fid3dFrame.size() > ATRACSYS_MAX_FIDUCIALS)
      {
        continue;
      }
      // Check that all fiducial probabilities are 1
      for (const auto& f : fid3dFrame)
      {
        deter += std::floor(f.probability);
      }
      if (deter == fid3dFrame.size())
      {
        fids = fid3dFrame;
        break;
      }
    }
    else if (result == ATR_RESULT::ERROR_NO_FRAME_AVAILABLE)
    {
      continue;
    }
    else
    {
      LOG_ERROR(AtrTracker.ResultToString(result));
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void TransformMarkerCoordinateSystem(Fiducials& fids)
{
  // Start processing fids positions to determine marker coordinate system.
  vec3 c(0.0, 0.0, 0.0);
  // Calculate the centroid of fiducials, also the origin of the new coordinate system.
  for (const auto& fid : fids)
  {
    c += vec3(fid);
  }
  c /= static_cast<double>(fids.size());

  // Calculate all fiducial-to-centroid distances and automatically store them in descending order.
  std::multimap<double, int, std::greater<double>> dists;
  for (int i = 0; i < fids.size(); ++i)
  {
    dists.insert({ (c - vec3(fids[i])).norm(), i });
  }
  // Look for the largest *unique* distance from the centroid, the corresponding fiducial will
  // define the x-axis.
  vec3 x;
  int xfidId{ -1 };
  for (auto prev = dists.begin(), curr = ++dists.begin(); curr != dists.end(); ++prev, ++curr)
  {
    if (std::abs(prev->first - curr->first) > 0.1)
    {
      xfidId = (prev == dists.begin()) ? prev->second : curr->second;
      x = vec3(fids[xfidId]);
      break;
    }
  }
  // Calculate all fiducial-to-x-axis distances and automatically store them in descending order.
  std::multimap<double, int, std::greater<double>> dists2;
  for (int i = 0; i < fids.size(); ++i)
  {
    double num = ((x - vec3(fids[i])).cross(x - c)).norm();
    dists2.insert({ num / (x - c).norm(), i });
  }
  // Look for the largest *unique* distance from x-axis, it will define the plane of the y-axis.
  vec3 p;
  int yfidId{ -1 };
  for (auto prev = dists2.begin(), curr = ++dists2.begin(); curr != dists2.end(); ++prev, ++curr)
  {
    if (std::abs(prev->first - curr->first) > 0.1)
    {
      yfidId = (prev == dists2.begin()) ? prev->second : curr->second;
      p = vec3(fids[yfidId]);
      break;
    }
  }
  // Calculate z-axis and y-axis.
  vec3 vx = (x - c).normalize();
  vec3 vz = (vx.cross(p - c)).normalize();
  vec3 vy = vz.cross(vx);

  // Create new fiducial order:
  // {fiducial defining x-axis, fiducial defining y-axis, remaining fiducials in descending order
  // of distance to x-axis}
  std::vector<int> newFidOrder{ xfidId, yfidId };
  for (auto d = dists2.begin(); d != dists2.end(); ++d)
  {
      if (d->second != xfidId && d->second != yfidId)
          newFidOrder.push_back(d->second);
  }
  // Apply new order to fiducials
  for (int i = 0; i < newFidOrder.size(); ++i) {
      size_t j = newFidOrder[i];
      while (j != i) {
          std::swap(fids[i], fids[j]);
          std::swap(newFidOrder[i], newFidOrder[j]);
          j = newFidOrder[i];
      }
  }
  // Store the coordinates of each fiducial in the new coordinate system.
  for (auto& fid : fids)
  {
    Fiducial f = fid;
    fid.xMm = (vec3(f) - c) * vx;
    fid.yMm = (vec3(f) - c) * vy;
    fid.zMm = (vec3(f) - c) * vz;
  }
}

//----------------------------------------------------------------------------
std::string WriteGeometryToString(const CustomGeometry& geom)
{
  std::ostringstream os;

  // write metadata
  os << "; " << geom.name << std::endl;
  os << "; " << geom.description << std::endl;
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  os << "; " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;

  // write geometry
  os << "[geometry]" << std::endl;
  os << "count=" << geom.fids.size() << std::endl;
  os << "id=" << geom.geometryId << std::endl;

  for (Fiducials::size_type i = 0; i < geom.fids.size(); i++)
  {
    os << "[fiducial" << i << "]" << std::endl;
    os << "x=" << geom.fids[i].xMm << std::endl;
    os << "y=" << geom.fids[i].yMm << std::endl;
    os << "z=" << geom.fids[i].zMm << std::endl;
  }

  return os.str();
}

//----------------------------------------------------------------------------
PlusStatus CollectMarkerSequence(CustomGeometry& geom, MarkerSequence& markerFrameList, int numFrames)
{
  std::string geoStr = WriteGeometryToString(geom);
  // Load initial marker for tracking
  int gid; // unused
  if (AtrTracker.LoadMarkerGeometryFromString(geoStr, gid) != Atracsys::Tracker::SUCCESS)
  {
    LOG_ERROR("Unable to load initial marker.");
    return PLUS_FAIL;
  }

  int m = 0;
  ProgressBar(m, numFrames);
  std::vector<Marker> markersFrame;
  std::map<std::string, std::string> events; // unused
  uint64_t ts = 0; // unused
  while (m < numFrames)
  {
    ATR_RESULT result = AtrTracker.GetMarkersInFrame(markersFrame, events, ts);
    if (result == ATR_SUCCESS)
    {
      if (markersFrame.size() == 1) // make sure there is only one marker tracked
      {
        ProgressBar(++m, numFrames);
        markerFrameList.push_back(markersFrame.front());
      }
    }
    else if (result == ATR_RESULT::ERROR_NO_FRAME_AVAILABLE)
    {
      continue;
    }
    else
    {
      LOG_ERROR(AtrTracker.ResultToString(result));
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus RefineMarkerGeometry(CustomGeometry& geom, MarkerSequence& markerFrameList)
{
  Fiducials accFids(geom.fids.size());
  for (int m = 0; m < markerFrameList.size(); ++m)
  {
    Fiducials fids = markerFrameList[m].GetFiducials();
    TransformMarkerCoordinateSystem(fids);
    // Accumulate fids coordinates for averaging
    for (int i = 0; i < fids.size(); ++i)
    {
      accFids[i].xMm += fids[i].xMm;
      accFids[i].yMm += fids[i].yMm;
      accFids[i].zMm += fids[i].zMm;
    }
  }

  // Complete the averaging and output the result
  for (int i = 0; i < accFids.size(); ++i)
  {
    accFids[i].xMm = remDecimals(accFids[i].xMm / markerFrameList.size(), 1e-4);
    accFids[i].yMm = remDecimals(accFids[i].yMm / markerFrameList.size(), 1e-4);
    accFids[i].zMm = remDecimals(accFids[i].zMm / markerFrameList.size(), 1e-4);
  }

  geom.fids = accFids;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus WriteGeometryToIniFile(const CustomGeometry& geom)
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
  std::ofstream file;
  file.open(fileName);
  file << WriteGeometryToString(geom);
  file.close();
  LOG_INFO("Done.");
  return PLUS_SUCCESS;
}