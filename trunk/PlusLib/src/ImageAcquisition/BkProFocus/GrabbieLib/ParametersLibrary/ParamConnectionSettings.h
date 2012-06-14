#pragma once
#include "StdAfx.h"   // For CString

static const int HOST_ADDRESS_LENGTH = 256;

class ParamConnectionSettingsImpl;

/// <summary> Place holder for connection settings. 
/// The constructor gets a name of a windows ini file.
/// If the file exists, then the fields in the class get filled from the 
/// initialization file. 
/// 
/// If the file does not exist, then the fields get populated 
/// </summary>
class ParamConnectionSettings
{

public:

	/// <summary>Constructor. If a file name is specified, and the file exists, then 
	/// the settings for the connections to the OEM interface and to the toolbox are
	/// loaded from there. If no name is given, or the file does not exist, default 
	/// settings are set. The settings are automatically written back to the ini file
	/// upon destruction of the object.
	/// 
	/// \note If a file name is not given, then a file with a name "NoName.ini" is created
	/// in the current working directory. 
	/// </summary>
	/// <param name="iniFileName">	Filename of the initialise file. </param>
	ParamConnectionSettings(CString iniFileName = "");

	char* GetScannerAddress();        ///< Returns the name of
	unsigned short GetOemPort() const;      ///< Returns the number of the TCP Port for the OEM interface
	unsigned short GetToolboxPort() const;  ///< Returns the number of the TCP Port for the Toolbox interface

	void SetScannerAddress(char* addr);        ///< Set new value for the IP address of the scanner.
	void SetOemPort(unsigned short port);      ///< Set new value for the TCP Port for the OEM interface
	void SetToolboxPort(unsigned short port);  ///< Set new value for the port where the toolbox answers
	
	/// <summary>	Loads the settings from windows ini file. </summary>
	/// <param name="fileName"> [in] Name of initialization file.</param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	/// \note The function has a side effect - it changes the file name.
	bool LoadSettingsFromIniFile(CString fileName);
	
	/// <summary>	Saves the settings to Windows Initialization file. </summary>
	/// <param name="fileName">	[in] Name of initialization file.  </param>
	/// \note The function has as a side effect to change the default
	void SaveSettingsToIniFile(CString fileName);

	/// <summary> Finaliser. The last values of OEM Port, Toolbox Port, and Scanner Address are saved to the  
	/// Initialization file. If such a file has never been specified, then a file in the working directory
	/// with a name "NoName.ini" is created. Saving of parameters can be prevented by passing
	/// save = false</summary>
	~ParamConnectionSettings();

private:
	ParamConnectionSettingsImpl* impl;   // Implementation of connection settings
};
