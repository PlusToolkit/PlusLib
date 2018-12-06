/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkPlusLogger.h"
#include "PlusCommon.h"

//-----------------------------------------------------------------------------
namespace
{
  vtkIGSIOSimpleRecursiveCriticalSection LoggerCreationCriticalSection;
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

    std::string strVersion = std::string("Software version: ") +
                               PlusCommon::GetPlusLibVersionString();

#ifdef _DEBUG
    strIGSIOVersion += " (debug build)";
#endif

    m_pInstance->LogMessage(LOG_LEVEL_INFO, strVersion, "vtkPlusLogger", __LINE__);
  }

  return m_pInstance;
}
