#include "StdAfx.h"
#include "DirectSoundCapture.h"

namespace VibroLib
{
	namespace AudioCard
	{
		BOOL CALLBACK CaptureEnumCallBack (LPGUID guid, LPCSTR desc, LPCSTR mod, LPVOID list)
		{
			DirectSoundCapture* pCapture = (DirectSoundCapture*)list;
			std::string description(desc);
			if (description.find(pCapture->DeviceIdentifier()) != std::string::npos)
			{
				pCapture->SetGuid(guid);
				return FALSE;
			}
			return TRUE;
		}

		DirectSoundCapture::DirectSoundCapture(void)
		{
			guiID = 0;
			pDirectSoundCapture = NULL;
		}

		DirectSoundCapture::~DirectSoundCapture(void)
		{
		}

		bool DirectSoundCapture::Initialize(const std::string& _deviceIdentifier)
		{
			deviceIdentifier = _deviceIdentifier;
			if (deviceIdentifier.compare("") == 0)
			{
				if (::DirectSoundCaptureCreate(NULL, &pDirectSoundCapture, NULL) != DS_OK)
					return false;
			}
			else
			{
				if (FAILED(DirectSoundCaptureEnumerate(CaptureEnumCallBack, this)))
					return false;
				if (::DirectSoundCaptureCreate(guiID, &pDirectSoundCapture, NULL) != DS_OK)
					return false;
			}

			return true;	
		}

		void DirectSoundCapture::SetGuid(LPGUID _guiID)
		{
			guiID = _guiID;
		}
	}
}
