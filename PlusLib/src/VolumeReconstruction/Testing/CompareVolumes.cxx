/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/**
* This program creates a vtkPolyData model that represents tracked ultrasound
* image slices in their tracked positions.
* It can be used to debug geometry problems in volume reconstruction.
* 
*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include <fstream>
#include <iostream>
#include <time.h>

#include "vtkDataSetReader.h"
#include "vtkDataSetWriter.h"
#include "vtkImageData.h"
#include "vtkImageClip.h"
#include "vtkImageFFT.h"
#include "vtkImageMagnitude.h"
#include "vtkImageFourierCenter.h"
#include "vtkImageLogarithmicScale.h"
#include "vtkMetaImageReader.h"
//#include "vtkMetaImageWriter.h"
//#include "vtkImageMathematics.h"
//#include "vtkImageLogic.h"
//#include "vtkImageAccumulator.h"

#include "vtkCompareVolumes.h"


int main( int argc, char** argv )
{
  bool printHelp(false);
  std::string inputGTFileName;
  std::string inputGTAlphaFileName;
  std::string inputTestingFileName;
  std::string inputTestingAlphaFileName;
  std::string inputSliceAlphaFileName;
  std::string outputStatsFileName;
  std::string outputTrueDiffFileName;
  std::string outputAbsoluteDiffFileName;
  std::string outputFourierTestFileName;
  std::string outputFourierGroundTruthFileName;
  std::string outputCroppedGTFileName;
  std::string outputCroppedTestFileName;
  std::vector<int> centerV;
  std::vector<int> sizeV;
  int center[3];
  int size[3];

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-ground-truth-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGTFileName, "The ground truth volume being compared against");
  args.AddArgument("--input-ground-truth-alpha", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGTAlphaFileName, "The ground truth volume's alpha component");
  args.AddArgument("--input-testing-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestingFileName, "The testing image to compare to the ground truth");
  args.AddArgument("--input-testing-alpha", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestingAlphaFileName, "The testing volume's alpha component");
  args.AddArgument("--input-slices-alpha", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSliceAlphaFileName, "The alpha component for when the slices are pasted into the volume, without hole filling");
  args.AddArgument("--output-cropped-ground-truth-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCroppedGTFileName, "The cropped ground truth volume");
  args.AddArgument("--output-cropped-testing-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCroppedTestFileName, "The cropped testing volume");
  args.AddArgument("--output-stats-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputStatsFileName, "The file to dump the statistics for the comparison");
  args.AddArgument("--output-diff-volume-true", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrueDiffFileName, "Save the true difference volume to this file");
  args.AddArgument("--output-diff-volume-absolute", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputAbsoluteDiffFileName, "Save the absolute difference volume to this file");
  args.AddArgument("--output-fourier-test-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFourierTestFileName, "The file to dump the fourier statistics for the testing volume");
  args.AddArgument("--output-fourier-ground-truth-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFourierGroundTruthFileName, "The file to dump the fourier statistics for the ground truth");
  args.AddArgument("--roi-center", vtksys::CommandLineArguments::MULTI_ARGUMENT, &centerV, "The point at the center of the region of interest");
  args.AddArgument("--roi-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &sizeV, "The size around the center point to consider");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");

  // examples:
  //--roi-center 5 6 7
  //--roi-size 4 5 8

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  /************************************************************/	
  
  // record the start time for data recording, see http://www.cplusplus.com/reference/clibrary/ctime/localtime/
  time_t rawtime;
  struct tm* timeInfo;
  time(&rawtime);
  timeInfo = localtime(&rawtime);
  char timeAndDate[60];
  strftime(timeAndDate,60,"%Y %m %d %H:%M",timeInfo);

  // Check file names
  if ( inputGTFileName.empty() || inputGTAlphaFileName.empty() || inputTestingFileName.empty() || inputTestingAlphaFileName.empty() || inputSliceAlphaFileName.empty() || outputCroppedTestFileName.empty() || outputCroppedGTFileName.empty() )
  {
    LOG_ERROR("input-ground-truth-image, input-ground-truth-alpha, input-testing-image, input-testing-alpha, input-slices-alpha, output-cropped-ground-truth-image, output-cropped-testing-image, and output-stats-file are required arguments!");
    LOG_ERROR("Help: " << args.GetHelp());
    exit(EXIT_FAILURE);
  }

  bool useWholeExtent((centerV.size() == 0)?true:false);

  if (!useWholeExtent)
  {
    // parse and check center
    if (centerV.size() == 3)
    {
      center[0] = centerV[0];
      center[1] = centerV[1];
      center[2] = centerV[2];
    }
    else
    {
      LOG_ERROR("Center needs to be at least 3 values (X,Y,Z)");
      exit(EXIT_FAILURE);
    }

    if (center[0] < 0 || center[1] < 0 || center[2] < 0) 
    {
      LOG_ERROR("Center must consist of positive integers");
      exit(EXIT_FAILURE);
    }

    // parse and check size
    if (sizeV.size() == 3) 
    {
      size[0] = sizeV[0];
      size[1] = sizeV[1];
      size[2] = sizeV[2];
    } 
    else if (sizeV.size() == 1) 
    {
      size[0] = size[1] = size[2] = sizeV[0];
    }
    else
    {
      LOG_ERROR("Size needs to be either 3 values (X,Y,Z), or 1 value for X = Y = Z");
      exit(EXIT_FAILURE);
    }

    if (size[0] <= 0 || size[1] <= 0 || size[2] <= 0) 
    {
      LOG_ERROR("Size must consist of positive integers");
      exit(EXIT_FAILURE);
    }
  } else {
    size[0] = size[1] = size[2] = center[0] = center[1] = center[2] = -1;
  }

  // read inputs
  vtkSmartPointer<vtkImageData> groundTruth = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> groundTruthAlpha = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> testingImage = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> testingAlpha = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> slicesAlpha = vtkSmartPointer<vtkImageData>::New();

  // read in the volumes
  LOG_INFO("Reading input ground truth image: " << inputGTFileName );
  vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
  reader->SetFileName(inputGTFileName.c_str());
  reader->Update();
  groundTruth = vtkImageData::SafeDownCast(reader->GetOutput());

  LOG_INFO("Reading input ground truth alpha: " << inputGTAlphaFileName );
  vtkSmartPointer<vtkMetaImageReader> reader2 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader2->SetFileName(inputGTAlphaFileName.c_str());
  reader2->Update();
  groundTruthAlpha = vtkImageData::SafeDownCast(reader2->GetOutput());

  LOG_INFO("Reading input testing image: " << inputTestingFileName );
  vtkSmartPointer<vtkMetaImageReader> reader3 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader3->SetFileName(inputTestingFileName.c_str());
  reader3->Update();
  testingImage = vtkImageData::SafeDownCast(reader3->GetOutput());

  LOG_INFO("Reading input testing alpha: " << inputTestingAlphaFileName );
  vtkSmartPointer<vtkMetaImageReader> reader4 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader4->SetFileName(inputTestingAlphaFileName.c_str());
  reader4->Update();
  testingAlpha = vtkImageData::SafeDownCast(reader4->GetOutput());

  LOG_INFO("Reading input slices alpha: " << inputSliceAlphaFileName );
  vtkSmartPointer<vtkMetaImageReader> reader5 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader5->SetFileName(inputSliceAlphaFileName.c_str());
  reader5->Update();
  slicesAlpha = vtkImageData::SafeDownCast(reader5->GetOutput());

  // check to make sure extents match
  int extentGT[6];
  groundTruth->GetExtent(extentGT[0],extentGT[1],extentGT[2],extentGT[3],extentGT[4],extentGT[5]);
  int extentGTAlpha[6];
  groundTruthAlpha->GetExtent(extentGTAlpha[0],extentGTAlpha[1],extentGTAlpha[2],extentGTAlpha[3],extentGTAlpha[4],extentGTAlpha[5]);
  int extentTest[6];
  testingImage->GetExtent(extentTest[0],extentTest[1],extentTest[2],extentTest[3],extentTest[4],extentTest[5]);
  int extentTestAlpha[6];
  testingAlpha->GetExtent(extentTestAlpha[0],extentTestAlpha[1],extentTestAlpha[2],extentTestAlpha[3],extentTestAlpha[4],extentTestAlpha[5]);
  int extentSlicesAlpha[6];
  slicesAlpha->GetExtent(extentSlicesAlpha[0],extentSlicesAlpha[1],extentSlicesAlpha[2],extentSlicesAlpha[3],extentSlicesAlpha[4],extentSlicesAlpha[5]);
  bool match(true);
  for (int i = 0; i < 6; i++)
    if (extentGT[i] != extentGTAlpha[i] || extentGT[i] != extentSlicesAlpha[i] || extentGT[i] != extentTestAlpha[i] || extentGT[i] != extentSlicesAlpha[i])
      match = false;
  if (!match) {
    LOG_ERROR("Image sizes do not match!");
    exit(EXIT_FAILURE);
  }

  // crop the image to the ROI
  vtkSmartPointer<vtkImageData> groundTruthCropped = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> groundTruthAlphaCropped = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> testingImageCropped = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> testingAlphaCropped = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> slicesAlphaCropped = vtkSmartPointer<vtkImageData>::New();
  if (!useWholeExtent) {
    // calculate and check new extents
    int updatedExtent[6];
    updatedExtent[0] = center[0]-size[0];
    updatedExtent[1] = center[0]+size[0];
    updatedExtent[2] = center[1]-size[1];
    updatedExtent[3] = center[1]+size[1];
    updatedExtent[4] = center[2]-size[2];
    updatedExtent[5] = center[2]+size[2];

    if (updatedExtent[0] < extentGT[0] || updatedExtent[1] >= extentGT[1] ||
        updatedExtent[2] < extentGT[2] || updatedExtent[3] >= extentGT[3] ||
        updatedExtent[4] < extentGT[4] || updatedExtent[5] >= extentGT[5]) {
          LOG_ERROR("Region of interest contains data outside the original volume! Extents are: " << updatedExtent[0] << " " << updatedExtent[1] << " " << updatedExtent[2] << " " << updatedExtent[3] << " " << updatedExtent[4] << " " << updatedExtent[5] << "\n" << "Original extent is: " << extentGT[0] << " " << extentGT[1] << " " << extentGT[2] << " " << extentGT[3] << " " << extentGT[4] << " " << extentGT[5]);
          exit(EXIT_FAILURE);
    }

    vtkSmartPointer<vtkImageClip> clipGT = vtkSmartPointer<vtkImageClip>::New();
    clipGT->SetInput(groundTruth);
    clipGT->SetClipData(1);
    clipGT->SetOutputWholeExtent(updatedExtent);
    clipGT->Update();
    groundTruthCropped = clipGT->GetOutput();
    
    vtkSmartPointer<vtkImageClip> clipGTAlpha = vtkSmartPointer<vtkImageClip>::New();
    clipGTAlpha->SetInput(groundTruthAlpha);
    clipGTAlpha->SetClipData(1);
    clipGTAlpha->SetOutputWholeExtent(updatedExtent);
    clipGTAlpha->Update();
    groundTruthAlphaCropped = clipGTAlpha->GetOutput();

    vtkSmartPointer<vtkImageClip> clipTest = vtkSmartPointer<vtkImageClip>::New();
    clipTest->SetInput(testingImage);
    clipTest->SetClipData(1);
    clipTest->SetOutputWholeExtent(updatedExtent);
    clipTest->Update();
    testingImageCropped = clipTest->GetOutput();
    
    vtkSmartPointer<vtkImageClip> clipTestAlpha = vtkSmartPointer<vtkImageClip>::New();
    clipTestAlpha->SetInput(testingAlpha);
    clipTestAlpha->SetClipData(1);
    clipTestAlpha->SetOutputWholeExtent(updatedExtent);
    clipTestAlpha->Update();
    testingAlphaCropped = clipTestAlpha->GetOutput();

    vtkSmartPointer<vtkImageClip> clipSlicesAlpha = vtkSmartPointer<vtkImageClip>::New();
    clipSlicesAlpha->SetInput(slicesAlpha);
    clipSlicesAlpha->SetClipData(1);
    clipSlicesAlpha->SetOutputWholeExtent(updatedExtent);
    clipSlicesAlpha->Update();
    slicesAlphaCropped = clipSlicesAlpha->GetOutput();
  }
  else
  {
    groundTruthCropped = groundTruth;
    groundTruthAlphaCropped = groundTruthAlpha;
    testingImageCropped = testingImage;
    testingAlphaCropped = testingAlpha;
    slicesAlphaCropped = slicesAlpha;
  }

  // calculate the histogram for the difference image
  LOG_INFO("Calculating difference images and statistics...");
  vtkSmartPointer<vtkCompareVolumes> histogramGenerator = vtkSmartPointer<vtkCompareVolumes>::New();
  histogramGenerator->SetInputGT(groundTruthCropped);
  histogramGenerator->SetInputGTAlpha(groundTruthAlphaCropped);
  histogramGenerator->SetInputTest(testingImageCropped);
  histogramGenerator->SetInputTestAlpha(testingAlphaCropped);
  histogramGenerator->SetInputSliceAlpha(slicesAlphaCropped);
  histogramGenerator->Update();

  // write cropped volumes
  LOG_INFO("Writing cropped volumes...");

  vtkSmartPointer<vtkDataSetWriter> writerCroppedGT = vtkSmartPointer<vtkDataSetWriter>::New();
  writerCroppedGT->SetFileTypeToBinary();
  writerCroppedGT->SetInput(groundTruthCropped);
  writerCroppedGT->SetFileName(outputCroppedGTFileName.c_str());
  writerCroppedGT->Update();

  vtkSmartPointer<vtkDataSetWriter> writerCroppedTest = vtkSmartPointer<vtkDataSetWriter>::New();
  writerCroppedTest->SetFileTypeToBinary();
  writerCroppedTest->SetInput(testingImageCropped);
  writerCroppedTest->SetFileName(outputCroppedTestFileName.c_str());
  writerCroppedTest->Update();

  LOG_INFO("Re-reading cropped volumes...");

  vtkSmartPointer<vtkDataSetReader> readerCropGT = vtkSmartPointer<vtkDataSetReader>::New();
  readerCropGT->SetFileName(outputCroppedGTFileName.c_str());
  readerCropGT->Update();
  groundTruthCropped = vtkImageData::SafeDownCast(readerCropGT->GetOutput());

  vtkSmartPointer<vtkDataSetReader> readerCropTest = vtkSmartPointer<vtkDataSetReader>::New();
  readerCropTest->SetFileName(outputCroppedTestFileName.c_str());
  readerCropTest->Update();
  testingImageCropped = vtkImageData::SafeDownCast(readerCropTest->GetOutput());

  // write data to a CSV
  if (!outputStatsFileName.empty()) {
    LOG_INFO("Writing output statistics: " << outputStatsFileName);
    int* TruHistogram = histogramGenerator->GetTrueHistogramPtr();
    int* AbsHistogram = histogramGenerator->GetAbsoluteHistogramPtr();
    int* AbsHistogramWithHoles = histogramGenerator->GetAbsoluteHistogramWithHolesPtr();
    std::ifstream testingFile(outputStatsFileName.c_str());
    std::ofstream outputStatsFile; // for use once we establish whether or not the file exists
    if (testingFile.is_open())
    { // if the file exists already, just output what we want
      testingFile.close(); // no reading, must open for writing now
      outputStatsFile.open(outputStatsFileName.c_str(),std::ios::out | std::ios::app);
      outputStatsFile << timeAndDate << ","
                   << inputGTFileName << ","
                   << inputGTAlphaFileName << ","
                   << inputSliceAlphaFileName << ","
                   << inputTestingFileName << ","
                   << inputTestingAlphaFileName << ","
                   << center[0] << "_" << center[1] << "_" << center[2] << ","
                   << size[0] << "_" << size[1] << "_" << size[2] << ","
                   << histogramGenerator->GetNumberOfHoles() << "," 
                   << histogramGenerator->GetNumberOfFilledHoles() << ","
                   << histogramGenerator->GetNumberVoxelsVisible() << ","
                   << histogramGenerator->GetTrueMaximum() << ","
                   << histogramGenerator->GetTrueMinimum() << ","
                   << histogramGenerator->GetTrueMedian() << ","
                   << histogramGenerator->GetTrueMean() << ","
                   << histogramGenerator->GetTrueStdev() << ","
                   << histogramGenerator->GetTrue5thPercentile() << ","
                   << histogramGenerator->GetTrue95thPercentile()<< "," 
                   << histogramGenerator->GetAbsoluteMaximum() << ","
                   << histogramGenerator->GetAbsoluteMinimum() << ","
                   << histogramGenerator->GetAbsoluteMedian() << ","
                   << histogramGenerator->GetAbsoluteMean() << ","
                   << histogramGenerator->GetAbsoluteStdev() << ","
                   << histogramGenerator->GetAbsolute5thPercentile() << ","
                   << histogramGenerator->GetAbsolute95thPercentile() << ","
                   << histogramGenerator->GetRMS() << ","
                   << histogramGenerator->GetAbsoluteMeanWithHoles();
      for (int i = 0; i < 511; i++)
        outputStatsFile << "," << TruHistogram[i];
      for (int i = 0; i < 256; i++)
        outputStatsFile << "," << AbsHistogram[i];
      for (int i = 0; i < 256; i++)
        outputStatsFile << "," << AbsHistogramWithHoles[i];
      outputStatsFile << std::endl;
    }
    else
    { // need to create the file and write to it. First give it a header row.
      testingFile.close(); // no reading, must open for writing now
      outputStatsFile.open(outputStatsFileName.c_str());
      outputStatsFile << "Time,Dataset - Ground Truth,Dataset - Ground Truth Alpha,Dataset - Slices Alpha,Dataset - Testing Image,Dataset - Testing Alpha,Region of Interest Center,Region of Interest Size,Number of Holes,Number of Filled Holes,Number of Visible Voxels,True Maximum Error,True Minimum Error,True Median Error,True Mean Error,True Standard Deviation,True 5th Percentile,True 95th Percentile,Absolute Maximum Error,Absolute Minimum Error,Absolute Median Error,Absolute Mean Error,Absolute Standard Deviation,Absolute 5th Percentile,Absolute 95th Percentile,RMS,Absolute Mean Error Including Holes";
      for (int i = 0; i < 511; i++) {
        outputStatsFile << "," << (i - 255);
      }
      for (int i = 0; i < 256; i++) {
        outputStatsFile << "," << i;
      }
      for (int i = 0; i < 256; i++) {
        outputStatsFile << "," << i;
      }
      outputStatsFile << std::endl;
      outputStatsFile << timeAndDate << ","
                   << inputGTFileName << ","
                   << inputGTAlphaFileName << ","
                   << inputSliceAlphaFileName << ","
                   << inputTestingFileName << ","
                   << inputTestingAlphaFileName << ","
                   << center[0] << "_" << center[1] << "_" << center[2] << ","
                   << size[0] << "_" << size[1] << "_" << size[2] << ","
                   << histogramGenerator->GetNumberOfHoles() << "," 
                   << histogramGenerator->GetNumberOfFilledHoles() << ","
                   << histogramGenerator->GetNumberVoxelsVisible() << ","
                   << histogramGenerator->GetTrueMaximum() << ","
                   << histogramGenerator->GetTrueMinimum() << ","
                   << histogramGenerator->GetTrueMedian() << ","
                   << histogramGenerator->GetTrueMean() << ","
                   << histogramGenerator->GetTrueStdev() << ","
                   << histogramGenerator->GetTrue5thPercentile() << ","
                   << histogramGenerator->GetTrue95thPercentile()<< "," 
                   << histogramGenerator->GetAbsoluteMaximum() << ","
                   << histogramGenerator->GetAbsoluteMinimum() << ","
                   << histogramGenerator->GetAbsoluteMedian() << ","
                   << histogramGenerator->GetAbsoluteMean() << ","
                   << histogramGenerator->GetAbsoluteStdev() << ","
                   << histogramGenerator->GetAbsolute5thPercentile() << ","
                   << histogramGenerator->GetAbsolute95thPercentile() << ","
                   << histogramGenerator->GetRMS() << ","
                   << histogramGenerator->GetAbsoluteMeanWithHoles();
      for (int i = 0; i < 511; i++)
        outputStatsFile << "," << TruHistogram[i];
      for (int i = 0; i < 256; i++)
        outputStatsFile << "," << AbsHistogram[i];
      for (int i = 0; i < 256; i++)
        outputStatsFile << "," << AbsHistogramWithHoles[i];
      outputStatsFile << std::endl;
    }
  }

  if (!outputAbsoluteDiffFileName.empty()) {
    LOG_INFO("Writing absolute difference image: " << outputAbsoluteDiffFileName);
    vtkSmartPointer<vtkDataSetWriter> writerAbs = vtkSmartPointer<vtkDataSetWriter>::New();
    writerAbs->SetFileTypeToBinary();
    writerAbs->SetInput(histogramGenerator->GetOutputAbsoluteDifferenceImage());
    writerAbs->SetFileName(outputAbsoluteDiffFileName.c_str());
    writerAbs->Update();
  }

  if (!outputTrueDiffFileName.empty()) {
    LOG_INFO("Writing true difference image: " << outputTrueDiffFileName);
    vtkSmartPointer<vtkDataSetWriter> writerTru = vtkSmartPointer<vtkDataSetWriter>::New();
    writerTru->SetFileTypeToBinary();
    writerTru->SetInput(histogramGenerator->GetOutputTrueDifferenceImage());
    writerTru->SetFileName(outputTrueDiffFileName.c_str());
    writerTru->Update();
  }

  if (!outputFourierGroundTruthFileName.empty()) {

    LOG_INFO("Writing ground truth fourier statistics: " << outputStatsFileName);

    vtkSmartPointer<vtkImageFFT> fourierTransGroundTruth = vtkSmartPointer<vtkImageFFT>::New();
    fourierTransGroundTruth->SetInput(groundTruthCropped);
    //fourierTransGroundTruth->ReleaseDataFlagOff();

    vtkSmartPointer<vtkImageMagnitude> fourierTransGroundTruthMag = vtkSmartPointer<vtkImageMagnitude>::New();
    fourierTransGroundTruthMag->SetInputConnection(fourierTransGroundTruth->GetOutputPort());
    
    vtkSmartPointer<vtkImageFourierCenter> fourierTransGroundTruthCen = vtkSmartPointer<vtkImageFourierCenter>::New();
    fourierTransGroundTruthCen->SetInputConnection(fourierTransGroundTruthMag->GetOutputPort());

    vtkSmartPointer<vtkImageLogarithmicScale> fourierTransGroundTruthLog = vtkSmartPointer<vtkImageLogarithmicScale>::New();
    fourierTransGroundTruthLog->SetInputConnection(fourierTransGroundTruthCen->GetOutputPort());

    vtkSmartPointer<vtkDataSetWriter> writerGTFFT = vtkSmartPointer<vtkDataSetWriter>::New();
    writerGTFFT->SetFileTypeToBinary();
    writerGTFFT->SetInputConnection(fourierTransGroundTruthLog->GetOutputPort());
    writerGTFFT->SetFileName(outputFourierGroundTruthFileName.c_str());
    writerGTFFT->Update();

  }

  if (!outputFourierTestFileName.empty()) {

    LOG_INFO("Writing testing image fourier statistics: " << outputStatsFileName);

    vtkSmartPointer<vtkImageFFT> fourierTransTest = vtkSmartPointer<vtkImageFFT>::New();
    fourierTransTest->SetInput(testingImageCropped);
    //fourierTransTest->ReleaseDataFlagOff();

    vtkSmartPointer<vtkImageMagnitude> fourierTransTestMag = vtkSmartPointer<vtkImageMagnitude>::New();
    fourierTransTestMag->SetInputConnection(fourierTransTest->GetOutputPort());
    
    vtkSmartPointer<vtkImageFourierCenter> fourierTransTestCen = vtkSmartPointer<vtkImageFourierCenter>::New();
    fourierTransTestCen->SetInputConnection(fourierTransTestMag->GetOutputPort());

    vtkSmartPointer<vtkImageLogarithmicScale> fourierTransTestLog = vtkSmartPointer<vtkImageLogarithmicScale>::New();
    fourierTransTestLog->SetInputConnection(fourierTransTestCen->GetOutputPort());

    vtkSmartPointer<vtkDataSetWriter> writerTestFFT = vtkSmartPointer<vtkDataSetWriter>::New();
    writerTestFFT->SetFileTypeToBinary();
    writerTestFFT->SetInputConnection(fourierTransTestLog->GetOutputPort());
    writerTestFFT->SetFileName(outputFourierTestFileName.c_str());
    writerTestFFT->Update();
  
  }

  return EXIT_SUCCESS;
}
