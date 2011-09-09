#include "PlusConfigure.h"

#include "DirectSoundCaptureBuffer.h"

namespace VibroLib
{
	namespace AudioCard
	{
		using namespace std;

		DirectSoundCaptureBuffer::DirectSoundCaptureBuffer(void)
			: Capturing(false)	
		{
		}

		DirectSoundCaptureBuffer::~DirectSoundCaptureBuffer(void)
		{
		}

		HRESULT DirectSoundCaptureBuffer::Initialize(DirectSoundCapture* pDSC, size_t SampleFrequency, size_t BufferMaxSamples)
		{
			HRESULT err = DS_OK;
			WAVEFORMATEX wvFormat;

			wvFormat.wFormatTag = 1;
			wvFormat.nChannels = 1;
			wvFormat.nSamplesPerSec = static_cast<DWORD>(SampleFrequency);
			wvFormat.nBlockAlign = 2;
			wvFormat.nAvgBytesPerSec = 2 * static_cast<DWORD>(SampleFrequency);
			wvFormat.wBitsPerSample = 16;
			wvFormat.cbSize = 0;

			DSCBUFFERDESC buffFormat;
			ZeroMemory(&buffFormat, sizeof(DSCBUFFERDESC));
			buffFormat.dwSize = sizeof(DSCBUFFERDESC);
			buffFormat.dwFlags = DSCBCAPS_WAVEMAPPED;
			buffFormat.dwBufferBytes = sizeof(signed short) * (DWORD)BufferMaxSamples;
			buffFormat.dwReserved = NULL;
			buffFormat.lpwfxFormat = &wvFormat;

			LPDIRECTSOUNDCAPTUREBUFFER buffTemp = NULL;
			if ((err = (*pDSC)->CreateCaptureBuffer(&buffFormat, &buffTemp, NULL)) != DS_OK)
				return err;

			pCaptureBuffer = buffTemp;
			BufferLength = BufferMaxSamples;
			SamplingFrequency = SampleFrequency;
			return DS_OK;
		}

		HRESULT DirectSoundCaptureBuffer::GetData(vector<signed short>& data)
		{
			if (pCaptureBuffer == NULL)
				return DSERR_UNINITIALIZED;


			DWORD lock_start, lock_stop;
			HRESULT err;

			if ((err = pCaptureBuffer->GetCurrentPosition(&lock_start, &lock_stop)) != DS_OK)
				return err;

			DWORD lock_length = (DWORD)(lock_stop < lock_start ? ((sizeof(signed short)*BufferLength - lock_start) + lock_stop) : (lock_stop - lock_start));

			DWORD block_one_length, block_two_length;
			BYTE* block_one, *block_two;
			if ((err = pCaptureBuffer->Lock(lock_start, lock_length, (LPVOID*)&block_one, &block_one_length, (LPVOID*)&block_two, &block_two_length, 0)) != DS_OK)
				return err;

			data.clear();
			data.reserve(lock_length / 2);
			for (size_t n = 0 ; n<block_one_length - 1 ; n += 2)
				data.push_back(block_one[n] + 256 * block_one[n + 1]);
			if (block_two_length > 0)
				for (size_t n = 0 ; n<block_two_length - 1 ; n += 2)
					data.push_back(block_two[n] + 256 * block_two[n + 1]);

			if ((err = pCaptureBuffer->Unlock((LPVOID)block_one, block_one_length, (LPVOID)block_two, block_two_length)) != DS_OK)
				return err;

			return DS_OK;
		}

		HRESULT DirectSoundCaptureBuffer::GetEntireBuffer(std::vector<signed short>& data)
		{
			if (pCaptureBuffer == NULL)
				return DSERR_UNINITIALIZED;

			HRESULT err;

			DWORD block_one_length;
			BYTE* block_one;
			StopCapture();
			if ((err = pCaptureBuffer->Lock(0, 0, (LPVOID*)&block_one, &block_one_length, NULL, 0, DSCBLOCK_ENTIREBUFFER)) != DS_OK)
				return err;

			data.clear();
			data.reserve(block_one_length/2);
			for (size_t n = 0 ; n<block_one_length - 1 ; n += 2)
				data.push_back(block_one[n] + 256 * block_one[n + 1]);

			if ((err = pCaptureBuffer->Unlock((LPVOID)block_one, block_one_length, NULL, 0)) != DS_OK)
				return err;

			StartCapture();
			return DS_OK;
		}

		HRESULT DirectSoundCaptureBuffer::GetData(std::vector<signed short>& data, double duration)
		{
			if (pCaptureBuffer == NULL)
				return DSERR_UNINITIALIZED;
			if (duration <= 0)
				return DSERR_INVALIDPARAM;

			DWORD lock_start, lock_stop;
			HRESULT err;

			if ((err = pCaptureBuffer->GetCurrentPosition(&lock_start, &lock_stop)) != DS_OK)
				return err;

			DWORD lock_length = (DWORD)(lock_stop < lock_start ? ((sizeof(signed short)*BufferLength - lock_start) + lock_stop) : (lock_stop - lock_start));

			double max_duration = (double)(lock_length / 2) / (double)SamplingFrequency;

			if (duration >= max_duration)
				return GetEntireBuffer(data);

			std::vector<signed short> temp_data;
			if ((err = GetData(temp_data)) != DS_OK)
				return err;
			if (temp_data.empty())
				return DSERR_UNINITIALIZED;
			data.clear();
			data.resize((size_t)(duration * SamplingFrequency), 0);
			std::copy(temp_data.end() - (size_t)(duration * SamplingFrequency), temp_data.end(), data.begin());
			return DS_OK;
		}

		HRESULT DirectSoundCaptureBuffer::StartCapture()
		{
			if (pCaptureBuffer != NULL)
			{
				HRESULT hr = pCaptureBuffer->Start(DSCBSTART_LOOPING);
				if (hr == DS_OK)
					Capturing =	true;	
				return hr;
			}
			return DSERR_UNINITIALIZED;
		}

		HRESULT DirectSoundCaptureBuffer::StopCapture()
		{
			if (pCaptureBuffer != NULL)
			{
				HRESULT hr = pCaptureBuffer->Stop();
				if (hr == DS_OK)
					Capturing =	false;	
				return hr;
			}
			return DSERR_UNINITIALIZED;
		}
	}
}