/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkFcsvReader.h"
#include "vtkFcsvWriter.h"

// STL includes
#include <stdio.h>
#include <stdlib.h>
#include <string>

// ITK includes
#include <itkLandmarkSpatialObject.h>

int main(int argc, char* argv[])
{
  vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_UNDEFINED);

  vtkSmartPointer<vtkFcsvReader> reader = vtkSmartPointer<vtkFcsvReader>::New();

  reader->SetFileName(argv[1]);
  reader->Update();

  LOG_DEBUG("Number Of Points: " << reader->GetFcsvDataObject().points.size());
  if (reader->GetFcsvDataObject().points.size() != 18)
  {
    LOG_ERROR("Number Of Points: mismatch");
    return EXIT_FAILURE;
  }

  try
  {
    reader->GetFcsvPoint(2).position[0] = 100;
    reader->GetFcsvPoint(2).position[1] = 200;
    reader->GetFcsvPoint(2).position[2] = 300;
    reader->GetFcsvPoint(2).selected = 0;
    reader->GetFcsvPoint(2).visibility = 1;
  }
  catch (const std::out_of_range& ex)
  {
    LOG_ERROR(ex.what());
    return EXIT_FAILURE;
  }

  // Write the updated fcsv data
  vtkSmartPointer<vtkFcsvWriter> writer = vtkSmartPointer<vtkFcsvWriter>::New();
  writer->SetFileName(argv[2]);
  writer->CopyFcsvDataObject(reader->GetFcsvDataObject());
  writer->Update();

  // Read the updated fcsv data
  vtkSmartPointer<vtkFcsvReader> reader2 = vtkSmartPointer<vtkFcsvReader>::New();
  reader2->SetFileName(argv[2]);
  reader2->Update();

  LOG_DEBUG("Number Of Points: " << reader2->GetFcsvDataObject().points.size());
  if (reader2->GetFcsvDataObject().points.size() != 18)
  {
    LOG_ERROR("Number Of Points: mismatch");
    return EXIT_FAILURE;
  }

  int fidIndex = 0;
  std::vector<FcsvPoint>::const_iterator it = reader2->GetFcsvDataObject().points.begin();
  while (it != reader2->GetFcsvDataObject().points.end())
  {
    LOG_DEBUG("Point position: " << (*it).position);
    if (fidIndex == 2)
    {
      if ((*it).position[0] != 100)
      {
        LOG_ERROR("Position[0] mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).position[1] != 200)
      {
        LOG_ERROR("Position[1] mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).position[2] != 300)
      {
        LOG_ERROR("Position[2] mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).selected != 0)
      {
        LOG_ERROR("selected mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).visibility != 1)
      {
        LOG_ERROR("visibility mismatch");
        return EXIT_FAILURE;
      }
    }
    ++it;
    fidIndex++;
  }

  return EXIT_SUCCESS;
}