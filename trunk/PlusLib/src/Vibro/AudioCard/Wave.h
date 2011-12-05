/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __Wave_h
#define __Wave_h

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

#include <vector>

namespace VibroLib
{
  namespace AudioCard
  {

    /*!
      \class Wave
      \brief Class to represent an audio wave, ie. a .wav file.

      In windows wave files are represented as simle BYTE arrays. However, they have a
      header related to the format of the audio wave, frequency, bits per
      sample, which is taken care of by this class.

      \ingroup PlusLibVibro
    */
    class VTK_EXPORT Wave
    {
    public:
      Wave(void);
      ~Wave(void);
      void FromData(unsigned char* data, size_t data_size, size_t bits_per_sample = 8, size_t samples_per_second = 8192, size_t channels = 1);
      void FromSignal(std::vector<double> signal, size_t samples_per_second = 8192, size_t channels = 1);
      DWORD DataLength() const { return wave_data->dataSIZE; }
      WAVEFORMATEX& DataFormat() const { return wave_fmt->fmtFORMAT; }
      BYTE* WaveDataStart() const {return wave + sizeof(WaveRIFF) + sizeof(WaveFMT) + sizeof(WaveDATA);}

    private:
      struct WaveRIFF 
      {
        char riffID[4];
        DWORD riffSIZE;
        char riffFORMAT[4];
      };
      struct WaveFMT
      {
        char fmtID[4];
        DWORD fmtSIZE;
        WAVEFORMATEX fmtFORMAT;
      };
      struct WaveDATA 
      {
        char dataID[4];
        DWORD dataSIZE;
      };

      BYTE* wave;
      WaveRIFF* wave_riff;
      WaveFMT* wave_fmt;
      WaveDATA* wave_data;
    };

  } // AudioCard
} // VibroLib

#endif // __Wave_h
