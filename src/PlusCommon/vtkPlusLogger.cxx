/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "vtkPlusLogger.h"

//-----------------------------------------------------------------------------
namespace
{
  vtkIGSIOSimpleRecursiveCriticalSection LoggerCreationCriticalSection;
}

//-------------------------------------------------------
vtkPlusLogger::vtkPlusLogger()
{
}

//-------------------------------------------------------
vtkPlusLogger::~vtkPlusLogger()
{
}

//-------------------------------------------------------
vtkIGSIOLogger* vtkPlusLogger::Instance()
{
  if (m_pInstance == NULL)
  {
    igsioLockGuard<vtkIGSIOSimpleRecursiveCriticalSection> loggerCreationGuard(&LoggerCreationCriticalSection);
    if (m_pInstance != NULL)
    {
      return m_pInstance;
    }

    vtkPlusLogger* newLoggerInstance = new vtkPlusLogger;
    // lock the instance even before making it available to make sure the instance is fully
    // initialized before anybody uses it
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> critSectionGuard(newLoggerInstance->m_CriticalSection);
    m_pInstance = newLoggerInstance;

    vtkPlusConfig::GetInstance(); // set the log file name from the XML config
    std::string strVersion = std::string("Software version: ") +
                               PlusCommon::GetPlusLibVersionString();

#ifdef _DEBUG
    strVersion += " (debug build)";
#endif

    m_pInstance->LogMessage(LOG_LEVEL_INFO, strVersion, "vtkPlusLogger", __LINE__);
  }

  return m_pInstance;
}
