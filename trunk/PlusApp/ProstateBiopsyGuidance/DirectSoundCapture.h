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
			bool Initialize(const std::string& _deviceIdentifier);
			LPDIRECTSOUNDCAPTURE operator->() {return pDirectSoundCapture;}
			void SetGuid(LPGUID _guiID);
			const std::string& DeviceIdentifier() {return deviceIdentifier;}

		private:
			LPDIRECTSOUNDCAPTURE pDirectSoundCapture;
			LPGUID guiID;	
			std::string deviceIdentifier;
		};
		
		BOOL CALLBACK CaptureEnumCallBack (LPGUID guid, LPCSTR desc, LPCSTR mod, LPVOID list);
	}
}
