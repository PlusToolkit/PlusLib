/*!=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkLineSegmentationAlgo_h
#define __vtkLineSegmentationAlgo_h

#include "vtkObject.h"
#include <deque>

class vtkTrackedFrameList;

/*!
  \class vtkLineSegmentationAlgo
  \brief Detect the position of a line (image of a plane) in an US image sequence.
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkLineSegmentationAlgo: public vtkObject
{
public:
  typedef unsigned char CharPixelType; // The natural type of the input image
  typedef itk::Image<CharPixelType,2/*image dimension*/> CharImageType;

  static vtkLineSegmentationAlgo* New();
  vtkTypeRevisionMacro(vtkLineSegmentationAlgo, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 
 
  /*! Sets the input US video frames */  
  void SetVideoFrames(vtkTrackedFrameList* videoFrames);

  /*! Sets the time range where the signal will be extracted from. If rangeMax<rangeMin then all the input frames will be used to genereate the signal. */
  void SetSignalTimeRange(double rangeMin, double rangeMax);

  /*! Sets a rectanguler region of interest. The algorithm will ignore everything outside the specified image region. If the rectangle size is (0,0) then no clipping is performed. */
  void SetClipRectangle(int clipRectangleOriginPix[2], int clipRectangleSizePix[2]);

  /*!
    Run the line detection algorithm on the input video frames
    \param errorDetail if the algorithm fails then the details of the problem are returned in this string
  */
  PlusStatus Update(); 

  /*! Get the timestamps of the frames where a line was successfully detected */
  void GetDetectedTimestamps(std::deque<double> &timestamps);

  /*! Get the line positions on the frames where a line was successfully detected */
  void GetDetectedPositions(std::deque<double> &positions); 

  /*! Enable/disable saving of intermediate images for debugging */
  void SetSaveIntermediateImages(bool saveIntermediateImages);
  
  void SetIntermediateFilesOutputDirectory(const std::string &outputDirectory);

protected:
  vtkLineSegmentationAlgo();
  virtual ~vtkLineSegmentationAlgo();

  PlusStatus VerifyVideoInput();

  PlusStatus ComputeVideoPositionMetric();

  PlusStatus FindPeakStart(std::deque<int> &intensityProfile,int MaxFromLargestArea, int startOfMaxArea, double &startOfPeak);

  PlusStatus FindLargestPeak(std::deque<int> &intensityProfile,int &MaxFromLargestArea, int &MaxFromLargestAreaIndex, int &startOfMaxArea);

  PlusStatus ComputeCenterOfGravity(std::deque<int> &intensityProfile, int startOfMaxArea, double &centerOfGravity);

  PlusStatus ComputeLineParameters(std::vector<itk::Point<double,2> > &data, std::vector<double> &planeParameters);

  void PlotIntArray(const std::deque<int> &intensityValues);

  void PlotDoubleArray(const std::deque<double> &intensityValues);

  void SaveIntermediateImage(int frameNumber, CharImageType::Pointer scanlineImage, double x_0, double y_0, double r_x, double r_y, int numOfValidScanlines, const std::vector<itk::Point<double,2> > &intensityPeakPositions);  

  /*! Update passed region to fit within the frame size. */
  void LimitToClipRegion(CharImageType::RegionType& region);

  ///

  vtkSmartPointer<vtkTrackedFrameList> m_VideoFrames; 

  std::deque<double> m_SignalValues; 
  std::deque<double> m_SignalTimestamps; 

  /*! If "true" then images of intermediate steps (i.e. scanlines used, detected lines) are saved in local directory */
  bool m_SaveIntermediateImages;

  /*! Directory where the intermediate files are written to */
  std::string m_IntermediateFilesOutputDirectory;

  double m_SignalTimeRangeMin;
  double m_SignalTimeRangeMax;

  /*! Clip rectangle origin for the processing (in pixels). Everything outside the rectangle is ignored. */
  int m_ClipRectangleOrigin[2];

  /*! Clip rectangle origin for the processing (in pixels). Everything outside the rectangle is ignored. */
  int m_ClipRectangleSize[2]; 

private:
  vtkLineSegmentationAlgo(const vtkLineSegmentationAlgo&);
  void operator=(const vtkLineSegmentationAlgo&);
};

#endif //  __vtkLineSegmentationAlgo_h 
