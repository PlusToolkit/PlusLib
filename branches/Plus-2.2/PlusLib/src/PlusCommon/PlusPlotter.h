/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusPlotter_h
#define __PlusPlotter_h

#include "vtkPlusCommonExport.h"

#include "vtkAxis.h"
#include "vtkChartXY.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkDataArray.h"
#include "vtkDelimitedTextWriter.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPlot.h"
#include "vtkPlotBar.h"
#include "vtkPlotLine.h"
#include "vtkPlotPoints.h"
#include "vtkPNGWriter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTable.h"
#include "vtkWindowToImageFilter.h"

class PlusPlotter
{
public:

  /*! 
    Write scatter plot to PNG file
  */    
  static PlusStatus WriteScatterChartToFile(const char* chartTitle, const char* yAxisText, vtkTable* inputTable, int xColumnIndex, int yColumnIndex, int imageSize[2], const char* outputImageFilename)
  {
    vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    view->GetScene()->AddItem(chart);
    
    vtkPlotPoints *scatterPlot = vtkPlotPoints::SafeDownCast(chart->AddPlot(vtkChart::POINTS));
    scatterPlot->SetInputData_vtk5compatible(inputTable, xColumnIndex, yColumnIndex);
    scatterPlot->SetColor(0,0,1);
    scatterPlot->SetMarkerStyle(vtkPlotPoints::CROSS);

    chart->SetShowLegend(false);

    chart->GetAxis(vtkAxis::LEFT)->SetTitle(yAxisText);
    chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(chartTitle);

    return WriteChartToFile(view, imageSize, outputImageFilename);
  }

  /*! 
    Write line plot to PNG file
  */    
  static PlusStatus WriteLineChartToFile(const char* chartTitle, const char* yAxisText, vtkTable* inputTable, int xColumnIndex, int y1ColumnIndex, int y2ColumnIndex, int imageSize[2], const char* outputImageFilename)
  {
    vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    view->GetScene()->AddItem(chart);
    
    if (y1ColumnIndex>=0)
    {
      vtkPlotLine *linePlot = vtkPlotLine::SafeDownCast(chart->AddPlot(vtkChart::LINE));
      linePlot->SetInputData_vtk5compatible(inputTable, xColumnIndex, y1ColumnIndex);
      //linePlot->SetColor(0,0,1);
    }    

    if (y2ColumnIndex>=0)
    {
      vtkPlotLine *linePlot = vtkPlotLine::SafeDownCast(chart->AddPlot(vtkChart::LINE));
      linePlot->SetInputData_vtk5compatible(inputTable, xColumnIndex, y2ColumnIndex);
      //linePlot1->SetColor(0,0,1);
    }

    chart->SetShowLegend(true);

    chart->GetAxis(vtkAxis::LEFT)->SetTitle(yAxisText);
    chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(chartTitle);

    return WriteChartToFile(view, imageSize, outputImageFilename);
  }

  /*! 
    Compute histogram and write to PNG file
  */    
  static PlusStatus WriteHistogramChartToFile(const char* chartTitle, vtkTable* inputTable, int inputColumnIndex, double valueRangeMin, double valueRangeMax, int numberOfBins, int imageSize[2], const char* outputImageFilename)
  {
    vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    view->GetScene()->AddItem(chart);

    vtkSmartPointer<vtkTable> histogramTable = vtkSmartPointer<vtkTable>::New();
    if (ComputeHistogram(inputTable, inputColumnIndex, histogramTable, valueRangeMin, valueRangeMax, numberOfBins)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }

    vtkPlotBar *barPlot = vtkPlotBar::SafeDownCast(chart->AddPlot(vtkChart::BAR));
    barPlot->SetInputData_vtk5compatible(histogramTable, 0 /* x column index */, 1 /* y column index */);
    barPlot->SetColor(0,0,1);

    chart->SetShowLegend(false);

    chart->GetAxis(vtkAxis::LEFT)->SetTitle("Number of samples");
    chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(chartTitle);

    return WriteChartToFile(view, imageSize, outputImageFilename);
  }

  /*! 
    Render plot and write to PNG file
  */
  static PlusStatus WriteChartToFile(vtkContextView* view, int imageSize[2], const char* outputImageFilename)
  {
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(view->GetRenderer());
    renderWindow->SetSize(imageSize);
    renderWindow->OffScreenRenderingOn(); 

    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->Update();

    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(outputImageFilename);
    writer->SetInputData_vtk5compatible(windowToImageFilter->GetOutput());
    writer->Write();

    return PLUS_SUCCESS;
  }

  /*! 
    Compute histogram for values in a table column
    \param TBD
  */
  static PlusStatus ComputeHistogram(vtkTable* inputTable, int inputColumnIndex, vtkTable* resultTable, double valueRangeMin, double valueRangeMax, unsigned int numberOfBins)
  {
    if (inputTable==NULL)
    {
      LOG_ERROR("PlusMath::ComputeHistogram failed: invalid input data table");
      return PLUS_FAIL;
    }
    vtkDataArray* inputArray = vtkDataArray::SafeDownCast(inputTable->GetColumn(inputColumnIndex));
    if (inputArray==NULL)
    {
      LOG_ERROR("PlusMath::ComputeHistogram failed: cannot find a valid data in column "<<inputColumnIndex);
    }

    std::vector<int> histogramBins(numberOfBins, 0);
    int numberOfValues = inputArray->GetNumberOfTuples();
    const double binSize = (valueRangeMax-valueRangeMin)/numberOfBins;
    for (int i=0; i<numberOfValues; i++)
    {
      int histogramBinIndex = floor((inputArray->GetTuple(i)[0]-valueRangeMin)/binSize);
      if (histogramBinIndex<0)
      {
        histogramBinIndex=0;
      }
      else if (histogramBinIndex>=numberOfBins)
      {
        histogramBinIndex=numberOfBins-1;
      }
      ++histogramBins[histogramBinIndex];
    }

    // Clear table
    while (resultTable->GetNumberOfColumns() > 0)
    {
      resultTable->RemoveColumn(0);
    }

    // Copy histogram to table
    vtkSmartPointer<vtkDoubleArray> arrX = vtkSmartPointer<vtkDoubleArray>::New();
    resultTable->AddColumn(arrX);
    arrX->SetName("Histogram bin center value");
    vtkSmartPointer<vtkDoubleArray> arrY = vtkSmartPointer<vtkDoubleArray>::New();
    resultTable->AddColumn(arrY);
    arrY->SetName("Number of items");
    resultTable->SetNumberOfRows(numberOfBins);
    for (int i = 0; i < numberOfBins; ++i)
    {
      resultTable->SetValue(i, 0, valueRangeMin+(i+0.5)*binSize);
      resultTable->SetValue(i, 1, histogramBins[i]);
    }

    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  static PlusStatus DumpTableToFile( vtkTable* table, const char* filename)
  {
    LOG_TRACE("PlusPlotter::DumpTableToFile"); 
    if ( table == NULL ) 
    {
      LOG_ERROR("Failed to dump table to file - Input table is NULL!"); 
      return PLUS_FAIL; 
    }
    if ( filename == NULL )
    {
      LOG_ERROR("Failed to dump table to file - Input filename is NULL!"); 
      return PLUS_FAIL; 
    }

    vtkSmartPointer<vtkDelimitedTextWriter> textWriter = vtkSmartPointer<vtkDelimitedTextWriter>::New(); 
    textWriter->SetFieldDelimiter("\t"); 
    textWriter->SetUseStringDelimiter(false); 
    textWriter->SetFileName(filename); 
    textWriter->SetInputData_vtk5compatible( table ); 
    textWriter->Update(); 

    return PLUS_SUCCESS; 
  }

};

#endif //__PlusPlotter_h
