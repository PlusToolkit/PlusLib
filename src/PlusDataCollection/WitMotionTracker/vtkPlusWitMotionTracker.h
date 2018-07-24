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
    virtual PlusStatus InternalConnect();

    virtual bool IsTracker() const { return true; }

protected:
    /*! Retrieves orientation from the string data streamed by the sensor */
    PlusStatus ParseMessage(std::string& textReceived, double* rotationQuat);

    PlusStatus DecodeData();

    PlusStatus ReceiveData();

    virtual PlusStatus NotifyConfigured();

    vtkPlusWitMotionTracker();
    ~vtkPlusWitMotionTracker();

protected:
    vtkPlusDataSource* Accelerometer;

    static const int MAX_DATA_LENGTH1 = 11;
    typedef std::array<unsigned char, vtkPlusWitMotionTracker::MAX_DATA_LENGTH1> StreamBufferType;

    // Only need 11 bytes in the buffer 
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
