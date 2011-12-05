/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __DirectSoundInstance_h
#define __DirectSoundInstance_h

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

namespace VibroLib
{
	namespace AudioCard
	{

    /*!
      \class DirectSoundInstance
      \brief Class to store a DirectSound object pointer
      \ingroup PlusLibVibro
    */
		class VTK_EXPORT DirectSoundInstance
		{
		public:
			DirectSoundInstance(void);
			~DirectSoundInstance(void);
			PlusStatus Initialize();
			PlusStatus SetPriority(HWND hWnd, DWORD priority = DSSCL_PRIORITY);
			LPDIRECTSOUND operator->() {return pDirectSound;}

		private:
			LPDIRECTSOUND pDirectSound;
		};
	}
}

#endif // __DirectSoundInstance_h
