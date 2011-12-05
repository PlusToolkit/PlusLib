/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "DirectSoundCaptureBuffer.h"

namespace VibroLib
{
namespace AudioCard
{
using namespace std;

//----------------------------------------------------------------------------
DirectSoundCaptureBuffer::DirectSoundCaptureBuffer(void)
	: Capturing(false)	
{
}

//----------------------------------------------------------------------------
DirectSoundCaptureBuffer::~DirectSoundCaptureBuffer(void)
{
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCaptureBuffer::Initialize(DirectSoundCapture* pDSC, size_t SampleFrequency, size_t BufferMaxSamples)
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
		return PLUS_FAIL;

	pCaptureBuffer = buffTemp;
	BufferLength = BufferMaxSamples;
	SamplingFrequency = SampleFrequency;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCaptureBuffer::GetData(vector<signed short>& data)
{
	if (pCaptureBuffer == NULL)
		return PLUS_FAIL;


	DWORD lock_start, lock_stop;
	HRESULT err;

	if ((err = pCaptureBuffer->GetCurrentPosition(&lock_start, &lock_stop)) != DS_OK)
		return PLUS_FAIL;

	DWORD lock_length = (DWORD)(lock_stop < lock_start ? ((sizeof(signed short)*BufferLength - lock_start) + lock_stop) : (lock_stop - lock_start));

	DWORD block_one_length, block_two_length;
	BYTE* block_one, *block_two;
	if ((err = pCaptureBuffer->Lock(lock_start, lock_length, (LPVOID*)&block_one, &block_one_length, (LPVOID*)&block_two, &block_two_length, 0)) != DS_OK)
		return PLUS_FAIL;

	data.clear();
	data.reserve(lock_length / 2);
	for (size_t n = 0 ; n<block_one_length - 1 ; n += 2)
		data.push_back(block_one[n] + 256 * block_one[n + 1]);
	if (block_two_length > 0)
		for (size_t n = 0 ; n<block_two_length - 1 ; n += 2)
			data.push_back(block_two[n] + 256 * block_two[n + 1]);

	if ((err = pCaptureBuffer->Unlock((LPVOID)block_one, block_one_length, (LPVOID)block_two, block_two_length)) != DS_OK)
		return PLUS_FAIL;

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCaptureBuffer::GetEntireBuffer(std::vector<signed short>& data)
{
	if (pCaptureBuffer == NULL)
		return PLUS_FAIL;

	HRESULT err;

	DWORD block_one_length;
	BYTE* block_one;
	StopCapture();
	if ((err = pCaptureBuffer->Lock(0, 0, (LPVOID*)&block_one, &block_one_length, NULL, 0, DSCBLOCK_ENTIREBUFFER)) != DS_OK)
		return PLUS_FAIL;

	data.clear();
	data.reserve(block_one_length/2);
	for (size_t n = 0 ; n<block_one_length - 1 ; n += 2)
		data.push_back(block_one[n] + 256 * block_one[n + 1]);

	if ((err = pCaptureBuffer->Unlock((LPVOID)block_one, block_one_length, NULL, 0)) != DS_OK)
		return PLUS_FAIL;

	StartCapture();
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCaptureBuffer::GetData(std::vector<signed short>& data, double duration)
{
	if (pCaptureBuffer == NULL)
		return PLUS_FAIL;
	if (duration <= 0)
		return PLUS_FAIL;

	DWORD lock_start, lock_stop;
	HRESULT err;

	if ((err = pCaptureBuffer->GetCurrentPosition(&lock_start, &lock_stop)) != DS_OK)
		return PLUS_FAIL;

	DWORD lock_length = (DWORD)(lock_stop < lock_start ? ((sizeof(signed short)*BufferLength - lock_start) + lock_stop) : (lock_stop - lock_start));

	double max_duration = (double)(lock_length / 2) / (double)SamplingFrequency;

	if (duration >= max_duration)
		return GetEntireBuffer(data);

	std::vector<signed short> temp_data;
	if ((err = GetData(temp_data)) != DS_OK)
		return PLUS_FAIL;
	if (temp_data.empty())
		return PLUS_FAIL;
	data.clear();
	data.resize((size_t)(duration * SamplingFrequency), 0);
	std::copy(temp_data.end() - (size_t)(duration * SamplingFrequency), temp_data.end(), data.begin());
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCaptureBuffer::StartCapture()
{
	if (pCaptureBuffer != NULL)
	{
		HRESULT hr = pCaptureBuffer->Start(DSCBSTART_LOOPING);
		if (hr == DS_OK)
			Capturing =	true;	
		return PLUS_SUCCESS;
	}
	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCaptureBuffer::StopCapture()
{
	if (pCaptureBuffer != NULL)
	{
		HRESULT hr = pCaptureBuffer->Stop();
		if (hr == DS_OK)
			Capturing =	false;	
		return PLUS_SUCCESS;
	}
	return PLUS_FAIL;
}

}
}