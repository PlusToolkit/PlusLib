/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusWitMotionTracker_h
#define __vtkPlusWitMotionTracker_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusGenericSerialDevice.h"

/*!
\class vtkPlusWitMotionTracker
\brief Interface for acquiring data from a Microchip MM7150 motion module

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusWitMotionTracker : public vtkPlusGenericSerialDevice
{
public:
    static vtkPlusWitMotionTracker *New();
    vtkTypeMacro(vtkPlusWitMotionTracker, vtkPlusGenericSerialDevice);

    virtual PlusStatus InternalUpdate();

    virtual bool IsTracker() const { return true; }

protected:
    /*! 
     * Decode the 11 byte frames provided by the Wit motion device
     * See https://github.com/PlusToolkit/PlusDoc/external/WitMotion/BWT901C.zip
     * for sample code, app, and more.
     */
    PlusStatus DecodeData();

    /*! Receive an 11 byte frame over serial */
    PlusStatus ReceiveData();

    virtual PlusStatus NotifyConfigured();

    vtkPlusWitMotionTracker();
    ~vtkPlusWitMotionTracker();

protected:
    vtkPlusDataSource*                          Accelerometer;

    // Only need MAX_DATA_LENGTH bytes in the buffer 
    static const int                            MAX_DATA_LENGTH = 11;
    typedef std::array<unsigned char, vtkPlusWitMotionTracker::MAX_DATA_LENGTH> StreamBufferType;

    StreamBufferType                            StreamData;
    std::array<double, 4>                       Orientation;
    std::array<double, 4>                       Acceleration;
    std::array<double, 4>                       Velocity;
    double                                      Temperature;

private:
    vtkPlusWitMotionTracker(const vtkPlusWitMotionTracker&);
    void operator=(const vtkPlusWitMotionTracker&);
};

#endif
