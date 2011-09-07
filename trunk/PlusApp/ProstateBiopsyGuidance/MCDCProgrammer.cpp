#include "StdAfx.h"
#include "MCDCProgrammer.h"


namespace VibroLib
{
	namespace MCDC
	{
		using namespace std;

		MCDCProgrammer::MCDCProgrammer(void)
		{
		}

		MCDCProgrammer::~MCDCProgrammer(void)
		{
			if (CommInterface.IsConnection())
				Disconnect();
		}

		int MCDCProgrammer::FindAndConnectToController()
		{
			size_t buf_size = 100000;
			char* buf = new char[buf_size];
			size_t len = (size_t)::QueryDosDevice(NULL, buf, (DWORD)buf_size);

			for (size_t n = 0 ; n<len ; ++n)
			{
				string s;
				size_t st = n;
				while (buf[n] != 0)
					++n;
				s.append(buf + st, buf + n);
				if (s.find("com") != string::npos || s.find("COM") != string::npos)
				{
					if (s.size() >= 4)
					{
						stringstream ss;
						ss << s.substr(3, s.size() - 3);
						try
						{
							int value;
							ss >> value;
							if (ConnectToComPort(value))
								return value;
						}
						catch (...)
						{
						}
					}
				}
			}

			delete [] buf;
			return -1;
		}

		bool MCDCProgrammer::ConnectToComPort(size_t ComID)
		{
			Disconnect();
			stringstream ss;
			ss << "COM" << ComID;
			if (CommInterface.Init(ss.str(), 115200, 0, 1, 8) != S_OK)
				return false;
			CommInterface.Purge();
			CommInterface.Start();
			WriteToSerial("EN\n");
			WriteToSerial("CST\n");
			string config;
			if (CommInterface.Read_N(config, 1, 200) != S_OK)
			{
				// We are not connected to an MCDC controller.
				Disconnect();
				return false;
			}
			WriteToSerial("DI\n");
			return true;
		}

		bool MCDCProgrammer::Disconnect()
		{
			if (CommInterface.IsConnection())
				CommInterface.UnInit();
			return true;
		}

		bool MCDCProgrammer::ProgramVoltMode(const std::vector<int>& signal)
		{
			if (!CommInterface.IsConnection())
				return false;

			WriteToSerial("PROGSEQ\n");
			WriteToSerial("A1\n");
			WriteToSerial("JMP1\n");
			WriteToSerial("A2\n");
			WriteToSerial("JPT2\n");
			WriteToSerial("RET\n");
			WriteToSerial("A3\n");
			WriteToSerial("RET\n");

			int points_to_write = MAX_PROGRAM_POINTS < (int)signal.size() ? MAX_PROGRAM_POINTS : (int)signal.size();
			for (int n=0 ; n<points_to_write ; ++n)
			{
				WriteToSerial(("A" + int2str(4 + 2*n) + "\n").c_str());
				WriteToSerial("CALL2\n");
				WriteToSerial(("U" + int2str(signal[n]) + "\n").c_str());
				WriteToSerial(("A" + int2str(4 + 2*n + 1) + "\n").c_str());
				if (n < points_to_write - 1)
				{
					WriteToSerial(("JPT" + int2str(4 + 2*n + 2) + "\n").c_str());
					WriteToSerial(("JMP" + int2str(4 + 2*n + 1) + "\n").c_str());
				}
			}

			WriteToSerial("JMP4\n");
			WriteToSerial("END\n");

			return true;
		}

		bool MCDCProgrammer::StartProgramVoltMode()
		{
			if (!CommInterface.IsConnection())
				return false;
			WriteToSerial("EN\n");
			WriteToSerial("ENPROG\n");
			WriteToSerial(("LCC"+ int2str(1500) + "\n").c_str());
			WriteToSerial(("LPC" + int2str(1500) + "\n").c_str());
			WriteToSerial("VOLTMOD\n");
			WriteToSerial("JMP4\n");			
			return true;
		}

		bool MCDCProgrammer::StopProgramVoltMode()
		{
			if (!CommInterface.IsConnection())
				return false;
			WriteToSerial("JMP1\n");
			WriteToSerial("U0\n");
			WriteToSerial("DIPROG\n");
			WriteToSerial("DI\n");
			return true;
		}
	}
}
