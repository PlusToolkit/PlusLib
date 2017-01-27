/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlusCompareVolumes.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPlusCompareVolumes - generates a histogram and statistics about volume reconstruction outputs
// .SECTION Description
// Currently this class only supports volume reconstructions of unsigned characters. The executable needs to be provided the following:
//   - A ground truth image
//   - A ground truth alpha image: This is used together with the slices alpha image to identify hole voxels
//   - A reconstructed "test" image
//   - A slices alpha image: The alpha channel if the slices are only pasted into the volume without hole filling

#ifndef __vtkPlusCompareVolumes_h
#define __vtkPlusCompareVolumes_h

#include "PlusConfigure.h"
#include "vtkThreadedImageAlgorithm.h"
#include <vector>

class vtkPlusCompareVolumes : public vtkThreadedImageAlgorithm
{
public:
  static vtkPlusCompareVolumes *New();
  vtkTypeMacro(vtkPlusCompareVolumes,vtkThreadedImageAlgorithm);

  // Description:
  // Set the Input1 of this filter.
  virtual void SetInputGT(vtkDataObject *input);;

  // Description:
  // Set the Input2 of this filter.
  virtual void SetInputGTAlpha(vtkDataObject *input);;

  // Description:
  // Set the Input3 of this filter.
  virtual void SetInputTest(vtkDataObject *input);;

  // Description:
  // Set the Input4 of this filter.
  virtual void SetInputTestAlpha(vtkDataObject *input);;

  // Description:
  // Set the Input5 of this filter.
  virtual void SetInputSliceAlpha(vtkDataObject *input);;

  // Description:
  // Output the images resulting from this filter
  vtkImageData* GetOutputTrueDifferenceImage();
  vtkImageData* GetOutputAbsoluteDifferenceImage();

  vtkGetMacro(RMS,double);
  vtkSetMacro(RMS,double);

  vtkGetMacro(TrueMean,double);
  vtkSetMacro(TrueMean,double);
  vtkGetMacro(TrueStdev,double);
  vtkSetMacro(TrueStdev,double);
  vtkGetMacro(TrueMedian,double);
  vtkSetMacro(TrueMedian,double);
  vtkGetMacro(TrueMinimum,double);
  vtkSetMacro(TrueMinimum,double);
  vtkGetMacro(TrueMaximum,double);
  vtkSetMacro(TrueMaximum,double);
  vtkGetMacro(True5thPercentile,double);
  vtkSetMacro(True5thPercentile,double);
  vtkGetMacro(True95thPercentile,double);
  vtkSetMacro(True95thPercentile,double);

  vtkGetMacro(AbsoluteMean,double);
  vtkSetMacro(AbsoluteMean,double);
  vtkGetMacro(AbsoluteStdev,double);
  vtkSetMacro(AbsoluteStdev,double);
  vtkGetMacro(AbsoluteMedian,double);
  vtkSetMacro(AbsoluteMedian,double);
  vtkGetMacro(AbsoluteMinimum,double);
  vtkSetMacro(AbsoluteMinimum,double);
  vtkGetMacro(AbsoluteMaximum,double);
  vtkSetMacro(AbsoluteMaximum,double);
  vtkGetMacro(Absolute5thPercentile,double);
  vtkSetMacro(Absolute5thPercentile,double);
  vtkGetMacro(Absolute95thPercentile,double);
  vtkSetMacro(Absolute95thPercentile,double);
  vtkGetMacro(NumberOfHoles,int);
  vtkSetMacro(NumberOfHoles,int);
  vtkGetMacro(NumberOfFilledHoles,int);
  vtkSetMacro(NumberOfFilledHoles,int);
  vtkGetMacro(NumberVoxelsVisible,int);
  vtkSetMacro(NumberVoxelsVisible,int);

  vtkGetMacro(AbsoluteMeanWithHoles,double);
  vtkSetMacro(AbsoluteMeanWithHoles,double);

  int* GetTrueHistogramPtr();
  int* GetAbsoluteHistogramPtr();
  int* GetAbsoluteHistogramWithHolesPtr();
  void incTrueHistogramAtIndex(int value);
  void incAbsoluteHistogramAtIndex(int value);
  void incAbsoluteHistogramWithHolesAtIndex(int value);
  void resetTrueHistogram();
  void resetAbsoluteHistogram();
  void resetAbsoluteHistogramWithHoles();

protected:
  vtkPlusCompareVolumes();
  ~vtkPlusCompareVolumes() {};

  double RMS;
  double TrueMean,     TrueStdev,     TrueMedian,     TrueMinimum,     TrueMaximum,     True95thPercentile,     True5thPercentile;
  double AbsoluteMean, AbsoluteStdev, AbsoluteMedian, AbsoluteMinimum, AbsoluteMaximum, Absolute95thPercentile, Absolute5thPercentile;
  double AbsoluteMeanWithHoles;
  int TrueHistogram[511];
  int AbsoluteHistogram[256];
  int AbsoluteHistogramWithHoles[256];
  int NumberOfHoles;
  int NumberOfFilledHoles;
  int NumberVoxelsVisible;

  virtual int RequestInformation (vtkInformation *, vtkInformationVector**, vtkInformationVector *);

  void ThreadedRequestData (vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector,
                            vtkImageData ***inData, vtkImageData **outData,
                            int ext[6], int id);

  virtual int FillInputPortInformation(int port, vtkInformation* info);


private:
  vtkPlusCompareVolumes(const vtkPlusCompareVolumes&);  // Not implemented.
  void operator=(const vtkPlusCompareVolumes&);  // Not implemented.
};

#endif