/*!=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPrincipalMotionDetectionAlgo_h
#define __vtkPrincipalMotionDetectionAlgo_h

#include <deque>
#include "vtkObject.h"

class vtkTrackedFrameList;

/*!
  \class vtkPrincipalMotionDetectionAlgo
  \brief Extract the motion component along the the principal axis of the motion. Used for computing a position metric from a periodically moving tool.
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPrincipalMotionDetectionAlgo: public vtkObject
{
public:
  static vtkPrincipalMotionDetectionAlgo* New();
  vtkTypeRevisionMacro(vtkPrincipalMotionDetectionAlgo, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 
 
  /*! Sets the input US video frames */  
  void SetTrackerFrames(vtkTrackedFrameList* trackerFrames);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */  
  void SetProbeToReferenceTransformName(const std::string& probeToReferenceTransformName);

  /*!
    Run the line detection algorithm on the input video frames
    \param errorDetail if the algorithm fails then the details of the problem are returned in this string
  */
  PlusStatus Update(); 

  /*! Get the timestamps of the frames where a line was successfully detected*/
  void GetDetectedTimestamps(std::deque<double> &timestamps);

  /*! Get the line positions on the frames where a line was successfully detected*/
  void GetDetectedPositions(std::deque<double> &positions); 

  void ComputePrincipalAxis(std::deque<itk::Point<double, 3> > &trackerPositions, itk::Point<double,3> &principalAxisOfMotion, int numValidFrames);

protected:
  vtkPrincipalMotionDetectionAlgo();
  virtual ~vtkPrincipalMotionDetectionAlgo();

  PlusStatus VerifyInputFrames();
  PlusStatus ComputeTrackerPositionMetric();

  vtkTrackedFrameList* m_TrackerFrames;
  std::string m_ProbeToReferenceTransformName;

  std::deque<double> m_SignalValues; 
  std::deque<double> m_SignalTimestamps; 

private:
  vtkPrincipalMotionDetectionAlgo(const vtkPrincipalMotionDetectionAlgo&);
  void operator=(const vtkPrincipalMotionDetectionAlgo&);
};

#endif //  __vtkPrincipalMotionDetectionAlgo_h 
