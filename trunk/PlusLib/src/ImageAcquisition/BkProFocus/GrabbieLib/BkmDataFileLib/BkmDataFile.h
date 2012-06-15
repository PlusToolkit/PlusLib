/// \file BkmDataFile.h
///
/// <summary> Declares classes and structures that implement storage of ultrasound data in a file.
///       The "main" class used to create, write and read files is  <b> BkmDf::BkmDataFile </b> 
/// 
///        An example of how to use class is given in BkdDataFileFormat.h, section SecExampleOfUse.
///          
///          The data file starts with 4 headers:
///          \li BkmDf::FileHeader 
///          \li BkmDf::FreeTextHeader
///          \li BkmDf::GenericDataHeader
///          \li BkmDf::ModeSpecificHeader 
///          They are implemented as structure. The mode specific header can be extended via inheritance.
///          Instructions of how to extend the functionality are given in BkmDataFileFormat.h, section SecAddNewFunc
///          
/// Design document (internal BK document) 
///     SDD26705 - Software Detailed Design for BkmDataFile, a Module for Storage (Save / Load) of Ultrasound Data
/// </summary>         
#ifndef __BkmDataFile_h
#define __BkmDataFile_h    1


#include <stdint.h>         /* Platform independent description of integer types */
#include <string.h>         /* Memcpy, NULL, etc.                                */
#include <stdio.h>          /* fopen() fclose()  fwrite() fread()                */

#include "BkmDataFileFormat.h"


namespace BkmDf{

const int BKMDF_MAX_NUM_DIM = 10;                  ///< Number of dimensions in a data set
const int BKMDF_MAGIC_ID =  0x46444B42;            ///< Magic ID for the file. Spells 'BKDF'
const int BKMDF_DATA_HEADER_VER = 1;               ///< Version of the Generic Data Header
const int BKMDF_MODE_HEADER_VER = 1;               ///< Version of the Mode Specific Header
const int BKMDF_FREE_TEXT_LEN  = 8192;             ///< Length of the text in the free text description
const int BKMDF_FILE_DESCR_LEN  = 256;             ///< Length of the file description in the File Header
const int BKMDF_ERRMSGTXT_LEN  = 1024;             ///< Length of error message



#if (defined(_MSC_VER) || defined(__GNUC__))
#pragma pack(push , 1)                           // We are reading and saving binary images. Need to ensure 32-bit/64-bit compatibility.
#endif






/// <summary>	
///   Enumerates automaticallyfra supported sample values. 
///   This enumerated type must be synchronized with the values
///   found in <b> USP/DataFormat.h </b>
///</summary>
enum SampleFormat
{
	SAMPLE_FORMAT_UINT8=0,
	SAMPLE_FORMAT_UINT16,
	SAMPLE_FORMAT_UINT16X2,
	SAMPLE_FORMAT_INT8,
	SAMPLE_FORMAT_INT16,
	SAMPLE_FORMAT_INT16X2,
	SAMPLE_FORMAT_FLOAT32,
	SAMPLE_FORMAT_FLOAT32X2,
	NUM_SAMPLE_FORMATS
};



/// <summary> 
///   Use this structure to explicitly specify the number of bytes in every sample.
///   The values are stored in a static array. They are used to fill-in the 
///   number of bytes per sample in the Generic Data Header.
///   In case that the data format is custom, then both values are set to (0,0).
///   In that case, the number of bytes per sample are set explicitly from outside the
///   library.
/// </summary>
struct SampleFormatInfo
{
	int bytesPerSample;     
	int numValues;
};




/// <summary> Table with sample formats </summary>
extern SampleFormatInfo sampleFormatInfoTable[];



/// <summary>	
///   PhysBuffLoc is used to facilitate the Grab/Inject functionality from/to  
///   various memory buffers.
/// </summary>
/// 
/// \note When you add a new phys location, remember to update physBuffLocTxt table 
enum PhysBuffLoc{
	PHYS_BUFF_LOC_RXBF = 0,             ///< Receive beamformer
	PHYS_BUFF_LOC_TXBF,                 ///< Transmit beamformer
	PHYS_BUFF_LOC_RTP,                  ///< Buffer, internal to the RTP fpga
	PHYS_BUFF_LOC_ACQ,                  ///< Located in acquisition memory (attached to RTP fpga)
	PHYS_BUFF_LOC_PCINPUT,              ///< Located in PC Input buffer
	PHYS_BUFF_LOC_PCREVIEW,             ///< Located in the Review buffer in the PC
	PHYS_BUFF_LOC_ACQ_MODULE,           ///< Acquisition module in the PC (requires research interface)
    NUM_PHYS_BUFF_LOCATIONS
};


/// <summary>  Table with text description of the physical locations </summary>
extern const char* physBuffLocTxt[];


/// <summary>	
///   Mode type description. This is intended to give some 
///   context information about the data. E.g. - B-mode, CFM, PW Doppler, etc.
/// </summary>
///
/// \note When you add a mode type, remember to add text description in imgModeTypeTxt 
enum ImgModeType{
	MODE_TYPE_AMODE = 0,       ///< Single scan line as a function of time
	MODE_TYPE_BMODE,           ///< An image. The signal amplitude is shown as brightness.
	MODE_TYPE_MMODE,           ///< M mode. Repeatedly scanned single scan line as a function of time
	MODE_TYPE_PWDOPPLER,       ///< Pulsed Wave Doppler. Normally displayed as a spectrogram
	MODE_TYPE_CFM,             ///< Color flow mapping. Color-coded image of the blood velocity.
	MODE_TYPE_VFI,             ///< Flow map. Each estimate is a vactor with a magnitude and direction.
	MODE_TYPE_ELASTO,          ///< Free-hand elasticity image. Color coded. 
	MODE_TYPE_IRRELEVANT,      ///< A new scan not described above
	NUM_MODE_TYPES
};


/// <summary> Table with text description of imaging modes </summary>
extern const char* imgModeTypeTxt[];






/// <summary>
///   Global File Header  structure. Contains pointers to the headers and to the data in the file.
///   The structure of the file header is illustrated in the figure below
///  
///  \image html headersFileHeader.png
///  \image latex headersFileHeader.pdf
///  
///  </summary>
struct FileHeader{
	int32_t magicID;               ///< A number that will identify the file to the operating system
	int32_t len;                   ///< Length of FileHeader in bytes
	int32_t numFrames;             ///< Number of frames in the file. The header is updated just before closing the file.
	int32_t textHeaderOffset;      ///< Offset from the start of file to the Free Text Header
	int32_t dataHeaderOffset;      ///< Offset from start of file to the Generic Data Header in bytes
	int32_t modeHeaderOffset;      ///< Offset from start of file to the Mode Specific Header in bytes
	int32_t dataOffset;            ///< Offset from start of file to start of data in bytes
	char Description[BKMDF_FILE_DESCR_LEN];  ///< Description of the file

	FileHeader();                  ///< Default constructor. Calls Reset()
    /// <summary>
    /// Create a human-readable c-string with the values of the object. 
    /// All headers have a ToString() method. 
    /// Example:
    /// <code>
    ///   char demoStr[256];
    ///   FileHeader theHeader;
    ///   printf("The Header\n%s", theHeader.ToString(demoStr, 256));
    /// </code>
    /// </summary>
    /// <param name="str">      [out] String buffer </param>
    /// <param name="strLen">	Length of the string buffer. </param>
    /// <returns>	\c str - a pointer to the location (same as the input str). </returns>
	char * ToString(char* str, size_t strLen); 

    /// <summary>   Clears all variables. Sets magicID and len to their values. </summary>
	void Reset();

};





/// <summary>
///   Data in the file is typically the result of scanning and is multi-dimensional data.
///   Instead of fixing the names of the dimensions and the order in which data is saved
///   (e.g. samples, lines, planes etc.) we will operate with the term "dimension".
///   The only requirement is that the elements along dim[0] are saved one after the other in the 
///   file, then the elements of dim[1], and so on. 
/// </summary>
struct DimInfo{
	int32_t headerLen;     ///< Header length in bytes. Offset from the first byte, where "real" data reside
	int32_t numElem;       ///< Number of elements. For example: num of samples, num of lines or num of planes.
	int32_t padLen;        ///< Number of bytes added at the end of the data for alignment purposes
};



/// <summary>	
///    Information about the frame size.  
///    A frame in the context of this file is all the data needed to reconstruct a
///    complete image on the screen at a given time instance.
///    
/// </summary>

struct FrameSizeInfo{
	int32_t frameLen;                      ///< Length of a frame in bytes. To calculate it see section \ref Sec
	SampleFormat sampleFormat;             ///< Description of the sample format
	SampleFormatInfo sampleInfo;           ///< Information about the size of a sample
	int32_t numDim;                        ///< Number of dimensions in the data set
	DimInfo dimInfo[BKMDF_MAX_NUM_DIM];    ///< Informaion about dimensions
	
	FrameSizeInfo(); 

    /// <summary>   Sets sampleFormat and sampleInfo. sampleInfo is taken from sampleFormatInfoTable . </summary>
    /// <param name="format">   Describes the format to use. </param>
	/// <returns> True upon success, false upon failure. There are no other options. </returns>
	bool SetSampleFormat(SampleFormat format);   

    /// <summary>   Sets dimensions from array. No header and no padding is assumed. </summary>
    /// <param name="numDims">  Number of dimensions. </param>
    /// <param name="dims">     Array of dimensions. </param>
    /// <returns> True upon success, false upon failure. The function fails when the number of dimensions
    ///  is less than 1 or greater than the maximum number of supported dimensions. In the latter case, the
    ///  length of the frame is set to 0 too. 
    ///   </returns>
	bool SetDimNoHeaderNoPad(int numDims, int dims[]);
};



/// <summary>	
///    Generic description of data in the file. This header should be sufficient to 
///    extract a single sample out of the file. The structure of the header is
///    shown in the image
///    
///    \image html headersGenericDataHeader.png
///    \image latex headersGenericDataHeader.pdf
///    
/// </summary>
struct GenericDataHeader{
	int32_t len;                 ///< Length of header in bytes
	int32_t ver;                 ///< Version of header implementation
	FrameSizeInfo frameSize;	 ///< Information about the frame

    /// <summary>   Default constructor. Calls Reset(). </summary>
    GenericDataHeader(); 

    /// <summary>
    /// Create a human-readable c-string with the values of the object. 
    /// All headers have a ToString() method. 
    /// Example:
    /// <code>
    ///   char demoStr[256];
    ///   FileHeader theHeader;
    ///   printf("The Header\n%s", theHeader.ToString(demoStr, 256));
    /// </code>
    /// </summary>
    /// <param name="str">      [out] String buffer </param>
    /// <param name="strLen">	Length of the string buffer. </param>
    /// <returns>	\c str - a pointer to the location (same as the input str). </returns>
	char* ToString(char* str, size_t strLen);  

    /// <summary>   Clears all data specific fields. Sets the fields \c len and \c ver. </summary>
	void Reset();
};





/// <summary> 
///   Information about the location of the data in the scanner.
/// </summary>
struct UltrasoundDataInfo{
	ImgModeType modeType;            ///< Information about mode type. Needed by external software
	PhysBuffLoc physBuffLoc;         ///< Phys location of the buffer 
	int32_t buffID;                  ///< Buffer id. Needed for inject. Can be mid-level mode or some other means of ID.

    /// <summary>   Default constructor. </summary>
	UltrasoundDataInfo(); 
};


/// <summary>	
/// This header contains mode-specific information, which is needed to interpret the data
/// and to unscramble data. The mode specific header can be extended and specialized in future
/// implementations. This is the only of the 4 headers that can be inherited. The member functions
/// are virtual. Instructions of how to calculate the length of the header 
/// are given in \ref SecAddNewFunc (see file BkmDataFileFormat.h)
/// 
/// 
///  \par
///  The layout of ModeSpecificHeader is given in the following image:
///  
///  \image html headersModeSpecificHeader.png
///  \image latex headersModeSpecificHeader.pdf  
///   
/// </summary>
struct ModeSpecificHeader{
	int32_t len;                     ///< Header length in bytes.
	int32_t ver;                     ///< Version of header implementation
	UltrasoundDataInfo dataInfo;     ///< Information regarding the ultrasound data.

    /// <summary>
    /// Create a human-readable c-string with the values of the object. 
    /// All headers have a ToString() method. 
    /// Example:
    /// <code>
    ///   char demoStr[256];
    ///   FileHeader theHeader;
    ///   printf("The Header\n%s", theHeader.ToString(demoStr, 256));
    /// </code>
    /// </summary>
    /// <param name="str">      [out] String buffer </param>
    /// <param name="strLen">	Length of the string buffer. </param>
    /// <returns>	\c str - a pointer to the location (same as the input str). </returns>
	virtual char * ToString(char* str, size_t strLen);

    /// <summary>   Clears all data specific fields. Sets the fields \c len and \c ver. </summary>
	virtual void Reset(); 

	/// <summary>
	///    Fills-in the values of the data member dataInfo from ultrasoundDataInfo. Overrided versions
	///    of this function must implement the mode-specific functionality
	/// </summary>
	/// <param name="ultrasoundDataInfo">	Information describing the ultras data. </param>
	virtual void InitializeFrom(const UltrasoundDataInfo& ultrasoundDataInfo); 

	/// <summary>
	///  Returns a pointer to the first valid data element. Used by the WriteHeaders() and ReadHeaders()
	///  functions of the class BkmDataFile. A binary image is saved to the disk. The start address
	///  is the address returned by GetPtr. The number of bytes stored is the value contained by the 
	///  \c len data member. This function is necessary, because the structure contains virtual
	///  functions, and there is a \c this pointer at the start of the memory allocated for the respective object.
	/// </summary>
	/// <returns>	null if it fails, else the pointer. </returns>
	void* GetPtr() { return (void*)&len;};

    /// <summary>   Default constructor. Calls Reset(). </summary>
	ModeSpecificHeader();

    /// <summary>   Constructor. Calls Reset() and InitializeFrom(). </summary>
    /// <param name="ultrasoundDataInfo">   [in,out] Information describing the ultrasound data. </param>
	ModeSpecificHeader(UltrasoundDataInfo& ultrasoundDataInfo);
	
};





/// <summary>	
///  Free text header is written at the start of the file so that the user/developer
///  can inspect visually the file using a text editor.
///  The layoyut of the FreeTextHeader is shown in the figure below:
///  
///  \image html headersFreeTextHeader.png
///  \image latex headersFreeTextHeader.pdf
///  
/// </summary>
struct FreeTextHeader{
	int32_t len;                          ///< Length of the header. Must be BKMD_FREE_TEXT_LEN + sizeof(int32_t)
	char freeText[BKMDF_FREE_TEXT_LEN];   ///< Text buffer where the headers can write their free-text description

    /// <summary>
    /// Create a human-readable c-string with the values of the object. 
    /// All headers have a ToString() method. 
    /// Example:
    /// <code>
    ///   char demoStr[256];
    ///   FileHeader theHeader;
    ///   printf("The Header\n%s", theHeader.ToString(demoStr, 256));
    /// </code>
    /// </summary>
    /// <param name="str">      [out] String buffer </param>
    /// <param name="strLen">	Length of the string buffer. </param>
    /// <returns>	\c str - a pointer to the location (same as the input str). </returns>
	char * ToString(char* str, size_t strLen);

    /// <summary>   Default constructor. Calls Reset(). </summary>
	FreeTextHeader();

    /// <summary>  Sets len. Fills freeText with '\0'. </summary>
	void Reset(); 
};



/// <summary>	Create (allocate) Mode Specific Headers.  </summary>
class ModeSpecificHeaderFactory
{
public:

	/// <summary>
	///  Creates a mode specific header. The type of header created depends
	///  on the input imgModeType. The created objects are of classes
	///  inheriting the base class ModeSpecificHeader, which is also 
	///  the default output.
	///  
	///  When you add a new Mode Specific Header, you must also modify
	///  this function and add a statement to allocate the new header.
	///  
	///  \note The function is called from BkmDataFile::OpenCreate and BkmDataFile::OpenReadWrite.
	/// </summary>
	/// <param name="imgModeType">	Type of the image mode. </param>
	/// <returns>	null if it fails, else. </returns>
	static ModeSpecificHeader* CreateModeSpecificHeader(ImgModeType imgModeType);
};



///<summary> 
///    Utility functions for Frame Size. This is a singleton. Consists of static functions and a single
///    static variable. 
///    
///    The functions fall into two categories:
///    \li Validation
///    \li Setup
///    The validation functions check if certain fields are within the valid ranges. They return true or
///    false. Use GetLastErrTxt() to get a text-description for the cause of validation failure.
///    
///    The setup functions are used to set default values or to calculate certain parameters based on
///    the frame size and dimensions.
///    
///</summary>
class FrameDimUtil{
public:

	/// <summary> Checks that the number of dimensions is within range, and that every dimension is positive
	///     and different than 0. </summary>
	/// <param name="frame"> Structure with definitions of the frame size. </param>
	/// <returns>true if the values of the structure are valid, false if not.  </returns>
	static bool ValidDims(const FrameSizeInfo& frame);

	/// <summary> Checks if the dimensions are valid, and if the calculated size is correct. </summary>
	/// <param name="frame"> Structure with definitions of the frame size. </param>
	/// <returns>true if the values of the structure are valid, false if not.  </returns>
	static bool ValidFrameSize(const FrameSizeInfo& frame);

	/// <summary> Finds what is the length of a frame measured in bytes</summary>
	/// <param name="frame"> Structure with definitions of the frame size. </param>
	/// <returns> The calculated frame size. If the dimensions are not valid, the function returns <b> 0 </b> </returns>
	static int32_t CalcFrameSize(const FrameSizeInfo& frame);

	/// <summary>	Calculates the pitch of the data set along all dimensions and writes them in an array. </summary>
	/// <param name="pitch">    [out] Array holding the pitch </param>
	/// <param name="numPitch">	 Length of the array pitch </param>
	/// <param name="size">   	[in] Structure with definitions of the frame size. </param>
	/// <returns> The number of dimensions for which the pitch was calculated. Returns -1 upon failure </returns>
	static int CalcPitch(int* pitch, const int numPitch, const FrameSizeInfo& size );


	/// <summary>	Calculates the length of the data along dimension 0. 
	/// 			This length can be used in memcpy() operations
	/// </summary>
	/// <param name="size">	[in] Structure with definitions of the frame size. </param>
	/// <returns>	The calculated length dim 0. If the number of dimensions is less than 1, -1 is returned (failure). </returns>
	static int CalcLenDim0(const FrameSizeInfo& size);

	/// <summary> Utility function. Sets default values for saving complex 16-bit IQ (in-phase and quadrature-phase) samples.
	///  Assumes no padding. The header is 2 16-bit values, i.e. 8 bytes long. Matches the data format of data sent over
	///  the research interface. </summary>
	/// <param name="frameSize">  [out] Data structure to hold the settings. </param>
	/// <param name="numSamples"> Number of samples in a line. </param>
	/// <param name="numLines"> Number of lines in a frame. </param>
	static void SetDefaultBmodeIQdataUltraView(FrameSizeInfo* frameSize, int numSamples, int numLines);

	/// <summary>Clears any error messages </summary>
	static void ResetLastError();

	/// <summary>	Returns a pointer to the last error message. </summary>
	/// <returns>	null if it fails, else the last error text. </returns>
	static char* GetLastErrTxt() {return lastError;}

private:
	static char lastError[BKMDF_ERRMSGTXT_LEN]; ///< Buffer for the last error message
};


/// <summary>
///   Utility functions to manipulate contents of the UltrasoundDataInfo structures.
///   The functions fall primarily into two categories:
///   Validation and Default Settings. 
///   
///   The validation functions return true or false. More information about why the 
///   validation has failed, is stored in the private variable lastError.  Access to 
///   lastError is provided by GetLastErrTxt() member function.
/// </summary>
class UltrasoundDataUtil{
public:

	/// <summary>	Checks if ultrasoundDataInfo.PhysBuffLoc is within the limits of the enumerated values. </summary>
	/// <param name="ultrasoundDataInfo">	Description of ultrasound data. </param>
	/// <returns>	true if valid, false if not. </returns>
	static bool ValidLocation(const UltrasoundDataInfo& ultrasoundDataInfo);   

	/// <summary>	Checks if ultrasoundDataInfo.modeType is within the limits of the enumerated values. </summary>
	/// <param name="ultrasoundDataInfo">	Description of ultrasound data. </param>
	/// <returns>	true if valid, false if not. </returns>
	static bool ValidMode(const UltrasoundDataInfo& ultrasoundDataInfo);

	/// <summary> Calls the rest of the validation functions. If one of them fails, then all fails </summary>
	/// <param name="ultrasoundDataInfo">	Description of ultrasound data. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	static bool ValidUltrasoundDataInfo(const UltrasoundDataInfo& ultrasoundDataInfo);

	/// <summary> Clears any error messages </summary>
	static void ResetLastError();

	/// <summary> Returns a pointer to the buffer with text describing the last encountered error </summary>
	static char* GetLastErrTxt() {return lastError;}

	/// <summary> Sets a bmode input buffer. Sets the mode to B-mode, 
	/// 		  the physical location to the PC input buffer. 
	/// 		  Finally assigns the buffID to the input variable buffID.
	/// </summary>
	/// <param name="ultrasoundDataInfo">	[out] Description of ultrasound data. </param>
	/// <param name="buffID">		 	Identifier for the buffer. </param>
	static void SetBmodeInputBuff(UltrasoundDataInfo* ultrasoundDataInfo, int buffID);

private:
	static char lastError[BKMDF_ERRMSGTXT_LEN];   ///< Placeholder for the text describing the last encountered error
};






/// <summary> 
///  BkmDataFile is used to store ultrasound data collected by
///  the research tools of BK Medical Aps. This class encapsulates methods
///  and data structures to open/close append and read frames of ultrasound data
///  in the data file.
///  
///  The format of the file is described in the include file BkmDataFileFormat.h
///  
///  An example of how to use the file is given in \ref SecExampleOfUse in BkmDataFileFormat.h
///  
///  
///  </summary>
class BkmDataFile{

public:
	/// <summary>	
	///   Default constructor.  
	///   Resets all member variables.
	///  </summary>
	BkmDataFile();
	
	/// <summary>	Destructor. Flushes data. Closes opened files.  </summary>
	~BkmDataFile();
	
	/// <summary>	Creates a new file </summary>
	/// <param name="filename">   Name of the file. </param>
	/// <param name="frameSize">  Size of a single frame. </param>
	/// <param name="ultraData">  Information describing the ultrasound data </param>
	/// <param name="description"> Free textual description of the data/experiment for the file. 
	/// 						   User provided. Limited to 255 characters. 
	///                            No unicode support.
	/// 						   </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool OpenCreate(const char* filename, const FrameSizeInfo &frameSize,  const UltrasoundDataInfo &ultraData, const char* description = NULL);
	
	/// <summary>	Opens an existing file for reading and writing. Reads the headers. </summary>
	/// <param name="filename">	Name of the file. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool OpenReadWrite(const char* filename);

	/// <summary>	
	///   Closes the file. All unsaved data is flushed to the disk. 
	///   Re-writes the headers to update the number of frames in the file-header. 
	///   The free-text header is also updated to reflect eventual changes.
	///   Resets the headers. All fields in the headers are set to default values.
	/// </summary>
	void Close(bool flushHeaders = true);

	/// <summary>	
	///    Appends a frame. The memory pointed to by frame is 
	///    literally written to the disk without reinterpretation. 
	///    This means, that the memory pointed-to by <tt>frame</tt> 
	///    contains both data and headers, in the right order.
	///  </summary>
	/// 
	/// <param name="frame">	[in] Points to the frame </param>
	/// 
	/// <returns>	true if it succeeds, false if it fails. </returns>
	///
	bool AppendRawFrame(const void *frame);
	
	/// <summary>	
	///  Reads a frame. The function reads the binary image from the disk 
	///  to the memory location pointed-to by buf. Headers and padding are 
	///  preserved. 
	///  </summary>
	/// <param name="buf">	 [out] Pointer to buffer </param>
	/// <param name="bufLen"> 	Length of the buffer. </param>
	/// <param name="frameNo">	The frame no. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool ReadRawFrame(void* buf, int bufLen, int frameNo);

	/// <summary>
	///   Clears the location with the error message by writing zeros to it.
	/// </summary>
	void ResetErrMsgTxt() {memset(this->errMsgTxt, 0, sizeof(this->errMsgTxt));}

	/// <summary>
	///   Returns a pointer to the error message stored internally in errMsgTxt. 
	///   The error messages are in English, and are in plain ASCII.
	///   
	///   All member functions that return boolean value write a textual 
	///   description of the encountered error in errMsgTxt.
	///   
	///   <code>
	///       BkmDataFile myFile;
	///       
	///       bool success = myFile.OpenReadWrite("non_existing_file.rf")
	///       if (success == false){
	///				printf("%s\n", myFile.GetErrMsgTxt);
	///       }
	///   </code>
	/// </summary>
	/// 
	/// <returns>	A pointer to the private c-string errMsgTxt. </returns>
	const char * GetErrMsgTxt()  {return errMsgTxt;}

	/// <summary>	Returns a pointer to the File Header  </summary>
	/// <returns>	The address of the field fileHeader </returns>
	FileHeader* GetFileHeaderPtr()  {return &fileHeader; }

	/// <summary>	Returns a pointer to the Generic Data Header  </summary>
	/// <returns>	The address of the field dataHeader. </returns>
	GenericDataHeader* GetDataHeaderPtr()  {return &dataHeader; }

	/// <summary>
	///   Returns a pointer to the Mode Specific Header. 
	///   If a specialization of the ModeSpecificHeader is used, make sure to 
	///   type-cast the result to the right type
	///  </summary>
	/// <returns>	null if it fails, else the mode header pointer. </returns>
	ModeSpecificHeader* GetModeHeaderPtr()  {return modeHeader; }

	/// <summary>
	///    Returns a pointer to the Free-text header.
	///  </summary>
	/// <returns> The address of TextHeader </returns>
	FreeTextHeader* GetTextHeaderPtr()  {return &textHeader; }
	
private:

	/// <summary>	Calls the Reset() methods of the headers to initialize them with default values </summary>
	void ResetHeaders();

	/// <summary>	Initializes the headers from UltrasoundDataInfo and FrameSizeInfo structures. </summary>
	/// <param name="frameSize"> A structure with the dimensions of a single frame </param>
	/// <param name="ultraData"> A structure describing the ultrasound data - mode, buffer location in scanner etc. </param>
	/// <param name="description">	Free textual description of the data/experiment for the file.
	/// 							User provided. Limited to 255 characters. No unicode support. </param>
	/// 
	/// <returns> true if it succeeds, false if it fails. Inspect the errMsgTxt for a human-readable explanation of why the error
	///  occurred </returns>
	bool InitializeHeaders( const FrameSizeInfo &frameSize,  const UltrasoundDataInfo &ultraData, const char* description);

	/// <summary>
	///   Write a textual description of the headers into the textHeader (Free Text Header). 
	///   This function simply calls the ToString() methods of the Header classes.
	///   The Free Text Header is written immediately after the File Header and allows for
	///   visual inspection of what the file contains using a standard text editor.
	///  </summary>
	void FillInTextHeader();

	/// <summary>
	///    Reads the headers from a file. This function is called by the OpenReadWrite() method.
	/// </summary>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool ReadHeaders();

	/// <summary>
	///      Writes the headers to the data file.
	///      This function is typically called from two methods  OpenCreate() and Close()
	///      OpenCreate() writes the headers to ensure that the next write location is the right
	///      location for the beginning of the data. Close() calls the function to ensure that the
	///      information in the headers reflects the end state of the data file, for example number of frames.
	///  </summary>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool WriteHeaders();



	FILE* fid;        ///< Handle to data file
	bool opened;      ///< Flag used to detect whether a file is opened or not
	
	FileHeader fileHeader;           ///< File Header. Contains information about dataOffset and number of frames
	GenericDataHeader dataHeader;    ///< Generic Data Header. Contains information about dataset dimensions and frame size
	ModeSpecificHeader* modeHeader;  ///< Mode Specific Header. Cotains information about the imaging mode. May vary across implementations.
	FreeTextHeader textHeader;       ///< Free text description of the information contained in fileHeader, dataHeader and modeHeader
	char errMsgTxt[BKMDF_ERRMSGTXT_LEN];  ///< Contains a human-readable description of the last encountered error.

};


#if (defined(_MSC_VER) || defined(__GNUC__))
#pragma pack(pop)                           // We are reading and saving binary images. Need to ensure 32-bit/64-bit compatibility.
#endif

};   // End of name space

#endif
