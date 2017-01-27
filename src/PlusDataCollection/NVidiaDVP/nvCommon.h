/***************************************************************************\
|*                                                                           *|
|*      Copyright 2007 NVIDIA Corporation.  All rights reserved.             *|
|*                                                                           *|
|*   NOTICE TO USER:                                                         *|
|*                                                                           *|
|*   This source code is subject to NVIDIA ownership rights under U.S.       *|
|*   and international Copyright laws.  Users and possessors of this         *|
|*   source code are hereby granted a nonexclusive, royalty-free             *|
|*   license to use this code in individual and commercial software.         *|
|*                                                                           *|
|*   NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE     *|
|*   CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR         *|
|*   IMPLIED WARRANTY OF ANY KIND. NVIDIA DISCLAIMS ALL WARRANTIES WITH      *|
|*   REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF         *|
|*   MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR          *|
|*   PURPOSE. IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL,            *|
|*   INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES          *|
|*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN      *|
|*   AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING     *|
|*   OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE      *|
|*   CODE.                                                                   *|
|*                                                                           *|
|*   U.S. Government End Users. This source code is a "commercial item"      *|
|*   as that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting       *|
|*   of "commercial computer  software" and "commercial computer software    *|
|*   documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995)   *|
|*   and is provided to the U.S. Government only as a commercial end item.   *|
|*   Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through        *|
|*   227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the       *|
|*   source code with only those rights set forth herein.                    *|
|*                                                                           *|
|*   Any use of this source code in individual and commercial software must  *|
|*   include, in the user documentation and internal comments to the code,   *|
|*   the above Disclaimer and U.S. Government End Users Notice.              *|
|*                                                                           *|
|*                                                                           *|
\***************************************************************************/

#ifndef NVCOMMON_H
#define NVCOMMON_H

#include "nvapi.h"

enum eTestPattern
{
  TEST_PATTERN_RGB_COLORBARS_100,
  TEST_PATTERN_RGB_COLORBARS_75,
  TEST_PATTERN_YCRCB_COLORBARS,
  TEST_PATTERN_COLORBARS8_75,
  TEST_PATTERN_COLORBARS8_100,
  TEST_PATTERN_COLORBARS10_75,
  TEST_PATTERN_COLORBARS10_100,
  TEST_PATTERN_FRAME,
  TEST_PATTERN_LOGO,
  TEST_PATTERN_RAMP8,
  TEST_PATTERN_RAMP16,
  TEST_PATTERN_BAR,
  TEST_PATTERN_LINES,
  TEST_PATTERN_SPHERE
};


// Definition of command line option structure.
struct nvOptions
{
  NVVIOSIGNALFORMAT videoFormat;
  NVVIODATAFORMAT dataFormat;
  NVVIOCOMPSYNCTYPE syncType;
  NVVIOSYNCSOURCE syncSource;
  eTestPattern testPattern;
  bool syncEnable;
  bool frameLock;
  int numFrames;
  int repeat;
  int gpu; //output GPU
  bool block;
  bool videoInfo;
  bool fps;
  int fsaa;
  int hDelay;
  int vDelay;
  int flipQueueLength;
  bool field;
  bool console;
  bool log;
  bool cscEnable;
  double cscOffset[3];
  double cscScale[3];
  float cscMatrix[3][3];
  float gamma[3];
  bool alphaComp;
  bool yComp;
  bool crComp;
  bool cbComp;
  int yCompRange[4];
  int crCompRange[4];
  int cbCompRange[4];
  int x;
  int y;
  int width;
  int height;
  char filename[100];
  char audioFile[100];
  int audioChannels;
  int audioBits;
  //Capture settings//
  int captureGPU; //capture GPU
  int captureDevice; //capture card number
  bool dualLink;
  NVVIOCOMPONENTSAMPLING sampling;
  int bitsPerComponent;
  bool expansionEnable;
  bool fullScreen;
};

#endif