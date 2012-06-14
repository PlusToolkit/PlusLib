#pragma once

#include "IAcquisitionDataReceiver.h"

class SapView;
class SapBuffer;

/// <summary>
/// The SaperaViewDataReceiver displays the received data in a Sapera View object.
/// 
/// The class uses the SapView class to display the received data. Documentation for
/// the SapView class should be installed along with Sapera.
/// </summary>
class SaperaViewDataReceiver : public IAcquisitionDataReceiver
{
public:
    SaperaViewDataReceiver(SapBuffer* saperaBuffer);

    virtual ~SaperaViewDataReceiver();

    virtual bool DataAvailable(int lines, int pitch, void const* frameData);

    virtual bool Prepare(int samples, int lines, int pitch);

    virtual bool Cleanup();

private:
    SapView* sapView;
};
