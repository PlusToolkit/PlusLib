#pragma once

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

#include "Wave.h"

namespace VibroLib
{
	namespace AudioCard
	{
		class VTK_EXPORT DirectSoundCapture
		{
		public:
			DirectSoundCapture(void);
			~DirectSoundCapture(void);
			PlusStatus Initialize();
			LPDIRECTSOUNDCAPTURE operator->() {return pDirectSoundCapture;}
		private:
			LPDIRECTSOUNDCAPTURE pDirectSoundCapture;
		};
	}
}
