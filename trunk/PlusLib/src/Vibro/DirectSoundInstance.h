#pragma once

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"


#ifdef VIBROLIB_EXPORTS
#define VIBROLIB_API __declspec(dllexport)
#else
#define VIBROLIB_API __declspec(dllimport)
#endif

namespace VibroLib
{
	namespace AudioCard
	{
		class VIBROLIB_API DirectSoundInstance
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
