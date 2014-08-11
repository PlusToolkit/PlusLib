/*=========================================================================

Program:   TelemedUltrasound
Module:
Language:  C++
Date:      $Date: 2005/06/06 21:50:15 $
Version:
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





// DeviceFilter.cpp: implementation of the DeviceFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "DeviceFilter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DeviceFilter::DeviceFilter()
{
  m_pBaseFilter = NULL;
  m_fChecked = true;
}

DeviceFilter::~DeviceFilter()
{
  RELEASE_INTERFACE(m_pBaseFilter);
}

// copy constructor
DeviceFilter::DeviceFilter(DeviceFilter &deviceFilter)
{
  m_strFilterName = deviceFilter.m_strFilterName;
  m_strFilterPath = deviceFilter.m_strFilterPath;
  m_pBaseFilter = deviceFilter.m_pBaseFilter;
  m_fChecked = true;

  if(m_pBaseFilter)
    m_pBaseFilter->AddRef();
}
