#pragma once

#include "AcquisitionInjector.h"
#include "AcquisitionSettings.h"

// as sapera comes with its own version of stdint.h (in cordef.h), the macro redefinition warnings are momentarily disabled
#pragma warning(push)
#pragma warning (disable: 4005)
#include "SapClassBasic.h"
#pragma warning(pop)

/// <summary>  
/// The Sapera processor implements the run function of the SapProcessor and, upon receiving a frame, 
/// calls the AcquisitionInjector. If configured to do so, it can discard frames in some order, but this
/// functionality is off by default.
/// </summary>
class SaperaProcessor : public SapProcessing
{
public:

    /// <summary>   Default constructor. </summary>
    SaperaProcessor();

    /// <summary>
    /// Constructor inherited from SapProcessing. The second and third parameter is not used, as the
    /// callback function does not match with the callback functions used by the engine.
    /// </summary>
    /// <param name="pBuffer">      [in,out] Pointer to the Sapera Buffer. </param>
    /// <param name="pCallback">    Function pointer to the function called when a new frame has been processed. </param>
    /// <param name="pContext">     [in,out] Pointer to the context given to the callback function. </param>
    SaperaProcessor(SapBuffer* pBuffer, SapProCallback pCallback = NULL, void *pContext = NULL);

    /// <summary>   Finaliser. </summary>
    virtual ~SaperaProcessor();

    /// <summary>
    /// This function is called by the Sapera Transfer object whenever a transfer has been completed.
    /// The function is specified by Sapera.
    /// </summary>
    /// <param name="pInfo">    [in,out] Contains context information, such as a pointer to the processing class instance. </param>
    static void TransferCallback(SapXferCallbackInfo* pInfo);

    /// <summary>
    /// Load settings from AcquisitionSettings. The function should be called just before grabbing
    /// starts.
    /// </summary>
    /// <param name="acquisitionSettings">  Reference to the settings to be used for acquisition. </param>
    /// <returns>   true if it succeeds, false if it fails. </returns>
    bool LoadAcquisitionSettings(const AcquisitionSettings& acquisitionSettings);

    /// <summary>   Get the AcquisitionInjector. </summary>
    /// <returns>   The currently used acquisition injector (can be NULL) </returns>
    AcquisitionInjector* GetAcquisitionInjector() const { return this->injector; }

    /// <summary>   Set the AcquisitionInjector. </summary>
    /// <param name="val">  [in,out] New value of pointer to acquisition injector. Can be NULL. </param>
    void SetAcquisitionInjector(AcquisitionInjector* val) { this->injector = val; }

    /// <summary>
    /// Loads settings from buffer. This function should be called just before grabbing starts.
    /// </summary>
    /// <returns>   True if loading was successful, and false otherwise. </returns>
    bool LoadBufferSettings();

    /// <summary>
    /// Returns the total number of frames captured in this grab.
    /// </summary>
    /// <returns>   The total number of frames captured in this grab. </returns>
    int GetFramesCaptured() { return this->framesCaptured; }

protected:

    /// <summary>
    /// The Run function is called whenever a frame is available. It is called by SapProcessing::
    /// Execute and SapProcessing::ExecuteNext, and will handle the frames in order.
    /// </summary>
    /// <returns>   TRUE if successful, FALSE otherwise. </returns>
    virtual BOOL Run();

private:
    /// <summary> Acquisition Injector used to process and inject data </summary>
    AcquisitionInjector* injector;

    // house-keeping used during grab
    /// <summary> Number of frames that Sapera has captured </summary>
    int framesCaptured;
    /// <summary> Number of frames grabbed and processed </summary>
    int framesGrabbed;
    /// <summary> Number of frames skipped in a row (book keeping) </summary>
    int framesSkippedInRow;
    /// <summary> Number of frames grabbed in a row (book keeping) </summary>
    int framesGrabbedInRow;

    // buffer information
    /// <summary> Distance between the start of two consecutive lines in buffer </summary>
    int pitchInBuffer;
    /// <summary> Actual number of frames in buffer </summary>
    int framesInBuffer;

    // data loaded from acquisition settings
    /// <summary> Adjacent frames to grab (loaded from AcquistionSettings) </summary>
    int adjacentFramesToGrab; 
    /// <summary> Frames to discard (loaded from AcquistionSettings) </summary>
    int framesToDiscard; 
    /// <summary> Number of lines in each frame </summary>
    int linesPerFrame;
};