January 13, 2014
3DGuidance driveBAY P/N: 940041 Rev E
Ascension Technology Corporation, Inc.

*******************************************************
This CD-ROM contains the relevant device drivers for the ATC driveBAY and trakSTAR systems. It also contains currently available Utilities. 
*******************************************************

DIFFERENCES FROM DRIVEBAY/TRAKSTAR REV D1 RELEASE:

-	Install.msi and USB driver are now both digitally signed for compatibility with newer versions of Windows.
-	See Readme files for Rev D1 for additional differences from the Rev D release.

**********************************************************
CONTENT LISTING:

[3DG API Developers] - Library and sample files for software developers.

[3DG API] - Contains latest .h,.lib,and DLL files. Use these when compiling applications to utilize latest functionality.
-	ATC3DG.h		n/a	   	8/22/12
-	ATC3DG.lib		n/a	   	8/22/12
-	ATC3DG.dll		36.0.19.8	8/22/12
-	ATC3DG64.lib	n/a	   	8/22/12
-	ATC3DG64.dll	36.0.19.8	8/22/12

[Binaries] – Build output folder for sample projects.

[Samples] - Sample VC++ projects for fundamental communication with the tracker using 3DGuidance API.  Sample projects are configured to locate ATC3DG.h and ATC3DG.lib in the [3DG API] folder and to place the output executable in the [Binaries] and [BinariesDebug] folders where a copy of ATC3DG.dll resides.  The projects are set up for default 32-bits builds.  Select the x64 build configuration for 64-bit builds.

[GetSynchronousRecord Sample] - Example of using the API to stream data from the tracker while monitoring the sensor status.

[Sample] – Project demonstrating basic initialization of the tracker and collection of data with GetAsynchronousRecord.

[Sample2] – Project showing appropriate use of each of the GetXXX/SetXXX calls, giving access to all configurable tracker parameters.

[Applications] – User applications.

- APITest.exe		36.0.21.3	8/21/12
Windows utility that permits communication with tracker using any single command/parameter defined in the 3DGuidance API, and displays tracker response.  32- and 64-bit versions.

- Cubes.exe  		36.0.20.3   8/22/12
Contains program demonstrating communication via USB using the 3DGuidance API.  32-bit version only.

- driveBAY Utility 	32.2.0.15	10/06/09
- tracker.dll	   	32.0.0.7	10/06/09
Utility and dll used for loading firmware updates and configuring power-up settings.  32-bit versions only.

[DSP Loader files] - Contains current firmware loader files 

-	POServer Dipole		6.55	 	10/18/12
-	MDSP				7.55		10/18/12
-	DiagStr				8.55		10/18/12
-	ErrStr				9.55		10/18/12

[USB Drivers] – Signed driver .inf file and coinstallers for USB installation for 32 and 64 bit platforms.


******************************************************
Device Driver Installation
**Install software before connecting the tracker**
1	Insert the CD-ROM into the PC CD-ROM tray.
2 	Follow prompts in the installer to copy all required files.
3 	When software installation is complete, connect power and the USB interface cable to the Tracker.
4 	Some versions of Windows may prompt the user with the New Hardware Wizard.
5 	Select the 'Install Automatically (Default)' radio button option.
6 	Press the 'Next' Button.
7 	A Window's message will appear, indicating that this driver is digitally signed by Northern Digital, Inc., Ascension’s parent company.
8 	Press the Install' button.
9 	Windows will install the appropriate files.
10 	Press the 'Finish' Button.