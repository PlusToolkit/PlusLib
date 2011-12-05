/*=Plus=header=begin======================================================
  Author: Ian Bell
  Date: 2010
  Program: Plus
  Copyright (c) University of British Columbia
  Modified: Siavash Khallaghi
  Contact: Hani Eskandari, hanie@ece.ubc.ca
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __DirectSoundBuffer_h
#define __DirectSoundBuffer_h

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

#include "Wave.h"
#include "DirectSoundInstance.h"

namespace VibroLib
{
	namespace AudioCard
	{

    /*!
      \class DirectSoundBuffer
      \brief Class to store a DirectSound buffer object pointer
      The class can be used to output an analog signal using a regular
      audio card. The signal can be amplified and converted to mechanical energy
      to serve as an excitation source for vibro-elastography.
      \ingroup PlusLibVibro
    */
    class VTK_EXPORT DirectSoundBuffer
		{
		public:
			DirectSoundBuffer(void);
			~DirectSoundBuffer(void);
			PlusStatus Initialize(DirectSoundInstance* pDSHandle, Wave& signal, DWORD dwFlags = 0, bool primary_buffer = false);
			
			LPDIRECTSOUNDBUFFER operator->() {return pBuffer;}
		private:
			
			PlusStatus InitializeSecondaryBuffer(DirectSoundInstance* pDSHandle, Wave& signal, DWORD dwFlags);
			PlusStatus InitializePrimaryBuffer(DirectSoundInstance* pDSHandle, Wave& signal, DWORD dwFlags = DSBCAPS_PRIMARYBUFFER);

			LPDIRECTSOUNDBUFFER pBuffer;
			DirectSoundInstance* pDSInstance;
		};
	}
}

#endif // __DirectSoundBuffer_h
