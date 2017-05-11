/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusWinProbeVideoSource_h
#define __vtkPlusWinProbeVideoSource_h

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

    /*! Set ON/OFF of collecting US data. */
    PlusStatus FreezeDevice(bool freeze);

    /*! Checks whether the device is frozen or live. */
    bool IsFrozen();

    /*! Sets GUID of the probe type to be used. */
    PlusStatus SetTransducerID(std::string guid);

    /*! Gets GUID of the probe type to be used. */
    std::string GetTransducerID();

    static const unsigned lines = 128u; //number of transducers of the probe
    static const unsigned samplesPerLine = 512;

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

    void FrameCallback(int length, uint8_t *ptr);
    friend int __stdcall frameCallback(int length, uint8_t *ptr);

    bool m_frozen;
    float m_depth;
    float m_frequency;
    uint8_t m_voltage;
    std::string m_transducerID;
    double m_ADCfrequency;
    uint8_t m_bModeBuffer[samplesPerLine][lines];
    PlusTrackedFrame::FieldMapType m_customFields;

private:
    vtkPlusWinProbeVideoSource(const vtkPlusWinProbeVideoSource &); // Not implemented
    void operator=(const vtkPlusWinProbeVideoSource &); // Not implemented
};

#endif
