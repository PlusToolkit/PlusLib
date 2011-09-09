#pragma once

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

#ifdef VIBROLIB_EXPORTS
#define VIBROLIB_API __declspec(dllexport)
#else
#define VIBROLIB_API __declspec(dllimport)
#endif

#include "Wave.h"

namespace VibroLib
{
	namespace AudioCard
	{
		class VIBROLIB_API DirectSoundCapture
		{
		public:
			DirectSoundCapture(void);
			~DirectSoundCapture(void);
			bool Initialize();
			LPDIRECTSOUNDCAPTURE operator->() {return pDirectSoundCapture;}
		private:
			LPDIRECTSOUNDCAPTURE pDirectSoundCapture;
		};
	}
}
