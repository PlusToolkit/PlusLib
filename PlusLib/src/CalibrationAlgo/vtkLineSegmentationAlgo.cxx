/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlaneParametersEstimator.h"
#include "PlusConfigure.h"
#include "RANSAC.h"
#include "TrackedFrame.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkLineIterator.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkRGBPixel.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "vtkChartXY.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkLineSegmentationAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkPen.h"
#include "vtkPlot.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTable.h"
#include "vtkTrackedFrameList.h"

static const double INTESNITY_THRESHOLD_PERCENTAGE_OF_PEAK = 0.5; // threshold (as the percentage of the peak intensity along a scanline) for COG
static const double MAX_CONSECUTIVE_INVALID_VIDEO_FRAMES = 10; // the maximum number of consecutive invalid frames before warning message issued
static const double MAX_PERCENTAGE_OF_INVALID_VIDEO_FRAMES = 0.1; // the maximum percentage of the invalid frames before warning message issued
static const double MIN_X_SLOPE_COMPONENT_FOR_DETECTED_LINE = 0.01; // if the detected line's slope's x-component is less than this (i.e. almost vertical), skip frame
static const int MINIMUM_NUMBER_OF_VALID_SCANLINES = 5; // minimum number of valid scanlines to compute line position
static const int NUMBER_OF_SCANLINES = 40; // number of scan-lines for line detection
static const unsigned int DIMENSION = 2; // dimension of video frames (used for Ransac plane)

enum PEAK_POS_METRIC_TYPE
{
  PEAK_POS_COG,
  PEAK_POS_START
};
const PEAK_POS_METRIC_TYPE PEAK_POS_METRIC = PEAK_POS_COG;

vtkCxxRevisionMacro(vtkLineSegmentationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkLineSegmentationAlgo);

//----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
vtkLineSegmentationAlgo::vtkLineSegmentationAlgo() 
: m_TrackedFrameList(NULL)
, m_SaveIntermediateImages(false)
, m_IntermediateFilesOutputDirectory("")
, m_SignalTimeRangeMin(0.0)
, m_SignalTimeRangeMax(-1.0)
{  
  m_ClipRectangleOrigin[0] = 0;
  m_ClipRectangleOrigin[1] = 0;
  m_ClipRectangleSize[0] = 0;
  m_ClipRectangleSize[1] = 0;
}

//----------------------------------------------------------------------------
vtkLineSegmentationAlgo::~vtkLineSegmentationAlgo() 
{
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::SetTrackedFrameList(vtkTrackedFrameList* aTrackedFrameList)
{
  m_TrackedFrameList = aTrackedFrameList;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::SetSignalTimeRange(double rangeMin, double rangeMax)
{
  m_SignalTimeRangeMin = rangeMin;
  m_SignalTimeRangeMax = rangeMax;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::SetSaveIntermediateImages(bool saveIntermediateImages)
{
  m_SaveIntermediateImages = saveIntermediateImages;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::SetIntermediateFilesOutputDirectory(const std::string &outputDirectory)
{
  m_IntermediateFilesOutputDirectory = outputDirectory;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::VerifyVideoInput()
{
  // Check if video frames have been assigned
  if(m_TrackedFrameList == NULL)
  {
    LOG_ERROR("vtkLineSegmentationAlgo video input data verification failed: no video data is set");
    return PLUS_FAIL;
  }

  // Make sure video frame list is not empty
  if(m_TrackedFrameList->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("vtkLineSegmentationAlgo video input data verification failed: no frames");
    return PLUS_FAIL;
  }

  // Check if TrackedFrameList is MF oriented BRIGHTNESS image
  if (vtkTrackedFrameList::VerifyProperties(m_TrackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS)!=PLUS_SUCCESS)
  {
    LOG_ERROR("vtkLineSegmentationAlgo video input data verification failed: video data orientation or type is not supported (MF orientation, BRIGHTNESS type is expected)");
    return PLUS_FAIL; 
  }

  // Check if there are enough valid consecutive video frames

  int totalNumberOfInvalidVideoFrames = 0;
  int greatestNumberOfConsecutiveInvalidVideoFrames = 0;
  int currentNumberOfConsecutiveInvalidVideoFrames = 0;
  bool signalTimeRangeDefined=(m_SignalTimeRangeMin<=m_SignalTimeRangeMax);
  for(unsigned int i = 0 ; i < m_TrackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    TrackedFrame* trackedFrame=m_TrackedFrameList->GetTrackedFrame(i);
    if (signalTimeRangeDefined && (trackedFrame->GetTimestamp()<m_SignalTimeRangeMin || trackedFrame->GetTimestamp()>m_SignalTimeRangeMax))
    {
      // frame is out of the specified signal range
      continue;
    }
    if(!trackedFrame->GetImageData()->IsImageValid())
    {
      ++totalNumberOfInvalidVideoFrames;
      ++currentNumberOfConsecutiveInvalidVideoFrames;
    }
    else
    {
      if(currentNumberOfConsecutiveInvalidVideoFrames > greatestNumberOfConsecutiveInvalidVideoFrames)
      {
        greatestNumberOfConsecutiveInvalidVideoFrames = currentNumberOfConsecutiveInvalidVideoFrames ;
      } 
      currentNumberOfConsecutiveInvalidVideoFrames = 0;
    }
  }

  double percentageOfInvalidVideoFrames  = totalNumberOfInvalidVideoFrames / static_cast<double>(m_TrackedFrameList->GetNumberOfTrackedFrames());
  if(percentageOfInvalidVideoFrames > MAX_PERCENTAGE_OF_INVALID_VIDEO_FRAMES)
  {
    LOG_WARNING("In vtkLineSegmentationAlgo "<<100*percentageOfInvalidVideoFrames << "% of the video frames were invalid. This warning " <<
      "gets issued whenever more than " << 100*MAX_PERCENTAGE_OF_INVALID_VIDEO_FRAMES << "% of the video frames are invalid because the " <<
      "accuracy of the computed time offset may be marginalised");
  }

  if(greatestNumberOfConsecutiveInvalidVideoFrames  > MAX_CONSECUTIVE_INVALID_VIDEO_FRAMES)
  {
    LOG_WARNING("In vtkLineSegmentationAlgo there were " << greatestNumberOfConsecutiveInvalidVideoFrames  << " invalid video frames in a row. This warning " <<
      "gets issued whenever there are more than " << MAX_CONSECUTIVE_INVALID_VIDEO_FRAMES << " invalid frames in a row because the " <<
      "accuracy of the computed time offset may be marginalised");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::ComputeVideoPositionMetric()
{
  m_SignalValues.clear();
  m_SignalTimestamps.clear();
  m_LineParameters.clear();

  //  For each video frame, detect line and extract mindpoint and slope parameters
  bool signalTimeRangeDefined=(m_SignalTimeRangeMin<=m_SignalTimeRangeMax);
  for(unsigned int frameNumber = 0; frameNumber < m_TrackedFrameList->GetNumberOfTrackedFrames(); ++frameNumber)
  {
    LOG_TRACE("Calculating video position metric for frame " << frameNumber);
    TrackedFrame* trackedFrame=m_TrackedFrameList->GetTrackedFrame(frameNumber);
    if (signalTimeRangeDefined && (trackedFrame->GetTimestamp()<m_SignalTimeRangeMin || trackedFrame->GetTimestamp()>m_SignalTimeRangeMax))
    {
      // frame is out of the specified signal range
      LOG_TRACE("Skip frame, it is out of the valid signal range");
      continue;
    }

    typedef float floatPixelType; //  The type of pixel used for the Hough accumulator

    // Get curent image
    CharImageType::Pointer localImage = trackedFrame->GetImageData()->GetImage<CharPixelType>();

    if(localImage.IsNull())
    {
      // Dropped frame
      continue;
    }

    // Create an image duplicator to copy the original image
    typedef itk::ImageDuplicator<CharImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    CharImageType::Pointer scanlineImage;
    CharImageType::Pointer IntensityPeakAndRansacLineImage;
    if(m_SaveIntermediateImages == true)
    {
      duplicator->SetInputImage(localImage);
      duplicator->Update();

      // Create an image copy to draw the scanlines on
      scanlineImage = duplicator->GetOutput();

      // Create an image copy to draw the detected intensity peaks and Ransac line
      IntensityPeakAndRansacLineImage = duplicator->GetOutput();
    }

    std::vector<itk::Point<double,2> > intensityPeakPositions;
    CharImageType::RegionType region = localImage->GetLargestPossibleRegion();
    LimitToClipRegion(region);

    int numOfValidScanlines = 0;

    for(int currScanlineNum = 0; currScanlineNum < NUMBER_OF_SCANLINES; ++currScanlineNum)
    {
      // Set the scanline start pixel
      CharImageType::IndexType startPixel;
      int scanlineSpacingPix = static_cast<int>(region.GetSize()[0] / (NUMBER_OF_SCANLINES + 1) );
      startPixel[0] = region.GetIndex()[0]+scanlineSpacingPix * (currScanlineNum + 1); // TODO: why the +1?
      startPixel[1] = region.GetIndex()[1];

      // Set the scanline end pixel
      CharImageType::IndexType endPixel;
      endPixel[0] = startPixel[0];
      endPixel[1] = startPixel[1]+region.GetSize()[1] - 1;

      std::deque<int> intensityProfile; // Holds intensity profile of the line
      itk::LineIterator<CharImageType> it(localImage, startPixel, endPixel); 
      it.GoToBegin();

      itk::LineIterator<CharImageType> *itScanlineImage = NULL;
      if(m_SaveIntermediateImages == true)
      {
        // Iterator for the scanline image copy
        // TODO: explain that it's expensive to instantiate this object
        itScanlineImage = new itk::LineIterator<CharImageType>(scanlineImage, startPixel, endPixel);
        itScanlineImage->GoToBegin();
      }

      while (!it.IsAtEnd())
      {
        intensityProfile.push_back((int)it.Get());
        if(m_SaveIntermediateImages == true)
        {
          // Set the pixels on the scanline image copy to white
          itScanlineImage->Set(255);
          ++(*itScanlineImage);
        }
        ++it;
      }

      // Delete the iterator declared with new()
      if(itScanlineImage != NULL)
      {
        delete itScanlineImage;
        itScanlineImage = NULL;
      }

      bool plotIntensityProfile = vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE;
      if(plotIntensityProfile)
      {
        // Plot the intensity profile
        PlotIntArray(intensityProfile);
      }

      // Find the max intensity value from the peak with the largest area
      int MaxFromLargestArea = -1;
      int MaxFromLargestAreaIndex = -1;
      int startOfMaxArea = -1;
      if(FindLargestPeak(intensityProfile, MaxFromLargestArea, MaxFromLargestAreaIndex, startOfMaxArea) == PLUS_SUCCESS)
      {
        double currPeakPos_y = -1; 
        switch (PEAK_POS_METRIC)
        {
        case PEAK_POS_COG:
          {
            /* Use center-of-gravity (COG) as peak-position metric*/
            if(ComputeCenterOfGravity(intensityProfile, startOfMaxArea, currPeakPos_y) != PLUS_SUCCESS)
            {
              // unable to compute center-of-gravity; this scanline is invalid
              continue;
            }
            break;
          }
        case PEAK_POS_START:
          {
            /* Use peak start as peak-position metric*/
            if(FindPeakStart(intensityProfile,MaxFromLargestArea, startOfMaxArea, currPeakPos_y) != PLUS_SUCCESS)
            {
              // unable to compute peak start; this scanline is invalid
              continue;
            }
            break;
          }
        }

        itk::Point<double, 2> currPeakPos;
        currPeakPos[0] = static_cast<double>(startPixel[0]);
        currPeakPos[1] = startPixel[1]+currPeakPos_y;
        intensityPeakPositions.push_back(currPeakPos);
        ++numOfValidScanlines;

      } // end if() found intensity peak

    } // end currScanlineNum loop

    if(numOfValidScanlines < MINIMUM_NUMBER_OF_VALID_SCANLINES)
    {
      //TODO: drop the frame from the analysis
      LOG_DEBUG("Only " << numOfValidScanlines << " valid scanlines; this is less than the required " << MINIMUM_NUMBER_OF_VALID_SCANLINES << ". Skipping frame" << frameNumber);
    }

    LineParameters params;
    if( ComputeLineParameters(intensityPeakPositions, params) == PLUS_SUCCESS )
    {
      if( params.lineDirectionVector_Image[0] < MIN_X_SLOPE_COMPONENT_FOR_DETECTED_LINE)
      {
        // Line is close to vertical, skip frame because intersection of 
        // line with image's horizontal half point is unstable
        continue;
      }

      m_LineParameters.push_back(params);

      // Store the y-value of the line, when the line's x-value is half of the image's width
      double t = ( region.GetIndex()[0] + 0.5 * region.GetSize()[0] - params.lineOriginPoint_Image[0] ) / params.lineDirectionVector_Image[0]; 
      m_SignalValues.push_back( std::abs( params.lineOriginPoint_Image[1] + t * params.lineDirectionVector_Image[1] ) );

      //  Store timestamp for image frame
      m_SignalTimestamps.push_back(m_TrackedFrameList->GetTrackedFrame(frameNumber)->GetTimestamp());

      if(m_SaveIntermediateImages == true)
      {
        SaveIntermediateImage(frameNumber, scanlineImage, 
          params.lineOriginPoint_Image[0], params.lineOriginPoint_Image[1], params.lineDirectionVector_Image[0], params.lineDirectionVector_Image[1], 
          numOfValidScanlines, intensityPeakPositions);
      }
    }
    else
    {
      LOG_WARNING("Unable to compute line parameters for frame " << frameNumber << ".");
    }

  } // end frameNum loop

  bool plotVideoMetric = vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE;
  if (plotVideoMetric)
  {
    PlotDoubleArray(m_SignalValues);
  }

  return PLUS_SUCCESS;

} //  End LineDetection

//-----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::FindPeakStart(std::deque<int> &intensityProfile,int MaxFromLargestArea, int startOfMaxArea, double &startOfPeak)
{
  // Start of peak is defined as the location at which it reaches 50% of its maximum value.
  double startPeakValue = MaxFromLargestArea * 0.5;

  int pixelIndex = startOfMaxArea;

  while( intensityProfile.at(pixelIndex) <= startPeakValue)
  {
    ++pixelIndex;
  }

  startOfPeak = --pixelIndex;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::FindLargestPeak(std::deque<int> &intensityProfile,int &MaxFromLargestArea, int &MaxFromLargestAreaIndex, int &startOfMaxArea)
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

  if(intensityProfile.size() == 0 )
  {
    LOG_ERROR("Intensity contains no elements");
    return PLUS_FAIL;
  }

  double intensityMax = intensityProfile.at(0);
  for(unsigned int pixelLoc = 1; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if(intensityProfile.at(pixelLoc) > intensityMax)
    {
      intensityMax = intensityProfile.at(pixelLoc);
    }
  }

  double peakIntensityThreshold = intensityMax * INTESNITY_THRESHOLD_PERCENTAGE_OF_PEAK; 

  for(int pixelLoc = 0; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if(intensityProfile.at(pixelLoc) > peakIntensityThreshold  && !underPeak)
    {
      // reached start of the peak
      underPeak = true;
      currentMax = intensityProfile.at(pixelLoc);
      currentMaxIndex = pixelLoc;
      currentArea = intensityProfile.at(pixelLoc);
      currentStart = pixelLoc;
    }
    else if(intensityProfile.at(pixelLoc) > peakIntensityThreshold  && underPeak)
    {
      // still under the the peak, cumulate the area
      currentArea += intensityProfile.at(pixelLoc);

      if(intensityProfile.at(pixelLoc) > currentMax)
      {
        currentMax = intensityProfile.at(pixelLoc);
        currentMaxIndex = pixelLoc;
      }
    }
    else if(intensityProfile.at(pixelLoc) < peakIntensityThreshold && underPeak)
    {
      // exited the peak area
      underPeak = false;
      if(currentArea > currentLargestArea)
      {
        currentLargestArea = currentArea;
        currentMaxFromLargestArea = currentMax;
        currentMaxFromLargestAreaIndex = currentMaxIndex;
        currentStartOfMaxArea = currentStart;
      }
    }
  } //end loop through intensity profile

  MaxFromLargestArea = currentMaxFromLargestArea;
  MaxFromLargestAreaIndex = currentMaxFromLargestAreaIndex;
  startOfMaxArea = currentStartOfMaxArea;

  if(currentLargestArea == 0)
  {
    // No peak for this scanline
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------

PlusStatus vtkLineSegmentationAlgo::ComputeCenterOfGravity(std::deque<int> &intensityProfile, int startOfMaxArea, double &centerOfGravity)
{
  if(intensityProfile.size() == 0)
  {
    return PLUS_FAIL;
  }

  double intensityMax = intensityProfile.at(0);
  for(unsigned int pixelLoc = 1; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if(intensityProfile.at(pixelLoc) > intensityMax)
    {
      intensityMax = intensityProfile.at(pixelLoc);
    }
  }

  double peakIntensityThreshold = intensityMax * INTESNITY_THRESHOLD_PERCENTAGE_OF_PEAK; 

  int pixelLoc = startOfMaxArea;
  int pointsInPeak = 0;
  double intensitySum = 0;
  while(intensityProfile.at(pixelLoc) > peakIntensityThreshold)
  {
    intensitySum += pixelLoc * intensityProfile.at(pixelLoc);
    pointsInPeak += intensityProfile.at(pixelLoc);
    ++pixelLoc;
  }

  if(pointsInPeak == 0)
  {
    return PLUS_FAIL;
  }

  centerOfGravity = intensitySum / pointsInPeak;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::ComputeLineParameters(std::vector<itk::Point<double,2> >& data, LineParameters& OutputParameters)
{
  std::vector<double> ransacParameterResult;
  typedef itk::PlaneParametersEstimator<DIMENSION> PlaneEstimatorType;
  typedef itk::RANSAC<itk::Point<double, DIMENSION>, double> RANSACType;

  //create and initialize the parameter estimator
  double maximalDistanceFromPlane = 0.5;
  PlaneEstimatorType::Pointer planeEstimator = PlaneEstimatorType::New();
  planeEstimator->SetDelta( maximalDistanceFromPlane );
  planeEstimator->LeastSquaresEstimate( data, ransacParameterResult );
  if( ransacParameterResult.empty() )
  {
    LOG_ERROR("Unable to fit line through points with least squares estimation");
  }
  else
  {
    LOG_TRACE("Least squares line parameters (n, a):");
    for( unsigned int i=0; i<(2*DIMENSION-1); i++ )
    {
      LOG_TRACE(" LS parameter: " << ransacParameterResult[i]);
    }      
  }

  //create and initialize the RANSAC algorithm
  double desiredProbabilityForNoOutliers = 0.999;
  RANSACType::Pointer ransacEstimator = RANSACType::New();  

  try
  {
    ransacEstimator->SetData( data );
  }
  catch( std::exception& e) 
  {
    LOG_ERROR(e.what());
    return PLUS_FAIL;
  }

  try
  {
    ransacEstimator->SetParametersEstimator( planeEstimator.GetPointer() );
  }
  catch( std::exception& e)
  {
    LOG_ERROR(e.what());
    return PLUS_FAIL;
  }


  try
  {
    ransacEstimator->Compute( ransacParameterResult, desiredProbabilityForNoOutliers );
  }
  catch( std::exception& e)
  {
    LOG_ERROR(e.what());
    return PLUS_FAIL;
  }

  if( ransacParameterResult.empty() )
  {
    LOG_ERROR("Unable to fit line through points with RANSAC, line segmentation failed");
    return PLUS_FAIL;
  }

  LOG_TRACE("RANSAC line fitting parameters (n, a):");

  for( unsigned int i=0; i<(2*DIMENSION-1); i++ )
  {
    LOG_TRACE(" RANSAC parameter: " << ransacParameterResult[i]);
  }

  OutputParameters.lineDirectionVector_Image[0] = -ransacParameterResult[1];
  OutputParameters.lineDirectionVector_Image[1] = ransacParameterResult[0];
  OutputParameters.lineOriginPoint_Image[0] = ransacParameterResult[2];
  OutputParameters.lineOriginPoint_Image[1] = ransacParameterResult[3];

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::SaveIntermediateImage(int frameNumber, CharImageType::Pointer scanlineImage, double x_0, double y_0, double r_x, double r_y, int numOfValidScanlines, const std::vector<itk::Point<double,2> > &intensityPeakPositions)
{
  // Write image showing the scan lines to file
  std::ostrstream scanLineImageFilename;
  scanLineImageFilename << m_IntermediateFilesOutputDirectory << "/scanLineImage" << std::setw(3) << std::setfill('0') << frameNumber << ".bmp" << std::ends;
  LOG_DEBUG("Save line segmentation intermediate image to "<<scanLineImageFilename.str());
  PlusVideoFrame::SaveImageToFile(scanlineImage, scanLineImageFilename.str());

  // Test writing of colour image to file
  typedef itk::RGBPixel<unsigned char> rgbPixelType;
  typedef itk::Image<rgbPixelType, 2> rgbImageType;
  rgbImageType::Pointer rgbImageCopy = rgbImageType::New();

  CharImageType::RegionType fullImageRegion = scanlineImage->GetLargestPossibleRegion();

  rgbImageType::IndexType start;
  start[0] =   fullImageRegion.GetIndex()[0];  // first index on X
  start[1] =   fullImageRegion.GetIndex()[0];  // first index on Y
  rgbImageType::SizeType  size;
  size[0]  = fullImageRegion.GetSize()[0];  // size along X
  size[1]  = fullImageRegion.GetSize()[1];  // size along Y
  rgbImageType::RegionType region;
  region.SetIndex( start );
  region.SetSize( size );
  rgbImageCopy->SetRegions( region );
  rgbImageCopy->Allocate();

  // Copy grayscale image to an RGB image to allow drawing of annotations in color
  for(unsigned int x_coord = region.GetIndex()[0]; x_coord  < region.GetSize()[0]; ++x_coord)
  {
    for(unsigned int y_coord = region.GetIndex()[1]; y_coord < region.GetSize()[1]; ++y_coord)
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

  float diag = vcl_sqrt((float)( size[0]*size[0] + size[1]*size[1] ));

  // Draw detected line
  for(int i = static_cast<int>(-diag); i < static_cast<int>(diag); ++i)
  {
    rgbImageType::IndexType currIndex;
    currIndex[0]=static_cast<int>(x_0 + i * r_x);
    currIndex[1]=static_cast<int>(y_0 + i * r_y);

    if(fullImageRegion.IsInside(currIndex))
    {
      rgbPixelType currRgbImagePixelVal;
      currRgbImagePixelVal.Set(0, 0, 255);
      rgbImageCopy->SetPixel(currIndex, currRgbImagePixelVal);
    }
  }

  // Draw intensity peaks (as squares)
  for(int scanLineIndex = 0; scanLineIndex < numOfValidScanlines; ++scanLineIndex)
  {
    unsigned int sqareCenterCoordX = static_cast<unsigned int>(intensityPeakPositions.at(scanLineIndex).GetElement(0));
    unsigned int sqareCenterCoordY = static_cast<unsigned int>(intensityPeakPositions.at(scanLineIndex).GetElement(1));

    for(unsigned int x = sqareCenterCoordX - 3; x < sqareCenterCoordX + 3; ++x)
    {
      for(unsigned int y = sqareCenterCoordY - 3; y < sqareCenterCoordY + 3; ++y)
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

  std::ostrstream rgbImageFilename;
  rgbImageFilename << m_IntermediateFilesOutputDirectory << "/rgbImage" << std::setw(3) << std::setfill('0') << frameNumber << ".png" << std::ends;

  typedef itk::ImageFileWriter<rgbImageType> rgbImageWriterType;
  rgbImageWriterType::Pointer rgbImageWriter = rgbImageWriterType::New();

  rgbImageWriter->SetFileName(rgbImageFilename.str());
  rgbImageWriter->SetInput(rgbImageCopy);
  rgbImageWriter->Update();
}

//----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::Update()
{
  if( VerifyVideoInput() != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }
  if( ComputeVideoPositionMetric() != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::PlotIntArray(const std::deque<int> &intensityValues)
{
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
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);

#if VTK_MAJOR_VERSION <= 5
  line->SetInput(table, 0, 1);
#else
  line->SetInputData(table, 0, 1);
#endif

  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::PlotDoubleArray(const std::deque<double> &intensityValues)
{
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
  for (int i = 0; i < intensityValues.size(); ++i)
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
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);

#if VTK_MAJOR_VERSION <= 5
  line->SetInput(table, 0, 1);
#else
  line->SetInputData(table, 0, 1);
#endif

  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::GetDetectedTimestamps(std::deque<double> &timestamps)
{
  timestamps = m_SignalTimestamps;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::GetDetectedPositions(std::deque<double> &positions)
{
  positions = m_SignalValues;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::GetDetectedLineParameters(std::vector<LineParameters>& parameters)
{
  parameters = m_LineParameters;
}

//-----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::SetClipRectangle(int clipRectangleOriginPix[2], int clipRectangleSizePix[2])
{
  m_ClipRectangleOrigin[0] = clipRectangleOriginPix[0];
  m_ClipRectangleOrigin[1] = clipRectangleOriginPix[1];
  m_ClipRectangleSize[0] = clipRectangleSizePix[0];
  m_ClipRectangleSize[1] = clipRectangleSizePix[1];
}

//----------------------------------------------------------------------------
void vtkLineSegmentationAlgo::LimitToClipRegion(CharImageType::RegionType& region)
{
  if( (m_ClipRectangleSize[0] <= 0) || (m_ClipRectangleSize[1] <= 0) )
  {
    // no clipping
    return;
  }

  // Clipping enabled
  int clipRectangleOrigin[2]={m_ClipRectangleOrigin[0],m_ClipRectangleOrigin[1]};
  int clipRectangleSize[2]={m_ClipRectangleSize[0],m_ClipRectangleSize[1]};

  // Adjust clipping region origin and size to fit inside the frame region
  CharImageType::IndexType imageOrigin=region.GetIndex();
  CharImageType::SizeType imageSize=region.GetSize();
  if (clipRectangleOrigin[0]<imageOrigin[0] || clipRectangleOrigin[1]<imageOrigin[1]
  || clipRectangleOrigin[0]>=imageOrigin[0]+imageSize[0] || clipRectangleOrigin[1]>=imageOrigin[1]+imageSize[1])
  {
    LOG_WARNING("ClipRectangleOrigin is invalid ("<<clipRectangleOrigin[0]<<", "<<clipRectangleOrigin[1]<<"). The frame size is "
      <<imageSize[0]<<"x"<<imageSize[1]<<". Using ("<<imageOrigin[0]<<","<<imageOrigin<<") as ClipRectangleOrigin.");
    clipRectangleOrigin[0]=0;
    clipRectangleOrigin[1]=0;
  }
  if (clipRectangleOrigin[0]+clipRectangleSize[0] >= imageOrigin[0]+imageSize[0])
  {
    // rectangle size is out of the framSize bounds, clip it to the available size
    clipRectangleSize[0]=imageOrigin[0]+imageSize[0]-clipRectangleOrigin[0];
    LOG_WARNING("Adjusting ClipRectangleSize x to "<<clipRectangleSize[0]);
  }
  if (clipRectangleOrigin[1]+clipRectangleSize[1] > imageSize[1])
  {
    // rectangle size is out of the framSize bounds, clip it to the available size
    clipRectangleSize[1]=imageOrigin[1]+imageSize[1]-clipRectangleOrigin[1];
    LOG_WARNING("Adjusting ClipRectangleSize y to "<<clipRectangleSize[1]);    
  }    

  if( (clipRectangleSize[0] <= 0) || (clipRectangleSize[1] <= 0) )
  {
    // after the adjustment it seems that there is no clipping is needed
    return;
  }

  // Save updated clipping parameters to the region    
  imageOrigin[0]=clipRectangleOrigin[0];
  imageOrigin[1]=clipRectangleOrigin[1];
  imageSize[0]=clipRectangleSize[0];
  imageSize[1]=clipRectangleSize[1];    
  region.SetIndex(imageOrigin);
  region.SetSize(imageSize);
}

//----------------------------------------------------------------------------
PlusStatus vtkLineSegmentationAlgo::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  if( aConfig == NULL )
  {
    LOG_ERROR("Null configuration sent to vtkLineSegmentationAlgo::ReadConfiguration.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* lineSegmentationElement = aConfig->FindNestedElementWithName("vtkLineSegmentationAlgo");
  if( lineSegmentationElement == NULL )
  {
    LOG_ERROR("Unable to find \'vtkLineSegmentationAlgo\' element. Cannot proceed with segmentation.");
    return PLUS_FAIL;
  }

  if ( !lineSegmentationElement->GetVectorAttribute("ClipRectangleOrigin", 2, m_ClipRectangleOrigin) || 
    !lineSegmentationElement->GetVectorAttribute("ClipRectangleSize", 2, m_ClipRectangleSize) )
  {
    LOG_WARNING("Cannot find ClipRectangleOrigin or ClipRectangleSize attribute in the vtkLineSegmentationAlgo configuration file; Using the largest ROI possible.");
    m_ClipRectangleOrigin[0] = -1;
    m_ClipRectangleOrigin[1] = -1;
    m_ClipRectangleSize[0] = -1;
    m_ClipRectangleSize[1] = -1;
  }

  const char* saveIntermediateImages = lineSegmentationElement->GetAttribute("SaveIntermediateImages");
  if( saveIntermediateImages != NULL && STRCASECMP(saveIntermediateImages, "TRUE") == 0 )
  {
    m_SaveIntermediateImages = true;
  }

  double signalTimeRange[2] = {0};
  if( lineSegmentationElement->GetVectorAttribute("SignalTimeRange", 2, signalTimeRange) )
  {
    m_SignalTimeRangeMin = signalTimeRange[0];
    m_SignalTimeRangeMax = signalTimeRange[1];
  }

  const char* intermediateOutputDirectory = lineSegmentationElement->GetAttribute("IntermediateFilesOutputDirectory");
  if( intermediateOutputDirectory != NULL )
  {
    m_IntermediateFilesOutputDirectory = std::string(intermediateOutputDirectory);
  }
  else
  {
    m_IntermediateFilesOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory();
  }

  return PLUS_SUCCESS;
}
