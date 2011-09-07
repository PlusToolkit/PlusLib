#pragma once

#include <windows.h>
#include <string>
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
			bool Initialize(const std::string& _deviceIdentifier);
			HRESULT SetPriority(HWND hWnd, DWORD priority = DSSCL_PRIORITY);
			LPDIRECTSOUND operator->() {return pDirectSound;}
			void SetGuid(LPGUID _guiID);
			const std::string& DeviceIdentifier() {return deviceIdentifier;}
			
		private:
			LPDIRECTSOUND pDirectSound;
			LPGUID guiID;	
			std::string deviceIdentifier;
		};

		BOOL CALLBACK EnumCallBack (LPGUID guid, LPCSTR desc, LPCSTR mod, LPVOID list);
	}
}
