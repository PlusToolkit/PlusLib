/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 
/*
* This tool performs the sensor fusion algorithms of the Phidget Spatial Tracker 
* tool on a pre recorded set of Phidget Sensor data.  Sensor fusion parameters and
* algorithms to be used can be set at run-time via command line arguments.  
*
*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include <iomanip>
#include <iostream>

#include "vtkSmartPointer.h"
#include "vtkMetaImageSequenceIO.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransform.h"
#include "vtkImageData.h" 
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkMatrix3x3.h"
#include "AhrsAlgo.h"
#include "MadgwickAhrsAlgo.h"
#include "MahonyAhrsAlgo.h"
#include "vtkPhidgetSpatialTracker.h"


void Update(AhrsAlgo* ahrsAlgo, TrackedFrame *frame, int westAxisIndex, bool useTimestamps, vtkMatrix4x4* filteredTiltSensorToTrackerTransformReturn=NULL);

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::string inputImgFile;
  std::string outputImgFile;
  std::string ahrsAlgoName;
  std::string baselineImgFile;

  std::vector<double> ahrsAlgoGain;
  int westAxisIndex = 0;
  int numberOfRepeatedFramesForInitialization= 0; 
  std::vector<double> initialAhrsAlgoGain;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgFile, "File name of input image");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgFile, "File name of the image with the transform added");
  args.AddArgument("--ahrs-algo",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &ahrsAlgoName, "Ahrs Algorithm for Filtered Tilt Sensor.  Allowed inputs: MADGWICK_IMU, MAHONY_IMU");
  args.AddArgument("--ahrs-algo-gain", vtksys::CommandLineArguments::MULTI_ARGUMENT, &ahrsAlgoGain, "Opt1: Proportional Feedback Gain.  Opt2: Integral Feedback Gain (Integral gain used in Mahony only). "); 
  args.AddArgument("--west-axis-index",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &westAxisIndex, "Axis index to constrain to west");
  args.AddArgument("--initial-repeated-frame-number",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfRepeatedFramesForInitialization, "Number of frames to process at initial high gain for convergance");
  args.AddArgument("--initial-gain",vtksys::CommandLineArguments::MULTI_ARGUMENT, &initialAhrsAlgoGain, "Gain to use during initial frames for faster convergance");
  args.AddArgument("--baseline-seq-file",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &baselineImgFile, "Known good baseline file used to validate results for testing");

  // Input arguments error checking
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputImgFile.empty())
  {
    std::cerr << "--input-seq-file required" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (outputImgFile.empty())
  {
    std::cerr << "Missing --output-seq-file parameter. Specification of the output image file name is required." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Read transformations data 
  LOG_DEBUG("Reading input meta file..."); 
  vtkSmartPointer< vtkTrackedFrameList > frameList = vtkSmartPointer< vtkTrackedFrameList >::New();
  frameList->ReadFromSequenceMetafile( inputImgFile.c_str() );
  LOG_DEBUG("Reading input file completed"); 


  //set up Ahrs Algorithm 
  AhrsAlgo* ahrsAlgo = NULL;
  if (STRCASECMP("MADGWICK_IMU", ahrsAlgoName.c_str())==0)
  {
    ahrsAlgo=new MadgwickAhrsAlgo;
  }
  else if (STRCASECMP("MAHONY_IMU", ahrsAlgoName.c_str())==0)
  {
    ahrsAlgo=new MahonyAhrsAlgo;
  }
  else
  {
    LOG_ERROR("Unable to recognize AHRS algorithm type: " << ahrsAlgoName <<". Supported types: MADGWICK_IMU, MAHONY_IMU");
    exit(EXIT_FAILURE);
  }

  float proportionalGain=1.5;
  float integralGain=0.0;
  if (ahrsAlgoGain.size() >0)
  {
    proportionalGain = ahrsAlgoGain[0];
  }
  if (ahrsAlgoGain.size() >1)
  {
    integralGain = ahrsAlgoGain[1];
  }
  
  float initialProportionalGain=1.5;
  float initialIntegralGain=0.0;
  if (initialAhrsAlgoGain.size() >0)
  {
    initialProportionalGain = initialAhrsAlgoGain[0];
  }
  if (initialAhrsAlgoGain.size() >1)
  {
    initialIntegralGain = initialAhrsAlgoGain[1];
  }

  // Process the frames  

  // Initialization with the same frame
  ahrsAlgo->SetGain(initialProportionalGain, initialIntegralGain);
  TrackedFrame *frame0=frameList->GetTrackedFrame(0);
  TrackedFrame *frame1=frameList->GetTrackedFrame(1);
  double samplingFreqHz=125;
  double timeDiffSec=fabs(frame1->GetTimestamp()-frame0->GetTimestamp());
  if (timeDiffSec>1e-4)
  {
    samplingFreqHz=1/timeDiffSec;
  }
  ahrsAlgo->SetSampleFreqHz(samplingFreqHz);
  for (int frameIndex = 0; frameIndex<numberOfRepeatedFramesForInitialization; frameIndex++)
  {     
    Update(ahrsAlgo, frame0, westAxisIndex, false);
  }

  //set gain to normal running value after convergance time
  ahrsAlgo->SetGain(proportionalGain, integralGain);
  int nFrames = frameList->GetNumberOfTrackedFrames();
  vtkSmartPointer<vtkMatrix4x4> filteredTiltSensorToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  for (int frameIndex = 0; frameIndex<nFrames; frameIndex++)
  { 
    TrackedFrame *frame=frameList->GetTrackedFrame(frameIndex); 
    Update(ahrsAlgo, frame, westAxisIndex, true, filteredTiltSensorToTrackerTransform);
    //Update(ahrsAlgo, frame, westAxisIndex, false, filteredTiltSensorToTrackerTransform);
    frame->SetCustomFrameTransform(PlusTransformName("FilteredTiltSensor","Tracker"),filteredTiltSensorToTrackerTransform);
    frame->SetCustomFrameTransformStatus(PlusTransformName("FilteredTiltSensor","Tracker"),FIELD_OK);
  }

  vtkSmartPointer<vtkMetaImageSequenceIO> outputImgSeqFileWriter = vtkSmartPointer<vtkMetaImageSequenceIO>::New(); 
  outputImgSeqFileWriter->SetFileName(outputImgFile.c_str()); 
  outputImgSeqFileWriter->SetTrackedFrameList(frameList);   
  outputImgSeqFileWriter->SetImageOrientationInFile(US_IMG_ORIENT_XX);
  outputImgSeqFileWriter->Write(); 
  
  //baseline image should be provided for testing only
  if (!baselineImgFile.empty())
  {
      // Read transformations data 
      LOG_DEBUG("Reading baseline meta file..."); 
      vtkSmartPointer< vtkTrackedFrameList > baselineFrameList = vtkSmartPointer< vtkTrackedFrameList >::New();
      baselineFrameList->ReadFromSequenceMetafile( baselineImgFile.c_str() );
      LOG_DEBUG("Reading baseline file completed"); 

      int numberOfErrors = 0;

      //confirm that the post processed filtered tilt is the same as that in the baseline
      for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
      {
        TrackedFrame *frame=frameList->GetTrackedFrame(frameIndex);
        TrackedFrame *baselineFrame=baselineFrameList->GetTrackedFrame(frameIndex);

        vtkSmartPointer<vtkMatrix4x4> filteredTilt = vtkSmartPointer<vtkMatrix4x4>::New();
        frame->GetCustomFrameTransform(PlusTransformName("FilteredTiltSensor","Tracker"), filteredTilt);

        vtkSmartPointer<vtkMatrix4x4> baselineFilteredTilt = vtkSmartPointer<vtkMatrix4x4>::New();
        baselineFrame->GetCustomFrameTransform(PlusTransformName("FilteredTiltSensor","Tracker"), baselineFilteredTilt);
         
        //check for element by element equality
        for(int r = 0; r < 4; r++)
        {
          for(int c = 0; c < 4; c++)
          {
            if(filteredTilt->GetElement(r,c) != baselineFilteredTilt->GetElement(r,c))
            {
              numberOfErrors++;
            }
          }
        }

      }

      if(numberOfErrors > 0)
      {
        std::cout << "Test exited with failures!!!" << std::endl; 
        return EXIT_FAILURE;
      }
  }

  return EXIT_SUCCESS;
}


void Update(AhrsAlgo* ahrsAlgo, TrackedFrame *frame, int westAxisIndex, bool useTimestamps, vtkMatrix4x4* filteredTiltSensorToTrackerTransformReturn/*=NULL*/)
{
  double timestamp = frame->GetTimestamp();
  vtkSmartPointer<vtkMatrix4x4> gyroscopeMat = vtkSmartPointer<vtkMatrix4x4>::New();
  frame->GetCustomFrameTransform(PlusTransformName("Gyroscope","Tracker"), gyroscopeMat);
  vtkSmartPointer<vtkMatrix4x4> accelerometerMat = vtkSmartPointer<vtkMatrix4x4>::New();
  frame->GetCustomFrameTransform(PlusTransformName("Accelerometer","Tracker"), accelerometerMat);

  if (useTimestamps)
  {
    ahrsAlgo->UpdateIMUWithTimestamp(
      vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(0,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(1,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(2,3)), 
      accelerometerMat->GetElement(0,3), accelerometerMat->GetElement(1,3), accelerometerMat->GetElement(2,3), timestamp);
  }
  else
  {
     ahrsAlgo->UpdateIMU(
      vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(0,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(1,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(2,3)), 
      accelerometerMat->GetElement(0,3), accelerometerMat->GetElement(1,3), accelerometerMat->GetElement(2,3));
  }

  double rotQuat[4]={0};
  ahrsAlgo->GetOrientation(rotQuat[0],rotQuat[1],rotQuat[2],rotQuat[3]);

  double rotMatrix[3][3]={0};
  vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix); 

  double filteredDownVector_Sensor[4] = {rotMatrix[2][0],rotMatrix[2][1],rotMatrix[2][2],0};
  vtkMath::Normalize(filteredDownVector_Sensor);

  vtkSmartPointer<vtkMatrix4x4> filteredTiltSensorToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkPhidgetSpatialTracker::ConstrainWestAxis(filteredDownVector_Sensor, westAxisIndex, filteredTiltSensorToTrackerTransform);

  // write back the results to the FilteredTiltSensor_AHRS algorithm
  for (int c=0;c<3; c++)
  {
    for (int r=0;r<3; r++)
    {
      rotMatrix[r][c]= filteredTiltSensorToTrackerTransform->GetElement(r,c);
    }
  }
  double filteredTiltSensorRotQuat[4]={0};
  vtkMath::Matrix3x3ToQuaternion(rotMatrix,filteredTiltSensorRotQuat);
  ahrsAlgo->SetOrientation(filteredTiltSensorRotQuat[0],filteredTiltSensorRotQuat[1],filteredTiltSensorRotQuat[2],filteredTiltSensorRotQuat[3]);

  if (filteredTiltSensorToTrackerTransformReturn!=NULL)
  {
    filteredTiltSensorToTrackerTransformReturn->DeepCopy(filteredTiltSensorToTrackerTransform);
  }
}
