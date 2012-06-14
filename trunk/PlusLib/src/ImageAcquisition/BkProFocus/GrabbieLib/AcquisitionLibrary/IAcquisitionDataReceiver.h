#pragma once

/// <summary>
/// The IAcquisitionDataReceiver specify an interface that must be implemented in order to receive 
/// grabbed data from the AcquisitionInjector. The IAcquisitionDataReceiver object must be added
/// to the list of IAcquisitionDataReceiver objects in the AcquisitionInjector in order to be called.
/// </summary>
class IAcquisitionDataReceiver
{
public:
    /// <summary> 
    /// This method is called whenever a frame of data is available from the Research Interface.
    /// The data will be in the raw format directly from the Research Interface. The pointer
    /// will be valid during execution of this method, but not after execution, so if the data 
    /// is needed after this method returns, the method must copy it into a local buffer.
    /// </summary>
    /// <param name="lines">        Number of lines in the frame. </param>
    /// <param name="pitch">        Number of bytes between the start of two consecutive lines in frame data. </param>
    /// <param name="frameData">    Pointer to the the start of the frame. </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    virtual bool DataAvailable(int lines, int pitch, void const* frameData) = 0;

    /// <summary>
    /// This method is called before grabbing starts, giving the data receiver the possibility
    /// to initialize whatever variables it needs.
    /// </summary>
    /// <param name="samples">  Number of samples per line. </param>
    /// <param name="lines">    Number of lines in the frame. </param>
    /// <param name="pitch">    Number of bytes per line, including headers. </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    virtual bool Prepare(int samples, int lines, int pitch) = 0;

    /// <summary>
    /// This method is called after grabbing is done, giving the data receiver the possibility
    /// to deallocate unneeded variables or whatever else is needed.
    /// </summary>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    virtual bool Cleanup() = 0;

};