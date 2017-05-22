/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusWinProbeVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUSImagingParameters.h"

#include "TestSessionWrapper.h"
#include "UltraVisionManagedDll.h"

#include <algorithm>
#include <PlusMath.h>

vtkStandardNewMacro(vtkPlusWinProbeVideoSource);

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    //os << indent << "CustomFields: " << m_customFields << std::endl;
    os << indent << "TransducerID: " << this->m_transducerID << std::endl;
    os << indent << "Frozen: " << this->m_frozen << std::endl;
    os << indent << "Voltage: " << this->GetVoltage() << std::endl;
    os << indent << "Frequency: " << this->GetTxTxFrequency() << std::endl;
    os << indent << "Depth: " << this->GetSSDepth() << std::endl;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
    LOG_TRACE("vtkPlusWinProbeVideoSource::ReadConfiguration");
    XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

    XML_READ_STRING_ATTRIBUTE_REQUIRED(TransducerID, deviceConfig);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, TxTxFrequency, deviceConfig);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, SSDepth, deviceConfig);
    //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(uint8_t, Voltage, deviceConfig);
    //above macro is not defined for uint8_t, so we implement it manually below:
    unsigned long tmpValue = 0;
    if (deviceConfig->GetScalarAttribute("Voltage", tmpValue))
    {
        this->SetVoltage(tmpValue);
    }

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
    XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

    deviceConfig->SetAttribute("TransducerID", this->m_transducerID.c_str());
    deviceConfig->SetFloatAttribute("TxTxFrequency", this->GetTxTxFrequency());
    deviceConfig->SetFloatAttribute("SSDepth", this->GetSSDepth());
    deviceConfig->SetUnsignedLongAttribute("Voltage", this->GetVoltage());

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource *thisPtr = NULL;

//this callback function is invoked after each frame is ready
int __stdcall frameCallback(int length, byte *ptr)
{
    thisPtr->FrameCallback(length, ptr);
    return length;
}

//at the front it goes
//3 bytes of counter
//1 byte of spatial compounding angle number
//3 bytes of counter
//1 byte of spatial compounding angle number
//4 bytes of time stamp
//4 bytes of timestamp
// timestamp counters are ticks since Execute on sampling frequency GetADCSamplingRate

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::FrameCallback(int length, uint8_t * ptr)
{
    vtkPlusDataSource* aSource = NULL;
    if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to retrieve the video source in the capturing device.");
        return;
    }

    uint32_t* timeStampCounter = reinterpret_cast<uint32_t*>(ptr + 8);
    double timestamp = *timeStampCounter / m_ADCfrequency;
    const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();
    LOG_DEBUG("Frame: " << FrameNumber << ". Timestamps:  vtk: " << unfilteredTimestamp << "  WP: " << timestamp);

    typedef uint16_t Frame[lines][samplesPerLine];
    Frame & frame = *reinterpret_cast<Frame*>(ptr + 16);
    const float logFactor = 22.989191664828804937976079129771f; // =255/ln(2^16-1)

#pragma omp parallel for
    for (unsigned l = 0; l < lines; l++)
    {
        for (unsigned s = 0; s < samplesPerLine; s++)
        {
            m_bModeBuffer[s][l] = static_cast<uint8_t>(logFactor*log(float(1 + frame[l][s])));
        }
    }
    

    int frameSize[3] = { 128,512,1 };

    if (aSource->GetNumberOfItems() == 0)
    {
        LOG_DEBUG("Set up image buffer for WinProbe");
        aSource->SetPixelType(VTK_UNSIGNED_CHAR);
        aSource->SetImageType(US_IMG_BRIGHTNESS);
        aSource->SetInputFrameSize(frameSize);

        LOG_INFO("Frame size: " << frameSize[0] << "x" << frameSize[1]
            << ", pixel type: " << vtkImageScalarTypeNameMacro(aSource->GetPixelType())
            << ", buffer image orientation: "
            << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetInputImageOrientation()));
    }

    if (aSource->AddItem(m_bModeBuffer,
        aSource->GetInputImageOrientation(),
        frameSize, VTK_UNSIGNED_CHAR,
        1, US_IMG_BRIGHTNESS, 0,
        this->FrameNumber,
        unfilteredTimestamp,
        UNDEFINED_TIMESTAMP,
        &this->m_customFields) != PLUS_SUCCESS)
    {
        LOG_ERROR("Error adding item to video source " << aSource->GetSourceId());
    }

    this->FrameNumber++;
    this->Modified();
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::vtkPlusWinProbeVideoSource()
    : m_frozen(true)
    , m_depth(36.0)
    , m_voltage(100)
    , m_frequency(10.0)
    , m_ADCfrequency(60.0e6)
{
    this->RequireImageOrientationInConfiguration = true;

    m_customFields;

    int imageSize[2] = { 512,128 };
    this->CurrentPixelSpacingMm[0] = this->GetSSDepth() / imageSize[0];
    this->CurrentPixelSpacingMm[1] = 36.0f / imageSize[1];
    this->CurrentPixelSpacingMm[2] = 1.0;

    Callback funcPtr = &frameCallback;
    thisPtr = this;
    WPSetCallback(funcPtr);
    WPInitialize();
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::~vtkPlusWinProbeVideoSource()
{
    if (this->Connected)
    {
        this->Disconnect();
    }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalConnect()
{
    LOG_DEBUG("Connect to WinProbe");
    WPConnect();
    WPLoadDefault();
    WPSetTransducerID(this->m_transducerID);
    //m_ADCfrequency = GetADCSamplingRate();
    SetCompoundAngleCount(0);

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalDisconnect()
{
    LOG_DEBUG("Disconnect from WinProbe");
    if (!this->m_frozen)
    {
        this->InternalStopRecording();
    }
    WPDisconnect();
    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStartRecording()
{
    m_frozen = false;
    WPExecute();
    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStopRecording()
{
    m_frozen = true;
    WPStopScanning();
    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::FreezeDevice(bool freeze)
{

    if (m_frozen == freeze) //already in desired mode
    {
        return PLUS_SUCCESS;
    }

    if (freeze)
    {
        this->InternalStopRecording();
    }
    else
    {
        this->InternalStartRecording();
    }

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
bool vtkPlusWinProbeVideoSource::IsFrozen()
{
    return m_frozen;
}


// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetTxTxFrequency(float frequency)
{
    m_frequency = frequency;
    if (Connected)
    {
        ::SetTxTxFrequency(frequency);
    }
    return PLUS_SUCCESS;
}

float vtkPlusWinProbeVideoSource::GetTxTxFrequency()
{
    if (Connected)
    {
        m_frequency = ::GetTxTxFrequency();
    }
    return m_frequency;
}

PlusStatus vtkPlusWinProbeVideoSource::SetVoltage(uint8_t voltage)
{
    m_voltage = voltage;
    if (Connected)
    {
        //::SetVoltage(voltage);
    }
    return PLUS_SUCCESS;
}

uint8_t vtkPlusWinProbeVideoSource::GetVoltage()
{
    if (Connected)
    {
        //m_voltage = ::GetVoltage();
    }
    return m_voltage;
}

PlusStatus vtkPlusWinProbeVideoSource::SetSSDepth(float depth)
{
    m_depth = depth;
    if (Connected)
    {
        ::SetSSDepth(depth);
    }
    return PLUS_SUCCESS;
}

float vtkPlusWinProbeVideoSource::GetSSDepth()
{
    if (Connected)
    {
        m_depth = ::GetSSDepth();
    }
    return m_depth;
}

PlusStatus vtkPlusWinProbeVideoSource::SetTransducerID(std::string guid)
{
    this->m_transducerID = guid;
    if (Connected)
    {
        WPSetTransducerID(guid);
    }
    return PLUS_SUCCESS;
}

std::string vtkPlusWinProbeVideoSource::GetTransducerID()
{
    return this->m_transducerID;
}
