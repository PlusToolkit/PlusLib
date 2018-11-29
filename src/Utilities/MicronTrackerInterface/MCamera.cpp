/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shi Sherebrin , Robarts Research Institute - London - Ontario , www.robarts.ca
*     Shahram Izadyar, Robarts Research Institute - London - Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto - Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "MCamera.h"

//----------------------------------------------------------------------------
MCamera::MCamera(mtHandle handle)
{
  // If a handle is provided to this class, don't create a new one
  char* DriverName = NULL;
  if (handle != 0)
  {
    this->Handle = handle;
  }
  else
  {
    this->Handle = Camera_New(DriverName, 1965);
  }
  this->OwnedByMe = true;
  this->LeftImage = NULL;
  this->RightImage = NULL;
  this->MiddleImage = NULL;
  // error handling here
}

//----------------------------------------------------------------------------
MCamera::~MCamera()
{
  if (this->Handle != 0 && this->OwnedByMe)
  {
    Camera_Free(this->Handle);
  }
  free(this->LeftImage);
  free(this->RightImage);
}

//----------------------------------------------------------------------------
mtHandle MCamera::getHandle()
{
  return this->Handle;
}

#if 0
//----------------------------------------------------------------------------
int MCamera::RawBufferValid()
{
  int result, r;
  r = Camera_RawBufferValidGet(this->Handle, &result);
  if (r != mtOK)
  {
    return r;
  }
  else
  {
    return result;
  }
}

unsigned char** MCamera::RawBufferAddr()
{
  int r;
  unsigned char** result = NULL;
  r = Camera_RawBufferAddrGet(this->Handle, &result);
  return result;
}
#endif

//----------------------------------------------------------------------------
int MCamera::getXRes()
{
  int result = 0, y, r;
  r = Camera_ResolutionGet(this->Handle, &result, &y);
  return (result);
}
//----------------------------------------------------------------------------
int MCamera::getYRes()
{
  int result = 0, x, r;
  r = Camera_ResolutionGet(this->Handle, &x, &result);
  return (result);
}

//----------------------------------------------------------------------------
int MCamera::getSerialNum()
{
  int serialNum = -1;
  Camera_SerialNumberGet(this->Handle, &serialNum);
  return (serialNum);
}

//----------------------------------------------------------------------------
int MCamera::getBitsPerPixel()
{
  int bitsPerPixel = -1;
  Camera_BitsPerPixelGet(this->Handle, &bitsPerPixel);
  return (bitsPerPixel);
}

//----------------------------------------------------------------------------
/** Gets the shutter opening time in msecs.  Returns -1 if not successful*/
double MCamera::getShutterTime()
{
  double shutterTime = 0;
  int r = Camera_ShutterMsecsGet(this->Handle, &shutterTime);
  return r == mtOK ? shutterTime : -1;
}

//----------------------------------------------------------------------------
/** Sets the shutter time of the camera. If successful returns 0, otherwise returns -1. */
int MCamera::setShutterTime(double sh)
{
  return Camera_ShutterMsecsSet(this->Handle, sh);
}

//----------------------------------------------------------------------------
/** Gets the minimum shutter time of the camera. Returns -1 if not successful. */
double MCamera::getMinShutterTime()
{
  double minShutterTime = 0;
  int result = Camera_ShutterMsecsMinGet(this->Handle, &minShutterTime);
  return result == mtOK ? minShutterTime : -1;
}

//----------------------------------------------------------------------------
/** Gets the maximum shutter time of the camera.  Returns -1 if not successful. */
double MCamera::getMaxShutterTime()
{
  double maxShutterTime = 0;
  int result = Camera_ShutterMsecsMaxGet(this->Handle, &maxShutterTime);
  return result == mtOK ? maxShutterTime : -1;
}

//----------------------------------------------------------------------------
/** Gets the limiter setting of the ShutterMsecs property controlling Camera_ExposureSet().
    Returns -1 if not successful. */
double MCamera::getShutterTimeLimit()
{
  double shutterTimeLimit = 0;
  int result = Camera_ShutterMsecsLimiterGet(this->Handle, &shutterTimeLimit);
  return result == mtOK ? shutterTimeLimit : -1;
}

//----------------------------------------------------------------------------
/** Sets the limiter setting of the ShutterMsecs property controlling Camera_ExposureSet().*/
double MCamera::setShutterTimeLimit(double msecs)
{
  int result = Camera_ShutterMsecsLimiterSet(this->Handle, msecs);
  return result == mtOK ? result : -1;
}

//----------------------------------------------------------------------------
/** Returns the latest frame time in msecs. Returns -1 if not successful. */
double MCamera::getFrameTime()
{
  double frameTime = 0;
  int result = Camera_FrameMTTimeSecsGet(this->Handle, &frameTime);
  return result == mtOK ? frameTime : -1;
}

//----------------------------------------------------------------------------
/** Returns the number of frames grabbed since the start of the MT. Returns -1 if not successful. */
int MCamera::getNumOfFramesGrabbed()
{
  int numOfFrames = 0;
  int result = Camera_FramesGrabbedGet(this->Handle, &numOfFrames);
  return (result == mtOK ? numOfFrames : -1);
}

//----------------------------------------------------------------------------
/** Gets the gain of the camera. Returns -1 of not successful. */
double MCamera::getGain()
{
  double gain = 0;
  int result = Camera_GainFGet(this->Handle, &gain);
  return (result == mtOK ? gain : -1);
}

//----------------------------------------------------------------------------
/** Sets the gain of the camera. Returns -1 if not successful. */
int MCamera::setGain(double g)
{
  return Camera_GainFSet(this->Handle, g);
}

//----------------------------------------------------------------------------
/** Gets the minimum gain of the camera. Returns -1 if not successful. */
double MCamera::getMinGain()
{
  double minGain = 0;
  int result = Camera_GainFMinGet(this->Handle, &minGain);
  return (result == mtOK ? minGain : -1);
}

//----------------------------------------------------------------------------
/** Gets the minimum gain of the camera. Returns -1 if not successful. */
double MCamera::getMaxGain()
{
  double maxGain = 0;
  int result = Camera_GainFMaxGet(this->Handle, &maxGain);
  return (result == mtOK ? maxGain : -1);
}

//----------------------------------------------------------------------------
/** Gets  the maximum setting of the GainF property available to Camera_ExposureSet().
    Returns -1 if not successful. */
double MCamera::getGainLimit()
{
  double gainLimit = 0;
  int result = Camera_GainFLimiterGet(this->Handle, &gainLimit);
  return result == mtOK ? gainLimit : -1;
}

//----------------------------------------------------------------------------
/** Sets the limiter setting of the GainF property controlling Camera_ExposureSet().
    If successful returns 0, otherwise returns -1. */
double MCamera::setGainLimit(double limit)
{
  int result = Camera_GainFLimiterSet(this->Handle, limit);
  return result == mtOK ? result : -1;
}

//----------------------------------------------------------------------------
/** Get the DB gain. Returns -1 if not successful. */
double MCamera::getDBGain()
{
  double dbGain = 0;
  int result = Camera_GainDBGet(this->Handle, &dbGain);
  return (result == mtOK ? dbGain : -1);
}

//----------------------------------------------------------------------------
/** Gets the exposure of the camera. Returns -1 if not successful. */
double MCamera::getExposure()
{
  double exposure = 0;
  int result = Camera_ExposureGet(this->Handle, &exposure);
  return (result == mtOK ? exposure : -1);
}

//----------------------------------------------------------------------------
/** Sets the exposure of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setExposure(double e)
{
  return Camera_ExposureSet(this->Handle, e);
}

//----------------------------------------------------------------------------
/** Gets the minimum exposure of the camera. Returns -1 if not successful. */
double MCamera::getMinExposure()
{
  double minExposure = 0;
  int result = Camera_ExposureMinGet(this->Handle, &minExposure);
  return (result == mtOK ? minExposure : -1);
}

//----------------------------------------------------------------------------
/** Gets the minimum exposure of the camera. Returns -1 if not successful. */
double MCamera::getMaxExposure()
{
  double maxExposure = 0;
  int result = Camera_ExposureMaxGet(this->Handle, &maxExposure);
  return (result == mtOK ? maxExposure : -1);
}

//----------------------------------------------------------------------------
/** Gets the number os CCDs available in the current camera. */
int MCamera::getSensorsNum()
{
  int numOfSensors = 0;
  int result = Camera_NumofSensorsGet(this->Handle, &numOfSensors);
  return numOfSensors;
}

//----------------------------------------------------------------------------
/** Gets the AutoExposure property of the camera. Returns -1 if true, 0 if false. */
int MCamera::getAutoExposure()
{
  int autoExposure = 0;
  int result = Camera_AutoExposureGet(this->Handle, &autoExposure);
  return autoExposure;
}

//----------------------------------------------------------------------------
/** Sets the AutoExposure property of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setAutoExposure(int ae)
{
  return Camera_AutoExposureSet(this->Handle, ae);
}

//----------------------------------------------------------------------------
/** Gets the value of the LightCoolness property of the camera. Returns -1 if not successful. */
double MCamera::getLightCoolness()
{
  double coolness = 0;
  int result = Camera_LightCoolnessGet(this->Handle, &coolness);
  return (result == mtOK ? coolness : -1);
}

//----------------------------------------------------------------------------
/** Gets the value of the LightCoolness property of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setLightCoolness(double value)
{
  return Camera_LightCoolnessSet(this->Handle, value);
}

//----------------------------------------------------------------------------
int MCamera::getThermalHazard()
{
  return Camera_LastFrameThermalHazard(this->Handle);
}

//----------------------------------------------------------------------------
int MCamera::adjustCoolnessFromColorVector(mtHandle ColorVectorHandle)
{
  return Camera_LightCoolnessAdjustFromColorVector(this->Handle, ColorVectorHandle, 0);
}

//----------------------------------------------------------------------------
//image - 0 left, 1 right, 2 middle
int MCamera::getProjectionOnImage(int image, double XYZ[], double* x, double* y)
{
  return Camera_ProjectionOnImage(this->Handle, image, XYZ, x, y);
}

//----------------------------------------------------------------------------
int MCamera::getImages(unsigned char** *li, unsigned char** *ri)
{
  if (this->LeftImage == NULL)
  {
    this->LeftImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes());
  }
  if (this->RightImage == NULL)
  {
    this->RightImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes());
  }

  int r = Camera_ImagesGet(this->Handle, (unsigned char*)this->LeftImage, (unsigned char*)this->RightImage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    return r;
  }
  else
  {
    *li = (unsigned char**)this->LeftImage;
    *ri = (unsigned char**)this->RightImage;
    return mtOK;
  }
}

//----------------------------------------------------------------------------
int MCamera::getImages3(unsigned char** *li, unsigned char** *ri, unsigned char** *mi)
{
  if (this->LeftImage == NULL)
  {
    this->LeftImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes());
  }
  if (this->RightImage == NULL)
  {
    this->RightImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes());
  }
  if (this->MiddleImage == NULL)
  {
    this->MiddleImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes());
  }
  int r = Camera_ImagesGet3(this->Handle, (unsigned char*)this->LeftImage, (unsigned char*)this->RightImage, (unsigned char*)this->MiddleImage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    *mi = NULL;
    return r;
  }
  else
  {
    *li = (unsigned char**)this->LeftImage;
    *ri = (unsigned char**)this->RightImage;
    *mi = (unsigned char**)this->MiddleImage;
    return mtOK;
  }
}

//----------------------------------------------------------------------------
/** Gets the grabbed images. Returns true if successful. Returns false if not. */
int MCamera::getHalfSizeImages(unsigned char** *li, unsigned char** *ri, int xRes, int yRes)
{
  if (this->LeftImage == NULL)
  {
    this->LeftImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes() / 2);
  }
  if (this->RightImage == NULL)
  {
    this->RightImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes() / 2);
  }
  int r = Camera_HalfSizeImagesGet(this->Handle, (unsigned char*)this->LeftImage, (unsigned char*)this->RightImage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    return r;
  }
  else
  {
    *li = (unsigned char**)this->LeftImage;
    *ri = (unsigned char**)this->RightImage;
    return mtOK;
  }
}

//----------------------------------------------------------------------------
int MCamera::getHalfSizeImages3(unsigned char** *li, unsigned char** *ri, unsigned char** *mi, int xRes, int yRes)
{
  if (this->LeftImage == NULL)
  {
    this->LeftImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes() / 2);
  }
  if (this->RightImage == NULL)
  {
    this->RightImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes() / 2);
  }
  if (this->MiddleImage == NULL)
  {
    this->MiddleImage = (unsigned char*)calloc(sizeof(unsigned char), getXRes() * getYRes() / 2);
  }
  int r = Camera_HalfSizeImagesGet3(this->Handle, (unsigned char*)this->LeftImage, (unsigned char*)this->RightImage, (unsigned char*)this->MiddleImage);
  if (r != mtOK)
  {
    *li = NULL;
    *ri = NULL;
    *mi = NULL;
    return r;
  }
  else
  {
    *li = (unsigned char**)this->LeftImage;
    *ri = (unsigned char**)this->RightImage;
    *mi = (unsigned char**)this->MiddleImage;
    return mtOK;
  }
}

//----------------------------------------------------------------------------
int MCamera::grabFrame()
{
  return Camera_GrabFrame(this->Handle);
}

//----------------------------------------------------------------------------
bool MCamera::getHdrModeEnabled()
{
  bool IsEnabled = false;
  int result = Camera_HdrEnabledGet(this->Handle, &IsEnabled);
  return IsEnabled;
}

//----------------------------------------------------------------------------
/** Sets the AutoExposure property of the camera. Returns 0 if successful, -1 if not. */
int MCamera::setHdrModeEnabled(bool NewVal)
{
  return Camera_HdrEnabledSet(this->Handle, NewVal);
}
