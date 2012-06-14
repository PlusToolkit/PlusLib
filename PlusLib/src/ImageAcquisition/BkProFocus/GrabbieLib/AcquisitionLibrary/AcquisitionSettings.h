#pragma once

#include <list>

#include <cstring>

/// <summary>
/// The acquisition settings class contains settings used by the grabber, as well as functionality to load settings from
/// and save settings to an ini file.
/// </summary>
class AcquisitionSettings
{
public:

    /// <summary>   Default constructor. </summary>
    AcquisitionSettings();

    /// <summary>   Destructor. </summary>
    ~AcquisitionSettings();

    /// <summary>
    /// Loads settings for grabbing.
    /// </summary>
    /// <param name="newAdjacentFrames">   Number of frames in a row that are to be grabbed and injected. </param>
    /// <param name="newDiscardFrames">    Number of frames to discard between each adjacentFrames frames. </param>
    /// <param name="newFramesToGrab">     Number of frames that are to be grabbed and injected. Set to 0 to grab indefinitely. </param>
    /// <param name="newFramesInBuffer">   Number of frames to allocate in grabber buffer (if set to -1, default is chosen). </param>
    /// <returns>   true if ini and use case loaded correctly, false otherwise. </returns>
    bool LoadGrabberSettings(int newAdjacentFrames, int newDiscardFrames, int newFramesToGrab, int newFramesInBuffer);

    /// <summary>
    /// Loads information on acquisition from Capture Card and Research Interface from the CasaEngine ini
    /// file.
    /// 
    /// The ini file is expected to be the standard CasaEngine ini file. The portion used must start with
    /// the section [RESEARCH_INTERFACE]. The function loads the following parameters
    /// 
    /// * comport: Number of the com port (default is -1).
    /// * baud_rate: Baud rate (default is 9600).
    /// * ccf_file: Camera-link Configuration File (no default name). Needed by Sapera to set
    /// a number of functions. The path should be relative, but can be absolute. This parameter is
    /// required.
    /// * comport_name: Name of com port (default is \\.\X64-CL_Express_1_Serial_0
    /// escape sequences not included). If the name is not COM(?) (where (?) is a number), the full
    /// name must be given. 
    /// * setup_commands: Commands sent over the Research Interface during
    /// setup (default is CLE,LID 1, FLT 1, TAF). At most 256 chars are supported. 
    /// 
    /// An ini file could contain the following:
    ///  
    /// [RESEARCH_INTERFACE] 
    /// comport=3 
    /// baud_rate=9600
    /// ccf_file=B_2202_Default_Default.ccf 
    /// comport_name=\\.\X64-CL_Express_1_Serial_0
    /// setup_commands=CLE,LID 1, FLT 1, TAF
    /// </summary>
    /// <param name="fileName"> [in] Name of initialization file.</param>
    /// <returns>   true if ini loaded correctly, false otherwise. </returns>
    bool LoadIni(CString fileName);

	/// <summary>   Saves settings to ini file. </summary>
    /// <returns>   true if ini loaded correctly, false otherwise. </returns>
	bool SaveToIniFile(char * UseCaseName);

    /// <summary>   Returns the number of the communication port loaded from the ini. </summary>
    /// <returns>   The number of the communication port. </returns>
    int GetComportNumber() const { return comportNumber; }

    /// <summary>   Returns the baud rate loaded from the ini. </summary>
    /// <returns>   The baud rate. </returns>
    int GetBaudRate() const { return baudRate; }

    /// <summary>   Returns absolute path to the ccf file loaded from the ini. </summary>
    /// <returns>   The path to the ccf file. </returns>
    CString GetCcfFile() const { return ccfFile; }

	/// <summary>   Sets absolute path to the ccf file loaded from the ini. </summary>
    /// <param name="val">  The new value. </param>
	void SetCcfFile(CString val) { this->ccfFile = val; }

    /// <summary>   Returns the name of the communication port. </summary>
    /// <returns>   The name of the communication port. </returns>
    CString GetComportName() const { return comportName; }

	/// <summary>   Sets the name of the communication port. </summary>
    /// <param name="val">  The new value. </param>
	void SetComportName(CString val) { this->comportName = val; }

    /// <summary>   Returns the setup commands to send to the research interface. </summary>
    /// <returns>   The setup commands to send to the research interface. </returns>
    CString GetSetupCommands() const { return setupCommands; }

    /// <summary>
    /// Get the number of frames to grab in the next acquisition. Set to zero to request an
    /// indefinite grab.
    /// </summary>
    /// <returns>   The frames to grab. </returns>
    int GetFramesToGrab() const { return framesToGrab; }

    /// <summary>   Set the number of frames to grab in the next acquisition. </summary>
    /// <param name="val">  The new value. </param>
    void SetFramesToGrab(int val) { this->framesToGrab = val; }

    /// <summary>   Get the number of adjacent frames to save. </summary>
    /// <returns>   The adjacent frames. </returns>
    int GetAdjacentFrames() const { return adjacentFrames; }

    /// <summary>   Set the number of adjacent frames to save. </summary>
    /// <param name="val">  The new value. </param>
    void SetAdjacentFrames(int val) { this->adjacentFrames = val; }

    /// <summary>   Get the number of frames to discard after saving the adjacent frames. </summary>
    /// <returns>   The discard frames. </returns>
    int GetDiscardFrames() const { return discardFrames; }

    /// <summary>   Set the number of frames to discard after setting the adjacent frames. </summary>
    /// <param name="val">  The value. </param>
    void SetDiscardFrames(int val) { this->discardFrames = val; }

    /// <summary>
    /// Get the number of frames to be allocated in grabber buffer. Default is 128. Note that the
    /// grabber can ignore the request.
    /// </summary>
    /// <returns>   The frames in grabber buffer. </returns>
    int GetFramesInGrabberBuffer() const { return framesInGrabberBuffer; }

    /// <summary>
    /// Set the number of frames to be allocated in grabber buffer. Default is 128. Note that the
    /// grabber can ignore the request.
    /// </summary>
    /// <param name="val">  The value. </param>
    void SetFramesInGrabberBuffer(int val) { framesInGrabberBuffer = val; }

    /// <summary>   Gets number of lines per frame. </summary>
    /// <returns>   number of lines per frame.</returns>
    /// <warning>   Invalid before it is set. </warning>
    int GetLinesPerFrame() const { return this->linesPerFrame; };

    /// <summary>   Gets the maximum expected number of lines per frame for all target streams. </summary>
    /// <param name=value> new value </param>
    void SetLinesPerFrame(int value) { this->linesPerFrame = value; };

    /// <summary>   Returns number of samples per line. </summary>
    /// <returns>   Returns number of samples per line. </returns>
    /// <warning>   Invalid before it is set. </warning>
    int GetRFLineLength() const { return this->samplesPerLine; }

    /// <summary>   Sets the number of samples per line.</summary>
    /// <param name=value> new value </param>
    void SetRFLineLength(int value) { this->samplesPerLine = value; }

    /// <summary>
    /// Given the number of frames to grab for the target, the number of frames to discard, and the
    /// number of adjacent frames to grab, this function computes the number of frames that the
    /// acquisition device will need to grab. 
    /// 
    /// The number of frames that the acquisition device needs
    /// to grab is given by framesToGrab*(adjacentFrames+discardFrames). If the user want 4 frames to
    /// be grabbed, with 1 adjacent frame and 2 frames discarded, the acquisition device will need to
    /// grab (1+2)*4 = 12 frames.
    /// </summary>
    /// <returns>
    /// The number of frames that the acquisition device must grab in order to ensure that the
    /// specified number of frames is grabbed.
    /// </returns>
    int CalculateNumberOfAcquisitionBufferFrames() const;

private:
    static bool RelativePathToAbsolutePath(const CString& relativePath, CString* absolutePath);

    /// <summary> Communication port number that communicate with research interface. Default is -1. </summary>
    int comportNumber;
    /// <summary>Baud Rate of communication link between research interface and capture card. Default is 9600. </summary>
    int baudRate;
    /// <summary> Absolute path to Camera-link Configuration File (configuration for capture card). </summary>
    CString ccfFile;
    /// <summary> Custom name of communication port (if any). Default is \\.\X64-CL_Express_1_Serial_0. </summary>
    CString comportName; 
    /// <summary> Commands sent to the research interface during setu to configure which data is sent. </summary>
    CString setupCommands;

    /// <summary> The number of frames to grab in the next acquisition </summary>
    int framesToGrab;
    /// <summary> Grab adjacentFrames after each other, e.g. grab 2 frames </summary>
    int adjacentFrames;
    /// <summary> Discard this number of frames after grabbing adjacentFrames frames, e.g. grab 2 frames and discard 5 </summary>
    int discardFrames;
    /// <summary> Specify the number of frames to be allocated by the buffer of the grabber. The grabber can ignore the value. </summary>
    int framesInGrabberBuffer;

    /// <summary> Number of lines per frame. Must be set before use. </summary>
    int linesPerFrame;
    /// <summary> Number of samples per line. Must be set before use. </summary>
    int samplesPerLine;
};