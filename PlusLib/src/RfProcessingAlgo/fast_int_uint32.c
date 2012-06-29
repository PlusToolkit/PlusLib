/*======================================================================================*/
/*                                                                                      */
/*   Main program for the fast interpolation program  by Joergen Arendt Jensen          */
/*   Name:  fast_int.c                                                                  */
/*   First version written February 13, 1999.                                           */
/*                                                                                      */
/*   This is version 1.10 of January 3, 2001 by Joergen Arendt Jensen                   */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
/*   This module is used for interfacing to the Matlab program. All procedures in the   */
/*  Matlab environment calls the program, that passes parameters on to the different    */
/*  procedures for fast interpolation of 2D and 3D images.                              */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
/*  Revision history:  First version:  February 13, 1999 by JAJ                         */
/*                                                                                      */
/*  Ver. 0.10, 13/ 2-99,  JAJ:  Initial trials with this version.                       */
/*  Ver. 1.00, 20/ 2-99,  JAJ:  First release version.                                  */
/*  Ver. 1.10,  3/ 1-01,  JAJ:  Version for 1024 by 1024 images.                        */
/*  Ver. 1.20, 28/ 1-10,  MJP:  Corrected calculation of x coordinates such that a      */
/*                                symmetric polar image results in a symmetric          */
/*                                rectangular image independent of even/uneven Nx       */
/*  Ver. 1.30, 29/ 8-11,  MAH: This version is for 32 bits usigned integere data        */
/*======================================================================================*/

/*-----------------------------------------------------*/
/*  This definition is only used on the HP-UX machine  */
/*-----------------------------------------------------*/

#define NO_BUILT_IN_SUPPORT_FOR_BOOL

/*  Include usual C type definitions                    */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

/*  Include typedefinitions for the mex file            */

#include "mex.h"

/*------------------------------------------------------------*/
/*  Define various data type, so that they are the same for   */
/*  different compilers and machines.                         */
/*------------------------------------------------------------*/

#define boolean   short

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
int     N_values;                                 /*  Number of values to calculate in the image                */
int     index_samp_line[Nz_max * Nx_max];         /*  Index for the data sample number                          */
int     image_index[Nz_max * Nx_max];             /*  Index for the image matrix                                */
                   
boolean table_set_up = 0;                         /*  Whether the tables has been set up                        */

double  start_depth;                              /*  Depth for start of image in meters                        */
double  image_size;                               /*  Size of image in meters                                   */
		   
double  start_of_data;                            /*  Depth for start of data in meters                         */
int     N_samples;                                /*  Number of data samples                                    */
double  delta_r;                                  /*  Sampling interval for data in meters                      */
		   
double  theta_start;                              /*  Angle for first line in image                             */
double  delta_theta;                              /*  Angle between individual lines                            */
int     N_lines;                                  /*  Number of acquired lines                                  */

double  scaling;                                  /*  Scaling factor form envelope to image                     */
int     Nz, Nx;                                   /*  Size of image in pixels                                   */


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
                   int     *N_values)       /*  Number of values to calculate in the image      */

		   
{int     i,j;            /*  Integer loop counters                      */

  double  z,z2,x;         /*  Image coordinates in meters                */
  double  dz,dx;          /*  Increments in image coordinates in meters  */
  double  radius;         /*  Radial distance                            */
  double  theta;          /*  Angle in degrees                           */

  double  samp;           /*  Sample number for interpolation            */
  double  line;           /*  Line number for interpolation              */
  int     index_samp;     /*  Index for the data sample number           */
  int     index_line;     /*  Index for the data line number             */
  double  samp_val;       /*  Sub-sample fraction for interpolation      */
  double  line_val;       /*  Sub-line fraction for interpolation        */

  boolean make_pixel;     /*  Whether the values is used in the image    */
  int     ij_index;       /*  Index into array                           */
  int     ij_index_coef;  /*  Index into coefficient array               */
 
  
  dz = image_size/Nz;
  dx = image_size/Nx;
  z = start_depth;
  ij_index = 0;
  ij_index_coef = 0;
  
  for (i=0; i<Nz; i++)
    {
      /*     x = -image_size/2; %% previous calculation (prior to vers. 1.20)*/
      x=-(Nx-1)/2.0*dx; /*new calculation to obtain symmetric, MJP */
      z2 = z*z;
     
      for (j=0; j<Nx; j++)
	{
	  /*  Find which samples to select from the envelope array  */
	      
	  radius = sqrt(z2+x*x);
	  theta = atan2 (x,z);
	  samp = (radius - start_of_data)/delta_r;
	  line = (theta - theta_start)/delta_theta;
	  index_samp = floor(samp);
	  index_line = floor(line);
	
	  /*  Test whether the samples are outside the array        */
	      
	  make_pixel =    (index_samp >= 0) && (index_samp+1 < N_samples)
	    && (index_line >= 0) && (index_line+1 < N_lines); 
	
	  if (make_pixel)
	    {
	      samp_val = samp - index_samp;
	      line_val = line - index_line;

	      /*  Calculate the coefficients if necessary   */
	      
	      weight_coef[ij_index_coef    ] = (1-samp_val)*(1-line_val)*scaling;
	      weight_coef[ij_index_coef + 1] =    samp_val *(1-line_val)*scaling;
	      weight_coef[ij_index_coef + 2] = (1-samp_val)* line_val   *scaling;
	      weight_coef[ij_index_coef + 3] =    samp_val * line_val   *scaling;

	      index_samp_line[ij_index] = index_samp + index_line*N_samples;
	      image_index[ij_index] = (Nx-j-1)*Nz + i;
	      ij_index++;
	      ij_index_coef = ij_index_coef + 4;
	    }
	  x = x + dx;
	}
      z = z + dz;
    }
  *N_values = ij_index;
  printf ("Table has now been set-up, %d x %d, %d %d values\n",Nz,Nx,ij_index,*N_values);
  
   
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

{int           i;                 /*  Integer loop counter                */
  int           ij_index_coef;     /*  Index into coefficient array        */
  unsigned int *env_pointer;       /*  Pointer to the envelope data        */
  float         *weight_pointer;   /*  Pointer to the weight coefficients  */
 
  ij_index_coef = 0;
  for (i=0; i<N_values; i++)
    {
      weight_pointer = &(weight_coef[ij_index_coef]);
      env_pointer = (unsigned int*) &(envelope_data[index_samp_line[i]]);
      image[image_index[i]] 
	=         weight_pointer[0] * env_pointer[0]
	+ weight_pointer[1] * env_pointer[1]
	+ weight_pointer[2] * env_pointer[N_samples]
	+ weight_pointer[3] * env_pointer[N_samples+1] + 0.5;
      ij_index_coef = ij_index_coef + 4;
    }
}


/*--------------------------------------------------------------------------*/
/*  Function called by Matlab when the matlab m-file fast_int is called.    */
/*  See the Matlab documentation for further explanation.                   */
/*                                                                          */
/*  Arguments:                                                              */
/*      nlhs - Number of left hand arguments, i.e., output variables.       */
/*      plhs - Pointers to the left hand arguments.                         */
/*                                                                          */
/*      nrhs - Number of right hand arguments, i.e., input variables.       */
/*      prhs - Pointers to the right hand arguments.                        */
/*--------------------------------------------------------------------------*/
  

void mexFunction(int  nlhs,         mxArray  *plhs[],
                 int  nrhs,   const mxArray  *prhs[]) 

{int main_code;    /*  Code for the module called   */
                   /*   1 - Table definition        */
                   /*   2 - Image interpolation     */
		   
  const unsigned int *envelope_data;  /*  Pointer to the envelope detected and log-compressed data */
  int           dimension[2];    /*  Dimension of the resulting array                         */
 
  /* Check for proper number of arguments */

  if (nrhs == 0) 
    mexErrMsgTxt("Error 1: Illegal calling of the fast_int mex-code");

  /*  Get the function code and the main code   */

  main_code = floor(mxGetScalar (prhs[0]) + 0.5);

  /*  Branch out to the different modules         */

  switch (main_code) 
    {
      /*   Make the tables for image interpolation  */
    case 1:{
      if ((nrhs<2) && (nlhs<1))
	mexErrMsgTxt("Error 5: Too few input parameters to make interpolation tables");
      else
	{

	  /*  Decode the variables for the table  */
		    
	  start_depth = mxGetScalar (prhs[1]);            /*  Depth for start of image in meters    */
	  image_size = mxGetScalar (prhs[2]);             /*  Size of image in meters               */
		   
	  start_of_data = mxGetScalar (prhs[3]);          /*  Depth for start of data in meters     */
	  delta_r = mxGetScalar (prhs[4]);                /*  Sampling interval for data in meters  */
	  N_samples = floor(mxGetScalar (prhs[5]) + 0.5); /*  Number of samples in one envelope line*/
		   
	  theta_start = mxGetScalar (prhs[6]);            /*  Angle for first line in image         */
	  delta_theta = mxGetScalar (prhs[7]);            /*  Angle between individual lines        */
	  N_lines = floor(mxGetScalar (prhs[8]) + 0.5);   /*  Number of acquired lines              */
		   
	  scaling = mxGetScalar (prhs[9]);                /*  Scaling factor form envelope to image */
	  Nz = floor(mxGetScalar (prhs[10]) + 0.5);       /*  Size of image in pixels               */
	  Nx = floor(mxGetScalar (prhs[11]) + 0.5);       /*  Size of image in pixels               */
	       
	  /*  Call the procedure and make the table  */
		
	  make_tables (start_depth, image_size, 
		       start_of_data, delta_r, N_samples, 
		       theta_start, delta_theta, N_lines, 
		       scaling, Nz, Nx,
		       weight_coef, index_samp_line, image_index, &N_values);
	       
	  table_set_up = 1;
	}
      break;
    }
	       
      /*   Make image interpolation                  */
    case 2:{ 
      if (table_set_up != 1)
	mexErrMsgTxt("Error 3: Table not set-up before calling make_interpolation, call make_tables first.");
      else
	if ((nrhs<2) && (nlhs<1))
	  mexErrMsgTxt("Error 4: Too few input and output parameters to make interpolation");
	else
	  {
	    /*  Decode the variables before calling   */
	    if (!mxIsUint32(prhs[1]))
	      mexErrMsgTxt("Input must be uint32");

	    envelope_data = (const unsigned int*) mxGetData(prhs[1]);      /*  The envelope detected and log-compressed data */

	    /*  Create the array that should hold the output image  */
		       
	    dimension[0]=Nz;
	    dimension[1]=Nx;
	    plhs[0]  = mxCreateNumericArray(2, dimension, mxUINT32_CLASS, mxREAL);

	    make_interpolation (envelope_data, N_samples,
				index_samp_line, image_index, weight_coef, N_values, 
				(unsigned int *) mxGetPr(plhs[0]));
	  }
      break;
    }
    default: {mexErrMsgTxt("Internal Error 2: Illegal main_code found in fast_int.");
	break;
    }  
    }

  return;
} 
