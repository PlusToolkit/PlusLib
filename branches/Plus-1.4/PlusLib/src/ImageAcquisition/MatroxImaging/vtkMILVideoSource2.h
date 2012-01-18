/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Danielle Pace (Robarts Research Institute
 and The University of Western Ontario)
=========================================================================*/  

#ifndef __vtkMILVideoSource2_h
#define __vtkMILVideoSource2_h

#include "vtkPlusVideoSource.h"

// digitizer hardware
#define VTK_MIL_DEFAULT        0
#define VTK_MIL_METEOR         "M_SYSTEM_METEOR"
#define VTK_MIL_METEOR_II      "M_SYSTEM_METEOR_II"
#define VTK_MIL_METEOR_II_DIG  "M_SYSTEM_METEOR_II_DIG"
#define VTK_MIL_METEOR_II_CL   "M_SYSTEM_METEOR_II_CL"
#define VTK_MIL_METEOR_II_1394 "M_SYSTEM_METEOR_II_1394"
#define VTK_MIL_CORONA         "M_SYSTEM_CORONA"
#define VTK_MIL_CORONA_II      "M_SYSTEM_CORONA_II"
#define VTK_MIL_PULSAR         "M_SYSTEM_PULSAR"
#define VTK_MIL_GENESIS        "M_SYSTEM_GENESIS"
#define VTK_MIL_GENESIS_PLUS   "M_SYSTEM_GENESIS_PLUS"
#define VTK_MIL_ORION          "M_SYSTEM_ORION"
#define VTK_MIL_CRONOS         "M_SYSTEM_CRONOS"
#define VTK_MIL_ODYSSEY        "M_SYSTEM_ODYSSEY"
#define  VTK_MIL_HELIOS      "M_SYSTEM_HELIOS"
#define VTK_MIL_IRIS      "M_SYSTEM_IRIS"
#define VTK_MIL_CRONOPLUS    "M_SYSTEM_CRONOSPLUS"
#define VTK_MIL_MORPHIS      "M_SYSTEM_MORPHIS"
#define VTK_MIL_MORPHISQXT    "M_SYSTEM_MORPHISQXT"
#define VTK_MIL_SOLIOS      "M_SYSTEM_SOLIOS"

// video inputs: 
#define VTK_MIL_MONO          0
#define VTK_MIL_COMPOSITE     1
#define VTK_MIL_YC            2
#define VTK_MIL_RGB           3
#define VTK_MIL_DIGITAL       4

// video formats:
#define VTK_MIL_RS170         0
#define VTK_MIL_NTSC          1
#define VTK_MIL_CCIR          2 
#define VTK_MIL_PAL           3 
#define VTK_MIL_SECAM         4
#define VTK_MIL_NONSTANDARD   5       

/*!
\class vtkMILVideoSource2 
\brief Support Matrox Imaging Library frame grabbers. Currently not functional.

  vtkMILVideoSource2 provides an interface to Matrox Meteor, MeteorII
  and Corona video digitizers through the Matrox Imaging Library 
  interface.  In order to use this class, you must link VTK with mil.lib,
  MIL version 5.0 or higher is required.
  vtkMILVideoSource2 is an updated version of vtkMILVideoSource and uses
  vtkPlusVideoSource instead of vtkVideoSource

  Caveats:
  With some capture cards, if this class is leaked and ReleaseSystemResources 
  is not called, you may have to reboot before you can capture again.
  vtkVideoSource used to keep a global list and delete the video sources
  if your program leaked, due to exit crashes that was removed.

  This class was kept so that in case of MIL support is requested by someone
  this could be used as a starting point, however currently this class is
  not functional in Plus.

\ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkMILVideoSource2 : public vtkPlusVideoSource
{
public:
  static vtkMILVideoSource2 *New();
  vtkTypeRevisionMacro(vtkMILVideoSource2,vtkPlusVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Standard VCR functionality: Record incoming video. */
  void Record();

  /*! Standard VCR functionality: Stop recording. */
  void Stop();

  /*! Grab a single video frame */
  void Grab();
 
  /*! Request a particular frame size (set the third value to 1) */
  void SetFrameSize(int x, int y, int z);
  /*! Request a particular frame size (set the third value to 1) */
  virtual void SetFrameSize(int dim[3]) 
  { 
    this->SetFrameSize(dim[0], dim[1], dim[2]); 
  };
  
  /*! Request a particular output format (default: VTK_RGB) */
  void SetOutputFormat(int format);

  /*! Set/Get the video channel */
  virtual void SetVideoChannel(int channel);
  vtkGetMacro(VideoChannel, int);

  /*! Set the video format */
  virtual void SetVideoFormat(int format);
  /*! Set the video format */
  void SetVideoFormatToNTSC() { this->SetVideoFormat(VTK_MIL_NTSC); };
  /*! Set the video format */
  void SetVideoFormatToPAL() { this->SetVideoFormat(VTK_MIL_PAL); };
  /*! Set the video format */
  void SetVideoFormatToSECAM() { this->SetVideoFormat(VTK_MIL_SECAM); };
  /*! Set the video format */
  void SetVideoFormatToRS170() { this->SetVideoFormat(VTK_MIL_RS170); };
  /*! Set the video format */
  void SetVideoFormatToCCIR() { this->SetVideoFormat(VTK_MIL_CCIR); };
  /*! Set the video format */
  void SetVideoFormatToNonStandard() 
  { 
    this->SetVideoFormat(VTK_MIL_NONSTANDARD); 
  };
  /*! Get the video format */
  vtkGetMacro(VideoFormat,int);
  
  /*! Set the video input */
  virtual void SetVideoInput(int input);
  /*! Set the video input */
  void SetVideoInputToMono() { this->SetVideoInput(VTK_MIL_MONO); };
  /*! Set the video input */
  void SetVideoInputToComposite() {this->SetVideoInput(VTK_MIL_COMPOSITE);};
  /*! Set the video input */
  void SetVideoInputToYC() { this->SetVideoInput(VTK_MIL_YC); };
  /*! Set the video input */
  void SetVideoInputToRGB() { this->SetVideoInput(VTK_MIL_RGB); };
  /*! Set the video input */
  void SetVideoInputToDigital() { this->SetVideoInput(VTK_MIL_DIGITAL); };
  /*! Get the video input */
  vtkGetMacro(VideoInput,int);

  /*!
    Set the video levels for composite/SVideo: the valid ranges are: 
    Contrast [0.0,2.0] 
  */
  virtual void SetContrastLevel(float contrast);
  /*! Get the contrast level */
  vtkGetMacro(ContrastLevel,float);
  /*!
    Set the video levels for composite/SVideo: the valid ranges are: 
    Brighness [0.0,255.0] 
  */
  virtual void SetBrightnessLevel(float brightness);
  /*! Get the brightness level */
  vtkGetMacro(BrightnessLevel,float);
  /*!
    Set the video levels for composite/SVideo: the valid ranges are: 
    Hue [-0.5,0.5] 
  */
  virtual void SetHueLevel(float hue);
  /*! Get the hue level */
  vtkGetMacro(HueLevel,float);
  /*!
    Set the video levels for composite/SVideo: the valid ranges are: 
    Saturation [0.0,2.0] 
  */
  virtual void SetSaturationLevel(float saturation);
  /*! Get the saturation level */
  vtkGetMacro(SaturationLevel,float);

  /*! Set the video levels for monochrome/RGB: valid values are between 0.0 and 255.0 */
  virtual void SetBlackLevel(float value);
  /*! Get the video levels for monochrome/RGB: valid values are between 0.0 and 255.0 */
  virtual float GetBlackLevel() { return this->BlackLevel; };
  /*! Set the video levels for monochrome/RGB: valid values are between 0.0 and 255.0 */
  virtual void SetWhiteLevel(float value);
  /*! Get the video levels for monochrome/RGB: valid values are between 0.0 and 255.0 */
  virtual float GetWhiteLevel() { return this->WhiteLevel; };

  /*!
    Set the system which you want use.  If you don't specify a system,
    then your primary digitizer will be autodetected.
  */
  vtkSetStringMacro(MILSystemType);
  /*!
    Get the system which you want use.  If you don't specify a system,
    then your primary digitizer will be autodetected.
  */ 
  vtkGetStringMacro(MILSystemType);
  /*! Set the system which you want use */
  void SetMILSystemTypeToMeteor() { this->SetMILSystemType(VTK_MIL_METEOR); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToMeteorII() { this->SetMILSystemType(VTK_MIL_METEOR_II); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToMeteorIIDig() { this->SetMILSystemType(VTK_MIL_METEOR_II_DIG); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToMeteorIICL() { this->SetMILSystemType(VTK_MIL_METEOR_II_CL); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToMeteorII1394() { this->SetMILSystemType(VTK_MIL_METEOR_II_1394); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToCorona() { this->SetMILSystemType(VTK_MIL_CORONA); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToCoronaII() { this->SetMILSystemType(VTK_MIL_CORONA_II); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToPulsar() { this->SetMILSystemType(VTK_MIL_PULSAR); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToGenesis() { this->SetMILSystemType(VTK_MIL_GENESIS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToGenesisPlus() { this->SetMILSystemType(VTK_MIL_GENESIS_PLUS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToOrion() { this->SetMILSystemType(VTK_MIL_ORION); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToCronos() { this->SetMILSystemType(VTK_MIL_CRONOS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToOdyssey() { this->SetMILSystemType(VTK_MIL_ODYSSEY); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToHelios() { this->SetMILSystemType(VTK_MIL_HELIOS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToIris() { this->SetMILSystemType(VTK_MIL_IRIS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToCronoPlus() { this->SetMILSystemType(VTK_MIL_CRONOPLUS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToMorphis() { this->SetMILSystemType(VTK_MIL_MORPHIS); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToMorphisQxT() { this->SetMILSystemType(VTK_MIL_MORPHISQXT); };
  /*! Set the system which you want use */
  void SetMILSystemTypeToSolis() { this->SetMILSystemType(VTK_MIL_SOLIOS); };

  /*! Set the system number if you have multiple systems of the same type */
  vtkSetMacro(MILSystemNumber,int);
  /*! Get the system number if you have multiple systems of the same type */
  vtkGetMacro(MILSystemNumber,int);

  /*! Set the DCF filename for non-standard video formats */
  vtkSetStringMacro(MILDigitizerDCF);
  /*! Get the DCF filename for non-standard video formats */
  vtkGetStringMacro(MILDigitizerDCF);

  /*! Set the digitizer number for systems with multiple digitizers */
  vtkSetMacro(MILDigitizerNumber,int);
  /*! Get the digitizer number for systems with multiple digitizers */
  vtkGetMacro(MILDigitizerNumber,int);

  /*! Set whether to display MIL error messages (default on) */
  virtual void SetMILErrorMessages(int yesno);
  /*! Set whether to display MIL error messages (default on) */
  vtkBooleanMacro(MILErrorMessages,int);
  /*! Get whether to display MIL error messages (default on) */
  vtkGetMacro(MILErrorMessages,int);

  /*! Allows fine-grained control */
  vtkSetMacro(MILAppID,long);
  /*! Allows fine-grained control */
  vtkGetMacro(MILAppID,long);
  /*! Allows fine-grained control */
  vtkSetMacro(MILSysID,long);
  /*! Allows fine-grained control */
  vtkGetMacro(MILSysID,long);
  /*! Allows fine-grained control */
  vtkGetMacro(MILDigID,long);
  /*! Allows fine-grained control */
  vtkGetMacro(MILBufID,long);

  /*!
    Initialize the driver (this is called automatically when the
    first grab is done).
  */
  void Initialize();

  /*! Free the driver (this is called automatically inside the destructor) */
  void ReleaseSystemResources();

  /*! For internal use only */
  void *OldHookFunction;
  /*! For internal use only */
  void *OldUserDataPtr;
  /*! For internal use only */
  int FrameCounter;
  /*! For internal use only */
  int ForceGrab;
  /*! For internal use only */
  void InternalGrab();

protected:
  vtkMILVideoSource2();
  ~vtkMILVideoSource2();

  virtual void AllocateMILDigitizer();
  virtual void AllocateMILBuffer();

  virtual void *MILInterpreterForSystem(const char *system);
  char *MILInterpreterDLL;

  int VideoChannel;
  int VideoInput;
  int VideoInputForColor;
  int VideoFormat;

  float ContrastLevel;
  float BrightnessLevel;
  float HueLevel;
  float SaturationLevel;

  float BlackLevel;
  float WhiteLevel;

  int FrameMaxSize[2];

  long MILAppID;
  long MILSysID;
  long MILDigID;
  long MILBufID;
  // long MILDispBufID;
  // long MILDispID;

  char *MILSystemType;
  int MILSystemNumber;

  int MILDigitizerNumber;
  char *MILDigitizerDCF;

  int MILErrorMessages;

  int MILAppInternallyAllocated;
  int MILSysInternallyAllocated;

  int FatalMILError;

  /*!
    Method for updating the virtual clock that accurately times the
    arrival of each frame, more accurately than is possible with
    the system clock alone because the virtual clock averages out the
    jitter.
  */
  double CreateTimeStampForFrame(unsigned long frame);

  double LastTimeStamp;
  unsigned long LastFrameCount;
  double EstimatedFramePeriod;
  double NextFramePeriod;

private:
  vtkMILVideoSource2(const vtkMILVideoSource2&);  // Not implemented.
  void operator=(const vtkMILVideoSource2&);  // Not implemented.
};

#endif
