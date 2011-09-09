#include "PlusConfigure.h"

#include "DirectSoundCapture.h"

namespace VibroLib
{
	namespace AudioCard
	{
		DirectSoundCapture::DirectSoundCapture(void)
		{
			pDirectSoundCapture = NULL;
		}

		DirectSoundCapture::~DirectSoundCapture(void)
		{
		}

		bool DirectSoundCapture::Initialize()
		{
			if (::DirectSoundCaptureCreate(NULL, &pDirectSoundCapture, NULL) != DS_OK)
				return false;
			return true;
		}
	}
}
