#include "FidPatternRecognition.h"
#include "vtkMath.h"

static const float DOT_STEPS  = 4.0;
static const float DOT_RADIUS = 6.0;

//-----------------------------------------------------------------------------

FidPatternRecognition::FidPatternRecognition()
{
}

//-----------------------------------------------------------------------------

FidPatternRecognition::~FidPatternRecognition()
{
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::ReadConfiguration(vtkXMLDataElement* segmentationParameters)
{
	m_FidSegmentation.ReadConfiguration(segmentationParameters);
	m_FidLineFinder.ReadConfiguration(segmentationParameters);
  m_FidLabelling.ReadConfiguration(segmentationParameters, m_FidLineFinder.GetMinTheta(), m_FidLineFinder.GetMaxTheta(), m_FidLineFinder.GetMaxLineErrorMm());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus FidPatternRecognition::RecognizePattern(PixelType * image, PatternRecognitionResult &patternRecognitionResult)
{
  m_FidSegmentation.Clear();
  m_FidLineFinder.Clear();
  m_FidLabelling.Clear();
  
  int bytes = m_FidSegmentation.GetFrameSize()[0]*m_FidSegmentation.GetFrameSize()[1]*sizeof(PixelType);

  memcpy( m_FidSegmentation.GetWorking(), image, bytes );
	memcpy( m_FidSegmentation.GetUnalteredImage(), image, bytes);

	m_FidSegmentation.MorphologicalOperations();

	m_FidSegmentation.Suppress( m_FidSegmentation.GetWorking(), m_FidSegmentation.GetThresholdImage()/100.00 );

	m_FidSegmentation.Cluster();

	m_FidLabelling.SetNumDots(m_FidSegmentation.GetDotsVector().size()); 
	m_FidSegmentation.SetCandidateFidValues(m_FidSegmentation.GetDotsVector());	  
	 
	if(m_FidSegmentation.GetDebugOutput()) 
	{
		m_FidSegmentation.WritePossibleFiducialOverlayImage(m_FidSegmentation.GetCandidateFidValues(), m_FidSegmentation.GetUnalteredImage()); 
	}

  m_FidLineFinder.SetCandidateFidValues(m_FidSegmentation.GetCandidateFidValues());
  m_FidLineFinder.SetDotsVector(m_FidSegmentation.GetDotsVector());
  m_FidLabelling.SetDotsVector(m_FidSegmentation.GetDotsVector());
   

	m_FidLineFinder.FindLines();

  m_FidLabelling.SetLinesVector(m_FidLineFinder.GetLinesVector());

	switch (m_FidSegmentation.GetFiducialGeometry())
	{
	case FidSegmentation::CALIBRATION_PHANTOM_6_POINT:
		m_FidLabelling.FindDoubleNLines();
		break;
	default:
		LOG_ERROR("Segmentation error: invalid phantom geometry identifier!"); 
		break;
	}

	patternRecognitionResult.SetAngles(m_FidLabelling.GetAngleConf());
	patternRecognitionResult.SetIntensity(m_FidLabelling.GetLinePairIntensity());
	patternRecognitionResult.SetNumDots(m_FidLabelling.GetDotsVector().size()); 
	patternRecognitionResult.SetDotsFound(m_FidLabelling.GetDotsFound());
	patternRecognitionResult.SetFoundDotsCoordinateValue(m_FidLabelling.GetFoundDotsCoordinateValue());
	patternRecognitionResult.SetCandidateFidValues(m_FidSegmentation.GetCandidateFidValues());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawResults( PixelType *image )
{
  if ( m_FidLabelling.GetPairsVector().size() > 0 )
  {
		DrawPair( image, m_FidLabelling.GetPairsVector().begin() );
  }
	else
	{
		cout << "ERROR: could not find the pair of the wires!  See other drawing outputs for more information!" << endl;
		DrawLines( image, m_FidLabelling.GetLinesVector().begin(), m_FidLabelling.GetLinesVector().size() );
		DrawDots( image, m_FidLabelling.GetDotsVector().begin(), m_FidLabelling.GetDotsVector().size() );
	}

}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawDots( PixelType *image, std::vector<Dot>::iterator dotsIterator, int ndots)
{
	for ( int d = 0; d < ndots; d++ ) {
		float row = dotsIterator[d].GetY();
		float col = dotsIterator[d].GetX();

		for ( float t = 0; t < 2*vtkMath::Pi(); t += vtkMath::Pi()/DOT_STEPS ) {
			int r = (int)floor( row + cos(t) * DOT_RADIUS );
			int c = (int)floor( col + sin(t)* DOT_RADIUS );

			if ( r >= 0 && r < m_FidLabelling.GetFrameSize()[1] && c >= 0 && c <=  m_FidLabelling.GetFrameSize()[0] )
      {
        image[r* m_FidLabelling.GetFrameSize()[0]+c] = UCHAR_MAX;
      }
		}

		image[static_cast<int>(floor(row)* m_FidLabelling.GetFrameSize()[0]+floor(col))] = 0; 
	}
}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawLines( PixelType *image, std::vector<Line>::iterator linesIterator, int nlines )
{
	for ( int l = 0; l < nlines; l++ )
	{
		float theta = linesIterator[l].GetLineSlope();
		float p = linesIterator[l].GetLinePosition();

		for (int i=0; i<3; i++)
		{
			DrawDots( image, m_FidLabelling.GetDotsVector().begin()+linesIterator[l].GetLinePoint(i), 1 );//watch out, check for iterators problems if errors
		}		

		if ( theta < vtkMath::Pi()/4 || theta > 3*vtkMath::Pi()/4 ) {
			for ( int y = 0; y <  m_FidLabelling.GetFrameSize()[1]; y++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float x = roundf(( p - y * sin(theta) ) / cos(theta));
				double x = floor( ( p - y * sin(theta) ) / cos(theta) + 0.5 );
				int r =  m_FidLabelling.GetFrameSize()[1] - y - 1;
				int c = (unsigned int)x;
				if ( c >= 0 && c <  m_FidLabelling.GetFrameSize()[0] )
					image[r* m_FidLabelling.GetFrameSize()[0]+c] = UCHAR_MAX;
			}
		}
		else {
			for ( int x = 0; x < m_FidLabelling.GetFrameSize()[0]; x++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float y = roundf(( p - x * cos(theta) ) / sin(theta));
				double y = floor( ( p - x * cos(theta) ) / sin(theta) + 0.5 );
				int r =  m_FidLabelling.GetFrameSize()[1] - (unsigned int)y - 1;
				int c = x;
				if ( r >= 0 && r <  m_FidLabelling.GetFrameSize()[1] )
					image[r* m_FidLabelling.GetFrameSize()[0]+c] = UCHAR_MAX;
			}
		}
	}
}

//-----------------------------------------------------------------------------

void FidPatternRecognition::DrawPair( PixelType *image, std::vector<LinePair>::iterator pairIterator )
{
	/* Drawing on the original. */
	DrawLines( image, m_FidLabelling.GetLinesVector().begin()+pairIterator->GetLine1(), 1 );
	DrawLines( image, m_FidLabelling.GetLinesVector().begin()+pairIterator->GetLine2(), 1 );
}

//-----------------------------------------------------------------------------