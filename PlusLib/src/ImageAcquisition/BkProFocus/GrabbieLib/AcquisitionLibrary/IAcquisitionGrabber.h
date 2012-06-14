#pragma once

class AcquisitionSettings;
class AcquisitionInjector;

/// <summary>
/// The IAcquisitionGrabber is a base class that specify the interface of an Acquisition Grabber. 
/// 
/// Specific implementations of IAcquisitionGrabber are intended to wrap and contain all hardware
/// dependencies. All hardware dependent header files should be included in the object (*.cpp) files,
/// not in the header.
/// </summary>
class IAcquisitionGrabber
{
public:

    /// <summary> Destructor. Note that it is non-abstract. </summary>
    virtual ~IAcquisitionGrabber() {};

    /// <summary> Initialize grabber with setup data. </summary>
    /// <param name="acquisitionSettings">  Reference to the acquisition settings used during the upcoming grab. </param>
    /// <returns>   True if initialized successfully, false if an error were encountered. </returns>
    virtual bool Init(const AcquisitionSettings& acquisitionSettings) = 0;

    /// <summary>   Destroy data allocated by grabber. </summary>
    /// <returns>   True if destroyed successfully, false if an error were encountered. </returns>
    virtual bool Destroy() = 0;

    /// <summary>
    /// Start grabbing data. When data is ready to be handled, the InjectData function of the
    /// acquisitionInjector will be called with a pointer to the data and some metadata. The
    /// specified number of frames is the number of frames sent to the injector, given the current
    /// setup.
    /// </summary>
    /// <param name="acquisitionInjector">  [in,out] A pointer to the object to receive data. Can be NULL. </param>
    /// <returns>   True if grabbing started successfully, false if an error were encountered. </returns>
    virtual bool StartGrabbing(AcquisitionInjector* acquisitionInjector) = 0;

    /// <summary>   Stop grabbing data. </summary>
    /// <returns>   True if grabbing stopped successfully, false if an error were encountered. </returns>
    virtual bool StopGrabbing() = 0;

    /// <summary>
    /// Saves the contents of the acquisition buffer into the specified path. If the actual grabber
    /// does not support saving the acquisition buffer (or it does not have one), the function will
    /// return false. If the path to the file is invalid, the function will return false. If the
    /// startFrame and endFrame are set to -1, all available frames will be saved.
    /// </summary>
    /// <param name="pathToFile">   [in,out] If non-null, full pathname of the file. </param>
    /// <param name="startFrame">   The start frame to save (set to -1 to start at first frame). </param>
    /// <param name="endFrame">     The end frame to save (set to -1 to continue to last frame). </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    virtual bool SaveBuffer(const char* pathToFile, int startFrame = -1, int endFrame = -1) = 0;

    /// <summary>   Investigates if the grabber is initialized correctly. </summary>
    /// <returns>   True if grabber is initialized correctly, false otherwise. </returns>
    virtual bool IsInitialized() const = 0;

    /// <summary>
    /// Investigates if hardware used by grabber is available, that is, if drivers are installed and
    /// a grabber card exists on the system.
    /// </summary>
    /// <returns>   True if grabber card is available, false otherwise. </returns>
    virtual bool IsHardwareAvailable() const = 0;

    /// <summary>   Investigates if a grab is currently in progress. </summary>
    /// <returns>   True if grab is active, false otherwise. </returns>
    virtual bool IsGrabbing() const = 0;
};