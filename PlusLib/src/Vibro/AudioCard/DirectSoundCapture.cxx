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

		PlusStatus DirectSoundCapture::Initialize()
		{
			if (::DirectSoundCaptureCreate(NULL, &pDirectSoundCapture, NULL) != DS_OK)
				return PLUS_FAIL;
			return PLUS_SUCCESS;
		}
	}
}
