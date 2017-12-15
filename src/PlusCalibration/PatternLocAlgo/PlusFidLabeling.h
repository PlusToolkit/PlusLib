/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_LABELLING_H
#define _FIDUCIAL_LABELLING_H

#include "PlusFidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
class vtkXMLDataElement;

/*!
\class LabelingResults
\brief This class contains the result of the labeling algorithm. It contains the x and y coordinates of
a dot as well as its wire and pattern id to allow identification of the dot.
\ingroup PlusLibPatternRecognition
*/
class vtkPlusCalibrationExport PlusLabelingResults
{
public:
  int patternId;//Id of the pattern
  int wireId;//Id of the wire in the pattern
  double x, y; //coordinate in the image plane
};

//-----------------------------------------------------------------------------

/*!
\class FidLabeling
\brief From a list of n-points lines, identifies the relationship between the lines and recognizes
patterns defined in the configuration file. It also labels the dots found.
\ingroup PlusLibPatternRecognition
*/
class vtkPlusCalibrationExport PlusFidLabeling
{
public:
  PlusFidLabeling();
  virtual ~PlusFidLabeling();

  /*! Update the parameters and computes the distance between 2 lines from the phantom definition file */
  void UpdateParameters();

  /*! Clear the member attributes when not needed anymore */
  void Clear();

  /*! Read the configuration file from a vtk XML data element */
  PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement, double minThetaRad, double maxThetaRad);

  /*! Set the size of the frame as an array */
  void SetFrameSize(const FrameSizeType& frameSize);

  /*! Compute the shortest distance from a point: dot, to a line: line */
  double ComputeDistancePointLine(PlusFidDot& dot, PlusFidLine& line);

  /*! Compute the shift between the middle of line1 and line2 */
  double ComputeShift(PlusFidLine& line1, PlusFidLine& line2);

  /*! Compute the slope of the line relative to the x-axis */
  double ComputeSlope(PlusFidLine& line);

  /*! Find the patterns defined by the configuration file */
  void FindPattern();

  /*! Update the CIRS phantom model 45 results once the pattern has been found, the order of the lines is:
  resultLine1: left-most, resultLine2: diagonal, resultLine3: right-most*/
  void UpdateCirsResults(const PlusFidLine& resultLine1, const PlusFidLine& resultLine2, const PlusFidLine& resultLine3);

  /*! Update the NWires results once the pattern has been found
  \param resultLines Found lines in ascending order of their StartPoint's Y coordinate (top line is first, bottom line is last)
  */
  void UpdateNWiresResults(std::vector<PlusFidLine*>& resultLines);

  /*! Sort the points of a line from right to left */
  void SortRightToLeft(PlusFidLine& line);

  /*! Sort the points of a line, used for sorting the points by distance from StartPoint */
  static bool SortCompare(const std::vector<double>& temporaryLine1, const std::vector<double>& temporaryLine2);

  /*! Sort points of a line by their distance from the start point of the line */
  PlusFidLine SortPointsByDistanceFromStartPoint(PlusFidLine& fiducials);

  //Accessors and mutators
  /*! Get the vector of dots found by FidSegmentation */
  std::vector<PlusFidDot>& GetDotsVector() {return m_DotsVector; };

  /*! Set the vector of dots found by FidSegmentation */
  void SetDotsVector(std::vector<PlusFidDot>& value) { m_DotsVector = value; };

  /*! Get the vector of the identified lines */
  std::vector<PlusFidLine>& GetFoundLinesVector() {return m_FoundLines; };

  /*! Set the vector of lines found by FidLineFinder */
  void SetLinesVector(std::vector< std::vector<PlusFidLine> >& value) { m_LinesVector = value; };

  /*! Get the pattern structure vector, this defines the patterns that the algorithm finds */
  std::vector<PlusFidPattern*>& GetPatterns();

  /*! Set the pattern structure vector, this defines the patterns that the algorithm finds */
  void SetPatterns(const std::vector<PlusFidPattern*>& value) { m_Patterns = value; };

  /*! Get the intensity of a pair of lines */
  double GetPatternIntensity() { return m_PatternIntensity; };

  /*! Set to true if the algorithm is successful and the correct dots are found, false otherwise */
  void SetDotsFound(bool value) { m_DotsFound = value; };

  /*! Get a boolean value that is true if the algorithm is successful and the correct dots are found, false otherwise */
  bool GetDotsFound() { return m_DotsFound; };

  /*! Get the coordinates of the found dots */
  std::vector<std::vector<double>> GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };

  /*! Get the vector of lines found by FidLineFinder */
  std::vector<std::vector<PlusFidLine>>  GetLinesVector() { return m_LinesVector; };

  /*! Set the approximate spacing in Mm per pixel */
  void SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };

  /*! Set the tolerance on the maximum distance between 2 lines, in percent */
  void SetMaxLinePairDistanceErrorPercent(double value) { m_MaxLinePairDistanceErrorPercent = value; };

  /*! Set the maximum angle allowed between two lines, in degree */
  void SetMaxAngleDifferenceDegrees(double value) { m_MaxAngleDiff = value; };

  /*! Set the minimum angle allowed for a line, in degrees */
  void SetMinThetaDeg(double value);

  /*! Set the maximum angle allowed for a line, in degrees */
  void SetMaxThetaDeg(double value);

  /*! Set the angle tolerance on the angle between two lines, in degrees */
  void SetAngleToleranceDeg(double value);

  /*! Set the maximum line shift, in mm */
  void SetMaxLineShiftMm(double aValue);

  /*! Get the maximum line shift, in mm */
  double GetMaxLineShiftMm();

  void SetAngleToleranceDegrees(double angleToleranceDegrees);

  void SetInclinedLineAngleDegrees(double inclinedLineAngleDegrees);

protected:
  std::array<unsigned int, 3> m_FrameSize;

  double m_ApproximateSpacingMmPerPixel;
  double m_MaxAngleDiff; // not used
  double m_MinLinePairDistMm;   // minimum distance between any two lines
  double m_MaxLinePairDistMm;  // maximum distance between any two lines
  double m_MinLinePairAngleRad; // minimum angle between any two lines
  double m_MaxLinePairAngleRad; // maximum angle between any two lines
  double m_MaxLineShiftMm; // maximum in-plane shift of the midpoint of the N fiducials
  double m_MaxLinePairDistanceErrorPercent;
  double m_MinThetaRad;
  double m_MaxThetaRad;

  bool m_DotsFound;

  double m_AngleToleranceRad;
  double m_InclinedLineAngleRad;
  double m_PatternIntensity;

  std::vector<PlusFidDot> m_DotsVector;
  std::vector<PlusFidLine> m_FoundLines;
  std::vector<PlusFidPattern*> m_Patterns;
  std::vector<PlusLabelingResults> m_Results;
  std::vector< std::vector<PlusFidLine> > m_LinesVector;
  std::vector< std::vector<double> > m_FoundDotsCoordinateValue;
};

#endif // _FIDUCIAL_LABELLING_H
