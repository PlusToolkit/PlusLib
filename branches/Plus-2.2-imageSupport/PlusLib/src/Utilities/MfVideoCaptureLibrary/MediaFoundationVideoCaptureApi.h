/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

This file is subject to the Code Project Open Source License.
See http://www.codeproject.com/info/cpol10.aspx

Original work by Evgeny Pereguda
http://www.codeproject.com/Members/Evgeny-Pereguda

Original "videoInput" library at
http://www.codeproject.com/Articles/559437/Capturing-video-from-web-camera-on-Windows-7-and-8

The "videoInput" library has been adapted to fit within a namespace.

=========================================================Plus=header=end*/

#ifndef __MediaFoundationVideoCaptureApi_h
#define __MediaFoundationVideoCaptureApi_h

#include <guiddef.h>
#include <mfapi.h>
#include <string>
#include <vector>

struct IMFMediaSource;

namespace MfVideoCapture
{
  // Structure for collecting info about types of video, which are supported by current video device
  class MediaType
  {
  public:
    unsigned int MF_MT_FRAME_SIZE;
    unsigned int height;
    unsigned int width;
    unsigned int MF_MT_YUV_MATRIX;
    unsigned int MF_MT_VIDEO_LIGHTING;
    unsigned int MF_MT_DEFAULT_STRIDE;
    unsigned int MF_MT_VIDEO_CHROMA_SITING;

    GUID MF_MT_MAJOR_TYPE;
    GUID MF_MT_SUBTYPE;
    GUID MF_MT_AM_FORMAT_TYPE;

    unsigned int MF_MT_FIXED_SIZE_SAMPLES;
    unsigned int MF_MT_VIDEO_NOMINAL_RANGE;
    unsigned int MF_MT_FRAME_RATE;
    unsigned int MF_MT_FRAME_RATE_low;
    unsigned int MF_MT_PIXEL_ASPECT_RATIO;
    unsigned int MF_MT_PIXEL_ASPECT_RATIO_low;
    unsigned int MF_MT_ALL_SAMPLES_INDEPENDENT;
    unsigned int MF_MT_FRAME_RATE_RANGE_MIN;
    unsigned int MF_MT_FRAME_RATE_RANGE_MIN_low;
    unsigned int MF_MT_SAMPLE_SIZE;
    unsigned int MF_MT_VIDEO_PRIMARIES;
    unsigned int MF_MT_INTERLACE_MODE;
    unsigned int MF_MT_FRAME_RATE_RANGE_MAX;
    unsigned int MF_MT_FRAME_RATE_RANGE_MAX_low;

    std::wstring MF_MT_AM_FORMAT_TYPEName;
    std::wstring MF_MT_MAJOR_TYPEName;
    std::wstring MF_MT_SUBTYPEName;  

    MediaType();
    ~MediaType();
    void Clear();
  };

  // Structure for collecting info about one parameter of current video device
  class Parameter
  {
  public:
    long CurrentValue;
    long Min;
    long Max;
    long Step;
    long Default; 
    long Flag;

    Parameter();
  };

  // Structure for collecting info about 17 parameters of current video device
  class CaptureDeviceParameters
  {
  public:
    Parameter Brightness;
    Parameter Contrast;
    Parameter Hue;
    Parameter Saturation;
    Parameter Sharpness;
    Parameter Gamma;
    Parameter ColorEnable;
    Parameter WhiteBalance;
    Parameter BacklightCompensation;
    Parameter Gain;

    Parameter Pan;
    Parameter Tilt;
    Parameter Roll;
    Parameter Zoom;
    Parameter Exposure;
    Parameter Iris;
    Parameter Focus;
  };

  /// The only visible class for controlling of video devices in format singleton
  class MediaFoundationVideoCaptureApi
  {
  public:
    // Getting of static instance of videoInput class
    static MediaFoundationVideoCaptureApi& GetInstance(); 

    // Closing video device with deviceID
    void CloseDevice(unsigned int deviceID);

    // Setting callback function for emergency events(for example: removing video device with deviceID) with userData
    void SetEmergencyStopEvent(unsigned int deviceID, void *userData, void(*func)(int, void *));

    // Closing all devices
    void CloseAllDevices();

    // Tell a device to start recording
    bool StartRecording(unsigned int deviceID);

    // Tell a device to stop recording
    bool StopRecording(unsigned int deviceID);

    // Getting of parametrs of video device with deviceID
    CaptureDeviceParameters GetParameters(unsigned int deviceID);

    // Setting of parametrs of video device with deviceID
    void SetParameters(unsigned int deviceID, CaptureDeviceParameters parametrs);

    // List any existing capture devices
    unsigned int ListDevices();

    // Get a list of existing capture device names
    void GetDeviceNames(std::vector< std::wstring > &deviceNames);

    // Getting numbers of formats, which are supported by videodevice with deviceID
    unsigned int GetFormatCount(unsigned int deviceID);

    // Get active format for the given device
    unsigned int GetDeviceActiveFormat(unsigned int deviceID);

    // Getting width of image, which is getting from videodevice with deviceID
    unsigned int GetWidth(unsigned int deviceID);

    // Getting height of image, which is getting from videodevice with deviceID
    unsigned int GetHeight(unsigned int deviceID);

    // Getting name of videodevice with deviceID
    wchar_t *GetCaptureDeviceName(unsigned int deviceID);

    // Getting interface MediaSource for Media Foundation from videodevice with deviceID
    IMFMediaSource *GetMediaSource(unsigned int deviceID);

    // Getting format with id, which is supported by videodevice with deviceID 
    MediaType GetFormat(unsigned int deviceID, int unsigned id);

    // Checking of existence of the suitable video devices
    bool AreDevicesAccessable() const;

    // Checking of using the videodevice with deviceID
    bool IsDeviceSetup(unsigned int deviceID);

    // Checking of using MediaSource from videodevice with deviceID
    bool IsDeviceMediaSource(unsigned int deviceID);

    // Checking of using Raw Data of pixels from video device with deviceID
    bool IsDeviceRawDataSource(unsigned int deviceID);

    // Initialization of video device with deviceID by media type with id
    bool SetupDevice(unsigned int deviceID, unsigned int id = 0);

    // Initialization of video device with deviceID by width w, height h, fps idealFramerate and subtype
    bool SetupDevice(unsigned int deviceID, unsigned int w, unsigned int h, unsigned int idealFramerate = 30, GUID subtype = MFVideoFormat_YUY2);

    // Checking of receiving of new frame from video device with deviceID 
    bool IsFrameNew(unsigned int deviceID);

  private:
    bool AccessToDevices;
    MediaFoundationVideoCaptureApi(void);
    virtual ~MediaFoundationVideoCaptureApi(void);
    void UpdateListOfDevices();
  };

}

#endif