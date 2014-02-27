#include "PlusBkProFocusCameraLinkReceiver.h"
#include "vtkBkProFocusCameraLinkVideoSource.h"

#include "ResearchInterface.h"

//////////// IAcquitisionDataReceiver interface

const int BYTES_PER_SAMPLE = 2; // One sample is one I or Q value, stored on 16 bits

const int HEADER_SIZE_BYTES = 4; // The header consists of two 16 bit fields

//----------------------------------------------------------------------------
PlusBkProFocusCameraLinkReceiver::PlusBkProFocusCameraLinkReceiver()
{
  m_ImagingMode=vtkBkProFocusCameraLinkVideoSource::RfMode;

  m_Frame = NULL;

  m_Decimation = 1;

  m_NumberOfRfSamplesPerLine = 0;
  m_MaxNumberOfLines = 0;
}

//----------------------------------------------------------------------------
PlusBkProFocusCameraLinkReceiver::~PlusBkProFocusCameraLinkReceiver()
{
}

//----------------------------------------------------------------------------
bool PlusBkProFocusCameraLinkReceiver::Prepare(int samples, int lines, int pitch)
{
  LOG_DEBUG("Prepare: samples"<<samples<<", lines="<<lines<<", pitch="<<pitch);

  int maxNumberOfLines = lines;
  // maxNumberOfRfSamples refers to the number of available 16-bit data samples (without the header)
  // need to subtract the the line header length, because the input "samples" includes the line header as well
  int numberOfRfSamplesPerLine = (samples-HEADER_SIZE_BYTES/BYTES_PER_SAMPLE) / m_Decimation;

  if (maxNumberOfLines==m_MaxNumberOfLines && numberOfRfSamplesPerLine==m_NumberOfRfSamplesPerLine && m_Frame!=NULL)
  {
    // frame buffer already allocated
    return true;
  }

  // initialize parameters
  m_MaxNumberOfLines = maxNumberOfLines;
  // adding 6 fixes the issue of mismatch between the prepared and received buffers. Perhaps this is
  // due to a header that is not accounted for.
  m_NumberOfRfSamplesPerLine = numberOfRfSamplesPerLine+6;
  
  if (m_Frame != NULL)
  {
    _aligned_free(m_Frame);
  }
  m_Frame = reinterpret_cast<unsigned char*>(_aligned_malloc(m_MaxNumberOfLines * m_NumberOfRfSamplesPerLine * BYTES_PER_SAMPLE, 16u));
  if (m_Frame == NULL)
  {
    LOG_ERROR("PlusBkProFocusCameraLinkReceiver::Prepare: Failed to allocate memory for m_Frame");
    return false;
  } 

  return true;
}

//----------------------------------------------------------------------------
bool PlusBkProFocusCameraLinkReceiver::Cleanup()
{
  if (m_Frame != NULL)
  {
    _aligned_free(m_Frame);
    m_Frame = NULL;
  }
  m_NumberOfRfSamplesPerLine = 0;
  m_MaxNumberOfLines = 0;
  return true;
}

//----------------------------------------------------------------------------
bool PlusBkProFocusCameraLinkReceiver::DataAvailable(int lines, int pitch, void const* frameData)
{
  if (frameData==NULL)
  {
    LOG_DEBUG("DataAvailable received empty m_Frame");
    return false;
  }
  if (m_Frame==NULL)
  {
    LOG_ERROR("Frame buffer is not initialized");
    return false;
  }
  if (m_CallbackVideoSource==NULL)
  {
    LOG_ERROR("No video source callback is set");
    return false;
  }

  
  const ResearchInterfaceLineHeader* header = reinterpret_cast<const ResearchInterfaceLineHeader*>(frameData);
  const unsigned char* inputFrame = reinterpret_cast<const unsigned char*>(frameData);
  
  // Copy as many sample pairs (2x16 bits) as available in the input and allocated in the output
  // pitch size is in 1x8 bits, not in samples (2x8 bits). 
  // Note the inconsistency: in Prepare(), pitch is defined in 2x8 samples!
  int numberOfSamplesInInput= (pitch-HEADER_SIZE_BYTES)/BYTES_PER_SAMPLE / m_Decimation;
  int numberOfSamplePairsInInput=numberOfSamplesInInput/2;
  int numberOfSamplePairsInOutput=m_NumberOfRfSamplesPerLine/2;    
  int numberOfSamplePairsToCopy=0;
  if (numberOfSamplePairsInOutput==numberOfSamplePairsInInput)
  {
    numberOfSamplePairsToCopy=numberOfSamplePairsInOutput;
  }
  else if (numberOfSamplePairsInOutput<numberOfSamplePairsInInput)
  {
	  LOG_WARNING("Not enough space allocated to store all the RF samples. Input: "<<numberOfSamplePairsInInput<<", output: "<<numberOfSamplePairsInOutput);
    numberOfSamplePairsToCopy=numberOfSamplePairsInOutput;
  }
  else // numberOfSamplePairsInInput < numberOfSamplePairsInOutput
  {
    LOG_WARNING("Not enough samples are available in the acquired frame, the end of the RF lines will be undefined");
    numberOfSamplePairsToCopy=numberOfSamplePairsInInput;
  }

  int numBmodeLines = 0; // number of bmode lines in this m_Frame
  for(int inputLineIndex = 0; inputLineIndex < lines; ++inputLineIndex)
  {
    // Each RF line has a header (two 16-bit fields, see ResearchInterfaceLineHeader), then data values, then some undefined values (padding) till the next line
    // Pitch is the total number of bytes of the RF line (including header, data, and padding)     
    header =  reinterpret_cast<const ResearchInterfaceLineHeader*>(inputFrame + inputLineIndex*pitch);

    if(header->ModelID != 0 || header->CFM != 0 || header->FFT != 0)
    {
      // Only process lines that refer to lines that can be converted to brightness lines,
      // so skip special lines (CFM, FFT, ...)
      continue;
    }

    if (numBmodeLines>=m_MaxNumberOfLines)
    {
      LOG_WARNING("Not enough lines are available in the frame buffer, ignore the acquired line");
      continue;
    }

    // TODO: check if header->LineLength can be used to make the numberOfSamplePairsToCopy computation more accurate
    // (now there might be junk pixels at the end of the line if the line lenght is different for different lines) 

    // 32 bit, one sample pair
    const int32_t* currentInputPosition = reinterpret_cast<const int32_t*>(inputFrame + inputLineIndex*pitch + HEADER_SIZE_BYTES);
    // Fill the output from the last line, because RF data is expected in FM orientation, so first scanline on the unmarked side of the transducer, but the first scanline in the BK buffer is the one on the marked side
    int32_t* currentOutputPosition = reinterpret_cast<int32_t*>(m_Frame + (m_MaxNumberOfLines-1-numBmodeLines)*m_NumberOfRfSamplesPerLine*BYTES_PER_SAMPLE );

    for(int samplePairIndex = 0; samplePairIndex < numberOfSamplePairsToCopy; ++samplePairIndex)
    {
      *currentOutputPosition = *currentInputPosition;
      currentInputPosition += m_Decimation; // copy every N-th sample pair (N=m_Decimation), ignore the rest
      ++currentOutputPosition;
    }

    ++numBmodeLines;
  }

  // compute bmode
  if(numBmodeLines == 0)
  {
    LOG_DEBUG("No B-mode compatible image lines were found");
    return false;
  }

  switch (m_ImagingMode)
  {
  case vtkBkProFocusCameraLinkVideoSource::BMode:
    {
      LOG_ERROR("B-mode imaging is not supported");
      break;
    }
  case vtkBkProFocusCameraLinkVideoSource::RfMode:
    {
      if (m_CallbackVideoSource!=NULL)
      {
        // AF: each sample in m_RfFrame is twice as large as in bmode, and we do not decimate
        int frameSizeInPix[2]={m_NumberOfRfSamplesPerLine, numBmodeLines}; // each I and Q value is a sample (there are numRfSamples/2 IQ pairs in one line)
        m_CallbackVideoSource->NewFrameCallback(m_Frame + (m_MaxNumberOfLines-numBmodeLines)*m_NumberOfRfSamplesPerLine*BYTES_PER_SAMPLE, frameSizeInPix, VTK_SHORT, US_IMG_RF_IQ_LINE);
      }
      break;
    }
  default:
    LOG_ERROR("Invalid imaging mode requested: "<<m_ImagingMode);
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
void PlusBkProFocusCameraLinkReceiver::SetPlusVideoSource(vtkBkProFocusCameraLinkVideoSource *videoSource)
{
  m_CallbackVideoSource = videoSource;
}

//----------------------------------------------------------------------------
void PlusBkProFocusCameraLinkReceiver::SetImagingMode(vtkBkProFocusCameraLinkVideoSource::ImagingModeType imagingMode)
{
  m_ImagingMode = imagingMode;
}

//----------------------------------------------------------------------------
void PlusBkProFocusCameraLinkReceiver::SetDecimation(int decimation)
{
  m_Decimation = decimation;
}
