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

    /*! Compute parameters such as the minimum and the maximum angle allowed for one line in the case where the sgmentation 
        parameters are to be computed. This allows a better precision and possibly an increase of computation speed. */
		void				ComputeParameters();

    /*! Clear the member attributes when not needed anymore */
    void        Clear();

    /*! Read the configuration file from a vtk XML data element */
		PlusStatus	ReadConfiguration( vtkXMLDataElement* rootConfigElement );

    /*! Set the size of the frame as an array */
    void        SetFrameSize( int frameSize[2] );

    /*! Find the n-points lines from a list of 2-points lines */
    void 				FindLinesNPoints();

    /*! Find 2-points lines from a list of Dots */
		void 				FindLines2Points();

    /*! Compute the length of the segment between 2 dots */
		float				SegmentLength( Dot *dot1, Dot *dot2 );

    /*! Compute the length of a line and sets it origin */
		float				LineLength( Line &line );

    /*! Compute the shortest distance from a point: dot, to a line: line */
    float       ComputeDistancePointLine(Dot dot, Line line);

    /*! Compute a line, all that is required is a set origin and the dots part of the line. It computes then the line length,
        the direction vector, the endpoint */
		void 				ComputeLine( Line &line );

    /*! Compute the angle between the lin formed by 2 dots and the x-axis */
		float				ComputeSlope( Dot *dot1, Dot *dot2 );

    /*! Return true if a line matches the requirements, false otherwise */
		bool				AcceptLine( Line &line );  

    /*! Return true if an angle is in the allowed angle range, false otherwise */
    bool        AcceptAngle(float angle);

    /*! Find lines, runs the FindLines2Points and FindLinesNPoints and then sort the lines by intensity */
		void				FindLines();

    //Accessors and mutators

    /*! Get the */
    double			GetMaxTheta() { return m_MaxTheta; };
    double			GetMinTheta() { return m_MinTheta; };

    double			GetMaxLineErrorMm() { return m_MaxLineErrorMm; };

    void				SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };

    std::vector<Pattern*>     GetPatterns() { return m_Patterns; };
    void        SetPatterns( std::vector<Pattern*> value ) { m_Patterns = value; };
    
    std::vector<std::vector<Line> >	GetLinesVector() { return m_LinesVector; };

    void				SetDotsVector(std::vector<Dot> value) { m_DotsVector = value; };

    std::vector<NWire> GetNWires();

    double *		GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg() { return m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg; };
    double *		GetImageToPhantomTransform() { return m_ImageToPhantomTransform; };

    void        SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };
    void        SetMaxLineLengthErrorPercent(double value) { m_MaxLineLengthErrorPercent = value; };
    void        SetCollinearPointsMaxDistanceFromLineMm(double value) { m_CollinearPointsMaxDistanceFromLineMm = value; };
    void        SetMaxLineErrorMm(double value) { m_MaxLineErrorMm = value; };
    void        SetMinThetaDegrees(double value) { m_MinTheta = value; };
    void        SetMaxThetaDegrees(double value) { m_MaxTheta = value; };

  protected:
		int					m_FrameSize[2];
		double			m_ApproximateSpacingMmPerPixel;
		double			m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[6];
		double			m_ImageToPhantomTransform[16];

		// line length and line pair distance errors in percent - read from phantom definition
		double 			m_MaxLineLengthErrorPercent;
		double 			m_MaxLinePairDistanceErrorPercent;

		double 			m_MaxLineErrorMm;

		double 			m_CollinearPointsMaxDistanceFromLineMm; 

		double 			m_MinTheta; 
		double 			m_MaxTheta;

		std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates

		std::vector<Dot>	m_DotsVector;
		std::vector<std::vector<Line> >	m_LinesVector;
    
    std::vector<Pattern*> m_Patterns;
};

#endif // _FIDUCIAL_LINE_FINDER_H
