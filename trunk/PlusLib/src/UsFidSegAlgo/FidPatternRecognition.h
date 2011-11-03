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

/*!
  \class FidPatternRecognition
  \brief This class manages the whole pattern recognition algorithm. From a vtk XML data element it handles
         the initialisation of the patterns from the phantom definition file, segments the image, find the n-points 
         lines and then find the pattern and label the dots.
  \ingroup PlusLibPatternRecognition
*/

class FidPatternRecognition
{
	public:
    FidPatternRecognition();
		virtual ~FidPatternRecognition();

    /*! Read the configuration file from a vtk XML data element */
    PlusStatus        ReadConfiguration(vtkXMLDataElement* rootConfigElement);

    /*!
      Run pattern recognition on a tracked frame list. It only segments the tracked frames which were not already segmented
      \param trackedFrameList Tracked frame list to segment
      \param numberOfSuccessfullySegmentedImages Out parameter holding the number of segmented images in this call (it is only equals the number of all segmented images in the tracked frame if it was not segmented at all)
    */
    PlusStatus        RecognizePattern(vtkTrackedFrameList* trackedFrameList, int* numberOfSuccessfullySegmentedImages = NULL);

    /*! 
      Run pattern recognition on a tracked frame list.
      \param trackedFrameList Tracked frame list to segment
    */
    PlusStatus        RecognizePattern(TrackedFrame* trackedFrame);

    /*! 
      Run pattern recognition on a tracked frame list.
      \param trackedFrameList Tracked frame list to segment
      \param patternRecognitionResult The result element.
    */
		PlusStatus        RecognizePattern(TrackedFrame* trackedFrame, PatternRecognitionResult &patternRecognitionResult);

    /*! Draw dots for debug purpose */
    void              DrawDots(PixelType* image);

    /*! Draw the results on an image for debug purpose */
    void              DrawResults(PixelType* image);

    /*! Get the FidSegmentation element, this element handles the segmentation part of the algorithm */
    FidSegmentation*	GetFidSegmentation() { return	& m_FidSegmentation; };

    /*!  Get the FidLineFinder element, this element finds the n-points lines from the segmented dots */
    FidLineFinder*		GetFidLineFinder() { return & m_FidLineFinder; };

    /*! Get the FidLabeling element, his element finds the pattern from the detected n-points lines */
    FidLabeling* 		  GetFidLabeling() { return & m_FidLabeling; };

  protected:

    /*! Reads the phantom definition and computes the NWires intersection if needed */
	  PlusStatus        ReadPhantomDefinition(vtkXMLDataElement* rootConfigElement);

	protected:
    
		FidSegmentation		m_FidSegmentation;
		FidLineFinder			m_FidLineFinder;
		FidLabeling 			m_FidLabeling;

    int               m_CurrentFrame;

};

//-----------------------------------------------------------------------------

#endif //_FID_PATTERN_RECOGNITION_H
