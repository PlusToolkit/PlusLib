#include "PlusBkProFocusReceiver.h"
#include "vtkBkProFocusVideoSource.h"

#include "ResearchInterface.h"

//////////// IAcquitisionDataReceiver interface

//----------------------------------------------------------------------------
PlusBkProFocusReceiver::PlusBkProFocusReceiver()
{
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

//----------------------------------------------------------------------------
PlusBkProFocusReceiver::~PlusBkProFocusReceiver()
{
}

//----------------------------------------------------------------------------
bool PlusBkProFocusReceiver::Prepare(int samples, int lines, int /*pitch*/)
{
  // ensure that pointers have been garbage collected
  _aligned_free(frame);
  frame = NULL;

  _aligned_free(bmodeFrame);
  bmodeFrame = NULL;

  // initialize parameters
  this->params.n_lines = lines;
  
  // TODO: check this, in CuteGrabbie it is simply: this->params.n_samples = samples / 2;
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

//----------------------------------------------------------------------------
bool PlusBkProFocusReceiver::Cleanup()
{
  _aligned_free(this->frame);
  this->frame = NULL;

  _aligned_free(this->bmodeFrame);
  this->bmodeFrame = NULL;

  return true;
}

//----------------------------------------------------------------------------
bool PlusBkProFocusReceiver::DataAvailable(int lintes, int pitch, void const* frameData)
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
    if(/*header->ModelID == 0 &&*/ header->CFM == 0 && header->FFT == 0) // TODO: check this, in CuteGrabbie "header->ModelID == 0" is not commented out
    {

      ++currentInputPosition; // ResearchInterfaceLineHeader is 32 bit, so look past it by adding one
      int32_t* currentOutputPosition = reinterpret_cast<int32_t*>(this->frame + bmodeLines*this->params.n_samples*bytesPerSample);

      // n_samples is 16 bit samples, but we need to iterate over 32 bit iq samples, 
      // so divide by 2 to get the right number
      for(int j = 0; j < this->params.n_samples / 2; ++j) // TODO: check this, it might need to be this->params.n_samples/this->decimation/2
      {
        *currentOutputPosition = *currentInputPosition;
        currentInputPosition += this->decimation;
        currentOutputPosition += 1;
      }

      // increment number of bmode lines found in this frame
      ++bmodeLines;

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

    //cimg_library::CImg<unsigned char> inputImage((const unsigned char*)bmodeFrame, this->params.n_samples / 2, bmodeLines);
    if (this->CallbackVideoSource!=NULL)
    {
      // the image is stored in memory line-by-line, thus the orientation is FM or FU (not the usual MF or UF)
      int frameSizeInPix[2]={this->params.n_samples/2, bmodeLines};      // TODO: check this, it may need to be {this->params.n_samples/2, this->params.n_lines} - from CuteGrabbie
      const int numberOfBitsPerPixel=8;
      this->CallbackVideoSource->NewFrameCallback(bmodeFrame, frameSizeInPix, numberOfBitsPerPixel);
    }
  }

  return true;
}

//----------------------------------------------------------------------------
void PlusBkProFocusReceiver::SetPlusVideoSource(vtkBkProFocusVideoSource *videoSource)
{
  this->CallbackVideoSource = videoSource;
}
