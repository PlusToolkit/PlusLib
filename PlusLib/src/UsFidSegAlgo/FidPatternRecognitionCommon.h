/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FIDUCIAL_ALGORITHM_COMMON_H
#define _FIDUCIAL_ALGORITHM_COMMON_H

#include <vector>

typedef unsigned char PixelType;

//-----------------------------------------------------------------------------

struct Item//TODO find about it and hopefully change it to a class with proper name
{
	Item(){ roff = 0; coff = 0; }; 
	Item( int r, int c) { roff = r; coff = c; }; 
	bool operator== (const Item& data) const { return (roff == data.roff && coff == data.coff) ; }
	int roff;
	int coff;
};

//-----------------------------------------------------------------------------

class Dot
{
	public:
		static bool lessThan( Dot &dot1, Dot &dot2 );//compare the intensity of 2 dots

		void	SetX(float value) { m_X = value; };
		float	GetX() { return m_X; };
		void	SetY(float value) { m_Y = value; };
		float	GetY() { return m_Y; };
		void	SetDotIntensity(float value) { m_DotIntensity = value; };
		float	GetDotIntensity() { return m_DotIntensity; };

	protected:
		float	m_X;
		float	m_Y;
    //float m_Position[4];
		float	m_DotIntensity;
};

//-----------------------------------------------------------------------------

class Position
{
	public:
		//static bool lessThan( Dot *b1, Dot *b2 );//compare two dots (coordinate wise)
		static bool lessThan( std::vector<Dot>::iterator b1, std::vector<Dot>::iterator b2 );
		
		void	SetX(int value) { m_X = value; };
		int		GetX() { return m_X; };
		void	SetY(int value) { m_Y = value; };
		int		GetY() { return m_Y; };

	protected:
		int		m_X;
		int		m_Y;
};

//-----------------------------------------------------------------------------

class Line
{
	public:
		static bool  lessThan( Line &line1, Line &line2 );//compare the intensity of 2 lines
		static bool  compareLines( Line line1, Line line2 );//compare two lines
    static float ComputeAngle(Line &line1);
    static float ComputeHalfSpaceAngle(Line &line);

    void				SetPoint(int aIndex, int aValue) { m_Points[aIndex] = aValue; };
    int					GetPoint(int aIndex) const{ return m_Points[aIndex]; };
    std::vector<int>*	GetPoints() { return &m_Points; };
		void				SetIntensity(float value) { m_Intensity = value; };
		float				GetIntensity() { return m_Intensity; };
		void				SetError(float value) { m_Error = value; };
		float				GetError() { return m_Error; };
		void				SetLength(float value) { m_Length = value; };
		float				GetLength() { return m_Length; };
    void				SetDirectionVector(int aIndex, float aValue) { m_DirectionVector[aIndex] = aValue; };
    float				GetDirectionVector(int aIndex) const{ return m_DirectionVector[aIndex]; };
    void        SetOrigin(int value) { m_Origin = value; };
    int         GetOrigin() { return m_Origin; };
    void        SetEndPoint(int value) { m_EndPoint = value; };
    int         GetEndPoint() { return m_EndPoint; };
    
    
	protected:
		std::vector<int>	m_Points; // indices of points that make up the line
		float				m_Intensity;
		float				m_Error;
		float				m_Length;
    float       m_DirectionVector[2];
    int         m_Origin;//index of the line origin
    int         m_EndPoint;//Index of the endpoint of the line
    

};

//-----------------------------------------------------------------------------

class LinePair
{
	public:
		static bool lessThan( LinePair &pair1, LinePair &pair2 );//compare the intensity of two pairs of lines

		void	SetLine1(int value) { m_Line1 = value; };
		int		GetLine1() { return m_Line1; };
		void	SetLine2(int value) { m_Line2 = value; };
		int		GetLine2() { return m_Line2; };
		void	SetLinePairIntensity(float value) { m_LinePairIntensity = value; };
		float	GetLinePairIntensity() { return m_LinePairIntensity; };
		void	SetLinePairError(float value) { m_LinePairError = value; };
		float	GetLinePairError() { return m_LinePairError; };
		void	SetAngleDifference(float value) { m_AngleDifference = value; };
		float	GetAngleDifference() { return m_AngleDifference; };
		void	SetAngleConf(float value) { m_AngleConf = value; };
		float	GetAngleConf() { return m_AngleConf; };

	protected:
		int		m_Line1;
		int		m_Line2;
		float	m_LinePairIntensity;
		float	m_LinePairError;
		float	m_AngleDifference;
		float	m_AngleConf;
};


//-----------------------------------------------------------------------------

class SortedAngle
{
	public:
		static bool lessThan( SortedAngle &pt1, SortedAngle &pt2 );//compare two vector angles

		void				SetAngle(double value) { m_Angle = value; };
		double				GetAngle() { return m_Angle; };
		void				SetPointIndex(int value) { m_PointIndex = value; };
		int					GetPointIndex() { return m_PointIndex; };
		void				SetCoordinate(std::vector<double> value) { m_Coordinate = value; };
		std::vector<double>	GetCoordinate() { return m_Coordinate; };

	protected:
		double				m_Angle;
		int					m_PointIndex;
		std::vector<double>	m_Coordinate;
};

//-----------------------------------------------------------------------------

struct Wire
{
  std::string name;
	double endPointFront[3];
	double endPointBack[3];
};

//-----------------------------------------------------------------------------

class Pattern
{
  public:
    unsigned short    id;
    std::vector<Wire> wires;
    std::vector<float> distanceToOriginMm;//These distances are in mm.
    std::vector<float> distanceToOriginToleranceMm;//These tolerances are in mm.

    //unused iterator to iterate only through one type of Patterns.
/*
    template<class T> class iterator
    {
      public:
        iterator(std::vector<Pattern>* ptr) : m_Pointer(ptr) {}
        iterator operator++() 
        {
          do
          {
            T* t = dynamic_cast<T*>(m_Pointer);
            if(t)//belongs to the type T
            {
              iterator i = *this;
              m_Pointer++;
              return i;
            }
            m_Pointer++;
          }while(m_Pointer != NULL);
          return *this;
        }
      protected:
        std::vector<Pattern> * m_Pointer;
    };
*/
};

//-----------------------------------------------------------------------------

class NWire : public Pattern
{
public:
	double  intersectPosW12[3]; // Use wire.id mod 3 if not first layer
	double  intersectPosW32[3];
};

//-----------------------------------------------------------------------------

class CoplanarParallelWires : public Pattern
{
};

//-----------------------------------------------------------------------------

#define WIRE1 3
#define WIRE2 4
#define WIRE3 5
#define WIRE4 0
#define WIRE5 1
#define WIRE6 2

class PatternRecognitionResult
{
	public:
		PatternRecognitionResult();
		virtual ~PatternRecognitionResult();

		void							Clear();

		void							SetDotsFound(bool value) { m_DotsFound = value; };
		bool							GetDotsFound() { return m_DotsFound; };

		void							SetFoundDotsCoordinateValue(std::vector< std::vector<double> > value) { m_FoundDotsCoordinateValue = value; };
		std::vector< std::vector<double> >	GetFoundDotsCoordinateValue() { return m_FoundDotsCoordinateValue; };

		void							SetAngles(float value) { m_Angles = value; };
		float							GetAngles() { return m_Angles; };

		void							SetIntensity(float value) { m_Intensity = value; };
		float							GetIntensity() { return m_Intensity; };
		void						  SetNumDots(double value) { m_NumDots = value; };
		double						GetNumDots() { return m_NumDots; };
		void							SetCandidateFidValues(std::vector<Dot> value) { m_CandidateFidValues = value; };
		std::vector<Dot>	GetCandidateFidValues() { return m_CandidateFidValues; };

	protected:
		/* True if the dots are found, false otherwise. */
		bool							m_DotsFound;

		/* X and Y values of found dots. */
		//vector<vector<double>> m_FoundDotsCoordinateValue;
		std::vector< std::vector<double> >	m_FoundDotsCoordinateValue; 
		/* The degree to which the lines are parallel and the dots linear.  On the
		 * range 0-1, with 0 being a very good angles score and 1 being the
		 * threshold of acceptability. */
		float							m_Angles;

		/* The combined intensity of the six dots. This is the sum of the pixel
		 * values after the morphological operations, with the pixel values on the
		 * range 0-1.  A good intensity score is over 100. A bad one (but still
		 * valid) is below 25. */
		float							m_Intensity;
		double						m_NumDots; // number of possibel fiducial points
		std::vector<Dot>	m_CandidateFidValues; // pointer to the fiducial candidates coordinates
};

//-----------------------------------------------------------------------------

#endif //_FIDUCIAL_ALGORITHM_COMMON_H
