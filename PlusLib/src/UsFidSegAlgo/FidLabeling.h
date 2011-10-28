/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_LABELLING_H
#define _FIDUCIAL_LABELLING_H

#include "FidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
#include "vtkXMLDataElement.h"

class LabelingResults
{
  public:
    int patternId;//Id of the pattern
    int wireId;//Id of the wire in the pattern
    float x,y;//coordinate in the image plane
};

class FidLabeling
{
	public:
		FidLabeling();
		virtual ~FidLabeling();

		void					UpdateParameters();
		void					ComputeParameters();
    void          Clear();

		PlusStatus		ReadConfiguration( vtkXMLDataElement* rootConfigElement, double minTheta, double maxTheta, double maxLineErrorMm );
    void          SetFrameSize(int frameSize[2]);

    float         ComputeDistancePointLine(Dot dot, Line line);
    float         ComputeShift(Line line1, Line line2);
    float         ComputeSlope( Line &line );

		void					FindDoubleNLines();
		void					FindPattern();
    void          UpdateCirsResults(Line resultLine1, Line resultLine2, Line resultLine3);
    void          UpdateNWiresResults(Line resultLine1, Line resultLine2);

		void					SortTopToBottom( LinePair *pair );
		void					SortRightToLeft( Line *line );
    static bool   SortCompare(std::vector<double> temporaryLine1, std::vector<double> temporaryLine2);
		Line          SortPointsByDistanceFromOrigin(Line fiducials); 

    //Accessors and mutators
    double				GetNumDots() { return m_NumDots; };
    void					SetNumDots(double value) { m_NumDots = value; };

    std::vector<Dot>		GetDotsVector() {return m_DotsVector; };	
    void				  SetDotsVector(std::vector<Dot> value) { m_DotsVector = value; };

    void				  SetLinesVector(std::vector<std::vector<Line> > value) { m_LinesVector = value; };

    std::vector<Pattern*> GetPatterns() { return m_Patterns; };
    void          SetPatterns( std::vector<Pattern*> value ) { m_Patterns = value; };

    float					GetAngleConf() { return m_AngleConf; };
    float					GetLinePairIntensity() { return m_LinePairIntensity; };

    void					SetDotsFound(bool value) { m_DotsFound = value; };
    bool					GetDotsFound() { return m_DotsFound; };

    std::vector<LinePair>		GetPairsVector() {return m_PairsVector; };	
    std::vector< std::vector<double> >	GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };
    std::vector<std::vector<Line> >	GetLinesVector() { return m_LinesVector; };

    void          SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };
    void          SetMaxLinePairDistanceErrorPercent(double value) { m_MaxLinePairDistanceErrorPercent = value; };
    void          SetMaxAngleDifferenceDegrees(double value) { m_MaxAngleDiff = value; };

    void          SetMinThetaDegrees(double value) { m_MinTheta = value; };
    void          SetMaxThetaDegrees(double value) { m_MaxTheta = value; };

    void          SetAngleToleranceDegrees(double value) { m_AngleTolerance = value; };

	protected:
    int					  m_FrameSize[2];

		double				m_ApproximateSpacingMmPerPixel;
		double 				m_MaxAngleDiff;
		double 				m_MinLinePairDistMm; 	
		double 				m_MaxLinePairDistMm;
		double 				m_MaxLinePairDistanceErrorPercent;
		double				m_NumDots;
		double 				m_MaxLineErrorMm;
		double 				m_MinTheta;
		double 				m_MaxTheta;
		
		bool					m_DotsFound;

		float					m_AngleConf;
    float         m_AngleTolerance;
    float         m_InclinedLineAngle;
		float					m_LinePairIntensity;
		
		std::vector<Dot>		  m_DotsVector;
    std::vector<Pattern*>  m_Patterns;
		std::vector<LinePair>	m_PairsVector;
    std::vector<std::vector<Line> > m_LinesVector;
		std::vector< std::vector<double> >  m_FoundDotsCoordinateValue;
    std::vector<LabelingResults> m_Results;
};

#endif // _FIDUCIAL_LABELLING_H
