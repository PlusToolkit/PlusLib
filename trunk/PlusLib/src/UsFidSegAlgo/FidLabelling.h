#ifndef _FIDUCIAL_LABELLING_H
#define _FIDUCIAL_LABELLING_H

#include "FidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
#include "vtkXMLDataElement.h"

class FidLabelling
{
	public:
		FidLabelling();
		virtual ~FidLabelling();

		void					UpdateParameters();
		void					ComputeParameters();
    void          Clear();

		PlusStatus		ReadConfiguration( vtkXMLDataElement* segmentationParameters, double minTheta, double maxTheta, double maxLineErrorMm );

		void					FindDoubleNLines();
		void					FindPairs();

		void					SortTopToBottom( LinePair *pair );
		void					SortRightToLeft( Line *line );
		static std::vector<std::vector<double> > SortInAscendingOrder(std::vector<std::vector<double> > fiducials); 

    //Accessors and mutators
    double				GetNumDots() { return m_NumDots; };
    void					SetNumDots(double value) { m_NumDots = value; };

    std::vector<Dot>		GetDotsVector() {return m_DotsVector; };	
    void				  SetDotsVector(std::vector<Dot> value) { m_DotsVector = value; };

    void				  SetLinesVector(std::vector<Line> value) { m_LinesVector = value; };
    void					SetNWires(std::vector<NWire> value) { m_NWires = value; };

    float					GetAngleConf() { return m_AngleConf; };
    float					GetLinePairIntensity() { return m_LinePairIntensity; };

    void					SetDotsFound(bool value) { m_DotsFound = value; };
    bool					GetDotsFound() { return m_DotsFound; };

    std::vector<LinePair>		GetPairsVector() {return m_PairsVector; };	
    std::vector< std::vector<double> >	GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };
    std::vector<Line>	GetLinesVector() { return m_LinesVector; };

    int *					GetFrameSize() { return m_FrameSize; };

    void          SetApproximateSpacingMmPerPixel(double value) { m_ApproximateSpacingMmPerPixel = value; };
    void          SetMaxLinePairDistanceErrorPercent(double value) { m_MaxLinePairDistanceErrorPercent = value; };
    void          SetMaxAngleDifferenceDegrees(double value) { m_MaxAngleDiff = value; };

	protected:
    int						  m_FrameSize[2];

		double					m_ApproximateSpacingMmPerPixel;
		double 					m_MaxAngleDiff;
		double 					m_MinLinePairDistMm; 	
		double 					m_MaxLinePairDistMm;
		double 					m_MaxLinePairDistanceErrorPercent;
		double					m_NumDots;
		double 					m_MaxLineErrorMm;
		double 					m_MinTheta;
		double 					m_MaxTheta;
		
		bool					  m_DotsFound;

		float					  m_AngleConf;
		float					  m_LinePairIntensity;

		std::vector<NWire>		m_NWires;
		std::vector<Dot>		  m_DotsVector;
		std::vector<Line>		  m_LinesVector;
		std::vector<LinePair>	m_PairsVector;

		std::vector< std::vector<double> >  m_FoundDotsCoordinateValue;
};

#endif // _FIDUCIAL_LABELLING_H
