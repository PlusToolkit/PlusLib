/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkIGSIOTrackedFrameList.h"

// Utility includes
#include <PlaneParametersEstimator.h>
#include <RANSAC.h>

// ITK includes
#include <itkBinaryThresholdImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkLineIterator.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkRGBPixel.h>
#include <itkResampleImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

// VTK includes
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#ifdef PLUS_RENDERING_ENABLED
#include <vtkContextView.h>
#endif
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkPlusLineSegmentationAlgo.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

static const double INTESNITY_THRESHOLD_PERCENTAGE_OF_PEAK = 0.5; // threshold (as the percentage of the peak intensity along a scanline) for COG
static const double MAX_CONSECUTIVE_INVALID_VIDEO_FRAMES = 10; // the maximum number of consecutive invalid frames before warning message issued
static const double MAX_PERCENTAGE_OF_INVALID_VIDEO_FRAMES = 0.1; // the maximum percentage of the invalid frames before warning message issued
static const double MIN_X_SLOPE_COMPONENT_FOR_DETECTED_LINE = 0.01; // if the detected line's slope's x-component is less than this (i.e. almost vertical), skip frame
static const int MINIMUM_NUMBER_OF_VALID_SCANLINES = 5; // minimum number of valid scanlines to compute line position
static const int NUMBER_OF_SCANLINES = 40; // number of scan-lines for line detection
static const unsigned int DIMENSION = 2; // dimension of video frames (used for Ransac plane)
static const double EXPECTED_LINE_SEGMENTATION_SUCCESS_RATE = 0.5; // log a warning if the actual line segmentation success rate (fraction of frames where the line segmentation was successful) is below this threshold

enum PEAK_POS_METRIC_TYPE
{
  PEAK_POS_COG,
  PEAK_POS_START
};
const PEAK_POS_METRIC_TYPE PEAK_POS_METRIC = PEAK_POS_COG;

vtkStandardNewMacro(vtkPlusLineSegmentationAlgo);

//----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkPlusLineSegmentationAlgo::vtkPlusLineSegmentationAlgo()
  : m_TrackedFrameList(vtkSmartPointer<vtkIGSIOTrackedFrameList>::New())
  , m_SaveIntermediateImages(false)
  , IntermediateFilesOutputDirectory("")
  , PlotIntensityProfile(false)
  , m_SignalTimeRangeMin(0.0)
  , m_SignalTimeRangeMax(-1.0)
{
  m_ClipRectangleOrigin[0] = 0;
  m_ClipRectangleOrigin[1] = 0;
  m_ClipRectangleSize[0] = 0;
  m_ClipRectangleSize[1] = 0;
}

//----------------------------------------------------------------------------
vtkPlusLineSegmentationAlgo::~vtkPlusLineSegmentationAlgo()
{
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SetTrackedFrameList(vtkIGSIOTrackedFrameList& aTrackedFrameList)
{
  m_TrackedFrameList->Clear();
  m_TrackedFrameList->AddTrackedFrameList(&aTrackedFrameList);
}

//----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SetTrackedFrame(igsioTrackedFrame& aTrackedFrame)
{
  m_TrackedFrameList->Clear();
  m_TrackedFrameList->AddTrackedFrame(&aTrackedFrame);
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SetSignalTimeRange(double rangeMin, double rangeMax)
{
  m_SignalTimeRangeMin = rangeMin;
  m_SignalTimeRangeMax = rangeMax;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SetSaveIntermediateImages(bool saveIntermediateImages)
{
  m_SaveIntermediateImages = saveIntermediateImages;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SetIntermediateFilesOutputDirectory(const std::string& outputDirectory)
{
  IntermediateFilesOutputDirectory = outputDirectory;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::VerifyVideoInput()
{
  // Make sure video frame list is not empty
  if (m_TrackedFrameList->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("vtkPlusLineSegmentationAlgo video input data verification failed: no frames");
    return PLUS_FAIL;
  }

  // Check if TrackedFrameList is MF oriented BRIGHTNESS image
  if (vtkIGSIOTrackedFrameList::VerifyProperties(m_TrackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusLineSegmentationAlgo video input data verification failed: video data orientation or type is not supported (MF orientation, BRIGHTNESS type is expected)");
    return PLUS_FAIL;
  }

  // Check if there are enough valid consecutive video frames
  int totalNumberOfInvalidVideoFrames = 0;
  int greatestNumberOfConsecutiveInvalidVideoFrames = 0;
  int currentNumberOfConsecutiveInvalidVideoFrames = 0;
  bool signalTimeRangeDefined = (m_SignalTimeRangeMin <= m_SignalTimeRangeMax);
  for (unsigned int i = 0 ; i < m_TrackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    igsioTrackedFrame* trackedFrame = m_TrackedFrameList->GetTrackedFrame(i);
    if (signalTimeRangeDefined && (trackedFrame->GetTimestamp() < m_SignalTimeRangeMin || trackedFrame->GetTimestamp() > m_SignalTimeRangeMax))
    {
      // frame is out of the specified signal range
      continue;
    }
    if (!trackedFrame->GetImageData()->IsImageValid())
    {
      ++totalNumberOfInvalidVideoFrames;
      ++currentNumberOfConsecutiveInvalidVideoFrames;
    }
    else
    {
      if (currentNumberOfConsecutiveInvalidVideoFrames > greatestNumberOfConsecutiveInvalidVideoFrames)
      {
        greatestNumberOfConsecutiveInvalidVideoFrames = currentNumberOfConsecutiveInvalidVideoFrames ;
      }
      currentNumberOfConsecutiveInvalidVideoFrames = 0;
    }
  }

  double percentageOfInvalidVideoFrames  = totalNumberOfInvalidVideoFrames / static_cast<double>(m_TrackedFrameList->GetNumberOfTrackedFrames());
  if (percentageOfInvalidVideoFrames > MAX_PERCENTAGE_OF_INVALID_VIDEO_FRAMES)
  {
    LOG_WARNING("In vtkPlusLineSegmentationAlgo " << 100 * percentageOfInvalidVideoFrames << "% of the video frames were invalid. This warning " <<
                "gets issued whenever more than " << 100 * MAX_PERCENTAGE_OF_INVALID_VIDEO_FRAMES << "% of the video frames are invalid because the " <<
                "accuracy of the computed time offset may be marginalised");
  }

  if (greatestNumberOfConsecutiveInvalidVideoFrames  > MAX_CONSECUTIVE_INVALID_VIDEO_FRAMES)
  {
    LOG_WARNING("In vtkPlusLineSegmentationAlgo there were " << greatestNumberOfConsecutiveInvalidVideoFrames  << " invalid video frames in a row. This warning " <<
                "gets issued whenever there are more than " << MAX_CONSECUTIVE_INVALID_VIDEO_FRAMES << " invalid frames in a row because the " <<
                "accuracy of the computed time offset may be marginalised");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::ComputeVideoPositionMetric()
{
  m_SignalValues.clear();
  m_SignalTimestamps.clear();

  LineParameters nonDetectedLineParams;
  nonDetectedLineParams.lineDetected = false;
  nonDetectedLineParams.lineOriginPoint_Image[0] = 0;
  nonDetectedLineParams.lineOriginPoint_Image[1] = 0;
  nonDetectedLineParams.lineDirectionVector_Image[0] = 0;
  nonDetectedLineParams.lineDirectionVector_Image[1] = 1;
  m_LineParameters.assign(m_TrackedFrameList->GetNumberOfTrackedFrames(), nonDetectedLineParams);

  //  For each video frame, detect line and extract mindpoint and slope parameters
  int numberOfSuccessfulLineSegmentations = 0;
  bool signalTimeRangeDefined = (m_SignalTimeRangeMin <= m_SignalTimeRangeMax);
  for (unsigned int frameNumber = 0; frameNumber < m_TrackedFrameList->GetNumberOfTrackedFrames(); ++frameNumber)
  {
    LOG_TRACE("Calculating video position metric for frame " << frameNumber);
    igsioTrackedFrame* trackedFrame = m_TrackedFrameList->GetTrackedFrame(frameNumber);
    if (signalTimeRangeDefined && (trackedFrame->GetTimestamp() < m_SignalTimeRangeMin || trackedFrame->GetTimestamp() > m_SignalTimeRangeMax))
    {
      // frame is out of the specified signal range
      LOG_TRACE("Skip frame, it is out of the valid signal range");
      continue;
    }

    // Get current image
    if (trackedFrame->GetImageData()->GetVTKScalarPixelType() != VTK_UNSIGNED_CHAR)
    {
      LOG_ERROR("vtkPlusLineSegmentationAlgo::ComputeVideoPositionMetric only supports 8-bit images");
      continue;
    }
    auto localImage = CharImageType::New();
    PlusCommon::DeepCopyVtkVolumeToItkImage<CharPixelType>(trackedFrame->GetImageData()->GetImage(), localImage);
    if (localImage.IsNull())
    {
      // Dropped frame
      LOG_ERROR("vtkPlusLineSegmentationAlgo::ComputeVideoPositionMetric failed to retrieve image data from frame");
      continue;
    }

    // Create an image duplicator to copy the original image
    typedef itk::ImageDuplicator<CharImageType> DuplicatorType;
    auto duplicator = DuplicatorType::New();
    CharImageType::Pointer scanlineImage;
    if (m_SaveIntermediateImages == true)
    {
      duplicator->SetInputImage(localImage);
      duplicator->Update();

      // Create an image copy to draw the scanlines on
      scanlineImage = duplicator->GetOutput();
    }

    std::vector<itk::Point<double, 2> > intensityPeakPositions;
    CharImageType::RegionType region = localImage->GetLargestPossibleRegion();
    LimitToClipRegion(region);

    int numOfValidScanlines = 0;

    for (int currScanlineNum = 0; currScanlineNum < NUMBER_OF_SCANLINES; ++currScanlineNum)
    {
      // Set the scanline start pixel
      CharImageType::IndexType startPixel;
      double scanlineSpacingPix = static_cast<double>(region.GetSize()[0] - 1) / (NUMBER_OF_SCANLINES - 1);
      startPixel[0] = region.GetIndex()[0] + scanlineSpacingPix * (currScanlineNum);
      startPixel[1] = region.GetIndex()[1];

      // Set the scanline end pixel
      CharImageType::IndexType endPixel;
      endPixel[0] = startPixel[0];
      endPixel[1] = startPixel[1] + region.GetSize()[1] - 1;

      std::deque<int> intensityProfile; // Holds intensity profile of the line
      itk::LineIterator<CharImageType> it(localImage, startPixel, endPixel);
      it.GoToBegin();

      itk::LineIterator<CharImageType>* itScanlineImage = NULL;
      if (m_SaveIntermediateImages == true)
      {
        // Iterator for the scanline image copy
        // it's time-consuming to instantiate this iterator, so only do it if intermediate image saving is requested
        itScanlineImage = new itk::LineIterator<CharImageType>(scanlineImage, startPixel, endPixel);
        itScanlineImage->GoToBegin();
      }

      while (!it.IsAtEnd())
      {
        intensityProfile.push_back((int)it.Get());
        if (m_SaveIntermediateImages == true)
        {
          // Set the pixels on the scanline image copy to white
          itScanlineImage->Set(255);
          ++(*itScanlineImage);
        }
        ++it;
      }

      // Delete the iterator declared with new()
      if (itScanlineImage != NULL)
      {
        delete itScanlineImage;
        itScanlineImage = NULL;
      }

      if (this->PlotIntensityProfile)
      {
        // Plot the intensity profile
        PlotIntArray(intensityProfile);
      }

      // Find the max intensity value from the peak with the largest area
      int maxFromLargestArea = -1;
      int maxFromLargestAreaIndex = -1;
      int startOfMaxArea = -1;
      if (FindLargestPeak(intensityProfile, maxFromLargestArea, maxFromLargestAreaIndex, startOfMaxArea) == PLUS_SUCCESS)
      {
        double currPeakPos_y = -1;
        switch (PEAK_POS_METRIC)
        {
          case PEAK_POS_COG:
            {
              /* Use center-of-gravity (COG) as peak-position metric*/
              if (ComputeCenterOfGravity(intensityProfile, startOfMaxArea, currPeakPos_y) != PLUS_SUCCESS)
              {
                // unable to compute center-of-gravity; this scanline is invalid
                continue;
              }
              break;
            }
          case PEAK_POS_START:
            {
              /* Use peak start as peak-position metric*/
              if (FindPeakStart(intensityProfile, maxFromLargestArea, startOfMaxArea, currPeakPos_y) != PLUS_SUCCESS)
              {
                // unable to compute peak start; this scanline is invalid
                continue;
              }
              break;
            }
        }

        itk::Point<double, 2> currPeakPos;
        currPeakPos[0] = static_cast<double>(startPixel[0]);
        currPeakPos[1] = startPixel[1] + currPeakPos_y;
        intensityPeakPositions.push_back(currPeakPos);
        ++numOfValidScanlines;

      } // end if() found intensity peak

    } // end currScanlineNum loop

    if (numOfValidScanlines < MINIMUM_NUMBER_OF_VALID_SCANLINES)
    {
      //TODO: drop the frame from the analysis
      LOG_DEBUG("Only " << numOfValidScanlines << " valid scanlines; this is less than the required " << MINIMUM_NUMBER_OF_VALID_SCANLINES << ". Skipping frame" << frameNumber);
    }

    LineParameters params;
    ComputeLineParameters(intensityPeakPositions, params);
    if (!params.lineDetected)
    {
      LOG_DEBUG("Unable to compute line parameters for frame " << frameNumber);
      continue;
    }
    if (params.lineDirectionVector_Image[0] < MIN_X_SLOPE_COMPONENT_FOR_DETECTED_LINE)
    {
      // Line is close to vertical, skip frame because intersection of
      // line with image's horizontal half point is unstable
      LOG_TRACE("Line on frame " << frameNumber << " is too close to vertical, skip the frame");
      continue;
    }

    ++numberOfSuccessfulLineSegmentations;
    m_LineParameters[frameNumber] = params;

    // Store the y-value of the line, when the line's x-value is half of the image's width
    double t = (region.GetIndex()[0] + 0.5 * region.GetSize()[0] - params.lineOriginPoint_Image[0]) / params.lineDirectionVector_Image[0];
    m_SignalValues.push_back(std::abs(params.lineOriginPoint_Image[1] + t * params.lineDirectionVector_Image[1]));

    //  Store timestamp for image frame
    m_SignalTimestamps.push_back(m_TrackedFrameList->GetTrackedFrame(frameNumber)->GetTimestamp());

    if (m_SaveIntermediateImages == true)
    {
      SaveIntermediateImage(frameNumber, scanlineImage,
                            params.lineOriginPoint_Image[0], params.lineOriginPoint_Image[1], params.lineDirectionVector_Image[0], params.lineDirectionVector_Image[1],
                            numOfValidScanlines, intensityPeakPositions);
    }

  } // end frameNum loop

  double segmentationSuccessRate = double(numberOfSuccessfulLineSegmentations) / m_TrackedFrameList->GetNumberOfTrackedFrames();
  if (segmentationSuccessRate < EXPECTED_LINE_SEGMENTATION_SUCCESS_RATE)
  {
    LOG_WARNING("Line segmentation success rate is very low (" << segmentationSuccessRate * 100 << "%): a line could only be detected on " << numberOfSuccessfulLineSegmentations << " frames out of " << m_TrackedFrameList->GetNumberOfTrackedFrames());
  }

  bool plotVideoMetric = vtkPlusLogger::Instance()->GetLogLevel() >= vtkPlusLogger::LOG_LEVEL_TRACE;
  if (plotVideoMetric)
  {
    PlotDoubleArray(m_SignalValues);
  }

  return PLUS_SUCCESS;

} //  End LineDetection

//-----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::FindPeakStart(std::deque<int>& intensityProfile, int maxFromLargestArea, int startOfMaxArea, double& startOfPeak)
{
  // Start of peak is defined as the location at which it reaches 50% of its maximum value.
  double startPeakValue = maxFromLargestArea * 0.5;

  int pixelIndex = startOfMaxArea;

  while (intensityProfile.at(pixelIndex) <= startPeakValue)
  {
    ++pixelIndex;
  }

  startOfPeak = --pixelIndex;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::FindLargestPeak(std::deque<int>& intensityProfile, int& maxFromLargestArea, int& maxFromLargestAreaIndex, int& startOfMaxArea)
{
  int currentLargestArea = 0;
  int currentArea = 0;
  int currentMaxFromLargestArea = 0;
  int currentMaxFromLargestAreaIndex = 0;
  int currentMax = 0;
  int currentMaxIndex = 0;
  bool underPeak = false;
  int currentStartOfMaxArea = 0;
  int currentStart = 0;

  if (intensityProfile.size() == 0)
  {
    LOG_ERROR("Intensity contains no elements");
    return PLUS_FAIL;
  }

  double intensityMax = intensityProfile.at(0);
  for (unsigned int pixelLoc = 1; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if (intensityProfile.at(pixelLoc) > intensityMax)
    {
      intensityMax = intensityProfile.at(pixelLoc);
    }
  }

  double peakIntensityThreshold = intensityMax * INTESNITY_THRESHOLD_PERCENTAGE_OF_PEAK;

  for (unsigned int pixelLoc = 0; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if (intensityProfile.at(pixelLoc) > peakIntensityThreshold  && !underPeak)
    {
      // reached start of the peak
      underPeak = true;
      currentMax = intensityProfile.at(pixelLoc);
      currentMaxIndex = pixelLoc;
      currentArea = intensityProfile.at(pixelLoc);
      currentStart = pixelLoc;
    }
    else if (intensityProfile.at(pixelLoc) > peakIntensityThreshold  && underPeak)
    {
      // still under the the peak, cumulate the area
      currentArea += intensityProfile.at(pixelLoc);

      if (intensityProfile.at(pixelLoc) > currentMax)
      {
        currentMax = intensityProfile.at(pixelLoc);
        currentMaxIndex = pixelLoc;
      }
    }
    else if (intensityProfile.at(pixelLoc) < peakIntensityThreshold && underPeak)
    {
      // exited the peak area
      underPeak = false;
      if (currentArea > currentLargestArea)
      {
        currentLargestArea = currentArea;
        currentMaxFromLargestArea = currentMax;
        currentMaxFromLargestAreaIndex = currentMaxIndex;
        currentStartOfMaxArea = currentStart;
      }
    }
  } //end loop through intensity profile

  maxFromLargestArea = currentMaxFromLargestArea;
  maxFromLargestAreaIndex = currentMaxFromLargestAreaIndex;
  startOfMaxArea = currentStartOfMaxArea;

  if (currentLargestArea == 0)
  {
    // No peak for this scanline
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------

PlusStatus vtkPlusLineSegmentationAlgo::ComputeCenterOfGravity(std::deque<int>& intensityProfile, int startOfMaxArea, double& centerOfGravity)
{
  if (intensityProfile.size() == 0)
  {
    return PLUS_FAIL;
  }

  double intensityMax = intensityProfile.at(0);
  for (unsigned int pixelLoc = 1; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if (intensityProfile.at(pixelLoc) > intensityMax)
    {
      intensityMax = intensityProfile.at(pixelLoc);
    }
  }

  double peakIntensityThreshold = intensityMax * INTESNITY_THRESHOLD_PERCENTAGE_OF_PEAK;

  int pixelLoc = startOfMaxArea;
  int pointsInPeak = 0;
  double intensitySum = 0;
  while (intensityProfile.at(pixelLoc) > peakIntensityThreshold)
  {
    intensitySum += pixelLoc * intensityProfile.at(pixelLoc);
    pointsInPeak += intensityProfile.at(pixelLoc);
    ++pixelLoc;
  }

  if (pointsInPeak == 0)
  {
    return PLUS_FAIL;
  }

  centerOfGravity = intensitySum / pointsInPeak;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::ComputeLineParameters(std::vector<itk::Point<double, 2> >& data, LineParameters& outputParameters)
{
  outputParameters.lineDetected = false;

  std::vector<double> ransacParameterResult;
  typedef itk::PlaneParametersEstimator<DIMENSION> PlaneEstimatorType;
  typedef itk::RANSAC<itk::Point<double, DIMENSION>, double> RANSACType;

  //create and initialize the parameter estimator
  double maximalDistanceFromPlane = 0.5;
  auto planeEstimator = PlaneEstimatorType::New();
  planeEstimator->SetDelta(maximalDistanceFromPlane);
  planeEstimator->LeastSquaresEstimate(data, ransacParameterResult);
  if (ransacParameterResult.empty())
  {
    LOG_DEBUG("Unable to fit line through points with least squares estimation");
  }
  else
  {
    LOG_TRACE("Least squares line parameters (n, a):");
    for (unsigned int i = 0; i < (2 * DIMENSION - 1); i++)
    {
      LOG_TRACE(" LS parameter: " << ransacParameterResult[i]);
    }
  }

  //create and initialize the RANSAC algorithm
  double desiredProbabilityForNoOutliers = 0.999;
  auto ransacEstimator = RANSACType::New();

  try
  {
    ransacEstimator->SetData(data);
  }
  catch (std::exception& e)
  {
    LOG_DEBUG(e.what());
    return;
  }

  try
  {
    ransacEstimator->SetParametersEstimator(planeEstimator.GetPointer());
  }
  catch (std::exception& e)
  {
    LOG_DEBUG(e.what());
    return;
  }


  try
  {
    ransacEstimator->Compute(ransacParameterResult, desiredProbabilityForNoOutliers);
  }
  catch (std::exception& e)
  {
    LOG_DEBUG(e.what());
    return;
  }

  if (ransacParameterResult.empty())
  {
    LOG_DEBUG("Unable to fit line through points with RANSAC, line segmentation failed");
    return;
  }

  LOG_TRACE("RANSAC line fitting parameters (n, a):");

  for (unsigned int i = 0; i < (2 * DIMENSION - 1); i++)
  {
    LOG_TRACE(" RANSAC parameter: " << ransacParameterResult[i]);
  }

  outputParameters.lineDetected = true;
  outputParameters.lineDirectionVector_Image[0] = -ransacParameterResult[1];
  outputParameters.lineDirectionVector_Image[1] = ransacParameterResult[0];
  outputParameters.lineOriginPoint_Image[0] = ransacParameterResult[2];
  outputParameters.lineOriginPoint_Image[1] = ransacParameterResult[3];
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SaveIntermediateImage(int frameNumber, CharImageType::Pointer scanlineImage, double x_0, double y_0, double r_x, double r_y, int numOfValidScanlines, const std::vector<itk::Point<double, 2> >& intensityPeakPositions)
{
  // The scanlineImage already contains the vertical lines, but not the segmented line.

  typedef itk::RGBPixel<unsigned char> rgbPixelType;
  typedef itk::Image<rgbPixelType, 2> rgbImageType;
  auto rgbImageCopy = rgbImageType::New();

  CharImageType::RegionType fullImageRegion = scanlineImage->GetLargestPossibleRegion();

  rgbImageType::IndexType start;
  start[0] =   fullImageRegion.GetIndex()[0];  // first index on X
  start[1] =   fullImageRegion.GetIndex()[0];  // first index on Y
  rgbImageType::SizeType  size;
  size[0]  = fullImageRegion.GetSize()[0];  // size along X
  size[1]  = fullImageRegion.GetSize()[1];  // size along Y
  rgbImageType::RegionType region;
  region.SetIndex(start);
  region.SetSize(size);
  rgbImageCopy->SetRegions(region);
  rgbImageCopy->Allocate();

  // Copy grayscale image to an RGB image to allow drawing of annotations in color
  for (unsigned int x_coord = region.GetIndex()[0]; x_coord  < region.GetSize()[0]; ++x_coord)
  {
    for (unsigned int y_coord = region.GetIndex()[1]; y_coord < region.GetSize()[1]; ++y_coord)
    {
      rgbImageType::IndexType currRgbImageIndex;
      currRgbImageIndex[0] = x_coord;
      currRgbImageIndex[1] = y_coord;

      CharImageType::IndexType currLocalImageIndex;
      currLocalImageIndex[0] = x_coord;
      currLocalImageIndex[1] = y_coord;

      CharPixelType currLocalImagePixelVal = scanlineImage->GetPixel(currLocalImageIndex);
      rgbPixelType currRgbImagePixelVal;
      currRgbImagePixelVal.Set(currLocalImagePixelVal, currLocalImagePixelVal, currLocalImagePixelVal);
      rgbImageCopy->SetPixel(currRgbImageIndex, currRgbImagePixelVal);
    }
  }

  float diag = sqrtf((float)(size[0] * size[0] + size[1] * size[1]));

  // Draw detected line
  for (int i = static_cast<int>(-diag); i < static_cast<int>(diag); ++i)
  {
    rgbImageType::IndexType currIndex;
    currIndex[0] = static_cast<int>(x_0 + i * r_x);
    currIndex[1] = static_cast<int>(y_0 + i * r_y);

    if (fullImageRegion.IsInside(currIndex))
    {
      rgbPixelType currRgbImagePixelVal;
      currRgbImagePixelVal.Set(0, 0, 255);
      rgbImageCopy->SetPixel(currIndex, currRgbImagePixelVal);
    }
  }

  // Draw intensity peaks (as squares)
  for (int scanLineIndex = 0; scanLineIndex < numOfValidScanlines; ++scanLineIndex)
  {
    unsigned int sqareCenterCoordX = static_cast<unsigned int>(intensityPeakPositions.at(scanLineIndex).GetElement(0));
    unsigned int sqareCenterCoordY = static_cast<unsigned int>(intensityPeakPositions.at(scanLineIndex).GetElement(1));

    for (unsigned int x = sqareCenterCoordX - 3; x < sqareCenterCoordX + 3; ++x)
    {
      for (unsigned int y = sqareCenterCoordY - 3; y < sqareCenterCoordY + 3; ++y)
      {
        rgbImageType::IndexType currIndex;
        currIndex[0] =  x;  // index on X
        currIndex[1] =  y;  // index on Y

        rgbPixelType currRgbImagePixelVal;
        currRgbImagePixelVal.Set(0, 0, 255);

        rgbImageCopy->SetPixel(currIndex, currRgbImagePixelVal);
      }
    }
  }

  std::ostringstream rgbImageFilename;
  if (!IntermediateFilesOutputDirectory.empty())
  {
    rgbImageFilename << IntermediateFilesOutputDirectory << "/";
  }
  rgbImageFilename << "LineSegmentationResult_" << std::setw(3) << std::setfill('0') << frameNumber << ".png" << std::ends;

  typedef itk::ImageFileWriter<rgbImageType> rgbImageWriterType;
  auto rgbImageWriter = rgbImageWriterType::New();

  rgbImageWriter->SetFileName(rgbImageFilename.str());
  rgbImageWriter->SetInput(rgbImageCopy);
  try
  {
    rgbImageWriter->Update();
  }
  catch (itk::ExceptionObject& e)
  {
    LOG_ERROR("Failed to write intermediate image in line segmentation algorithm, check if the destination directory exists: " << rgbImageFilename.str() << "\n" << e);
    return;
  }

  LOG_DEBUG("Line segmentation intermediate image is saved to: " << rgbImageFilename.str());
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::Update()
{
  if (VerifyVideoInput() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (ComputeVideoPositionMetric() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::PlotIntArray(const std::deque<int>& intensityValues)
{
#ifdef PLUS_RENDERING_ENABLED
  //  Create table
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  //  Create array corresponding to the time values of the tracker plot
  vtkSmartPointer<vtkIntArray> arrPixelPositions = vtkSmartPointer<vtkIntArray>::New();
  arrPixelPositions->SetName("Pixel Positions");
  table->AddColumn(arrPixelPositions);

  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkIntArray> arrIntensityProfile = vtkSmartPointer<vtkIntArray>::New();
  arrIntensityProfile->SetName("Intensity Profile");
  table->AddColumn(arrIntensityProfile);

  // Set the tracker data
  table->SetNumberOfRows(intensityValues.size());
  for (unsigned int i = 0; i < intensityValues.size(); ++i)
  {
    table->SetValue(i, 0, i);
    table->SetValue(i, 1, (intensityValues.at(i)));
  }

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  // Add the two line plots
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);

  line->SetInputData(table, 0, 1);

  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
#else
  LOG_ERROR("Function not available when VTK_RENDERING_BACKEND is None!");
#endif
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::PlotDoubleArray(const std::deque<double>& intensityValues)
{
#ifdef PLUS_RENDERING_ENABLED
  //  Create table
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  //  Create array corresponding to the time values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrPixelPositions = vtkSmartPointer<vtkDoubleArray>::New();
  arrPixelPositions->SetName("Pixel Positions");
  table->AddColumn(arrPixelPositions);

  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrIntensityProfile = vtkSmartPointer<vtkDoubleArray>::New();
  arrIntensityProfile->SetName("Intensity Profile");
  table->AddColumn(arrIntensityProfile);

  // Set the tracker data
  table->SetNumberOfRows(intensityValues.size());
  for (unsigned int i = 0; i < intensityValues.size(); ++i)
  {
    table->SetValue(i, 0, i);
    table->SetValue(i, 1, (intensityValues.at(i)));
  }

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  // Add the two line plots
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);

  line->SetInputData(table, 0, 1);

  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
#else
  LOG_ERROR("Function not available when VTK_RENDERING_BACKEND is None!");
#endif
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::GetDetectedTimestamps(std::deque<double>& timestamps)
{
  timestamps = m_SignalTimestamps;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::GetDetectedPositions(std::deque<double>& positions)
{
  positions = m_SignalValues;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::GetDetectedLineParameters(std::vector<LineParameters>& parameters)
{
  parameters = m_LineParameters;
}

//-----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::SetClipRectangle(int clipRectangleOriginPix[2], int clipRectangleSizePix[2])
{
  m_ClipRectangleOrigin[0] = clipRectangleOriginPix[0];
  m_ClipRectangleOrigin[1] = clipRectangleOriginPix[1];
  m_ClipRectangleSize[0] = clipRectangleSizePix[0];
  m_ClipRectangleSize[1] = clipRectangleSizePix[1];
}

//----------------------------------------------------------------------------
void vtkPlusLineSegmentationAlgo::LimitToClipRegion(CharImageType::RegionType& region)
{
  if ((m_ClipRectangleSize[0] <= 0) || (m_ClipRectangleSize[1] <= 0))
  {
    // no clipping
    return;
  }

  // Clipping enabled
  CharImageType::IndexValueType clipRectangleOrigin[2] =
  {
    m_ClipRectangleOrigin[0],
    m_ClipRectangleOrigin[1]
  };
  CharImageType::SizeValueType clipRectangleSize[2] =
  {
    m_ClipRectangleSize[0],
    m_ClipRectangleSize[1]
  };

  // Adjust clipping region origin and size to fit inside the frame region
  CharImageType::IndexType imageOrigin = region.GetIndex();
  CharImageType::SizeType imageSize = region.GetSize();
  if (clipRectangleOrigin[0] < imageOrigin[0]
      || clipRectangleOrigin[1] < imageOrigin[1]
      || clipRectangleOrigin[0] >= imageOrigin[0] + static_cast<CharImageType::OffsetValueType>(imageSize[0])
      || clipRectangleOrigin[1] >= imageOrigin[1] + static_cast<CharImageType::OffsetValueType>(imageSize[1]))
  {
    LOG_WARNING("ClipRectangleOrigin is invalid (" << clipRectangleOrigin[0] << ", " << clipRectangleOrigin[1] <<
                "). The frame size is "
                << imageSize[0] << "x" << imageSize[1] <<
                ". Using ("
                << imageOrigin[0] << "," << imageOrigin[1] <<
                ") as ClipRectangleOrigin.");
    clipRectangleOrigin[0] = 0;
    clipRectangleOrigin[1] = 0;
  }
  if (clipRectangleOrigin[0] + clipRectangleSize[0] >= imageOrigin[0] + imageSize[0])
  {
    // rectangle size is out of the framSize bounds, clip it to the available size
    clipRectangleSize[0] = imageOrigin[0] + imageSize[0] - clipRectangleOrigin[0];
    LOG_WARNING("Adjusting ClipRectangleSize x to " << clipRectangleSize[0]);
  }
  if (clipRectangleOrigin[1] + clipRectangleSize[1] > imageSize[1])
  {
    // rectangle size is out of the framSize bounds, clip it to the available size
    clipRectangleSize[1] = imageOrigin[1] + imageSize[1] - clipRectangleOrigin[1];
    LOG_WARNING("Adjusting ClipRectangleSize y to " << clipRectangleSize[1]);
  }

  if ((clipRectangleSize[0] <= 0) || (clipRectangleSize[1] <= 0))
  {
    // after the adjustment it seems that there is no clipping is needed
    return;
  }

  // Save updated clipping parameters to the region
  imageOrigin[0] = clipRectangleOrigin[0];
  imageOrigin[1] = clipRectangleOrigin[1];
  imageSize[0] = clipRectangleSize[0];
  imageSize[1] = clipRectangleSize[1];
  region.SetIndex(imageOrigin);
  region.SetSize(imageSize);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(lineSegmentationElement, aConfig, "vtkPlusLineSegmentationAlgo");

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SaveIntermediateImages, lineSegmentationElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(PlotIntensityProfile, lineSegmentationElement);

  this->IntermediateFilesOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory();
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(IntermediateFilesOutputDirectory, lineSegmentationElement);

  int clipRectangleOrigin[2];
  int clipRectangleSize[2];
  if (!lineSegmentationElement->GetVectorAttribute("ClipRectangleOrigin", 2, clipRectangleOrigin) ||
      !lineSegmentationElement->GetVectorAttribute("ClipRectangleSize", 2, clipRectangleSize))
  {
    LOG_WARNING("Cannot find ClipRectangleOrigin or ClipRectangleSize attribute in the vtkPlusLineSegmentationAlgo configuration file; Using the largest ROI possible.");
    m_ClipRectangleOrigin[0] = 0;
    m_ClipRectangleOrigin[1] = 0;
    m_ClipRectangleSize[0] = -1;
    m_ClipRectangleSize[1] = -1;
  }
  else
  {
    if (clipRectangleOrigin[0] < 0)
    {
      clipRectangleOrigin[0] = 0;
    }
    if (clipRectangleOrigin[1] < 0)
    {
      clipRectangleOrigin[1] = 0;
    }
    if (clipRectangleSize[0] < 0)
    {
      clipRectangleSize[0] = -1;
    }
    if (clipRectangleSize[1] < 0)
    {
      clipRectangleSize[1] = -1;
    }

    m_ClipRectangleOrigin[0] = clipRectangleOrigin[0];
    m_ClipRectangleOrigin[1] = clipRectangleOrigin[1];
    m_ClipRectangleSize[0] = clipRectangleSize[0];
    m_ClipRectangleSize[1] = clipRectangleSize[1];
  }

  double signalTimeRange[2] = {0};
  if (lineSegmentationElement->GetVectorAttribute("SignalTimeRange", 2, signalTimeRange))
  {
    m_SignalTimeRangeMin = signalTimeRange[0];
    m_SignalTimeRangeMax = signalTimeRange[1];
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLineSegmentationAlgo::Reset()
{
  m_SignalValues.clear();
  m_SignalTimestamps.clear();
  m_LineParameters.clear();

  return PLUS_SUCCESS;
}
