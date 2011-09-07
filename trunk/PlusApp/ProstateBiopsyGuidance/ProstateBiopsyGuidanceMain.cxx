#include "ProstateBiopsyGuidanceGUI.h"
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include "VibroLib.h" // vibro lab
// QT includes
#include <qapplication.h>
#include "DirectSoundCaptureBuffer.h"
#include "DirectSoundCapture.h"
#include "DirectSoundCaptureBuffer.h"
#include "DirectSoundInstance.h"
#include "DirectSoundBuffer.h"
#include "dsound.h"
#include "ProstateBiopsyGuidance.h"
#include "MCDCProgrammer.h"
#include "ProstateBiopsyGuidanceGUI.h"
#include "SerialBuffer.h"
#include "SerialCommHelper.h"
#include "SignalGenerator.h"
#include "Timer.h"
#include "Wave.h"

# define LOW_FREQUENCY_VOLUME -900
bool DSBufferSet;
int DSSamplingFrequency = 20000;
VibroLib::AudioCard::DirectSoundBuffer m_DSBuffer;
std::vector<double> VibrationFrequencies;
VibroLib::AudioCard::DirectSoundInstance m_DSInstance;
bool g_isSystemBusy;
void SetCurrentVibrationFrequencies(const std::vector<double>& frequencies)	{ VibrationFrequencies = frequencies; }
double AbsMax(std::vector<double> signal)
	{
		double max = 0.0;
		for (size_t n=0 ; n<signal.size(); ++n)
			if (abs(signal[n]) > max)
				max = std::abs(signal[n]);
		return max;
	}

std::vector<double> GenerateMultiFrequency(std::vector<double> frequencies, double sampling_frequency, int nsamples)
	{
		const static double pi = 3.14159265;
		double T = 1/sampling_frequency;
		std::vector<double> signal(nsamples, 0);
		for (size_t f=0 ; f<frequencies.size() ; ++f)
			for (size_t n=0 ; n<(size_t)nsamples ; ++n)
				signal[n] += sin((double)n*T*2*pi*frequencies[f]);
		double max = AbsMax(signal);
		for (size_t n=0 ; n<(size_t)nsamples ; ++n)
			signal[n] = signal[n]/max;
		return signal;
	}


void SetVibrationFrequency1(const char* frequencies)
{
	std::stringstream ss;
	ss << frequencies;
	std::vector<double> dfrequencies;
	while (!ss.eof())
	{
		try
		{
			double freq = 0.0;
			ss >> freq;
			if (freq < 0 || freq >= 22000)
				throw "Invalid frequency! Must be positive and less than 22 kHz.";
			if (freq == 0.0)
				continue;
			dfrequencies.push_back(freq);
		}
		catch(...)
		{
			// Error handling:
			//printf("Error parsing input frequencies");
			return;
		}		
	}

	if (dfrequencies.empty())
		return;

	SetCurrentVibrationFrequencies(dfrequencies);

	int duration = DSSamplingFrequency * 2;
	std::vector<double> signal = GenerateMultiFrequency(dfrequencies, DSSamplingFrequency, duration);
	VibroLib::AudioCard::Wave wv;
	wv.FromSignal(signal, DSSamplingFrequency, 1);

	HRESULT hr = DS_OK;
//	if ((hr = m_DSBuffer.Initialize(&m_DSInstance, wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS | DSBCAPS_CTRLVOLUME , false )) != DS_OK)
//	{
		//dprintf("Unable to initialize buffer... HRESULT: %d\n", hr);\
		//system("PAUSE");
//		return; // error cannot initialize buffer.
//	}
	DSBufferSet = true;
return;
}
void OnPresetOne()
{
	g_isSystemBusy = true;
	if( DSBufferSet )
		m_DSBuffer->Stop();
//	ipCore.pause();
//	m_currentPreset = 1;

//	if(m_currentMode != IPVE_TRANSFER)
//		swapModes(IPVE_TRANSFER);


	SetVibrationFrequency1("20.0 92.0 110.0");
	m_DSBuffer->SetVolume(LOW_FREQUENCY_VOLUME);

	HRESULT hr;
	if (DSBufferSet)
	{
		if ((hr = m_DSBuffer->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)
		{
			return; // error cannot play buffer.
		}
	}


}
int main( int argc, char** argv )
{
 /*    QApplication app(argc, argv);
     ProstateBiopsyGuidanceGUI dialog;
     return dialog.exec();*/
/*******************************************/
	std::vector<double> signal(2);
	signal.push_back(5);
	signal.push_back(6);
//	VibroLib::GenerateSinusoidRepeatable(1000,2000,0,0);	
//	SetVibrationFrequency("100");
	OnPresetOne();
//	VibroLib::GenerateSinusoid(1000,100000,10);
	cout<<"did it vibrate\n";
	int q;
	cin>>q;
//	VibroLib::AbsMax(signal);//GenerateSinusoid(10,10,10);

	return 0;
}