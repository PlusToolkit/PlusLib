/*=Plus=header=begin======================================================
  Author: Ian Bell
  Date: 2010
  Program: Plus
  Copyright (c) University of British Columbia
  Modified: Siavash Khallaghi
  Contact: Hani Eskandari, hanie@ece.ubc.ca
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "SignalGenerator.h"

#include <fstream>
namespace VibroLib
{

using namespace std;

//----------------------------------------------------------------------------
VTK_EXPORT vector<int> GenerateSinusoid(double signal_frequency, double sampling_frequency, int amplitude, int offset, int nsamples)
{
	double T = 1/sampling_frequency;
	vector<int> signal;
	for (int n=0 ; n<nsamples ; ++n)
		signal.push_back((int)((double)offset + (double)amplitude * sin((double)n*T*2*pi*signal_frequency)));
	return signal;
}

//----------------------------------------------------------------------------
VTK_EXPORT std::vector<double> GenerateSinusoid(double signal_frequency, double sampling_frequency, int nsamples)
{
	double T = 1/sampling_frequency;
	vector<double> signal;
	for (int n=0 ; n<nsamples ; ++n)
		signal.push_back(sin((double)n*T*2*pi*signal_frequency));
	return signal;
}

//----------------------------------------------------------------------------
VTK_EXPORT std::vector<int> GenerateSinusoidRepeatable(double signal_frequency, double sampling_frequency, int amplitude, int offset)
{
	return GenerateSinusoid(signal_frequency, sampling_frequency, amplitude, offset, (int)ceil(sampling_frequency/signal_frequency) - 1);
}

//----------------------------------------------------------------------------
VTK_EXPORT vector<int> GenerateMultiFrequencyClamped(vector<double> frequencies, double sampling_frequency, int amplitude, int offset, int nsamples)
{
	vector<double> signal = GenerateMultiFrequencyClamped(frequencies, sampling_frequency, nsamples);
	vector<int> sig_int(nsamples,0);
	for (size_t n=0 ; n<(size_t)nsamples ; ++n)
		sig_int[n] = (int)((double)offset + (double)amplitude*signal[n]);
	return sig_int;
}

//----------------------------------------------------------------------------
VTK_EXPORT std::vector<double> GenerateMultiFrequencyClamped(std::vector<double> frequencies, double sampling_frequency, int nsamples)
{
	double T = 1/sampling_frequency;
	vector<double> signal(nsamples, 0);
	for (size_t f=0 ; f<frequencies.size() ; ++f)
		for (size_t n=0 ; n<(size_t)nsamples ; ++n)
			signal[n] += sin((double)n*T*2*pi*frequencies[f]);
	for (size_t n=0 ; n<(size_t)nsamples ; ++n)
		signal[n] = (signal[n]*((-0.5*cos((double)n*T*2*pi*(1/((double)nsamples*T))) + 0.5)));
	double max = AbsMax(signal);
	for (size_t n=0 ; n<(size_t)nsamples ; ++n)
		signal[n] = signal[n]/max;
	return signal;
}

//----------------------------------------------------------------------------
VTK_EXPORT std::vector<double> GenerateMultiFrequency(std::vector<double> frequencies, double sampling_frequency, int nsamples)
{
	double T = 1/sampling_frequency;
	vector<double> signal(nsamples, 0);
	for (size_t f=0 ; f<frequencies.size() ; ++f)
		for (size_t n=0 ; n<(size_t)nsamples ; ++n)
			signal[n] += sin((double)n*T*2*pi*frequencies[f]);
	double max = AbsMax(signal);
	for (size_t n=0 ; n<(size_t)nsamples ; ++n)
		signal[n] = signal[n]/max;
	return signal;
}

}