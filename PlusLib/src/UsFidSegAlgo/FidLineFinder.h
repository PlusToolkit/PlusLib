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

		void				UpdateParameters();
		void				ComputeParameters();

    void        Clear();

		PlusStatus	ReadConfiguration( vtkXMLDataElement* segmentationParameters );

    void 				FindLines3Points();
		void 				FindLines2Points();

		float				SegmentLength( Dot *d1, Dot *d2 );
		float				LineLength( Line &line, std::vector<Dot> dots );

		void 				ComputeLine( Line &line, std::vector<Dot> dots );
		float				ComputeSlope( Dot *dot1, Dot *dot2 );

		bool				AcceptLine( Line &line );  

		void				FindLines();

		void				SortRightToLeft( Line *line );

    //Accessors and mutators
    double			GetMaxTheta() { return m_MaxTheta; };
    double			GetMinTheta() { return m_MinTheta; };

    double			GetMaxLineErrorMm() { return m_MaxLineErrorMm; };

    void				SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };
    void				SetNWires(std::vector<NWire> value) { m_NWires = value; };
    
    std::vector<Line>	GetLinesVector() { return m_LinesVector; };

    void				SetDotsVector(std::vector<Dot> value) { m_DotsVector = value; };

    double *		GetImageNormalVectorInPhantomFrameMaximumRotationAngleDeg() { return m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg; };
    double *		GetImageToPhantomTransform() { return m_ImageToPhantomTransform; };

    void        SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };
    void        SetMaxLineLengthErrorPercent(double value) { m_MaxLineLengthErrorPercent = value; };
    void        SetFindLines3PtDistanceMm(double value) { m_FindLines3PtDistanceMm = value; };
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

		// min and max length of 3pt line (mm) - computed from input error percent and NWire definitions
		double 			m_MinLineLenMm;
		double 			m_MaxLineLenMm;
		double 			m_MaxLineErrorMm;

		double 			m_FindLines3PtDistanceMm; 

		double 			m_MinTheta; 
		double 			m_MaxTheta;

		/* The degree to which the lines are parallel and the dots linear.  On the
		 * range 0-1, with 0 being a very good angles score and 1 being the
		 * threshold of acceptability. */
		float			  m_Angles;

		std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates

		std::vector<Dot>	m_DotsVector;

		std::vector<Line>	m_LinesVector;
		std::vector<Line>	m_TwoPointsLinesVector;
		std::vector<NWire>	m_NWires;
};

#endif // _FIDUCIAL_LINE_FINDER_H
