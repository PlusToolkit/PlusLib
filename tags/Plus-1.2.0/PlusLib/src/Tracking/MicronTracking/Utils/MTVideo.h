/**************************************************************
*
*     MTVideo public header
*   
*     Written by: Claudio Gatti
*     Copyright Claron Technology 2000-2003
*
***************************************************************/

#define MTVIDEOAPI_EXPORTS  // Since MtVideo is now embedded in MTC make this permanently exposrt


#ifdef MTVIDEOAPI_EXPORTS
#  undef MTVIDEOAPI_EXPORTS
#  ifdef _WIN32
#define MTVIDEO_API extern "C" __declspec(dllexport)
#  else
#    define MTVIDEO_API extern
#  endif
#else        /* else ! MTVIDEOAPI_EXPORTS */
#  ifdef _WIN32
#define MTVIDEO_API extern "C" __declspec(dllimport)
#  else
#    define MTVIDEO_API 
#  endif
#endif        /* MTVIDEOAPI_EXPORTS */

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */
                                                                                

#  ifdef _WIN32
#define STDCALL int _stdcall
#else
#define STDCALL int 
#endif

#define MTVERSION 16

enum mtvCompletionCode {
  mtvOK=1,
  mtvDigiclopsError = -1,
  mtvWrongDigiclopsVersion= -2,
  mtvInvalidSerialNumber = -3,
  mtvNoCamerasPresent = -4,
  mtvMaxCamerasExceeded = -5,
  mtvResolutionNotSupported = -6,
  mtvSetFramerateError = -7
};


MTVIDEO_API STDCALL MTsetFirstCompatibleMediaFormat( int ix, int minX, int minY, int bitCount );

MTVIDEO_API STDCALL MTsetAllUncompressedMediaFormat( int x, int y, int bitcount);

MTVIDEO_API STDCALL MTsetUncompressedMediaFormat( int ix, int x, int y, int bitcount);

MTVIDEO_API STDCALL MTgrabBuffer(int ix, void *buffer, int bufferix);

MTVIDEO_API STDCALL MTgrabBothBuffers(int ix, void *lbuffer, void *rbuffer);

MTVIDEO_API STDCALL MTgrabRawBuffer( int ix, void *buffer);

MTVIDEO_API STDCALL MTgrabRawBufferSize(int ix, int *pBufferSize);

MTVIDEO_API STDCALL MTgrabBufferSize(int ix, int *pBufferSize);

MTVIDEO_API STDCALL MTgrabBufferDimensions(int ix, int *pX, int *pY);

MTVIDEO_API STDCALL MTflipHorizontal(int ix);
MTVIDEO_API STDCALL MTflipVertical(int ix);

MTVIDEO_API STDCALL MTsetColorEnable(int ix,int lValue);

MTVIDEO_API STDCALL MTsetGamma(int ix, int lValue);
MTVIDEO_API STDCALL MTgetGamma(int ix, int *lValue, int *lMode);

MTVIDEO_API STDCALL MTsetWhiteBalance(int ix, int lValue);
MTVIDEO_API STDCALL MTgetWhiteBalance(int ix, int *lValue, int *lMode);
MTVIDEO_API STDCALL MTsetAutoWhiteBalance(int ix);

MTVIDEO_API STDCALL MTsetExposure(int ix, int lValue);
MTVIDEO_API STDCALL MTgetExposure(int ix, int *lValue, int *lMode);
MTVIDEO_API STDCALL MTsetAutoExposure(int ix);

MTVIDEO_API STDCALL MTsetSharpness(int ix, int lValue);
MTVIDEO_API STDCALL MTgetSharpness(int ix, int *lValue, int *lMode);

MTVIDEO_API STDCALL MTsetAutoGain(int ix);
MTVIDEO_API STDCALL MTsetGain(int ix, int lValue);
MTVIDEO_API STDCALL MTgetGain(int ix, int *lValue, int *lMode);

MTVIDEO_API STDCALL MTsetShutter(int ix, int lValue);
MTVIDEO_API STDCALL MTgetShutter(int ix, int *lValue, int *lMode);
MTVIDEO_API STDCALL MTsetAutoShutter(int ix);


MTVIDEO_API STDCALL MTsetContrast(int ix, int lValue);
MTVIDEO_API STDCALL MTgetContrast(int ix, int *lValue, int *lMode);
MTVIDEO_API STDCALL MTsetAutoContrast(int ix);

MTVIDEO_API STDCALL MTsetBrightness(int ix, int lValue);
MTVIDEO_API STDCALL MTgetBrightness(int ix, int *lValue, int *lMode);
MTVIDEO_API STDCALL MTsetAutoBrightness(int ix);

MTVIDEO_API STDCALL MTsetSaturation(int ix, int lValue);
MTVIDEO_API STDCALL MTgetSaturation(int ix, int *lValue, int *lMode);
MTVIDEO_API STDCALL MTsetAutoSaturation(int ix);

MTVIDEO_API STDCALL MTtogglePreview(int ix);

MTVIDEO_API STDCALL MTexit(void);
MTVIDEO_API STDCALL MTinit( int minX, int minY, int bitCount,
      int numCam , char *driverNamePtr);

MTVIDEO_API STDCALL MTgetCameraPropertyRange(int ix, int MtCameraProperty, int *lExists, int *lMin, int *lMax, int *lDefault, int *lAuto, int *lManual);

MTVIDEO_API STDCALL MTgetHandleFromSerialNumber(int SerialNum, int *ix);
MTVIDEO_API STDCALL MTgetCurrentSerialNumber(int num, int *piSerialNum);
MTVIDEO_API STDCALL MTgetCapFilterSerial( int  num, int *serialNum);
MTVIDEO_API STDCALL MTgetCapFilter( int  num, char *capFilterFriendlyName, int *ptrLen);
MTVIDEO_API STDCALL MTnumCapFilters(char  *driverNamePtr);
MTVIDEO_API STDCALL MTenumMediaFormat( int ix );
MTVIDEO_API STDCALL MTGetCameraRegister( int ix, unsigned long reg, unsigned long *val );
MTVIDEO_API STDCALL MTGetTemperature( int ix, double *temp );
MTVIDEO_API STDCALL MTGetSecondsFromPowerup( int ix, unsigned long *secs );
MTVIDEO_API STDCALL MTGetFirmwareVersion(int ix, int *major, int *minor, int* type, int * rev );

#  ifdef __cplusplus
}
#endif
