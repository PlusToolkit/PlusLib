// A2HelloWorld.cpp : Defines the entry point for the console application.
//

#include <conio.h>
#include "stdio.h"
#include "windows.h"
#include "..\Common\SEIDrv32.h"


int main(int argc, char* argv[])
{
	long lResult = 0;
	long lCOMPort = 0;
	long lDevices = 0;
	long lModel = 0;
	long lSerialNumber = 0;
	long lVersion = 0;
	long lAddress = 0;
	long lPosition = 0;

	// SEI Initialization.
    // Start the SEI Server Program, and look for devices on the SEI bus
    // the zero means to look on all com ports, and the AUTOASSIGN means
    // that if there are address conflicts on the SEI bus, the device
    // addresses will automatically be reassigned so there are no conflicts
    // Initialization.

	printf("A2 Hello World!\n");
	
	lResult = InitializeSEI(lCOMPort, REINITIALIZE | AUTOASSIGN);

	if (lResult == 0)
	{
		lCOMPort =  GetCommPort();
		printf("Initialized SEI bus on COM port %d.\n", lCOMPort);
		
		lDevices = GetNumberOfDevices();
		
		printf("Found %d devices.\n", lDevices);

		if ( lDevices > 0)
		{
			for (int iDeviceNum = 0; iDeviceNum < lDevices; iDeviceNum++)
			{
				// This demo assumes that there is one A2 device attached to the SEI bus.
				lResult = GetDeviceInfo(iDeviceNum, &lModel, &lSerialNumber, &lVersion, &lAddress);
								
				if (lResult == 0)
				{
					printf("Model = %d, Serial Number = %d, Version = %d, Address = %d\n", lModel, lSerialNumber, lVersion, lAddress);
					if (lModel == 2)
					{
						lResult = A2GetPosition(lAddress, &lPosition);
						printf("\tPosition = %d\n", lPosition);
					}
					
				}
				else
				{
					printf("Failed to get device inforation for device 0.  Result = %d\n", lResult);
				}
			}
		}
	}

	printf("Press any key to continue.");
	getchar();
	return 0;
}

