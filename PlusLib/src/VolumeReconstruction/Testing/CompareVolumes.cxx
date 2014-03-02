/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/**
* This program creates or adds to a csv file some information about the quality of hole filling in volume reconstruction.
*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include <fstream>
#include <iostream>
#include <time.h>

#include "vtkDataSetReader.h"
#include "vtkDataSetWriter.h"
#include "vtkImageClip.h"
#include "vtkImageData.h"
#include "vtkImageMathematics.h"
#include "vtkImageHistogramStatistics.h"
#include "vtkMetaImageReader.h"

#include "vtkCompareVolumes.h"

//-----------------------------------------------------------------------------
int SimpleCompareVolumes(vtkImageData* testVol, vtkImageData* refVol, double simpleCompareMaxError)
{
  if (testVol==NULL)
  {
    LOG_ERROR("Test volume is invalid");
    return EXIT_FAILURE;
  }
  if (refVol==NULL)
  {
    LOG_ERROR("Reference volume is invalid");
    return EXIT_FAILURE;
  }

  int *testExt=testVol->GetExtent();
  int *refExt=refVol->GetExtent();
  if (testExt[0]!=refExt[0] || testExt[1]!=refExt[1]
    || testExt[2]!=refExt[2] || testExt[3]!=refExt[3]
    || testExt[4]!=refExt[4] || testExt[5]!=refExt[5])
  {
    LOG_ERROR("Test volume extents ("<<testExt[0]<<", "<<testExt[1]<<", "<<testExt[2]<<", "<<testExt[3]<<", "<<testExt[4]<<", "<<testExt[5]<<")" \
      << " do not match reference volume extents ("<<refExt[0]<<", "<<refExt[1]<<", "<<refExt[2]<<", "<<refExt[3]<<", "<<refExt[4]<<", "<<refExt[5]<<")")
    return EXIT_FAILURE;
  }

  double *testSpacing=testVol->GetSpacing();
  double *refSpacing=refVol->GetSpacing();
  if (testSpacing[0]!=refSpacing[0] || testSpacing[1]!=refSpacing[1] || testSpacing[2]!=refSpacing[2])
  {
    LOG_ERROR("Test volume spacing ("<<testSpacing[0]<<", "<<testSpacing[1]<<", "<<testSpacing[2]<<")" \
      << " does not match reference volume spacing ("<<refSpacing[0]<<", "<<refSpacing[1]<<", "<<refSpacing[2]<<")")
    return EXIT_FAILURE;
  }

  double *testOrigin=testVol->GetOrigin();
  double *refOrigin=refVol->GetOrigin();
  if (testOrigin[0]!=refOrigin[0] || testOrigin[1]!=refOrigin[1] || testOrigin[2]!=refOrigin[2])
  {
    LOG_ERROR("Test volume origin ("<<testOrigin[0]<<", "<<testOrigin[1]<<", "<<testOrigin[2]<<")" \
      << " does not match reference volume origin ("<<refOrigin[0]<<", "<<refOrigin[1]<<", "<<refOrigin[2]<<")")
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkImageMathematics> imageDiff = vtkSmartPointer<vtkImageMathematics>::New();
  imageDiff->SetOperationToSubtract();
  imageDiff->SetInput( 0, testVol );
  imageDiff->SetInput( 1, refVol );

  vtkSmartPointer<vtkImageMathematics> imageAbsDiff = vtkSmartPointer<vtkImageMathematics>::New();
  imageAbsDiff->SetOperationToAbsoluteValue();
  imageAbsDiff->SetInputConnection(imageDiff->GetOutputPort());

  vtkSmartPointer<vtkImageHistogramStatistics> statistics = vtkSmartPointer<vtkImageHistogramStatistics>::New();
  statistics->SetInputConnection(imageAbsDiff->GetOutputPort());
  statistics->GenerateHistogramImageOff();
  statistics->Update(); 

  double maxVal = statistics->GetMaximum();
  double meanVal = statistics->GetMean();
  double stdev = statistics->GetStandardDeviation(); 

  LOG_INFO("Absolute difference between pixels: max="<<maxVal<<", mean="<<meanVal<<", stdev="<<stdev);
  if (stdev>simpleCompareMaxError)
  {
    LOG_ERROR("Standard deviation of the absolute difference between the images ("<<stdev<<") is larger than the maximum allowed image difference ("<<simpleCompareMaxError<<")");
    return EXIT_FAILURE;
  }

  LOG_INFO("Standard deviation of the absolute difference between the images ("<<stdev<<") is smaller than the maximum allowed image difference ("<<simpleCompareMaxError<<")");  

  return EXIT_SUCCESS;
}



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
  std::vector<int> roiOriginV;
  std::vector<int> roiSizeV;
  double simpleCompareMaxError=-1;

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--ground-truth-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGTFileName, "The ground truth volume being compared against");
  args.AddArgument("--ground-truth-alpha", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputGTAlphaFileName, "The ground truth volume's alpha component");
  args.AddArgument("--testing-image", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestingFileName, "The testing image to compare to the ground truth");
  args.AddArgument("--testing-alpha", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestingAlphaFileName, "The testing volume's alpha component");
  args.AddArgument("--slices-alpha", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSliceAlphaFileName, "The alpha component for when the slices are pasted into the volume, without hole filling");
  args.AddArgument("--roi-origin", vtksys::CommandLineArguments::MULTI_ARGUMENT, &roiOriginV, "The three x y z values describing the origin for the region of interest's extent, in volume IJK coordinates");
  args.AddArgument("--roi-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &roiSizeV, "The three x y z values describing the size for the region of interest's extent, in volume IJK coordinates");
  args.AddArgument("--output-stats-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputStatsFileName, "The file to dump the statistics for the comparison");
  args.AddArgument("--output-diff-volume-true", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrueDiffFileName, "Save the true difference volume to this file");
  args.AddArgument("--output-diff-volume-absolute", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputAbsoluteDiffFileName, "Save the absolute difference volume to this file");
  args.AddArgument("--simple-compare-max-error", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &simpleCompareMaxError, "If specified, a simple comparison of the volumes is performed (no detailed statistics are computed, only the ground truth and test volumes are used) and if the stdev of pixel values of the absolute difference image is larger than the specified value then the test returns with failure");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");

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
  
  // Check file names
  if ( inputGTFileName.empty() || inputTestingFileName.empty() )
  {
    LOG_ERROR("ground-truth-image and testing-image are required arguments");
    LOG_ERROR("Help: " << args.GetHelp());
    exit(EXIT_FAILURE);
  }

  // read in the volumes

  LOG_INFO("Reading input ground truth image: " << inputGTFileName );
  vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
  reader->SetFileName(inputGTFileName.c_str());
  reader->Update();
  vtkImageData* groundTruth = vtkImageData::SafeDownCast(reader->GetOutput());

  LOG_INFO("Reading input testing image: " << inputTestingFileName );
  vtkSmartPointer<vtkMetaImageReader> reader3 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader3->SetFileName(inputTestingFileName.c_str());
  reader3->Update();
  vtkImageData* testingImage = vtkImageData::SafeDownCast(reader3->GetOutput());

  /************************************************************/
  // Simple volume comparison

  if (simpleCompareMaxError>=0)
  {
    LOG_INFO("Perform simple volume comparison");
    return SimpleCompareVolumes(testingImage, groundTruth, simpleCompareMaxError);
  }

  /************************************************************/
  // Full volume comparison, with detailed statistic


  // record the start time for data recording, see http://www.cplusplus.com/reference/clibrary/ctime/localtime/
  time_t rawtime;
  struct tm* timeInfo;
  time(&rawtime);
  timeInfo = localtime(&rawtime);
  char timeAndDate[60];
  strftime(timeAndDate,60,"%Y %m %d %H:%M",timeInfo);

  // Check file names
  if ( inputGTAlphaFileName.empty() || inputTestingAlphaFileName.empty() || inputSliceAlphaFileName.empty() || outputStatsFileName.empty() )
  {
    LOG_ERROR("ground-truth-image, ground-truth-alpha, testing-image, testing-alpha, slices-alpha, and output-stats-file are required arguments!");
    LOG_ERROR("Help: " << args.GetHelp());
    exit(EXIT_FAILURE);
  }

  // read in the additional volumes

  LOG_INFO("Reading input ground truth alpha: " << inputGTAlphaFileName );
  vtkSmartPointer<vtkMetaImageReader> reader2 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader2->SetFileName(inputGTAlphaFileName.c_str());
  reader2->Update();
  vtkImageData* groundTruthAlpha = vtkImageData::SafeDownCast(reader2->GetOutput());

  LOG_INFO("Reading input testing alpha: " << inputTestingAlphaFileName );
  vtkSmartPointer<vtkMetaImageReader> reader4 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader4->SetFileName(inputTestingAlphaFileName.c_str());
  reader4->Update();
  vtkImageData* testingAlpha = vtkImageData::SafeDownCast(reader4->GetOutput());

  LOG_INFO("Reading input slices alpha: " << inputSliceAlphaFileName );
  vtkSmartPointer<vtkMetaImageReader> reader5 = vtkSmartPointer<vtkMetaImageReader>::New();
  reader5->SetFileName(inputSliceAlphaFileName.c_str());
  reader5->Update();
  vtkImageData* slicesAlpha = vtkImageData::SafeDownCast(reader5->GetOutput());

  // check to make sure extents match
  int *extentGT=groundTruth->GetExtent();
  int *extentGTAlpha=groundTruthAlpha->GetExtent();
  int *extentTest=testingImage->GetExtent();
  int *extentTestAlpha=testingAlpha->GetExtent();
  int *extentSlicesAlpha=slicesAlpha->GetExtent();
  bool match(true);
  for (int i = 0; i < 6; i++)
    if (extentGT[i] != extentGTAlpha[i] || extentGT[i] != extentSlicesAlpha[i] || extentGT[i] != extentTestAlpha[i] || extentGT[i] != extentSlicesAlpha[i])
      match = false;
  if (!match) 
  {
    LOG_ERROR("Image sizes do not match!");
    exit(EXIT_FAILURE);
  }

  // crop the image to the ROI

  vtkImageData* groundTruthRoi = groundTruth;
  vtkImageData* groundTruthAlphaRoi = groundTruthAlpha;
  vtkImageData* testingImageRoi = testingImage;
  vtkImageData* testingAlphaRoi = testingAlpha;
  vtkImageData* slicesAlphaRoi = slicesAlpha;
  int roiExtent[6]={0}; // array format of roi extent
  for (int extentIndex = 0; extentIndex < 6; extentIndex++)
  {
    roiExtent[extentIndex] = extentGT[extentIndex];
  }

  // Create the clippers here to be able use their outputs throughout this method
  vtkSmartPointer<vtkImageClip> clipGT = vtkSmartPointer<vtkImageClip>::New();
  vtkSmartPointer<vtkImageClip> clipGTAlpha = vtkSmartPointer<vtkImageClip>::New();
  vtkSmartPointer<vtkImageClip> clipTest = vtkSmartPointer<vtkImageClip>::New();
  vtkSmartPointer<vtkImageClip> clipTestAlpha = vtkSmartPointer<vtkImageClip>::New();
  vtkSmartPointer<vtkImageClip> clipSlicesAlpha = vtkSmartPointer<vtkImageClip>::New();
        
  if (roiOriginV.size() == 3 && roiSizeV.size() == 3)
  {
    // Clip volumes to a ROI

    roiExtent[0] = roiOriginV[0];
    roiExtent[1] = roiOriginV[0] + roiSizeV[0] - 1;
    roiExtent[2] = roiOriginV[1];
    roiExtent[3] = roiOriginV[1] + roiSizeV[1] - 1;
    roiExtent[4] = roiOriginV[2];
    roiExtent[5] = roiOriginV[2] + roiSizeV[2] - 1;

    // check new extents
    if (roiExtent[0] < extentGT[0] || roiExtent[1] >= extentGT[1] ||
      roiExtent[2] < extentGT[2] || roiExtent[3] >= extentGT[3] ||
      roiExtent[4] < extentGT[4] || roiExtent[5] >= extentGT[5])
    {
        LOG_ERROR("Region of interest contains data outside the original volume's IJK coordinates! Extents are: " << roiExtent[0] << " " << roiExtent[1] << " " << roiExtent[2] << " " << roiExtent[3] << " " << roiExtent[4] << " " << roiExtent[5] << "\n" << "Original extent is: " << extentGT[0] << " " << extentGT[1] << " " << extentGT[2] << " " << extentGT[3] << " " << extentGT[4] << " " << extentGT[5]);
        exit(EXIT_FAILURE);
    }

    clipGT->SetInput(groundTruth);
    clipGT->SetClipData(1);
    clipGT->SetOutputWholeExtent(roiExtent);
    clipGT->Update();
    groundTruthRoi = clipGT->GetOutput();
        
    clipGTAlpha->SetInput(groundTruthAlpha);
    clipGTAlpha->SetClipData(1);
    clipGTAlpha->SetOutputWholeExtent(roiExtent);
    clipGTAlpha->Update();
    groundTruthAlphaRoi = clipGTAlpha->GetOutput();

    clipTest->SetInput(testingImage);
    clipTest->SetClipData(1);
    clipTest->SetOutputWholeExtent(roiExtent);
    clipTest->Update();
    testingImageRoi = clipTest->GetOutput();
        
    clipTestAlpha->SetInput(testingAlpha);
    clipTestAlpha->SetClipData(1);
    clipTestAlpha->SetOutputWholeExtent(roiExtent);
    clipTestAlpha->Update();
    testingAlphaRoi = clipTestAlpha->GetOutput();
    
    clipSlicesAlpha->SetInput(slicesAlpha);
    clipSlicesAlpha->SetClipData(1);
    clipSlicesAlpha->SetOutputWholeExtent(roiExtent);
    clipSlicesAlpha->Update();
    slicesAlphaRoi = clipSlicesAlpha->GetOutput();
  }
  else if (roiOriginV.size() != 0 || roiSizeV.size() != 0)
  {
    LOG_ERROR("ROI size and origin each need to be 3 values in volume IJK coordinates: X Y Z");
    exit(EXIT_FAILURE);
  }  

  // calculate the histogram for the difference image
  LOG_INFO("Calculating difference images and statistics...");
  vtkSmartPointer<vtkCompareVolumes> histogramGenerator = vtkSmartPointer<vtkCompareVolumes>::New();
  histogramGenerator->SetInputGT(groundTruthRoi);
  histogramGenerator->SetInputGTAlpha(groundTruthAlphaRoi);
  histogramGenerator->SetInputTest(testingImageRoi);
  histogramGenerator->SetInputTestAlpha(testingAlphaRoi);
  histogramGenerator->SetInputSliceAlpha(slicesAlphaRoi);
  histogramGenerator->Update();

  // write data to a CSV
  if (!outputStatsFileName.empty())
  {
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
                   << roiExtent[0] << "_" << roiExtent[1] << "_" << roiExtent[2] << "_" << roiExtent[3] << "_" << roiExtent[4] << "_" << roiExtent[5] << ","
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
      outputStatsFile << "Time,Dataset - Ground Truth,Dataset - Ground Truth Alpha,Dataset - Slices Alpha,Dataset - Testing Image,Dataset - Testing Alpha,Region of Interest Extent,Number of Holes,Number of Filled Holes,Number of Visible Voxels,True Maximum Error,True Minimum Error,True Median Error,True Mean Error,True Standard Deviation,True 5th Percentile,True 95th Percentile,Absolute Maximum Error,Absolute Minimum Error,Absolute Median Error,Absolute Mean Error,Absolute Standard Deviation,Absolute 5th Percentile,Absolute 95th Percentile,RMS,Absolute Mean Error Including Holes";
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
                   << roiExtent[0] << "_" << roiExtent[1] << "_" << roiExtent[2] << "_" << roiExtent[3] << "_" << roiExtent[4] << "_" << roiExtent[5] << ","
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

  if (!outputAbsoluteDiffFileName.empty()) 
  {
    LOG_INFO("Writing absolute difference image: " << outputAbsoluteDiffFileName);
    vtkSmartPointer<vtkDataSetWriter> writerAbs = vtkSmartPointer<vtkDataSetWriter>::New();
    writerAbs->SetFileTypeToBinary();
    writerAbs->SetInput(histogramGenerator->GetOutputAbsoluteDifferenceImage());
    writerAbs->SetFileName(outputAbsoluteDiffFileName.c_str());
    writerAbs->Update();
  }

  if (!outputTrueDiffFileName.empty()) 
  {
    LOG_INFO("Writing true difference image: " << outputTrueDiffFileName);
    vtkSmartPointer<vtkDataSetWriter> writerTru = vtkSmartPointer<vtkDataSetWriter>::New();
    writerTru->SetFileTypeToBinary();
    writerTru->SetInput(histogramGenerator->GetOutputTrueDifferenceImage());
    writerTru->SetFileName(outputTrueDiffFileName.c_str());
    writerTru->Update();
  }

  return EXIT_SUCCESS;
}
