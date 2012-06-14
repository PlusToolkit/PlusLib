#pragma once

#include "ParamConnectionSettings.h"
#include "ObservedCmdCtrlState.h"
#include "ObserverCmdCtrlState.h"

#include "OemParams.h"

class CmdAndCtrlImpl;   // This will be a private implementation of CommandAndControl





/// <summary> 
/// Command control. 
/// 
/// </summary>
class CommandAndControl: public ObservedCmdCtrlState
{
public:
	/// <summary> Constructor. 
	/// \li Initialize internal variables
	/// \li Connect to OEM interface, Tcp2Toolbox as commanded by ctrlSettings
	/// \li Check if LatestCalcUseCase is actually saved, if CmdCtrlSettings::useConsoleLastOkUseCase is true
	/// \li Register a callback function - CmdAndCtrlImpl::AutoUpdate() if CmdCtrlSettings::autoUpdate is true
	///      <b> Notice that auto update is available only when connected to Console </b>
	/// </summary>
	/// <param name="paramSettings"> [in] Settings for the connection - OemPort, ToolboxPort, ScannerAddress. </param>
	/// <param name="ctrlSetting">	 [in] Settings for the CommandAndControl object - useToolbox, useConsole etc. </param>
	/// 
	/// \note The actual functionality is in the private implementation - CmdAndCtrlImpl 
	/// \note ctrlSettings is an input only. The actual values of CmdAndCtrlImpl::cmdCtrlState is 
	///       determined by the success of different checks in the constructor. 
	/// \note CmdAndCtrlImpl::cmdCtrlState <b> MUST </b> reflect what is possible at RUN TIME. 
	///       Many of the command functions depend on it.
	///
	CommandAndControl(ParamConnectionSettings* paramSettings, CmdCtrlSettings* ctrlSetting );

	/// <summary>	Finaliser. </summary>
	virtual ~CommandAndControl();

	/// <summary>	Gets the command control settings. </summary>
	/// <returns>	Settings used to control the operation. </returns>
	CmdCtrlSettings GetCmdCtrlSettings();

	/// <summary>Return the state of the scanner (Unknown, Freeze or Scan) </summary>
	/// <returns>Scanner state - Unknown, Freeze or Scan. </returns>
	ScannerState GetScannerState();

	/// <summary> Return own state </summary>
	/// <returns>Own (Command And Control) state </returns>
	CmdCtrlState GetCmdCtrlState();



	/// <summary> Reads the parameters available via the OEM interface and saves them to file.
	/// To use this command, there must be a connection to Console (CmdCtrlSettings::useConsole = true).
	/// </summary>
	/// <param name="filename">	[in] Name of the file to save data </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SaveOemParameters(char *filename);

	/// <summary> Get an usecase from the Toolbox
	///   \li Freeze the toolbox
	///   \li Ask the toolbox to read all parameters
	///   \li Ask the toolbox to save the use case to a file
	///   \li Ask tcp2toolbox to transfer the usecase
	///   \li Save the usecase to a file with a name \c filename
	/// </summary>
	/// <param name="filename">	[in] Name of the file to save data. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SaveUseCaseToolbox(char *filename);


	/// <summary> Get the latest saved use case from the console and save it to a file.
	/// 
	/// </summary>
	/// <param name="filename">	[in] Name of the file to save data. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SaveLatestCalcUseCaseConsole(char *filename);

	/// <summary>Calculate the size of Sapera Buffers for 1 frame. 
	///  The behaviour of the function depends upon the CmdCtrlSettings parameter in the constructor.
	///  If CmdCtrlSettings::useConsoleLastOkUseCase is enabled then the function uses the last
	///  savef usecase to calculate the buffer size. If Console is enabled (CmdCtrlSettings::useConsole == true), 
	///  then the function uses the OEM interface to calculate the size of the buffer. 
	///  Finally if CmdCtrlSettings::useToolbox is true, an use case is transferred from the Toolbox and 
	/// used to calculate the size of the buffer. The imlpementation of these functions is 
	/// in the ParamSyncConnection.
	/// </summary>
	/// <param name="numSamples"> [out] Number of samples </param>
	/// <param name="numLines"> [out] Number of scan lines </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	/// 
	/// \see ParamSyncConnection::CalcSapBufSizeUsingToolbox 
	/// ParamSyncConnection::CalcSapBufSizeFromLatestUseCase 
	/// ParamSyncConnection::CalcSapBufSizeUsingConsole
    ///
	bool CalcSaperaBufSize(int *numSamples, int *numLines);

	/// <summary> Get the mode of the scanner </summary>
	virtual ScannerMode GetScannerMode();

	//void Snap(int num_frames);
	//void Grab();
	//void Stop();
	
	//bool Save();
	//bool AutoSave();

	void Attach(ObserverCmdCtrlState* observer);
	void Detach(ObserverCmdCtrlState* observer);

public:
	CmdAndCtrlImpl * impl;
};

