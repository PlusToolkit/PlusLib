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


void ConstrainWestAxis(double downVector_Sensor[3], int westAxisIndex, vtkMatrix4x4* sensorToTrackerTransform);

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::string inputImgFile;
  std::string outputImgFile;
  std::string ahrsAlgoName;

  std::vector<double> ahrsAlgoGain;
  int westAxisIndex = 0;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgFile, "File name of input image");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgFile, "File name of the image with the transform added");
  args.AddArgument("--ahrs-algo",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &ahrsAlgoName, "Ahrs Algorithm for Filtered Tilt Sensor.  Allowed inputs: MADGWICK_IMU, MAHONY_IMU");
  args.AddArgument("--ahrs-algo-gain", vtksys::CommandLineArguments::MULTI_ARGUMENT, &ahrsAlgoGain, "Opt1: Proportional Feedback Gain.  Opt2: Integral Feedback Gain (Integral gain used in Mahony only). "); 
  args.AddArgument("--west-axis-index",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &westAxisIndex, "Axis index to constrain to west");

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

  ahrsAlgo->SetGain(proportionalGain, integralGain);

  // Process the frames

  int nFrames = frameList->GetNumberOfTrackedFrames();

  //manually determine initial orientation and set into AHRS algorithm
  TrackedFrame *frame=frameList->GetTrackedFrame(0);
  float timestamp = frame->GetTimestamp();

  vtkSmartPointer<vtkMatrix4x4> gyroscopeMat = vtkSmartPointer<vtkMatrix4x4>::New();
  frame->GetCustomFrameTransform(PlusTransformName("Gyroscope","Tracker"), gyroscopeMat);

  vtkSmartPointer<vtkMatrix4x4> accelerometerMat = vtkSmartPointer<vtkMatrix4x4>::New();
  frame->GetCustomFrameTransform(PlusTransformName("Accelerometer","Tracker"), accelerometerMat);

  ahrsAlgo->UpdateIMUWithTimestamp(
    vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(0,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(1,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(2,3)), 
    accelerometerMat->GetElement(0,3), accelerometerMat->GetElement(1,3), accelerometerMat->GetElement(2,3), timestamp);

  double filteredDownVector_Sensor[4] = {accelerometerMat->GetElement(0,3), accelerometerMat->GetElement(1,3), accelerometerMat->GetElement(2,3),0};
  vtkMath::Normalize(filteredDownVector_Sensor);

  vtkSmartPointer<vtkMatrix4x4> filteredTiltSensorToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  ConstrainWestAxis(filteredDownVector_Sensor, westAxisIndex, filteredTiltSensorToTrackerTransform);

  // write back the results to the FilteredTiltSensor_AHRS algorithm orientation
  double rotMatrix[3][3]={0};
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

  frame->SetCustomFrameTransform(PlusTransformName("FilteredTiltSensor","Tracker"),filteredTiltSensorToTrackerTransform);
  frame->SetCustomFrameTransformStatus(PlusTransformName("FilteredTiltSensor","Tracker"),FIELD_OK);

  for (int frameIndex = 1; frameIndex<nFrames; frameIndex++)
  { 

    TrackedFrame *frame=frameList->GetTrackedFrame(frameIndex);
    float timestamp = frame->GetTimestamp();

    vtkSmartPointer<vtkMatrix4x4> gyroscopeMat = vtkSmartPointer<vtkMatrix4x4>::New();
    frame->GetCustomFrameTransform(PlusTransformName("Gyroscope","Tracker"), gyroscopeMat);

    vtkSmartPointer<vtkMatrix4x4> accelerometerMat = vtkSmartPointer<vtkMatrix4x4>::New();
    frame->GetCustomFrameTransform(PlusTransformName("Accelerometer","Tracker"), accelerometerMat);
 
    ahrsAlgo->UpdateIMUWithTimestamp(
      vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(0,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(1,3)), vtkMath::RadiansFromDegrees(gyroscopeMat->GetElement(2,3)), 
      accelerometerMat->GetElement(0,3), accelerometerMat->GetElement(1,3), accelerometerMat->GetElement(2,3), timestamp);

    double rotQuat[4]={0};
    ahrsAlgo->GetOrientation(rotQuat[0],rotQuat[1],rotQuat[2],rotQuat[3]);

    double rotMatrix[3][3]={0};
    vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix); 

    double filteredDownVector_Sensor[4] = {rotMatrix[2][0],rotMatrix[2][1],rotMatrix[2][2],0};
    vtkMath::Normalize(filteredDownVector_Sensor);

    vtkSmartPointer<vtkMatrix4x4> filteredTiltSensorToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    ConstrainWestAxis(filteredDownVector_Sensor, westAxisIndex, filteredTiltSensorToTrackerTransform);

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

    frame->SetCustomFrameTransform(PlusTransformName("FilteredTiltSensor","Tracker"),filteredTiltSensorToTrackerTransform);
    frame->SetCustomFrameTransformStatus(PlusTransformName("FilteredTiltSensor","Tracker"),FIELD_OK);
  }

  vtkSmartPointer<vtkMetaImageSequenceIO> outputImgSeqFileWriter = vtkSmartPointer<vtkMetaImageSequenceIO>::New(); 
  outputImgSeqFileWriter->SetFileName(outputImgFile.c_str()); 
  outputImgSeqFileWriter->SetTrackedFrameList(frameList); 
  outputImgSeqFileWriter->SetImageOrientationInFile(frameList->GetImageOrientation());
  outputImgSeqFileWriter->Write(); 

  return EXIT_SUCCESS;
}

//Forces axis ni the transformation matrix to align with the 'west' direction in the tracker frame
void ConstrainWestAxis(double downVector_Sensor[3], int westAxisIndex, vtkMatrix4x4* lastSensorToTrackerTransform)
{
  if (westAxisIndex<0 || westAxisIndex>=3)
  {
    westAxisIndex = 1;
  }

  // Sensor axis vector that is assumed to always point to West. This is chosen so that cross(westVector_Sensor, downVector_Sensor) = southVector_Sensor.
  double westVector_Sensor[4]={0,0,0,0};
  double southVector_Sensor[4] = {0,0,0,0};

  westVector_Sensor[westAxisIndex]=1; 

  vtkMath::Cross(westVector_Sensor, downVector_Sensor, southVector_Sensor); // compute South
  vtkMath::Normalize(southVector_Sensor);
  vtkMath::Cross(downVector_Sensor, southVector_Sensor, westVector_Sensor); // compute West
  vtkMath::Normalize(westVector_Sensor);

  // row 0
  lastSensorToTrackerTransform->SetElement(0,0,southVector_Sensor[0]);
  lastSensorToTrackerTransform->SetElement(0,1,southVector_Sensor[1]);
  lastSensorToTrackerTransform->SetElement(0,2,southVector_Sensor[2]);
  // row 1
  lastSensorToTrackerTransform->SetElement(1,0,westVector_Sensor[0]);
  lastSensorToTrackerTransform->SetElement(1,1,westVector_Sensor[1]);
  lastSensorToTrackerTransform->SetElement(1,2,westVector_Sensor[2]);
  // row 2
  lastSensorToTrackerTransform->SetElement(2,0,downVector_Sensor[0]);
  lastSensorToTrackerTransform->SetElement(2,1,downVector_Sensor[1]);
  lastSensorToTrackerTransform->SetElement(2,2,downVector_Sensor[2]);

}