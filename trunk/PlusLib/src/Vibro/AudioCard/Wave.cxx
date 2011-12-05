/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "Wave.h"

namespace VibroLib
{
namespace AudioCard
{

//----------------------------------------------------------------------------
Wave::Wave(void)
{
  wave_riff = new WaveRIFF();
  wave_fmt = new WaveFMT();
  wave_data = new WaveDATA();
  wave = NULL;
}

//----------------------------------------------------------------------------
Wave::~Wave(void)
{
  delete wave_riff;
  delete wave_fmt;
  delete wave_data;
  if (wave)
    delete [] wave;
}

//----------------------------------------------------------------------------
void Wave::FromData(unsigned char* data, size_t data_size, size_t bits_per_sample, size_t samples_per_second, size_t channels)
{
  size_t header_offset = sizeof(WaveRIFF)+sizeof(WaveFMT)+sizeof(WaveDATA);

  // Allocated memory for the wave:
  wave = new BYTE[header_offset + data_size];

  // Initialize headers:
  wave_riff->riffID[0] = 'R'; 
  wave_riff->riffID[1] = 'I'; 
  wave_riff->riffID[2] = 'F'; 
  wave_riff->riffID[3] = 'F'; 

  wave_riff->riffFORMAT[0] = 'W'; 
  wave_riff->riffFORMAT[1] = 'A'; 
  wave_riff->riffFORMAT[2] = 'V'; 
  wave_riff->riffFORMAT[3] = 'E'; 

  wave_riff->riffSIZE = static_cast<DWORD>(data_size + header_offset - 8);		

  wave_fmt->fmtID[0] = 'f'; 
  wave_fmt->fmtID[1] = 'm'; 
  wave_fmt->fmtID[2] = 't'; 
  wave_fmt->fmtID[3] = ' '; 

  wave_fmt->fmtSIZE = sizeof(WAVEFORMATEX);
  wave_fmt->fmtFORMAT.wFormatTag = 1;
  wave_fmt->fmtFORMAT.nChannels = static_cast<WORD>(channels);
  wave_fmt->fmtFORMAT.nSamplesPerSec = static_cast<DWORD>(samples_per_second);
  wave_fmt->fmtFORMAT.nBlockAlign = static_cast<WORD>((bits_per_sample / 8) * channels);
  wave_fmt->fmtFORMAT.nAvgBytesPerSec = wave_fmt->fmtFORMAT.nBlockAlign*static_cast<DWORD>(samples_per_second);
  wave_fmt->fmtFORMAT.wBitsPerSample = static_cast<WORD>(bits_per_sample);
  wave_fmt->fmtFORMAT.cbSize = 0;

  wave_data->dataID[0] = 'd'; 
  wave_data->dataID[1] = 'a'; 
  wave_data->dataID[2] = 't'; 
  wave_data->dataID[3] = 'a'; 

  wave_data->dataSIZE = static_cast<DWORD>(data_size);

  // Copy headers:
  memcpy(reinterpret_cast<void*>(wave), reinterpret_cast<void*>(wave_riff), sizeof(WaveRIFF));
  memcpy(reinterpret_cast<void*>(wave + sizeof(WaveRIFF)), reinterpret_cast<void*>(wave_fmt), sizeof(WaveFMT));
  memcpy(reinterpret_cast<void*>(wave + sizeof(WaveRIFF) + sizeof(WaveRIFF)), reinterpret_cast<void*>(wave_riff), sizeof(WaveDATA));

  // Copy data:
  memcpy(reinterpret_cast<void*>(wave + header_offset), reinterpret_cast<void*>(data), data_size);
}

//----------------------------------------------------------------------------
void Wave::FromSignal(std::vector<double> signal, size_t samples_per_second, size_t channels)
{
  size_t bits_per_sample = 16;
  size_t data_size = signal.size() * bits_per_sample / 8;
  size_t header_offset = sizeof(WaveRIFF)+sizeof(WaveFMT)+sizeof(WaveDATA);

  // Allocated memory for the wave:
  wave = new BYTE[header_offset + data_size];

  // Initialize headers:
  wave_riff->riffID[0] = 'R'; 
  wave_riff->riffID[1] = 'I'; 
  wave_riff->riffID[2] = 'F'; 
  wave_riff->riffID[3] = 'F'; 

  wave_riff->riffFORMAT[0] = 'W'; 
  wave_riff->riffFORMAT[1] = 'A'; 
  wave_riff->riffFORMAT[2] = 'V'; 
  wave_riff->riffFORMAT[3] = 'E'; 

  wave_riff->riffSIZE = static_cast<DWORD>(data_size + header_offset - 8);		

  wave_fmt->fmtID[0] = 'f'; 
  wave_fmt->fmtID[1] = 'm'; 
  wave_fmt->fmtID[2] = 't'; 
  wave_fmt->fmtID[3] = ' '; 

  wave_fmt->fmtSIZE = sizeof(WAVEFORMATEX);
  wave_fmt->fmtFORMAT.wFormatTag = 1;
  wave_fmt->fmtFORMAT.nChannels = static_cast<WORD>(channels);
  wave_fmt->fmtFORMAT.nSamplesPerSec = static_cast<DWORD>(samples_per_second);
  wave_fmt->fmtFORMAT.nBlockAlign = static_cast<WORD>((bits_per_sample / 8) * channels);
  wave_fmt->fmtFORMAT.nAvgBytesPerSec = wave_fmt->fmtFORMAT.nBlockAlign*static_cast<DWORD>(samples_per_second);
  wave_fmt->fmtFORMAT.wBitsPerSample = static_cast<WORD>(bits_per_sample);
  wave_fmt->fmtFORMAT.cbSize = 0;

  wave_data->dataID[0] = 'd'; 
  wave_data->dataID[1] = 'a'; 
  wave_data->dataID[2] = 't'; 
  wave_data->dataID[3] = 'a'; 

  wave_data->dataSIZE = static_cast<DWORD>(data_size);
  signed short max_amp = (1 << 15) - 1;
  for (size_t n = 0 ; n< signal.size() ; ++n)
  {
    signed short val = (signed short)(signal[n] * (double)max_amp);
    *(wave + header_offset + 2*n) = val & 0x00FF;
    *(wave + header_offset + 2*n + 1) = val >> 8;
  }
}

}
}
