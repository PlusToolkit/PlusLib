// LibTest.cpp : Defines the entry point for the console application.
//

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
//#include "MCDCProgrammer.h"
#include "SignalGenerator.h"
#include <fstream>
using namespace std;

/*void RunMCDC()
{
	VibroLib::MCDC::MCDCProgrammer mcdc;
	int comid;
	if ((comid = mcdc.FindAndConnectToController()) >= 0)
		printf("Succesfully connected to MCDC controller at: COM%d\n", comid);
	else
	{
		printf("Failed to find comm port...\n");
		system("PAUSE");
	}

	std::vector<double> frequencies;
	for (int n=90 ; n<=120 ; n+=10)
		frequencies.push_back((double)n);
	//std::vector<int> signal = VibroLib::GenerateMultiFrequencyClamped(frequencies, 710.0, 2500, 0, 100);
	std::vector<int> signal = VibroLib::GenerateSinusoidRepeatable(23.667, 710.0, 2000, 0);


	std::ofstream os;
	os.open("C:\\gen_progs\\VibroApp\\IanWork\\clamped_test.txt");
	for (size_t n=0 ; n<signal.size() ; ++n)
		os << signal[n] << "\n";
	os.close();
	printf("Programming...\n");
	mcdc.ProgramVoltMode(signal);
	printf("Running...\n");
	mcdc.StartProgramVoltMode();
	system("PAUSE");
	mcdc.StopProgramVoltMode();
}*/

void RunAudio()
{
	try{
		HRESULT hr;

		size_t samples_per_sec = 8192;
		size_t bits_per_sample = 8;

		std::vector<double> frequencies;
		for (int n=70 ; n<=100 ; n+=30)
			frequencies.push_back((double)n);
		std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, 8192.0, 20000);
		VibroLib::AudioCard::Wave Wv;
		//Wv.FromData(sig, signal.size(), bits_per_sample, samples_per_sec);
		Wv.FromSignal(signal);
		VibroLib::AudioCard::DirectSoundInstance dsi;
		dsi.Initialize();

		if ((hr=dsi.SetPriority(GetConsoleWindow(), DSSCL_PRIORITY)) != DS_OK)
		{
			printf("Unable to set cooperative level: HRESULT: %d\n", hr);
			system("PAUSE");
		}

		VibroLib::AudioCard::DirectSoundBuffer dsb;
		if ((hr = dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )) != DS_OK)
		{
			printf("Unable to initialize buffer... HRESULT: %d\n", hr);
			system("PAUSE");
		}
		
		if ((hr = dsb->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)
			std::cout << "Failed to play buffer... HRESULT: " << hr << "\n";
		else
		{
			printf("\nRunning...\n");
//			system("PAUSE");
//			ifstream myfile;
//			myfile.open("duration.txt");
//			int duration;
//			myfile >> duration;
//			myfile.close();
//			cout<<duration<<endl;
			Sleep(6000);
			dsb->Stop();
		}
	}
	catch(...)
	{
	}

	

}

int main(int argc, CHAR* argv[])
{
	RunAudio();
	return 0;
}

