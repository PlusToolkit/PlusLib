#include "PlusBkProFocusReceiver.h"
#include "vtkBkProFocusVideoSource.h"

#include "ResearchInterface.h"

//////////// IAcquitisionDataReceiver interface

//----------------------------------------------------------------------------
PlusBkProFocusReceiver::PlusBkProFocusReceiver()
{
  this->ImagingMode=vtkBkProFocusVideoSource::RfMode;

  m_Frame = NULL;
  m_RfFrame = NULL;
  m_BModeFrame = NULL;

  m_Decimation = 2; // ignore IQ samples in each line

  m_BModeConvertParams.alg= BMODE_DRC_SQRT;
  m_BModeConvertParams.n_lines = 0;
  m_BModeConvertParams.n_samples = 0;
  m_BModeConvertParams.len = 0;
  m_BModeConvertParams.min = 0;
  m_BModeConvertParams.max = 0;
  m_BModeConvertParams.scale = 0;
  m_BModeConvertParams.dyn_range = 50;
  m_BModeConvertParams.offset = 10;

  m_NumberOfRfSamples = 0;
}

//----------------------------------------------------------------------------
PlusBkProFocusReceiver::~PlusBkProFocusReceiver()
{
}

//----------------------------------------------------------------------------
bool PlusBkProFocusReceiver::Prepare(int samples, int lines, int pitch)
{
  LOG_DEBUG("Prepare: samples"<<samples<<", lines="<<lines<<", pitch="<<pitch);

  // initialize parameters
  m_BModeConvertParams.n_lines = lines;

  // TODO: check this, in CuteGrabbie it is simply: m_BModeConvertParams.n_samples = samples / 2;
  m_NumberOfRfSamples = samples-2; // subtract 2 due to header
  m_BModeConvertParams.n_samples = m_NumberOfRfSamples / 2;
  

  // the number of the samples must be 16 byte aligned
  m_BModeConvertParams.n_samples -= m_BModeConvertParams.n_samples % 8; // each sample is 2 bytes, so mod 8

  // compute derived parameters
  bmode_set_params_sqrt(&m_BModeConvertParams);
  
  // Allocate memory for storage buffers
  
  if(m_Frame != NULL)
  {
    _aligned_free(m_Frame);
  }
  // m_BModeConvertParams.len = number of B-mode pixels in the image
  // one B-mode pixel is computed from an IQ sample = 2*16 bits = 32 bits = 4 bytes
  // therefore we have to allocate 4 bytes for each B-mode pixel
  m_Frame = reinterpret_cast<unsigned char*>(_aligned_malloc(4 * m_BModeConvertParams.len, 16u));
  if (m_Frame == NULL)
  {
    LOG_ERROR("PlusBkProFocusReceiver::Prepare: Failed to allocate memory for m_Frame");
  }
  
  if(m_BModeFrame != NULL)
  {
    _aligned_free(m_BModeFrame);
  }
  // m_BModeConvertParams.len = number of B-mode pixels in the image
  m_BModeFrame = reinterpret_cast<unsigned char*>(_aligned_malloc(m_BModeConvertParams.len, 16u));
  if (m_BModeFrame == NULL)
  {
    LOG_ERROR("PlusBkProFocusReceiver::Prepare: Failed to allocate memory for m_BModeFrame");
  }

  // Prepare the buffer to copy the input entirely
  if(m_RfFrame != NULL)
  {
    _aligned_free(m_RfFrame);
  }  
  m_RfFrame = reinterpret_cast<unsigned char*>(_aligned_malloc(m_BModeConvertParams.n_lines*m_NumberOfRfSamples*2, 32u));
  if (m_RfFrame == NULL)
  {
    LOG_ERROR("PlusBkProFocusReceiver::Prepare: Failed to allocate memory for m_RfFrame");
  }

  return (m_Frame != NULL) && (m_BModeFrame != NULL) && (m_RfFrame != NULL);
}

//----------------------------------------------------------------------------
bool PlusBkProFocusReceiver::Cleanup()
{
  if(m_Frame != NULL)
  {
    _aligned_free(m_Frame);
    m_Frame = NULL;
  }
  if(m_BModeFrame != NULL)
  {
    _aligned_free(m_BModeFrame);
    m_BModeFrame = NULL;
  }
  if(m_RfFrame != NULL)
  {
    _aligned_free(m_RfFrame);
    m_RfFrame = NULL;
  }
  return true;
}

//----------------------------------------------------------------------------
bool PlusBkProFocusReceiver::DataAvailable(int lines, int pitch, void const* frameData)
{
  if(m_Frame == NULL || m_BModeFrame == NULL)
  {
    LOG_DEBUG("DataAvailable received empty m_Frame");
    return false;
  }

  if (this->CallbackVideoSource==NULL)
  {
    LOG_WARNING("No BK video source callback is set");
    return false;
  }

  const ResearchInterfaceLineHeader* header = reinterpret_cast<const ResearchInterfaceLineHeader*>(frameData);
  const unsigned char* inputFrame = reinterpret_cast<const unsigned char*>(frameData);

  // decimate received data into m_Frame
  const int bytesPerSample = 2;

  int numBmodeLines = 0; // number of bmode lines in this m_Frame
  for(int i = 0; i < m_BModeConvertParams.n_lines; ++i)
  {
    // AF: each line has a header (1 32-bit sample) and gap after the data
    //  pitch is the offset of header for the ith sample line 
    const int32_t* currentInputPosition = reinterpret_cast<const int32_t*>(inputFrame + i*pitch);
    header =  reinterpret_cast<const ResearchInterfaceLineHeader*>(currentInputPosition);

    // only show bmode line --> AF: Should we copy RF data outside this if?
    if(header->ModelID == 0 && header->CFM == 0 && header->FFT == 0)
    {
      int32_t* currentOutputPosition = reinterpret_cast<int32_t*>(m_Frame + numBmodeLines*m_BModeConvertParams.n_samples*bytesPerSample);
      // AF: each sample in m_RfFrame is twice as large as in bmode, and we do not decimate
      int32_t* currentRFOutputPosition = reinterpret_cast<int32_t*>(m_RfFrame + numBmodeLines*m_NumberOfRfSamples*bytesPerSample);

      ++currentInputPosition; // AF: skip the header

      // n_samples is 16 bit samples, but we need to iterate over 32 bit iq samples, 
      // so divide by 2 to get the right number
      for(int j = 0; j < m_BModeConvertParams.n_samples /m_Decimation; ++j)
      {
        *currentOutputPosition = *currentInputPosition;
        currentOutputPosition += 1;		

        *currentRFOutputPosition = *currentInputPosition;
        *(currentRFOutputPosition+1) = *(currentInputPosition+1);
        currentRFOutputPosition += 2;

        currentInputPosition += m_Decimation;
      }

      ++numBmodeLines;
    }
  }

  // compute bmode
  if(numBmodeLines == 0)
  {
    LOG_DEBUG("No B-mode image lines were found");
    return false;
  }

  switch (this->ImagingMode)
  {
  case vtkBkProFocusVideoSource::BMode:
    {
      int tempLines = m_BModeConvertParams.n_lines;
      m_BModeConvertParams.n_lines = numBmodeLines;
      bmode_set_params_sqrt(&m_BModeConvertParams);
      bmode_detect_compress_sqrt_16sc_8u(reinterpret_cast<int16_t*>(m_Frame), m_BModeFrame, &(m_BModeConvertParams));
      m_BModeConvertParams.n_lines = tempLines;
      bmode_set_params_sqrt(&m_BModeConvertParams);
      // The image is stored in memory line-by-line, thus the orientation is FM or FU (not the usual MF or UF)
      int frameSizeInPix[2]={m_BModeConvertParams.n_samples/2, m_BModeConvertParams.n_lines};      // TODO: check this, it may need to be {m_BModeConvertParams.n_samples/2, m_BModeConvertParams.n_lines} - from CuteGrabbie; or try to change m_BModeConvertParams.n_lines to numBmodeLines
      const int numberOfBitsPerPixel=8;
      this->CallbackVideoSource->NewFrameCallback(m_BModeFrame, frameSizeInPix, itk::ImageIOBase::UCHAR, US_IMG_BRIGHTNESS);
      break;
    }
  case vtkBkProFocusVideoSource::RfMode:
    {
      if (this->CallbackVideoSource!=NULL)
      {
        // AF: each sample in m_RfFrame is twice as large as in bmode, and we do not decimate
        int frameSizeInPix[2]={m_NumberOfRfSamples, m_BModeConvertParams.n_lines}; // each I and Q value is a sample (there are numRfSamples/2 IQ pairs in one line)
        this->CallbackVideoSource->NewFrameCallback(m_RfFrame, frameSizeInPix, itk::ImageIOBase::SHORT, US_IMG_RF_IQ_LINE);
      }
      break;
    }
  default:
    LOG_ERROR("Invalid imaging mode requested: "<<this->ImagingMode);
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
void PlusBkProFocusReceiver::SetPlusVideoSource(vtkBkProFocusVideoSource *videoSource)
{
  this->CallbackVideoSource = videoSource;
}

//----------------------------------------------------------------------------
void PlusBkProFocusReceiver::SetImagingMode(vtkBkProFocusVideoSource::ImagingModeType imagingMode)
{
  this->ImagingMode = imagingMode;
}
