/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusMath.h"
#include "igsioTrackedFrame.h"
#include "igsioVideoFrame.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPlusTransverseProcessEnhancer.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"

// VTK includes
#include <vtkImageAccumulate.h>
#include <vtkImageCast.h>
#include <vtkImageDilateErode3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageIslandRemoval2D.h>
#include <vtkImageSobel2D.h>
#include <vtkImageThreshold.h>
#include <vtkObjectFactory.h>

#include <cmath>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusTransverseProcessEnhancer);

//----------------------------------------------------------------------------
vtkPlusTransverseProcessEnhancer::vtkPlusTransverseProcessEnhancer() : vtkPlusBoneEnhancer()
{
}

//----------------------------------------------------------------------------
vtkPlusTransverseProcessEnhancer::~vtkPlusTransverseProcessEnhancer()
{
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// Takes a vtkSmartPointer<vtkImageData> with clearly defined possible bone segments as an
// argument and modifies it so the bone areas that are too close to the camera's edge are removed.
void vtkPlusTransverseProcessEnhancer::RemoveOffCameraBones(vtkSmartPointer<vtkImageData> inputImage)
{
  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  unsigned char* vOutput = 0;

  int distanceVerticalBuffer = 10;    // For a bone to be valid, it must be this distance from the transducer
  int distanceHorizontalBuffer = 20;  // For a bone to be valid, it must be this distance from the horizontal sides of the frame
  int boneMinSize = 10;               // Minimum bone size a bone must have to be valid
  std::vector<std::map<std::string, int> > boneAreas = this->BoneAreasInfo;

  int boneHalfLen;
  bool clearArea;
  bool foundBone;

  this->BoneAreasInfo.clear();

  for (int areaIndex = boneAreas.size() - 1; areaIndex >= 0; --areaIndex)
  {
    std::map<std::string, int> currentArea = boneAreas.at(areaIndex);

    clearArea = false;
    boneHalfLen = ((currentArea["yMax"] - currentArea["yMin"]) + 1) / 2;

    //check if the bone is to close too the scan's edge
    if (currentArea["yMax"] + distanceVerticalBuffer >= dims[1] - 1 || currentArea["yMin"] - distanceVerticalBuffer <= 0)
    {
      clearArea = true;
    }
    //check if given the size, the bone is too close to the scan's edge
    else if (boneHalfLen + currentArea["yMax"] >= dims[1] - 1 || (currentArea["yMin"] - 1) - boneHalfLen <= 0)
    {
      clearArea = true;
    }
    //check if the bone is too close/far from the transducer 
    else if (currentArea["depth"] < distanceHorizontalBuffer || currentArea["depth"] > dims[0] - distanceHorizontalBuffer)
    {
      clearArea = true;
    }
    //check if the bone is to small
    else if (currentArea["yMax"] - currentArea["yMin"] <= boneMinSize)
    {
      clearArea = true;
    }

    //If it does not meet the criteria, remove the bones in this area
    if (clearArea == true)
    {
      //search through the area where the pixels are known to be
      for (int y = currentArea["yMax"]; y >= currentArea["yMin"]; --y)
      {
        int x = currentArea["xMax"] - this->BonePushBackPx;
        foundBone = false;
        while (x >= currentArea["xMin"] - this->BonePushBackPx && x >= 0 && foundBone == false)
        {
          vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
          if (*vOutput != 0)
          {
            //remove all pixels in the outline
            *vOutput = 0;
            for (int removeBonex = std::max(0, x - (this->BoneOutlineDepthPx - 1)); removeBonex < x; ++removeBonex)
            {
              vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(removeBonex, y, 0));
              *vOutput = 0;
            }

            foundBone = true;
          }
          x--;
        }
      }
    }
    else
    {
      this->BoneAreasInfo.push_back(currentArea);
    }
  }
}

//----------------------------------------------------------------------------
// Takes an unmodified vtkSmartPointer<vtkImageData> of an ultrasound as its first argument, and a more
// enhanced version of said image, with clearly defined possible bone segments as the second argument.
// This function modifies the second argument so as to remove any bone segments that have a higher
// amount of bone potential in the areas next to it than there is within the areas themselves.
void vtkPlusTransverseProcessEnhancer::CompareShadowAreas(vtkSmartPointer<vtkImageData> originalImage, vtkSmartPointer<vtkImageData> inputImage)
{
  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  float vInput = 0;
  unsigned char* vOutput = 0;

  //Variables used for measuring the size and intensity sum for bone, above, and below areas
  int boneLen;
  int boneHalfLen;
  float boneArea;
  float aboveSum;
  float areaSum;
  float belowSum;

  float aboveAvgShadow; //Shadow intensity of the above area
  float areaAvgShadow;  //Shadow intensity of the area
  float belowAvgShadow; //Shadow intensity of the below area

  std::map<std::string, int> currentArea;
  std::vector<std::map<std::string, int> > boneAreas = this->BoneAreasInfo;

  bool foundBone;
  this->BoneAreasInfo.clear();

  for (int areaIndex = boneAreas.size() - 1; areaIndex >= 0; --areaIndex)
  {
    currentArea = boneAreas.at(areaIndex);

    aboveSum = 0;
    areaSum = 0;
    belowSum = 0;

    boneLen = (currentArea["yMax"] - currentArea["yMin"]) + 1;
    boneHalfLen = boneLen / 2;
    boneArea = boneLen * currentArea["depth"];

    //gather sum of shadow areas from above the area
    for (int y = currentArea["yMax"] + boneHalfLen; y > currentArea["yMax"]; --y)
    {
      for (int x = dims[0] - 1; x >= currentArea["depth"]; --x)
      {
        vInput = (originalImage->GetScalarComponentAsFloat(x, y, 0, 0));
        aboveSum += vInput;
      }
    }
    //gather sum of shadow areas from the area
    for (int y = currentArea["yMax"]; y >= currentArea["yMin"]; --y)
    {
      for (int x = dims[0] - 1; x >= currentArea["depth"]; --x)
      {
        vInput = (originalImage->GetScalarComponentAsFloat(x, y, 0, 0));
        areaSum += vInput;
      }
    }
    //gather sum of shadow areas from below the area
    for (int y = currentArea["yMin"] - boneHalfLen; y < currentArea["yMin"]; ++y)
    {
      for (int x = dims[0] - 1; x >= currentArea["depth"]; --x)
      {
        vInput = (originalImage->GetScalarComponentAsFloat(x, y, 0, 0));
        belowSum += vInput;
      }
    }

    //Calculate average shadow intensity
    aboveAvgShadow = aboveSum / (boneArea / 2);
    areaAvgShadow = areaSum / boneArea;
    belowAvgShadow = belowSum / (boneArea / 2);

    //If there is a higher amount of bones around it, remove the area
    if (aboveAvgShadow - areaAvgShadow <= areaAvgShadow / 2 || belowAvgShadow - areaAvgShadow <= areaAvgShadow / 2)
    {

      for (int y = currentArea["yMax"]; y >= currentArea["yMin"]; --y)
      {
        //search through the area where the pixels are known to be
        int x = currentArea["xMax"] - this->BonePushBackPx;
        foundBone = false;
        while (x >= currentArea["xMin"] - this->BonePushBackPx && x >= 0 && foundBone == false)
        {
          vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
          if (*vOutput != 0)
          {
            //remove all pixels in the outline
            *vOutput = 0;

            for (int removeBonex = std::max(0, x - (this->BoneOutlineDepthPx - 1)); removeBonex < x; ++removeBonex)
            {
              vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(removeBonex, y, 0));
              *vOutput = 0;
            }
            foundBone = true;
          }
          x--;
        }
      }
    }
    else
    {
      this->BoneAreasInfo.push_back(currentArea);
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTransverseProcessEnhancer::ProcessFrame(igsioTrackedFrame* inputFrame, igsioTrackedFrame* outputFrame)
{

  this->BoneAreasInfo.clear();

  vtkSmartPointer<vtkImageData> intermediateImage = vtkPlusBoneEnhancer::UnprocessedFrameToLinearImage(inputFrame);
  intermediateImage->DeepCopy(this->LinesImage);

  //Save this image so that it can be used for comparison with the output image
  vtkSmartPointer<vtkImageData> originalImage = vtkSmartPointer<vtkImageData>::New();
  originalImage->DeepCopy(intermediateImage);

  vtkPlusBoneEnhancer::RemoveNoise(intermediateImage);

  this->RemoveOffCameraBones(intermediateImage);
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_09PostFilters_2PostRemoveOffCamera", intermediateImage);
  }
  this->CompareShadowAreas(originalImage, intermediateImage);
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_09PostFilters_3PostCompareShadowAreas", intermediateImage);
  }
  vtkPlusBoneEnhancer::LinearToFanImage(intermediateImage, outputFrame);

  return PLUS_SUCCESS;
}
