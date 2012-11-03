/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsSimulatorAlgo.h"
#include "vtkImageAlgorithm.h"
#include "vtkInformation.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkSmartPointer.h"
#include "vtkImageStencil.h"
#include "vtkPolyDataNormals.h"
#include "vtkTriangleFilter.h"
#include "vtkStripper.h"
#include "vtkInformationVector.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkImageStencilData.h"
#include "vtkPolyData.h"
#include "vtkSTLReader.h"
#include "vtkXMLPolyDataReader.h"
#include "vtksys/SystemTools.hxx"
#include "vtkRfProcessor.h"
#include "vtkUsScanConvert.h"

#include "vtkModifiedBSPTree.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkUsSimulatorAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkUsSimulatorAlgo);

//-----------------------------------------------------------------------------
vtkUsSimulatorAlgo::vtkUsSimulatorAlgo()
{
  SetNumberOfInputPorts(1);
  SetNumberOfOutputPorts(1);

  this->BackgroundValue = 155;
  this->ModelToImageMatrix = NULL;

  this->ModelFileName = NULL;
  this->ImageCoordinateFrame = NULL;
  this->ReferenceCoordinateFrame = NULL;

  this->ModelLocalizer=vtkModifiedBSPTree::New();

  this->NumberOfScanlines=256;
  this->NumberOfSamplesPerScanline=1000;

  this->RfProcessor=vtkRfProcessor::New();
}

//-----------------------------------------------------------------------------
vtkUsSimulatorAlgo::~vtkUsSimulatorAlgo()
{
  SetModelToImageMatrix(NULL); 
  SetModelFileName(NULL); 
  SetImageCoordinateFrame(NULL); 
  SetReferenceCoordinateFrame(NULL); 
  if (this->RfProcessor!=NULL)
  {
    this->RfProcessor->Delete();
    this->RfProcessor=NULL;
  }
}

//-----------------------------------------------------------------------------
void vtkUsSimulatorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
int vtkUsSimulatorAlgo::FillInputPortInformation(int, vtkInformation * info)
{
  info->Set(vtkImageAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData"); 
  return 1; 
}

//-----------------------------------------------------------------------------
int vtkUsSimulatorAlgo::FillOutputPortInformation(int, vtkInformation * info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData"); 

  return 1; 
}

//-----------------------------------------------------------------------------
int vtkUsSimulatorAlgo::RequestData(vtkInformation* request,vtkInformationVector** inputVector,vtkInformationVector* outputVector)
{
  if (this->ModelFileName == NULL)
  {
    LOG_ERROR("ModelFileName is not specified in vtkUsSimulatorAlgo element of the configuration nor is set as input!");
    return PLUS_FAIL;     
  }

  if (this->ModelToImageMatrix == NULL)
  {
    LOG_ERROR("No Model to US image transform specified " ); 
    return 1; 
  }

  // Get input
  vtkInformation* inInfoPort = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0); 

  vtkPolyData* modelModel = vtkPolyData::SafeDownCast(inInfoPort->Get(vtkDataObject::DATA_OBJECT()));
  if (modelModel == NULL)
  {
    LOG_ERROR("Model specified is empty");
    return 1; 
  }

  vtkSmartPointer<vtkImageData> scanLines = vtkSmartPointer<vtkImageData>::New(); // image data containing the scanlines in rows (FM orientation)
  scanLines->SetExtent(0,this->NumberOfSamplesPerScanline-1,0,this->NumberOfScanlines-1,0,0);
  scanLines->SetScalarTypeToUnsignedChar();
  scanLines->AllocateScalars();

  vtkSmartPointer<vtkMatrix4x4> imageToModelMatrix= vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(this->ModelToImageMatrix, imageToModelMatrix); 

  // Create BSPTree for fast scanline-model intersection computation
  this->ModelLocalizer->SetDataSet(modelModel); 
  this->ModelLocalizer->SetMaxLevel(24); 
  this->ModelLocalizer->SetNumberOfCellsPerNode(32);
  this->ModelLocalizer->BuildLocator(); 
  vtkSmartPointer<vtkPoints> scanLineIntersectionWithModel = vtkSmartPointer<vtkPoints>::New(); 

  vtkUsScanConvert* scanConverter=this->RfProcessor->GetScanConverter();
  if (scanConverter==NULL)
  {
    LOG_ERROR("ScanConverter is not defined");
    return 1;
  }
  // The input image extent has to be set before calling the scanConverter's
  // GetScanLineEndPoints or GetDistanceBetweenScanlineSamplePointsMm methods
  scanConverter->SetInputImageExtent(scanLines->GetExtent());

  double outputImageSpacingMm[3]={1.0,1.0,1.0};
  scanConverter->GetOutputImageSpacing(outputImageSpacingMm);

  double distanceBetweenScanlineSamplePointsMm=scanConverter->GetDistanceBetweenScanlineSamplePointsMm();

  vtkSmartPointer<vtkPoints> intersectionPoints = vtkSmartPointer<vtkPoints>::New(); 
  for(int scanLineIndex=0;scanLineIndex<this->NumberOfScanlines; scanLineIndex++)
  {
    double scanLineStartPoint_Image[4] = {0,0,0,1}; // scanline start position in Image coordinate system
    double scanLineEndPoint_Image[4] = {0,0,0,1}; // scanline end position in Image coordinate system
    scanConverter->GetScanLineEndPoints(scanLineIndex, scanLineStartPoint_Image, scanLineEndPoint_Image);
    
    double scanLineStartPoint_Model[4] = {0,0,0,1};
    double scanLineEndPoint_Model[4] = {0,0,0,1};
    imageToModelMatrix->MultiplyPoint(scanLineStartPoint_Image,scanLineStartPoint_Model);
    imageToModelMatrix->MultiplyPoint(scanLineEndPoint_Image,scanLineEndPoint_Model);
    intersectionPoints->Reset();
    
    //const double startTime = vtkAccurateTimer::GetSystemTime(); 
    this->ModelLocalizer->IntersectWithLine(scanLineStartPoint_Model, scanLineEndPoint_Model,NULL,intersectionPoints,NULL);
    //const double stopTime = vtkAccurateTimer::GetSystemTime(); 
    //LOG_INFO("this->ModelLocalizer->IntersectWithLine: "<<(stopTime-startTime)*1000<<" msec");
    vtkIdType numIntersectionPoints = intersectionPoints->GetNumberOfPoints(); 

    double scanLineIntersectionPoint_Image[4] = {0,0,0,1}; 

    
    const unsigned char INSIDE_OBJECT_COLOUR = 20; 
    const unsigned char OUTSIDE_OBJECT_COLOUR = 155; 

    unsigned char pixelColour = OUTSIDE_OBJECT_COLOUR; // grey

    int currentPixelIndex=0;
    bool isInsideObject=false;
    int scanLineExtent[6]={0,this->NumberOfSamplesPerScanline-1,scanLineIndex,scanLineIndex,0,0};
    unsigned char* dstPixelAddress=(unsigned char*)scanLines->GetScalarPointerForExtent(scanLineExtent);
    for(vtkIdType intersectionIndex=0;intersectionIndex<=numIntersectionPoints; intersectionIndex++)
    {      
      // determine end of segment position and pixel color
      int endOfSegmentPixelIndex=currentPixelIndex;
      if(intersectionIndex<numIntersectionPoints)
      {
        double *intersectionPoint=intersectionPoints->GetPoint(intersectionIndex);
        double scanLineIntersectionPoint_Model[4]={intersectionPoint[0],intersectionPoint[1],intersectionPoint[2],1};
        this->ModelToImageMatrix->MultiplyPoint(scanLineIntersectionPoint_Model,scanLineIntersectionPoint_Image); 

        // Compute the distance between the scanline start point and the current intersection point to determine
        // the index of the intersection point in the scanline buffer
        double xCoordDiffMm=(scanLineIntersectionPoint_Image[0]-scanLineStartPoint_Image[0])*outputImageSpacingMm[0];
        double yCoordDiffMm=(scanLineIntersectionPoint_Image[1]-scanLineStartPoint_Image[1])*outputImageSpacingMm[1];
        double distanceOfIntersectionPointFromScanLineStartPointMm=sqrt(xCoordDiffMm*xCoordDiffMm+yCoordDiffMm*yCoordDiffMm);
        endOfSegmentPixelIndex=distanceOfIntersectionPointFromScanLineStartPointMm/distanceBetweenScanlineSamplePointsMm;
      }
      else
      {
        // last segment, after all the intersection points
        endOfSegmentPixelIndex=this->NumberOfSamplesPerScanline-1;
      }
      pixelColour=isInsideObject?INSIDE_OBJECT_COLOUR:OUTSIDE_OBJECT_COLOUR;

      //LOG_DEBUG("Segment from "<<currentPixelIndex<<" to "<<endOfSegmentPixelIndex);

      // fill the segment with const values
      int numberOfFilledPixels=endOfSegmentPixelIndex-currentPixelIndex+1;
      memset(dstPixelAddress,pixelColour,numberOfFilledPixels);
      dstPixelAddress+=numberOfFilledPixels;
      currentPixelIndex+=numberOfFilledPixels;
      isInsideObject=!isInsideObject;
    }
  }

  vtkImageData* simulatedUsImage = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())); 
  if (simulatedUsImage == NULL)
  {
    LOG_ERROR("vtkUsSimulatorAlgo output type is invalid");
    return 1; 
  }
  this->RfProcessor->SetRfFrame(scanLines, US_IMG_BRIGHTNESS);
  //simulatedUsImage->DeepCopy(scanLines); // TODO: check if it's enough or DeepCopy is needed  
  simulatedUsImage->DeepCopy(this->RfProcessor->GetBrightessScanConvertedImage()); // TODO: check if it's enough or DeepCopy is needed  

  return 1; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorAlgo::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkUsSimulatorVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Saved Data video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  // vtkUsSimulatorAlgo section
  vtkXMLDataElement* usSimulatorAlgoElement = config->FindNestedElementWithName("vtkUsSimulatorAlgo"); 

  if (usSimulatorAlgoElement == NULL)
  {
    LOG_ERROR("Unable to find vtkUsSimulatorAlgo element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Background value
  int backgroundValue = -1;
  if ( usSimulatorAlgoElement->GetScalarAttribute("BackgroundValue", backgroundValue)) 
  {
    this->SetBackgroundValue(backgroundValue); 
  }

  int numberOfScanlines = -1;
  if ( usSimulatorAlgoElement->GetScalarAttribute("NumberOfScanlines", numberOfScanlines )) 
  {
    this->SetNumberOfScanlines(numberOfScanlines); 
  }

  int numberOfSamplesPerScanline = -1;
  if ( usSimulatorAlgoElement->GetScalarAttribute("NumberOfSamplesPerScanline", numberOfSamplesPerScanline )) 
  {
    this->SetNumberOfSamplesPerScanline(numberOfSamplesPerScanline); 
  }

  // Model file name

  const char* modelFileName = usSimulatorAlgoElement->GetAttribute("ModelFileName");

  if(modelFileName)
  {
    std::string foundAbsoluteImagePath;
    if (vtkPlusConfig::GetAbsoluteImagePath(modelFileName, foundAbsoluteImagePath) == PLUS_SUCCESS)
    {
      if (LoadModel(foundAbsoluteImagePath)!=PLUS_SUCCESS)
      {
        return PLUS_FAIL;     
      }
    }

    else
    {
      LOG_ERROR("Cannot find input model file!");
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_ERROR("ModelFileName is not specified in vtkUsSimulatorAlgo element of the configuration"); 
    return PLUS_FAIL;
  }

  // Reference coordinate frame
  const char* imageCoordinateFrame = usSimulatorAlgoElement->GetAttribute("ImageCoordinateFrame");
  if (imageCoordinateFrame == NULL)
  {
    LOG_ERROR("ImageCoordinateFrame is not specified in vtkUsSimulatorAlgo element of the configuration");
    return PLUS_FAIL;     
  }
  this->SetImageCoordinateFrame(imageCoordinateFrame);

  // Reference coordinate frame
  const char* referenceCoordinateFrame = usSimulatorAlgoElement->GetAttribute("ReferenceCoordinateFrame");
  if (referenceCoordinateFrame == NULL)
  {
    LOG_ERROR("ReferenceCoordinateFrame is not specified in vtkUsSimulatorAlgo element of the configuration");
    return PLUS_FAIL;     
  }
  this->SetReferenceCoordinateFrame(referenceCoordinateFrame);

  this->RfProcessor->ReadConfiguration(usSimulatorAlgoElement);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorAlgo::LoadModel(std::string absoluteImagePath)
{
  this->SetModelFileName(absoluteImagePath.c_str());
  // Load Model

  std::string fileExt=vtksys::SystemTools::GetFilenameLastExtension(this->ModelFileName);
  vtkSmartPointer<vtkPolyData> model; 

  if (STRCASECMP(fileExt.c_str(),".stl")==0)
  {  
    vtkSmartPointer<vtkSTLReader> modelReader = vtkSmartPointer<vtkSTLReader>::New();
    modelReader->SetFileName(this->ModelFileName);
    modelReader->Update();
    model = modelReader->GetOutput();
  }
  else //if (STRCASECMP(fileExt.c_str(),".vtp")==0)
  {
    vtkSmartPointer<vtkXMLPolyDataReader> modelReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    modelReader->SetFileName(this->ModelFileName);
    modelReader->Update();
    model = modelReader->GetOutput();
  }

  if (model.GetPointer()==NULL || model->GetNumberOfPoints()==0)
  {
    LOG_ERROR("Model specified cannot be found: "<<absoluteImagePath);    
    return PLUS_FAIL;
  }

  this->SetInput(model);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorAlgo::GetFrameSize(int frameSize[2])
{
  vtkUsScanConvert* scanConverter=this->RfProcessor->GetScanConverter();
  if (scanConverter==NULL)
  {
    LOG_ERROR("Unable to determine frame size, ScanConverter is not defined");
    return PLUS_FAIL;
  }
  scanConverter->GetOutputImageSizePixel(frameSize);
  return PLUS_FAIL;
}
