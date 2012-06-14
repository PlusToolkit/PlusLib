#pragma once

#include "LogLibrary.h"
/**
  \file CmdCtrlTypes.h
  \brief Declare some structures to be used by the user of CommandAndControl objects

 */

/// <summary> Settings for the Command and Control class </summary>
struct CmdCtrlSettings
{
	bool useToolBox;                ///< Shoudl we use the Toolbox ?
	bool useConsole;                ///< Should we use the Console ?
	bool useConsoleLastOkUseCase;   ///< Should we use the latest (auto)saved use case by the Console?
	bool autoUpdate;                ///< Should we subscribe to Console events and update scanner state and buffer sizes every time?

	/// <summary>By default everything is disabled </summary>
	CmdCtrlSettings(){
		useToolBox = false;
		useConsole = false;
		useConsoleLastOkUseCase = false;
		autoUpdate = false;
	};
	
	void LoadFromIniFile(char * UseCaseName)
	{
		char SectionName[] = "Command And Control";
		this->useToolBox = (1 == (GetPrivateProfileInt(SectionName, "useToolbox", 0, UseCaseName)));
		this->useConsole = (1 == (GetPrivateProfileInt(SectionName, "useConsole", 0, UseCaseName)));
		this->useConsoleLastOkUseCase = (1 ==(GetPrivateProfileInt(SectionName, "useConsoleLastOkUseCase", 0, UseCaseName)));
		this->autoUpdate = (1 ==(GetPrivateProfileInt(SectionName, "autoUpdate", 0, UseCaseName)));
	}

	
	void SaveToIniFile(char * UseCaseName)
	{
		char SectionName[] = "Command And Control";
		
		WritePrivateProfileString(SectionName, "useToolbox", ((useToolBox==true)?"1":"0"), UseCaseName);
		WritePrivateProfileString(SectionName, "useConsole", ((useConsole==true)?"1":"0"), UseCaseName);
		WritePrivateProfileString(SectionName, "useConsoleLastOkUseCase", ((useConsoleLastOkUseCase==true)?"1":"0"), UseCaseName);
		WritePrivateProfileString(SectionName, "autoUpdate", ((autoUpdate==true)?"1":"0"), UseCaseName);
	}
};

/// <summary> Values that represent ScannerState.  </summary>
enum ScannerState  {
	SCAN_STATE_UNKNOWN = 0,   ///< Do not know if the scanner is scanning or frozen
	SCAN_STATE_FREEZE,        ///< Scanner is Frozen
	SCAN_STATE_SCAN           ///< Scanner is scanning
};

/// <summary>	Values that represent state of command-and control object.  </summary>
enum CmdCtrlState  {
	CMD_STATE_READY = 0,         ///< Finished last task. Waiting for commands
	CMD_STATE_WAITING_SYNC,      ///< Waiting for a sync
	CMD_STATE_SNAPPING,          ///< Acquiring a number of frames
	CMD_STATE_GRABBING,          ///< Grabbing
};
