#include "StdAfx.h"
#include "DirectSoundBuffer.h"



namespace VibroLib
{
	namespace AudioCard
	{
		DirectSoundBuffer::DirectSoundBuffer(void)
		{
			pBuffer = NULL;
		}

		DirectSoundBuffer::~DirectSoundBuffer(void)
		{
			if (pBuffer)
				pBuffer->Release();
		}

		HRESULT DirectSoundBuffer::Initialize(DirectSoundInstance* pDSHandle, Wave& signal, DWORD dwFlags, bool primary_buffer)
		{
			if (primary_buffer)
				return InitializePrimaryBuffer(pDSHandle, signal, dwFlags);
			else
				return InitializeSecondaryBuffer(pDSHandle, signal, dwFlags);
		}

		HRESULT DirectSoundBuffer::InitializePrimaryBuffer(DirectSoundInstance* pDSHandle, Wave& signal, DWORD dwFlags)
		{
			HRESULT hr;
			pDSInstance = pDSHandle;

			DSBUFFERDESC dsBuffDesc;
			ZeroMemory(&dsBuffDesc, sizeof (DSBUFFERDESC));
			dsBuffDesc.dwSize = sizeof (DSBUFFERDESC);
			dsBuffDesc.dwFlags = dwFlags | DSBCAPS_PRIMARYBUFFER | DSBCAPS_LOCHARDWARE;
			dsBuffDesc.dwBufferBytes = 0;
			dsBuffDesc.lpwfxFormat = NULL;

			LPDIRECTSOUNDBUFFER lpdsb = NULL;
			if ((hr=(*pDSInstance)->CreateSoundBuffer(&dsBuffDesc, &lpdsb, NULL)) != DS_OK)
			{
				printf("Failed to create primary sound buffer: %d\n", hr);
				return hr; // Wave format not supported?
			}


			pBuffer = lpdsb;

			if ((hr = pBuffer->SetFormat(&signal.DataFormat())) != DS_OK)
				return hr;

			// Check and validate buffer size:
			DSBCAPS dsbcaps; 
			dsbcaps.dwSize = sizeof(DSBCAPS); 
			pBuffer->GetCaps((LPDSBCAPS)&dsbcaps); 
			DWORD BufferSize = dsbcaps.dwBufferBytes;

			// Check that the primary buffer exists in hardware:
			//if (!(dsbcaps.dwFlags & DSBCAPS_LOCHARDWARE))
			//	return DSERR_UNSUPPORTED;

			PBYTE pBuffData;
			DWORD data_length;

			if ((hr=pBuffer->Lock(0, 0, (LPVOID*)&pBuffData, &data_length, NULL, 0, DSBLOCK_ENTIREBUFFER )) != DS_OK)
			{
				printf("Failed to lock sound buffer: %d\n", hr);
				return hr;
			}

			size_t offset = 0;
			while (offset < data_length)
			{
				size_t copy_length = (data_length - offset) < signal.DataLength() ? data_length : signal.DataLength();
				memcpy((void*)(pBuffData + offset), signal.WaveDataStart(), copy_length);
				offset += copy_length;
			}

			if ((hr=pBuffer->Unlock(pBuffData, data_length, NULL, 0)) != DS_OK)
			{
				printf("Failed to unlock sound buffer: %d\n", hr);
				return hr;
			}
			return DS_OK;
		}

		HRESULT DirectSoundBuffer::InitializeSecondaryBuffer(DirectSoundInstance* pDSHandle, Wave& signal, DWORD dwFlags)
		{
			if (dwFlags & DSBCAPS_PRIMARYBUFFER)
				return DSERR_INVALIDPARAM;

			HRESULT hr;
			pDSInstance = pDSHandle;

			DSBUFFERDESC dsBuffDesc;
			ZeroMemory(&dsBuffDesc, sizeof (DSBUFFERDESC));
			dsBuffDesc.dwSize = sizeof (DSBUFFERDESC);
			dsBuffDesc.dwFlags = dwFlags;
			dsBuffDesc.dwBufferBytes = signal.DataLength();
			dsBuffDesc.lpwfxFormat = &signal.DataFormat();

			printf("Wave Size: %d", dsBuffDesc.lpwfxFormat->cbSize);
			LPDIRECTSOUNDBUFFER lpdsb = NULL;
			signal.DataFormat().cbSize = 0;
			if ((hr=(*pDSInstance)->CreateSoundBuffer(&dsBuffDesc, &lpdsb, NULL)) != DS_OK)
			{
				printf("Failed to create sound buffer: %d\n", hr);
				return hr; // Wave format not supported?
			}
			
			pBuffer = lpdsb;

			PBYTE pBuffData;
			DWORD data_length;

			if ((hr=pBuffer->Lock(0, 0, (LPVOID*)&pBuffData, &data_length, NULL, 0, DSBLOCK_ENTIREBUFFER )) != DS_OK)
			{
				printf("Failed to lock sound buffer: %d\n", hr);
				return hr;
			}

			memcpy((void*)pBuffData, signal.WaveDataStart(), data_length);

			if ((hr=pBuffer->Unlock(pBuffData, data_length, NULL, 0)) != DS_OK)
			{
				printf("Failed to unlock sound buffer: %d\n", hr);
				return hr;
			}
			return DS_OK;
		}
	}
}
