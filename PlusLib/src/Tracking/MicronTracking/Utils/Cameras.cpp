/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/

#include "Cameras.h"
#include "MCamera.h"
#include "MTC.h"
#include "MTVideo.h"

/****************************/
/** Constructor */
Cameras::Cameras()
{
  this->ownedByMe = TRUE;
  this->mCurrCam = NULL;
  this->mFailedCam = NULL;
  // error handling here
}

/****************************/
/** Destructor */
Cameras::~Cameras()
{
  // Clear all previously connected camera
  for (std::vector<MCamera *>::iterator camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end(); camsIterator++)
  {
    free(*camsIterator);
  }
  m_vCameras.clear();

  if (mCurrCam != NULL)
  {
    free(mCurrCam);
    mCurrCam=NULL;
  }

  if (mFailedCam != NULL) 
  {
    free(mFailedCam);
    mFailedCam=NULL;
  }
}

/****************************/
/** Returns the camera with the index of /param index. */
MCamera* Cameras::getCamera(int index)
{
  if (index>=(int)this->m_vCameras.size() || index<0)
  {
    return NULL;
  }
  return this->m_vCameras[index];
}

int Cameras::grabFrame(MCamera *cam)
{
  int result = 0; // success
  if (cam == NULL)
  {
    // grab from all cameras
    std::vector<MCamera *>::iterator camsIterator;
    for (camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end(); camsIterator++)
    {
      if (!(*camsIterator)->grabFrame())
      {
        mFailedCam = *camsIterator;
        result = -1;
        break;
      }
    }
  }
  else
  {
    if (!cam->grabFrame())
    {
      mFailedCam = cam;
      result = -1;
    }
  }
  return result;
}


void Cameras::Detach()
{
  MTexit();
}


int Cameras::getMTHome(std::string &mtHomeDirectory)
{
  const char mfile[] = "MTHome";

#ifdef _WIN32
  HKEY topkey=HKEY_LOCAL_MACHINE;
  const char subkey[]="SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";

  /* Check registry key to determine log file name: */
  LONG err=ERROR_SUCCESS;
  HKEY key=NULL;
  if ( (err = RegOpenKeyEx(topkey, subkey, 0, KEY_QUERY_VALUE, &key)) != ERROR_SUCCESS ) 
  {
    return (-1);
  }

  const int smtHomeDirectorySize=512;
  char smtHomeDirectory[smtHomeDirectorySize+1];
  smtHomeDirectory[smtHomeDirectorySize]=0;
  DWORD value_type=0;
  DWORD value_size = smtHomeDirectorySize;
  if ( RegQueryValueEx( key, mfile, 0,  /* reserved */ &value_type, (unsigned char*)smtHomeDirectory, &value_size ) != ERROR_SUCCESS || value_size <= 1 )
  {
    /* size always >1 if exists ('\0' terminator) ? */
    return (-1);
  }
  mtHomeDirectory=smtHomeDirectory;
#else
  char *localNamePtr = getenv(mfile);
  if ( localNamePtr == NULL) 
  {
    return (-1);
  }
  mtHomeDirectory=localNamePtr;
#endif

  return(0);
}

int Cameras::AttachAvailableCameras()
{
  std::string calibrationDir;
  if ( getMTHome(calibrationDir) != 0 ) 
  {
    // No Environment
    return -1;
  } 
  calibrationDir+=std::string("/CalibrationFiles");

#if 0
  // Clear all previously connected camera
  vector<MCamera *>::iterator camsIterator;
  for (camsIterator = m_vCameras.begin(); camsIterator != m_vCameras.end(); camsIterator++)
  {
    free (*camsIterator);
  }
  if (mCurrCam != NULL) 
  {
    free(mCurrCam);
  }
  if (mFailedCam != NULL) 
  {
    free(mFailedCam);
  }
#endif
  int result = Cameras_AttachAvailableCameras((char*)calibrationDir.c_str());
  if ( result != mtOK) 
  {
    return -1;
  }

  // Number of the attached cameras
  this->m_attachedCamNums = Cameras_Count();
  if (this->m_attachedCamNums <=0) 
  {
    return -1;
  }

  // Populate the array of camera that are already attached
  for (int c=0; c < this->m_attachedCamNums; c++)
  {
    if ( c > MaxCameras) 
    {
      break;
    }
    int camHandle=0;
    if (Cameras_ItemGet( c , &camHandle)==mtOK)
    {
      m_vCameras.push_back( new MCamera(camHandle));
    }
  }

  return 0;
}

/****************************/
/** Returns the shutter preference of the cameras. Returns 0 if successful, -1 if not. */
int Cameras::getShutterPreference()
{   
  int shutterPref = 0;
  int r = -1;//Cameras_ShutterPreferenceGet(this->m_handle, &shutterPref);
  return r == mtOK ? shutterPref : -1;
}

/****************************/
/** Sets the shutter preference of the cameras. Returns 0 if successful, -1 if not. */
int Cameras::setShutterPreference(int val)
{
  int r = -1;//Cameras_ShutterPreferenceSet(this->m_handle, val);
  return r == mtOK ? r : -1;
}

/****************************/
/** Returns the gain preference of the cameras. Returns 0 if successful, -1 if not. */
int Cameras::getGainPreference()
{
  int gainPref = 0;
  int r = -1;//Cameras_gainPreferenceGet(this->m_handle, &gainPref);
  return r == mtOK ? gainPref : -1;
}

/****************************/
/** Sets the gain preference of the cameras. Returns 0 if successful, -1 if not. */
int Cameras::setGainPreference(int val)
{
  int r = -1;//Cameras_gainPreferenceSet(this->m_handle, val);
  return r == mtOK ? r : -1;
}

