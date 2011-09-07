#include "StdAfx.h"
#include "DirectSoundInstance.h"


namespace VibroLib
{
	namespace AudioCard
	{
		BOOL CALLBACK EnumCallBack (LPGUID guid, LPCSTR desc, LPCSTR mod, LPVOID list)
		{
			DirectSoundInstance* pInstance = (DirectSoundInstance*)list;
			std::string description(desc);
			if (description.find(pInstance->DeviceIdentifier()) != std::string::npos)
			{
				pInstance->SetGuid(guid);
				return FALSE;
			}
			return TRUE;
		}

		DirectSoundInstance::DirectSoundInstance(void)
		{
			guiID = 0;
			pDirectSound = NULL;
		}

		DirectSoundInstance::~DirectSoundInstance(void)
		{
		}

		bool DirectSoundInstance::Initialize(const std::string& _deviceIdentifier)
		{
			deviceIdentifier = _deviceIdentifier;
			if (deviceIdentifier.compare("") == 0)
			{
				if (::DirectSoundCreate(NULL, &pDirectSound, NULL) != DS_OK)
					return false;
			}
			else
			{
				if (FAILED(DirectSoundEnumerate(EnumCallBack, this)))
					return false;
				if (::DirectSoundCreate(guiID, &pDirectSound, NULL) != DS_OK)
					return false;
			}

			return true;		
		}

		HRESULT DirectSoundInstance::SetPriority(HWND hWnd, DWORD priority)
		{
			HRESULT hr;
			if (!pDirectSound)
				return DSERR_UNINITIALIZED;
			if ((hr = pDirectSound->SetCooperativeLevel(hWnd, priority)) != DS_OK)
				return hr;
			return DS_OK;
		}

		void DirectSoundInstance::SetGuid(LPGUID _guiID)
		{
			guiID = _guiID;
		}
	}
}
