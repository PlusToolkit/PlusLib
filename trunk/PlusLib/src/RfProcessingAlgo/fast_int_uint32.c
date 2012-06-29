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

/*  Include usual C type definitions                    */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

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
int     image_index[Nz_max * Nx_max];             /*  Index for the image matrix                                *

/*------------------------------------------------------------------------------*/
/*  Function for calculating the different weight tables for the interpolation  */
/*------------------------------------------------------------------------------*/

void  make_tables (double  start_depth,     /*  Depth for start of image in meters    */
                   double  image_size,      /*  Size of image in meters               */

                   double  start_of_data,   /*  Depth for start of data in meters     */
                   double  delta_r,         /*  Sampling interval for data in meters  */
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

/*--------------------------------------------------------*/
/*  Function for making the actual image interpolation    */
/*  It is assumed that the image has been previously      */
/*  initialized.                                          */
/*--------------------------------------------------------*/
void make_interpolation (const unsigned int  *envelope_data,   /*  The envelope detected and log-compressed data */
                         int            N_samples,        /*  Number of samples in one envelope line        */

                         int            *index_samp_line, /*  Index for the data sample number              */
                         int            *image_index,     /*  Index for the image matrix                    */
                         float          *weight_coef,     /*  The weight table                              */
                         int            N_values,         /*  Number of values to calculate in the image      */

                         unsigned int *image)            /*  The resulting image                           */
{
  int ij_index_coef = 0; //Index into coefficient array
  for (int i=0; i<N_values; i++)
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
