#ifndef __USFIDSEGRESULTFILE_H
#define __USFIDSEGRESULTFILE_H

#include "PlusConfigure.h"
#include <iostream>

class SegmentationParameters;
class SegmentationResults;

class UsFidSegResultFile
{
public:
	static void WriteSegmentationResultsHeader(std::ostream &outFile);
	static void WriteSegmentationResultsParameters(std::ostream &outFile, SegmentationParameters &params, const std::string &trueFidFileName);
	static void WriteSegmentationResultsStats(std::ostream &outFile, double meanFid, double meanFidCandidate=-1);
	static void WriteSegmentationResultsFooter(std::ostream &outFile);
	static void WriteSegmentationResults(std::ostream &outFile, SegmentationResults &segResults, const std::string &inputTestcaseName, int currentFrameIndex, const std::string &inputImageSequenceFileName);

	static const char *TEST_RESULTS_ELEMENT_NAME;
	static const char *TEST_CASE_ELEMENT_NAME;
	static const char *ID_ATTRIBUTE_NAME;
}; 

#endif //__USFIDSEGRESULTFILE_H
