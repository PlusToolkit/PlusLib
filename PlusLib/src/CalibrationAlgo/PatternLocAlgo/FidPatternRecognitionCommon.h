/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_ALGORITHM_COMMON_H
#define _FIDUCIAL_ALGORITHM_COMMON_H

#include "vtkCalibrationAlgoExport.h"

#include <vector>
#include <string>

/*!
\class FidDot
\brief This class defines a single dot made obtained from the segmentation part of the algorithm.
It contains the X and Y coordinate of the dot as well as its intensity. Also contains an operator 
to check if dots are equal (both X and Y are the same).
\ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport FidDot
{
public:
  /*! Compare the intensity of 2 dots */
  static bool IntensityLessThan( const FidDot &dot1, const FidDot &dot2 );

  /*! Compare the position of 2 dots */
  static bool PositionLessThan( std::vector<FidDot>::iterator b1, std::vector<FidDot>::iterator b2 );

  /*! Set the x coordinate of the dot */
  void SetX(double value) { m_X = value; };

  /*! Get the x coordinate of the dot */
  double GetX() const { return m_X; };

  /*! Set the y coordinate of the dot */
  void  SetY(double value) { m_Y = value; };

  /*! Get the y coordinate of the dot */
  double GetY() const { return m_Y; };

  /*! Set the intensity of the dot */
  void SetDotIntensity(double value) { m_DotIntensity = value; };

  /*! Get the intensity of the dot */
  double GetDotIntensity() const { return m_DotIntensity; };

  /*! Get Euclidean distance from another point */
  double GetDistanceFrom(FidDot &d);

  /*! Compare two dots, coordinate-wise */
  bool operator== (const FidDot& data) const { return (m_X == data.m_X && m_Y == data.m_Y) ; }

protected:
  double  m_X;
  double  m_Y;
  double  m_DotIntensity;
};

//-----------------------------------------------------------------------------
/*!
\class FidLine
\brief This class defines a single line made from a vector of dots obtained from the segmentation part of the algorithm.
It contains the list of indexes of the dots, the length of the line, the origin and the endpoint as well as the
line intensity and its direction vector.
\ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport FidLine
{
public:
  /*! Compare the intensity of 2 lines */
  static bool lessThan( const FidLine& line1, const FidLine& line2 );

  /*! Compare 2 lines to know if they are the same */
  static bool compareLines( const FidLine& line1, const FidLine& line2 );

  /*! Compute the angle in radians between the line and the positive x-axis, value between -Pi and +Pi */
  static double ComputeAngleRad(const FidLine& line1);

  /*! Compute the angle difference in radians between two lines, the result value is between 0 and Pi/2 */
  static double ComputeAngleRad( const FidLine& line1, const FidLine& line2 );

  /*! Set the point of line
  \param aIndex is the index in the dots vector of the line
  \param aValue is the index of the dot in the general vector of dots
  */
  void SetPoint(int aIndex, int aValue) { m_Points[aIndex] = aValue; };

  /*! Get the point of the line that has index aIndex */
  int  GetPoint(int aIndex) const { return m_Points[aIndex]; };

  /*! Get the vector of dots making the line */
  const std::vector<int>&  GetPoints() const { return m_Points; };

  /*! Set the intensity of the line, which is the sum of its dots intensity */
  void SetIntensity(double value) { m_Intensity = value; };

  /*! Get the intensity of the line, which is the sum of its dots intensity */
  double GetIntensity() const { return m_Intensity; };

  /*! Set the length of the line */
  void SetLength(double value) { m_Length = value; };

  /*! Get the length of the line */
  double GetLength() const { return m_Length; };

  /*! Set the direction vector that defines the line */
  void SetDirectionVector(int aIndex, double aValue) { m_DirectionVector[aIndex] = aValue; };

  /*! Get the direction vector that defines the line */
  const double*  GetDirectionVector() const { return m_DirectionVector; };

  /*! Set the start point index of the line. It is an index in the m_DotsVector */
  void  SetStartPointIndex(int index) { m_StartPointIndex = index; };

  /*! Get the start point index of the line. It is an index in the m_DotsVector */
  int GetStartPointIndex() const { return m_StartPointIndex; };

  /*! Set the other end point of the line. It is the index of the end point in the m_DotsVector */
  void SetEndPointIndex(int index) { m_EndPointIndex = index; };

  /*! Get the other end point of the line. It is the index of the end point in the m_DotsVector */
  int GetEndPointIndex() const { return m_EndPointIndex; };

  /*! Resize the points vector */
  void ResizePoints( int aNewSize ) { m_Points.resize(aNewSize); }

  /*! Add a point to the line */
  void AddPoint( int aPoint ) { m_Points.push_back(aPoint); }

protected:
  std::vector<int>  m_Points; // indices of points that make up the line
  double            m_Intensity;
  double            m_Length;
  double            m_DirectionVector[2];
  int               m_StartPointIndex;//index of startpoint of the line, all the other line points are towards the positive m_DirectionVector direction from this point
  int               m_EndPointIndex;//Index of the endpoint of the line
};

//-----------------------------------------------------------------------------
/*!
\class FidWire
\brief This structure defines a single fiducial wire by its name and its endpoint (front and back)
\ingroup PlusLibPatternRecognition
*/
struct vtkCalibrationAlgoExport FidWire
{
  std::string Name;
  double EndPointFront[3];
  double EndPointBack[3];
};

//-----------------------------------------------------------------------------
/*!
\class FidPattern
\brief This class stores the different Patterns defined in the configuration file. It contains the wires
defintion, the distance from the line origin of each expected "dot" and the tolerances on these
distances.
\ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport FidPattern
{
public:
  virtual ~FidPattern() { };

  std::vector<FidWire> Wires;
  std::vector<double> DistanceToOriginMm; //These distances are in mm.
  std::vector<double> DistanceToOriginToleranceMm; //These tolerances are in mm.
};

//-----------------------------------------------------------------------------
/*!
\class NWire
\brief The struct NWire is a child from FidPattern and has two more features that are the intersections of the NWires
between lines 1 and 2, and, 2 and 3.
\ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport NWire : public FidPattern
{
public:
  virtual ~NWire() { };

  double  IntersectPosW12[3];
  double  IntersectPosW32[3];
};

//-----------------------------------------------------------------------------
/*!
\class CoplanarParallelWires
\brief The struct CoplanarparallelWires is a child from FidPattern and represents an undefinite number of coplanar 
parallel fiducial wires.
\ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport CoplanarParallelWires : public FidPattern
{
public:
  virtual ~CoplanarParallelWires() { };
};

//-----------------------------------------------------------------------------
/*!
\class PatternRecognitionResult
\brief This class stores the FidPattern Recognition algorithm results.
\ingroup PlusLibPatternRecognition
*/
class vtkCalibrationAlgoExport PatternRecognitionResult
{
public:
  PatternRecognitionResult();
  virtual ~PatternRecognitionResult();

  /*! Clear the classe attributes once they are not needed anymore */
  void Clear();

  /*! Set the m_DotsFound to true if the algorithm found the corresponding dots, to false otherwise */
  void SetDotsFound(bool value) { m_DotsFound = value; };

  /*! Get m_DotsFound */
  bool GetDotsFound() { return m_DotsFound; };

  /*! Set the coordinates of the found dots */
  void SetFoundDotsCoordinateValue(std::vector< std::vector<double> > value) { m_FoundDotsCoordinateValue = value; };

  /*! Get the coordinates of the found dots */
  std::vector< std::vector<double> >&  GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };

  /*! Set the cumulate intensity of all dots in the pattern */
  void SetIntensity(double value) { m_Intensity = value; };

  /*! Get the cumulate intensity of all dots in the pattern */
  double GetIntensity() const { return m_Intensity; };

  /*! Set the number of candidate points that have been found */
  void SetNumDots(double value) { m_NumDots = value; };

  /*! Get the number of candidate points that have been found */
  double GetNumDots() const { return m_NumDots; };

  /*! Set the dots that are considered candidates */
  void SetCandidateFidValues(std::vector<FidDot> value) { m_CandidateFidValues = value; };

  /*! Get the dots that are considered candidates */
  const std::vector<FidDot>& GetCandidateFidValues() const { return m_CandidateFidValues; };

protected:
  /*! True if the dots are found, false otherwise. */
  bool m_DotsFound;

  /*! X and Y values of found dots. */
  std::vector< std::vector<double> >  m_FoundDotsCoordinateValue;

  /*! The combined intensity of the dots. This is the sum of the pixel
  values after the morphological operations, with the pixel values on the
  range 0-1.  A good intensity score is over 100. A bad one (but still
  valid) is below 25. */
  double m_Intensity;

  /*! number of possible fiducial points */
  double m_NumDots; 

  /*! pointer to the fiducial candidates coordinates */
  std::vector<FidDot>  m_CandidateFidValues; 
};

#endif //_FIDUCIAL_ALGORITHM_COMMON_H
