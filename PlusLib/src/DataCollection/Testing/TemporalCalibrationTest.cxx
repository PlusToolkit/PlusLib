#include "TemporalCalibration.h"

int main()
{
  // Test default constructor
  double defaultSamplingResolution = 0.001;
  std::string defaultTrackerFilename = "S:\\data\\TemporalTrackedUSCalibration\\CARS_DATA\\BaselineStudy\\Trial_01\\BufferDump_Tracker_20111229_122334.mha";
  std::string defaultVideoFilename = "S:\\data\\TemporalTrackedUSCalibration\\CARS_DATA\\BaselineStudy\\Trial_01\\BufferDump_Video_20111229_122334.mha";
  std::string defaultOutputFilename = "C:\\Documents and Settings\\moult\\My Documents";
  TemporalCalibration *testTemporalCalibrationObject = new TemporalCalibration(defaultTrackerFilename,
                                                                               defaultVideoFilename, 
                                                                               defaultOutputFilename, 
                                                                               defaultSamplingResolution);

  
  testTemporalCalibrationObject->CalculateTimeOffset();
  std::cout << testTemporalCalibrationObject->getTimeOffset() << std::endl;

  int i = 0;
  //-----------------------------------------------------------------------//

 


  return 0;
}
