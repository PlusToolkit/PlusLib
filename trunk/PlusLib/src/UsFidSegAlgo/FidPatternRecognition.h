#ifndef _FID_PATTERN_RECOGNITION_H
#define _FID_PATTERN_RECOGNITION_H

#include "FidPatternRecognitionCommon.h"
#include "FidSegmentation.h"
#include "FidLineFinder.h"
#include "FidLabeling.h"

#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

class FidPatternRecognition
{
	public:
    FidPatternRecognition();
		virtual ~FidPatternRecognition();

    PlusStatus        ReadConfiguration(vtkXMLDataElement* segmentationParameters);
		PlusStatus        RecognizePattern(PixelType* image, int imageSize[2], PatternRecognitionResult &patternRecognitionResult);

    void              DrawDots(PixelType* image, std::vector<Dot>::iterator dotsIterator, int ndots);
    void              DrawLines(PixelType* image, std::vector<Line>::iterator linesIterator, int nlines);
    void              DrawPair(PixelType* image, std::vector<LinePair>::iterator pairIterator);
    void              DrawResults(PixelType* image);

    FidSegmentation*	GetFidSegmentation() { return	& m_FidSegmentation; };
    FidLineFinder*		GetFidLineFinder() { return & m_FidLineFinder; };
    FidLabeling* 		GetFidLabeling() { return & m_FidLabeling; };

	protected:
    
		FidSegmentation		m_FidSegmentation;
		FidLineFinder			m_FidLineFinder;
		FidLabeling 			m_FidLabeling;

};

//-----------------------------------------------------------------------------

#endif //_FID_PATTERN_RECOGNITION_H
