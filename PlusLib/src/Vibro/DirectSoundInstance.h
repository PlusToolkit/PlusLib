#pragma once

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

namespace VibroLib
{
	namespace AudioCard
	{
		class VTK_EXPORT DirectSoundInstance
		{
		public:
			DirectSoundInstance(void);
			~DirectSoundInstance(void);
			bool Initialize();
			HRESULT SetPriority(HWND hWnd, DWORD priority = DSSCL_PRIORITY);
			LPDIRECTSOUND operator->() {return pDirectSound;}

		private:
			LPDIRECTSOUND pDirectSound;
		};
	}
}
