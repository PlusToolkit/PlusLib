May 14, 2010

*******************************************************
This directory contains a 'Rev D' upgrade package for operation of the ATC trakSTAR system
on a 64-bit version of Windows. 

See text below this content listing for upgrade instructions.

For assistance with changing settings or loading new firmware using the trakSTAR Utility, 
see the Utility Operation Guide included in the trakSTAR Utility folder.

*******************************************************

Specific items new for this release:

*64-bit Support - 64bit versions of Windows Vista OS now supported. 
		  Requires installation of the winusb.sys driver
		- Windows 7 64-bit support via manual driver installation 
		  (Windows 7 currently not supported by auto-installer) 
		* Please note that the API (DLL) does not yet support programming of 64-bit applications.

*ATC3DGm.DLL - version 32.0.7.41
	**Enhancements
		1. New parameters added to GetXXXXParameter calls,
 		   to provide user with Part number and Model 
		   string of each component (board, sensor, and
		   transmitter).
		2. Support for user accessible memory storage in
		   sensors -see VPD parameter type. 
		3. Support for 64-bit driver
		4. New user API access to Report Rate parameter
		5. New 'Pre-init' parameters - gives API access to
		   the settings stored in the ATC3DG.ini file. 
		   These can be manipulated PRIOR to initialization.
		6. Improved efficiency of error retrieval method 
		   during GetAsynchronous calls - improves 
		   throughput.
	**Bug Fixes-
		1. Additional 500mS added to the initialize delay
		   following a reset. Corrects a timeout bug observed
		   on some Vista OS machines. 
		2. Modified items preventing Unicode support.
		3. Corrected timestamp synchronization method for 
		   multi-unit configurations. 
		
*Cubes - version 32.0.20.9
	**Enhancements
		1. Implements enhanced Status Bit information, as 
		   returned by GetSensorStatus.  
		2. Implements support for new media parameter types.
		   Allows operation with the RS232 interface 
		   (single unit only).

*DSP firmware Revisions -versions: X.53
	**Enhancements-
		1. Added support for user accessible memory in 
		   individual tracker components. See new VPD
		   parameters.
		2. Additional information added to that returned 
		   with the Status bits.
		3. Improved WRT performance - Implemented algorithm
		   changes which allow sensor operation in closer 
		   to the transmitter.

	**Bug Fixes-
		1. Corrected several low-level DSP communication bugs.
		2. Corrected a bug resutled in incomplete response 
		   to the system status examine command.
		3. Corrected a bug that resulted in the wrong error
		   code being returned when an invalid prom was 
		   detected.  


*APITest (formerly PCITalk)- version 32.0.0.7
	**Enhancements
		1. Adopted medSAFE version of this app for consistency
		   across 3DGuidance product line.

*trakSTAR Utility - version 32.3.0.15

*tracker.dll - version 32.0.0.7
	
*Other
	1. pciTalk.exe renamed to APITest.exe - for consistency with 
	   3DG medSAFE.
	2. Updated Sample projects to include 2008.NET build support

**********************************************************
CONTENT LISTING:

[3D Guidance API]
   	- Contains latest .h,.lib,and DLL files. Use these 
	  when compiling applications to utilize latest
	  functionality.
		-ATC3DG.h	n/a	   9/24/09
		-ATC3DG.lib	n/a	   10/06/09
		-ATC3DG.DLL	32.0.7.41  10/06/09

[APITest]	Windows utility that permits communication with tracker using 
		any single command/parameter defined in the 3DGuidance API, 
		and displays tracker response.

[Cubes]	- Contains program demonstrating communication via USB
	  using the 3DGuidance API
		-Cubes	  32.0.20.9   10/08/09

[trakSTAR_Utility] - Contains Utility used for loading firmware 
		     updates and configuring power-up settings.
			-trakSTAR Utility 32.3.0.15  10/06/09
			-tracker.dll	  32.0.0.7  10/06/09

		[DSP-Loader files] - Contains current firmware 
				     loader files 
			-dipole PO	6.53
			-mdsp		7.53
			-diag_strings	8.53
			-error_strings	9.53

[Samples]	Contains subdirectories Sample,Sample2,and GetSynchRecordSample
		containing C++ Project files/sample code for fundamental 
		communication with the tracker using 3DGuidance API. 
		Sample 2 project shows appropriate use of each of the 
		GETXXX/SETXXXX calls, giving access to all configurable tracker
		parameters. GetSynchronousRecord Sample gives example of the
		GetSynchronousRecord() call.

[USB Install] - Contains installation files (.inf) and low level USB driver for
		operation of 3DGuidance driveBAY system on 64-bit Windows OS.


******************************************************
Upgrade Instructions
******************************************************
Use of the 64-bit driver requires two items:
-Upgrade firmware to 'Rev D'- using existing host machine
-Install new device driver on 64-bit host machine. 
NOte: If trakSTAR is running 'Rev D' firmware (denoted by Sector Rev labels 'X.53' 
      as shown in in 'Flash Maintenance' tab of utility), skip upgrade of firmware

******************************************************
Upgrade Firmware
1. Connect the tracker to the existing Windows host machine
2. Start the trakSTAR Utility application and follow prompts to connect to 
   the tracker. (see trakSTAR Utility Operation.pdf for specific details)
   Note that a new version of the Utility is included with this Rev D upgrade.
3. Select the 'Flash Maintenance' tab and upgrade each of the 4 accessible 
   active sectors, beginning with the 'Dipole PO' sector. When the browse for file 
   window opens, point the utility to the 'DSP-Loader files' sub-directory included 
   with this upgrade. 
4. After upgrade, each of the sectors should show the DSP version numbers listed 
   above (X.53).

******************************************************
Device Driver Installation
1. When the firmware in the tracker has been updated, remove the tracker and connect to the 64-bit host 
   machine.
2. Windows will prompt the user with the New Hardware Wizard.
3. Select the 'Install from a list or a specific location (Advanced)' radio button option.
4. Press the 'Next' Button.
5. Select the 'Include this location in the search' check box.
6. Press the 'Browse' button.
7. Navigate to the directory containing the REv D upgrade package.
8. Navigate to the 'USB Install' sub-directory.
9. Press the 'OK' button.
10.Press the 'Next' button.
11.A Window's message will appear, indicating that this driver has not passed 'Windows Logo Testing'
12.Press the 'Continue Anyway' button.
13.Windows will install the appropriate files.
14.Press the 'Finish' Button.
**Note that the new API files will not be copied to the host machine as part of this upgrade.
 These files can be found in the '3D Guidance API' folder included with the upgrade.


