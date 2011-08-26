#include "PlusConfigure.h"
#include <iostream>
#include <fstream> 
#include <strstream>
#include <limits>

#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtkSmartPointer.h"

#include "UsFidSegResultFile.h"

static const int FIDUCIAL_COUNT_PER_SLICE=7;
static const int MAX_FIDUCIAL_COUNT=50;

int main(int argc, char **argv)
{
	std::string inputXmlFileName;
	std::string outputFcsvFileName;

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_WARNING;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputXmlFileName, "Input XML file name");
	args.AddArgument("--output", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFcsvFileName, "Output FCSV file name");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");		
	
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
	
	if (inputXmlFileName.empty() || outputFcsvFileName.empty())
	{
		std::cerr << "input and output file names are required" << std::endl;
		exit(EXIT_FAILURE);
	}

	/////////////////

	std::ofstream of; 
	of.open( outputFcsvFileName.c_str());

	vtkXMLDataElement* rootElem = vtkXMLUtilities::ReadElementFromFile(inputXmlFileName.c_str());	
	// check to make sure we have the right element
	if (rootElem == NULL )
	{
		LOG_ERROR("Reading XML data file failed: " << inputXmlFileName);
		exit(EXIT_FAILURE);
	}
	if (strcmp(rootElem->GetName(), UsFidSegResultFile::TEST_RESULTS_ELEMENT_NAME) != 0)
	{
		LOG_ERROR("XML data file is invalid");
		exit(EXIT_FAILURE);
	}

	// Write header
	of << "# Fiducial List file " << outputFcsvFileName << std::endl;
	of << "# version = 2" << std::endl;
	of << "# name = UsFid" << std::endl;
	of << "# numPoints = 0" << std::endl;
	of << "# symbolScale = 5" << std::endl;
	of << "# symbolType = 12" << std::endl;
	of << "# visibility = 1" << std::endl;
	of << "# textScale = 4.5" << std::endl;
	of << "# color = 0.4,1,1" << std::endl;
	of << "# selectedColor = 1,0.5,0.5" << std::endl;
	of << "# opacity = 1" << std::endl;
	of << "# ambient = 0" << std::endl;
	of << "# diffuse = 1" << std::endl;
	of << "# specular = 0" << std::endl;
	of << "# power = 1" << std::endl;
	of << "# locked = 0" << std::endl;
	of << "# numberingScheme = 0" << std::endl;
	of << "# columns = label,x,y,z,sel,vis" << std::endl;

	// Write fiducial lines

	for (int nestedElemInd=0; nestedElemInd<rootElem->GetNumberOfNestedElements(); nestedElemInd++)
	{
		vtkSmartPointer<vtkXMLDataElement> currentElem=rootElem->GetNestedElement(nestedElemInd); 
		if (currentElem==NULL)  
		{
			LOG_ERROR("Invalid current data element");
			exit(EXIT_FAILURE);
		}
		if (strcmp(currentElem->GetName(),UsFidSegResultFile::TEST_CASE_ELEMENT_NAME)!=0)
		{ 
			// ignore all non-test-case elements
			continue;	
		}

		vtkSmartPointer<vtkXMLDataElement> inputElem=currentElem->FindNestedElementWithName("Input");
		int frameIndex=0;
		inputElem->GetScalarAttribute("ImageSeqFrameIndex",frameIndex);

		vtkSmartPointer<vtkXMLDataElement> outputElem=currentElem->FindNestedElementWithName("Output");
		
		const int MAX_FIDUCIAL_COORDINATE_COUNT=MAX_FIDUCIAL_COUNT*2;
		double fiducialPoints[MAX_FIDUCIAL_COORDINATE_COUNT]; // baseline fiducial Points
		memset(fiducialPoints,0,sizeof(fiducialPoints[0] * MAX_FIDUCIAL_COORDINATE_COUNT));
		int fidPointsRead = outputElem->GetVectorAttribute("SegmentationPoints", MAX_FIDUCIAL_COORDINATE_COUNT, fiducialPoints);

		if (fidPointsRead %2 != 0)
		{
			LOG_ERROR("Unpaired baseline fiducial coordinates");
			exit(EXIT_FAILURE);
		}

		if (fidPointsRead >MAX_FIDUCIAL_COORDINATE_COUNT)
		{
			LOG_WARNING("Too many baseline fiducials");
		}

		struct FiducialInfo
		{
			std::string label;
			double x;
			double y;
			double z;
			int sel;
			int vis;
		};

		std::vector<FiducialInfo> fids(FIDUCIAL_COUNT_PER_SLICE);
		for (int i=0; i<FIDUCIAL_COUNT_PER_SLICE; i++)
		{

			fids[i].label="test";
			fids[i].x=0;
			fids[i].y=0;
			fids[i].z=frameIndex;
			fids[i].sel=0; 
			fids[i].vis=0;
		}
		
		const int FID_COUNT_TAB2_5_POINT=5;
		const int FID_COUNT_TAB2_6_POINT=6; 
		int fidIndices[FID_COUNT_TAB2_5_POINT]={0,/* skip 1 */ 2, 3, 4, /* skip 5 */ 6};

		if (fidPointsRead==FID_COUNT_TAB2_5_POINT*2) // 5 fiducials are found, we assume that the 2nd and the 6th are not found
		{
			
			for(int f=0; f<FID_COUNT_TAB2_5_POINT; ++f)
			{
				// X and Y coordinates should be inverted, because FCSV (Slicer) assumes RAS coordinate system, while
				// itk images use LPS coordinate system
				double x=-fiducialPoints[f*2+0];
				double y=-fiducialPoints[f*2+1];
				fids[fidIndices[f]].x=x;
				fids[fidIndices[f]].y=y;
				fids[fidIndices[f]].sel=1; 
				fids[fidIndices[f]].vis=1;

			}
		}
		else // found no fiducials
		{
			
			for(int f=0; f<FID_COUNT_TAB2_5_POINT; ++f)
			{
				// X and Y coordinates should be inverted, because FCSV (Slicer) assumes RAS coordinate system, while
				// itk images use LPS coordinate system
				
				fids[fidIndices[f]].x=0;
				fids[fidIndices[f]].y=0;
				fids[fidIndices[f]].sel=1; 
				fids[fidIndices[f]].vis=0;

			}
		
		}

		 if (fidPointsRead==FID_COUNT_TAB2_6_POINT*2)
		{

			for(int fidIndex=0; fidIndex<FID_COUNT_TAB2_6_POINT; ++fidIndex)
			{
				int fidIndices[FID_COUNT_TAB2_6_POINT]={0, 1 , 2, 3, 4, 5 };
				// X and Y coordinates should be inverted, because FCSV (Slicer) assumes RAS coordinate system, while
				// itk images use LPS coordinate system
				double x=-fiducialPoints[fidIndex*2+0];
				double y=-fiducialPoints[fidIndex*2+1];
				fids[fidIndices[fidIndex]].x=x;
				fids[fidIndices[fidIndex]].y=y;
				fids[fidIndices[fidIndex]].vis=1;
			} 

		}

		for (int i=0; i<FIDUCIAL_COUNT_PER_SLICE; i++)
		{
			of << fids[i].label << "," << fids[i].x << "," << fids[i].y << "," << fids[i].z << "," << fids[i].sel << "," << fids[i].vis << std::endl;
		}
	
	}
	
	if ( rootElem != NULL ) 
	{
		rootElem->Delete(); 
	}

	LOG_DEBUG("Done!"); 
		
 	return EXIT_SUCCESS;
}
