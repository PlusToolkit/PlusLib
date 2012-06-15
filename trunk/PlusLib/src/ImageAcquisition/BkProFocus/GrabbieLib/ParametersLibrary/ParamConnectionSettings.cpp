#include "Windows.h"
#include <sstream>

#include "ParamConnectionSettings.h"

#include <stdio.h>


/// <summary> Place holder for connection settings. 
///  Contains the possibility to initialize from a windows.ini file
/// </summary>
class ParamConnectionSettingsImpl
{
public:

	static const unsigned short DefaultOemPort = 7915;
	static const unsigned short DefaultTlbxPort = 5001;
	

private:
	char scannerAddress[HOST_ADDRESS_LENGTH];
	unsigned short oemPort;
	unsigned short tlbxPort;
	std::string iniFileName;


public:

	ParamConnectionSettingsImpl(std::string fileName="")
	{
		
		if (fileName.size() < 1)
		{
			fileName = ".\\NoName.ini";
		}

		SetIniFileName(fileName);

		if (!FileExists(fileName)){
			SetDefaultSettings();
		}else{
			LoadSettingsFromIniFile();
		}
	};


	char* GetScannerAddress() 
	{
		return scannerAddress;
	}

	unsigned short GetOemPort() const 
	{ 
		return oemPort;
	};


	unsigned short GetToolboxPort() const 
	{ 
		return tlbxPort; 
	};


	void SetScannerAddress(char* addr) 
	{ 
		memset(scannerAddress, 0, sizeof(scannerAddress));
		strncpy_s(scannerAddress, sizeof(scannerAddress), addr, strlen(addr)); 
	}; 


	void SetOemPort(unsigned short port) 
	{ 
		oemPort = port; 
	};


	void SetToolboxPort(unsigned short port) 
	{
		tlbxPort = port;
	};

	void SetIniFileName(std::string fileName) 
	{
		this->iniFileName = fileName;
	};


	std::string GetIniFileName()
	{
		return iniFileName;
	};


	~ParamConnectionSettingsImpl()
	{
		//SaveSettingsToIniFile();
		
	}


	/// <summary>	Loads the settings from windows ini file. 
	/// If no file name is given, settings are loaded from the file whose name
	/// was set with SetIniFileName.
	/// 
	/// The function has a side effect that it changes the name of the file name
	/// associated with the settings.
	///  </summary>
	/// 
	/// <param name="fileName"> [in] Name of initialization file.</param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	/// \note The function has a side effect - it changes the file name.
	bool LoadSettingsFromIniFile(std::string fileName = "")
	{
		
		if (fileName.size() > 1)
		{
			SetIniFileName(fileName);
		}

		if (!FileExists(this->iniFileName))
		{
			SetDefaultSettings();
			return false;
		}

    this->oemPort = (unsigned short) GetPrivateProfileInt("OemToolboxConnection", "OemPort", DefaultOemPort, this->iniFileName.c_str());
		this->tlbxPort =(unsigned short) GetPrivateProfileInt("OemToolboxConnection", "ToolboxPort", DefaultTlbxPort, this->iniFileName.c_str());
		GetPrivateProfileString("OemToolboxConnection", "ScannerAddress", "localhost",this->scannerAddress, sizeof(this->scannerAddress), this->iniFileName.c_str());

		return true;
	}

	/// <summary>	Saves the settings to initialise file. </summary>
	/// <param name="fileName">	[in] Name of initialization file. If no file has been passed, then the
	/// the settings are written to the file with name set by SetIniFileName </param>
	/// \note The function has as a side effect to change the default
	void SaveSettingsToIniFile(std::string fileName = "")
	{
		
		if (fileName.size() > 1)
		{
			SetIniFileName(fileName);
		}
	
		std::ostringstream oemPortString; 
    oemPortString << this->oemPort << std::ends; 
    WritePrivateProfileString("OemToolboxConnection", "OemPort", oemPortString.str().c_str(), this->iniFileName.c_str() );

    std::ostringstream tlbxPortString; 
    tlbxPortString << this->tlbxPort << std::ends; 
		WritePrivateProfileString("OemToolboxConnection", "ToolboxPort", tlbxPortString.str().c_str(), this->iniFileName.c_str());

		WritePrivateProfileString("OemToolboxConnection", "ScannerAddress", this->scannerAddress, this->iniFileName.c_str());

	}

	
	/// <summary>
	///  Sets the default settings. 
	///   oemPort = 7915
	///   tlbxPort = 7915
	///   scannerAddress = "localhost"
	///  </summary>
	void SetDefaultSettings()
	{
		this->oemPort = DefaultOemPort;
		this->tlbxPort = DefaultTlbxPort;
		memset(this->scannerAddress, 0, sizeof(this->scannerAddress));
		sprintf_s(this->scannerAddress, sizeof(this->scannerAddress), "%s", "localhost" );
	}



	/// <summary>	Queries if a given file exists. </summary>
	/// <param name="fileName">	Filename of the file. </param>
	/// <returns>	true if file exists, false if it does not . </returns>
	bool FileExists(std::string fileName)
	{
		DWORD fileAttr;
		fileAttr = GetFileAttributes(fileName.c_str());
		
		if (INVALID_FILE_ATTRIBUTES == fileAttr)
		{
			return false;
		}

		return true;
	}

};



ParamConnectionSettings::ParamConnectionSettings(std::string iniFileName)
	:	impl(new ParamConnectionSettingsImpl(iniFileName))
{
}



char* ParamConnectionSettings::GetScannerAddress()
{
	return impl->GetScannerAddress();
}


unsigned short ParamConnectionSettings::GetOemPort() const
{
	return impl->GetOemPort();
}


unsigned short ParamConnectionSettings::GetToolboxPort() const
{
	return impl->GetToolboxPort();
}


void ParamConnectionSettings::SetScannerAddress(char* addr)
{
	impl->SetScannerAddress(addr);
}


void ParamConnectionSettings::SetOemPort(unsigned short port)
{
	impl->SetOemPort(port);
}


void ParamConnectionSettings::SetToolboxPort(unsigned short port)
{
	impl->SetToolboxPort(port);
}

ParamConnectionSettings::~ParamConnectionSettings()
{
	delete impl; 
}


void ParamConnectionSettings::SaveSettingsToIniFile(std::string fileName)
{

	impl->SaveSettingsToIniFile(fileName);
}


bool ParamConnectionSettings::LoadSettingsFromIniFile(std::string fileName)
{
	return impl->LoadSettingsFromIniFile(fileName);
}



