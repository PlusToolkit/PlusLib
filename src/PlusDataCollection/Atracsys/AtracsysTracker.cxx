/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "AtracsysTracker.h"
#include <iostream>

using namespace Atracsys;

Tracker::Tracker()
{

}

Tracker::~Tracker()
{

}

ATRACSYS_ERROR Tracker::Connect()
{
  // initialize SDK
  this->FtkLib = ftkInit();

  if (this->FtkLib == NULL)
  {
    return ERROR_UNABLE_TO_GET_FTK_HANDLE;
  }

  // testing
  std::cout << "connected to tracker" << std::endl;
}

ATRACSYS_ERROR Tracker::EnableDataSending()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::DisableDataSending()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::EnableIRStrobe()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::DisableIRStrobe()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::GetDroppedFrameCount()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::ResetLostFrameCount()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::SetUserLEDState(int red, int green, int blue, int frequency)
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::GetUserLEDState(int& red, int& green, int& blue, int& frequency)
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::EnableUserLED()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::DisableUserLED()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::LoadMarkerGeometry(std::string filePath)
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::EnableWirelessMarkerPairing()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::DisableWirelessMarkerPairing()
{
  return ERROR_NONE;
}

ATRACSYS_ERROR Tracker::GetMarkersInFrame(std::vector<Marker>& markers)
{
  return ERROR_NONE;
}