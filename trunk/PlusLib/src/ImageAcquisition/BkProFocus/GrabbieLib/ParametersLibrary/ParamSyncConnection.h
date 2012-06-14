#pragma once

#include "OemParams.h"  // TODO: remove this

	

class ParamSyncConnectionImp;
class TcpClient;

//////////////////////////////////////////////////////////////////////////
//
// ParamSyncConnection
//
// Design: SDD25278
//
// Purpose: Read and synchronize state from external scanner.
//
// Requirements:
// - Must be able to read a usecase from the external scanner using the Toolbox Command Protocol over a TCPClient.
// - Must register callbacks for the parameters mentioned under ParamSync requirements, using the OEM Protocol over a TCPClient.
//
//////////////////////////////////////////////////////////////////////////

/// <summary>Read and synchronize state from external scanner.
///  Design: SDD25278.	 
/// </summary>
/// Some useful examples are given in \ref PageReadOemParamList
class ParamSyncConnection
{
public:
	/// <summary>	Constructor. </summary>
	/// <param name="oemClient">		   	[in] A pointer to the TCPClient handling the OEM protocol. No ownership is taken. </param>
	/// <param name="toolboxCommandClient">	[in] A pointer to the TCPClient handling the Toolbox Command protocol. No ownership is taken. </param>
	ParamSyncConnection(TcpClient* oemClient, TcpClient* toolboxCommandClient);
	/// <summary>	Destructor. </summary>
	virtual ~ParamSyncConnection();

	/// <summary>
	/// Establish a TCPIP connection to the external scanner for the OEM protocol.
	/// IP address and Port is read from the CasaEngine.ini file.
	/// </summary>
	/// <returns>	False if an error occurs. </returns>
	
	virtual bool ConnectOEMInterface();
	/// <summary>
	/// Establish a TCPIP connection to the external scanner for the Toolbox Command protocol.
	/// IP address and Port is read from the CasaEngine.ini file.
	/// </summary>
	/// <returns>	False if an error occurs. </returns>
	
	virtual bool ConnectToolboxCommandInterface();
	/// <summary>	Read a usecase from the external scanner using the <b>Toolbox</b>. </summary>
	/// <param name="ppszUseCaseBuffer">	[out] returns a pointer to the buffer containing the read usecase.
	/// 									The buffer is only valid until another command is sent to the external scanner.
	/// 									The buffer is owned by ParamSyncConnection. Do not delete it. </param>
	/// <returns>	False if an error occurs. </returns>

	virtual bool GetUseCase(char** useCaseBuffer);
	//virtual int DisconnectOEMInterface();
	//virtual int DisconnectToolboxCommandInterface();


	/// <summary> Reads "C:\Console\LatestCalcOkUseCase.dat" from external scanner. 
	/// The use case that has been saved after tha last successful Calc()
	/// The following conditions are needed for this function to be successful:
	///  \li In  Console.ini or MyConsole.ini, add the line  \t EnableUseCaseDumpAfterEachCalc=1
	///  \li Start the Tool \t tcp2toolbox. It intercepts the command \t GET_USECASE 
	///  \li Establish a connection to the Toolbox (Toolbox Client)
	/// </summary>
	///
	/// <param name="ppszUseCaseBuffer">	[out] returns a pointer to the buffer containing the read usecase.
	/// 									The buffer is only valid until another command is sent to the external scanner.
	/// 									The buffer is owned by ParamSyncConnection. Do not delete it. </param>
	/// <returns>	False if an error occurs. </returns>
	bool GetConsoleLatestCalcOKUseCase(char ** useCaseBuffer);

	/// <summary>	Gets the scanner mode using the OEM protocol </summary>
	/// <returns>	The scanner mode oem. </returns>
	ScannerMode GetScannerModeOem();

    /// <summary> Gets all relevant OEM parameters based on the current mode of the scanner.</summary>
    /// <param name="vec">	[out] Pointer to a vector of oemArray objects. The function
    ///  appends oemArray object to it. The caller must free both the oemArray objects 
    ///  and the vector object itself </param>
    /// <returns>	true if it succeeds, false if it fails. </returns>
    bool GetAllOemParams(OemArrayVector *vec);


	/// <summary>Calculate the size of a single frame transferred over Research interface
	/// using  the Toolbox. The toolbox is frozen, the usecase is stored on the scanner, 
	/// then sent to the client and stored in a local file. Finally, the local file
	/// is parsed and the buffer size is calculated. </summary>
	/// <param name="numSamples">	[in,out] If non-null, number of samples. </param>
	/// <param name="numLines">  	[in,out] If non-null, number of lines. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool CalcSapBufSizeUsingToolbox(int *numSamples, int* numLines);


	/// <summary> Gets latest use case, saves it, and calculates the needed size </summary>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool CalcSapBufSizeFromLatestUseCase(int * numSamples, int *numLines);

	/// <summary> Gets the needed OEM parameters and calculated the needed size </summary>
	/// \todo Implement proper handling of VFI. As of August 3rd 2011, VFI is not reported as submode
	///       Implement correct handling of Compounding. As of August 3rd, 2011, Number of compound angles is not 
	///       reported by the OEM interface
	bool CalcSapBufSizeUsingConsole(int *numSamples, int*numLines);

	/// Reads a <b> TEXT</b> file from the scanner and saves it locally
	bool GetFileFromScanner(char* remoteName, char* localName);

	/// 
	size_t SendOemQuery(LPCTSTR query, bool getReply = true);

	/// <summary>	Gets a single oem parameter value. </summary>
	/// <param name="oemArr">	[in,out] If non-null, the oem arr. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool GetSingleOemParamVal(oemArray *oemArr, char view='\0');



	 
private:
	/// Private implementation.
	ParamSyncConnectionImp* impl;
};