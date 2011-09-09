#pragma once

#include "SerialCommHelper.h"
#include <vector>
#include <string>
#include <sstream>

#ifdef VIBROLIB_EXPORTS
#define VIBROLIB_API __declspec(dllexport)
#else
#define VIBROLIB_API __declspec(dllimport)
#endif


namespace VibroLib
{
	namespace MCDC
	{
		class VIBROLIB_API MCDCProgrammer
		{
		private:
			CSerialCommHelper CommInterface;
			inline std::string int2str(int value)
			{
				std::stringstream ss;
				ss << value;
				return ss.str();
			}

			inline void WriteToSerial(const char* s)
			{
				CommInterface.Write(s);
				Sleep(15);
			}
		public:
			MCDCProgrammer(void);
			~MCDCProgrammer(void);

			enum
			{
				MAX_PROGRAM_POINTS = 250,
				CLOCK_FREQUENCY = 710,
				MAX_VOLTAGE_AMP = 32767
			};
			
			/*
			Queries all available COM ports to see if they are the MCDC 3006 - S 
			controller. It then connects to the first available MCDC 3006 - S
			controller. Returns false if no controller is available.
			*/
			int FindAndConnectToController();

			/*
			Attempts to connect to the specified port, if the port is unavailable
			or if there is not an MCDC controller at the other end the connection
			is terminated and the function returns false.
			*/
			bool ConnectToComPort(size_t ComID);

			/*
			If we are connected to a controller this function will disconnect.
			*/
			bool Disconnect();

			/*
			Sends the specified signal to the controller, note that a maximum of
			MAX_PROGRAM_POINTS data points may be programmed. Note that the voltage command sent is:

			V = (amplitude / 32767) * Uv

			Where Uv is the supply voltage, thus the amplitudes must be between +/- 32767.
			*/
			bool ProgramVoltMode(const std::vector<int>& signal);

			bool StartProgramVoltMode();

			bool StopProgramVoltMode();
		};
	}
}
