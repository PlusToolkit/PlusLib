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

// Command line processing functions.
bool ParseCommandLine( char* szCmdLine[], nvOptions* options );

#endif