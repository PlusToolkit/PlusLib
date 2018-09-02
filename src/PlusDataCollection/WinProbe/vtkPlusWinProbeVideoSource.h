/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusWinProbeVideoSource_h
#define __vtkPlusWinProbeVideoSource_h

#include <thread>

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusUsDevice.h"
#include "vtkPlusUsImagingParameters.h"

/*!
 \class vtkPlusWinProbeVideoSource
 \brief Class for acquiring ultrasound images from WinProbe ultrasound systems.

 Requires PLUS_USE_WINPROBE_VIDEO option in CMake.
 Requires the WinProbeSDK.

 \ingroup PlusLibDataCollection.
*/
class vtkPlusDataCollectionExport vtkPlusWinProbeVideoSource : public vtkPlusUsDevice
{
public:
    /*! Constructor for a smart pointer of this class*/
    static vtkPlusWinProbeVideoSource * New();
    vtkTypeMacro(vtkPlusWinProbeVideoSource, vtkPlusDevice);
    virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

    /*! Specify the device connected to this class */
    virtual bool IsTracker() const { return false; }

    /*! Read configuration from xml data */
    virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

    /*! Write configuration to xml data */
    virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

    /* Set the transmit frequency of US probe (MHz) */
    PlusStatus SetTxTxFrequency(float frequency);

    /* Get the transmit frequency of US probe (MHz) */
    float GetTxTxFrequency();

    /* Set the voltage of US probe (percent?) */
    PlusStatus SetVoltage(uint8_t voltage);

    /* Get the voltage of US probe (percent?) */
    uint8_t GetVoltage();

    /* Set the scan depth of US probe (mm) */
    PlusStatus SetSSDepth(float depth);

    /* Get the scan depth of US probe (mm) */
    float GetSSDepth();

    /* Get the width of current transducer (mm) */
    float GetTransducerWidth();

    /* Get the pixel spacing for all 3 axes (mm) */
    const double * GetCurrentPixelSpacingMm();

    /* Get the TGC value, index 0 to 7, value 0.0 to 40.0 */
    double GetTimeGainCompensation(int index);

    /* Set the TGC value, index 0 to 7, value 0.0 to 40.0 */
    PlusStatus SetTimeGainCompensation(int index, double value);

    /* Get the focal depth, index 0 to 4 */
    float GetFocalPointDepth(int index);

    /* Set the focal depth, index 0 to 4 */
    PlusStatus SetFocalPointDepth(int index, float depth);

    /*! Set ON/OFF of collecting US data. */
    PlusStatus FreezeDevice(bool freeze);

    /*! Checks whether the device is frozen or live. */
    bool IsFrozen();

    /*! Sets GUID of the probe type to be used. */
    PlusStatus SetTransducerID(std::string guid);

    /*! Gets GUID of the probe type to be used. */
    std::string GetTransducerID();

    /*! Sets the noise floor for intensity range compression. */
    void SetMinValue(const uint16_t minValue) { m_MinValue = minValue; }

    /*! Gets the noise floor for intensity range compression. */
    uint16_t GetMinValue() const { return m_MinValue; }

    /*! Sets the typical high value for intensity range compression. */
    void SetMaxValue(const uint16_t maxValue) { m_MaxValue = maxValue; }

    /*! Gets the typical high value for intensity range compression. */
    uint16_t GetMaxValue() const { return m_MaxValue; }

    /*! Sets the threshold value for switching from log to linear mapping for intensity range compression. */
    void SetLogLinearKnee(const uint16_t threshold) { m_Knee = threshold; }

    /*! Gets the threshold value for switching from log to linear mapping for intensity range compression. */
    uint16_t GetLogLinearKnee() const { return m_Knee; }

    /*! Sets the maximum output value for log mapping of intensity range. */
    void SetLogMax(const uint8_t threshold) { m_OutputKnee = threshold; }

    /*! Gets the maximum output value for log mapping of intensity range. */
    uint8_t GetLogMax() const { return m_OutputKnee; }

    static const uint32_t wraparoundTSC = 1e9;

protected:
    /*! Constructor */
    vtkPlusWinProbeVideoSource();

    /*! Destructor */
    ~vtkPlusWinProbeVideoSource();

    /*! Device-specific connect */
    virtual PlusStatus InternalConnect() VTK_OVERRIDE;

    /*! Device-specific disconnect */
    virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

    /*! Device-specific recording start */
    virtual PlusStatus InternalStartRecording() VTK_OVERRIDE;

    /*! Device-specific recording stop */
    virtual PlusStatus InternalStopRecording() VTK_OVERRIDE;

    /*! Updates internal spacing based on current depth */
    void AdjustSpacing();

    /*! Updates buffer size based on current depth */
    void AdjustBufferSize();

    //to be run in a separate thread
    void Watchdog();

    void FrameCallback(int length, char * data, char *hHeader, char *hGeometry);
    friend int __stdcall frameCallback(int length, char * data, char *hHeader, char *hGeometry);

    bool m_wrapTimeStampCounter = false;
    float m_depth = 26.0; //mm
    float m_width = 38.1; //mm
    float m_frequency = 10.9; //MHz
    uint8_t m_voltage = 40;
    std::string m_transducerID; //GUID
    double m_ADCfrequency = 60.0e6; //MHz
    double m_timestampOffset = 0; //difference between program start time and latest InternalStartRecording()
    int32_t m_transducerCount = 128;
    int32_t m_samplesPerLine = 512;
    PlusTrackedFrame::FieldMapType m_customFields;
    std::thread * m_watchdog = nullptr;
    double m_lastTimestamp = 0.0; //for watchdog
    double m_timeGainCompensation[8];
    float m_focalPointDepth[4];
    uint16_t m_MinValue = 16; //noise floor
    uint16_t m_MaxValue = 16384; //maximum typical value
    uint16_t m_Knee = 4096; // threshold value for switching from log to linear
    uint8_t m_OutputKnee = 64; // log-linear knee in output range

private:
    vtkPlusWinProbeVideoSource(const vtkPlusWinProbeVideoSource &); // Not implemented
    void operator=(const vtkPlusWinProbeVideoSource &); // Not implemented
};

#endif