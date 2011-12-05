/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "DirectSoundCapture.h"

namespace VibroLib
{
namespace AudioCard
{

//----------------------------------------------------------------------------
DirectSoundCapture::DirectSoundCapture(void)
{
	pDirectSoundCapture = NULL;
}

//----------------------------------------------------------------------------
DirectSoundCapture::~DirectSoundCapture(void)
{
}

//----------------------------------------------------------------------------
PlusStatus DirectSoundCapture::Initialize()
{
	if (::DirectSoundCaptureCreate(NULL, &pDirectSoundCapture, NULL) != DS_OK)
  {
    LOG_ERROR("DirectSoundCaptureCreate failed");
		return PLUS_FAIL;
  }
	return PLUS_SUCCESS;
}

}
}
