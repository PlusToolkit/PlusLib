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
			
			if (pDirectSound==NULL)
			{
				LOG_ERROR("DirectSoundInstance::SetPriority failed because pDirectSound is invalid");
				return PLUS_FAIL;
			}
			HRESULT hr=pDirectSound->SetCooperativeLevel(hWnd, priority);
			if (hr != DS_OK)
			{
				LOG_ERROR( "DirectSoundInstance::SetPriority failed because failed to set cooperative level, error: " << hr);
				return PLUS_FAIL;
			}
			return PLUS_SUCCESS;
		}
	}
}
