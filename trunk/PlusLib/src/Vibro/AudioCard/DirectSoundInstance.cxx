/*=Plus=header=begin======================================================
  Author: Ian Bell
  Date: 2010
  Program: Plus
  Copyright (c) University of British Columbia
  Modified: Siavash Khallaghi
  Contact: Hani Eskandari, hanie@ece.ubc.ca
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "DirectSoundInstance.h"


namespace VibroLib
{
namespace AudioCard
{

//----------------------------------------------------------------------------
DirectSoundInstance::DirectSoundInstance(void)
{
	pDirectSound = NULL;
}

//----------------------------------------------------------------------------
DirectSoundInstance::~DirectSoundInstance(void)
{
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundInstance::Initialize()
{
	HRESULT hr=::DirectSoundCreate(NULL, &pDirectSound, NULL);
	if ( hr != DS_OK)
	{
		LOG_ERROR("Failed to initialize DirectSound, error: "<<hr);
		return PLUS_FAIL;
	}
	if (pDirectSound==NULL)
	{
		LOG_ERROR("Invalid DirectSound pointer");
		return PLUS_FAIL;
	}
	return PLUS_SUCCESS;		
}

//----------------------------------------------------------------------------
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
