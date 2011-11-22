/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/

#ifndef __UTILITYFUNCTIONS_H__
#define __UTILITYFUNCTIONS_H__

#include <vector>
#include <string>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
//#include "FL/filename.H"
#include <iostream>
#include "math.h"

#ifdef _DEBUG
  #pragma warning(disable:4786)
#endif



#define PI 3.1415926535897932384626433832795

#define Cos6Degs 0.994522
#define Cos8Degs 0.990268
#define Cos10Degs 0.985
#define Cos15Degs 0.966
#define Cos20Degs 0.94
#define Cos25Degs 0.906
#define Cos30Degs 0.866
#define Cos40Degs 0.766
#define UnknownX 1E20

namespace mtUtils
{
  enum    mti_Error{
            MTI_FAIL = 0,
            MTI_SUCCESS = 1,
            MTI_CAMERA_NOT_FOUND = 2,
            MTI_GRAB_FRAME_ERROR = 3,
            MTI_MARKER_CAPTURED = 4,
            MTI_NO_MARKER_CAPTURED = 5,
            MTI_CAMERA_INDEX_OUT_OF_RANGE = 6
          };
  void getCurrPath(char* currPath);
  void getFileNamesFromDirectory(std::vector<std::string> &fileNames, char* dir, bool returnCompletePath);
  int  renameFile(const std::string& oldName, const std::string& newName, char* dir);
  int  removeFile(const std::string& fileName, char* dir); 
  double acos(double x);
  std::string handleErrors(int errorNum);

};

#endif
