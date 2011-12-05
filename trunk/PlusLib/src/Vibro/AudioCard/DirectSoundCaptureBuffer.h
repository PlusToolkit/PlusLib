/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __DirectSoundCaptureBuffer_h
#define __DirectSoundCaptureBuffer_h

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

#include "Wave.h"
#include "DirectSoundCapture.h"
#include <vector>

namespace VibroLib
{
	namespace AudioCard
	{

    /*!
      \class DirectSoundCaptureBuffer
      \brief Class to store a DirectSound capture buffer object pointer
      \ingroup PlusLibVibro
    */
		class VTK_EXPORT DirectSoundCaptureBuffer
		{
		public:
			DirectSoundCaptureBuffer(void);
			~DirectSoundCaptureBuffer(void);

			/*!
        Initializes a capture buffer using 16 bits per sample, at the
			  specified frequency. The buffer will have a size in samples equal
			  to BufferMaxSamples.
			  \return DS_OK on success
			*/
			PlusStatus Initialize(DirectSoundCapture* pDSC, size_t SampleFrequency, size_t BufferMaxSamples);

			/*! Reads the buffer without pausing the capture */
			PlusStatus GetData(std::vector<signed short>& data);

			/*! Pausing the capture in order to read the entire buffer. Capture is then resumed. */
			PlusStatus GetEntireBuffer(std::vector<signed short>& data);

			/*! 
        Reads the specified duration of data, if the duration
			  is as long as or longer than the buffer length capture
			  is paused.
			*/
			PlusStatus GetData(std::vector<signed short>& data, double duration);

			PlusStatus StartCapture();
			PlusStatus StopCapture();

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

#endif // __DirectSoundCaptureBuffer_h
