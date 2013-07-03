/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file vtkCenterOfRotationCalibAlgoTest.cxx 
\brief This test computes center of rotation on a recorded data set and 
compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkMath.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtkTrackedFrameList.h"
#include "vtkLineSegmentationAlgo.h"
#include "PlusMath.h"

const double MAX_ORIGIN_DISTANCE_PIXEL=10;
const double MAX_LINE_ANGLE_DIFFERENCE_DEG=3;

//----------------------------------------------------------------------------
void WriteLineSegmentationResultsToFile(const std::string& resultSaveFilename, const std::vector<vtkLineSegmentationAlgo::LineParameters> &lineParameters)
{
  std::ofstream outFile; 
  outFile.open(resultSaveFilename.c_str());	
  outFile << "<LineSegmentationResults>" << std::endl;
  for ( unsigned int frameIndex = 0; frameIndex < lineParameters.size(); ++frameIndex )
  {
    outFile << "  <Frame Index=\"" <<frameIndex<<"\" SegmentationStatus=\"";
    if (lineParameters[frameIndex].lineDetected)
    {
      outFile << "OK\" "
        << std::fixed << std::setprecision(8)
        << "LineOriginPointPx=\""<<lineParameters[frameIndex].lineOriginPoint_Image[0]<<" "<<lineParameters[frameIndex].lineOriginPoint_Image[1]<<"\" "
        << "LineDirectionVectorPx=\""<<lineParameters[frameIndex].lineDirectionVector_Image[0]<<" "<<lineParameters[frameIndex].lineDirectionVector_Image[1]<<"\" ";
    }
    else
    {
      outFile << "Failed\"";
    }
    outFile << " />" << std::endl;      
  }
  outFile << "</LineSegmentationResults>" << std::endl;  
  outFile.close(); 
}

//----------------------------------------------------------------------------
PlusStatus ReadLineSegmentationResultsFromFile(const std::string& resultSaveFilename, std::vector<vtkLineSegmentationAlgo::LineParameters> &lineParameters)
{
  lineParameters.clear();
  if (resultSaveFilename.empty())
  {
    LOG_ERROR("Cannot read line segmentation results, filename is empty");
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkXMLDataElement> resultsElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(resultSaveFilename.c_str()));      
  if ( resultsElem == NULL )
  {
    LOG_ERROR("Failed to read baseline file: "<< resultSaveFilename);
    return PLUS_FAIL;
  }
  if (resultsElem->GetName()==NULL || STRCASECMP( resultsElem->GetName(), "LineSegmentationResults" ) != 0)
  {
    LOG_ERROR("Unable to find LineSegmentationResults XML data element in baseline: " << resultSaveFilename); 
    return PLUS_FAIL;
  }
  for ( int childElemIndex = 0; childElemIndex < resultsElem->GetNumberOfNestedElements(); ++childElemIndex)
  {
    vtkXMLDataElement* frameElem = resultsElem->GetNestedElement(childElemIndex); 
    if ( frameElem==NULL || frameElem->GetName()==NULL || STRCASECMP( frameElem->GetName(), "Frame" ) != 0 )
    {
      LOG_ERROR("Invalid child element in LineSegmentationResults: #"<<childElemIndex);
      return PLUS_FAIL;
    }
    int frameIndex=0;
    if ( !frameElem->GetScalarAttribute( "Index", frameIndex) )
    {
      LOG_ERROR("Unable to find Index element in LineSegmentationResults: child index "<<childElemIndex);
      return PLUS_FAIL;
    }

    vtkLineSegmentationAlgo::LineParameters currentLineParams;

    const char* baselineSegmentationStatusString = frameElem->GetAttribute("SegmentationStatus");  
    if (baselineSegmentationStatusString==NULL)
    {
      LOG_ERROR("SegmentationStatus is not available in the baseline for frame "<<frameIndex);
      return PLUS_FAIL;
    }
    currentLineParams.lineDetected=!STRCASECMP(baselineSegmentationStatusString, "OK");

    if (currentLineParams.lineDetected)
    {
      double baselineLineOriginPoint_Image[2]={0,0};
      if ( !frameElem->GetVectorAttribute( "LineOriginPointPx", 2, currentLineParams.lineOriginPoint_Image) )
      {
        LOG_ERROR("Unable to find LineOriginPointPx element in LineSegmentationResults for frame "<<frameIndex);
        return PLUS_FAIL;
      }
      if ( !frameElem->GetVectorAttribute( "LineDirectionVectorPx", 2, currentLineParams.lineDirectionVector_Image) )
      {
        LOG_ERROR("Unable to find LineDirectionVectorPx element in LineSegmentationResults for frame "<<frameIndex);
        return PLUS_FAIL;
      }
    }
    else
    {
      currentLineParams.lineOriginPoint_Image[0]=0;
      currentLineParams.lineOriginPoint_Image[1]=0;
      currentLineParams.lineDirectionVector_Image[0]=0;
      currentLineParams.lineDirectionVector_Image[1]=1;
    }

    if (frameIndex>=lineParameters.size())
    {
      // expand the results array to be able to store the results
      vtkLineSegmentationAlgo::LineParameters nonDetectedLineParams;
      nonDetectedLineParams.lineDetected=false;
      nonDetectedLineParams.lineOriginPoint_Image[0]=0;
      nonDetectedLineParams.lineOriginPoint_Image[1]=0;
      nonDetectedLineParams.lineDirectionVector_Image[0]=0;
      nonDetectedLineParams.lineDirectionVector_Image[1]=1;
      lineParameters.insert( lineParameters.end(), frameIndex-lineParameters.size()+1, nonDetectedLineParams);
    }
    lineParameters[frameIndex]=currentLineParams;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int CompareLineSegmentationResults(const std::vector<vtkLineSegmentationAlgo::LineParameters> &lineParameters, const std::vector<vtkLineSegmentationAlgo::LineParameters> &baselineLineParameters)
{
  int numberOfFailures=0;

  for ( int frameIndex = 0; frameIndex < lineParameters.size(); ++frameIndex)
  {
    LOG_DEBUG("Comparing frame "<<frameIndex);
    if ( frameIndex>=baselineLineParameters.size() )
    {
      LOG_ERROR("Unable to find frame "<<frameIndex<<" in LineSegmentationResults baseline");
      numberOfFailures++;
      continue;
    }

    vtkLineSegmentationAlgo::LineParameters currentParam=lineParameters[frameIndex];
    vtkLineSegmentationAlgo::LineParameters baselineParam=baselineLineParameters[frameIndex];

    if (currentParam.lineDetected!=baselineParam.lineDetected)
    {
      LOG_ERROR("SegmentationStatus mismatch in Frame #" << frameIndex << ": current=" << currentParam.lineDetected << ", baseline=" << baselineParam.lineDetected);
      ++numberOfFailures;
      continue;
    }

    LOG_DEBUG(" Line detection status: "<<(currentParam.lineDetected?"detected":"not detected"));
    if (!currentParam.lineDetected)
    {
      // no segmentation data
      continue;
    }

    // Compare origin to baseline 
    double baselineOrigin3d[3]={baselineParam.lineOriginPoint_Image[0],baselineParam.lineOriginPoint_Image[1],0};
    double currentLinePoint1[3]={currentParam.lineOriginPoint_Image[0],currentParam.lineOriginPoint_Image[1],0};
    const double lineLen=50; // pick a second point along the line at this distance, any positive number would do
    double currentLinePoint2[3]={currentLinePoint1[0]+currentParam.lineDirectionVector_Image[0]*lineLen,
      currentLinePoint1[1]+currentParam.lineDirectionVector_Image[1]*lineLen,0};
    double distanceOfBaselineOriginFromCurrentLinePx=PlusMath::ComputeDistanceLinePoint(currentLinePoint1, currentLinePoint2, baselineOrigin3d);
    if (distanceOfBaselineOriginFromCurrentLinePx>MAX_ORIGIN_DISTANCE_PIXEL)
    {
      LOG_ERROR("Line position mismatch in Frame #" << frameIndex << ": baseline origin point distance from current line is "<<distanceOfBaselineOriginFromCurrentLinePx<<" pixels");
      numberOfFailures++;
    }
    else
    {
      LOG_DEBUG(" Line distance: "<<distanceOfBaselineOriginFromCurrentLinePx<<" pixels");
    }

    // Compare direction to baseline 
    double baselineVec3d[3]={baselineParam.lineDirectionVector_Image[0],baselineParam.lineDirectionVector_Image[1],0};
    double currentVec3d[3]={currentParam.lineDirectionVector_Image[0],currentParam.lineDirectionVector_Image[1],0};
    double angleDeg = acos(vtkMath::Dot(baselineVec3d,currentVec3d));
    if (angleDeg>MAX_LINE_ANGLE_DIFFERENCE_DEG)
    {
      LOG_ERROR("Line angle mismatch in Frame #" << frameIndex << ": angle difference is "<<angleDeg<<" deg, vector coordinates: "
        <<"current=(" << std::fixed << currentParam.lineDirectionVector_Image[0] << ", " << currentParam.lineDirectionVector_Image[1] << ") "
        <<"baseline=(" << baselineParam.lineDirectionVector_Image[0] << ", " << baselineParam.lineDirectionVector_Image[1] << ").");
      numberOfFailures++;
    }
    else
    {
      LOG_DEBUG(" Line angle difference is "<<angleDeg<<" deg");
    }
  }

  return numberOfFailures;
}

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int numberOfFailures(0); 

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  bool printHelp=false;
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  std::string inputSequenceMetafile;
  std::vector<int> clipRectOrigin;
  std::vector<int> clipRectSize;
  std::string inputBaselineFileName;
  bool saveImages=false;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
  args.AddArgument("--seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Input sequence metafile name with path");	  
  args.AddArgument("--clip-rect-origin", vtksys::CommandLineArguments::MULTI_ARGUMENT, &clipRectOrigin, "Origin of the clipping rectangle");
  args.AddArgument("--clip-rect-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &clipRectSize, "Size of the clipping rectangle");
  args.AddArgument("--save-images", vtksys::CommandLineArguments::NO_ARGUMENT, &saveImages, "Save images with detected lines overlaid");	
  args.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Input xml baseline file name with path");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  if ( inputSequenceMetafile.empty() )
  {
    std::cerr << "--seq-file argument is required" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }  

  LOG_DEBUG("Read input sequence");
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafile); 
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkLineSegmentationAlgo> lineSegmenter = vtkSmartPointer<vtkLineSegmentationAlgo>::New(); 

  if (clipRectOrigin.size()>0 || clipRectSize.size()>0)
  {
    // clip rectangle specified
    if (clipRectOrigin.size()!=2 || clipRectSize.size()!=2)
    {
      LOG_ERROR("--clip-rect-origin and --clip-rect-size arguments shall contain exactly two values each");
      exit(EXIT_FAILURE);
    }
    int origin[2]={clipRectOrigin[0], clipRectOrigin[1]};
    int size[2]={clipRectSize[0], clipRectSize[1]};
    lineSegmenter->SetClipRectangle(origin, size);
  }

  lineSegmenter->SetTrackedFrameList(trackedFrameList);
  lineSegmenter->SetSaveIntermediateImages(saveImages);
  lineSegmenter->SetIntermediateFilesOutputDirectory(vtkPlusConfig::GetInstance()->GetOutputDirectory());

  LOG_DEBUG("Segment lines");
  if ( lineSegmenter->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get line positions from video frames");
    return PLUS_FAIL;
  }
  std::vector<vtkLineSegmentationAlgo::LineParameters> lineParameters;
  lineSegmenter->GetDetectedLineParameters(lineParameters);

  // Save results to file
  std::string resultSaveFilename=vtkPlusConfig::GetInstance()->GetOutputPath("LineSegmentationResults.xml");
  LOG_INFO("Save calibration results to XML file: "<<resultSaveFilename); 
  WriteLineSegmentationResultsToFile(resultSaveFilename, lineParameters);

  // Compare result to baseline
  if (!inputBaselineFileName.empty())
  {
    LOG_INFO("Comparing result with baseline..."); 
    std::vector<vtkLineSegmentationAlgo::LineParameters> baselineLineParameters;
    if (ReadLineSegmentationResultsFromFile(inputBaselineFileName, baselineLineParameters)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read baseline data file");
      exit(EXIT_FAILURE);
    }
    int numberOfFailures=CompareLineSegmentationResults(lineParameters, baselineLineParameters);
    if (numberOfFailures>0)
    {
      LOG_ERROR("Number of differences compared to baseline: "<<numberOfFailures<<". Test failed!");
      exit(EXIT_FAILURE);
    }
  }

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS; 
}
