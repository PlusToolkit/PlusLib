#pragma once

#include "IAcquisitionDataReceiver.h"

class BmodeViewDataReceiverImpl;

/// <summary>
/// The BmodeViewDataReceiver converts the received data to B-Mode and display it using CImg. 
/// 
/// Documentation for CImg is available from http://cimg.sourceforge.net/reference/index.html
/// </summary>
class BmodeViewDataReceiver : public IAcquisitionDataReceiver
{
public:
    BmodeViewDataReceiver();

    virtual ~BmodeViewDataReceiver();

    virtual bool DataAvailable(int lines, int pitch, void const* frameData);

    virtual bool Prepare(int samples, int lines, int pitch);

    virtual bool Cleanup();

private:
    BmodeViewDataReceiverImpl* impl;
};
