/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __AtracsysConstants_h
#define __AtracsysConstants_h

namespace Atracsys
{
  const int FTK_MAX_FIDUCIALS = 4;

  enum ATRACSYS_ERROR
  {
    SUCCESS = 0,
    ERROR_UNABLE_TO_GET_FTK_HANDLE,
    ERROR_NO_DEVICE_CONNECTED,
    ERROR_UNABLE_TO_LOAD_MARKER,
	  ERROR_FAILURE_TO_LOAD,
    ERROR_OPTION_NOT_AVAILABLE_ON_FTK,
    ERROR_FAILED_TO_SET_OPTION,
    ERROR_FAILED_TO_LOAD_GEOMETRY,
    ERROR_FAILED_TO_CLOSE_SDK
  };

  enum OPTIONS
  {
    OPTION_DATA_SENDING = 20,
    OPTION_WIRELESS_PAIRING_ENABLE = 40,
    OPTION_IR_STROBE = 50,
    OPTION_LOST_FRAME_COUNT = 60,
    OPTION_RESET_LOST_FRAME_COUNT = 69,
    OPTION_LED_RED_COMPONENT = 90,
    OPTION_LED_GREEN_COMPONENT = 91,
    OPTION_LED_BLUE_COMPONENT = 92,
    OPTION_LED_FREQUENCY = 93,
    OPTION_LED_ENABLE = 94,
    OPTION_EPIPOLAR_MIN_DISTANCE = 2001,
    OPTION_MATCHING_TOLERANCE = 3002,
    OPTION_MAX_REGISTRATION_MEAN_ERROR = 3003,
    OPTION_MAX_MISSION_POINTS = 3004,
    OPTION_MAX_TRACKING_RANGE = 3005,
    OPTION_ONBOARD_PROCESSING = 6000,
    OPTION_IMAGE_STREAMING = 6003,
    OPTION_WIRELESS_MARKER_PAIRING = 7000
  };

  enum DEVICE_TYPE
  {
    UNKNOWN = 0,
    SPRYTRACK_180 = 1,
    FUSIONTRACK_500 = 2,
    FUSIONTRACK_250  = 3
  };

  enum IMAGE_PROCESSING_TYPE
  {
    PROCESSING_ONBOARD,
    PROCESSING_SEND_IMAGES
  };
}
#endif