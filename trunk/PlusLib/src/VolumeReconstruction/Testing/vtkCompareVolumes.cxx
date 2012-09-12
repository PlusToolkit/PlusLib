/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSimpleImageFilterExample.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkCompareVolumes.h"

#include "PlusMath.h"

#include "vtkImageData.h"
#include "vtkImageProgressIterator.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <vector>
#include <list>


vtkStandardNewMacro(vtkCompareVolumes);


vtkCompareVolumes::vtkCompareVolumes()
{
  this->SetNumberOfInputPorts(5);
  this->SetNumberOfOutputPorts(2);
  this->SetNumberOfThreads(1); // TODO: Remove when testing is done
}


int vtkCompareVolumes::RequestInformation (
  vtkInformation       * vtkNotUsed( request ),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector * outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0); // true difference image
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_DOUBLE, 1); // this has been set to 1 output scalar, type VTK_DOUBLE
  vtkInformation* outInfo2 = outputVector->GetInformationObject(1); // absolute difference image
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo2, VTK_DOUBLE, 1); // this has been set to 1 output scalar, type VTK_DOUBLE
  return 1;
}


template <class T>
void vtkCompareVolumesExecute(vtkCompareVolumes *self,
                              vtkImageData* inData,
                              vtkImageData* outData,
                              T* gtPtr,
                              T* gtAlphaPtr,
                              T* testPtr,
                              T* testAlphaPtr,
                              T* slicesAlphaPtr,
                              double* outPtrTru,
                              double* outPtrAbs,
                              int outExt[6],
                              int id)
{

  vtkIdType inOffsets[3]={0}; //x,y,z
	inData->GetIncrements(inOffsets[0],inOffsets[1],inOffsets[2]);
  
  vtkIdType outOffsets[3]={0}; //x,y,z
	outData->GetIncrements(outOffsets[0],outOffsets[1],outOffsets[2]);

  int xtemp(0),ytemp(0),ztemp(0); // indices for each of the axes
  self->resetTrueHistogram(); // count inside the class variables
  self->resetAbsoluteHistogramWithHoles();
  self->resetAbsoluteHistogram();
  std::vector<double> trueDifferences; // store all differences here
  std::vector<double> absoluteDifferences;
  std::vector<double> absoluteDifferencesInAllHoles; // same as absolute difference, but in hole voxels - 
                                                  // this can be added to find the absolute error when 
                                                  // we consider holes to be part of the image (and 
                                                  // choose to not ignore them in the error computation)
                                                  // note these are not put into the histogram

  int countVisibleVoxels(0);
  int countFilledHoles(0);
  int countHoles(0);

  // iterate through all voxels
	for (ztemp = 0; ztemp <= outExt[5] - outExt[4]; ztemp++)
	{
		for (ytemp = 0; ytemp <= outExt[3] - outExt[2]; ytemp++)
		{
			for (xtemp = 0; xtemp <= outExt[1] - outExt[0]; xtemp++)
			{
        int inIndex  =  inOffsets[0]*xtemp+ inOffsets[1]*ytemp+ inOffsets[2]*ztemp;
        int outIndex = outOffsets[0]*xtemp+outOffsets[1]*ytemp+outOffsets[2]*ztemp;
        if (gtAlphaPtr[inIndex] != 0) 
        {
          countVisibleVoxels++;
          if (slicesAlphaPtr[inIndex] == 0) 
          {
            countHoles++;
            double difference = (double)gtPtr[inIndex] - testPtr[inIndex];
            self->incAbsoluteHistogramWithHolesAtIndex(PlusMath::Round(abs(difference)));
            absoluteDifferencesInAllHoles.push_back(abs(difference));
            if (testAlphaPtr[inIndex] != 0) {
              countFilledHoles++;
              trueDifferences.push_back(difference);
              self->incTrueHistogramAtIndex(PlusMath::Round(difference));
              outPtrTru[outIndex] = difference; // cast to double to minimize precision loss
              absoluteDifferences.push_back(abs(difference));
              self->incAbsoluteHistogramAtIndex(PlusMath::Round(abs(difference)));
              outPtrAbs[outIndex] = abs(difference);
            }
          }
          else // not a hole, but these may still be different
          {
            //double difference = (double)gtPtr[inIndex] - testPtr[inIndex]; // cast to double to minimize precision loss
            outPtrTru[outIndex] = 0.0; //difference;
            outPtrAbs[outIndex] = 0.0; //abs(difference);
          } // end slicesAlphaPtr check
        } 
        else 
        {
          outPtrTru[outIndex] = 0.0;
          outPtrAbs[outIndex] = 0.0;
        } // end gtAlphaPtr check
      } // end x loop
    } // end y loop
  } // end z loop

  double absoluteMeanWithHoles(0.0); // include holes in this computation
  // on this iteration, add only holes
  for (int i = 0; i < absoluteDifferencesInAllHoles.size(); i++)
  {
    absoluteMeanWithHoles += absoluteDifferencesInAllHoles[i];
  }

  // mean calculations
  double trueMean(0.0);
  double absoluteMean(0.0); // do not include holes in this computation
  double rms(0.0);
  for (int i = 0; i < countFilledHoles; i++) 
  {
    trueMean += trueDifferences[i];
    absoluteMean += absoluteDifferences[i];
    rms += trueDifferences[i] * trueDifferences[i];
  }

  // divide by the number of filled holes
  if (countFilledHoles != 0) {
    trueMean /= countFilledHoles;
    absoluteMean /= countFilledHoles;
    rms = sqrt(rms/countFilledHoles);
  }
  else
  {
    trueMean = 0;
    absoluteMean = 0;
    rms = 0;
  }

  // divide by the total number of holes
  if (countHoles != 0)
  {
    absoluteMeanWithHoles /= countHoles;
  }
  else
  {
    absoluteMeanWithHoles = 0;
  }

  // stdev calculations
  double trueStdev = 0.0; double absoluteStdev = 0.0;
  for (int i = 0; i < countFilledHoles; i++) 
  {
    trueStdev += pow((trueDifferences[i]-trueMean),2);
    absoluteStdev += pow((absoluteDifferences[i]-absoluteMean),2);
  }
  if (countFilledHoles != 0) 
  {
    trueStdev = sqrt(trueStdev/countFilledHoles);
    absoluteStdev = sqrt(absoluteStdev/countFilledHoles);
  }
  else
  {
    trueStdev = 0;
    absoluteStdev = 0;
  }

  // need to sort, temporarily store in a list, sort, then assign back to vector <== THIS IS SLOW SLOW SLOW, as in about half a minute for this alone, so TODO: Make this faster
  std::list<double> trueDifferencesList;
  std::list<double> absoluteDifferencesList;
  for (int i = 0; i < countFilledHoles; i++) {
    trueDifferencesList.push_front(trueDifferences.back());
    trueDifferences.pop_back();
    absoluteDifferencesList.push_front(absoluteDifferences.back());
    absoluteDifferences.pop_back();
  }
  trueDifferencesList.sort();
  absoluteDifferencesList.sort();
  for (int i = 0; i < countFilledHoles; i++) {
    trueDifferences.push_back(trueDifferencesList.front());
    trueDifferencesList.pop_front();
    absoluteDifferences.push_back(absoluteDifferencesList.front());
    absoluteDifferencesList.pop_front();
  }

  double true5thPercentile(0.0);
  double true95thPercentile(0.0);
  double absolute5thPercentile(0.0);
  double absolute95thPercentile(0.0);
  double absoluteMedian(0.0);
  double trueMedian(0.0);
  double trueMinimum(0.0);
  double trueMaximum(0.0);
  double absoluteMinimum(0.0);
  double absoluteMaximum(0.0);

  if (countFilledHoles != 0) {
    trueMinimum = trueDifferences[0];
    trueMaximum = trueDifferences[countFilledHoles-1];
    absoluteMinimum = absoluteDifferences[0];
    absoluteMaximum = absoluteDifferences[countFilledHoles-1];

    // old median calculation
    //double trueMedian = (countFilledHoles%2==0)?(trueDifferences[countFilledHoles/2]+trueDifferences[(countFilledHoles/2)-1])/2.0:trueDifferences[(countFilledHoles-1)/2];
    //double absoluteMedian = (countFilledHoles%2==0)?(absoluteDifferences[countFilledHoles/2]+absoluteDifferences[(countFilledHoles/2)-1])/2.0:absoluteDifferences[(countFilledHoles-1)/2];

    double medianRank = (countFilledHoles-1)*0.5;
    double medianFraction = fmod(medianRank,1.0);
    int medianFloor = (int)floor(medianRank); if (medianFloor < 0) medianFloor = 0;
    int medianCeil = (int)ceil(medianRank); if (medianCeil > (countFilledHoles-1)) medianCeil = (countFilledHoles-1);
    trueMedian = trueDifferences[medianFloor]*(1-medianFraction) + trueDifferences[medianCeil]*medianFraction;
    absoluteMedian = absoluteDifferences[medianFloor]*(1-medianFraction) + absoluteDifferences[medianCeil]*medianFraction;

    double percentile5rank = (countFilledHoles-1)*0.05;
    double percentile5fraction = fmod(percentile5rank,1.0);
    int percentile5floor = (int)floor(percentile5rank); if (percentile5floor < 0) percentile5floor = 0;
    int percentile5ceil = (int)ceil(percentile5rank); if (percentile5ceil > (countFilledHoles-1)) percentile5ceil = (countFilledHoles-1);
    true5thPercentile = trueDifferences[percentile5floor]*(1-percentile5fraction) + trueDifferences[percentile5ceil]*percentile5fraction;
    absolute5thPercentile = absoluteDifferences[percentile5floor]*(1-percentile5fraction) + absoluteDifferences[percentile5ceil]*percentile5fraction;

    double percentile95rank = (countFilledHoles-1)*0.95;
    double percentile95fraction = fmod(percentile95rank,1.0);
    int percentile95floor = (int)floor(percentile95rank); if (percentile95floor < 0) percentile95floor = 0;
    int percentile95ceil = (int)ceil(percentile95rank); if (percentile95ceil > (countFilledHoles-1)) percentile95ceil = (countFilledHoles-1);
    true95thPercentile = trueDifferences[percentile95floor]*(1-percentile95fraction) + trueDifferences[percentile95ceil]*percentile95fraction;
    absolute95thPercentile = absoluteDifferences[percentile95floor]*(1-percentile95fraction) + absoluteDifferences[percentile95ceil]*percentile95fraction;
  }

  self->SetNumberOfHoles(countHoles);
  self->SetNumberVoxelsVisible(countVisibleVoxels);
  self->SetNumberOfFilledHoles(countFilledHoles);

  self->SetTrue95thPercentile(true95thPercentile);
  self->SetTrue5thPercentile(true5thPercentile);
  self->SetTrueMaximum(trueMaximum);
  self->SetTrueMinimum(trueMinimum);
  self->SetTrueMedian(trueMedian);
  self->SetTrueStdev(trueStdev);
  self->SetTrueMean(trueMean);

  self->SetAbsolute95thPercentile(absolute95thPercentile);
  self->SetAbsolute5thPercentile(absolute5thPercentile);
  self->SetAbsoluteMaximum(absoluteMaximum);
  self->SetAbsoluteMinimum(absoluteMinimum);
  self->SetAbsoluteMedian(absoluteMedian);
  self->SetAbsoluteStdev(absoluteStdev);
  self->SetAbsoluteMean(absoluteMean);

  self->SetAbsoluteMeanWithHoles(absoluteMeanWithHoles);

  self->SetRMS(rms);

  int dummy = 0;
}

void vtkCompareVolumes::ThreadedRequestData (
  vtkInformation * vtkNotUsed( request ),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector * vtkNotUsed( outputVector ),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6], int threadId)
{
  if (inData[0][0] == NULL || inData[1][0] == NULL || inData[2][0] == NULL || inData[3][0] == NULL)
  {
    vtkErrorMacro(<< "Input must be specified.");
    return;
  }

  // this filter expects that all inputs are the same type as output.
  if (inData[1][0]->GetScalarType() != inData[0][0]->GetScalarType() ||
      inData[2][0]->GetScalarType() != inData[0][0]->GetScalarType() ||
      inData[3][0]->GetScalarType() != inData[0][0]->GetScalarType() )
  {
    vtkErrorMacro(<< "Execute: input ScalarTypes must match ScalarType " 
                  << inData[0][0]->GetScalarType());
    return;
  }
  
  vtkImageData* inVolData0 = inData[0][0];
  void* gtPtr = inVolData0->GetScalarPointer();
  
  vtkImageData* inVolData1 = inData[1][0];
  void* gtAlphaPtr = inVolData1->GetScalarPointer();
  
  vtkImageData* inVolData2 = inData[2][0];
  void* testPtr = inVolData2->GetScalarPointer();
  
  vtkImageData* inVolData3 = inData[3][0];
  void* testAlphaPtr = inVolData3->GetScalarPointer();

  vtkImageData* inVolData4 = inData[4][0];
  void* slicesAlphaPtr = inVolData4->GetScalarPointer();
  
  vtkImageData* outVolData0 = outData[0];
  double* outPtrTru = static_cast<double *>(outVolData0->GetScalarPointer());
  
  vtkImageData* outVolData1 = outData[1];
  double* outPtrAbs = static_cast<double *>(outVolData1->GetScalarPointer());
  
  switch (inVolData0->GetScalarType())
  {
    vtkTemplateMacro(
      vtkCompareVolumesExecute(this, inVolData0, outVolData0,
                               static_cast<VTK_TT *>(gtPtr),   static_cast<VTK_TT *>(gtAlphaPtr), 
                               static_cast<VTK_TT *>(testPtr), static_cast<VTK_TT *>(testAlphaPtr), 
                               static_cast<VTK_TT *>(slicesAlphaPtr), 
                               outPtrTru, outPtrAbs, outExt, threadId)
                    );
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
  }
}

int vtkCompareVolumes::FillInputPortInformation(int port, vtkInformation* info)
{
  /*if (port == 1)
  {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  }*/
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}


