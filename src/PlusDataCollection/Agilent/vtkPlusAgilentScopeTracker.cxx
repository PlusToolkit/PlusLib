/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
ODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

// Local includes
#include "PlusConfigure.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusAgilentScopeTracker.h"

// Agilent includes
#include "AgMD1Fundamental.h"

// VTK includes
#include <vtkCharArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkSocketCommunicator.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>

// STL includes
#include <fstream>
#include <iostream>

// System includes
#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <queue>
#include <vector>

using namespace std;

namespace
{
  const int VIRTUAL_SROM_SIZE = 1024;

  bool AgilentError(const std::string& functionName, ViStatus status)
  {
    char ErrMsg[256];
    if (status)
    {
      Acqrs_errorMessage(VI_NULL, status, ErrMsg, 256);
      cout << functionName << ": " << ErrMsg << endl;
      return true;
    }

    return false;
  }

}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusAgilentScopeTracker);

//----------------------------------------------------------------------------
vtkPlusAgilentScopeTracker::vtkPlusAgilentScopeTracker()
  : InstrumentCount(0)
  , Status(0)
  , InstrumentID(0)
  , SpeedOfSound(1480)
  , SampleFrequencyHz(420000000.0)
  , DelayTimeSec(0.000002)
  , SampleCountPerAcquisition(11350)
  , SegmentCountPerAcquisition(1) // Currently fixed to 1, as we only support single segment acquisition
  , FullScale(1.0)
  , Offset(0.0)
  , Coupling(3) // Currently fixed to 3
  , Bandwidth(0)
  , TrigCoupling(0)
  , Slope(0)
  , Level(20.0) // In % of vertical full scale when using internal trigger
  , FirstPeakToNeedleTip(vtkSmartPointer<vtkMatrix4x4>::New())
  , SecondPeakToNeedleTip(vtkSmartPointer<vtkMatrix4x4>::New())
  , ThirdPeakToNeedleTip(vtkSmartPointer<vtkMatrix4x4>::New())
  , SignalImage(vtkSmartPointer<vtkImageData>::New())
  , EnvelopeArray(NULL)
  , DataArray(NULL)
  , HannWindow(NULL)
  , KernelSize(153)
  , PeakIdxArray(NULL)
  , MinPeakDistance(280)
{
  this->FirstPeakToNeedleTip->Identity();
  this->SecondPeakToNeedleTip->Identity();
  this->ThirdPeakToNeedleTip->Identity();

  this->SampleIntervalSec = 1.0 / (this->SampleFrequencyHz);

  // No callback function provided by the scope's SDK, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 50;

  // Zero out data structures
  this->DataDescription.returnedSamplesPerSeg = -1;
  this->DataDescription.indexFirstPoint = -1;
  this->DataDescription.sampTime = -1.0;
  this->DataDescription.vGain = -1.0;
  this->DataDescription.vOffset = -1.0;
  this->DataDescription.returnedSegments = -1;
  this->DataDescription.nbrAvgWforms = -1;
  this->DataDescription.actualTriggersInAcqLo = 0;
  this->DataDescription.actualTriggersInAcqHi = 0;
  this->DataDescription.actualDataSize = 0;
  this->DataDescription.reserved2 = -1;
  this->DataDescription.reserved3 = -1.0;
  this->DataDescription.actualDataSize = 0;

  this->ReadParameters.dataType = -1;
  this->ReadParameters.readMode = -1;
  this->ReadParameters.firstSegment = -1;
  this->ReadParameters.nbrSegments = -1;
  this->ReadParameters.firstSampleInSeg = -1;
  this->ReadParameters.nbrSamplesInSeg = -1;
  this->ReadParameters.segmentOffset = -1;
  this->ReadParameters.dataArraySize = -1;
  this->ReadParameters.segDescArraySize = -1;
  this->ReadParameters.flags = -1;
  this->ReadParameters.reserved = -1;
  this->ReadParameters.reserved2 = -1.0;
  this->ReadParameters.reserved3 = -1.0;

  this->SegmentDescription.horPos = -1.0;
  this->SegmentDescription.timeStampLo = 0;
  this->SegmentDescription.timeStampHi = 0;
}

//----------------------------------------------------------------------------
vtkPlusAgilentScopeTracker::~vtkPlusAgilentScopeTracker()
{
  if (this->DataArray != NULL)
  {
    delete this->DataArray;
  }
  if (this->EnvelopeArray != NULL)
  {
    delete this->EnvelopeArray;
  }
  if (this->HannWindow != NULL)
  {
    delete this->HannWindow;
  }
  if (this->PeakIdxArray != NULL)
  {
    delete this->PeakIdxArray;
  }
}

//----------------------------------------------------------------------------
void vtkPlusAgilentScopeTracker::PrintSelf(std::ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << indent << "LastFrameNumber: " << this->LastFrameNumber << std::endl;
  os << indent << "InstrumentCount: " << this->InstrumentCount << std::endl;
  os << indent << "Status: " << this->Status << std::endl;
  os << indent << "ResourceName: " << this->ResourceName << std::endl;
  os << indent << "OptionsString: " << this->OptionString << std::endl;
  os << indent << "InstrumentID: " << this->InstrumentID << std::endl;
  os << indent << "SpeedOfSound: " << this->SpeedOfSound << std::endl;
  os << indent << "SampleFrequencyHz: " << this->SampleFrequencyHz << std::endl;
  os << indent << "SampleIntervalSec: " << this->SampleIntervalSec << std::endl;
  os << indent << "DelayTimeSec: " << this->DelayTimeSec << std::endl;
  os << indent << "SampleCountPerAcquisition: " << this->SampleCountPerAcquisition << std::endl;
  os << indent << "SegmentCountPerAcquisition: " << this->SegmentCountPerAcquisition << std::endl;
  os << indent << "FullScale: " << this->FullScale << std::endl;
  os << indent << "Offset: " << this->Offset << std::endl;
  os << indent << "Coupling: " << this->Coupling << std::endl;
  os << indent << "Bandwidth: " << this->Bandwidth << std::endl;
  os << indent << "TrigCoupling: " << this->TrigCoupling << std::endl;
  os << indent << "Slope: " << this->Slope << std::endl;
  os << indent << "Level: " << this->Level << std::endl;
  os << indent << "MinPeakDistance: " << this->MinPeakDistance << std::endl;
  this->FirstPeakToNeedleTip->PrintSelf(os, indent);
  this->SecondPeakToNeedleTip->PrintSelf(os, indent);
  this->ThirdPeakToNeedleTip->PrintSelf(os, indent);

  // 1D signal is packaged into an image with data type int1
  this->SignalImage->PrintSelf(os, indent);

  // Data reading members
  os << indent << "ReadParameters: " << std::endl;
  os << indent << "  dataType: " << this->ReadParameters.dataType << std::endl;
  os << indent << "  readMode: " << this->ReadParameters.readMode << std::endl;
  os << indent << "  firstSegment: " << this->ReadParameters.firstSegment << std::endl;
  os << indent << "  nbrSegments: " << this->ReadParameters.nbrSegments << std::endl;
  os << indent << "  firstSampleInSeg: " << this->ReadParameters.firstSampleInSeg << std::endl;
  os << indent << "  nbrSamplesInSeg: " << this->ReadParameters.nbrSamplesInSeg << std::endl;
  os << indent << "  segmentOffset: " << this->ReadParameters.segmentOffset << std::endl;
  os << indent << "  dataArraySize: " << this->ReadParameters.dataArraySize << std::endl;
  os << indent << "  segDescArraySize: " << this->ReadParameters.segDescArraySize << std::endl;
  os << indent << "  flags: " << this->ReadParameters.flags << std::endl;
  os << indent << "  reserved: " << this->ReadParameters.reserved << std::endl;
  os << indent << "  reserved2: " << this->ReadParameters.reserved2 << std::endl;
  os << indent << "  reserved3: " << this->ReadParameters.reserved3 << std::endl;

  os << indent << "DataDescription: " << std::endl;
  os << indent << "  returnedSamplesPerSeg: " <<  this->DataDescription.returnedSamplesPerSeg << std::endl;
  os << indent << "  indexFirstPoint: " << this->DataDescription.indexFirstPoint << std::endl;
  os << indent << "  sampTime: " << this->DataDescription.sampTime << std::endl;
  os << indent << "  vGain: " << this->DataDescription.vGain << std::endl;
  os << indent << "  vOffset: " << this->DataDescription.vOffset << std::endl;
  os << indent << "  returnedSegments: " << this->DataDescription.returnedSegments << std::endl;
  os << indent << "  nbrAvgWforms: " << this->DataDescription.nbrAvgWforms << std::endl;
  os << indent << "  actualTriggersInAcqLo: " << this->DataDescription.actualTriggersInAcqLo << std::endl;
  os << indent << "  actualTriggersInAcqHi: " << this->DataDescription.actualTriggersInAcqHi << std::endl;
  os << indent << "  actualDataSize: " << this->DataDescription.actualDataSize << std::endl;
  os << indent << "  reserved2: " << this->DataDescription.reserved2 << std::endl;
  os << indent << "  reserved3: " << this->DataDescription.reserved3 << std::endl;
  os << indent << "  actualDataSize: " << this->DataDescription.actualDataSize << std::endl;

  os << indent << "SegmentDescription: " << std::endl;
  os << indent << "  horPos: " << this->SegmentDescription.horPos << std::endl;
  os << indent << "  timeStampLo: " << this->SegmentDescription.timeStampLo << std::endl;
  os << indent << "  timeStampHi: " << this->SegmentDescription.timeStampHi << std::endl;

  // Data buffer for data that is read
  if (this->ReadParameters.dataArraySize != -1)
  {
    os << indent << "DataArray: " << std::endl;
    os << indent << "  ";
    for (int i = 0; i < this->ReadParameters.dataArraySize; ++i)
    {
      os << indent << this->DataArray[i];
    }
    os << indent << std::endl;

    os << indent << "EnvelopeArray: " << std::endl;
    os << indent << "  ";
    for (int i = 0; i < this->ReadParameters.dataArraySize; ++i)
    {
      os << indent << this->EnvelopeArray[i];
    }
    os << indent << std::endl;

    os << indent << "PeakIdxArray: " << std::endl;
    os << indent << "  ";
    for (int i = 0; i < this->ReadParameters.dataArraySize; ++i)
    {
      os << indent << this->PeakIdxArray[i];
    }
    os << indent << std::endl;
  }

  // Envelope detection via a moving average filter
  os << indent << "HannWindow: " << std::endl;
  for (int i = 0; i < this->KernelSize; ++i)
  {
    os << indent << this->HannWindow[i];
  }
  os << indent << std::endl;

  // Peak detection
  os << indent << "PeakEntries: " << std::endl;
  for (auto it = this->PeakEntries.begin(); it != this->PeakEntries.end(); ++it)
  {
    os << indent << "  X: " << it->first << ", " << "Y: " << it->second << std::endl;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::Probe()
{
  // The following call will automatically detect ASBus connections between digitizers
  // and combine connected digitizers (of identical model!) into multi-instruments.
  // The call returns the number of multi-instruments and/or single instruments.
  this->Status = AcqrsD1_multiInstrAutoDefine("", &(this->InstrumentCount));
  if (AgilentError("AcqrsD1_multiInstrAutoDefine", this->Status))
  {
    return PLUS_FAIL;
  }

  if (this->InstrumentCount < 1)
  {
    LOG_ERROR("No instrument found!");
    return PLUS_FAIL; // No instrument found
  }

  LOG_INFO(this->InstrumentCount << " Agilent Acqiris Digitizer(s) found on your PC");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::InternalUpdate()
{
  vtkPlusDataSource* firstPeakToolSource(nullptr);
  vtkPlusDataSource* secondPeakToolSource(nullptr);
  vtkPlusDataSource* thirdPeakToolSource(nullptr);

  std::string fullToolName = std::string("FirstPeakTo") + this->GetToolReferenceFrameName();
  this->GetDataSource(fullToolName, firstPeakToolSource);
  fullToolName = std::string("SecondPeakTo") + this->GetToolReferenceFrameName();
  this->GetDataSource(fullToolName, secondPeakToolSource);
  fullToolName = std::string("ThirdPeakTo") + this->GetToolReferenceFrameName();
  this->GetDataSource(fullToolName, thirdPeakToolSource);

  vtkPlusDataSource* videoSource(nullptr);
  this->GetFirstVideoSource(videoSource);

  // Start the acquisition
  this->Status = AcqrsD1_acquire(this->InstrumentID);
  if (AgilentError("AcqrsD1_acquire", this->Status))
  {
    return PLUS_FAIL;
  }

  // Wait for interrupt to signal the end of acquisition with a timeout of 2 seconds
  // Note: The maximum value is 10 seconds. See 'Reference Manual' for more details.
  this->Status = AcqrsD1_waitForEndOfAcquisition(this->InstrumentID, 2000);
  if (AgilentError("AcqrsD1_waitForEndOfAcquisition", this->Status))
  {
    // Clean up acquisition so we can try again
    this->Status = AcqrsD1_stopAcquisition(this->InstrumentID);
    AgilentError("AcqrsD1_stopAcquisition", this->Status);
    return PLUS_FAIL;
  }

  if (this->Status != VI_SUCCESS)
  {
    // Acquisition did not complete successfully
    // Note: In case of a timeout, 'AcqrsD1_forceTrig' (software trigger) may be used.
    // See 'Reference Manual' for more details.
    this->StopRecording();
    LOG_ERROR("The acquisition has been stopped - data invalid!");
    return PLUS_FAIL;
  }

  // Readout of the waveform
  this->Status = AcqrsD1_readData(this->InstrumentID, 1, &(this->ReadParameters), this->DataArray, &(this->DataDescription), &(this->SegmentDescription));
  if (AgilentError("AcqrsD1_readData", this->Status))
  {
    return PLUS_FAIL;
  }

  //---- Envelope Detection via a moving average filter with a Hanning Kernel
  ViInt32 halfWindowSize = this->KernelSize / 2;
  for (ViInt32 s = this->DataDescription.indexFirstPoint; s < this->DataDescription.indexFirstPoint + this->DataDescription.returnedSamplesPerSeg; s++)
  {
    ViReal64 sum(0.0);
    ViReal64 count = 0;
    for (int i = -halfWindowSize; i < halfWindowSize + 1; ++i)
    {
      if (s + i < this->DataDescription.indexFirstPoint || s + i > this->DataDescription.indexFirstPoint + this->DataDescription.returnedSamplesPerSeg)
      {
        continue;
      }
      sum += abs(DataArray[s + i]) * this->HannWindow[i + halfWindowSize];
      count += this->HannWindow[i + halfWindowSize]; // should be 1 if not using Hanning window
    }
    this->EnvelopeArray[s - this->DataDescription.indexFirstPoint] = sum / count;
  }

  memcpy(this->SignalImage->GetScalarPointer(), this->EnvelopeArray, sizeof(short)*this->SampleCountPerAcquisition);

  // Find the absolute max (use more advanced algorithm for more robust tissue barrier detection)
  ViReal64 absMaxValue(-1.0);
  ViReal64 absMaxValueIndex = 0;

  for (ViInt32 i = this->DataDescription.indexFirstPoint; i < this->DataDescription.indexFirstPoint + this->DataDescription.returnedSamplesPerSeg; i++)
  {
    ViReal64 value = this->EnvelopeArray[i]; // *this->DataDescription.vGain - this->DataDescription.vOffset; // Volts
    if (abs(value) > absMaxValue)
    {
      absMaxValue = abs(value);
      absMaxValueIndex = i;
    }
  }

  // Find all peak indices
  ViReal64 noiseLevel = 0.1 * absMaxValue;
  ViInt32 peakIdxCount(0);
  for (ViInt32 i = 1; i < this->DataDescription.returnedSamplesPerSeg - 1; i++)
  {
    if (this->EnvelopeArray[i] < noiseLevel)
    {
      continue;
    }

    ViReal64 diffPrev = this->EnvelopeArray[i] - this->EnvelopeArray[i - 1];
    ViReal64 diffNext = this->EnvelopeArray[i + 1] - this->EnvelopeArray[i];
    if ((diffPrev * diffNext) <= 0 && diffPrev > 0)
    {
      this->PeakIdxArray[peakIdxCount] = i;
      peakIdxCount++;
    }
  }

  if (peakIdxCount == 0)
  {
    LOG_INFO("No peaks found. Skipping frame.");
    return PLUS_SUCCESS;
  }

  // check for distances between peaks
  ViInt32 peakIndex(0);
  ViInt32 nextIndex(0);

  for (ViInt32 i = 0; i < peakIdxCount; i++)
  {
    peakIndex = this->PeakIdxArray[i];

    if (i + 1 < peakIdxCount)
    {
      nextIndex = this->PeakIdxArray[i + 1];
      if ((nextIndex - peakIndex) > this->MinPeakDistance)  //peaks are far away
      {
        PeakEntries.push_back(std::pair<ViInt16, ViInt32>(this->EnvelopeArray[peakIndex], peakIndex));
      }
      else if (this->EnvelopeArray[peakIndex] > this->EnvelopeArray[nextIndex]) //peaks are closer than required min distance
      {
        if (this->EnvelopeArray[nextIndex] > 0.5 * this->EnvelopeArray[peakIndex])
        {
          i++;
        }
        PeakEntries.push_back(std::pair<ViInt16, ViInt32>(this->EnvelopeArray[peakIndex], peakIndex));
      }
    }
    else
    {
      PeakEntries.push_back(std::pair<ViInt16, ViInt32>(this->EnvelopeArray[peakIndex], peakIndex));
    }
  }

  std::sort(begin(PeakEntries), end(PeakEntries));
  std::reverse(begin(PeakEntries), end(PeakEntries));

  auto largest = PeakEntries[0].first;
  double largestIdx = PeakEntries[0].second;
  // Calculate the mm offset of the abs max value
  double mmOffset1 = (this->DelayTimeSec * this->SpeedOfSound * 1000 / 2.0) + (absMaxValueIndex * this->SpeedOfSound * 1000 / (2.0 * this->SampleFrequencyHz));
  // Transformation based on the mm offset of the abs max values
  this->FirstPeakToNeedleTip->SetElement(2, 3, mmOffset1);
  this->ToolTimeStampedUpdate(firstPeakToolSource->GetId(), this->FirstPeakToNeedleTip.Get(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP);

  if (this->PeakEntries.size() > 1)
  {
    auto second_largest = PeakEntries[1].first;
    double second_largestIdx = PeakEntries[1].second;
    double mmOffset2 = (this->DelayTimeSec * this->SpeedOfSound * 1000 / 2.0) + (second_largestIdx * this->SpeedOfSound * 1000 / (2.0 * this->SampleFrequencyHz));
    this->SecondPeakToNeedleTip->SetElement(2, 3, mmOffset2);
    this->ToolTimeStampedUpdate(secondPeakToolSource->GetId(), this->SecondPeakToNeedleTip.Get(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP);
  }
  else
  {
    this->SecondPeakToNeedleTip->SetElement(2, 3, 0.0);
    this->ToolTimeStampedUpdate(secondPeakToolSource->GetId(), this->SecondPeakToNeedleTip.Get(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP);
  }

  if (this->PeakEntries.size() > 2)
  {
    auto third_largest = PeakEntries[2].first;
    double third_largestIdx = PeakEntries[2].second;
    double mmOffset3 = (this->DelayTimeSec * this->SpeedOfSound * 1000 / 2.0) + (third_largestIdx * this->SpeedOfSound * 1000 / (2.0 * this->SampleFrequencyHz));
    this->ThirdPeakToNeedleTip->SetElement(2, 3, mmOffset3);
    this->ToolTimeStampedUpdate(thirdPeakToolSource->GetId(), this->ThirdPeakToNeedleTip.Get(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP);
  }
  else
  {
    this->ThirdPeakToNeedleTip->SetElement(2, 3, 0.0);
    this->ToolTimeStampedUpdate(thirdPeakToolSource->GetId(), this->ThirdPeakToNeedleTip.Get(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP);
  }

  videoSource->AddItem(this->SignalImage, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS, this->FrameNumber);
  this->FrameNumber++;

  this->PeakEntries.clear();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  // This function will read the data from the rootConfigElement, and populate your member variables
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, SampleFrequencyHz, deviceConfig);
  this->SampleIntervalSec = 1.0 / (this->SampleFrequencyHz);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, DelayTimeSec, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, SampleCountPerAcquisition, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, FullScale, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, Offset, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, Bandwidth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, TrigCoupling, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, Slope, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, Level, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, SpeedOfSound, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, KernelSize, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, MinPeakDistance, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfig);

  deviceConfig->SetDoubleAttribute("SampleFrequencyHz", this->SampleFrequencyHz);
  deviceConfig->SetDoubleAttribute("DelayTimeSec", this->DelayTimeSec);
  deviceConfig->SetIntAttribute("SampleCountPerAcquisition", this->SampleCountPerAcquisition);
  deviceConfig->SetDoubleAttribute("FullScale", this->FullScale);
  deviceConfig->SetDoubleAttribute("Offset", this->Offset);
  deviceConfig->SetIntAttribute("Bandwidth", this->Bandwidth);
  deviceConfig->SetIntAttribute("TrigCoupling", this->TrigCoupling);
  deviceConfig->SetIntAttribute("Slope", this->Slope);
  deviceConfig->SetDoubleAttribute("Level", this->Level);
  deviceConfig->SetIntAttribute("SpeedOfSound", this->SpeedOfSound);
  deviceConfig->SetIntAttribute("KernelSize", this->KernelSize);
  deviceConfig->SetIntAttribute("MinPeakDistance", this->MinPeakDistance);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::NotifyConfigured()
{
  // This function makes sure the user has configured the device in the config file according to the scope's needs
  vtkPlusDataSource* source(nullptr);
  if (this->GetFirstVideoSource(source) != PLUS_SUCCESS)
  {
    LOG_ERROR("Agilent scope device requires a video data source to write 1D signal to.");
    return PLUS_FAIL;
  }

  std::string fullToolName = std::string("FirstPeakTo") + this->GetToolReferenceFrameName();
  if (this->GetDataSource(fullToolName, source) != PLUS_SUCCESS)
  {
    LOG_ERROR("Agilent scope device requires a tool data source with ID \"FirstPeak\" to envelope detected first peak to.");
    return PLUS_FAIL;
  }

  fullToolName = std::string("SecondPeakTo") + this->GetToolReferenceFrameName();
  if (this->GetDataSource(fullToolName, source) != PLUS_SUCCESS)
  {
    LOG_ERROR("Agilent scope device requires a tool data source with ID \"SecondPeak\" to envelope detected second peak to.");
    return PLUS_FAIL;
  }

  fullToolName = std::string("ThirdPeakTo") + this->GetToolReferenceFrameName();
  if (this->GetDataSource(fullToolName, source) != PLUS_SUCCESS)
  {
    LOG_ERROR("Agilent scope device requires a tool data source with ID \"ThirdPeak\" to envelope detected second peak to.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::InternalConnect()
{
  // Initialization of the instrument
  strncpy(this->ResourceName, "PCI::INSTR0", 16);
  strncpy(this->OptionString, "", 32);
  this->Status = Acqrs_InitWithOptions(this->ResourceName, VI_FALSE, VI_TRUE, this->OptionString, &this->InstrumentID);
  if (AgilentError("Acqrs_InitWithOptions", this->Status))
  {
    return PLUS_FAIL;
  }

  // Configuration of the digitizer
  // Configure timebase
  this->Status = AcqrsD1_configHorizontal(this->InstrumentID, this->SampleIntervalSec, this->DelayTimeSec);
  if (AgilentError("AcqrsD1_configHorizontal", this->Status))
  {
    return PLUS_FAIL;
  }

  this->Status = AcqrsD1_configMemory(this->InstrumentID, this->SampleCountPerAcquisition, this->SegmentCountPerAcquisition);
  if (AgilentError("AcqrsD1_configMemory", this->Status))
  {
    return PLUS_FAIL;
  }

  // Confirm valid sample count and segment count
  ViInt32 sampleCountPerAcq;
  ViInt32 segmentCountPerAcq;
  this->Status = AcqrsD1_getMemory(this->InstrumentID, &sampleCountPerAcq, &segmentCountPerAcq);
  if (AgilentError("AcqrsD1_getMemory", this->Status))
  {
    return PLUS_FAIL;
  }
  if (sampleCountPerAcq != this->SampleCountPerAcquisition || segmentCountPerAcq != this->SegmentCountPerAcquisition)
  {
    LOG_WARNING("Sample count or segment count per acquisition does not match requested values. Replacing with actual values.");
    this->SampleCountPerAcquisition = sampleCountPerAcq;
    this->SegmentCountPerAcquisition = segmentCountPerAcq;
  }

  this->SignalImage->SetDimensions(this->SampleCountPerAcquisition, 1, 1);
  this->SignalImage->AllocateScalars(VTK_SHORT, 1);

  vtkPlusDataSource* source(nullptr);
  this->GetFirstVideoSource(source);

  source->SetInputFrameSize(this->SampleCountPerAcquisition, 1, 1);
  source->SetNumberOfScalarComponents(1);
  source->SetPixelType(VTK_SHORT);

  // Configure vertical settings of channel 1
  this->Status = AcqrsD1_configVertical(this->InstrumentID, 1, this->FullScale, this->Offset, this->Coupling, this->Bandwidth);
  if (AgilentError("AcqrsD1_configVertical", this->Status))
  {
    return PLUS_FAIL;
  }

  // Configure edge trigger on channel 1
  this->Status = AcqrsD1_configTrigClass(this->InstrumentID, 0, 0x00000001, 0, 0, 0.0, 0.0);
  if (AgilentError("AcqrsD1_configTrigClass", this->Status))
  {
    return PLUS_FAIL;
  }

  // Configure the trigger conditions of channel 1 (internal trigger)
  this->Status = AcqrsD1_configTrigSource(this->InstrumentID, 1, this->TrigCoupling, this->Slope, this->Level, 0.0);
  if (AgilentError("AcqrsD1_configTrigSource", this->Status))
  {
    return PLUS_FAIL;
  }

  // Definition of the read parameters for raw ADC readout
  this->ReadParameters.dataType = ReadInt16; // 16bit, raw ADC values data type
  this->ReadParameters.readMode = ReadModeStdW; // Single-segment read mode
  this->ReadParameters.firstSegment = 0;
  this->ReadParameters.nbrSegments = 1;
  this->ReadParameters.firstSampleInSeg = 0;
  this->ReadParameters.nbrSamplesInSeg = this->SampleCountPerAcquisition;
  this->ReadParameters.segmentOffset = 0;
  this->ReadParameters.dataArraySize = (this->SampleCountPerAcquisition + 32) * sizeof(ViInt16); // Array size in bytes
  this->ReadParameters.segDescArraySize = sizeof(AqSegmentDescriptor);

  this->ReadParameters.flags = 0;
  this->ReadParameters.reserved = 0;
  this->ReadParameters.reserved2 = 0;
  this->ReadParameters.reserved3 = 0;

  // Read the channel 1 waveform as raw ADC values
  this->DataArray = new ViInt16[this->ReadParameters.dataArraySize / sizeof(ViInt16)];
  this->EnvelopeArray = new ViInt16[this->ReadParameters.dataArraySize / sizeof(ViInt16)];
  this->PeakIdxArray = new ViInt32[this->ReadParameters.dataArraySize / sizeof(ViInt32)];

  //---- Hanning Window
  this->HannWindow = new ViReal64[this->KernelSize];

  for (int k = 0; k < this->KernelSize; k++)
  {
    this->HannWindow[k] = 0.5 * (1 - cos((2 * vtkMath::Pi() * k) / (this->KernelSize - 1)));
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::InternalDisconnect()
{
  // Close the instrument
  this->Status = Acqrs_close(this->InstrumentID);
  if (AgilentError("Acqrs_close", this->Status))
  {
    return PLUS_FAIL;
  }

  // Free remaining resources
  this->Status = Acqrs_closeAll();
  if (AgilentError("Acqrs_closeAll", this->Status))
  {
    return PLUS_FAIL;
  }

  delete [] this->DataArray;
  delete [] this->EnvelopeArray;
  delete [] this->PeakIdxArray;

  return PLUS_SUCCESS;
}