/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include <iomanip>
#include <iostream>

#include "vtkRfProcessor.h"
#include "vtkXMLUtilities.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageSequenceIO.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransform.h"
#include "vtkImageData.h" 
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkMatrix3x3.h"


double AngleRad(double* a, double *b){
  double aMag = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
  double bMag = sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
  double dot = (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
  return acos(dot/(aMag*bMag));
}

void MultiplyQuaternion(double q1[4], double q2[4], double q[4] )
{
  double ww = q1[0]*q2[0];
  double wx = q1[0]*q2[1];
  double wy = q1[0]*q2[2];
  double wz = q1[0]*q2[3];

  double xw = q1[1]*q2[0];
  double xx = q1[1]*q2[1];
  double xy = q1[1]*q2[2];
  double xz = q1[1]*q2[3];

  double yw = q1[2]*q2[0];
  double yx = q1[2]*q2[1];
  double yy = q1[2]*q2[2];
  double yz = q1[2]*q2[3];

  double zw = q1[3]*q2[0];
  double zx = q1[3]*q2[1];
  double zy = q1[3]*q2[2];
  double zz = q1[3]*q2[3];

  q[0] = ww-xx-yy-zz;
  q[1] = wx+xw+yz-zy;
  q[2] = wy-xz+yw+zx;
  q[3] = wz+xy-yx+zw;
}

//-----------------------------------------------------------------------------

/*
 * The purpose of this tool is to assist in post-processing and volume
 * reconstruction of the data collected using Phidget 3/3/3 tracker.
 *
 * The fusion algorithm has standing issues at the moment, and to work around
 * those issues this tool derives GyroscopeSensor to Tracker transforms using
 * AngularRate readings for each of the 3 axes. The transforms capturing
 * rotation around each of the axes separately, and by composing rotation
 * around all three axes together are added to the output sequence file.
 *
 * Frames that correspond to angular rotation that exceeds user-defined
 * threshold can be discarded. For the Nucletron/SPOT system, sweep angle is
 * 140 degrees, and the angular rate during the slow phase of the sweep
 * corresponds to ~3.5 deg/sec. In some cases, frames acquired during the
 * "fast" phase of the sweep (0->-70 deg and 70->0 deg) can have incorrect
 * spatial position possibly due to lack of temporal calibration.
 *
 * Andrey Fedorov, BWH
 */

int main(int argc, char **argv)
{
  std::string inputImgFile;
  std::string inputConfigFile;
  std::string outputImgFile;
  std::string frameTransform;
  int angularRateThresholdAxis = -1;
  double angularRateMin=0, angularRateMax=180.;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgFile, "File name of input image");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgFile, "File name of the image with the transform added");
  args.AddArgument("--angular-rate-min", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &angularRateMin, "Minimum angular rate (degrees) to use for filtering. Used when thresholding based on angular rate is enabled (--angular-rate-threshold=1).");
  args.AddArgument("--angular-rate-max", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &angularRateMax, "Maximum angular rate (degrees) to use for filtering. Used when thresholding based on angular rate is enabled (--angular-rate-threshold=1).");
  args.AddArgument("--angular-rate-threshold-axis", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &angularRateThresholdAxis, "Enable selection of frames based on the angular rate threshold around the specified axis.");

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
    std::cerr << "--input-file required" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (outputImgFile.empty())
  {
    std::cerr << "Missing --output-file parameter. Specification of the output image file name is required." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Read transformations data 
  LOG_DEBUG("Reading input meta file..."); 
  vtkSmartPointer< vtkTrackedFrameList > frameList = vtkSmartPointer< vtkTrackedFrameList >::New();
  frameList->ReadFromSequenceMetafile( inputImgFile.c_str() );
  LOG_DEBUG("Reading input file completed"); 

  // Process the frames
  int nFrames = frameList->GetNumberOfTrackedFrames();

  vtkSmartPointer<vtkTrackedFrameList> newFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();

  float angularPosition[3] = {0,0,0};

  TrackedFrame prevFrame(*frameList->GetTrackedFrame(0));
  float prevTime = prevFrame.GetTimestamp();
  int nFramesIncluded = 0;

  for (int i = 1; i<nFrames; i++)
  {    
    vtkSmartPointer<vtkMatrix4x4> gyroscopeMat = vtkSmartPointer<vtkMatrix4x4>::New();
    TrackedFrame frame(*frameList->GetTrackedFrame(i));
    frame.GetCustomFrameTransform(PlusTransformName("Gyroscope","Tracker"), gyroscopeMat);
    float angularRate[3], time;
    for(int j=0;j<3;j++)
      angularRate[j] = gyroscopeMat->GetElement(j,3);
    time = frame.GetTimestamp();
    for(int j=0;j<3;j++)
      angularPosition[j] += vtkMath::RadiansFromDegrees(angularRate[j]*(time-prevTime));

    double quaternion[3][4];
    bzero(&quaternion[0],3*4*sizeof(double));

    for(int j=0;j<3;j++){
      double gyroscopeRot3[3][3];
      vtkSmartPointer<vtkMatrix4x4> gyroscopeRot4 = vtkSmartPointer<vtkMatrix4x4>::New();
      gyroscopeRot4->Identity();

      quaternion[j][0] = cos(.5*angularPosition[j]);
      quaternion[j][j+1] = sin(.5*angularPosition[j]);
 
      vtkMath::QuaternionToMatrix3x3(&quaternion[j][0], gyroscopeRot3);
      for(int c=0;c<3;c++){
        for(int r=0;r<3;r++){
          gyroscopeRot4->SetElement(r,c,gyroscopeRot3[r][c]);
        }
      }
    
      std::ostringstream tfmName;
      tfmName << "GyroscopeOrientationAxis" << j;

      frame.SetCustomFrameTransform(PlusTransformName(tfmName.str().c_str(),"Tracker"),gyroscopeRot4);
      frame.SetCustomFrameTransformStatus(PlusTransformName(tfmName.str().c_str(),"Tracker"),FIELD_OK);
    }

    double combinedQuaternion[3] = {0,0,0};
    MultiplyQuaternion(&quaternion[0][0],&quaternion[1][0],&combinedQuaternion[0]);
    MultiplyQuaternion(&combinedQuaternion[0],&quaternion[2][0],&combinedQuaternion[0]);

    double gyroscopeRot3[3][3];
    vtkSmartPointer<vtkMatrix4x4> gyroscopeRot4 = vtkSmartPointer<vtkMatrix4x4>::New();
    gyroscopeRot4->Identity();
  
    vtkMath::QuaternionToMatrix3x3(&combinedQuaternion[0], gyroscopeRot3);
    for(int c=0;c<3;c++){
      for(int r=0;r<3;r++){
        gyroscopeRot4->SetElement(r,c,gyroscopeRot3[r][c]);
      }
    }

    frame.SetCustomFrameTransform(PlusTransformName("GyroscopeOrientation","Tracker"),gyroscopeRot4);
    frame.SetCustomFrameTransformStatus(PlusTransformName("GyroscopeOrientation","Tracker"),FIELD_OK);
    
    if(angularRateThresholdAxis == -1 ||
      (angularRate[angularRateThresholdAxis]>=angularRateMin &&
       angularRate[angularRateThresholdAxis]<angularRateMax)){
      newFrameList->AddTrackedFrame(&frame);
      nFramesIncluded++;
    }

    prevTime = time;
  }

  std::cout << nFramesIncluded << " frames included out of the " << nFrames << " input frames." << std::endl;

  vtkSmartPointer<vtkMetaImageSequenceIO> outputImgSeqFileWriter = vtkSmartPointer<vtkMetaImageSequenceIO>::New(); 
  outputImgSeqFileWriter->SetFileName(outputImgFile.c_str()); 
  outputImgSeqFileWriter->SetTrackedFrameList(newFrameList); 
  outputImgSeqFileWriter->SetImageOrientationInFile(newFrameList->GetImageOrientation());
  outputImgSeqFileWriter->Write(); 

  return 0;
}
