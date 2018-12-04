/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusXmlUtils_h
#define __PlusXmlUtils_h

#include <igsioXmlUtils.h>
#include "vtkXMLUtilities.h"

/*!
  \class PlusXmlUtils
  \brief Utility methods for XML tree manipulation
  \ingroup PlusLibCommon
*/

class PlusXmlUtils
{
public:
  /*! Attempt to read an XML file file from the current directory or the device set configuration file directory */
  static PlusStatus ReadDeviceSetConfigurationFromFile(vtkXMLDataElement* config, const char* filename)
  {
    if (config == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: invalid config input");
      return PLUS_FAIL;
    }
    if (filename == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: filename is not specified");
      return PLUS_FAIL;
    }

    std::string filePath = filename;
    if (!vtksys::SystemTools::FileExists(filePath.c_str(), true))
    {
      filePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(filename);
      if (!vtksys::SystemTools::FileExists(filePath.c_str(), true))
      {
        LOG_ERROR("Reading device set configuration file failed: " << filename << " does not exist in the current directory or in " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
        return PLUS_FAIL;
      }
    }

    vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(filePath.c_str()));
    if (rootElement == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: syntax error in " << filename);
      return PLUS_FAIL;
    }
    config->DeepCopy(rootElement);
    return PLUS_SUCCESS;
  }

};
#endif //__PlusXmlUtils_h
