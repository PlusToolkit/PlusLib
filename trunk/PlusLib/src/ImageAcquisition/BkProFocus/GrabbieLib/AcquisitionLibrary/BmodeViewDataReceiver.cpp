#pragma once

#include "CImgWrapper.h"

#include "BmodeViewDataReceiver.h"
#include "ResearchInterface.h"
#include "libbmode.h"

/// <summary>   Implementation of the functions and private variables of the BmodeViewDataReceiverImpl. </summary>
class BmodeViewDataReceiverImpl
{
public:
    BmodeViewDataReceiverImpl()
    {
        this->display = new cimg_library::CImgDisplay();
        this->frame = NULL;
        this->bmodeFrame = NULL;
        this->decimation = 2; // ignore every second IQ sample in each line

        this->params.alg= BMODE_DRC_SQRT;
        this->params.n_lines = 0;
        this->params.n_samples = 0;
        this->params.len = 0;
        this->params.min = 0;
        this->params.max = 0;
        this->params.scale = 0;
        this->params.dyn_range = 50;
        this->params.offset = 10;
    }

    ~BmodeViewDataReceiverImpl()
    {
        delete display;
    }

    bool DataAvailable(int /*lines*/, int pitch, void const* frameData)
    {
        if(this->frame == NULL || this->bmodeFrame == NULL)
        {
            return false;
        }

        // decimate received data into frame
        int bytesPerSample = 2;
        const unsigned char* inputFrame = reinterpret_cast<const unsigned char*>(frameData);
        int bmodeLines = 0; // number of bmode lines in this frame
        for(int i = 0; i < this->params.n_lines; ++i)
        {
            const int32_t* currentInputPosition = reinterpret_cast<const int32_t*>(inputFrame + i*pitch);
            const ResearchInterfaceLineHeader* header = reinterpret_cast<const ResearchInterfaceLineHeader*>(currentInputPosition);

            // only show bmode line
            if(/*header->ModelID == 0 &&*/ header->CFM == 0 && header->FFT == 0)
            {
                // increment number of bmode lines found in this frame
                ++bmodeLines;

                ++currentInputPosition; // ResearchInterfaceLineHeader is 32 bit, so look past it by adding one
                int32_t* currentOutputPosition = reinterpret_cast<int32_t*>(this->frame + i*this->params.n_samples*bytesPerSample);

                // n_samples is 16 bit samples, but we need to iterate over 32 bit iq samples, 
                // so divide by 2 to get the right number
                for(int j = 0; j < this->params.n_samples / 2; ++j)
                {
                    *currentOutputPosition = *currentInputPosition;
                    currentInputPosition += this->decimation;
                    currentOutputPosition += 1;
                }
            }
        }

        // compute bmode
        if(bmodeLines > 0)
        {
            int tempLines = this->params.n_lines;
            this->params.n_lines = bmodeLines;
            bmode_set_params_sqrt(&params);
            bmode_detect_compress_sqrt_16sc_8u(reinterpret_cast<int16_t*>(this->frame), this->bmodeFrame, &(this->params));
            this->params.n_lines = tempLines;
            bmode_set_params_sqrt(&params);

            // display result
            cimg_library::CImg<unsigned char> inputImage((const unsigned char*)bmodeFrame, this->params.n_samples / 2, bmodeLines);
            inputImage.display(*display);
            display->set_title("B-mode");
        }

        return true;
    }

    bool Prepare(int samples, int lines, int /*pitch*/)
    {
        // ensure that pointers have been garbage collected
        _aligned_free(frame);
        frame = NULL;

        _aligned_free(bmodeFrame);
        bmodeFrame = NULL;

        // initialize parameters
        this->params.n_lines = lines;
        this->params.n_samples = (samples-2) / this->decimation; // subtract 2 due to header
        
        // the number of the samples must be 16 byte aligned
        this->params.n_samples -= this->params.n_samples % 8; // each sample is 2 bytes, so mod 8

        // compute derived parameters
        bmode_set_params_sqrt(&params);

        // each sample is four bytes
        this->frame = reinterpret_cast<unsigned char*>(_aligned_malloc(4 * this->params.len, 16u));

        // bmode frame reduces two 2x16bit samples to one 8 bit sample, so it is one quarter the size of frame
        this->bmodeFrame = reinterpret_cast<unsigned char*>(_aligned_malloc(this->params.len, 16u));

        return this->frame != NULL && this->bmodeFrame != NULL;
    }

    bool Cleanup()
    {
        _aligned_free(this->frame);
        this->frame = NULL;

        _aligned_free(this->bmodeFrame);
        this->bmodeFrame = NULL;

        return true;
    }

private: 
    TBModeParams params;
    cimg_library::CImgDisplay* display;
    unsigned char* frame;
    unsigned char* bmodeFrame;
    int decimation;
};

BmodeViewDataReceiver::BmodeViewDataReceiver()
{
    impl = new BmodeViewDataReceiverImpl();
}

BmodeViewDataReceiver::~BmodeViewDataReceiver()
{
    delete impl;
}

bool BmodeViewDataReceiver::DataAvailable(int lines, int pitch, void const* frameData)
{
    return impl->DataAvailable(lines, pitch, frameData);
}

bool BmodeViewDataReceiver::Prepare(int samples, int lines, int pitch)
{
    return impl->Prepare(samples, lines, pitch);
}

bool BmodeViewDataReceiver::Cleanup() 
{
    return true;
}