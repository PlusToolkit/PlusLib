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

#include "vtkXMLDataElement.h"

class TrackedFrame; 
class vtkTrackedFrameList; 

/*!
\class FidPatternRecognition
\brief This class manages the whole pattern recognition algorithm. From a vtk XML data element it handles
the initialization of the patterns from the phantom definition file, segments the image, find the n-points 
lines and then find the pattern and label the dots.
\ingroup PlusLibPatternRecognition
*/

class FidPatternRecognition
{

public:
  FidPatternRecognition();
  virtual ~FidPatternRecognition();

  /*! Read the configuration file from a vtk XML data element */
  PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

   /*!
  Run pattern recognition on a tracked frame list.
  It only segments the tracked frames which were not already segmented
  \param trackedFrameList Tracked frame list to segment
  \param numberOfSuccessfullySegmentedImages Out parameter holding the number of segmented images in this call (it is only equals the number of all segmented images in the tracked frame if it was not segmented at all)
  \param segmentedFramesIndices Indices of the frames that were properly segmented
  */
  PlusStatus RecognizePattern(vtkTrackedFrameList* trackedFrameList, PatternRecognitionError &patternRecognitionError, int* numberOfSuccessfullySegmentedImages = NULL, std::vector<int> *segmentedFramesIndices = NULL );
 
  /*! 
  Run pattern recognition on a tracked frame list.
  \param trackedFrame image to segment
  */
  PlusStatus RecognizePattern(TrackedFrame* trackedFrame, PatternRecognitionError &patternRecognitionError);

  /*! 
  Run pattern recognition on a tracked frame list.
  \param trackedFrame image to segment
  \param patternRecognitionResult object to store the pattern recognition result
  */
  PlusStatus RecognizePattern(TrackedFrame* trackedFrame, PatternRecognitionResult &patternRecognitionResult, PatternRecognitionError &patternRecognitionError);

  /*! Draw dots for debug purpose */
  void DrawDots(PixelType* image);

  /*! Draw the results on an image for debug purpose */
  void DrawResults(PixelType* image);

  /*! Get the FidSegmentation element, this element handles the segmentation part of the algorithm */
  FidSegmentation*	GetFidSegmentation() { return	& m_FidSegmentation; };

  /*!  Get the FidLineFinder element, this element finds the n-points lines from the segmented dots */
  FidLineFinder* GetFidLineFinder() { return & m_FidLineFinder; };

  /*! Get the FidLabeling element, his element finds the pattern from the detected n-points lines */
  FidLabeling* GetFidLabeling() { return & m_FidLabeling; };

  /*! Set the maximum tolerance on the line length in Mm */
  void SetMaxLineLengthToleranceMm(double value);

  /*! Set the maximum number of candidates to consider */
  void SetNumberOfMaximumFiducialPointCandidates(int aMax);

  /*! Reads the phantom definition and computes the NWires intersection if needed */
  PlusStatus        ReadPhantomDefinition(vtkXMLDataElement* rootConfigElement);

protected:

  FidSegmentation         m_FidSegmentation;
  FidLineFinder           m_FidLineFinder;
  FidLabeling             m_FidLabeling;

  double                  m_MaxLineLengthToleranceMm;

  int                     m_CurrentFrame;
};

//-----------------------------------------------------------------------------

#endif //_FID_PATTERN_RECOGNITION_H
