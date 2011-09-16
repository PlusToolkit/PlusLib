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

		PlusStatus DirectSoundInstance::Initialize()
		{
			if (::DirectSoundCreate(NULL, &pDirectSound, NULL) != DS_OK)
				return PLUS_FAIL;
			return PLUS_SUCCESS;		
		}

		PlusStatus DirectSoundInstance::SetPriority(HWND hWnd, DWORD priority)
		{
			HRESULT hr;
			if (!pDirectSound)
				return PLUS_FAIL;
			if ((hr = pDirectSound->SetCooperativeLevel(hWnd, priority)) != DS_OK)
				return PLUS_FAIL;
			return PLUS_SUCCESS;
		}
	}
}
