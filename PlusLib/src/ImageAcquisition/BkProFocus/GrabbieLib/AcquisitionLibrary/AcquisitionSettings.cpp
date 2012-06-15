#include <Windows.h> // for MAX_PATH
#include <assert.h>
#include <sstream>
#include <tchar.h> // for _T

#include "AcquisitionSettings.h"


/// <summary> The default number of frames in the grabber buffer. </summary>
static const size_t DefaultGrabberBufferFrames = 128u;

AcquisitionSettings::AcquisitionSettings() 
    : comportNumber(-1), baudRate(-1), ccfFile(_T("")), comportName(_T("")), setupCommands(_T(""))
    , framesToGrab(0), adjacentFrames(1), discardFrames(0), framesInGrabberBuffer(DefaultGrabberBufferFrames)
    , linesPerFrame(0), samplesPerLine(0)
{
}

AcquisitionSettings::~AcquisitionSettings()
{
}

bool AcquisitionSettings::LoadGrabberSettings(int newAdjacentFrames, int newDiscardFrames, int newFramesToGrab, int newFramesInBuffer)
{
    this->adjacentFrames = newAdjacentFrames;
    this->discardFrames = newDiscardFrames;
    this->framesToGrab = newFramesToGrab;
    this->framesInGrabberBuffer = DefaultGrabberBufferFrames;
    if(newFramesInBuffer > 0)
    {
        this->framesInGrabberBuffer = newFramesInBuffer;
    }

    // ensure validity of loaded data
    assert(this->adjacentFrames >= 0);
    assert(this->discardFrames >= 0);
    assert(this->framesToGrab >= 0);
    assert(this->framesInGrabberBuffer > 0);

    return true;
}


bool AcquisitionSettings::LoadIni(std::string fileName)
{
    // investigate if iniFile contain the full path or the relative path
    std::string iniFileAbsolutePath;
    if(!RelativePathToAbsolutePath(fileName, &iniFileAbsolutePath))
    {
        // getting absolute path failed
        return false;
    }
    
    size_t idx = iniFileAbsolutePath.find_last_of("/\\");    
    std::string filePath = iniFileAbsolutePath.substr(0,idx); // directory name without a trailing slash
    char ccfFileBuffer[MAX_PATH];
    char comportNameBuffer[MAX_PATH];
    char setupCommandsBuffer[MAX_PATH];

    // load settings from ini - consider optimizing by searching the ini manually
    comportNumber = GetPrivateProfileInt("RESEARCH_INTERFACE", "comport", -1, iniFileAbsolutePath.c_str());
    baudRate =  GetPrivateProfileInt("RESEARCH_INTERFACE", "baud_rate", 9600, iniFileAbsolutePath.c_str());
    GetPrivateProfileString("RESEARCH_INTERFACE", "ccf_file", "IniFile.ccf", ccfFileBuffer, MAX_PATH, iniFileAbsolutePath.c_str());
    GetPrivateProfileString("RESEARCH_INTERFACE", "comport_name", "\\\\.\\X64-CL_Express_1_Serial_1", comportNameBuffer, MAX_PATH, iniFileAbsolutePath.c_str());
    GetPrivateProfileString("RESEARCH_INTERFACE", "setup_commands", "CLE;LID 1;FLT 15;TAF", setupCommandsBuffer, MAX_PATH, iniFileAbsolutePath.c_str());
    comportName = std::string(comportNameBuffer);
    setupCommands = std::string(setupCommandsBuffer);

    // compute absolute path to ccf file if relative path is specified
    ccfFile = std::string(ccfFileBuffer);

    // if path contains ':', it contains a drive, so it must be an absolute path
    if (ccfFile.find(':')==std::string::npos)
    {
      // ':' not found, append root path
      ccfFile = filePath + "\\" + ccfFile; 
    }

    return true;
}

bool AcquisitionSettings::SaveToIniFile(char * UseCaseName)
{
	char SectionName[] = "RESEARCH_INTERFACE";

	std::string str;

  std::ostringstream comPortNumberString; 
  comPortNumberString << this->comportNumber << std::ends; 
	WritePrivateProfileString(SectionName, "comport", comPortNumberString.str().c_str(), UseCaseName);

  std::ostringstream baudRateString; 
  baudRateString << this->baudRate << std::ends; 
	WritePrivateProfileString(SectionName, "baudRate", baudRateString.str().c_str(), UseCaseName);

  WritePrivateProfileString(SectionName, "ccf_file", this->ccfFile.c_str(), UseCaseName);
	WritePrivateProfileString(SectionName, "comport_name", this->comportName.c_str(), UseCaseName);
	WritePrivateProfileString(SectionName, "setup_commands", this->setupCommands.c_str(), UseCaseName);

	return true;
}

int AcquisitionSettings::CalculateNumberOfAcquisitionBufferFrames() const
{
    if(discardFrames > 0)
    {
        return framesToGrab * (adjacentFrames + discardFrames);
    }
    return framesToGrab;
}

/// Utility function
bool AcquisitionSettings::RelativePathToAbsolutePath(const std::string& relativePath, std::string* absolutePath)
{
    if (relativePath.find(':')==std::string::npos)
    {
        // relative path, so find the absolute path
        TCHAR absolutePathBuffer[MAX_PATH];
        DWORD status = GetCurrentDirectory(MAX_PATH, absolutePathBuffer);
        if (status == 0)
        {
            // getting absolute path failed
            return false;
        }
        (*absolutePath)=std::string(absolutePathBuffer) + "\\" + relativePath;
    }
    else
    {
        (*absolutePath) = relativePath;
    }
    return true;
}