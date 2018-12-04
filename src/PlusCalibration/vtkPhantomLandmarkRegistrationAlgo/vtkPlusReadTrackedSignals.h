/*!=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusReadTrackedSignals_h
#define __vtkPlusReadTrackedSignals_h

#include "vtkPlusCalibrationExport.h"

#include <deque>
#include "vtkObject.h"

//class vtkIGSIOTrackedFrameList;

/*!
  \class vtkPlusReadTrackedSignals
  \brief Extract the motion component along the the principal axis of the motion. Used for computing a position metric from a periodically moving tool.
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusCalibrationExport vtkPlusReadTrackedSignals : public vtkObject
{
public:
  static vtkPlusReadTrackedSignals* New();
  vtkTypeMacro(vtkPlusReadTrackedSignals, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Sets the input US video frames */
  void SetTrackerFrames(vtkIGSIOTrackedFrameList* trackerFrames);

  /*! Sets the time range where the signal will be extracted from. If rangeMax<rangeMin then all the input frames will be used to genereate the signal. */
  void SetSignalTimeRange(double rangeMin, double rangeMax);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */
  void SetProbeToReferenceTransformName(const std::string& probeToReferenceTransformName);

  /*!
    Run the line detection algorithm on the input video frames
    \param errorDetail if the algorithm fails then the details of the problem are returned in this string
  */
  PlusStatus Update();

  /*! Get the timestamps of the frames where a line was successfully */
  void GetTimestamps(std::deque<double>& timestamps);

  void GetSignalStylusRef(std::deque<double>& signalComponent);
  void GetSignalStylusTipRef(std::deque<double>& signalComponent);
  void GetSignalZ(std::deque<double>& signalComponent);
  void GetSignalStylusTipSpeed(std::deque<double>& signalComponent);

  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);
  vtkGetStringMacro(ObjectMarkerCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(ObjectPivotPointCoordinateFrame);

protected:
  vtkPlusReadTrackedSignals();
  virtual ~vtkPlusReadTrackedSignals();

  vtkSetStringMacro(ObjectMarkerCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ObjectPivotPointCoordinateFrame);

  PlusStatus VerifyInputFrames();
  PlusStatus ComputeTrackerPositionMetric();

  vtkIGSIOTrackedFrameList* m_TrackerFrames;

  /*! Name of the object marker coordinate frame (eg. Stylus) */
  char*               ObjectMarkerCoordinateFrame;
  /*! Name of the reference coordinate frame (eg. Reference) */
  char*               ReferenceCoordinateFrame;
  /*! Name of the object pivot point coordinate frame (eg. StylusTip) */
  char*               ObjectPivotPointCoordinateFrame;

  vtkSmartPointer<vtkMatrix4x4> StylusTipToStylusTransform;

  std::deque<double> m_SignalTimestamps;
  std::deque<double> m_SignalStylusRef;
  std::deque<double> m_SignalStylusTipRef;
  std::deque<double> m_SignalStylusTipSpeed;

  double m_SignalTimeRangeMin;
  double m_SignalTimeRangeMax;

private:
  vtkPlusReadTrackedSignals(const vtkPlusReadTrackedSignals&);
  void operator=(const vtkPlusReadTrackedSignals&);
};

#endif //  __vtkPlusReadTrackedSignals_h 
