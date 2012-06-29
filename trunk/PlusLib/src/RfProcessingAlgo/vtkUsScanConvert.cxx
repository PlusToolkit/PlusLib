/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*
Based on the fast interpolation program (fast_int_uint32.c) by Joergen Arendt Jensen.
Version 1.30, downloaded on June 29, 2012 from
http://server.elektro.dtu.dk/personal/jaj/31545/exercises/exercise1/programs/
With permission from the author: "You are allowed to include the source code on
non-commercial terms in your toolbox and distribute it."
*/

#include "vtkUsScanConvert.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

vtkStandardNewMacro(vtkUsScanConvert);

/*-----------------------------------------------------------*/
/*    Define the global constants used by the program        */
/*-----------------------------------------------------------*/

#define  Nz_max   1024   /*  Largest number of pixels in x-direction  */
#define  Nx_max   1024   /*  Largest number of pixels in y-direction  */
#define  Ncoef_max   4   /*  Largest number of weight coefficients    */

/*-------------------------------------------------------------*/
/*    Define the global variables used by the program          */
/*-------------------------------------------------------------*/

float   weight_coef[Nz_max * Nx_max * Ncoef_max]; /*  Coefficients for the weighting of the image data          */
int     index_samp_line[Nz_max * Nx_max];         /*  Index for the data sample number                          */
int     image_index[Nz_max * Nx_max];             /*  Index for the image matrix                                */

/*------------------------------------------------------------------------------*/
/*  Function for calculating the different weight tables for the interpolation  */
/*------------------------------------------------------------------------------*/

void  make_tables (double  start_depth,     /*  Depth for start of image in mm        */
                   double  image_size,      /*  Size of image in mm                   */

                   double  start_of_data,   /*  Depth for start of data in mm         */
                   double  delta_r,         /*  Sampling interval for data in mm      */
                   int     N_samples,       /*  Number of data samples                */

                   double  theta_start,     /*  Angle for first line in image         */
                   double  delta_theta,     /*  Angle between individual lines        */
                   int     N_lines,         /*  Number of acquired lines              */

                   double  scaling,         /*  Scaling factor form envelope to image */
                   int     Nz,              /*  Size of image in pixels               */
                   int     Nx,              /*  Size of image in pixels               */


                   float   *weight_coef,    /*  The weight table                      */
                   int     *index_samp_line,/*  Index for the data sample number      */
                   int     *image_index,    /*  Index for the image matrix            */
                   int     &N_values)       /*  Number of values to calculate in the image      */
{

  // Increments in image coordinates in meters
  double dz = image_size/Nz;
  double dx = image_size/Nx;

  // Image coordinate in meters
  double z = start_depth;

  int ij_index = 0; // Index into array
  int ij_index_coef = 0; // Index into coefficient array 

  for (int i=0; i<Nz; i++)
  {
    double x=-(Nx-1)/2.0*dx; // image coordinate, in meters
    double z2 = z*z;

    for (int j=0; j<Nx; j++)
    {
      // Find which samples to select from the envelope array 
      double radius = sqrt(z2+x*x); // Radial distance
      double theta = atan2 (x,z); // Angle in degrees
      double samp = (radius - start_of_data)/delta_r; // Sample number for interpolation
      double line = (theta - theta_start)/delta_theta; // Line number for interpolation
      int index_samp = floor(samp); // Index for the data sample number
      int index_line = floor(line); // Index for the data line number

      // Test whether the samples are outside the array
      bool make_pixel = 
        (index_samp >= 0) && (index_samp+1 < N_samples) && 
        (index_line >= 0) && (index_line+1 < N_lines); 
      if (make_pixel)
      {
        double samp_val = samp - index_samp; // Sub-sample fraction for interpolation
        double line_val = line - index_line; // Sub-line fraction for interpolation

        //  Calculate the coefficients
        weight_coef[ij_index_coef    ] = (1-samp_val)*(1-line_val)*scaling;
        weight_coef[ij_index_coef + 1] =    samp_val *(1-line_val)*scaling;
        weight_coef[ij_index_coef + 2] = (1-samp_val)* line_val   *scaling;
        weight_coef[ij_index_coef + 3] =    samp_val * line_val   *scaling;

        index_samp_line[ij_index] = index_samp + index_line*N_samples;
        image_index[ij_index] = (Nx-j-1)*Nz + i;
        ij_index++;
        ij_index_coef += 4;
      }
      x = x + dx;
    }
    z = z + dz;
  }
  N_values = ij_index;

  LOG_DEBUG("Table has now been set-up, "<<Nz<<" x "<<Nx<<", "<<ij_index<<" "<<N_values<<" values\n");
}

//----------------------------------------------------------------------------
vtkUsScanConvert::vtkUsScanConvert()
{
  this->OutputImageExtent[0]=0;
  this->OutputImageExtent[1]=399;
  this->OutputImageExtent[2]=0;
  this->OutputImageExtent[3]=599;
  this->OutputImageExtent[4]=0; // not used
  this->OutputImageExtent[5]=1; // not used
  this->OutputImageSpacing[0]=0.2;
  this->OutputImageSpacing[1]=0.2;
  this->OutputImageSpacing[2]=1; // not used

  this->ImageStartDepthMm=10;
  this->ImageSizeMm=40;
  this->RadiusStartMm=15;
  this->RadiusDeltaMm=0.2;
  this->ThetaStartDeg=-30.0;
  this->ThetaDeltaDeg=1.0;
  this->OutputIntensityScaling=1.0;
  
  this->InterpolationTableSize=0; // N_values
}

//----------------------------------------------------------------------------
vtkUsScanConvert::~vtkUsScanConvert()
{
}

//----------------------------------------------------------------------------
// Computes any global image information associated with regions.
int vtkUsScanConvert::RequestInformation (vtkInformation * vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),this->OutputImageExtent,6);
  outInfo->Set(vtkDataObject::SPACING(),this->OutputImageSpacing,3);

  return 1;
}

//----------------------------------------------------------------------------
// The templated execute function handles all the data types.
// T: originally developed for unsigned int 
template <class T>
void vtkUsScanConvertExecute(vtkUsScanConvert *self,
                             vtkImageData *inData, T *inPtr,
                             vtkImageData *outData, T *outPtr,
                             int interpolationTableExt[6], int id)
{
  T *envelope_data=inPtr; // The envelope detected and log-compressed data
  int N_samples=inData->GetWholeExtent()[1]-inData->GetWholeExtent()[0]+1; // Number of samples in one envelope line
  T *image=outPtr; // The resulting image

  int ij_index_coef = interpolationTableExt[0]*4; //Index into coefficient array
  for (int i=interpolationTableExt[0]; i<=interpolationTableExt[1]; i++)
  {
    float *weight_pointer = &(weight_coef[ij_index_coef]); // Pointer to the weight coefficients
    unsigned int *env_pointer = (unsigned int*) &(envelope_data[index_samp_line[i]]); // Pointer to the envelope data
    image[image_index[i]] =
      weight_pointer[0] * env_pointer[0] // (+0, +0)
    + weight_pointer[1] * env_pointer[1] // (+1, +0)
    + weight_pointer[2] * env_pointer[N_samples] // (+0, +1)
    + weight_pointer[3] * env_pointer[N_samples+1] // (+1, +1)
    + 0.5; // for rounding
    ij_index_coef += 4;
  }
}

void vtkUsScanConvert::ThreadedRequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *vtkNotUsed(outputVector),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6], int id)
{

  if (this->InterpolationTableSize==0)
  {
    int numberOfSamples=inData->GetWholeExtent()[1]-inData->GetWholeExtent()[0];
    int numberOfLines=inData->GetWholeExtent()[3]-inData->GetWholeExtent()[2];
    int outputImageSizeX=this->OutputImageExtent[1]-this->OutputImageExtent[0]+1;
    int outputImageSizeY=this->OutputImageExtent[3]-this->OutputImageExtent[2]+1;
    // interpolation table has not been computed yet
    make_tables (this->StartDepthMm, this->ImageSizeMm,
      this->RadiusStartMm, this->RadiusDeltaMm, numberOfSamples,
      this->ThetaStartDeg, this->ThetaDeltaDeg, numberOfLines,
      this->OutputIntensityScaling, outputImageSizeX, outputImageSizeY
      weight_coef, index_samp_line, image_index,
      this->InterpolationTableSize);
  }

  void *inPtr = inData[0][0]->GetScalarPointer();
  void *outPtr = outData[0]->GetScalarPointer();

  // this filter expects that input is the same type as output.
  if (inData[0][0]->GetScalarType() != outData[0]->GetScalarType())
  {
    vtkErrorMacro("Execute: input ScalarType, "
      << inData[0][0]->GetScalarType()
      << ", must match out ScalarType "
      << outData[0]->GetScalarType());
    return;
  }

  switch (inData[0][0]->GetScalarType())
  {
    vtkTemplateMacro(
      vtkUsScanConvertExecute(this, inData[0][0],
      static_cast<VTK_TT *>(inPtr), outData[0], 
      static_cast<VTK_TT *>(outPtr),
      outExt, id));
  default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
  }
}

void vtkUsScanConvert::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "MagnificationFactors: ( "
    << this->MagnificationFactors[0] << ", "
    << this->MagnificationFactors[1] << ", "
    << this->MagnificationFactors[2] << " )\n";

  os << indent << "Interpolate: " << (this->Interpolate ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
// Splits data into num pieces for processing by each thread.
// Usually the output extent is split into pieces, but in our case
// we need to split the interpolation table.
// This method returns the number of pieces resulting from a successful split.
// This can be from 1 to "total".  
// If 1 is returned, the extent cannot be split.
int vtkThreadedImageAlgorithm::SplitExtent(int splitExt[6], int startExt[6], int num, int total)
{
  // startExt is not used, because we split the interpolation table

  // Starting extent
  int min = 0;
  int max = this->InterpolationTableSize-1;;

  splitExt[0]=min;
  splitExt[1]=max;
  splitExt[2]=0;
  splitExt[3]=0;
  splitExt[4]=0;
  splitExt[5]=0;

  if (min>=max)
  {
    // Cannot split interpolation table, as it's empty or has only one element
    return 1;
  }

  // determine the actual number of pieces that will be generated
  int range = max - min + 1;
  int valuesPerThread = static_cast<int>(ceil(range/static_cast<double>(total)));
  int maxThreadIdUsed = static_cast<int>(ceil(range/static_cast<double>(valuesPerThread))) - 1;
  if (num < maxThreadIdUsed)
  {
    splitExt[0] = splitExt[0] + num*valuesPerThread;
    splitExt[1] = splitExt[0] + valuesPerThread - 1;
  }
  if (num == maxThreadIdUsed)
  {
    splitExt[0] = splitExt[0] + num*valuesPerThread;
  }

  vtkDebugMacro("  Split Piece: ( " <<splitExt[0]<< ", " <<splitExt[1]<< ", "
    << splitExt[2] << ", " << splitExt[3] << ", "
    << splitExt[4] << ", " << splitExt[5] << ")");

  return maxThreadIdUsed + 1;
} 
