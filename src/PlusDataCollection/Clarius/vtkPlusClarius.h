/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) UBC Biomedical Signal and Image Computing Laboratory. All rights reserved.
    See License.txt for details.
    =========================================================Plus=header=end*/

#pragma once

#ifndef _VTKPLUSCLARIUS_H
#define _VTKPLUSCLARIUS_H

// Local Includes
#include "vtkPlusConfig.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUsDevice.h"

// System Includes
#include <thread>
#include <string>
#include <stdio.h>
#include <fstream>

// Clarius Includes
#include "listen.h"

// OpenCV includes
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

/*!
\class vtkPlusClarius
\brief Interface to the Clarius ultrasound scans
This class talks with a Clarius Scanner over the Clarius API.
Requires PLUS_USE_CLARIUS option in CMake.
 \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusClarius : public vtkPlusDevice
  /*vtkPlusCLARIUS is a subclass of vtkPlusDevice*/
{
public:
  vtkTypeMacro(vtkPlusClarius, vtkPlusDevice);
  /*! This is a singleton pattern New. There will only be ONE
  reference to a vtkPlusClarius object per process. Clients that
  call this must call Delete on the object so that the reference
  counting will work. The single instance will be unreferenced
  when the program exits. */
  static vtkPlusClarius* New();

  /*! return the singleton instance with no reference counting */
  static vtkPlusClarius* GetInstance();
  
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static const std::string BMODE_PORT_NAME;
  static const std::string RFMODE_PORT_NAME;
  static const std::string PARAMETERS_XML_ELEMENT_TAG;
  static const std::string PARAMETER_XML_ELEMENT_TAG;

  /*!
  Probe to see to see if the device is connected to the
  computer. This method should be overridden in subclasses.
  */
  PlusStatus Probe();

  /*! Hardware device SDK version. This method should be overridden in subclasses. */
  std::string GetSdkVersion();

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Perform any completion tasks once configured
   a multi-purpose function which is called after all devices have been configured,
   all inputs and outputs have been connected between devices,
   but before devices begin collecting data.
   This is the last chance for your device to raise an error about improper or insufficient configuration.
  */
  PlusStatus NotifyConfigured();
  bool IsTracker() const { return false; }
  /*! Return the reference frame */
  // PlusStatus GetToolReferenceFrameFromTrackedFrame(igsioTrackedFrame& aFrame, std::string& aToolReferenceFrameName);

  PlusStatus SetFrameHeight(int FrameHeight);
  PlusStatus SetFrameWidth(int FrameWidth);
  PlusStatus SetIpAddress(std::string IpAddress);
  PlusStatus SetTcpPort(unsigned int TcpPort);

protected:
  vtkPlusClarius();
  ~vtkPlusClarius();
  
  unsigned int TcpPort;
  int UdpPort;
  std::string IpAddress;
  
  std::string ImuSourceId;

  std::string VideoOutputChannelName;

  cv::Mat cvImage;
  vtkPlusDataSource *ImageSource;
  vtkPlusDataSource *ImuSource;
  long FrameNumber;

  int FrameWidth;
  int FrameHeight;
  std::string PathToSecKey; // path to security key;

  std::vector<ClariusPosInfo*> RawImus;

  PlusStatus InternalConnect();
  /* Device-specific on-update function */
  PlusStatus InternalUpdate();
  PlusStatus InternalDisconnect();
  PlusStatus InternalStartRecording();
  PlusStatus InternalStopRecording();
  PlusStatus CheckOutputChannels();
  PlusStatus ValidateInputChannels();
  
private:
  static vtkPlusClarius* instance;

  static void ErrorFn(const char *err);
  static void FreezeFn(int val);
  static void ProgressFn(int progress);
  static void NewImageFn(const void *newImage, const ClariusImageInfo *nfo, int npos, const ClariusPosInfo* pos);  
  static void SaveDataCallback(const void *newImage, const ClariusImageInfo *nfo, int npos, const ClariusPosInfo *pos);

  PlusStatus WritePosesToCsv(const ClariusImageInfo *nfo, int npos, const ClariusPosInfo* pos, int frameNum, double internalSystemTime, double systemTime);
  std::ofstream RawImuDataStream;
};
#endif //_VTKPLUSCLARIUS_H