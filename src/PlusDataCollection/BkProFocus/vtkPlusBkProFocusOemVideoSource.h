/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// GRAB_FRAME API was partly contributed by by Xin Kang at SZI, Children's National Medical Center

#ifndef __vtkPlusBkProFocusOemVideoSource_h
#define __vtkPlusBkProFocusOemVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "PlusTrackedFrame.h"
#include "vtkPlusUsDevice.h"

/*!
  \class vtkPlusBkProFocusOemVideoSource
  \brief Class for acquiring ultrasound images from BK ultrasound systems through the OEM interface

  Requires the PLUS_USE_BKPROFOCUS_VIDEO option in CMake.

  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusBkProFocusOemVideoSource : public vtkPlusUsDevice
{
public:

  static vtkPlusBkProFocusOemVideoSource* New();
  vtkTypeMacro(vtkPlusBkProFocusOemVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! BK scanner address */
  vtkSetMacro(ScannerAddress, std::string);

  /*! BK scanner address */
  vtkGetMacro(ScannerAddress, std::string);

  /*! BK scanner OEM port */
  vtkSetMacro(OemPort, unsigned short);

  /*!
    Enable/disable continuous streaming. Continuous streaming (GRAB_FRAME command) requires extra license
    from BK but it allows faster image acquisition.
  */
  vtkSetMacro(ContinuousStreamingEnabled, bool);

  /*!
  Enable/disable continuous streaming. Continuous streaming (GRAB_FRAME command) requires extra license
  from BK but it allows faster image acquisition.
  */
  vtkGetMacro(ContinuousStreamingEnabled, bool);

  /*!
  Enable/disable continuous streaming. Continuous streaming (GRAB_FRAME command) requires extra license
  from BK but it allows faster image acquisition.
  */
  vtkBooleanMacro(ContinuousStreamingEnabled, bool);

  /*! Enable/disable color in the streamed video */
  vtkSetMacro(ColorEnabled, bool);

  /*! Enable/disable color in the streamed video */
  vtkGetMacro(ColorEnabled, bool);

  /*! Enable/disable color in the streamed video */
  vtkBooleanMacro(ColorEnabled, bool);

  /*! Enable/disable offline testing */
  vtkSetMacro(OfflineTesting, bool);

  /*! Enable/disable offline testing */
  vtkGetMacro(OfflineTesting, bool);

  /*! Enable/disable offline testing */
  vtkBooleanMacro(OfflineTesting, bool);

  /*! Path to image file used for offline testing */
  vtkSetStringMacro(OfflineTestingFilePath);

  /*! Path to image file used for offline testing */
  vtkGetStringMacro(OfflineTestingFilePath);

protected:

  static const char* KEY_DEPTH;
  static const char* KEY_GAIN;

  // Size of the ultrasound image. Only used if ContinuousStreamingEnabled is true.
  std::array<unsigned int, 2> UltrasoundWindowSize;

  //Parameter values recaived from the BK scanner.
  double StartLineX_m, StartLineY_m, StartLineAngle_rad, StartDepth_m, StopLineX_m, StopLineY_m, StopLineAngle_rad, StopDepth_m;
  int pixelLeft_pix, pixelTop_pix, pixelRight_pix, pixelBottom_pix;
  int grabFramePixelLeft_pix, grabFramePixelTop_pix, grabFramePixelRight_pix, grabFramePixelBottom_pix;
  double tissueLeft_m, tissueTop_m, tissueRight_m, tissueBottom_m;
  int gain_percent;

  //Probe type of the connected probes
  IGTLIO_PROBE_TYPE probeTypePortA, probeTypePortB, probeTypePortC, probeTypePortM;

  //The current probe port
  std::string probePort;

  /*! Constructor */
  vtkPlusBkProFocusOemVideoSource();
  /*! Destructor */
  ~vtkPlusBkProFocusOemVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Start continuous data streaming from BK. Requires additional license from BK. */
  PlusStatus StartContinuousDataStreaming();

  /*! Stop continuous data streaming from BK. */
  PlusStatus StopContinuousDataStreaming();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*! The internal function which actually does the grab.  */
  PlusStatus InternalUpdate();

  /*! Read and process all received messages until an image message is read. */
  PlusStatus ProcessMessagesAndReadNextImage();

  /*! Read Next received message from BK. */
  PlusStatus ReadNextMessage();

  /*! Remove special characters from the BK message (SOH, EOT and ESC). Also restore inverted characters.*/
  std::vector<char> removeSpecialCharacters(std::vector<char> inMessage);

  /*! Add additional binary data to the image.
   * Tested on a BK5000 scanner:
   * The specified size of the binary data block don't match the actual size (sizes wary a lot).
   * The specified size are sometimes smaller than the actual image,
   * and the data block are sometimes (much) larger than the actual image.
   * Currently all the binary data are added to the image.
   */
  int addAdditionalBinaryDataToImageUntilEOTReached(char& character, std::vector<char>& rawMessage);

  /*! Send a query over socket communication. */
  PlusStatus SendQuery(std::string query);

  /*! Add special characters required by the BK protocol. */
  std::string AddSpecialCharacters(std::string query);

  /*! Request all parameters from BK scanner. */
  PlusStatus RequestParametersFromScanner();

  /*! Various BK queries. */
  PlusStatus QueryImageSize();
  PlusStatus QueryGeometryScanarea();
  PlusStatus QueryGeometryPixel();
  PlusStatus QueryGeometryUsGrabFrame();
  PlusStatus QueryGeometryTissue();
  PlusStatus QueryGain();
  PlusStatus QueryTransducerList();
  PlusStatus QueryTransducer();

  /*! BK commands. */
  PlusStatus SubscribeToParameterChanges();
  PlusStatus ConfigEventsOn();
  //  PlusStatus CommandPowerDopplerOn();

  /*! Functions that parse received BK messages, and stores the values in private variables. */
  void ParseImageSize(std::istringstream& replyStream);
  void ParseGeometryScanarea(std::istringstream& replyStream);
  void ParseGeometryPixel(std::istringstream& replyStream);
  void ParseGeometryUsGrabFrame(std::istringstream& replyStream);
  void ParseGeometryTissue(std::istringstream& replyStream);
  void ParseGain(std::istringstream& replyStream);
  void ParseTransducerList(std::istringstream& replyStream);
  void ParseTransducerData(std::istringstream& replyStream);

  PlusStatus DecodePngImage(unsigned char* pngBuffer, unsigned int pngBufferSize, vtkImageData* decodedImage);

  // Calculate values for the OpenIGTLinkIO standard.

  /*! Sector origin relative to upper left corner of image in pixels */
  virtual std::vector<double> CalculateOrigin() override;

  /*! Probe sector angles relative to down, in radians.
   *  2 angles for 2D, and 4 for 3D probes.
   * For regular imaging with linear probes these will be 0 */
  virtual std::vector<double> CalculateAngles() override;

  /*! Boundaries to cut away areas outside the US sector, in pixels.
   * 4 for 2D, and 6 for 3D. */
  virtual std::vector<double> CalculateBoundingBox() override;

  /*! Start, stop depth for the imaging, in mm. */
  virtual std::vector<double> CalculateDepths() override;

  /*! Width of linear probe. */
  virtual double CalculateLinearWidth() override;

  //Utility functions

  /*! Is the probe used to create the image a sector probe? (Not a linear probe.) */
  bool IsSectorProbe();

  /*! Find sector width in radians, for sector probes. */
  double CalculateWidthInRadians();

  /*! Calculate imaging depth in mm. */
  double CalculateDepthMm();

  /*! Calculate the gain used in the image. */
  int CalculateGain();

  /*! Calculate image start depth in mm. */
  double GetStartDepth();

  /*! Calculate image stop depth in mm. */
  double GetStopDepth();

  /*! Calculate x value of first line in sector, in mm. */
  double GetStartLineX();

  /*! Calculate y value of first line in sector, in mm. */
  double GetStartLineY();

  /*! Calculate x value of last line in sector, in mm. */
  double GetStopLineX();

  /*! Calculate y value of last line in sector, in mm. */
  double GetStopLineY();

  /*! Angle between top of ultrasound sector and first line in sector, in radians. */
  double GetStartLineAngle();

  /*! Angle between top of ultrasound sector and last line in sector, in radians. */
  double GetStopLineAngle();

  /*! Calculate spacing in x direction, in mm. */
  double GetSpacingX();

  /*! Calculate spacing in y direction, in mm. */
  double GetSpacingY();

  /*! Get probe type. */
  virtual IGTLIO_PROBE_TYPE GetProbeType() override;

  /*! Add OpenIGTLinkIO parameters to FrameFields. */
  PlusStatus AddParametersToFrameFields();

  /*! Read theOemClientReadBuffer into a string. Discards the ; at the end of the string */
  std::string ReadBufferIntoString();

  /*! Remove double quotes from a string. E.g. "testString" -> testString */
  std::string RemoveQuotationMarks(std::string inString);

  void SetProbeTypeForPort(std::string port, std::string probeTypeString);

  //Values read from the xml config file
  /*! BK scanner address */
  std::string ScannerAddress;

  /*! BK OEM port */
  unsigned short OemPort;

  /*! Enable continuous streaming. Requires a separate license from BK. */
  bool ContinuousStreamingEnabled;

  /*! Enable streaming of color images. */
  bool ColorEnabled;

  /*! Turn on offline testing. Useful for simple testing without a BK scanner. */
  bool OfflineTesting;

  /*! Path to test image sent when OfflineTesting is turned on. */
  char* OfflineTestingFilePath;

  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;

  /// Container to hold calculated field values
  PlusTrackedFrame::FieldMapType FrameFields;

private:
  vtkPlusBkProFocusOemVideoSource(const vtkPlusBkProFocusOemVideoSource&);  // Not implemented.
  void operator=(const vtkPlusBkProFocusOemVideoSource&);  // Not implemented.

  //Special characters used in BK communication protocol
  enum
  {
    SOH = 1,  ///> Start of header.
    EOT = 4,  ///> End of header.
    ESC = 27, ///> Escape, used for escaping the other special characters. The character itself is inverted.
  };
};

#endif
