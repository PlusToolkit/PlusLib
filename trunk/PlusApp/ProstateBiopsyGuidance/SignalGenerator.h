#pragma once

#ifdef VIBROLIB_EXPORTS
#define VIBROLIB_API __declspec(dllexport)
#else
#define VIBROLIB_API __declspec(dllimport)
#endif

#include <vector>
#include <math.h>

namespace VibroLib
{
	const static double pi = 3.14159265;
	VIBROLIB_API std::vector<int> GenerateSinusoid(double signal_frequency, double sampling_frequency, int amplitude, int offset, int nsamples);
	VIBROLIB_API std::vector<double> GenerateSinusoid(double signal_frequency, double sampling_frequency, int nsamples);
	VIBROLIB_API std::vector<int> GenerateSinusoidRepeatable(double signal_frequency, double sampling_frequency, int amplitude, int offset);
	VIBROLIB_API std::vector<int> GenerateMultiFrequencyClamped(std::vector<double> frequencies, double sampling_frequency, int amplitude, int offset, int nsamples);
	VIBROLIB_API std::vector<double> GenerateMultiFrequencyClamped(std::vector<double> frequencies, double sampling_frequency, int nsamples);
	VIBROLIB_API std::vector<double> GenerateMultiFrequency(std::vector<double> frequencies, double sampling_frequency, int nsamples);


	inline double Norm(std::vector<double> signal)
	{
		double norm = 0.0;
		for (size_t n=0 ; n<signal.size(); ++n)
			norm += signal[n]*signal[n];
		return sqrt(norm);
	}

	inline double AbsMax(std::vector<double> signal)
	{
		double max = 0.0;
		for (size_t n=0 ; n<signal.size(); ++n)
			if (abs(signal[n]) > max)
				max = abs(signal[n]);
		return max;
	}
}