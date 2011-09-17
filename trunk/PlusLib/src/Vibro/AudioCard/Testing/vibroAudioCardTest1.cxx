// Basic test to check if the vibroAudioCard class is functioning.
// A sinusoidal wave is generated and played through the speaker.
// You should be able to hear a humming sound when it is running

#include "PlusConfigure.h"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>

#include "Wave.h"
#include "DirectSoundInstance.h"
#include "DirectSoundBuffer.h"
#include "DirectSoundCapture.h"
#include "DirectSoundCaptureBuffer.h"
#include "SignalGenerator.h"
#include <fstream>

PlusStatus RunAudio()
{
		PlusStatus status;
		
		HRESULT hr;

		std::vector<double> frequencies;															// Creat a vector for frequencies which will be generated
		
		for (int n=70 ; n<=100 ; n+=30) frequencies.push_back((double)n);							// put frequency value in frequencies and this can be changed to generate frequency needed
		
		std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, 8192.0, 20000);	// Generate multiple frequencies: frequencies: Input frequency you defined, double sampling_frequency, int nsamples.
		
		VibroLib::AudioCard::Wave Wv;

		Wv.FromSignal(signal);

		VibroLib::AudioCard::DirectSoundInstance dsi;

		if (dsi.Initialize()!=PLUS_SUCCESS)
		{
			return PLUS_FAIL;
		}

		// Sets the application to the priority level.
		// Applications with this cooperative level can call the DirectSoundBuffer.setFormat 
		// and DirectSound.compact methods (http://timgolden.me.uk/pywin32-docs/directsound_DSSCL_PRIORITY.html).
		HWND consoleWindow=GetConsoleWindow();
		if (consoleWindow==NULL)
		{
			LOG_ERROR( "Unable to get console window." );
			return PLUS_FAIL;
		}
		if (dsi.SetPriority(consoleWindow, DSSCL_PRIORITY)!=PLUS_SUCCESS)  
		{
			return PLUS_FAIL;
		}

		VibroLib::AudioCard::DirectSoundBuffer dsb;

		if ((status = dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )) != PLUS_SUCCESS) // initialize the wave with this properties DSBCAPS_GLOBALFOCUS & DSBCAPS_STICKYFOCUS : Continue to play sound in buffer even if the user switch to another application.
		{		
			LOG_ERROR( "Unable to initialize buffer." );
			return PLUS_FAIL;
		}
		
		if ((hr = dsb->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)         // Play the wave previously initialized and return error if could not
		{															// DSBPLAY_LOOPING: Will restart playing once the Audio buffer reachs end. Until stopped
				
			LOG_ERROR( "Failed to play buffer... HRESULT: " << hr );
			return PLUS_FAIL;
		}

		LOG_INFO("Vibrating for 6 seconds, you should hear a humming sound.");
		Sleep(6000);
		dsb->Stop(); 
		return PLUS_SUCCESS;
}

int main(int argc, CHAR* argv[])
{
	if ( RunAudio()!=PLUS_SUCCESS ) 
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

