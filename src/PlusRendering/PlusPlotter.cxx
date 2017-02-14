/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusPlotter.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDataArray.h>
#include <vtkDelimitedTextWriter.h>
#include <vtkDoubleArray.h>
#include <vtkInformation.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPNGWriter.h>
#include <vtkPlot.h>
#include <vtkPlotBar.h>
#include <vtkPlotLine.h>
#include <vtkPlotPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkWindowToImageFilter.h>

//----------------------------------------------------------------------------
PlusStatus PlusPlotter::WriteScatterChartToFile(const std::string& chartTitle,
    const std::string& yAxisText,
    vtkTable& inputTable,
    int xColumnIndex,
    int yColumnIndex,
    int imageSize[2],
    const std::string& outputImageFilename)
{
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  vtkPlotPoints* scatterPlot = vtkPlotPoints::SafeDownCast(chart->AddPlot(vtkChart::POINTS));
  scatterPlot->SetInputData(&inputTable, xColumnIndex, yColumnIndex);
  scatterPlot->SetColor(0, 0, 1);
  scatterPlot->SetMarkerStyle(vtkPlotPoints::CROSS);

  chart->SetShowLegend(false);

  chart->GetAxis(vtkAxis::LEFT)->SetTitle(yAxisText);
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(chartTitle);

  return WriteChartToFile(*view, imageSize, outputImageFilename);
}

//----------------------------------------------------------------------------
PlusStatus PlusPlotter::WriteLineChartToFile(const std::string& chartTitle,
    const std::string& yAxisText,
    vtkTable& inputTable,
    int xColumnIndex,
    int y1ColumnIndex,
    int y2ColumnIndex,
    int imageSize[2],
    const std::string& outputImageFilename)
{
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  if (y1ColumnIndex >= 0)
  {
    vtkPlotLine* linePlot = vtkPlotLine::SafeDownCast(chart->AddPlot(vtkChart::LINE));
    linePlot->SetInputData(&inputTable, xColumnIndex, y1ColumnIndex);
    //linePlot->SetColor(0,0,1);
  }

  if (y2ColumnIndex >= 0)
  {
    vtkPlotLine* linePlot = vtkPlotLine::SafeDownCast(chart->AddPlot(vtkChart::LINE));
    linePlot->SetInputData(&inputTable, xColumnIndex, y2ColumnIndex);
    //linePlot1->SetColor(0,0,1);
  }

  chart->SetShowLegend(true);

  chart->GetAxis(vtkAxis::LEFT)->SetTitle(yAxisText);
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(chartTitle);

  return WriteChartToFile(*view, imageSize, outputImageFilename);
}

//----------------------------------------------------------------------------
PlusStatus PlusPlotter::WriteHistogramChartToFile(const std::string& chartTitle,
    vtkTable& inputTable,
    int inputColumnIndex,
    double valueRangeMin,
    double valueRangeMax,
    int numberOfBins,
    int imageSize[2],
    const std::string& outputImageFilename)
{
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  vtkSmartPointer<vtkTable> histogramTable = vtkSmartPointer<vtkTable>::New();
  if (ComputeHistogram(inputTable, inputColumnIndex, *histogramTable, valueRangeMin, valueRangeMax, numberOfBins) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  vtkPlotBar* barPlot = vtkPlotBar::SafeDownCast(chart->AddPlot(vtkChart::BAR));
  barPlot->SetInputData(histogramTable, 0 /* x column index */, 1 /* y column index */);
  barPlot->SetColor(0, 0, 1);

  chart->SetShowLegend(false);

  chart->GetAxis(vtkAxis::LEFT)->SetTitle("Number of samples");
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(chartTitle);

  return WriteChartToFile(*view, imageSize, outputImageFilename);
}

//----------------------------------------------------------------------------
PlusStatus PlusPlotter::WriteChartToFile(vtkContextView& view,
    int imageSize[2],
    const std::string& outputImageFilename)
{
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(view.GetRenderer());
  renderWindow->SetSize(imageSize);
  renderWindow->OffScreenRenderingOn();

  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWindow);
  windowToImageFilter->Update();

  vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(outputImageFilename.c_str());
  writer->SetInputData(windowToImageFilter->GetOutput());
  writer->Write();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusPlotter::ComputeHistogram(vtkTable& inputTable,
    int inputColumnIndex,
    vtkTable& resultTable,
    double valueRangeMin,
    double valueRangeMax,
    unsigned int numberOfBins)
{
  vtkDataArray* inputArray = vtkDataArray::SafeDownCast(inputTable.GetColumn(inputColumnIndex));
  if (inputArray == NULL)
  {
    LOG_ERROR("PlusMath::ComputeHistogram failed: cannot find a valid data in column " << inputColumnIndex);
  }

  std::vector<int> histogramBins(numberOfBins, 0);
  int numberOfValues = inputArray->GetNumberOfTuples();
  const double binSize = (valueRangeMax - valueRangeMin) / numberOfBins;
  for (int i = 0; i < numberOfValues; i++)
  {
    int histogramBinIndex = static_cast<int>(floor((inputArray->GetTuple(i)[0] - valueRangeMin) / binSize));
    if (histogramBinIndex < 0)
    {
      histogramBinIndex = 0;
    }

    unsigned int histogramBinIndexUint = static_cast<unsigned int>(histogramBinIndex);
    if (histogramBinIndexUint >= numberOfBins)
    {
      histogramBinIndexUint = numberOfBins - 1;
    }
    ++histogramBins[histogramBinIndexUint];
  }

  // Clear table
  while (resultTable.GetNumberOfColumns() > 0)
  {
    resultTable.RemoveColumn(0);
  }

  // Copy histogram to table
  vtkSmartPointer<vtkDoubleArray> arrX = vtkSmartPointer<vtkDoubleArray>::New();
  resultTable.AddColumn(arrX);
  arrX->SetName("Histogram bin center value");
  vtkSmartPointer<vtkDoubleArray> arrY = vtkSmartPointer<vtkDoubleArray>::New();
  resultTable.AddColumn(arrY);
  arrY->SetName("Number of items");
  resultTable.SetNumberOfRows(numberOfBins);
  for (unsigned int i = 0; i < numberOfBins; ++i)
  {
    resultTable.SetValue(i, 0, valueRangeMin + (i + 0.5)*binSize);
    resultTable.SetValue(i, 1, histogramBins[i]);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusPlotter::WriteTableToFile(vtkTable& table, const std::string& filename)
{
  LOG_TRACE("PlusPlotter::WriteTableToFile");

  if (filename.empty())
  {
    LOG_ERROR("Failed to dump table to file - Input filename is empty!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkDelimitedTextWriter> textWriter = vtkSmartPointer<vtkDelimitedTextWriter>::New();
  textWriter->SetFieldDelimiter("\t");
  textWriter->SetUseStringDelimiter(false);
  textWriter->SetFileName(filename.c_str());
  textWriter->SetInputData(&table);
  textWriter->Update();

  return PLUS_SUCCESS;
}
