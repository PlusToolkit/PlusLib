#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "ParamSyncConnection.h"
#include "TCPClient.h"
#include "OemParams.h"

#include "UseCaseParser.h"
#include "UseCaseStructs.h"
#include "LogLibrary.h"


#define __DEFINE_VARS__   1
#include "OemParamsList.h" // When __DEFINE_VARS__ is defined, then the variables are initialized in the header file
#undef __DEFINE_VARS__

#include <iostream>



//#include "CasaLogInterface.h"

/// Size of the read buffer.
static const int READ_BUFFER_SIZE = 1024 * 1024;
/// Read buffer. This is used to receive messages from the external scanner.
static char s_ReadBuffer[READ_BUFFER_SIZE];

/// <summary>
///  Enumeration of the Scan Types
///  The following is very Proprietary. It may change from build to build
/// 		 
///  </summary>

typedef enum ScanModeEnum
{
	RTSC_MIDLEVEL_MODE_B1=0,	///< B Mode (2D)
	RTSC_MIDLEVEL_MODE_B2,		///< B Mode (2D)
	RTSC_MIDLEVEL_MODE_M1,		///< M Mode (1D - time-motion)
	RTSC_MIDLEVEL_MODE_M2,		///< M Mode (1D - time-motion)
	RTSC_MIDLEVEL_MODE_C1,		///< CFM Mode (2D)
	RTSC_MIDLEVEL_MODE_MC1,		///< M-CFM Mode (1D - time-motion)
	RTSC_MIDLEVEL_MODE_PWD1,	///< Pulsed Doppler Mode (1D - time-motion)
	RTSC_MIDLEVEL_MODE_CWD1,	///< Continuous Doppler Mode (1D - time-motion)
	RTSC_MIDLEVEL_MODE_DUM1,	///< Dummy Mode

	RTSC_MIDLEVEL_MODE_B3,		///< B Mode (2D)
	RTSC_MIDLEVEL_MODE_B4,
	RTSC_MIDLEVEL_MODE_B5,
	RTSC_MIDLEVEL_MODE_B6,
	RTSC_MIDLEVEL_MODE_B7,
	RTSC_MIDLEVEL_MODE_B8,
	RTSC_MIDLEVEL_MODE_B9,
	RTSC_MIDLEVEL_MODE_B10,
	RTSC_MIDLEVEL_MODE_B11,
	RTSC_MIDLEVEL_MODE_B12,

	RTSC_MIDLEVEL_MODE_C2,      ///< CFM Mode (2D), for duplicate display
	RTSC_MIDLEVEL_MODE_ELASTO,  ///< Elastography
	RTSC_MIDLEVEL_MODE_EXPMT1,  ///< Just for fun

	RTSC_MIDLEVEL_MODE_DBG1,    ///< Internal RTSC SW diagnostics and debug Mode

	RTSC_N_MIDLEVEL_MODES,      ///< Number of MidLevelModes

	RTSC_MIDLEVEL_MODE_UNUSED,  ///< For unused modes in BF_XmitDataTbl[bm].MidLevelModeSel
	FORCESIZE11 = 0x7FFFFFFF    ///< Ensure 32 bit size
} ScanMode;



/// <summary>
/// Private implementation of ParamSyncConnection 
/// </summary>
class ParamSyncConnectionImp
{
public:
	/// Constructor.asd
	ParamSyncConnectionImp(TcpClient* oemClient, TcpClient* toolboxCommandClient);
	/// Destructor.
	~ParamSyncConnectionImp();

public:
	/// Private implementation of ParamSyncConnection::ConnectOEMInterface().
	bool ConnectOEMInterface();
	
	/// Private implementation of ParamSyncConnection::ConnectToolboxInterface().
	bool ConnectToolboxInterface();

	/// Privare implementation of GetFileFromScanner
	bool GetFileFromScanner(char* remoteName, char* localName);

	/// Private implementation of ParamSyncConnection::GetUseCase().
	bool GetUseCase(char** useCaseBuffer);

	/// Private implementation of ParamSyncConnection::GetConsoleLastCalcOkUseCase()
	bool GetConsoleLatestCalcOKUseCase(char ** useCaseBuffer);


	/// <summary>	Gets a single oem parameter value. </summary>
	bool GetSingleOemParamVal(oemArray *oemArr, char view='\0');



	/// <summary>Calculate the size of a single frame transferred over Research interface
	/// using  the Toolbox. The toolbox is frozen, the usecase is stored on the scanner, 
	/// then sent to the client and stored in a local file. Finally, the local file
	/// is parsed and the buffer size is calculated. </summary>
	/// <param name="numSamples">	[in,out] If non-null, number of samples. </param>
	/// <param name="numLines">  	[in,out] If non-null, number of lines. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool CalcSapBufSizeUsingToolbox(int *numSamples, int* numLines);


    /// <summary>	Calculates the sap buffer size from latest use case.
	/// The use case from teh lates successful Calc() operation is saved in c:\console\
	/// This <b>test facility</b> is enabled in the console.ini file. 
	/// The function transfers the file from the scanner to a local file. 
	/// It then uses the UseCaseParser to parse it, and to calculate the size of the  buffer.
	/// </summary>
	/// <param name = "numSamples"> [out] Number of samples </param>
	/// <param name = "numLines"> [out] Number of lines </param>
    bool CalcSapBufSizeFromLatestUseCase(int * numSamples, int *numLines);


	/// <summary> Calculate the size of the sapera buffers using only the OEM interface </summary>
	bool CalcSapBufSizeUsingConsole(int *numSamples, int *numLines);

	/// <summary>	Gets an oem parameter list. </summary>
	/// <param name="list">	The list. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool GetOemParamList(OemParamDefStruct list[], bool getA, bool getB, oemArray **var, int *nvar);

	/// <summary>	Gets an oem vector array from list. See \ref PageReadOemParamList for example. </summary>
	/// <param name="list">	The list. Global lists are defined in OemParamsList.h </param>
	/// <param name="getA">	true to get view A. Can use ScannerMode structure for this </param>
	/// <param name="getB">	true to get view B. Can use ScannerMode structure for this </param>
	/// <param name="vec"> 	[out] Pointer to a vector of oemArray objects. The function appends
	/// 					oemArray object to it. The caller must free both the oemArray objects and
	/// 					the vector object itself. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool GetOemVectorArrayFromList(OemParamDefStruct list[], bool getA, bool getB, OemArrayVector *vec);

	/// <summary> Gets all relevant OEM parameters based on the current mode of the scanner.</summary>
	/// <param name="vec">	[out] Pointer to a vector of oemArray objects. The function
	///  appends oemArray object to it. The caller must free both the oemArray objects 
	///  and the vector object itself </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool GetAllOemParams(OemArrayVector *vec);

	/// <summary> Returns the scanner mode using the OEM interface. </summary>
	/// <returns>	The scanner mode. </returns>
	ScannerMode GetScannerModeOem();

    /// <summary>Get an integer using OEM interface. </summary>
    /// <param name="name">	[in] Name of the OEM parameter. </param>
    /// <param name="view">	The view. 'A', 'B' or '\0' </param>
    /// <returns> Integer value of OEM parameter. </returns>
	/// \code
	/// int num_lines = syncObj.GetOemInteger("b_scanlines_count", '\0');
	/// \encdode
    int GetOemInteger(char *name, char view);

	/// <summary>Get a double number using OEM interface. </summary>
	/// <param name="name">	[in] Name of the OEM parameter. </param>
	/// <param name="view">	The view. 'A', 'B' or '\0' </param>
	/// <returns>	The oem integer. </returns>
	/// \code
	/// double fs = syncObj.GetOemDouble("B_SAMPLE_FREQUENCY", '\0');
	/// \encdode
	double GetOemDouble(char *name, char view);

private:
	/// <summary>	Sends a toolbox command. </summary>
	/// <param name="command"> 	The command (including arguments) to send. </param>
	/// <param name="getReply">	Decides if a reply is expected. If so, the reply is read
	/// 						into s_ReadBuffer. </param>
	/// <returns>
	/// The length of the reply in s_ReadBuffer. If an error occured or
	/// bGetReply was false, 0 is returned.
	/// </returns>
	size_t SendToolboxCommand(LPCTSTR command, bool getReply = true);
	/// <summary>	Send the ReadAllParams command to the toolbox on the external scanner. </summary>
	/// <returns>	false if an error occurred. </returns>
	bool ToolboxReadAllParams();
	/// <summary>
	/// Send a command to cause the toolbox on the external scanner to save the
	/// current usecase to the harddrive on the external scanner.
	/// </summary>
	/// <returns>	false if an error occurred. </returns>
	bool ToolboxSaveUsecaseOnScanner();
	/// <summary>
	/// Send a command to cause the tcp2toolbox tool on the external scanner to
	/// transfer the usecase on the external scanner that has been saved by
	/// a previous call to ToolboxSaveUsecaseOnScanner().
	/// </summary>
	/// <param name="useCaseBuffer">	[out] returns a pointer to a buffer with the received usecase.
	/// 								ParamSyncControl owns the buffer. Do not delete it. The buffer is only valid
	/// 								until the next command is sent to the external scanner. </param>
	/// <returns>	false if an error occurred. </returns>
	bool ToolboxGetUsecaseFromScanner(char** useCaseBuffer);



	/// <summary>	Send a command to cause the external scanner to freeze. </summary>
	/// <param name="wasScanning">	[out] returns the scanning state previously to freezing.
	/// 							(true = scanning, false = frozen). Can be NULL, in which case the state is not
	/// 							returned. NOTE: Does not work reliably!!! </param>
	/// <returns>	false if an error occurred. </returns>
	bool ToolboxFreeze(bool* wasScanning);


	/// <summary>	Send a command to the external scanner to unfreeze. NOTE: does not work reliably!!! </summary>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool ToolboxUnfreeze();

   /// <summary>	Calculates the sap buffer size from saved use case. </summary>
   /// <param name="numSamples">	[out] number of samples. </param>
   /// <param name="numLines">  	[out] Number of lines. </param>
   /// <param name="filename">  	[in]  Name of file with use case. </param>
   /// <returns>	true if it succeeds, false if it fails. </returns>
   bool CalcSapBufSizeFromSavedUseCase(int * numSamples, int *numLines, char* filename);

public:

	/// <summary>	Sends an oem query. </summary>
	/// <param name="query">   	The query. </param>
	/// <param name="getReply">	Decides if a reply is expected. If so, the reply is read into
	/// 						s_ReadBuffer. </param>
	/// <returns>	. </returns>
	size_t SendOemQuery(LPCTSTR query, bool getReply = true);
	

private:
	/// <summary>	Test the reply received from the toolbox (in s_ReadBuffer). </summary>
	/// <returns>	true if the reply is "OK". Otherwise returns false. </returns>
	bool IsToolboxReplyOK() const;
	

private:
	/// Pointer to the TCPClient responsible for the OEM protocol connection. No ownership.
	TcpClient* oemClient;
	/// Pointer to the TCPClient responsible for the Toolbox Command protocol connection. No ownership.
	TcpClient* toolboxCommandClient;
};


//////////////////////////////////////////////////////////////////////////
//
//
// ParamSyncConnection Implementation
//
//
//////////////////////////////////////////////////////////////////////////

ParamSyncConnection::ParamSyncConnection(TcpClient* oemClient, TcpClient* toolboxCommandClient)
:	impl(new ParamSyncConnectionImp(oemClient, toolboxCommandClient))
{

}
ParamSyncConnection::~ParamSyncConnection()
{
	if(this->impl)
	{
		delete this->impl;
		this->impl = NULL;
	}
}
bool ParamSyncConnection::ConnectOEMInterface()
{
	return this->impl->ConnectOEMInterface();
}
bool ParamSyncConnection::ConnectToolboxCommandInterface()
{
	return this->impl->ConnectToolboxInterface();
}
bool ParamSyncConnection::GetUseCase(char** useCaseBuffer)
{
	return this->impl->GetUseCase(useCaseBuffer);
}

bool ParamSyncConnection::GetConsoleLatestCalcOKUseCase(char ** useCaseBuffer)
{
	return this->impl->GetConsoleLatestCalcOKUseCase(useCaseBuffer);
}

size_t ParamSyncConnection::SendOemQuery(LPCTSTR query, bool getReply /* = true */)
{
	return this->impl->SendOemQuery(query, getReply);
}


bool ParamSyncConnection::GetAllOemParams(OemArrayVector *vec)
{
	return impl->GetAllOemParams(vec);
}



bool ParamSyncConnection::GetSingleOemParamVal(oemArray *oemArr, char view /* ='\0'*/)
{
	return this->impl->GetSingleOemParamVal(oemArr, view);
}





bool ParamSyncConnection::CalcSapBufSizeFromLatestUseCase(int * numSamples, int *numLines)
{
	return this->impl->CalcSapBufSizeFromLatestUseCase(numSamples, numLines);
}

bool ParamSyncConnection::GetFileFromScanner(char* remoteName, char* localName)
{
	return impl->GetFileFromScanner(remoteName, localName);
}



ScannerMode ParamSyncConnection::GetScannerModeOem()
{
	return impl->GetScannerModeOem();
}


bool ParamSyncConnection::CalcSapBufSizeUsingToolbox(int *numSamples, int* numLines)
{
	return this->impl->CalcSapBufSizeUsingToolbox(numSamples, numLines);
}





bool  ParamSyncConnection::CalcSapBufSizeUsingConsole(int *numSamples, int *numLines)
{
	return this->impl->CalcSapBufSizeUsingConsole(numSamples, numLines);
}



//////////////////////////////////////////////////////////////////////////
//
//
// ParamSyncConnectionImp Implementation
//
//
//////////////////////////////////////////////////////////////////////////

ParamSyncConnectionImp::ParamSyncConnectionImp(TcpClient* oemClient, TcpClient* toolboxCommandClient)
:	oemClient(oemClient),
	toolboxCommandClient(toolboxCommandClient)
{
	assert(this->oemClient);
	assert(this->toolboxCommandClient);
}


ParamSyncConnectionImp::~ParamSyncConnectionImp()
{
	// this->oemClient and this->toolboxCommandClient are owned by ParamSyncModule, not by us.
}


bool ParamSyncConnectionImp::ConnectOEMInterface()
{
	return this->oemClient->Start(); // Start reader thread.
}


bool ParamSyncConnectionImp::ConnectToolboxInterface()
{
	return this->toolboxCommandClient->Start(); // Start reader thread.
}


bool ParamSyncConnectionImp::GetUseCase(char** useCaseBuffer)
{
	memset(s_ReadBuffer, 0, READ_BUFFER_SIZE);

	// Ensure that the scanner is not running.
	bool res = ToolboxFreeze(NULL);
	// Read all parameters from the scanner to the toolbox
	res = res && ToolboxReadAllParams();
	// Save the usecase loaded in the toolbox to the scanner hd
	res = res && ToolboxSaveUsecaseOnScanner();
	// Read the newly saved usecase on the scanner hd
	res = res && ToolboxGetUsecaseFromScanner(useCaseBuffer);

	return res;
}



bool ParamSyncConnectionImp::GetConsoleLatestCalcOKUseCase(char ** useCaseBuffer)
{
	 
	memset(s_ReadBuffer, 0, READ_BUFFER_SIZE);


	*useCaseBuffer = NULL;

	SendToolboxCommand("GET_USECASE \"c:\\Console\\LatestCalcOkUseCase.dat\"", true);
	int size;
	if(sscanf_s(s_ReadBuffer, "OK #%d", &size) != 1)
		return false;
	// Skip header
	char* p = s_ReadBuffer + 4; // Skip "OK #"
	for(; *p >= '0' && *p <= '9'; p++) // Skip size spec
		;
	*useCaseBuffer = p;

	return true;

}

size_t ParamSyncConnectionImp::SendToolboxCommand(LPCTSTR command, bool getReply /* = true */)
{
	this->toolboxCommandClient->Write(command, strlen(command));
	if(!getReply)
		return 0;
	try
	{
		size_t len = this->toolboxCommandClient->Read(s_ReadBuffer, READ_BUFFER_SIZE);	// Read reply.
		assert(s_ReadBuffer[len] == '\0');    // Is guaranteed by read(), however paranoia is good.
		// TRACE("Received reply: " << s_ReadBuffer);
		const char errorString[] = "ERROR";
		if(_strnicmp(errorString, s_ReadBuffer, sizeof(errorString) - 1) == 0)
		{
			// ERROR!!!
			//LogEngineError2(_T("Toolbox command '%s' received error reply: '%s'"), command, s_ReadBuffer);
			return 0;
		}
		if(len == 0)
		{
			// No reply!!!
			//LogEngineError1(_T("Toolbox command '%s' godt no reply\n"), command);
		}
		return len;
	}
	catch(TcpClientWaitException e)
	{
		//LogEngineError1(_T("Reading reply from toolbox command %s failed"), command);

		// TODO: Handle timeout. How? Reset connection?
	}
	return 0;
}


size_t ParamSyncConnectionImp::SendOemQuery(LPCTSTR query, bool getReply /* = true */)
{
	//std::cout << "Send: " << query << std::endl;
	
	memset(s_ReadBuffer, 0, sizeof(s_ReadBuffer));

	this->oemClient->Write(query, strlen(query));
	if(!getReply)
		return 0;
	try
	{
		size_t len = this->oemClient->Read(s_ReadBuffer, READ_BUFFER_SIZE);	// Read reply.
		assert(s_ReadBuffer[len] == '\0');    // Is guaranteed by read(), however paranoia is good.
		//TRACE("Received reply: %s\n", s_ReadBuffer);
		const char errorString[] = "ERROR";
		if(_strnicmp(errorString, s_ReadBuffer, sizeof(errorString) - 1) == 0)
		{
			// ERROR!!!
			//LogEngineError2(_T("Toolbox command '%s' received error reply: '%s'"), command, s_ReadBuffer);
			return 0;
		}
		if(len == 0)
		{
			// No reply!!!
			//LogEngineError1(_T("Toolbox command '%s' godt no reply\n"), command);
		}
		// std::cout << "OEM : " << s_ReadBuffer << std::endl;

		return len;
	}
	catch(TcpClientWaitException e)
	{
		//LogEngineError1(_T("Reading reply from toolbox command %s failed"), command);

		// TODO: Handle timeout. How? Reset connection?
	}
	return 0;
}

bool ParamSyncConnectionImp::ToolboxReadAllParams()
{
	SendToolboxCommand("ReadAllParams", true);
	if(!IsToolboxReplyOK())
	{
		//LogEngineError1(_T("Toolbox command ReadAllParams failed: %s\n"), s_ReadBuffer);
		return false;
	}
	return true;	
}


bool ParamSyncConnectionImp::ToolboxSaveUsecaseOnScanner()
{
	SendToolboxCommand("SaveAs c:\\temp_usecase.dat", true);
	if(!IsToolboxReplyOK())
	{
		//LogEngineError1(_T("Toolbox command SaveAs failed: %s\n"), s_ReadBuffer);
		return false;
	}
	return true;
}


bool ParamSyncConnectionImp::ToolboxGetUsecaseFromScanner(char** useCaseBuffer)
{
	*useCaseBuffer = NULL;

	SendToolboxCommand("GET_USECASE \"c:\\temp_usecase.dat\"", true);
	int size;
	if(sscanf_s(s_ReadBuffer, "OK #%d", &size) != 1)
		return false;
	// Skip header
	char* p = s_ReadBuffer + 4; // Skip "OK #"
	for(; *p >= '0' && *p <= '9'; p++) // Skip size spec
		;
	*useCaseBuffer = p;

	return true;
}


bool ParamSyncConnectionImp::ToolboxFreeze(bool* wasScanning)
{
	SendToolboxCommand("FREEZE", true);
	if(IsToolboxReplyOK())
	{
		if(wasScanning)
			*wasScanning = true;
		return true; // Succeeded. Scanner was scanning but is now frozen.
	}
	if(strcmp(s_ReadBuffer, "Scanner already frozen") == 0)
	{
		if(wasScanning)
			*wasScanning = false;
		return true; // Succeeded. Scanner was already frozen.
	}
	//LogEngineError1(_T("Toolbox command FREEZE failed: %s\n"), s_ReadBuffer);
	return false;
}


bool ParamSyncConnectionImp::ToolboxUnfreeze()
{
	SendToolboxCommand("UNFREEZE", true);
	if(!IsToolboxReplyOK())
	{
		//LogEngineError1(_T("Toolbox command UNFREEZE failed: %s\n"), s_ReadBuffer);
		return false;
	}
	return true;
}


bool ParamSyncConnectionImp::IsToolboxReplyOK() const
{
	return strcmp(s_ReadBuffer, "OK") == 0;
}


bool ParamSyncConnectionImp::GetSingleOemParamVal(oemArray *oemArr, char view)
{
	static char query_string[256];
	bool success = true;


	
	assert(oemArr != NULL);

	/*
	 * We need to clean the buffer from previous queries
	 */
	//memset(s_ReadBuffer, 0, sizeof(s_ReadBuffer));

	if (view != '\0')
	{
		oemArr->setView(view);
	}

	oemArr->fillQueryString(query_string, sizeof(query_string), NULL, (view!='\0'));	
	size_t len = SendOemQuery(query_string, true);

	if (len == 0)
	{
		return false;    // Did not get a proper reply
	}

	//oemArr->setFromDataString(s_ReadBuffer, strlen(s_ReadBuffer));
	int retval = fill_oem_array_from_data_str(oemArr, s_ReadBuffer, (int)strlen(s_ReadBuffer));
	success = (retval == 0);
	return success;
}



int ParamSyncConnectionImp::GetOemInteger(char *name, char view)
{
	oemArray var;
	var.setOemName(name);
	this->GetSingleOemParamVal(&var, view);
	return ((int) var);
}


double ParamSyncConnectionImp::GetOemDouble(char *name, char view)
{
	oemArray var;
	var.setOemName(name);
	this->GetSingleOemParamVal(&var, view);
	return ((double) var);
}





bool ParamSyncConnectionImp::GetOemParamList(OemParamDefStruct list[], bool getA, bool getB, oemArray **var, int *nvar)
{
	bool success = true;
	// First count the number of elements in the table

	unsigned int numVarWithView = 0;
	unsigned int numVarNoView = 0;
	unsigned int numEntries = 0;

	while(list[numEntries].name != NULL)
	{
		numVarWithView +=  static_cast<int>(list[ numEntries ].withView);
		numVarNoView +=   1 - static_cast<int>(list[ numEntries ].withView);

		++numEntries;
	}
	
	assert((numVarNoView + numVarWithView) == numEntries);
	

	// Allocate the output
	*nvar = (static_cast<int>(getA) + static_cast<int>(getB))  * numVarWithView + numVarNoView;

	oemArray *lvar;
	lvar = new oemArray[*nvar];
	*var = lvar;
	char view = '\0';
	unsigned int n;
	unsigned int varCount = 0;
	for (n = 0; n<numEntries; n++)
	{
		//std::cout << " n = " << n;
		if (list[ n ].withView == false)
		{
			view = '\0';
			lvar[varCount].setOemName(list[ n ].name);
			bool retval = this->GetSingleOemParamVal(&lvar[ varCount ], view);

			++varCount;
			success = success && retval;
		}
		else   // Needs a view
		{
			if (getA == true)
			{
				view = 'A';
				lvar[varCount].setOemName(list[ n ].name);
				bool retval = this->GetSingleOemParamVal(&lvar[ varCount ], view);

				++varCount;
				success = success && retval;
			}

			if (getB == true)
			{
				view = 'B';
				lvar[varCount].setOemName(list[ n ].name);
				bool retval = this->GetSingleOemParamVal(&lvar[ varCount ], view);

				++varCount;
				success = success && retval;
			}
		}

	}

	assert( varCount == (unsigned int)(*nvar));
	return success;
}




bool ParamSyncConnectionImp::GetOemVectorArrayFromList(OemParamDefStruct list[], bool getA, bool getB, OemArrayVector *vec)
{
	bool success = true;	
	char view = '\0';
	unsigned int n = 0;

	oemArray* var;

	while(list[n].name!= NULL && success == true)
	{
		
		if (list[ n ].withView == false)
		{
			view = '\0';
			var = new oemArray(list[n].name);
			bool retval = this->GetSingleOemParamVal(var, view);
			vec->push_back(var);
			success = success && retval;
		}
		else   // Needs a view
		{
			if (getA == true)
			{
				view = 'A';
				var = new oemArray(list[n].name);
				bool retval = this->GetSingleOemParamVal(var, view);
				vec->push_back(var);
				success = success && retval;
			}

			if (getB == true)
			{
				view = 'B';
				var = new oemArray(list[n].name);
				bool retval = this->GetSingleOemParamVal(var, view);
				vec->push_back(var);
				success = success && retval;
			}
		}

		n++;
	}
	
	return success;
}



// Get the scanner (image) mode using OEM interface

ScannerMode ParamSyncConnectionImp::GetScannerModeOem()
{
	ScannerMode mode;
	
	/* Detect which views are active */


	oemArray bSplit("b_split", oemCHAR);
	oemArray bSimultaneousSplit("b_simultaneous_split", oemCHAR);

	while (this->GetSingleOemParamVal(&bSplit, '\0') == false);
	while (this->GetSingleOemParamVal(&bSimultaneousSplit, '\0') == false);

	this->GetSingleOemParamVal(&bSplit, '\0');
	this->GetSingleOemParamVal(&bSimultaneousSplit, '\0');

	

	//   View A is present when 
	//   split is off
	//   simultaneous split is on
	//   split has the value 'A'. - There is split mode, not simultaneous, and A is activated

	char b_split[10]; 
	strncpy_s(b_split,10 , bSplit.getSzString(),9);
	b_split[ 9 ] = '\0';

	char b_sim_split[10];

	strncpy_s(b_sim_split, 10, bSimultaneousSplit.getSzString(),9);
	b_sim_split[ 9 ] = '\0';



	if (!(_strnicmp(b_split, "on", 2)))
	{
		mode.b_split = true;
	}else{
		mode.b_split = false;
	}


	if (!(_strnicmp(b_sim_split, "on", 2)))
	{
		mode.simultaneous_split = true;
	}
	else
	{
		mode.simultaneous_split = false;
	}



	//  View A is present when 
	//  b_split == "off" or b_split == 'A' or simultaneous_split is on
	
	if ( mode.simultaneous_split == true || (b_split[0]!='B'))
	{
		mode.present_A = true;
	}
	else
	{
		mode.present_A = false;
	}


	// View B is activated when
	// split is equal to 'B'
	// 
	// simultaneous split is on

	if ( mode.simultaneous_split == true || (b_split[0] == 'B'))	
	{
		mode.present_B = true;
	}
	else
	{
		mode.present_B = false;
	}



	oemArray image_mode;
	oemArray compound;

	if (mode.present_A)
	{
		image_mode.setOemName("IMAGE_MODE");
		this->GetSingleOemParamVal(&image_mode, 'A');
		int mode_val = (int) image_mode;

		compound.setOemName("B_COMPOUND");
		this->GetSingleOemParamVal(&compound, 'A');
		mode.A.present_compound = (_strnicmp(compound.getSzString(), "on",2) == 0);

		
		mode.A.present_cfm = ((mode_val & MASK_MODE_C)>0);
		mode.A.present_power = ((mode_val & MASK_MODE_P)>0);         // Power Doppler
		mode.A.present_pwd = ((mode_val & MASK_MODE_D)>0);           // Pulsed Wave Doppler
		mode.A.present_cwd = ((mode_val & MASK_MODE_CW)>0);           // Continu
		mode.A.present_m = ((mode_val & MASK_MODE_M)>0);             // M-mode

	}


	if (mode.present_B)
	{
		image_mode.setOemName("IMAGE_MODE");
		this->GetSingleOemParamVal(&image_mode, 'B');
		int mode_val = (int) image_mode;

		compound.setOemName("B_COMPOUND");
		this->GetSingleOemParamVal(&compound, 'B');
		mode.B.present_compound = (_strnicmp(compound.getSzString(), "on",2) == 0);

		mode.B.present_cfm = ((mode_val & MASK_MODE_C)>0);
		mode.B.present_power = ((mode_val & MASK_MODE_P)>0);         // Power Doppler
		mode.B.present_pwd = ((mode_val & MASK_MODE_D)>0);           // Pulsed Wave Doppler
		mode.B.present_cwd = ((mode_val & MASK_MODE_CW)>0);           // Continu
		mode.B.present_m = ((mode_val & MASK_MODE_M)>0);             // M-mode
	}
	return mode;

}



bool ParamSyncConnectionImp::GetAllOemParams(OemArrayVector *vec)
{
	ScannerMode mode = this->GetScannerModeOem();
	bool success;


	success = GetOemVectorArrayFromList(gOemBmodeParamsTable, mode.present_A, mode.present_B, vec);

	if (!success)
	{
		return(success);
	}

	if (mode.A.present_cfm || mode.B.present_cfm)
	{
		success = GetOemVectorArrayFromList(gOemCpParamsTable, mode.A.present_cfm, mode.B.present_cfm, vec);
		success = success & GetOemVectorArrayFromList(gOemCmodeParamsTable, mode.A.present_cfm, mode.B.present_cfm, vec);
	}

	if (! success)
	{
		return (success);
	}

	if (mode.A.present_power || mode.B.present_power)
	{
		success = GetOemVectorArrayFromList(gOemCpParamsTable, mode.A.present_power, mode.B.present_power, vec);
		success = success & GetOemVectorArrayFromList(gOemPmodeParamsTable, mode.A.present_power, mode.B.present_power, vec);
	}
	
	if (!success)
	{
		return(success);
	}


	if (mode.A.present_pwd || mode.B.present_pwd)   //  Pulsed wave doppler
	{
		success = GetOemVectorArrayFromList(gOemPwDopParamsTable, mode.A.present_pwd, mode.B.present_pwd, vec);
	}
	if (!success)
	{
		return (success);
	}



	if (mode.A.present_cwd || mode.B.present_cwd)
	{
		success = GetOemVectorArrayFromList(gOemCwDopParamsTable, mode.A.present_cwd, mode.B.present_cwd, vec);	
	}

	if (! success)
	{
		return success;
	}


	if (mode.A.present_m || mode.B.present_m)
	{
		success = GetOemVectorArrayFromList(gOemMmodeParamsTable, mode.A.present_m, mode.B.present_m, vec);
	}

	if (!success)
	{
		return success;
	}


	success = success & GetOemVectorArrayFromList(gOemTgcParamsTable, mode.present_A, mode.present_B, vec);
	success = success & GetOemVectorArrayFromList(gOemGeneralParamsTable, mode.present_A, mode.present_B, vec);


	return success;
}



bool ParamSyncConnectionImp::CalcSapBufSizeFromSavedUseCase(int * numSamples, int *numLines, char* filename)
{
	bool success = true;

	*numSamples = 0;
	*numLines = 0;

	UseCaseParser parser(filename);
	Int32Vector activeMidLevelModes;

	success = parser.GetActiveMidlevelModes(&activeMidLevelModes);

	if (success == false)
	{
		return false;
	}


	ScanParams scanParams;
	MacroShotParams macroShotParams;
	DopParams dopParams;
	CfmParams cfmParams;



	// check for illegal modes
	unsigned n = 0;
	for (n = 0; n < activeMidLevelModes.size(); ++n)
	{
		switch(activeMidLevelModes[n])
		{
		case RTSC_MIDLEVEL_MODE_M1:		///< M Mode (1D - time-motion)
		case RTSC_MIDLEVEL_MODE_M2:		///< M Mode (1D - time-motion)
      LogPrintf("ERROR: Param Sync Connection: M-mode not implemented yet." );
			//MessageBox(NULL, "M-mode not implemented yet ", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
			success = false;
			break;

		case RTSC_MIDLEVEL_MODE_MC1:		///< M-CFM Mode (1D - time-motion)
			success = false;
      LogPrintf("ERROR: Param Sync Connection: Motion Color is not supported." );
			//MessageBox(NULL, "Motion Color is not supported", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
			break;

		case RTSC_MIDLEVEL_MODE_CWD1:	///< Continuous Doppler Mode (1D - time-motion)
      LogPrintf("ERROR: Param Sync Connection: Continuous Wave Doppler is not supported yet." );
			//MessageBox(NULL, " Continuous Wave Doppler is not supported yet ", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
			success = false;
			break;

		case RTSC_MIDLEVEL_MODE_DUM1:	///< Dummy Mode
			break;

		case RTSC_MIDLEVEL_MODE_ELASTO:  ///< Elastography
      LogPrintf("ERROR: Param Sync Connection: Elastography is not supported yet." );
			//MessageBox(NULL, " Elastography is not supported yet ", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
			success = false;
			break;

		case RTSC_MIDLEVEL_MODE_EXPMT1:  ///< Just for fun
      LogPrintf("ERROR: Param Sync Connection: Experimental Modes are not supported yet." );
			//MessageBox(NULL, "Experimental Modes are not supported yet", "Param Sync Connection", MB_ICONWARNING | MB_OK);
			success = false;
			break;

		case RTSC_MIDLEVEL_MODE_UNUSED:  ///< For unused modes in BF_XmitDataTbl[bm].MidLevelModeSel
			LogPrintf("ERROR: Param Sync Connection: Found unused Mid Level Mode." );
      //MessageBox(NULL, "Found unused Mid Level Mode", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
			success = false;
			break;
		}
	}

	if (!success)
	{
		return success;
	}


	// First add information for B-mode 	
	for (n = 0; n < activeMidLevelModes.size(); n++)
	{
		switch(activeMidLevelModes[n])
		{
		case RTSC_MIDLEVEL_MODE_B1:	     ///< B Mode (2D)
		case RTSC_MIDLEVEL_MODE_B2:		///< B Mode (2D)
		case RTSC_MIDLEVEL_MODE_B3:		///< B Mode (2D)
		case RTSC_MIDLEVEL_MODE_B4:
		case RTSC_MIDLEVEL_MODE_B5:
		case RTSC_MIDLEVEL_MODE_B6:
		case RTSC_MIDLEVEL_MODE_B7:
		case RTSC_MIDLEVEL_MODE_B8:
		case RTSC_MIDLEVEL_MODE_B9:
		case RTSC_MIDLEVEL_MODE_B10:
		case RTSC_MIDLEVEL_MODE_B11:
		case RTSC_MIDLEVEL_MODE_B12:
			//DbgPrintf("B-mode : %d \n", activeMidLevelModes[n]);
			parser.ReadUseCaseStruct("ScanParams", activeMidLevelModes[n], &scanParams, gScanParamsDef );
			*numLines += scanParams.StopLineNumQ - scanParams.StartLineNumQ + 1;
			*numSamples = __max(*numSamples,  scanParams.LineLengthRF);
			break;
		}
	}

	// Then add information about C-mode
	for (n = 0; n < activeMidLevelModes.size(); n++)
	{
		switch(activeMidLevelModes[n])
		{
		case RTSC_MIDLEVEL_MODE_C1:		///< CFM Mode (2D)
		case RTSC_MIDLEVEL_MODE_C2:      ///< CFM Mode (2D), for duplicate display
			
			parser.ReadUseCaseStruct("ScanParams", activeMidLevelModes[n], &scanParams, gScanParamsDef );
			parser.ReadUseCaseStruct("MacroShotParams", -1, &macroShotParams, gMacroShotParamsDef);
			parser.ReadUseCaseStruct("CFMParams", scanParams.CFM_Sel, &cfmParams, gCfmParamsDef );

			int numCfmLines = scanParams.StopLineNumQ - scanParams.StartLineNumQ + 1;
			if ( cfmParams.CFM_Mode == 6 )      // Transvers flow
			{  // We have double the number of lines and twice the size of the interleave factor.
				numCfmLines = numCfmLines * 2;
				macroShotParams.C_InterleaveFactor = macroShotParams.C_InterleaveFactor * 2;
			}
			int maxNumCfmBlocks = (int) ceil((double)numCfmLines / (double)macroShotParams.C_InterleaveFactor) + 1;
			int cfmBlockLen = macroShotParams.ShotsPerEstimate * macroShotParams.C_InterleaveFactor;

			*numLines += cfmBlockLen * maxNumCfmBlocks;
			*numSamples = __max(*numSamples, scanParams.LineLengthRF);
			break;
		}
	}

	// Then add information about Doppler

	for (n = 0; n < activeMidLevelModes.size(); n++)
	{
		switch(activeMidLevelModes[n])
		{
		case RTSC_MIDLEVEL_MODE_PWD1:	///< Pulsed Doppler Mode (1D - time-motion)
			parser.ReadUseCaseStruct("ScanParams", activeMidLevelModes[n], &scanParams, gScanParamsDef );
			parser.ReadUseCaseStruct("MacroShotParams", -1, &macroShotParams, gMacroShotParamsDef);
			parser.ReadUseCaseStruct("DopParams", scanParams.DOP_Sel, &dopParams, gDopParamsDef );
			
			int pwdBlockLen = macroShotParams.SHOT_T;
			int numPwdBlocks = (int)ceil(((double)*numLines) / (double)(macroShotParams.SKIP_T+1));
			//DbgPrintf("%s (Line %d) \n", __FUNCTION__, __LINE__);
			//DbgPrintf("numPwdBlocks : %d \n", numPwdBlocks);
			//DbgPrintf(" pwdBlockLen : %d \n", pwdBlockLen);

			*numLines = *numLines  + numPwdBlocks*pwdBlockLen;
			break;

		}
	}
	return success;

}

//

bool ParamSyncConnectionImp::CalcSapBufSizeFromLatestUseCase(int * numSamples, int *numLines)
{
	
	char *useCaseBuf = NULL;
	bool success = this->GetConsoleLatestCalcOKUseCase(&useCaseBuf);

	*numSamples = 0;
	*numLines = 0;

	if (!success)
	{
		return false;
	}

	FILE *fid;
	fid = fopen("./LatestUseCase.dat", "wb");


	if (fid == NULL)
	{
		return false;  
	}

	fwrite(useCaseBuf, strlen(useCaseBuf), 1, fid);

	fclose(fid);


	success = this->CalcSapBufSizeFromSavedUseCase(numSamples, numLines, "./LatestUseCase.dat");

	return success;
}




bool ParamSyncConnectionImp::CalcSapBufSizeUsingToolbox(int *numSamples, int* numLines)
{
	bool success;

	char * useCaseBuf;

	success = this->GetUseCase(&useCaseBuf);
	
	if (!success)
	{
		return false;
	}


	FILE *fid;
	fid = fopen("./ToolboxUseCase.dat", "wb");


	if (fid == NULL)
	{
		return false;  
	}

	fwrite(useCaseBuf, strlen(useCaseBuf), 1, fid);

	fclose(fid);

	success = this->CalcSapBufSizeFromSavedUseCase(numSamples, numLines, "./ToolboxUseCase.dat");

	return success;


}




bool  ParamSyncConnectionImp::CalcSapBufSizeUsingConsole(int *numSamples, int *numLines)
{
	*numLines = 0;
	*numSamples = 0;
	
	// ::MessageBox(NULL, "Function not implemented yet", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
	ScannerMode mode = this->GetScannerModeOem();
	
	if (mode.A.present_cwd || mode.B.present_cwd)
	{
    LogPrintf("ERROR: Param Sync Connection: Continuous Wave Doppler not supported yet." );
		//MessageBox(NULL, "Continuous Wave Doppler not supported yet", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
		return false;
	}

	if (mode.A.present_m || mode.B.present_m)
	{
    LogPrintf("ERROR: Param Sync Connection: M-mode not supported yet." );
		//MessageBox(NULL, "M-mode not supported yet", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
		return false;
	}


	oemArray var;

	
	// Get the B-mode first
    if (mode.present_A)
	{
		// Get first the number of lines
		int numAngles = 1;
		
		if (mode.A.present_compound)
		{
			LogPrintf("Compounding is on. Unknown number of angles. Assuming 5 angles \n");
			numAngles = 5;
		}
		var.setOemName("b_scanlines_count");
		this->GetSingleOemParamVal(&var, 'A');

		*numLines += numAngles*(this->GetOemInteger("b_scanlines_count", 'A'));
		*numSamples = __max( *numSamples, (this->GetOemInteger("b_rf_line_length", 'A')) );
		DbgPrintf("view A, Bimage numAngles : %d\n", numAngles);
		DbgPrintf("view A, Accumulated numLines : %d\n", *numLines);
	}

	if (mode.present_B)
	{
		// Get first the number of lines
		int numAngles = 1;
		
		if (mode.B.present_compound)
		{
			LogPrintf("Compounding is on. Unknown number of angles. Assuming 5 angles \n");
			numAngles = 5;
		}
		*numLines += numAngles*(this->GetOemInteger("b_scanlines_count", 'B'));
		*numSamples = __max(*numSamples, (this->GetOemInteger("b_rf_line_length", 'B')));

		DbgPrintf("view B, Bimage numAngles : %d\n", numAngles);
		DbgPrintf("view B, Accumulated numLines : %d\n", *numLines);

	}

    // Add CFM info


	if (mode.A.present_cfm || mode.B.present_cfm || mode.A.present_power || mode.B.present_power)
	{
		int interleaveFactor = 0;
		int shotsPerEstimate = 0;
		int numCpLines = 0;

		interleaveFactor = this->GetOemInteger("cp_interleave_factor", '\0');   // 1 CFM only. No need for view. 
		                                                                         // View is reported back.

		numCpLines = this->GetOemInteger("cp_scanlines_count", '\0');

		if (mode.A.present_cfm || mode.B.present_cfm)
		{
			shotsPerEstimate = this->GetOemInteger("c_shots_per_estimate", 0);
		}
		else
		{
			shotsPerEstimate = this->GetOemInteger("p_shots_per_estimate", 0);
		}

		if (mode.A.present_cfm || mode.B.present_cfm)
		{
			
			oemArray var;
			var.setOemName("c_sub_mode");
			this->GetSingleOemParamVal(&var, '\0');
			
			char * subMode = var.getSzString();

			if (!_strcmpi(subMode, "VELOCITY") || !_strcmpi(subMode, "VAR+VEL") || !_strcmpi(subMode, "VARIANCE"))
			{
				interleaveFactor = interleaveFactor;
			}
			else
			{
				DbgPrintf("Assuming vector flow. Doubling the interleave factor and the number of lines. May change in the future");
					interleaveFactor = interleaveFactor * 2;
					numCpLines = numCpLines * 2;
			}
			
		}
		
		int maxNumBlocks = (int) ceil((double)numCpLines / (double)interleaveFactor) + 1;
		int blockLen = shotsPerEstimate * interleaveFactor;

		*numLines += blockLen * maxNumBlocks;
	}

	if (mode.A.present_pwd || mode.B.present_pwd)
	{
		// This is really serious
    LogPrintf("ERROR: Param Sync Connection: Pulsed Wave Doppler not well supported by OEM. You should use UseCases instead. Assuming that every second shot is doppler shot." );
		//MessageBox(NULL, "Pulsed Wave Doppler not well supported by OEM. You should use UseCases instead.\n Assuming that every second shot is doppler shot.", "Param Sync Connection", MB_ICONWARNING | MB_OK); 
		*numLines = 2 * (*numLines);
	}
	return true;
}


///
bool ParamSyncConnectionImp::GetFileFromScanner(char* remoteName, char* localName)
{
	memset(s_ReadBuffer, 0, READ_BUFFER_SIZE);


	size_t querylen = strlen(remoteName) + 32;
	char* query = new char[querylen];
	memset(query, 0, querylen);


	sprintf_s(query, querylen, "GET_USECASE \"%s\"", remoteName);
	size_t anslen = SendToolboxCommand(query, true);
	delete [] query;

	
	int size;
	if(sscanf_s(s_ReadBuffer, "OK #%d", &size) != 1)
		return false;

	// Skip header
	char* p = s_ReadBuffer + 4; // Skip "OK #"
	for(; *p >= '0' && *p <= '9'; p++) // Skip size spec
		;
	

	size_t bytesToSave = anslen - ((size_t) p - (size_t) s_ReadBuffer);
	FILE *fid;
	fid = fopen(localName, "wb");

	if (fid == NULL)
	{
		return false;  
	}

	fwrite(p, bytesToSave, 1, fid);
	fclose(fid);

	
	return true;
}


