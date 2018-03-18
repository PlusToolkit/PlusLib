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
    ERROR_NONE = 0,
    ERROR_UNABLE_TO_GET_FTK_HANDLE = 1
  };

  enum OPTIONS
  {
    OPTION_DATA_SENDING = 20,
    OPTION_WIRELESS_PAIRING_ENABLE = 40,
    OPTION_IR_STROBE = 50,
    OPTION_LOST_FRAME_COUNT = 60,
    OPTION_RESET_LOST_FRAME_COUNT = 69,
    OPTION_LED_RED_COMPONENT = 90,
    OPTION_LED_BLUE_COMPONENT = 91,
    OPTION_LED_GREEN_COMPONENT = 92,
    OPTION_LED_ENABLE = 93,
    OPTION_EPIPOLAR_MIN_DISTANCE = 2001,
    OPTION_MATCHING_TOLERANCE = 3002,
    OPTION_MAX_REGISTRATION_MEAN_ERROR = 3003,
    OPTION_MAX_MISSION_POINTS = 3004,
    OPTION_MAX_TRACKING_RANGE = 3005
  };
}
#endif