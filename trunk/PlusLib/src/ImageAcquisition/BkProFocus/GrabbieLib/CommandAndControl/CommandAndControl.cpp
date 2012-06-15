#include "tchar.h" // for _T
#include <iostream>
#include <assert.h>

#include "LogLibrary.h"
#include "CommandAndControl.h"
#include "TCPClient.h"
#include "Acquisition.h"
#include "ParamSyncConnection.h"
#include "ParamConnectionSettings.h"
#include "OemParams.h"
#include "OemParamsList.h"
#include "UseCaseParser.h" 
#include "UseCaseStructs.h"
#include "WSAIF.h"




static unsigned const TCP_CLIENT_BUFFER_SIZE = (2*1024*1024);   //!< Buffer size in the TCP client objects
static unsigned const TCP_SMALL_BUFFER_SIZE = (2*1024);         //!< Used for parameter to callback functions

/// <summary>Private implementation of CommandAndControl class </summary>
class CmdAndCtrlImpl: public CallbackEnabled, public ObservedCmdCtrlState
{
public:

	/// <summary> Constructor. 
	/// \li Initialize internal variables
	/// \li Connect to OEM interface, Tcp2Toolbox as commanded by ctrlSettings
	/// \li Check if LatestCalcUseCase is actually saved, if CmdCtrlSettings::useConsoleLastOkUseCase is true
	/// \li Register a callback function - CmdAndCtrlImpl::AutoUpdate() if CmdCtrlSettings::autoUpdate is true
	///      <b> Notice that auto update is available only when connected to Console </b>
	/// </summary>
	/// <param name="pSettings">  	[in] If non-null, options for controlling the operation. </param>
	/// <param name="ctrlSetting">	[in] If non-null, the control setting. </param>
	/// \note ctrlSettings is an input only. The actual values of CmdAndCtrlImpl::cmdCtrlState is 
	/// determined by the success of different checks in the constructor. 
	/// \note CmdAndCtrlImpl::cmdCtrlState <b> MUST </b> reflect what is possible at RUN TIME. 
	///       Many of the command functions depend on it.
	///
	
	CmdAndCtrlImpl(ParamConnectionSettings* pSettings,  CmdCtrlSettings* ctrlSetting);
	~CmdAndCtrlImpl();

	ScannerState GetScannerState();   ///< Return the state of the scanner
	CmdCtrlState GetCmdCtrlState();   ///< Return own state
	//void Notify();                    ///< Call all observers  

	/// <summary>Private implementation of CommandAndControl::SaveOemParameters </summary>
	/// <param name="filename">[in] File name  </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SaveOemParameters(char *filename);

	/// <summary>Private implementation of CommandAndControl::SaveLatestCalcUseCaseConsole </summary>
	/// <param name="filename">	[in] File name. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SaveLatestCalcUseCaseConsole(char *filename);

	/// <summary>	Saves a use case from toolbox.  Private implementation of CommandAndControl::bool SaveUseCaseToolbox(char *filename);</summary>
	/// <param name="filename">	[in] File name. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SaveUseCaseToolbox(char *filename);

	/// <summary>	Gets the command control settings. </summary>
	/// <returns>	The command control settings. </returns>
	CmdCtrlSettings GetCmdCtrlSettings();

	/// Private implementation of GetSaperaBufSize

	bool CalcSaperaBufSize(int *numLines, int *numSamples);

	/// Returns a structure with the scan mode
	ScannerMode GetScannerMode();

	/// Notify observers for a change in the state
	void Notify();


private:
	/// <summary> Gets Console.ini from the scanner and checks for
	///   EnableUseCaseDumpAfterEachCalc=1
	/// </summary>
	bool IsLatestCalcSavedToUseCase();

	/// <summary>Register a callback to be called when the scanner is 
	/// FROZEN or UNFROZEn. The registered callback function is AutoUpdate().
	/// The callback function is added to the lists of callbacks only the first time.
	/// It stays registered until the object is destroyed. The operation is reflected
	/// by the two member variables - subscribedToEvents adn eventCallbackRegistered</summary>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool SubscribeOemEvents();

	/// <summary>Instructs the s</summary>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool UnSubscribeOemEvents();

	/// <summary> Callback function. Updates the size of the SaperaBuffers </summary>
	/// <param name="buf"> [in] Contains the message from the UltraView Scanner </param>
	/// <param name="bufferSize"> The length of the message </param>
	void static AutoUpdate(char* buf, size_t bufferSize);

	/// <summary>Changes the values of the state </summary>
	/// <param name="scanner">	New values for scanner state. </param>
	/// <param name="command">	New values for CommandAndControl object. </param>
	void SetState(ScannerState scanner, CmdCtrlState command);

	

private:
	TcpClient *tbxClient;    ///< Communication with tcp2toolbox
	TcpClient *oemClient;    ///< Communication with OEM interface
	WSAIF wsaif;             ///< Wrapper for Windows Sockets Interface

    CmdCtrlState  cmdCtrlState;  ///< CommandAndControl state
	ScannerState  scannerState;  ///< Scanner state

	ParamSyncConnection* parSyncConnection;    ///< Object communicating with parameters
	char buffer[TCP_SMALL_BUFFER_SIZE];        ///< Buffer used to pass parameters to the callback function 
	
	bool subscribedToEvents;         ///< Flag used internally to control the behaviour of SubscribeOemEvents()
	bool eventCallbackRegistered;    ///< Flag used internally to control the behaviour of SubscribeOemEvents()

	ParamConnectionSettings parConnectSetting;  ///< Connection settings - IP address etc.
	CmdCtrlSettings cmdCtrlSettings;            ///< Settings for the Command And Control Module - how to behave
	

	static int numInstances;           ///< Singleton object. Count no more than 1 

public:
	static CmdAndCtrlImpl*  ptrThis;   ///< Pointer to the single object. Used by the auto-update

private:    // Handling multithreading
	 CRITICAL_SECTION criticalState;

};


// ----------------------------------------------------------------------------
// 
//    Command And Control - Private implementation
//    
// ----------------------------------------------------------------------------

int CmdAndCtrlImpl::numInstances = 0;
CmdAndCtrlImpl* CmdAndCtrlImpl::ptrThis = NULL;


CmdAndCtrlImpl::CmdAndCtrlImpl( ParamConnectionSettings* pSettings,  CmdCtrlSettings* ctrlSetting)
{
	if (CmdAndCtrlImpl::numInstances > 0)
	{
    LogPrintf("ERROR: Command And Control must be singleton" );
		//MessageBox(NULL,"Command And Control must be singleton !!! .", "Error", MB_OK );
		assert(false);    // In debug mode come here
		_exit(1);         // In release mode - terminate
	}
	else
	{
		CmdAndCtrlImpl::numInstances ++;
	}
	this->parConnectSetting.SetOemPort(pSettings->GetOemPort());
	this->parConnectSetting.SetToolboxPort(pSettings->GetToolboxPort());
	this->parConnectSetting.SetScannerAddress(pSettings->GetScannerAddress());

	this->cmdCtrlSettings = *ctrlSetting;

    /*
	 *           Thread protection here
	 */
	InitializeCriticalSection( &criticalState );
	



	/*
	 *         Communications
	 */
	tbxClient = new TcpClient( &wsaif, TCP_CLIENT_BUFFER_SIZE, 
		                       this->parConnectSetting.GetToolboxPort(), 
							   this->parConnectSetting.GetScannerAddress());
	
	oemClient = new TcpClient( &wsaif, TCP_CLIENT_BUFFER_SIZE, 
								this->parConnectSetting.GetOemPort(), 
								this->parConnectSetting.GetScannerAddress());

	parSyncConnection = new ParamSyncConnection(this->oemClient, this->tbxClient);


	/*
	 *        Set behaviour
	 */
	this->subscribedToEvents = false;
	this->eventCallbackRegistered = false;
	
	bool success = true;
	if (cmdCtrlSettings.useConsole == true)
	{
		LogPrintf("Connecting to Console at '%s' Port %d ... ", 
			       this->parConnectSetting.GetScannerAddress(),
				   this->parConnectSetting.GetOemPort());

		success = parSyncConnection->ConnectOEMInterface();
		if (!success)
		{
      LogPrintf("ERROR: Could not establish connection to OEM. DISABLING OEM USAGE" );
			//MessageBox(NULL,"Could not establish connection to OEM.\n DISABLING OEM USAGE.", "Error", MB_OK );
			cmdCtrlSettings.useConsole = false;    // We cannot use the console
		}
		else
		{
			LogPrintf(" SUCCESS \n");
		}
	
	}

	if (cmdCtrlSettings.useConsoleLastOkUseCase || cmdCtrlSettings.useToolBox )
	{
		LogPrintf("Connecting to tcp2toolbox at '%s' Port %d ... ", 
			this->parConnectSetting.GetScannerAddress(),
			this->parConnectSetting.GetToolboxPort());

		success = parSyncConnection->ConnectToolboxCommandInterface();
		if (!success)
		{
      LogPrintf("ERROR: Could not establish connection to Tcp2Toolbox. DISABLING TOOLBOX USAGE." );
      //MessageBox(NULL,"Could not establish connection to Tcp2Toolbox.\nDISABLING TOOLBOX USAGE.", "Error", MB_OK );
			cmdCtrlSettings.useConsoleLastOkUseCase = false; 
			cmdCtrlSettings.useToolBox = false;
		}
		else
		{
			LogPrintf(" SUCCESS \n");
		}
	}
	
	// By this point the different useXXX flags are true if the user desires them and if 
	// the right connections has been established
	
	// We need to check also if all conditions are properly working.
	if (!cmdCtrlSettings.useConsole )  // If there is no connection to Console, then cannot do auto update
	{
		if (cmdCtrlSettings.useConsoleLastOkUseCase || cmdCtrlSettings.autoUpdate)
		{
			cmdCtrlSettings.useConsoleLastOkUseCase = false;
			cmdCtrlSettings.autoUpdate = false;
      LogPrintf("ERROR: No connection to Console! Disabling automatic update." );
			//MessageBox(NULL," No connection to Console!\n Disabling automatic update.", "Error", MB_OK );
		}
		
		
	}

	// If there is no connection to Tcp2Toolbox, useConsoleLastUseCase must be false
	if (cmdCtrlSettings.useConsoleLastOkUseCase)
	{
		LogPrintf("Inspecting if UseCase is automatically saved ");
		if (!IsLatestCalcSavedToUseCase())
		{
      LogPrintf("ERROR: Auto Save of Use Case is not enabled by Console. Disabling automatic update from LatestUseCase." );
			//MessageBox(NULL,"Auto Save of Use Case is not enabled by Console.\nDisabling automatic update from LatestUseCase.", "Error", MB_OK );
			cmdCtrlSettings.useConsoleLastOkUseCase = false;
		}
		else
		{
			LogPrintf(" SUCCESS \n");
		}
	}
	
	if (cmdCtrlSettings.autoUpdate)
	{
		LogPrintf("Subscribing to OEM events \n");
		this->SubscribeOemEvents();
	}

	CmdAndCtrlImpl::ptrThis = this;

}



CmdAndCtrlImpl::~CmdAndCtrlImpl()
{

	if (subscribedToEvents)
	{
		UnSubscribeOemEvents();
	}

	DeleteCriticalSection(&criticalState);
	

	tbxClient->Stop();               // We need to stop the clients to unregister
	oemClient->Stop();               // any pending callback functions

	delete parSyncConnection;
	delete tbxClient;
	delete oemClient;

	CmdAndCtrlImpl::numInstances--;   // Decrease the count
	CmdAndCtrlImpl::ptrThis = NULL;

}





bool CmdAndCtrlImpl::IsLatestCalcSavedToUseCase()
{
	if (this->cmdCtrlSettings.useConsoleLastOkUseCase)
	{
		parSyncConnection->GetFileFromScanner("C:\\Console\\console.ini", ".\\console.ini");

		int EnableUseCaseDumpAfterEachCalc = GetPrivateProfileInt("TestFacilities", "EnableUseCaseDumpAfterEachCalc", 0, ".\\console.ini"); 
	
		return (EnableUseCaseDumpAfterEachCalc == 1);
	}
	return false;
}



bool CmdAndCtrlImpl::SubscribeOemEvents()
{
	
	if (!eventCallbackRegistered)
	{
		this->oemClient->RegisterCallbackFunc(AutoUpdate, this->buffer, sizeof(this->buffer),"EVENT:");
		this->oemClient->RegisterCallbackFunc(AutoUpdate, this->buffer, sizeof(this->buffer),"SDATA:");
		eventCallbackRegistered = true;
	}
	
	this->parSyncConnection->SendOemQuery("CONFIG:EVENTS: 1;");
	// Subscriptiuon to data does not seem to work
	// this->parSyncConnection->SendOemQuery("CONFIG:DATA:SUBSCRIBE IMAGE_MODE;"); 
	subscribedToEvents = true;
	
	return subscribedToEvents;
}



bool CmdAndCtrlImpl::UnSubscribeOemEvents()
{
	this->parSyncConnection->SendOemQuery("CONFIG:EVENTS: 0;");
	subscribedToEvents = false;
	return true;
}



void CmdAndCtrlImpl::AutoUpdate(char* buf, size_t bufferSize)
{
	
	assert(bufferSize > 0);
	bufferSize = bufferSize; // dummy to escape C4189 in release builds
	if (CmdAndCtrlImpl::ptrThis == NULL)
	{
    LogPrintf("ERROR: CommandAndControl object has already been destroyed." );
		//MessageBox(NULL, _T("CommandAndControl object has already been destroyed"), _T("BUG"), MB_OK);
		assert(false);
		
		_exit(1);
	}

	ScannerState newState = ptrThis->scannerState;
	if (! _strnicmp("EVENT:", buf, strlen("EVENT:") ))
	{
		if (! _strnicmp("UNFREEZE", &buf[strlen("EVENT:")], strlen("UNFREEZE")) )
		{
			newState = SCAN_STATE_SCAN;
		}
		else if (! _strnicmp("FREEZE", &buf[strlen("EVENT:")], strlen("FREEZE")))
		{
			newState = SCAN_STATE_FREEZE;
		} 
		else
		{
			newState = SCAN_STATE_UNKNOWN;
		}


	}
	ptrThis->SetState(newState, ptrThis->cmdCtrlState);
	
}



CmdCtrlState CmdAndCtrlImpl::GetCmdCtrlState()
{
	return this->cmdCtrlState;
}


ScannerState CmdAndCtrlImpl::GetScannerState()
{
	return this->scannerState;
}



void CmdAndCtrlImpl::Notify()
{

	for (unsigned int n = 0; n < this->observerList.size(); n++)
	{
		observerList[n]->Update();
	}
}




void CmdAndCtrlImpl::SetState(ScannerState scanner, CmdCtrlState command)
{
	EnterCriticalSection(&criticalState);
	this->scannerState = scanner;
	this->cmdCtrlState = command;
	this->Notify();
	LeaveCriticalSection(&criticalState);
}






bool CmdAndCtrlImpl::SaveOemParameters(char *filename)
{

	OemArrayVector oemParams;


	if (this->cmdCtrlSettings.useConsole == false)
	{
		return false;
	}


	FILE *fid = fopen(filename, "wb");

	if (fid == NULL)
	{
		return false;
	}

	LogPrintf("Getting parameters from OEM interface ... ");


	bool success = this->parSyncConnection->GetAllOemParams(&oemParams);
	LogPrintf(" DONE \n");

	LogPrintf("Saving OEM parameters to %s ", filename);

	unsigned int n;
	for (n = 0; n < oemParams.size(); n++)
	{
		append_oem_array_to_file(fid, oemParams[ n ]);
	}
	
	fclose(fid);

	LogPrintf(" DONE \n");
	success = true;

	size_t numElem = oemParams.size();

	for (n = 0; n < numElem; n++)
	{
		oemArray *pArray = oemParams.back();
		delete pArray;
		oemParams.pop_back();
	}
	

	return success;
}



bool CmdAndCtrlImpl::SaveLatestCalcUseCaseConsole(char *filename)
{
	bool success = true;
	DbgPrintf("Saving to %s \n", filename);
	
    // 
	if (cmdCtrlSettings.useConsoleLastOkUseCase == false)
	{
		LogPrintf(" SaveLatestCalcUseCaseConsole() is not enabled .");
		success = false;
		return false;
	}
	
	char *useCaseBuffer;
	LogPrintf("Fetching latest use case saved by Console ... ");
	success = parSyncConnection->GetConsoleLatestCalcOKUseCase(&useCaseBuffer);
	if (!success)
	{
		LogPrintf(" FAILED \n");
		success = false;
		return success;
	}
	else
	{
		LogPrintf(" SUCCESS \n");
	}


	FILE *fid = fopen(filename, "wt");
	if (fid == NULL)
	{
		LogPrintf("Could not create file %s \n", filename);
		success = false;
		return success;
	}
	
	size_t cnt = fwrite(useCaseBuffer, strlen(useCaseBuffer),1,fid);
	fclose(fid);

	if (cnt != 1)
	{
		LogPrintf("Writing use case '%s' FAILED\n", filename);
		success = false;
	}
	else
	{
		LogPrintf("Writing use case '%s' SUCCESS\n", filename);
		success = true;
	}

	return success;
}




bool CmdAndCtrlImpl::SaveUseCaseToolbox(char *filename)
{
	bool success = true;

	if (cmdCtrlSettings.useToolBox == false)
	{
		success = false;
		LogPrintf("Saving Toolbox UseCase to '%s' is not enabled \n", filename);
		return success;
	}

	char *useCaseBuffer;

	LogPrintf("Getting use case from the scanner ... ");
	success = parSyncConnection->GetUseCase(&useCaseBuffer);
	if (success)
	{
		LogPrintf(" SUCCESS \n");
	}
	else
	{
		LogPrintf(" FAILED \n");
		return success;
	}

	FILE *fid = fopen(filename, "wt");
	if (fid == NULL)
	{
		LogPrintf("Could not create file %s \n", filename);
		success = false;
		return success;
	}

	size_t cnt = fwrite(useCaseBuffer, strlen(useCaseBuffer),1,fid);
	fclose(fid);

	if (cnt != 1)
	{
		LogPrintf("Writing use case '%s' FAILED\n", filename);
		success = false;
	}
	else
	{
		LogPrintf("Writing use case '%s' SUCCESS\n", filename);
		success = true;
	}


	return success;
}



bool CmdAndCtrlImpl::CalcSaperaBufSize(int *numSamples, int *numLines)
{
	bool success = true;

	if (cmdCtrlSettings.useConsole && cmdCtrlSettings.useConsoleLastOkUseCase)
	{
		success = this->parSyncConnection->CalcSapBufSizeFromLatestUseCase(numSamples, numLines);
	}
	else if (cmdCtrlSettings.useConsole)
	{
		success = this->parSyncConnection->CalcSapBufSizeUsingConsole(numSamples, numLines);
	}
	else if (cmdCtrlSettings.useToolBox)
	{
		LogPrintf("Hope you have frozen the Console (if running).");
		success = this->parSyncConnection->CalcSapBufSizeUsingToolbox(numSamples, numLines);
	}
	else
	{
    LogPrintf("Param Sync Connection: Not using either Console nor Toolbox." );
		//::MessageBox(NULL, "Not using either Console nor Toolbox ", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
		success = false;
	}
	return success;
}


ScannerMode CmdAndCtrlImpl::GetScannerMode()
{
	ScannerMode scanMode;


	if (cmdCtrlSettings.useConsole)
	{
		scanMode = this->parSyncConnection->GetScannerModeOem();
	}
	else
	{
		LogPrintf("\n Asked for Scanner Mode. Failed: ScannerMode accessible only via OEM interface ");
		scanMode.present_A = true;
		
	}
	return scanMode;
}



CmdCtrlSettings CmdAndCtrlImpl::GetCmdCtrlSettings()
{
	return this->cmdCtrlSettings;
}



// ----------------------------------------------------------------------------
// 
//    Command and Control class
//    
// ----------------------------------------------------------------------------

CommandAndControl::CommandAndControl(ParamConnectionSettings* pSettings,  CmdCtrlSettings* ctrlSetting):
impl(new CmdAndCtrlImpl(pSettings,ctrlSetting))
{

}



CommandAndControl::~CommandAndControl()
{
	delete impl;
}


CmdCtrlState CommandAndControl::GetCmdCtrlState()
{
	return impl->GetCmdCtrlState();
}



ScannerState CommandAndControl::GetScannerState()
{
	return impl->GetScannerState();
}



// 
bool CommandAndControl::SaveOemParameters(char *filename)
{
	return impl->SaveOemParameters(filename);
}



bool CommandAndControl::SaveLatestCalcUseCaseConsole(char *filename)
{
	return impl->SaveLatestCalcUseCaseConsole(filename);
}


bool CommandAndControl::SaveUseCaseToolbox(char *filename)
{
	return this->impl->SaveUseCaseToolbox(filename);
}


bool CommandAndControl::CalcSaperaBufSize(int *numSamples, int *numLines)
{
	return this->impl->CalcSaperaBufSize(numSamples, numLines);
}


ScannerMode CommandAndControl::GetScannerMode()
{
	return this->impl->GetScannerMode();
}



void CommandAndControl::Attach(ObserverCmdCtrlState* observer)
{
	this->impl->Attach(observer);
}


void CommandAndControl::Detach(ObserverCmdCtrlState* observer)
{
	this->impl->Detach(observer);
}

CmdCtrlSettings CommandAndControl::GetCmdCtrlSettings()
{
	return this->impl->GetCmdCtrlSettings();
}