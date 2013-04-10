/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.
Authors include: Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
Siddharth Vikal (Queen's University, Kingston, Ontario, Canada)
=========================================================================*/  

#ifndef __vtkSonixVideoSource_h
#define __vtkSonixVideoSource_h

#include "PlusConfigure.h"
#include "vtkPlusDevice.h"
#include "ulterius.h"
#include "ulterius_def.h"
#include "ImagingModes.h" // Ulterius imaging modes

//BTX

class uDataDesc;
class ulterius;

class VTK_EXPORT vtkSonixVideoSource;

/*!
\class vtkSonixVideoSourceCleanup 
\brief Class that cleans up (deletes singleton instance of) vtkSonixVideoSource when destroyed
\ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkSonixVideoSourceCleanup
{
public:
  vtkSonixVideoSourceCleanup();
  ~vtkSonixVideoSourceCleanup();
};
//ETX

/*!
  \class vtkSonixVideoSource 
  \brief VTK interface for video input from Ultrasonix machine

  vtkSonixVideoSource is a class for providing video input interfaces between VTK and Ultrasonix machine.
  The goal is to provide the ability to be able to do acquisition
  in various imaging modes, buffer the image/volume series being acquired
  and stream the frames to output. 
  Note that the data coming out of the Sonix rp through ulterius is always RGB
  This class talks to Ultrasonix's Ulterius SDK for executing the tasks 
  Parameter setting doesn't work with Ulterius-2.x

  Usage:
  sonixGrabber->SetSonixIP("130.15.7.212");
  sonixGrabber->SetImagingMode(0);
  sonixGrabber->SetAcquisitionDataType(udtBPost);
  sonixGrabber->Record();  
  imageviewer->SetInput(sonixGrabber->GetOutput());
  See vtkSonixVideoSourceTest1.cxx for more details

  \ingroup PlusLibImageAcquisition
*/

class VTK_EXPORT vtkSonixVideoSource : public vtkPlusDevice
{
public:
  vtkTypeRevisionMacro(vtkSonixVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 

  /*!
    This is a singleton pattern New.  There will only be ONE
    reference to a vtkOutputWindow object per process.  Clients that
    call this must call Delete on the object so that the reference
    counting will work.   The single instance will be unreferenced when
    the program exits.
  */
  static vtkSonixVideoSource* New();
  
  /*! Return the singleton instance with no reference counting. */
  static vtkSonixVideoSource* GetInstance();

  /*!
    Supply a user defined output window. Call ->Delete() on the supplied
    instance after setting it.
  */
  static void SetInstance(vtkSonixVideoSource *instance);

  //BTX
  /*!
    Use this as a way of memory management when the
    program exits the SmartPointer will be deleted which
    will delete the Instance singleton
  */
  static vtkSonixVideoSourceCleanup Cleanup;
  //ETX

  /*! Read main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write main configuration from/to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Get the IP address of the Ultrasonix host machine */
  vtkSetStringMacro(SonixIP); 
  /*! Set the IP address of the Ultrasonix host machine */
  vtkGetStringMacro(SonixIP); 

  /*! Set ultrasound transmitter frequency (MHz) */
  PlusStatus SetFrequency(int aFrequency);
  /*! Get ultrasound transmitter frequency (MHz) */
  PlusStatus GetFrequency(int& aFrequency);

  /*! Set the depth (mm) of B-mode ultrasound */
  PlusStatus SetDepth(int aDepth);
  /*! Get the depth (mm) of B-mode ultrasound */
  PlusStatus GetDepth(int& aDepth);

  /*! Set the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetGain(int aGain);
  /*! Get the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetGain(int& aGain);

  /*! Set the DynRange (dB) of B-mode ultrasound */
  PlusStatus SetDynRange(int aDynRange);
  /*! Get the DynRange (dB) of B-mode ultrasound */
  PlusStatus GetDynRange(int& aDynRange);

  /*! Set the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetZoom(int aZoom);
  /*! Get the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetZoom(int& aZoom);

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSector(int aSector);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSector(int& aSector);

  /*! Set the CompressionStatus to 0 for compression off, 1 for compression on. (Default: off) */
  PlusStatus SetCompressionStatus(int aCompressionStatus);
  /*! Get the CompressionStatus to 0 for compression off, 1 for compression on. */
  PlusStatus GetCompressionStatus(int& aCompressionStatus);

  /*! Set the Timeout (ms) value for network function calls. */
  PlusStatus SetTimeout(int aTimeout);
  
  /*!
    Request a particular data type from sonix machine by means of a bitmask.
    The mask must be applied before any data can be acquired via realtime imaging or cine retrieval

    udtScreen = 0x00000001,   // Screen
    udtBPre = 0x00000002,     // B Pre Scan Converted
    udtBPost = 0x00000004,    // B Post Scan Converted (8 bit)
    udtBPost32 = 0x00000008,  // B Post Scan Converted (32 bit)
    udtRF = 0x00000010,       // RF
    udtMPre = 0x00000020,     // M Pre Scan Converted
    udtMPost = 0x00000040,    // M Post Scan Converted
    udtPWRF = 0x00000080,     // PW RF
    udtPWSpectrum = 0x00000100,           
    udtColorRF = 0x00000200,              
    udtColorCombined = 0x00000400,
    udtColorVelocityVariance = 0x00000800,
    udtElastoCombined = 0x00002000, // Elasto + B-image (32 bit)
    udtElastoOverlay = 0x00004000,  // Elasto Overlay (8 bit)
    udtElastoPre = 0x00008000,      // Elasto Pre Scan Coverted (8 bit)
    udtECG = 0x00010000,
    udtGPS = 0x00020000,
    udtPNG = 0x10000000
  */
  PlusStatus SetAcquisitionDataType(int aAcquisitionDataType);
  /*! Get acquisition data type  bitmask. */
  PlusStatus GetAcquisitionDataType(int &acquisitionDataType);

  /*!
    Request a particular mode of imaging
    Usable values are described in ImagingModes.h (default: B-mode)  
    BMode = 0,
    MMode = 1,
    ColourMode = 2,
    PwMode = 3,
    TriplexMode = 4,
    PanoMode = 5,
    DualMode = 6,
    QuadMode = 7,
    CompoundMode = 8,
    DualColourMode = 9,
    DualCompoundMode = 10,
    CwMode = 11,
    RfMode = 12,
    ColorSplitMode = 13,
    F4DMode = 14,
    TriplexCwMode = 15,
    ColourMMode = 16,
    ElastoMode = 17,
    SDUVMode = 18,
    AnatomicalMMode = 19,
    ElastoComparativeMode = 20,
    FusionMode = 21,
    VecDopMode = 22,
    BiplaneMode = 23,
    ClinicalRfMode = 24,
    RfCompoundMode = 25,
    SHINEMode = 26,
    ColourRfMode = 27,
  */
  PlusStatus SetImagingMode(int mode);
  /*! Get current imaging mode */
  PlusStatus GetImagingMode(int & mode);

  /*! 
    Set the time required for setting up the connection.
    The value depends on the probe type, typical values are between 2000-3000ms. (Default: 3000)
  */
  vtkSetMacro(ConnectionSetupDelayMs, int);  
  /*! Set the time required for setting up the connection. */
  vtkGetMacro(ConnectionSetupDelayMs, int);

  /*! 
    Set the SharedMemoryStatus(1) to bypass TCP on local access.
  */
  vtkSetMacro(SharedMemoryStatus, int);  
  /*! Get the SharedMemoryStatus. */
  vtkGetMacro(SharedMemoryStatus, int);

  /*! Get the displayed frame rate. */
  PlusStatus GetDisplayedFrameRate(int &aFrameRate);

  /*! Get the displayed frame size. */
  PlusStatus GetDisplayedFrameSize(int &aFrameWidth, int &aFrameHeight);

  /*! Set RF decimation. This requires ultrerius be connected. */
  PlusStatus SetRFDecimation(int decimation);

  /*! Set speckle reduction filter (filterIndex: 0=off,1,2). This requires ultrerius be connected.  */
  PlusStatus SetPPFilter(int filterIndex);

   /*! Set maximum frame rate limit on exam software (frLimit=403 means 40.3Hz). This requires ultrerius be connected.  */
  PlusStatus SetFrameRateLimit(int frLimit);

  /*! Print the list of supported parameters. For diagnostic purposes only. */
  PlusStatus PrintListOfImagingParameters();

  /*!
  \enum RfAcquisitionModeType
  \brief Defines RF acquisition mode types (0=B only, 1=RF only, 2=B and RF, 3=ChRF, 4=B and ChRF)
  */
  enum RfAcquisitionModeType
  {
    RF_UNKNOWN = -1, 
    RF_ACQ_B_ONLY = 0, 
    RF_ACQ_RF_ONLY = 1, 
    RF_ACQ_B_AND_RF = 2,
    RF_ACQ_CHRF_ONLY = 3, 
    RF_ACQ_B_AND_CHRF = 4 
  }; 
  /*! Set RF acquire mode */
  PlusStatus SetRfAcquisitionMode(RfAcquisitionModeType mode);
  /*! Get current RF acquire mode */
  PlusStatus GetRfAcquisitionMode(RfAcquisitionModeType & mode);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  /*! Constructor */
  vtkSonixVideoSource();
  /*! Destructor */
  virtual ~vtkSonixVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*!
    Record incoming video.  The recording
    continues indefinitely until StopRecording() is called. 
  */
  virtual PlusStatus InternalStartRecording();

  /*! Stop recording or playing */
  virtual PlusStatus InternalStopRecording();

  virtual PlusStatus InternalUpdate();

  /*! Get the last error string returned by Ulterius */
  std::string GetLastUlteriusError();

  virtual PlusStatus Reset();

  ////////////////////////

  /*! For internal use only */
  PlusStatus AddFrameToBuffer(void * data, int type, int sz, bool cine, int frmnum);

  /*! For internal use only */
  PlusStatus SetParamValue(char* paramId, int paramValue, int &validatedParamValue);
  /*! For internal use only */
  PlusStatus GetParamValue(char* paramId, int& paramValue, int &validatedParamValue);

  ulterius Ult;
  uDataDesc DataDescriptor;

  int Frequency;
  int Depth;
  int Sector; 
  int Gain; 
  int DynRange; 
  int Zoom; 
  int AcquisitionDataType;
  int ImagingMode;
  int OutputFormat;
  int CompressionStatus; 
  int Timeout;
  int ConnectionSetupDelayMs;
  int SharedMemoryStatus;
  RfAcquisitionModeType RfAcquisitionMode;
  int SoundVelocity;

  char *SonixIP;

  /*!
    Indicates if connection to the device has been established. It's not the same as the Connected parameter,
    because Connected indicates that the connection is successfully completed; while UlteriusConnected
    indicates that the connection to Ulterius has been established (so that Ulterius calls are allowed),
    but the connection initialization (setup of requested imaging parameters, etc.) may fail.
  */
  bool UlteriusConnected;
    
private:
 
  static vtkSonixVideoSource* Instance;
  static bool vtkSonixVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum);
  vtkSonixVideoSource(const vtkSonixVideoSource&);  // Not implemented.
  void operator=(const vtkSonixVideoSource&);  // Not implemented.
};

#endif
