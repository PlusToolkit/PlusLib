/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusPlotter_h
#define __PlusPlotter_h

#include "vtkPlusRenderingExport.h"

// Local includes
#include "PlusCommon.h"

class vtkTable;
class vtkContextView;

class vtkPlusRenderingExport PlusPlotter
{
public:
  /*! Write scatter plot to PNG file */
  static PlusStatus WriteScatterChartToFile(const std::string& chartTitle,
      const std::string& yAxisText,
      vtkTable& inputTable,
      int xColumnIndex,
      int yColumnIndex,
      int imageSize[2],
      const std::string& outputImageFilename);

  /*! Write line plot to PNG file */
  static PlusStatus WriteLineChartToFile(const std::string& chartTitle,
                                         const std::string& yAxisText,
                                         vtkTable& inputTable,
                                         int xColumnIndex,
                                         int y1ColumnIndex,
                                         int y2ColumnIndex,
                                         int imageSize[2],
                                         const std::string& outputImageFilename);

  /*! Compute histogram and write to PNG file */
  static PlusStatus WriteHistogramChartToFile(const std::string& chartTitle,
      vtkTable& inputTable,
      int inputColumnIndex,
      double valueRangeMin,
      double valueRangeMax,
      int numberOfBins,
      int imageSize[2],
      const std::string& outputImageFilename);

  /*! Render plot and write to PNG file */
  static PlusStatus WriteChartToFile(vtkContextView& view,
                                     int imageSize[2],
                                     const std::string& outputImageFilename);

  /*! Compute histogram for values in a table column */
  static PlusStatus ComputeHistogram(vtkTable& inputTable,
                                     int inputColumnIndex,
                                     vtkTable& resultTable,
                                     double valueRangeMin,
                                     double valueRangeMax,
                                     unsigned int numberOfBins);

  /*! Write table to delimited text file */
  static PlusStatus WriteTableToFile(vtkTable& table, const std::string& filename);
};

#endif //__PlusPlotter_h