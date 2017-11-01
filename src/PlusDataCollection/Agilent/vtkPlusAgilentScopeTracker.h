/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAgilentScopeTracker_h
#define __vtkPlusAgilentScopeTracker_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// Agilent type includes
#include <AgMD1FundamentalDataTypes.h>
#include <vpptype.h>

// VTK includes
#include <vtkSmartPointer.h>

class vtkMatrix;

class vtkDoubleArray;
class vtkPlot;
class vtkView;
class vtkChartXY;
class vtkContextView;

/*!
  \class vtkPlusAgilentScopeTracker
  \brief Interface class for Agilent oscilloscope devices


  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusAgilentScopeTracker : public vtkPlusDevice
{
public:
  static vtkPlusAgilentScopeTracker* New();
  vtkTypeMacro(vtkPlusAgilentScopeTracker, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool IsTracker() const { return true; }

  /*!
    Probe to see if the PCI card is present in the system
  */
  PlusStatus Probe();

  /*! Request data from the oscilloscope and push the new transforms to the tools. */
  PlusStatus InternalUpdate();

  /*! Read tracker configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Read tracker configuration from xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Confirm that the device has been properly configured */
  virtual PlusStatus NotifyConfigured();

  vtkSetMacro(SampleFrequencyHz, ViReal64);
  vtkGetMacro(SampleFrequencyHz, ViReal64);

  vtkSetMacro(DelayTimeSec, ViReal64);
  vtkGetMacro(DelayTimeSec, ViReal64);

  vtkSetMacro(SampleCountPerAcquisition, ViInt32);
  vtkGetMacro(SampleCountPerAcquisition, ViInt32);

  vtkSetMacro(FullScale, ViReal64);
  vtkGetMacro(FullScale, ViReal64);

  vtkSetMacro(Offset, ViReal64);
  vtkGetMacro(Offset, ViReal64);

  vtkSetMacro(Bandwidth, ViInt32);
  vtkGetMacro(Bandwidth, ViInt32);

  vtkSetMacro(TrigCoupling, ViInt32);
  vtkGetMacro(TrigCoupling, ViInt32);

  vtkSetMacro(Slope, ViInt32);
  vtkGetMacro(Slope, ViInt32);

  vtkSetMacro(Level, ViReal64);
  vtkGetMacro(Level, ViReal64);

  vtkSetMacro(SpeedOfSound, ViInt32);
  vtkGetMacro(SpeedOfSound, ViInt32);

  vtkSetMacro(KernelSize, ViInt32);
  vtkGetMacro(KernelSize, ViInt32);

  vtkSetMacro(MinPeakDistance, ViInt32);
  vtkGetMacro(MinPeakDistance, ViInt32);

protected:
  vtkPlusAgilentScopeTracker();
  virtual ~vtkPlusAgilentScopeTracker();

  /// Connect to the oscilloscope hardware
  virtual PlusStatus InternalConnect();
  /// Disconnect from the oscilloscope hardware
  virtual PlusStatus InternalDisconnect();

  /// Index of the last frame number
  unsigned long LastFrameNumber;

  // Agilent scope details
  ViInt32 InstrumentCount;
  ViStatus Status;
  ViChar ResourceName[16]; // Resource string
  ViChar OptionString[32]; // No options necessary
  ViSession InstrumentID; // Instrument handle

  // Configuration parameters
  ViInt32 SpeedOfSound; // m/s
  ViReal64 SampleFrequencyHz;
  ViReal64 SampleIntervalSec;
  ViReal64 DelayTimeSec;
  ViInt32 SampleCountPerAcquisition;
  ViInt32 SegmentCountPerAcquisition; // Currently fixed to 1, as we only support single segment acquisition
  ViReal64 FullScale;
  ViReal64 Offset;
  ViInt32 Coupling; // Currently fixed to 3
  ViInt32 Bandwidth;
  ViInt32 TrigCoupling;
  ViInt32 Slope;
  ViReal64 Level; // In % of vertical full scale when using internal trigger
  ViInt32 MinPeakDistance;

  vtkSmartPointer<vtkMatrix4x4> FirstPeakToNeedleTip;
  vtkSmartPointer<vtkMatrix4x4> SecondPeakToNeedleTip;
  vtkSmartPointer<vtkMatrix4x4> ThirdPeakToNeedleTip;

  // 1D signal is packaged into an image with data type int16
  vtkSmartPointer<vtkImageData> SignalImage;

  // Data reading members
  AqReadParameters ReadParameters;
  AqDataDescriptor DataDescription;
  AqSegmentDescriptor SegmentDescription;

  // Data buffer for data that is read
  ViInt16* DataArray;
  ViInt16* EnvelopeArray;

  // Envelope detection via a moving average filter
  ViReal64* HannWindow;
  ViInt32 KernelSize; // an odd integer

  // Peak detection
  ViInt32*                                  PeakIdxArray;
  std::vector<std::pair<ViInt16, ViInt32>>  PeakEntries;

private:
  vtkPlusAgilentScopeTracker(const vtkPlusAgilentScopeTracker&);
  void operator=(const vtkPlusAgilentScopeTracker&);
};

#endif