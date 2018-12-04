/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _FID_PATTERN_RECOGNITION_H
#define _FID_PATTERN_RECOGNITION_H

#include "PlusFidPatternRecognitionCommon.h"
#include "PlusFidSegmentation.h"
#include "PlusFidLineFinder.h"
#include "PlusFidLabeling.h"

#include "vtkXMLDataElement.h"

//class igsioTrackedFrame; 
//class vtkIGSIOTrackedFrameList;

/*!
\class FidPatternRecognition
\brief This class manages the whole pattern recognition algorithm. From a vtk XML data element it handles
the initialization of the patterns from the phantom definition file, segments the image, find the n-points
lines and then find the pattern and label the dots.
\ingroup PlusLibPatternRecognition
*/

class vtkPlusCalibrationExport PlusFidPatternRecognition
{

public:
  enum PatternRecognitionError
  {
    PATTERN_RECOGNITION_ERROR_NO_ERROR,
    PATTERN_RECOGNITION_ERROR_UNKNOWN,
    PATTERN_RECOGNITION_ERROR_TOO_MANY_CANDIDATES
  };

  PlusFidPatternRecognition();
  virtual ~PlusFidPatternRecognition();

  /*! Read the configuration file from a vtk XML data element */
  PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

  /*!
  Run pattern recognition on a tracked frame list.
  It only segments the tracked frames which were not already segmented
  \param trackedFrameList Tracked frame list to segment
  \param numberOfSuccessfullySegmentedImages Out parameter holding the number of segmented images in this call (it is only equals the number of all segmented images in the tracked frame if it was not segmented at all)
  \param segmentedFramesIndices Indices of the frames that were properly segmented
  */
  PlusStatus RecognizePattern(vtkIGSIOTrackedFrameList* trackedFrameList, PatternRecognitionError& patternRecognitionError, int* numberOfSuccessfullySegmentedImages = NULL, std::vector<unsigned int>* segmentedFramesIndices = NULL);

  /*!
  Run pattern recognition on a tracked frame list.
  \param trackedFrame image to segment
  \param patternRecognitionError returns detailed information about the success of the recognition
  \param frameIndex index of the current frame, only used for saving debug information (determine output image file name)
  */
  PlusStatus RecognizePattern(igsioTrackedFrame* trackedFrame, PatternRecognitionError& patternRecognitionError, unsigned int frameIndex);

  /*!
  Run pattern recognition on a tracked frame list.
  \param trackedFrame image to segment
  \param patternRecognitionResult object to store the pattern recognition result
  \param patternRecognitionError returns detailed information about the success of the recognition
  \param frameIndex index of the current frame, only used for saving debug information (determine output image file name)
  */
  PlusStatus RecognizePattern(igsioTrackedFrame* trackedFrame, PlusPatternRecognitionResult& patternRecognitionResult, PatternRecognitionError& patternRecognitionError, unsigned int frameIndex);

  /*! Draw dots for debug purpose */
  void DrawDots(PlusFidSegmentation::PixelType* image);

  /*! Draw the results on an image for debug purpose */
  void DrawResults(PlusFidSegmentation::PixelType* image);

  /*! Get the FidSegmentation element, this element handles the segmentation part of the algorithm */
  PlusFidSegmentation* GetFidSegmentation() { return  & m_FidSegmentation; };

  /*!  Get the FidLineFinder element, this element finds the n-points lines from the segmented dots */
  PlusFidLineFinder* GetFidLineFinder() { return & m_FidLineFinder; };

  /*! Get the FidLabeling element, his element finds the pattern from the detected n-points lines */
  PlusFidLabeling* GetFidLabeling() { return & m_FidLabeling; };

  /*! Get the pattern structure vector, this defines the patterns that the algorithm finds */
  std::vector<PlusFidPattern*>& GetPatterns() { return m_Patterns; };

  /*! Set the maximum tolerance on the line length in Mm */
  void SetMaxLineLengthToleranceMm(double value);

  /*! Set the maximum number of candidates to consider */
  void SetNumberOfMaximumFiducialPointCandidates(int aMax);

  /*! Reads the phantom definition and computes the NWires intersection if needed */
  PlusStatus ReadPhantomDefinition(vtkXMLDataElement* rootConfigElement);

protected:

  PlusFidSegmentation           m_FidSegmentation;
  PlusFidLineFinder             m_FidLineFinder;
  PlusFidLabeling               m_FidLabeling;
  std::vector<PlusFidPattern*>  m_Patterns;

  double                        m_MaxLineLengthToleranceMm;
};

//-----------------------------------------------------------------------------

#endif //_FID_PATTERN_RECOGNITION_H
