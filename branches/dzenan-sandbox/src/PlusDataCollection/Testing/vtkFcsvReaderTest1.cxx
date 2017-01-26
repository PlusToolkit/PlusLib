/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkFcsvReader.h"

// STL includes
#include <stdio.h>
#include <string>
#include <stdlib.h>

// ITK includes
#include <itkLandmarkSpatialObject.h>

int main(int argc, char* argv[])
{
  vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_UNDEFINED);

  vtkSmartPointer<vtkFcsvReader> reader = vtkSmartPointer<vtkFcsvReader>::New();

  reader->SetFileName(argv[1]);
  reader->Update();

  LOG_DEBUG("Color information:" << reader->GetFcsvDataObject().color[0]
            << "  " << reader->GetFcsvDataObject().color[1]
            << "  " << reader->GetFcsvDataObject().color[2]);

  //Second way of iteration ( good way : access to many data)
  int fidIndex = 0;
  std::vector<FcsvPoint>::const_iterator it = reader->GetFcsvDataObject().points.begin();
  while (it != reader->GetFcsvDataObject().points.end())
  {
    LOG_DEBUG("Point position: " << (*it).position);

    if (fidIndex == 1)
    {
      if ((*it).position[0] != -319.5)
      {
        LOG_ERROR("Point position mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).position[1] != -224.608)
      {
        LOG_ERROR("Point position mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).position[2] != 77)
      {
        LOG_ERROR("Point position mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).selected != 0)
      {
        LOG_ERROR("Selected mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).visibility != 1)
      {
        LOG_ERROR("Visibility mismatch");
        return EXIT_FAILURE;
      }
    }
    if (fidIndex == 2)
    {
      if ((*it).selected != 1)
      {
        LOG_ERROR("Selected mismatch");
        return EXIT_FAILURE;
      }
      if ((*it).visibility != 0)
      {
        LOG_ERROR("Visibility mismatch");
        return EXIT_FAILURE;
      }
    }

    ++it;
    fidIndex++;
  }

  return EXIT_SUCCESS;
}