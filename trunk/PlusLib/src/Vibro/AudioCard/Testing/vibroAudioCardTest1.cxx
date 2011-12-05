/*=Plus=header=begin======================================================
  Author: Siavash Khallaghi
  Date: 2010
  Program: Plus
  Copyright (c) University of British Columbia
  Contact: siavashk@ece.ubc.ca
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vibroAudioCardTest1.cxx
  \brief Basic test to check if the vibroAudioCard class is functioning

  A sinusoidal wave is generated and played through the speaker.
  You should be able to hear a humming sound when it is running.
*/

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
  // Create a vector for frequencies for which sinusoid waves will be generated
  std::vector<double> frequencies;															
  for (int n=70; n<=100 ; n+=30) 
  {
    frequencies.push_back((double)n);
  }

  // Generate a wave object containing a sinusoidal signal of multiple frequencies
  const double sampling_frequency=8192.0;
  const int nsamples=20000;
  std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, sampling_frequency, nsamples);	
  VibroLib::AudioCard::Wave Wv;
  Wv.FromSignal(signal);

  // Initialize the DirectSound interface
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
  // Initialize the wave with this properties DSBCAPS_GLOBALFOCUS & DSBCAPS_STICKYFOCUS:
  // Continue to play sound in buffer even if the user switch to another application.
  if (dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )!=PLUS_SUCCESS) 
  {		
    LOG_ERROR( "Unable to initialize buffer." );
    return PLUS_FAIL;
  }

  // Play the wave previously initialized and return error if could not
  // DSBPLAY_LOOPING: Will restart playing once the Audio buffer reachs end. Until stopped.
  HRESULT hr=dsb->Play(0,0,DSBPLAY_LOOPING);
  if (hr!=DS_OK)         
  {															
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

