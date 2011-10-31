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

class FidLineFinder
{
	public:
		FidLineFinder();
		virtual ~FidLineFinder();

		void				ComputeParameters();

    void        Clear();

		PlusStatus	ReadConfiguration( vtkXMLDataElement* rootConfigElement );
    void        SetFrameSize( int frameSize[2] );

    void 				FindLinesNPoints();
		void 				FindLines2Points();

		float				SegmentLength( Dot *d1, Dot *d2 );
		float				LineLength( Line &line );
    float       ComputeDistancePointLine(Dot dot, Line line);


		void 				ComputeLine( Line &line );
		float				ComputeSlope( Dot *dot1, Dot *dot2 );

		bool				AcceptLine( Line &line );  
    bool        AcceptAngle(float angle);

		void				FindLines();

		void				SortRightToLeft( Line *line );

    //Accessors and mutators
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

		float			  m_Angles;

		std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates

		std::vector<Dot>	m_DotsVector;
		std::vector<std::vector<Line> >	m_LinesVector;
    
    std::vector<Pattern*> m_Patterns;
};

#endif // _FIDUCIAL_LINE_FINDER_H
