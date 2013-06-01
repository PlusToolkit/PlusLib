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
int main(int argc, char **argv)
{
  int numberOfFailures(0); 

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  bool printHelp(false);
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  std::string inputSequenceMetafile("");   
  std::vector<int> clipRectOrigin;
  std::vector<int> clipRectSize;
  std::string inputBaselineFileName(""); 
  bool saveImages(false);

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

  if ( lineSegmenter->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get line positions from video frames");
    return PLUS_FAIL;
  }
  std::vector<vtkLineSegmentationAlgo::LineParameters> lineParameters;
  lineSegmenter->GetDetectedLineParameters(lineParameters);

  //*********************************************************************
  // Save results to file

  std::string resultSaveFilename=vtkPlusConfig::GetInstance()->GetOutputPath("LineSegmentationResults.xml");
  LOG_INFO("Save calibration results to XML file: "<<resultSaveFilename); 
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

  //*********************************************************************
  // Compare result to baseline

  if (!inputBaselineFileName.empty())
  {
    LOG_INFO("Comparing result with baseline..."); 

    vtkSmartPointer<vtkXMLDataElement> resultsElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()));      
    if ( resultsElem == NULL )
    {
      LOG_ERROR("Failed to read baseline file: "<< inputBaselineFileName);
      exit(EXIT_FAILURE);
    }
    if (resultsElem->GetName()==NULL || STRCASECMP( resultsElem->GetName(), "LineSegmentationResults" ) != 0)
    {
      LOG_ERROR("Unable to find LineSegmentationResults XML data element in baseline: " << inputBaselineFileName); 
      exit(EXIT_FAILURE);
    }

    for ( int childElemIndex = 0; childElemIndex < resultsElem->GetNumberOfNestedElements(); ++childElemIndex)
    {
      vtkXMLDataElement* baselineFrameElem = resultsElem->GetNestedElement(childElemIndex); 
      if ( baselineFrameElem==NULL || baselineFrameElem->GetName()==NULL || STRCASECMP( baselineFrameElem->GetName(), "Frame" ) != 0 )
      {
        LOG_ERROR("Invalid child element in LineSegmentationResults: #"<<childElemIndex);
        ++numberOfFailures;
        continue;
      }

      int baselineFrameIndex=0;
      if ( !baselineFrameElem->GetScalarAttribute( "Index", baselineFrameIndex) )
      {
        LOG_ERROR("Unable to find Index element in LineSegmentationResults: child index "<<childElemIndex);
        numberOfFailures++; 
        continue;
      }

      const char* baselineSegmentationStatusString = baselineFrameElem->GetAttribute("SegmentationStatus");  
      if (baselineSegmentationStatusString ==NULL)
      {
        LOG_ERROR("SegmentationStatus is not available in the baseline for frame "<<baselineFrameIndex);
        numberOfFailures++; 
        continue;
      }
      bool baselineSegmentationStatus=!STRCASECMP(baselineSegmentationStatusString, "OK");
      if (baselineSegmentationStatus!=lineParameters[baselineFrameIndex].lineDetected)
      {
        LOG_ERROR("SegmentationStatus mismatch in Frame #" << baselineFrameIndex 
          << ": current=" << lineParameters[baselineFrameIndex].lineDetected 
          << ", baseline=" << baselineSegmentationStatus);
        ++numberOfFailures;
        continue;
      }

      if (!baselineSegmentationStatus)
      {
        // no segmentation data
        continue;
      }

      // Compare origin to baseline 
      double baselineLineOriginPoint_Image[2]={0,0};
      if ( !baselineFrameElem->GetVectorAttribute( "LineOriginPointPx", 2, baselineLineOriginPoint_Image) )
      {
        LOG_ERROR("Unable to find LineOriginPointPx element in LineSegmentationResults: frame "<<baselineFrameIndex);
        numberOfFailures++; 
      }
      else
      {

        double baselineOrigin3d[3]={baselineLineOriginPoint_Image[0],baselineLineOriginPoint_Image[1],0};
        double currentLinePoint1[3]={lineParameters[baselineFrameIndex].lineOriginPoint_Image[0],lineParameters[baselineFrameIndex].lineOriginPoint_Image[1],0};
        const double lineLen=50; // any positive number would do, just pick a second point along the line at this distance
        double currentLinePoint2[3]={currentLinePoint1[0]+lineParameters[baselineFrameIndex].lineDirectionVector_Image[0]*lineLen,
          currentLinePoint1[1]+lineParameters[baselineFrameIndex].lineDirectionVector_Image[1]*lineLen,0};
        double dist=PlusMath::ComputeDistanceLinePoint(currentLinePoint1, currentLinePoint2, baselineOrigin3d);
        if (dist>MAX_ORIGIN_DISTANCE_PIXEL)
        {
          LOG_ERROR("LineOriginPointPx differ from baseline for frame "<<baselineFrameIndex<<": distance="<<dist<<" pixels "
            <<"current=(" << std::fixed << lineParameters[baselineFrameIndex].lineOriginPoint_Image[0] << ", " << lineParameters[baselineFrameIndex].lineOriginPoint_Image[1] << ") "
            <<"baseline=(" << baselineLineOriginPoint_Image[0] << ", " << baselineLineOriginPoint_Image[1] << ").");
          numberOfFailures++;
        }
      }

      // Compare direction to baseline 
      double baselineLineDirectionVector_Image[2]={0,1};
      if ( !baselineFrameElem->GetVectorAttribute( "LineDirectionVectorPx", 2, baselineLineDirectionVector_Image) )
      {
        LOG_ERROR("Unable to find LineDirectionVectorPx element in LineSegmentationResults: frame "<<baselineFrameIndex);
        numberOfFailures++; 
      }
      else
      {
        double baselineVec3d[3]={baselineLineDirectionVector_Image[0],baselineLineDirectionVector_Image[1],0};
        double currentVec3d[3]={lineParameters[baselineFrameIndex].lineDirectionVector_Image[0],lineParameters[baselineFrameIndex].lineDirectionVector_Image[1],0};
        double angleDeg = acos(vtkMath::Dot(baselineVec3d,currentVec3d));
        if (angleDeg>MAX_LINE_ANGLE_DIFFERENCE_DEG)
        {
          LOG_ERROR("LineDirectionVectorPx differ from baseline for frame "<<baselineFrameIndex<<": angle difference is "<<angleDeg<<" deg, vector coordinates: "
            <<"current=(" << std::fixed << lineParameters[baselineFrameIndex].lineDirectionVector_Image[0] << ", " << lineParameters[baselineFrameIndex].lineDirectionVector_Image[1] << ") "
            <<"baseline=(" << baselineLineDirectionVector_Image[0] << ", " << baselineLineDirectionVector_Image[1] << ").");
          numberOfFailures++;
        }
      }

    }

    if ( numberOfFailures > 0 ) 
    {
      LOG_INFO("Test failed!"); 
      return EXIT_FAILURE; 
    }
  }

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS; 
}
