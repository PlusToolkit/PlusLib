/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_LINE_FINDER_H
#define _FIDUCIAL_LINE_FINDER_H

#include "FidPatternRecognitionCommon.h"
#include "PlusConfigure.h"

#include "vtkXMLDataElement.h"

/*!
\class FidLineFinder
\brief This class is used to find the n-points lines from a list of dots. The lines have fixed length and tolerance
and their direction vector restricted according to the configuration file. It first finds 2-points lines and 
then computes n-points lines from these 2-points lines.
\ingroup PlusLibPatternRecognition
*/

class FidLineFinder
{
public:
  FidLineFinder();
  virtual ~FidLineFinder();

  /*! Set the size of the frame as an array */
  void SetFrameSize( int frameSize[2] );

  /*! Set the values of the candidate fiducials */
  void SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };

  /*! Set the vector of dots that have been found by FidSegmentation */
  void SetDotsVector(std::vector<Dot> value) { m_DotsVector = value; };

  /*! Set the pattern structure vector, this defines the patterns that the algorthm finds */
  void SetPatterns( std::vector<Pattern*> value ) { m_Patterns = value; };

  /*! Read the configuration file from a vtk XML data element */
  PlusStatus	ReadConfiguration( vtkXMLDataElement* rootConfigElement );

  /*! Find lines, runs the FindLines2Points and FindLinesNPoints and then sort the lines by intensity */
  void FindLines();

  /*! Get the vector of lines, this vector contains all lines of different number of points that match the criteria */
  std::vector<std::vector<Line> >	GetLinesVector() { return m_LinesVector; };

  /*! Get the maximum angle allowed for a line, in radians */
  double GetMaxThetaRad() { return m_MaxThetaRad; };

  /*! Get the minimum angle allowed for a line, in radians */
  double GetMinThetaRad() { return m_MinThetaRad; };

  /*! Get the pattern structure vector, this defines the patterns that the algorthm finds */
  std::vector<Pattern*> GetPatterns() { return m_Patterns; };

  /*! Get the NWires vector, these NWires are extracted from the pattern vector */
  std::vector<NWire> GetNWires();

  /*! Clear the member attributes when not needed anymore */
  void Clear();

protected:
  /*! Compute parameters such as the minimum and the maximum angle allowed for one line in the case where the sgmentation 
  parameters are to be computed. This allows a better precision and possibly an increase of computation speed. */
  void ComputeParameters();

  /*! Find the n-points lines from a list of 2-points lines */
  void FindLinesNPoints();

  /*! Find 2-points lines from a list of Dots */
  void FindLines2Points();

  /*! Compute the length of the segment between 2 dots */
  float SegmentLength( Dot *dot1, Dot *dot2 );

  /*! Compute the shortest distance from a point: dot, to a line: line */
  float ComputeDistancePointLine(Dot dot, Line line);

  /*! Compute a line, all that is required is a set origin and the dots part of the line. It computes then the line length,
  the direction vector, the endpoint */
  void ComputeLine( Line &line );

  /*! Compute the angle between thee lin formed by 2 dots and the x-axis, in radian */
  static float ComputeAngleRad( Dot *dot1, Dot *dot2 );

  /*! Return true if a line matches the requirements, false otherwise */
  bool AcceptLine( Line &line );  

  /*! Return true if an angle is in the allowed angle range, false otherwise */
  bool AcceptAngleRad(float angleRad);

  //Accessors and mutators

  /*! Get the maximum rotation vector, this maximum rotation represents the physical limitation of the probe, 
  used for automatic parameters computation */
  double * GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg() { return m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg; };

  /*! Get the image to phantom transform matrix */
  double * GetImageToPhantomTransform() { return m_ImageToPhantomTransform; };

  /*! Set the approximate spacing in Mm per pixel */
  void SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };

  /*! Set the maximum distance from a point to a line when the point is tested to be a point of the line */
  void SetCollinearPointsMaxDistanceFromLineMm(double value) { m_CollinearPointsMaxDistanceFromLineMm = value; };

  /*! Set the minimum angle allowed for a line, in degrees */
  void SetMinThetaDeg(double value);

  /*! Set the maximum angle allowed for a line, in degrees */
  void SetMaxThetaDeg(double value);

protected:
  int					m_FrameSize[2];
  double			m_ApproximateSpacingMmPerPixel;
  double			m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[6];
  double			m_ImageToPhantomTransform[16];

  // line length and line pair distance errors in percent - read from phantom definition
  double 			m_MaxLinePairDistanceErrorPercent;
  double 			m_CollinearPointsMaxDistanceFromLineMm; 
  double 			m_MinThetaRad; 
  double 			m_MaxThetaRad;

  std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates

  std::vector<Dot>	m_DotsVector;
  std::vector<std::vector<Line> >	m_LinesVector;

  std::vector<Pattern*> m_Patterns;
};

#endif // _FIDUCIAL_LINE_FINDER_H
