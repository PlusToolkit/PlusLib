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
#include "vtkPlusRecursiveCriticalSection.h"
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

namespace
{
  const int VIRTUAL_SROM_SIZE = 1024;

  bool AgilentError(const std::string& functionName, ViStatus status)
  {
    char ErrMsg[256];
    if (status)
    {
      Acqrs_errorMessage(VI_NULL, status, ErrMsg, 256); cout << functionName << ": " << ErrMsg << endl;
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
  , DelayTimeSec(0.0)
  , SampleCountPerAcquisition(1000)
  , SegmentCountPerAcquisition(1) // Currently fixed to 1, as we only support single segment acquisition
  , FullScale(1.0)
  , Offset(0.0)
  , Coupling(3) // Currently fixed to 3
  , Bandwidth(0)
  , TrigCoupling(0)
  , Slope(0)
  , Level(20.0) // In % of vertical full scale when using internal trigger
  , ZOffsetToTracker(vtkSmartPointer<vtkMatrix4x4>::New())
  , SignalImage(vtkSmartPointer<vtkImageData>::New())
{
  ZOffsetToTracker->Identity();

  this->SampleIntervalSec = 1.0 / (this->SampleFrequencyHz);

  // No callback function provided by the scope's SDK, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true; // TO EXPLAIN
  this->AcquisitionRate = 50;
}

//---------------------------------------------------------------------------- Goli :)
vtkPlusAgilentScopeTracker::~vtkPlusAgilentScopeTracker()
{
  /*
  string s = "hello world";
  string* s_ptr = &s;
  string* s_null_ptr = nullptr;
  string& s_null_ref = nullptr; // illegal
  string& s_ref = s; // legal

  std::fostream f("c:\output.txt");
  std::iostream& f_ref = f;
  ...PrintSelf(f_ptr, indent);
  */

  delete this->DataArray;
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

  // TODO : add in all of the other member variables we created
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
  vtkPlusDataSource* toolSource(nullptr);
  this->GetFirstActiveTool(toolSource);

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

  short* ptr = (short*)(this->SignalImage->GetScalarPointer());
  memcpy(ptr, this->DataArray, sizeof(short)*this->SampleCountPerAcquisition);

  // Find the absolute max (easy algorithm to start, TODO : use more advanced algorithm for more robust tissue barrier detection)
  ViReal64 absMaxValue(-1.0);
  ViReal64 absMaxValueIndex = 0;
  ViInt32 firstPoint = this->DataDescription.indexFirstPoint;
  for (ViInt32 i = firstPoint; i < firstPoint + this->DataDescription.returnedSamplesPerSeg; i++)
  {
    ViReal64 value = DataArray[i] * this->DataDescription.vGain - this->DataDescription.vOffset; // Volts
    if (abs(value) > absMaxValue)
    {
      absMaxValue = abs(value);
      absMaxValueIndex = i;
    }
  }

  double mmOffset = (this->DelayTimeSec * this->SpeedOfSound * 1000 / 2.0 ) + ( absMaxValueIndex * this->SpeedOfSound * 1000 / (2.0 * this->SampleFrequencyHz));

  // Calculate the mm offset of the abs max value
  this->ZOffsetToTracker->SetElement(2, 3, mmOffset);

  this->ToolTimeStampedUpdate(toolSource->GetId(), this->ZOffsetToTracker.Get(), TOOL_OK, this->FrameNumber, UNDEFINED_TIMESTAMP);
  videoSource->AddItem(this->SignalImage, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS, this->FrameNumber);
  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAgilentScopeTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  // This function will read the data from the rootConfigElement, and populate your member variables
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, SampleFrequencyHz, deviceConfig);
  this->SampleIntervalSec = 1.0 / (this->SampleFrequencyHz );

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, DelayTimeSec, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, SampleCountPerAcquisition, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, FullScale, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, Offset, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, Bandwidth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, TrigCoupling, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, Slope, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViReal64, Level, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(ViInt32, SpeedOfSound, deviceConfig);

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

  if (this->GetFirstActiveTool(source) != PLUS_SUCCESS)
  {
    LOG_ERROR("Agilent scope device requires a tool data source to write envelope-detected distance transform to.");
    return PLUS_FAIL;
  }

  // TODO : check user parameters are sane (aka: does the agilent SDK have any restrictions on parameter values)
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

  return PLUS_SUCCESS;
}