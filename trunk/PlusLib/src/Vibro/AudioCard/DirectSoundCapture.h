/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __DirectSoundCapture_h
#define __DirectSoundCapture_h

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"

#include "Wave.h"

namespace VibroLib
{
	namespace AudioCard
	{

    /*!
      \class DirectSoundCapture
      \brief Class to store a DirectSound capture object pointer
      \ingroup PlusLibVibro
    */
    class VTK_EXPORT DirectSoundCapture
		{
		public:
			DirectSoundCapture(void);
			~DirectSoundCapture(void);
			PlusStatus Initialize();
			LPDIRECTSOUNDCAPTURE operator->() {return pDirectSoundCapture;}
		private:
			LPDIRECTSOUNDCAPTURE pDirectSoundCapture;
		};
	}
}

#endif // __DirectSoundCapture_h
