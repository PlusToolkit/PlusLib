#pragma once

#include "stdafx.h"
#include "Serial.h"
#include "AcquisitionSettings.h"

/// <summary>
/// The struct gives a convenient and consistent access to the 
/// fields of the line header used by the Research Interface. All fields
/// are unsigned ints as the compiler may align data on type 
/// boundaries. The header consists of two 16 bit fields.
/// </summary>
typedef struct RILineHeader 
{
	unsigned int  ModelID       : 3;
	unsigned int  LastLine      : 1;
	unsigned int  ZoneID        : 3;
	unsigned int  FirstZone     : 1;
    unsigned int  CFM           : 1;
    unsigned int  CFMBlockStart : 1;
    unsigned int  CFMLastLine   : 1;
    unsigned int  FFT           : 1;
	unsigned int  Unused0       : 4;
  
    
    unsigned int  LineLength    : 16; ///< Number of non-complex samples per line, header and footer not included
} ResearchInterfaceLineHeader;

/// <summary>
/// Wraps the contents of the optional line footer used by the Research
/// Interface for convenience when accessing variables. All fields
/// are unsigned ints as the compiler may align data on type 
/// boundaries. The footer consists of two 16 bit fields.
/// </summary>
typedef struct RILineFooter 
{
    unsigned int  LID           : 16;
    unsigned int  CRC           : 16;
} ResearchInterfaceLineFooter;

/// <summary>  
/// Research interface class handles communication with the research interface. It contains methods to send
/// a command to the research interface, run setup commands on the research interface as well as opening and closing
/// the connection. While it is constructed to be used as a part of the Acquisition Module, it could also be used
/// by a command line tool.</summary>
class ResearchInterface
{
public:
    ResearchInterface();
    ~ResearchInterface();

    /// <summary>  Opens a connection to the Research Interface using the specified connection settings. </summary>
    /// <param name="acquisitionSettings">  Specification of the connection settings. </param>
    /// <returns>   Status: True if it went well, and false if it failed. </returns>
    bool OpenConnection(const AcquisitionSettings& acquisitionSettings);

    /// <summary>
    /// Sets up an open connection to the Research Interface using the specified connection and default settings.
    /// </summary>
    /// <param name="acquisitionSettings">  Specification of the connection settings. </param>
    /// <returns>   Status: True if it went well, and false if it failed. </returns>
    bool SetupConnection(const AcquisitionSettings& acquisitionSettings);

    /// <summary>   Closes an open connection to the Research Interface. </summary>
    /// <returns>   Status: True if it went well, and false if it failed. </returns>
    bool CloseConnection();

    /// <summary>   Sends a command to the research interface. </summary>
    /// <param name="command">  Command to execute. </param>
    /// <param name="reply">    [out] Reply from research interface. </param>
    /// <returns>   Status: True if it went well, and false if it failed. </returns>
    bool SendCommand(const CString& command, CString* reply);

    /// <summary>
    /// Returns the latest error. After getting the error, the ResetError function should be called. 
    /// </summary>
    /// <returns>   The latest error. </returns>
    CString GetError() const;

    /// <summary>   Resets the error. </summary>
    void ResetError();

private:
    /// <summary> Serial communication with the research interface </summary>
    Serial researchInterfaceCommunicator;

    /// <summary> The most recent error message </summary>
    CString errorText;
};