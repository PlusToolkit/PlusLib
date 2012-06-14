#pragma once

#include "IAcquisitionDataReceiver.h"

class AcquisitionInjectorImpl;

/// <summary>
/// The AcquisitionInjector receives data and injects it into the configured targets. 
/// 
/// Data receivers must implement the IAcquisitionDataReceiver interface, and can be
/// added using the method AddDataReceiver and removed using the method AddDataReceiver.
/// Injection of data into targets can be stopped using the StopInjection method. The method
/// may still inject the currently received frame before actually stopping further injections.
/// 
/// The methods InjectData, PrepareInjection and CleanupInjection are called by the grabber, and
/// should not be called externally.
/// </summary>
class AcquisitionInjector
{
public:
    AcquisitionInjector();
    ~AcquisitionInjector();

    /// <summary> 
    /// Injects raw frame data into the configured targets.
    /// </summary>
    /// <param name="lines">        Number of lines in the frame. </param>
    /// <param name="pitch">        Number of bytes between the start of two consecutive lines in frame data. </param>
    /// <param name="frameData">    Pointer to the the start of the frame. </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool InjectData(int lines, int pitch, void const* frameData);

    /// <summary>
    /// Prepares the AcquisitionInjector to receive data by allocating internal buffers, and loading
    /// misc. settings.
    /// </summary>
    /// <param name="samples">  Number of samples per line. </param>
    /// <param name="lines">    Number of lines in the frame. </param>
    /// <param name="pitch">    Number of bytes per line, including headers. </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool PrepareInjection(int samples, int lines, int pitch);

    /// <summary>   Cleans up internal buffers and other artifacts from injection. </summary>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool CleanupInjection();

    /// <summary>
    /// Adds a data receiver to list of data receivers.
    /// </summary>
    /// <param name="dataReceiver">    Pointer to data receiver to add. </param>
    void AddDataReceiver(IAcquisitionDataReceiver* dataReceiver);

    /// <summary>
    /// Removes a data receiver from list of data receivers
    /// </summary>
    /// <param name="dataReceiver">    Pointer to data receiver to remove. </param>
    void RemoveDataReceiver(IAcquisitionDataReceiver* dataReceiver);

    /// <summary>   Stops injection of data, starting with the next received frame. </summary>
    void StopInjection();

private:
    AcquisitionInjectorImpl* impl;

};