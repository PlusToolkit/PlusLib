/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include "MTC.h"

#include "MCamera.h"

/****************************/
/** Constructor */
MCamera::MCamera(mtHandle handle)
{
  // If a handle is provided to this class, don't create a new one
  char *DriverName = NULL;
  if (handle != 0)
  {
    this->m_handle = handle;
  }
  else
  {
    this->m_handle = Camera_New( DriverName,1965 ); 
  }
  this->ownedByMe = TRUE;
  limage = NULL;
  rimage = NULL;
  mimage = NULL;
  // error handling here
}

/****************************/
/** Destructor */
MCamera::~MCamera()
{
  if (this->m_handle != 0 && this->ownedByMe )
    Camera_Free(this->m_handle);
  free(limage);
  free(rimage);
}
mtHandle MCamera::Handle()
{
  return m_handle;
}

#if 0
bool MCamera::RawBufferValid()
{
  int result, r;
  r = Camera_RawBufferValidGet(m_handle, &result);
  if (r != mtOK)
    return false;
  else
    return ( result != 0 );
}

unsigned char **MCamera::RawBufferAddr()
{
  int  r;
  unsigned char **result = NULL;
  r = Camera_RawBufferAddrGet(m_handle, &result);
  return result;
}
#endif

int MCamera::getXRes()
{
  int result = 0, y, r;
  r = Camera_ResolutionGet(m_handle, &result, &y);
  return ( result);

}
int MCamera::getYRes()
{
  int result =0, x, r;
  r = Camera_ResolutionGet(m_handle, &x, &result);
  return ( result);

}

int MCamera::getSerialNum()
{
  int serialNum = -1;
  Camera_SerialNumberGet(m_handle, &serialNum);
  return (serialNum);
}

int MCamera::getBitsPerPixel()
{
  int bitsPerPixel = -1;
  Camera_BitsPerPixelGet(m_handle, &bitsPerPixel);
  return ( bitsPerPixel);
}

/****************************/
/** Gets the shtutter opening time in msecs.  Returns -1 if not successful*/
double MCamera::getShutterTime()
{
  double shutterTime = 0;
  int r = Camera_ShutterMsecsGet(this->m_handle, &shutterTime);
  return r == mtOK ? shutterTime : -1;
}

/****************************/
/** Sets the shutter time of the camera. If successful returns 0, otherwise returns -1. */
int MCamera::setShutterTime(double sh)
{
  int result = Camera_ShutterMsecsSet(this->m_handle, sh);
  return result == mtOK ? result : -1;
}

/****************************/
/** Gets the minimum shutter time of the camera. Returns -1 if not successful. */
double MCamera::getMinShutterTime()
{
  double minShutterTime = 0;
  int result = Camera_ShutterMsecsMinGet(this->m_handle, &minShutterTime);
  return result == mtOK ? minShutterTime : -1;
}

/****************************/
/** Gets the maximum shutter time of the camera.  Returns -1 if not successful. */
double MCamera::getMaxShutterTime()
{
  double maxShutterTime = 0;
  int result = Camera_ShutterMsecsMaxGet(this->m_handle, &maxShutterTime);
  return result == mtOK ? maxShutterTime : -1;
}

/****************************/
/** Gets the limiter setting of the ShutterMsecs property controlling Camera_ExposureSet().
    Returns -1 if not successful. */
double MCamera::getShutterTimeLimit()
{
  double shutterTimeLimit = 0;
  int result = Camera_ShutterMsecsLimiterGet (this->m_handle, &shutterTimeLimit);
  return result == mtOK ? shutterTimeLimit : -1;
}

/****************************/
/** Sets the limiter setting of the ShutterMsecs property controlling Camera_ExposureSet().
    If successful returns 0, otherwise returns -1. */
double MCamera::setShutterTimeLimit(double msecs)
{
  int result = Camera_ShutterMsecsLimiterSet (this->m_handle, msecs);
  return result == mtOK ? result : -1;
}

/****************************/
/** Returns the latest frame time in msecs. Returns -1 if not successful. */
double MCamera::getFrameTime()
{
  double frameTime = 0;
  int result = Camera_FrameMTTimeSecsGet(this->m_handle, &frameTime);
  return result == mtOK ? frameTime : -1;
}

/****************************/
/** Returns the number of frames grabbed since the start of the MT. Returns -1 if not successful. */
int MCamera::getNumOfFramesGrabbed()
{
  int numOfFrames = 0;
  int result = Camera_FramesGrabbedGet(this->m_handle, &numOfFrames);
  return (result == mtOK ? numOfFrames : -1);
}

/****************************/
/** Gets the gain of the camera. Returns -1 of not successful. */
double MCamera::getGain()
{
  double gain = 0;
  int result = Camera_GainFGet(this->m_handle, &gain);
  return (result == mtOK ? gain : -1);
}

/****************************/
/** Sets the gain of the camera. Returns -1 if not successful. */
int MCamera::setGain(double g)
{
  int result = Camera_GainFSet(this->m_handle, g);
  return (result == mtOK ? result : -1);
}

/****************************/
/** Gets the minimum gain of the camera. Returns -1 if not successful. */
double MCamera::getMinGain()
{
  double minGain = 0;
  int result = Camera_GainFMinGet(this->m_handle, &minGain);
  return (result == mtOK ? minGain : -1);
}

/****************************/
/** Gets the minimum gain of the camera. Returns -1 if not successful. */
double MCamera::getMaxGain()
{
  double maxGain = 0;
  int result = Camera_GainFMaxGet(this->m_handle, &maxGain);
  return (result == mtOK ? maxGain : -1);
}

/****************************/
/** Gets  the maximum setting of the GainF property available to Camera_ExposureSet().
    Returns -1 if not successful. */
double MCamera::getGainLimit()
{
  double gainLimit = 0;
  int result = Camera_GainFLimiterGet (this->m_handle, &gainLimit);
  return result == mtOK ? gainLimit : -1;
}

/****************************/
/** Sets the limiter setting of the GainF property controlling Camera_ExposureSet().
    If successful returns 0, otherwise returns -1. */
double MCamera::setGainLimit(double limit)
{
  int result = Camera_GainFLimiterSet (this->m_handle, limit);
  return result == mtOK ? result : -1;
}

/****************************/
/** Get the DB gain. Returns -1 if not successful. */
double MCamera::getDBGain()
{ 
  double dbGain = 0;
  int result = Camera_GainDBGet(this->m_handle, &dbGain);
  return (result == mtOK ? dbGain : -1);
}

/****************************/
/** Gets the exposure of the camera. Returns -1 if not successful. */
double MCamera::getExposure()
{
  double exposure = 0;
  int result = Camera_ExposureGet(this->m_handle, &exposure);
  return (result == mtOK ? exposure : -1);
}

/****************************/
/** Sets the exposure of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setExposure(double e)
{
  int result = Camera_ExposureSet(this->m_handle, e);
  return (result == mtOK ? result : -1);
}

/****************************/
/** Gets the minimum exposure of the camera. Returns -1 if not successful. */
double MCamera::getMinExposure()
{
  double minExposure = 0;
  int result = Camera_ExposureMinGet(this->m_handle, &minExposure);
  return (result == mtOK ? minExposure : -1);
}

/****************************/
/** Gets the minimum exposure of the camera. Returns -1 if not successful. */
double MCamera::getMaxExposure()
{
  double maxExposure = 0;
  int result = Camera_ExposureMaxGet(this->m_handle, &maxExposure);
  return (result == mtOK ? maxExposure : -1);
}

/****************************/
/** Gets the number os CCDs available in the current camera. */
int MCamera::getSensorsNum()
{
	int numOfSensors = 0;
	int result = Camera_NumofSensorsGet(this->m_handle, &numOfSensors);
	return numOfSensors;
}

/****************************/
/** Gets the AutoExposure property of the camera. Returns -1 if true, 0 if false. */
int MCamera::getAutoExposure()
{
  int autoExposure = 0;
  int result = Camera_AutoExposureGet(this->m_handle, &autoExposure);
  return autoExposure;
}

/****************************/
/** Sets the AutoExposure property of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setAutoExposure(int ae)
{
  int result = Camera_AutoExposureSet(this->m_handle, ae);
  return (result == mtOK ? result : -1);
}

/****************************/
/** Gets the value of the LightCoolness property of the camera. Returns -1 if not successful. */
double MCamera::getLightCoolness()
{
  double coolness = 0;
  int result = Camera_LightCoolnessGet(this->m_handle, &coolness);
  return (result == mtOK ? coolness : -1);
}

/****************************/
/** Gets the value of the LightCoolness property of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setLightCoolness(double value)
{
  int result = Camera_LightCoolnessSet(this->m_handle, value);
  return (result == mtOK ? result : -1);
}

mtMeasurementHazardCode MCamera::getThermalHazard()
{
  return Camera_LastFrameThermalHazard(this->m_handle);
}

int MCamera::AdjustCoolnessFromColorVector(mtHandle ColorVectorHandle)
{
  int result = Camera_LightCoolnessAdjustFromColorVector(this->m_handle, ColorVectorHandle, 0 );
  return (result == mtOK ? result : -1);  
}

//image - 0 left, 1 right, 2 middle
bool MCamera::getProjectionOnImage( int image, double XYZ[], double *x, double *y )
{
	mtCompletionCode  result =  Camera_ProjectionOnImage( this->m_handle, image, XYZ, x, y );
	return( result == mtOK ? true : false );
}

bool MCamera::getImages( unsigned char ***li, unsigned char ***ri)
{
  if (limage == NULL)
  {
    limage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes());
  }
  if (rimage == NULL)
  {
    rimage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes());
  }

  int r = Camera_ImagesGet(m_handle, (unsigned char *)limage, (unsigned char *)rimage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    return false;
  }
  else
  {
    *li = (unsigned char **)limage;
    *ri = (unsigned char **)rimage;
    return true;
  }
}

bool MCamera::getImages3( unsigned char ***li, unsigned char ***ri, unsigned char ***mi)
{
  if (limage == NULL)
  {
    limage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes());
  }
  if (rimage == NULL)
  {
    rimage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes());
  }
  if (mimage == NULL)
  {
    mimage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes());
  }
  int r = Camera_ImagesGet3(m_handle, (unsigned char *)limage, (unsigned char *)rimage, (unsigned char *)mimage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    *mi = NULL;
    return false;
  }
  else
  {
    *li = (unsigned char **)limage;
    *ri = (unsigned char **)rimage;
    *mi = (unsigned char **)mimage;
    return true;
  }
}

/****************************/
/** Gets the grabbed images. Returns true if successful. Returns false if not. */
bool MCamera::getHalfSizeImages(unsigned char ***li, unsigned char ***ri, int xRes, int yRes)
{
  if (limage == NULL)
  {
    limage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes()/2);
  }
  if (rimage == NULL)
  {
    rimage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes()/2);
  }
  int r = Camera_HalfSizeImagesGet(m_handle, (unsigned char *)limage, (unsigned char *)rimage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    return false;
  }
  else
  {
    *li = (unsigned char **)limage;
    *ri = (unsigned char **)rimage;
    return true;
  }
}

bool MCamera::getHalfSizeImages3(unsigned char ***li, unsigned char ***ri, unsigned char ***mi, int xRes, int yRes)
{
  if (limage == NULL)
  {
    limage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes()/2);
  }
  if (rimage == NULL)
  {
    rimage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes()/2);
  }
  if (mimage == NULL)
  {
    mimage = (unsigned char *)calloc (sizeof(unsigned char), getXRes() * getYRes()/2);
  }
  int r = Camera_HalfSizeImagesGet3(m_handle, (unsigned char *)limage, (unsigned char *)rimage, (unsigned char *)mimage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    *mi = NULL;
    return false;
  }
  else
  {
    *li = (unsigned char **)limage;
    *ri = (unsigned char **)rimage;
    *mi = (unsigned char **)mimage;
    return true;
  }
}

bool MCamera::grabFrame( )
{
  int r = Camera_GrabFrame (this->m_handle );
  return (mtOK == r);
}

/****************************/
bool MCamera::getHdrModeEnabled()
{
  bool IsEnabled = false;
  int result = Camera_HdrEnabledGet(this->m_handle, &IsEnabled);
  return IsEnabled;
}

/****************************/
/** Sets the AutoExposure property of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setHdrModeEnabled(bool NewVal)
{
  int result = Camera_HdrEnabledSet(this->m_handle, NewVal);
  return (result == mtOK ? result : -1);
}
