#include "UsFidSegResultFile.h" 
#include "FidPatternRecognition.h"

//----------------------------------------------------------------------------

// Write segmentation results

const char *UsFidSegResultFile::TEST_RESULTS_ELEMENT_NAME="TestResults";
const char *UsFidSegResultFile::TEST_CASE_ELEMENT_NAME="TestCase";
const char *UsFidSegResultFile::ID_ATTRIBUTE_NAME="id"; // this must not be changed, as this is expected by vtkXMLParser

void UsFidSegResultFile::WriteSegmentationResultsHeader(std::ostream &outFile)
{
	outFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
	outFile << "<"<<TEST_RESULTS_ELEMENT_NAME<<">" << std::endl; 
}

void UsFidSegResultFile::WriteSegmentationResultsParameters(std::ostream &outFile, FidPatternRecognition &patternRecognition, const std::string &trueFidFileName)
{
	outFile << " <AlgorithmOptions SegmentationThreshold=\"" << patternRecognition.GetFidSegmentation()->GetThresholdImagePercent() <<  "\" ImportSegResultsFromFile=\"" << trueFidFileName.c_str() << "\" />" << std::endl;
}

void UsFidSegResultFile::WriteSegmentationResultsStats(std::ostream &outFile, double meanFid, double meanFidCandidate/*=-1*/)
{
	outFile << " <Statistics>" << std::endl;
	outFile << "  <Segmentation MeanFoundFiducialsCount=\"" << meanFid;
	if (meanFidCandidate>=0)
	{
		outFile << "\" MeanFiducialCandidateCount=\"" << meanFidCandidate;
	}
	outFile << "\" />" << std::endl;
	outFile << " </Statistics>" << std::endl;
}


void UsFidSegResultFile::WriteSegmentationResultsFooter(std::ostream &outFile)
{
	outFile << "</"<<TEST_RESULTS_ELEMENT_NAME<<">" << std::endl; 
}

/* Write segmentation results into XML:
<TestCase id="UsTestSeq001_000">
<Input ImageSeqFileName="UsTestSeq001.b8" ImageSeqFrameIndex="0" />
<Output SegmentationSuccess="1" 
SegmentationQualityInAngleScore="33.3"
SegmentationQualityInIntensityScore="44.3"
SegmentationPoints="12 12 23 23 34 34 45 45 56 56 67 67" />
</TestCase>
*/
void UsFidSegResultFile::WriteSegmentationResults(std::ostream &outFile, PatternRecognitionResult &segResults, const std::string &inputTestcaseName, int currentFrameIndex, const std::string &inputImageSequenceFileName)
{ 	
	LOG_DEBUG("Writing test case " << inputTestcaseName.c_str() << " frame " << currentFrameIndex);
	bool algoSuccessful=segResults.GetDotsFound() && (segResults.GetFoundDotsCoordinateValue().size()>0);

	outFile << "  <"<<TEST_CASE_ELEMENT_NAME<<" "<<ID_ATTRIBUTE_NAME<<"=\"" << inputTestcaseName.c_str() << "_" << currentFrameIndex << "\">" << std::endl;
	outFile << "    <Input ImageSeqFileName=\"" << inputImageSequenceFileName.c_str() << "\" ImageSeqFrameIndex=\"" << currentFrameIndex << "\" />" << std::endl;
	
	outFile << "    <Output SegmentationSuccess=\"" << algoSuccessful << "\"";
	if (algoSuccessful)
	{
		outFile << std::endl;
		if (segResults.GetAngles()>=0)
		{
			outFile << "      SegmentationQualityInAngleScore=\""<< segResults.GetAngles() <<"\"" << std::endl;
		}
		if (segResults.GetIntensity()>=0)
		{
			outFile << "      SegmentationQualityInIntensityScore=\"" << segResults.GetIntensity()<< "\""<< std::endl;
		}
		if (segResults.GetFoundDotsCoordinateValue().size()>0)
		{
			outFile << "      SegmentationPoints=\"";
			for (unsigned int pt=0; pt<segResults.GetFoundDotsCoordinateValue().size(); pt++)
			{
				if (pt>0)
				{
					outFile << " ";
				}
				// Print only the x, y componentes (z will be always 0).
				if (segResults.GetFoundDotsCoordinateValue()[pt].size()>=2)
				{
					outFile << segResults.GetFoundDotsCoordinateValue()[pt][0] 
						<< " " << segResults.GetFoundDotsCoordinateValue()[pt][1];
				}
				else
				{
					LOG_ERROR("Point "<<pt<<" has only "<<segResults.GetFoundDotsCoordinateValue()[pt].size()<<" components, while at least 2 is required");
				}
			}// end for

		} // end top level if 
		outFile << "\"";
	}
	outFile << " />" << std::endl;
	if (segResults.GetNumDots()>0)
		{

			outFile << "     <FiducialPointCandidates>"<<std::endl;
			for(int i = 0; i<segResults.GetNumDots(); i++)
			{ 
			
			outFile << "       <Point id=\"" << i << "\" Intensity =\""<< segResults.GetCandidateFidValues()[i].GetDotIntensity() << "\" Positon=\"" << segResults.GetCandidateFidValues()[i].GetX() 
				<< " " << segResults.GetCandidateFidValues()[i].GetY() << "\" />" << std::endl; 
			} 
			outFile << "       </FiducialPointCandidates>"<<std::endl; 


		}
	
	outFile << "  </"<<TEST_CASE_ELEMENT_NAME<<">" << std::endl;
}

