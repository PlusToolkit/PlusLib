/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FID_PATTERN_RECOGNITION_H
#define _FID_PATTERN_RECOGNITION_H

#include "FidPatternRecognitionCommon.h"
#include "FidSegmentation.h"
#include "FidLineFinder.h"
#include "FidLabeling.h"

#include "vtkTrackedFrameList.h"

#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

class FidPatternRecognition
{
	public:
    FidPatternRecognition();
		virtual ~FidPatternRecognition();

    PlusStatus        ReadConfiguration(vtkXMLDataElement* rootConfigElement);

    /*!
      Run pattern recognition on a tracked frame list. It only segments the tracked frames which were not already segmented
      \param trackedFrameList Tracked frame list to segment
      \param numberOfSuccessfullySegmentedImages Out parameter holding the number of segmented images in this call (it is only equals the number of all segmented images in the tracked frame if it was not segmented at all)
    */
    PlusStatus        RecognizePattern(vtkTrackedFrameList* trackedFrameList, int* numberOfSuccessfullySegmentedImages = NULL);
    PlusStatus        RecognizePattern(TrackedFrame* trackedFrame);
		PlusStatus        RecognizePattern(TrackedFrame* trackedFrame, PatternRecognitionResult &patternRecognitionResult);

    void              DrawDots(PixelType* image, std::vector<Dot>::iterator dotsIterator, int ndots);
    void              DrawLines(PixelType* image, std::vector<Line>::iterator linesIterator, int nlines);
    void              DrawPair(PixelType* image, std::vector<LinePair>::iterator pairIterator);
    void              DrawResults(PixelType* image);

    PlusStatus        ComputeNWireIntersections();

    FidSegmentation*	GetFidSegmentation() { return	& m_FidSegmentation; };
    FidLineFinder*		GetFidLineFinder() { return & m_FidLineFinder; };
    FidLabeling* 		  GetFidLabeling() { return & m_FidLabeling; };

  protected:

	  PlusStatus        ReadPhantomDefinition(vtkXMLDataElement* rootConfigElement);

	protected:
    
		FidSegmentation		m_FidSegmentation;
		FidLineFinder			m_FidLineFinder;
		FidLabeling 			m_FidLabeling;

    int               m_CurrentFrame;

};

//-----------------------------------------------------------------------------

#endif //_FID_PATTERN_RECOGNITION_H
