/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*      Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto- Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "Cameras.h"
#include "MCamera.h"
#include "MicronTrackerLoggerMacros.h"

//----------------------------------------------------------------------------
Cameras::Cameras()
{
  this->OwnedByMe = true;
  this->CurrentCamera = NULL;
  this->LastFailedCamera = NULL;
  Cameras_HistogramEqualizeImagesSet(true);
  // error handling here
}

//----------------------------------------------------------------------------
Cameras::~Cameras()
{
  // Clear all previously connected camera
  for (std::vector<MCamera*>::iterator camsIterator = this->CameraList.begin(); camsIterator != this->CameraList.end(); ++camsIterator)
  {
    free(*camsIterator);
  }
  this->CameraList.clear();

  if (this->CurrentCamera != NULL)
  {
    free(this->CurrentCamera);
    this->CurrentCamera = NULL;
  }

  if (this->LastFailedCamera != NULL)
  {
    free(this->LastFailedCamera);
    this->LastFailedCamera = NULL;
  }
}

//----------------------------------------------------------------------------
mtHandle Cameras::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
int Cameras::getCount()
{
  return this->AttachedCameraCount;
}

//----------------------------------------------------------------------------
bool Cameras::getHistogramEqualizeImages()
{
  bool R;
  Cameras_HistogramEqualizeImagesGet(&R);
  return R;
}

//----------------------------------------------------------------------------
int Cameras::setHistogramEqualizeImages(bool on_off)
{
  return Cameras_HistogramEqualizeImagesSet(on_off);
}

//----------------------------------------------------------------------------
MCamera* Cameras::getCamera(int index)
{
  if (index >= (int)this->CameraList.size() || index < 0)
  {
    LOG_ERROR("Invalid camera index: " << index);
    return NULL;
  }
  return this->CameraList[index];
}

//----------------------------------------------------------------------------
int Cameras::grabFrame(MCamera* cam)
{
  int result = mtOK; // success
  if (cam == NULL)
  {
    // grab from all cameras
    std::vector<MCamera*>::iterator camsIterator;
    for (camsIterator = this->CameraList.begin(); camsIterator != this->CameraList.end(); camsIterator++)
    {
      if ((*camsIterator)->grabFrame() != mtOK)
      {
        this->LastFailedCamera = *camsIterator;
        result = false;
        break;
      }
    }
  }
  else
  {
    result = cam->grabFrame();
    if (result != mtOK)
    {
      this->LastFailedCamera = cam;
    }
  }
  return result;
}

//----------------------------------------------------------------------------
void Cameras::detach()
{
  Cameras_Detach();
}

//----------------------------------------------------------------------------
int Cameras::getMTHome(std::string& mtHomeDirectory)
{
  const char mfile[] = "MTHome";

#ifdef _WIN32
  HKEY topkey = HKEY_LOCAL_MACHINE;
  const char subkey[] = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";

  /* Check registry key to determine log file name: */
  HKEY key = NULL;
  if (RegOpenKeyEx(topkey, subkey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to open registry key: " << subkey);
    return mtInternalMTError;
  }

  const int smtHomeDirectorySize = 512;
  char smtHomeDirectory[smtHomeDirectorySize + 1];
  smtHomeDirectory[smtHomeDirectorySize] = 0;
  DWORD value_type = 0;
  DWORD value_size = smtHomeDirectorySize;
  if (RegQueryValueEx(key, mfile, 0,  /* reserved */ &value_type, (unsigned char*)smtHomeDirectory, &value_size) != ERROR_SUCCESS || value_size <= 1)
  {
    /* size always >1 if exists ('\0' terminator) ? */
    LOG_ERROR("Failed to get environment variable " << mfile);
    return mtInternalMTError;
  }
  mtHomeDirectory = smtHomeDirectory;
#else
  char* localNamePtr = getenv(mfile);
  if (localNamePtr == NULL)
  {
    return mtNoEnvironmentVariable;
  }
  mtHomeDirectory = localNamePtr;
#endif

  return mtOK;
}

//----------------------------------------------------------------------------
int Cameras::attachAvailableCameras()
{
  std::string calibrationDir;
  int result = getMTHome(calibrationDir);
  if (result != mtOK)
  {
    // No Environment
    LOG_ERROR("MT home directory was not found");
    return result;
  }
  calibrationDir += std::string("/CalibrationFiles");

#if 0
  // Clear all previously connected camera
  vector<MCamera*>::iterator camsIterator;
  for (camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end(); ++camsIterator)
  {
    free(*camsIterator);
  }
  if (mCurrCam != NULL)
  {
    free(mCurrCam);
    mCurrCam = NULL;
  }
  if (mFailedCam != NULL)
  {
    free(mFailedCam);
    mFailedCam = NULL;
  }
#endif

  mtCompletionCode res = Cameras_AttachAvailableCameras((char*)calibrationDir.c_str());
  if (res != mtOK)
  {
    LOG_ERROR("Failed to attach cameras using calibration data at: " << calibrationDir.c_str());
    return res;
  }

  // Number of the attached cameras
  this->AttachedCameraCount = Cameras_Count();
  if (this->AttachedCameraCount <= 0)
  {
    LOG_ERROR("No attached cameras were found");
    return mtCameraNotFound;
  }

  // Populate the array of camera that are already attached
  for (int c = 0; c < this->AttachedCameraCount; c++)
  {
    if (c > MaxCameras)
    {
      break;
    }
    mtHandle camHandle = 0;
    if (Cameras_ItemGet(c, &camHandle) == mtOK)
    {
      this->CameraList.push_back(new MCamera(camHandle));
    }
  }

  return mtOK;
}
