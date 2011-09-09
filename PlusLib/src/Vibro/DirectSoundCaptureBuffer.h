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
#include "DirectSoundCapture.h"
#include "Timer.h"
#include <vector>

namespace VibroLib
{
	namespace AudioCard
	{
		class VIBROLIB_API DirectSoundCaptureBuffer
		{
		public:
			DirectSoundCaptureBuffer(void);
			~DirectSoundCaptureBuffer(void);
			/* Initializes a capture buffer using 16 bits per sample, at the
			specified frequency. The buffer will have a size in samples equal
			to BufferMaxSamples.
			
			Returns DS_OK on success.
			*/
			HRESULT Initialize(DirectSoundCapture* pDSC, size_t SampleFrequency, size_t BufferMaxSamples);

			/* Reads the buffer without pausing the capture.
			*/
			HRESULT GetData(std::vector<signed short>& data);

			/* Pausing the capture in order to read the entire
			buffer. Capture is then resumed.
			*/
			HRESULT GetEntireBuffer(std::vector<signed short>& data);

			/* Reads the specified duration of data, if the duration
			is as long as or longer than the buffer length capture
			is paused.
			*/
			HRESULT GetData(std::vector<signed short>& data, double duration);

			HRESULT StartCapture();
			HRESULT StopCapture();

			const size_t& Size() const {return BufferLength;}
			const size_t& Frequency() const {return SamplingFrequency;}
			const bool& Running() const {return Capturing;}

		private:
			LPDIRECTSOUNDCAPTUREBUFFER pCaptureBuffer;
			size_t BufferLength;
			size_t SamplingFrequency;
			bool Capturing;
		};
	}
}
