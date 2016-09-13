#ifndef NVSDIUTIL_H
#define NVSDIUTIL_H

#if WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/wglext.h>

#include "nvapi.h"

std::string SignalFormatToString( NVVIOSIGNALFORMAT format );
std::string DataFormatToString( NVVIODATAFORMAT format );
std::string SyncStatusToString( NVVIOSYNCSTATUS status );
std::string SyncSourceToString( NVVIOSYNCSOURCE source );
std::string ComponentSamplingFormatToString( NVVIOCOMPONENTSAMPLING sampling );
std::string ColorSpaceToString( NVVIOCOLORSPACE space );
std::string LinkIDToString( NVVIOLINKID id );

#endif