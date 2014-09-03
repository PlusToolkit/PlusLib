/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkFanAngleDetectorAlgo.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkFanAngleDetectorAlgo);

//----------------------------------------------------------------------------
vtkFanAngleDetectorAlgo::vtkFanAngleDetectorAlgo()
{
  this->Image=NULL;

  this->MaxFanAnglesDeg[0]=0.0;
  this->MaxFanAnglesDeg[1]=0.0;
  this->DetectedFanAnglesDeg[0]=0.0;
  this->DetectedFanAnglesDeg[1]=0.0;
  this->IsFrameEmpty=true;

  this->ClipRectangleOrigin[0] = 0;
  this->ClipRectangleOrigin[1] = 0;
  this->ClipRectangleSize[0] = 0;
  this->ClipRectangleSize[1] = 0;

  this->FanOrigin[0] = 0.0;
  this->FanOrigin[1] = 0.0;
  this->FanRadiusStart = 0.0;
  this->FanRadiusStop = 500.0;

  this->FilterRadiusPixel = 10;
  this->BrightnessThreshold = 30;
  this->FanAngleMarginDeg = -3;

  this->EvaluatedDepthsRadiusPercentage.push_back(15);
  this->EvaluatedDepthsRadiusPercentage.push_back(30);
  this->EvaluatedDepthsRadiusPercentage.push_back(50);
  this->EvaluatedDepthsRadiusPercentage.push_back(70);

}

//----------------------------------------------------------------------------
vtkFanAngleDetectorAlgo::~vtkFanAngleDetectorAlgo()
{
  SetImage(NULL);
}

//----------------------------------------------------------------------------
void vtkFanAngleDetectorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkFanAngleDetectorAlgo::Update()
{
  double presetAngleLeft = this->MaxFanAnglesDeg[0];
  double presetAngleRight = this->MaxFanAnglesDeg[1];
  int xOrigin = this->FanOrigin[0];
  int yOrigin = this->FanOrigin[1];
  vtkImageData* frameImage = this->Image;
  int* imageExtent=frameImage->GetExtent();

  // create bands at the specified radius values
  std::vector<BandInfo> bands;
  BandInfo band;
  for (EvaluatedDepthsRadiusPercentageType::iterator radiusPercentageIt=this->EvaluatedDepthsRadiusPercentage.begin();
    radiusPercentageIt!=this->EvaluatedDepthsRadiusPercentage.end(); ++radiusPercentageIt)
  {
    band.TestRadius = this->FanRadiusStart + (this->FanRadiusStop-this->FanRadiusStart) * (*radiusPercentageIt)/100;
    bands.push_back(band);
  }

  // Compute sampling point positions
  double maxFanAnglesRad[2] = { vtkMath::RadiansFromDegrees(this->MaxFanAnglesDeg[0]), vtkMath::RadiansFromDegrees(this->MaxFanAnglesDeg[1]) };
  double sampleDistancePixel = 1; // sampling distance along the circle circumference
  for (std::vector<BandInfo>::iterator bandIt=bands.begin(); bandIt!=bands.end(); ++bandIt)
  {
    bandIt->AngleIncrementRad = sampleDistancePixel/bandIt->TestRadius;
    int numberOfSamples = (maxFanAnglesRad[1]-maxFanAnglesRad[0])/bandIt->AngleIncrementRad;
    for (int sampleIndex=0; sampleIndex<numberOfSamples; sampleIndex++)
    {
      double angleRad = maxFanAnglesRad[0]+sampleIndex*bandIt->AngleIncrementRad;
      int posX = vtkMath::Round(xOrigin + bandIt->TestRadius*sin(angleRad));
      int posY = vtkMath::Round(yOrigin + bandIt->TestRadius*cos(angleRad));
      if (posX<imageExtent[0] || posX>imageExtent[1] || posY<imageExtent[2] || posY>imageExtent[3])
      {
        // out of image extent
        continue;
      }
      bandIt->TestThetaRad.push_back(angleRad);
      bandIt->TestValue.push_back(frameImage->GetScalarComponentAsDouble(posX,posY,0,0));
    }
  }

  // Find all regions with intensity above the threshold
	double numberOfAveragedSamples  = this->FilterRadiusPixel*2; // moving average filter size
  for (std::vector<BandInfo>::iterator bandIt=bands.begin(); bandIt!=bands.end(); ++bandIt)
  {
  	int j = numberOfAveragedSamples;
    int nTheta = bandIt->TestThetaRad.size();
    for (int j=numberOfAveragedSamples; j<nTheta; j++)
    {
			double testCount = 0;
			for (int k = 0; k < numberOfAveragedSamples; k++)
      {
				if (bandIt->TestValue[j - numberOfAveragedSamples + k] >= this->BrightnessThreshold)
        {
          testCount++;
        }
			}
			if (!bandIt->Valid && testCount >= numberOfAveragedSamples/2.0) // it was invalid but now at least half of the pixels over threshold
      {
        bandIt->LeftLogRad.push_back(bandIt->TestThetaRad[j - numberOfAveragedSamples]);
				bandIt->Valid = true;
			} 
      else if (bandIt->Valid && testCount <= numberOfAveragedSamples/2.0 )  // it was valid but now at least half of the pixels are below threshold
      {
        bandIt->RightLogRad.push_back( bandIt->TestThetaRad[j - 1]);
				bandIt->Valid = false;
			}
    }
    if (bandIt->LeftLogRad.size()>bandIt->RightLogRad.size())
    {
      // peak not closed
      bandIt->RightLogRad.push_back( maxFanAnglesRad[1] );
    }
  }

	// Pick the largest continuous area of pixel value for each test radius and send it to fanAngles
  for (std::vector<BandInfo>::iterator bandIt=bands.begin(); bandIt!=bands.end(); ++bandIt)
  {
    bandIt->DetectedFanAnglesRad[0] = 0;
    bandIt->DetectedFanAnglesRad[1] = 0;
    for (int k = 0; k < bandIt->LeftLogRad.size(); k++)
    {
      if ( (bandIt->RightLogRad[k]-bandIt->LeftLogRad[k]) > (bandIt->DetectedFanAnglesRad[1]-bandIt->DetectedFanAnglesRad[0]) )
      {
        bandIt->DetectedFanAnglesRad[0] = bandIt->LeftLogRad[k];
        bandIt->DetectedFanAnglesRad[1] = bandIt->RightLogRad[k];
      }
    }
  }

	// search for the broadest of the three angular windows and set them as the outputs
	// broadest in case there's a ventricle or something
  double outputAngleLeftRad = bands.front().DetectedFanAnglesRad[0];
	double outputAngleRightRad = bands.front().DetectedFanAnglesRad[1];
  for (std::vector<BandInfo>::iterator bandIt=bands.begin(); bandIt!=bands.end(); ++bandIt)
  {
    if ( (bandIt->DetectedFanAnglesRad[1]-bandIt->DetectedFanAnglesRad[0]) > (outputAngleRightRad - outputAngleLeftRad) )
    {
			outputAngleLeftRad = bandIt->DetectedFanAnglesRad[0];
			outputAngleRightRad = bandIt->DetectedFanAnglesRad[1];
		}
	}

	// set the blank frame case (i.e. both angles = presetRightAngle) to be 0 degrees for consistency
	if (outputAngleLeftRad == 0 && outputAngleRightRad == 0)
  {
    this->DetectedFanAnglesDeg[0]=0.0;
    this->DetectedFanAnglesDeg[1]=0.0;
    this->IsFrameEmpty=true;
	}
  else
  {
  	// save results, apply a margin if necessary
    this->DetectedFanAnglesDeg[0]=vtkMath::DegreesFromRadians(outputAngleLeftRad)-this->FanAngleMarginDeg;
    this->DetectedFanAnglesDeg[1]=vtkMath::DegreesFromRadians(outputAngleRightRad)+this->FanAngleMarginDeg;
    if (this->DetectedFanAnglesDeg[0]<this->MaxFanAnglesDeg[0])
    {
      this->DetectedFanAnglesDeg[0]=this->MaxFanAnglesDeg[0];
    }
    if (this->DetectedFanAnglesDeg[1]>this->MaxFanAnglesDeg[1])
    {
      this->DetectedFanAnglesDeg[1]=this->MaxFanAnglesDeg[1];
    }
    this->IsFrameEmpty=false;
  }
}
