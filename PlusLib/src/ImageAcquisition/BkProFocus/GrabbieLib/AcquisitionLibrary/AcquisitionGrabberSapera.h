#pragma once

#include "IAcquisitionGrabber.h"
#include "ResearchInterface.h"

// Forward declarations so we do not have to include Sapera headers directly into our header files
class SapAcquisition;
class SapBuffer;
class SapLocation;
class SapTransfer;
class SaperaProcessor;
class AcquisitionInjector;
class AcquisitionSettings;

/// <summary>
/// The AcquisitionGrabber grab iq data from a capture card and call the callback function in the
/// injector.
/// </summary>
class AcquisitionGrabberSapera : public IAcquisitionGrabber
{
public:
    AcquisitionGrabberSapera();
    ~AcquisitionGrabberSapera();

    bool Init(const AcquisitionSettings& acquisitionSettings);

    bool Destroy();

    bool StartGrabbing(AcquisitionInjector* acquisitionInjector);

    bool StopGrabbing();

    bool SaveBuffer(const char* pathToFile, int startFrame = -1, int endFrame = -1);

    bool IsInitialized() const;

    bool IsHardwareAvailable() const;

    bool IsGrabbing() const;

    /// <summary>
    /// Gets a pointer to the sapera buffer used by the grabber. The pointer can be NULL.
    /// </summary>
    /// <returns>   Pointer to the sapera buffer used by grabber. Can be NULL. </returns>
    SapBuffer* GetBuffer() const;

    /// <summary>
    /// Investigates if hardware used by grabber is available, that is, if drivers are installed and
    /// a grabber card exists on the system.
    /// </summary>
    /// <returns>   True if grabber card is available, false otherwise. </returns>
    static bool IsSaperaAvailable();

protected:

    /// <summary>
    /// Find the available Sapera servers on the machine, investigate if they support acquisition and
    /// choose one to use.
    /// </summary>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool FindSaperaDevice();

    /// <summary>   Allocate Sapera objects. </summary>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool AllocateSapera();

    /// <summary>   Destroy and delete all allocated Sapera objects. </summary>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool DestroySapera();

    /// <summary>
    /// Template function to destroy and delete the specified Sapera object. 
    /// The compiler should be able to deduce the template parameter, so there
    /// should be no reason to explicitly specify it.
    /// </summary>
    /// <tparam name="saperaObject"> [in,out] If non-null, the sapera object. </tparam>
    /// <param name="saperaObject"> [in,out] If non-null, the sapera object. </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    template<typename SaperaObjectType>
    bool DestroySaperaObject(SaperaObjectType** saperaObject);

    /// <summary> Pointer to object containing settings for grabber </summary>
    const AcquisitionSettings* settings; 

    /// <summary> Research Interface </summary>
    ResearchInterface researchInterface;

    /// <summary> The sapera location </summary>
    SapLocation* saperaLocation;
    /// <summary> The sapera acquisition </summary>
    SapAcquisition* saperaAcquisition;
    /// <summary> Buffer for sapera data </summary>
    SapBuffer* saperaBuffer;
    /// <summary> The sapera processor </summary>
    SaperaProcessor* saperaProcessor; 
    /// <summary> The sapera transfer </summary>
    SapTransfer* saperaTransfer; 

    /// <summary> Board name - we use a ptr to escape including Sapera directly in this header file. </summary>
    char* acquisitionServerName;
    /// <summary> Index of the capture device on server. </summary>
    int acquisitionDeviceIndex; 
    /// <summary> True if sapera objects are initalized correctly, false otherwise. </summary>
    bool saperaInitialized;
};