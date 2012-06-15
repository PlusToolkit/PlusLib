#include "SaperaProcessor.h"

SaperaProcessor::SaperaProcessor() : SapProcessing(NULL,NULL,NULL)
{
    this->injector = NULL;
    this->framesCaptured = 0;
    this->framesGrabbed = 0;
    this->framesGrabbedInRow = 0;
    this->framesSkippedInRow = 0;
    this->pitchInBuffer = 0;
    this->framesInBuffer = 0;
    this->adjacentFramesToGrab = 0;
    this->framesToDiscard = 0;
    this->linesPerFrame = 0;
}


SaperaProcessor::SaperaProcessor(SapBuffer* pBuffer, SapProCallback pCallback, void *pContext) 
    : SapProcessing(pBuffer,pCallback,pContext)
{
    this->injector = NULL;
    this->framesCaptured = 0;
    this->framesGrabbed = 0;
    this->framesGrabbedInRow = 0;
    this->framesSkippedInRow = 0;
    this->pitchInBuffer = 0;
    this->framesInBuffer = 0;
    this->adjacentFramesToGrab = 0;
    this->framesToDiscard = 0;
    this->linesPerFrame = 0;
}

SaperaProcessor::~SaperaProcessor()
{
    // do nothing, destruction of the buffer is handled externally
}

void SaperaProcessor::TransferCallback(SapXferCallbackInfo* pInfo)
{
	SaperaProcessor* saperaProcessorInstance = (SaperaProcessor*) pInfo->GetContext();

    // Execute is a Sapera method that (eventually) calls the Run method.
    // It will process as many frames as possible without slowing down 
    // transfer, so it may skip some frames. As an alternative, 
    // ExecuteNext() can be used, however it may slow down the acquisition
    // process if the processing cannot keep up the the transfer.
	saperaProcessorInstance->Execute();
}

BOOL SaperaProcessor::Run()
{
    ++framesCaptured;

    // investigate if we are to grab or skip the next frame
    if (framesGrabbedInRow >= adjacentFramesToGrab)
    {
        // we have grabbed the needed number of frames, so consider discarding frames
        if (framesSkippedInRow < framesToDiscard)
        {
            // discard this frame
            ++framesSkippedInRow;
            return TRUE;
        }
        else
        {
            // stop skipping and start grabbing
            framesSkippedInRow = 0;
            framesGrabbedInRow = 0;
        }
    }
    ++framesGrabbedInRow;
    ++framesGrabbed;

    // load frame into frame data
    if(injector != NULL)
    {
        void* frameData;
        this->m_pBuffers->GetAddress(m_pBuffers->GetIndex(), &frameData);

        // inject data
        bool status =  injector->InjectData(linesPerFrame, pitchInBuffer, frameData);

        // release frame data so it can be recycled
        // it may be possible to improve performance by running this function when 
        // the data has been copied from the frameData buffer in the InjectData function
        this->m_pBuffers->ReleaseAddress(frameData);

        return (status ? TRUE : FALSE);
    }
    return TRUE;
}

bool SaperaProcessor::LoadBufferSettings()
{
    if (this->m_pBuffers != NULL)
    {
        this->pitchInBuffer = this->m_pBuffers->GetPitch();
        this->framesInBuffer = this->m_pBuffers->GetCount();

        return true;
    }
    return false;
}

bool SaperaProcessor::LoadAcquisitionSettings(const AcquisitionSettings& acquisitionSettings)
{
    this->framesCaptured = 0;
    this->framesGrabbed = 0;
    this->framesGrabbedInRow = 0;
    this->framesSkippedInRow = 0;

    this->adjacentFramesToGrab = acquisitionSettings.GetAdjacentFrames();
    this->framesToDiscard = acquisitionSettings.GetDiscardFrames();
    this->linesPerFrame = acquisitionSettings.GetLinesPerFrame();

    return true;
}
