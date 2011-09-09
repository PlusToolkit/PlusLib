#include "PlusConfigure.h"
#include "DirectSoundInstance.h"


namespace VibroLib
{
	namespace AudioCard
	{
		DirectSoundInstance::DirectSoundInstance(void)
		{
			pDirectSound = NULL;
		}

		DirectSoundInstance::~DirectSoundInstance(void)
		{
		}

		bool DirectSoundInstance::Initialize()
		{
			if (::DirectSoundCreate(NULL, &pDirectSound, NULL) != DS_OK)
				return false;
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
	}
}
