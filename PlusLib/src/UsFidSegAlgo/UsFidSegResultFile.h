/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __USFIDSEGRESULTFILE_H
#define __USFIDSEGRESULTFILE_H

#include "PlusConfigure.h"
#include "FidPatternRecognition.h"
#include <iostream>

/*!
  \class UsFidSegResultFile
  \brief This class can write fiducial segmentation results to an XML file
  \ingroup PlusLibPatternRecognition
*/ 
class UsFidSegResultFile
{
public:
	static void WriteSegmentationResultsHeader(std::ostream &outFile);
	static void WriteSegmentationResultsParameters(std::ostream &outFile, FidPatternRecognition &patternRcognitionObject, const std::string &trueFidFileName);
	static void WriteSegmentationResultsStats(std::ostream &outFile, double meanFid, double meanFidCandidate=-1);
	static void WriteSegmentationResultsFooter(std::ostream &outFile);
	static void WriteSegmentationResults(std::ostream &outFile, PatternRecognitionResult &segResults, const std::string &inputTestcaseName, int currentFrameIndex, const std::string &inputImageSequenceFileName);

	static const char *TEST_RESULTS_ELEMENT_NAME;
	static const char *TEST_CASE_ELEMENT_NAME;
	static const char *ID_ATTRIBUTE_NAME;
}; 

#endif //__USFIDSEGRESULTFILE_H
