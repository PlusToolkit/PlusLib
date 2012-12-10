/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusRevision.h"
#include "vtkPlusDeviceFactory.h"

int main(int argc, char **argv)
{

  LOG_INFO("Plus SVN revision: " << PLUSLIB_REVISION); 

#if defined _WIN64
  LOG_INFO("Build mode: Win64");
#elif defined _WIN32
  LOG_INFO("Build mode: Win32");
#else
  LOG_INFO("Build mode: other");
#endif

  vtkSmartPointer<vtkPlusDeviceFactory> deviceFactory = vtkSmartPointer<vtkPlusDeviceFactory>::New(); 
  std::ostringstream supportedDevices; 
  deviceFactory->PrintAvailableDevices(supportedDevices, vtkIndent()); 
  LOG_INFO(supportedDevices.str()); 

  return EXIT_SUCCESS; 
}