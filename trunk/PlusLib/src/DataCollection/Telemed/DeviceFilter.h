/*=========================================================================

Program:   TelemedUltrasound
Module:
Language:  C++
Date:      $Date: 2005/06/06 21:50:34 $
Version
Author:    Julien Jomier, Vincent Le Digarcher

Copyright (c) 2002 CADDLab @ UNC. All rights reserved.
See itkUNCCopyright.txt for details.

Copyright © 1992-2005, TELEMED Ltd.
Vilnius/Lithuania
http://www.telemed.lt/


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/







////////////////////////////////////////////////////////////
//                                                        //
// DeviceFilter.h: interface for the DeviceFilter class. //
//                                                        //
////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEFILTER_H__2B9F3654_4AC9_4A10_8940_AEC4EEF5D1EB__INCLUDED_)
#define AFX_DEVICEFILTER_H__2B9F3654_4AC9_4A10_8940_AEC4EEF5D1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include <Strmif.h>

// class DeviceFilter implements storage for ultrasonic devices.
// class contains IBaseFilter interface to the device proxy plug-in
// device name and device path.
// Device path is a unique string that a system defines for each device.

class DeviceFilter
{
public:
  IBaseFilter* m_pBaseFilter;
  std::string m_strFilterName;
  std::string m_strFilterPath;
  DeviceFilter(DeviceFilter &deviceFilter);
  DeviceFilter();
  virtual ~DeviceFilter();

  bool m_fChecked;
};

#define RELEASE_INTERFACE(pObj) \
  if(pObj) {\
  pObj->Release(); pObj = NULL;}

#endif // !defined(AFX_DEVICEFILTER_H__2B9F3654_4AC9_4A10_8940_AEC4EEF5D1EB__INCLUDED_)
