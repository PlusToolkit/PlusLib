#include "BkmDataFile.h"

#include <assert.h>   // define NDEBUG to disable assertion
#include <stdio.h>
#include <stdarg.h>


using namespace BkmDf;


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)    // Some warnings associated with _CRT_SECURE (fopen instead of fopen_s)
#endif



#ifdef _MSC_VER  
/// <summary> Microsoft uses sprintf_s for secure printing to a string. C99 standard specifies snprintf.
/// 		  Instead of using a fully-fledged implementation of snprintf, a short implementation is
/// 		  provided here, which makes it possible to use snprintf with GCC and MSVC  </summary>
/// <param name="buffer">	[out] A buffer where text is written to </param>
/// <param name="count"> 	Size of buffer including place for '\0'. </param>
/// <param name="fmt">   	Format string. </param>
/// <returns> Number of successfully written characters. Negative upon failure. </returns>
static int snprintf(char *buffer, size_t count, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = _vsnprintf_s(buffer,count, count-1, fmt, ap);
	if (ret < 0)
		buffer[count-1] = '\0';
	va_end(ap);
	return ret;
}
#endif



#ifdef _MSC_VER
/// <summary> Makes sure that in MSVC, strncpy_s is used. Needed to avoid warnings .</summary>
/// <param name="dst">  	Destination string. </param>
/// <param name="src">  	Source string. </param>
/// <param name="count">	Maximum number of characters to copy. <b> count must be less or equal to sizeof(dst) </b> </param>
#define strncpy(dst, src, count)   strncpy_s(dst, count, src, count)    /* Assumes that dst is indeed at least count long */
#endif


/// <summary>
///    Collection of sizes that will be used to calculate frame size and
///    memory location (offset)	 of samples.
///    
///    This code is copied from DataFormat.cpp (USP frame work) and must
///    be synchronized with the respective code
/// </summary>
SampleFormatInfo BkmDf::sampleFormatInfoTable[NUM_SAMPLE_FORMATS] = 
{
	{ 1, 1 }, // SAMPLE_FORMAT_UINT8,
	{ 2, 1 }, // SAMPLE_FORMAT_UINT16,
	{ 4, 2 }, // SAMPLE_FORMAT_UINT16X2,
	{ 1, 1 }, // SAMPLE_FORMAT_INT8,
	{ 2, 1 }, // SAMPLE_FORMAT_INT16,
	{ 4, 2 }, // SAMPLE_FORMAT_INT16X2,
	{ 4, 1 }, // SAMPLE_FORMAT_FlOAT32,
	{ 8, 2 }, // SAMPLE_FORMAT_FlOAT32X2,
};



/// <summary> Table with text description of the physical locations  </summary>
const char * BkmDf::physBuffLocTxt[NUM_PHYS_BUFF_LOCATIONS] = {
	"Rx Beamformer",                   // PHYS_BUFF_LOC_RXBF
	"Tx Beamformer",                   // PHYS_BUFF_LOC_TXBF
	"RTP",                             // PHYS_BUFF_LOC_RTP
	"Acq. Buf",                        // PHYS_BUFF_LOC_ACQ
	"PC, In Buf",                      // PHYS_BUFF_LOC_PCINPUT
	"PC, Review Buf",                  // PHYS_BUFF_LOC_PCREVIEW
    "PC, Acq Module",                  // PHYS_BUFF_LOC_ACQ_MODULE
};




/// <summary> Table with text description of imaging modes </summary>
const char* BkmDf::imgModeTypeTxt[NUM_MODE_TYPES] = {
    "A-mode",             // MODE_TYPE_AMODE
	"B-mode",             // MODE_TYPE_BMODE
	"M-mode",             // MODE_TYPE_MMODE
	"PW Doppler",         // MODE_TYPE_PWDOPPLER
	"CFM",                // MODE_TYPE_CFM
	"VFI",                // MODE_TYPE_VFI
	"E-mode",             // MODE_TYPE_ELASTO
	"Not important",      // MODE_TYPE_IRRELEVANT
};
	



char FrameDimUtil::lastError[BKMDF_ERRMSGTXT_LEN] = "";

#ifdef _MSC_VER
#pragma region Implementation of FrameDimUtils
//    Utility functions for Frame Size. This is a singleton. Consists of static functions and a single
//    static variable. 
//    
//    The functions fall into two categories:
//    \li Validation
//    \li Setup
//    The validation functions check if certain fields are within the valid ranges. They return true or
//    false. Use GetLastErrTxt() to get a text-description for the cause of validation failure.
//    
//    The seup functions are used to set default values or to calculate certain parameters based on
//    the frame size and dimensions.
//    
#endif


// Validate the dimensions of the frame
bool FrameDimUtil::ValidDims(const FrameSizeInfo& frameSize)
{
	int n;   // Iteration variable
	

	ResetLastError();

	if (frameSize.sampleFormat < 0 || frameSize.sampleFormat >= NUM_SAMPLE_FORMATS){
		snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Sample format out of range. sampleFormat = %d. Range = (%d, %d)", frameSize.sampleFormat, SAMPLE_FORMAT_UINT8, NUM_SAMPLE_FORMATS-1);
		return false;
	}


	if ((frameSize.sampleInfo.numValues != sampleFormatInfoTable[ frameSize.sampleFormat].numValues) ||	(frameSize.sampleInfo.bytesPerSample!= sampleFormatInfoTable[ frameSize.sampleFormat].bytesPerSample)) {
		snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Wrong sample format information ( %d x %d ). Should be ( %d x %d )", 
			     frameSize.sampleInfo.bytesPerSample, frameSize.sampleInfo.numValues,
				 sampleFormatInfoTable[frameSize.sampleFormat].bytesPerSample, sampleFormatInfoTable[frameSize.sampleFormat].numValues);
		return false;
	}



	if (frameSize.numDim > BKMDF_MAX_NUM_DIM || frameSize.numDim < 1){
		snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Wrong number of dimensions : %d", frameSize.numDim);
		return false;
	}

	
	// Ensure that dimensions are positive
	for (n = 0; n < frameSize.numDim; n++){
		if (frameSize.dimInfo[n].headerLen < 0){
			snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Negative headerLen. frameSize.dimInfo[%d].headerLen = %d", n, frameSize.dimInfo[n].headerLen);
			return false;
		}

		if (frameSize.dimInfo[n].padLen < 0){
			snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Negative padLen. frameSize.dimInfo[%d].padLen = %d", n, frameSize.dimInfo[n].padLen);
			return false;
		}

		if (frameSize.dimInfo[n].numElem < 0){
			snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Negative numElem. frameSize.dimInfo[%d].numElem = %d", n, frameSize.dimInfo[n].numElem);
			return false;
		}

	}

	return true;
}



// Validates first the dimensions of the frame and then the calculated frame size
bool FrameDimUtil::ValidFrameSize(const FrameSizeInfo& frameSize)
{
	ResetLastError();

	if (ValidDims(frameSize) == false){
		return false;
	}

	// dimLen is the length of data set in bytes
	int32_t dimLen = FrameDimUtil::CalcFrameSize(frameSize);

	if (dimLen != frameSize.frameLen){
		snprintf(FrameDimUtil::lastError, sizeof(FrameDimUtil::lastError), "Field 'frameLen' is %d . Should be = %d", frameSize.frameLen, dimLen);
		return false;
	}

	return true;
}



// Returns the size of the calculated frame
int32_t FrameDimUtil::CalcFrameSize(const FrameSizeInfo& frameSize)
{
	
	if (ValidDims(frameSize) == false){
		return 0;
	}

	// dimLen is the length of data set in bytes
	int32_t dimLen = frameSize.sampleInfo.bytesPerSample;


	for (int n=0; n< frameSize.numDim; n++)
	{
		dimLen = dimLen * frameSize.dimInfo[n].numElem 
			+ frameSize.dimInfo[n].headerLen
			+ frameSize.dimInfo[n].padLen;
	}


	return dimLen;
}



int FrameDimUtil::CalcPitch(int* pitch, const int numPitch, const FrameSizeInfo& size )
{

	if (numPitch < size.numDim)
	{
		snprintf(lastError, sizeof(lastError), " Too few elements for pitch. \n");
		assert(false);
		return -1;
	}

	int copyLen = size.dimInfo[0].numElem * size.sampleInfo.bytesPerSample;
	pitch[0] =  size.dimInfo[0].headerLen + size.dimInfo[0].padLen + copyLen;

	for (int n = 1; n < size.numDim; n++)
	{
		pitch[n] = pitch[n-1] * size.dimInfo[n].numElem + size.dimInfo[n].headerLen + size.dimInfo[n].padLen;
	}

	return size.numDim;
}



int FrameDimUtil::CalcLenDim0(const FrameSizeInfo& size)
{
	assert(size.numDim > 0);
	if (size.numDim < 1)
	{
		return -1;
	}

	return (size.dimInfo[0].numElem * size.sampleInfo.bytesPerSample);
}



// Fills a FrameSizeInfo structure with default values for data captured using UltraView research interface
void FrameDimUtil::SetDefaultBmodeIQdataUltraView(FrameSizeInfo* frameSize, int numSamples, int numLines)
{
	
	frameSize->numDim = 2;
	
	frameSize->sampleFormat = SAMPLE_FORMAT_INT16X2;
	frameSize->sampleInfo = sampleFormatInfoTable[ SAMPLE_FORMAT_INT16X2 ];

	frameSize->dimInfo[0].headerLen = 4;
	frameSize->dimInfo[0].padLen = 2;      // There is a single 16-bit value with a counter for lines
	frameSize->dimInfo[0].numElem = numSamples;

	frameSize->dimInfo[1].headerLen = 0;
	frameSize->dimInfo[1].padLen = 0;
	frameSize->dimInfo[1].numElem = numLines;

	frameSize->frameLen = FrameDimUtil::CalcFrameSize(*frameSize);
}




// Clears the last error
void FrameDimUtil::ResetLastError()
{
	FrameDimUtil::lastError[0]='\0';
}




#ifdef _MSC_VER
#pragma endregion FrameDimUtils
#endif




#ifdef _MSC_VER
#pragma region Implementation of UltrasoundDataUtil
//   UltrasoundDataUtil
//  
//   Utility functions to manipulate contents of the UltrasoundDataInfo structures.
//   The functions fall primarily into two categories:
//   Validation and Default Settings. 
//   
//   The validation functions return true or false. More information about why the 
//   validation has failed, is stored in the private variable lastError.  Access to 
//   lastError is provided by GetLastErrTxt() member function.
// 
#endif


// Static variable containing text description of the last error
char UltrasoundDataUtil::lastError[BKMDF_ERRMSGTXT_LEN] = "";

// Inspects whether the location is correct
bool UltrasoundDataUtil::ValidLocation(const UltrasoundDataInfo& ultrasoundDataInfo)
{
	ResetLastError();
	if ((ultrasoundDataInfo.physBuffLoc < 0) || (ultrasoundDataInfo.physBuffLoc > (NUM_PHYS_BUFF_LOCATIONS-1))){
		snprintf(UltrasoundDataUtil::lastError, sizeof(UltrasoundDataUtil::lastError), "Invalid physBuffLoc : %d. Valid range = (%d, %d)",
			ultrasoundDataInfo.physBuffLoc, 0, NUM_PHYS_BUFF_LOCATIONS - 1);
		return false;
	}

	if (ultrasoundDataInfo.buffID < 0){
		snprintf(UltrasoundDataUtil::lastError, sizeof(UltrasoundDataUtil::lastError), "Invalid buffID : %d ", ultrasoundDataInfo.buffID );
		return false;
	}

	return true;
}



// Inspects whether the chosen mode is correct
bool UltrasoundDataUtil::ValidMode(const UltrasoundDataInfo& ultrasoundDataInfo)
{
	ResetLastError();
	if ((ultrasoundDataInfo.modeType < 0) || (ultrasoundDataInfo.modeType > (NUM_MODE_TYPES-1))){
		snprintf(UltrasoundDataUtil::lastError, sizeof(UltrasoundDataUtil::lastError), "Invalid modeType : %d. Valid range = (%d, %d)",
			ultrasoundDataInfo.modeType, 0, NUM_MODE_TYPES - 1);
		return false;
	}
	return true;

}


// Inspects all fields of UltrasoundDataInfo structure
bool UltrasoundDataUtil::ValidUltrasoundDataInfo(const UltrasoundDataInfo& ultrasoundDataInfo)
{
	if (ValidMode(ultrasoundDataInfo) == false){
		return false;         // Error message has been filled by Valid mode
	}

	if (ValidLocation(ultrasoundDataInfo) == false){
		return false;  // Error message is filled by ValidLocation()
	}

	return true;
}

// Resets last erro
void UltrasoundDataUtil::ResetLastError()
{
	UltrasoundDataUtil::lastError[0] = '\0';
}

// Sets the UltrasoundDataInfo. Source - PC Input buffer. Type - Bmode. Finally buffID is assigned
void UltrasoundDataUtil::SetBmodeInputBuff(UltrasoundDataInfo* ultrasoundDataInfo, int buffID)
{
	ultrasoundDataInfo->modeType = MODE_TYPE_BMODE;
	ultrasoundDataInfo->physBuffLoc = PHYS_BUFF_LOC_PCINPUT;
	ultrasoundDataInfo->buffID = buffID;
}


#ifdef _MSC_VER
#pragma endregion Implementation of UltrasoundDataUtil
#endif





FrameSizeInfo::FrameSizeInfo() 
{
	this->sampleFormat = SAMPLE_FORMAT_UINT8;     // Description of the sample format
	this->sampleInfo = sampleFormatInfoTable[SAMPLE_FORMAT_UINT8];           
	this->numDim = 0;                             // Number of dimensions
	for (int n = 0; n < BKMDF_MAX_NUM_DIM; n++) {
		dimInfo[n].headerLen = 0;   
		dimInfo[n].numElem = 0;
		dimInfo[n].padLen = 0;
	}
	this->frameLen = FrameDimUtil::CalcFrameSize(*this);
}



bool FrameSizeInfo::SetSampleFormat(SampleFormat format)
{
	bool success = false;

	assert(format >= 0 && format < NUM_SAMPLE_FORMATS);

	if (format >= 0 && format < NUM_SAMPLE_FORMATS){
		this->sampleFormat = format;
		this->sampleInfo = sampleFormatInfoTable[ format ];
		success = true;
	}

	return success;
}


// The function returns true if dimensions are valid, otherwise false. 
// When dimensions are wrong, the length of the frame is set to 0
bool FrameSizeInfo::SetDimNoHeaderNoPad(int numDims, int dims[])
{
	bool success = false;

	assert(numDims > 0 && numDims <= BKMDF_MAX_NUM_DIM);

	if (numDims > 0 && numDims <= BKMDF_MAX_NUM_DIM){  // Assert is inactive when NDEBUG is defined !!!
		this->numDim = numDims;
		for (int n = 0; n < numDims; n++){
			dimInfo[n].numElem = dims[n];
			dimInfo[n].headerLen = 0;
			dimInfo[n].padLen = 0;
		}
		success = true;
	}
	this->frameLen = FrameDimUtil::CalcFrameSize(*this);   // Will return zero when dimensions are not valid !!!
	return success;
}




UltrasoundDataInfo::UltrasoundDataInfo()
{  // Just to have some valid inputs
	this->buffID = 0;
	this->modeType = MODE_TYPE_BMODE;
	this->physBuffLoc = PHYS_BUFF_LOC_PCREVIEW;
}





ModeSpecificHeader* ModeSpecificHeaderFactory::CreateModeSpecificHeader(ImgModeType imgModeType)
{
	ModeSpecificHeader *header = NULL;

	switch(imgModeType){
	case MODE_TYPE_AMODE:
		header = new ModeSpecificHeader;
		break;

	case MODE_TYPE_BMODE:
		header = new ModeSpecificHeader;
		break;

	case MODE_TYPE_MMODE:
		header = new ModeSpecificHeader;
		break;

	case MODE_TYPE_PWDOPPLER:
		header = new ModeSpecificHeader;
		break;

	case MODE_TYPE_CFM:
		header = new ModeSpecificHeader;
		break;

	case MODE_TYPE_VFI:
		header = new ModeSpecificHeader;
		break;
	
	case MODE_TYPE_ELASTO:
		header = new ModeSpecificHeader;
		break;

	case MODE_TYPE_IRRELEVANT:
		header = new ModeSpecificHeader;
		break;
	
	default:
		header = new ModeSpecificHeader;
		assert(false);
		break;

	}

	
	return header;
}






char * FileHeader::ToCString(char* str, size_t strLen)
{
	const char *fmtFileHeader = "\n\n\
File Header   \n\n\
             len : %d bytes (Header Length) \n\
       numFrames : %d\n\
textHeaderOffset : %d bytes\n\
dataHeaderOffset : %d bytes\n\
modeHeaderOffset : %d bytes\n\
      dataOffset : %d bytes\n\n\
";
	int numWrBytes = snprintf(str, strLen, fmtFileHeader, 	
	    this->len,                   ///< Length of FileHeader in bytes
	    this->numFrames,             ///< Number of frames in the file. The header is updated just before closing the file.
	    this->textHeaderOffset,      ///< Offset from the start of file to the Free Text Header
	    this->dataHeaderOffset,      ///< Offset from start of file to the Generic Data Header in bytes
	    this->modeHeaderOffset,      ///< Offset from start of file to the Mode Specific Header in bytes
	    this->dataOffset);           ///< Data Offset
	assert(numWrBytes > 0);
	numWrBytes = numWrBytes; // dummy to escape C4189 in release builds
	return str;
}

FileHeader::FileHeader()
{
	this->Reset();
}




void FileHeader::Reset()
{
	this->magicID = BKMDF_MAGIC_ID;
	this->len = sizeof(FileHeader);   
	this->numFrames = 0;
	this->dataHeaderOffset = 0;
	this->modeHeaderOffset = 0;
	this->textHeaderOffset = 0;
}




char* GenericDataHeader::ToCString(char *str, size_t numLen)
{
	const char *fmtDataHeader = "\n\n\
Generic Data Header\n\n\
          len : %d bytes (Header length)\n\
          ver : %d \n\
     frameLen : %d bytes \n\
Sample format : ( %d x %d ) bytes\n\
       numDim : %d (Num Dims of Data set)\n\
";

	char* pOrigStr = str;
	int numWrBytes = snprintf(str, numLen, 
		fmtDataHeader, 
		this->len,
		this->ver,
		this->frameSize.frameLen,
		this->frameSize.sampleInfo.bytesPerSample,
		this->frameSize.sampleInfo.numValues,
		this->frameSize.numDim);

	

	numLen = numLen - numWrBytes;
	str = str + numWrBytes;

	if ((numWrBytes < 0) || (numLen == 0)){  // If could not write, or there is no more space
		return pOrigStr;                          // Just return whatever was successfully written
	}


const char *fmtDim = "\n\
dim[ %2d ]\n\
    headerLen : %d bytes\n\
      numElem : %d \n\
       padLen : %d bytes\n\
";
	for (int n = 0; n < this->frameSize.numDim; n++)
	{
		
		numWrBytes = snprintf(str, numLen, 
			fmtDim, n,
			this->frameSize.dimInfo[n].headerLen,
			this->frameSize.dimInfo[n].numElem,
			this->frameSize.dimInfo[n].padLen);
		numLen = numLen = numWrBytes;
		str = str + numWrBytes;

		if ((numWrBytes < 0) || (numLen == 0)){  // If could not write, or there is no more space
			return pOrigStr;                     // Just return whatever was successfully written
		}
	}

	return pOrigStr;
}



GenericDataHeader::GenericDataHeader()
{
	this->Reset();
}



void GenericDataHeader::Reset()
{
	this->len = sizeof(GenericDataHeader);
	this->ver = BKMDF_DATA_HEADER_VER;
	this->frameSize.sampleFormat = SAMPLE_FORMAT_UINT8;                           ///< Description of the sample format
	this->frameSize.sampleInfo = sampleFormatInfoTable[ SAMPLE_FORMAT_UINT8 ];    ///< Information about the size of a sample
	this->frameSize.numDim = 0;                                                   ///< Number of dimensions in the data set
	for (int n = 0; n < BKMDF_MAX_NUM_DIM; n++){
		this->frameSize.dimInfo[n].numElem = 0;    ///< Information about dimensions
		this->frameSize.dimInfo[n].headerLen = 0;
		this->frameSize.dimInfo[n].padLen = 0;
	}
	
}




char * ModeSpecificHeader::ToCString(char* str, size_t strLen)
{
	const char* fmtModeHeader = "\n\n\
Mode Specific Header\n\n\
        len : %d (bytes) (Len of mode-specific-header)\n\
        ver : %d \n\
   modeType : %d ( %s ) \n\
physBuffLoc : %d ( %s)\n\
     buffID : %d \n\
";
	snprintf(str, strLen, fmtModeHeader,
		this->len,
		this->ver,
		this->dataInfo.modeType,  imgModeTypeTxt[ this->dataInfo.modeType ],
		this->dataInfo.physBuffLoc,  physBuffLocTxt[ this->dataInfo.physBuffLoc ],
		this->dataInfo.buffID);
	return str;
}


void ModeSpecificHeader::Reset()
{
	this->len = sizeof(ModeSpecificHeader) - sizeof(ModeSpecificHeader*); //sizeof() returns the length of the structure. 
	                                                                      // ModeSpecificHeader has virtual functions. 
	                                                                      // The compiler inserts a "this" pointer
	                                                                      // in front of the first field of the structure (offset 0).
	
	this->ver = BKMDF_MODE_HEADER_VER;                  // Version of header implementation
	this->dataInfo.buffID = 0;                          // Buffer ID is used by Inject/Acq module
	this->dataInfo.modeType = MODE_TYPE_BMODE;          // Imaging mode
	this->dataInfo.physBuffLoc = PHYS_BUFF_LOC_RXBF;    // Physical buffer location. Default - receiver
}


void ModeSpecificHeader::InitializeFrom(const UltrasoundDataInfo& ultrasoundDataInfo)
{
	if (UltrasoundDataUtil::ValidUltrasoundDataInfo(ultrasoundDataInfo) == true){
		this->dataInfo = ultrasoundDataInfo;
	}else{
		this->dataInfo.modeType = MODE_TYPE_BMODE;
		this->dataInfo.physBuffLoc = PHYS_BUFF_LOC_PCINPUT;
		this->dataInfo.buffID = 0;
	}
}



ModeSpecificHeader::ModeSpecificHeader()
{
	this->Reset();
}


ModeSpecificHeader::ModeSpecificHeader(UltrasoundDataInfo& ultrasoundDataInfo)
{
	this->Reset();
	this->InitializeFrom(ultrasoundDataInfo);
}


char * FreeTextHeader::ToCString(char* str, size_t strLen)
{
	snprintf(str, strLen, "%s", this->freeText);
	return str;
}



FreeTextHeader::FreeTextHeader()
{
	this->Reset();
}



void FreeTextHeader::Reset()
{
	this->len = sizeof(FreeTextHeader);
	memset(this->freeText, 0, sizeof(this->freeText));
}



BkmDataFile::BkmDataFile()
{
	this->opened = false;
	this->fid = NULL;
	this->modeHeader = new ModeSpecificHeader;
	ResetHeaders();

}



BkmDataFile::~BkmDataFile()
{
	if (opened == true)
	{
		Close();
	}
	
	if (this->modeHeader != NULL)
	{
		delete this->modeHeader;
	}
}




bool BkmDataFile::OpenCreate(const char* filename, const FrameSizeInfo &frameSize,  const UltrasoundDataInfo &ultraData, const char* description)
{

	ResetErrMsgTxt();
	if (opened == true){
		Close();
	}


	this->fid = fopen(filename,"w+b");

	if (this->fid == NULL) {
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Could not open file : %s ", filename);
		this->opened = false;
		return (this->opened);
	}

	this->opened = true;
	
	if (InitializeHeaders(frameSize, ultraData, description) == false){
		this->Close(false);
		return this->opened;
	}

	if (WriteHeaders() == false){
		char *tmpErr = new char[BKMDF_ERRMSGTXT_LEN];   // Keep a copy of the error.
		snprintf(tmpErr,BKMDF_ERRMSGTXT_LEN, this->errMsgTxt);
		this->Close(false);
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Failed at writing headers to file %s. Error: %s", filename, tmpErr);
		delete [] tmpErr;
		return this->opened;
	}

	return this->opened;
}




bool BkmDataFile::OpenReadWrite(const char* filename)
{
	ResetErrMsgTxt();

	if (opened == true){
		Close();
	}


	this->fid = fopen(filename,"r+b");

	if (this->fid == NULL) {
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Could not open file : %s ", filename);
		this->opened = false;
		return (this->opened);
	}

	this->opened = true;

	if (ReadHeaders() == false){
		char *tmpErr = new char[BKMDF_ERRMSGTXT_LEN];   // Keep a copy of the error.
		snprintf(tmpErr,BKMDF_ERRMSGTXT_LEN, this->errMsgTxt);
		this->Close(false);
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Failed at reading headers from the file %s. Error: %s", filename, tmpErr);
		delete [] tmpErr;
	}

	return this->opened;
	
}




void BkmDataFile::Close(bool flushHeaders)
{
	if (this->opened == false){
		return;
	}

	if (flushHeaders == true){
		FillInTextHeader();    // Update the textual description. Needed to reflect eventual 
		WriteHeaders();
	}

	fflush(this->fid);    // Redundant maybe
	fclose(this->fid);
	this->opened = false;

	
	ResetHeaders();
}




bool BkmDataFile::AppendRawFrame(const void *frame)
{
	ResetErrMsgTxt();

	if (this->opened == false){
		snprintf(errMsgTxt, sizeof(errMsgTxt), "File is not opened ");
		return false;
	}
	fseek(this->fid, 0, SEEK_END);   // Position at the end
	
	size_t count = fwrite(frame, dataHeader.frameSize.frameLen, 1, this->fid);

	if (count == 1){
		fileHeader.numFrames ++; 
		return true;
	}else{
		snprintf(errMsgTxt, sizeof(errMsgTxt), "Could not write to file ");
		return false;
	}

}


bool BkmDataFile::ReadRawFrame(void* buf, int bufLen, int frameNo)
{
	ResetErrMsgTxt();

	

	if (buf == NULL){
		snprintf(errMsgTxt, sizeof(errMsgTxt), "NULL pointer for input argument buf");
		assert(false);
		return false;
	}
	if (this->opened == false){
		snprintf(errMsgTxt, sizeof(errMsgTxt), "File is not opened ");
		return false;
	}


	if (frameNo > (this->fileHeader.numFrames -1)  || frameNo < 0){
		snprintf(errMsgTxt,sizeof(errMsgTxt), "File has %d frames. Attempt to read frame num. %d", this->fileHeader.numFrames, frameNo);
		return false;
	}
	
	
	if (bufLen < this->dataHeader.frameSize.frameLen){
		snprintf(errMsgTxt, sizeof(errMsgTxt), "Buffer for reading data is too small ");
		return false;
	}
	
	int offset = frameNo * this->dataHeader.frameSize.frameLen + this->fileHeader.dataOffset;
	fseek(this->fid, offset, SEEK_SET );
	size_t count = fread(buf, this->dataHeader.frameSize.frameLen, 1, this->fid );

	if (count != 1){
		snprintf(errMsgTxt,sizeof(errMsgTxt), "Could not read a frame ");
		return false;
	}
	return true;
}





bool BkmDataFile::InitializeHeaders( const FrameSizeInfo &frameSize,  const UltrasoundDataInfo &dataInfo, const char* description)
{

	// Clear all information in the headers
	ResetHeaders();
	
	bool valid = FrameDimUtil::ValidDims(frameSize);

	if (valid == false){
		snprintf(errMsgTxt, BKMDF_ERRMSGTXT_LEN, "Dimensions do not validate. Error:\n%s", FrameDimUtil::GetLastErrTxt());
		assert(false);
		return false;
	}

	valid = UltrasoundDataUtil::ValidUltrasoundDataInfo(dataInfo);
	if (valid == false){
		snprintf(errMsgTxt, BKMDF_ERRMSGTXT_LEN, "UltrasoundDataInfo does not validate: Error:\n%s", UltrasoundDataUtil::GetLastErrTxt());
		return false;
	}


	if (this->modeHeader != NULL){   // Release allocated memory and allocate a new header
		delete this->modeHeader;     // of the right type
		this->modeHeader = NULL;     // Not necessary because it get allocated below. But just for the principle.
	}

	this->modeHeader = ModeSpecificHeaderFactory::CreateModeSpecificHeader(dataInfo.modeType);


	// Set the Offsets in the file. The file organization is
	//       +-------------------------------------------------+
	//       |     File Header                                 |
	//       +-------------------------------------------------+
	//       |     Free Text Header                            |
	//       +-------------------------------------------------+
	//       |     Generic Data Header                         |
	//       +-------------------------------------------------+
	//       |     Mode Specific Header                        |
	//       +-------------------------------------------------+
	//       |     Data (Payload)                              |
	//       

	this->fileHeader.textHeaderOffset = this->fileHeader.len;
	this->fileHeader.dataHeaderOffset = this->fileHeader.textHeaderOffset + this->textHeader.len;
	this->fileHeader.modeHeaderOffset = this->fileHeader.dataHeaderOffset + this->dataHeader.len;
	this->fileHeader.dataOffset = this->fileHeader.modeHeaderOffset + this->modeHeader->len;


	// Set the actual values of the headers
	this->dataHeader.frameSize = frameSize;
	this->modeHeader->InitializeFrom(dataInfo);
	
	
	this->dataHeader.frameSize.frameLen = FrameDimUtil::CalcFrameSize(frameSize);
	
	// Fill-in the "description" part of the file header
	// To avoid new lines we set a full line of spaces terminated with '\0'
	memset(this->fileHeader.Description, ' ', BKMDF_FILE_DESCR_LEN-1);
	this->fileHeader.Description[BKMDF_FILE_DESCR_LEN-1] = '\0';
	

	if (description != NULL)
	{  // If the user has provided a description, copy the description as is.
		snprintf(this->fileHeader.Description, BKMDF_FILE_DESCR_LEN-1, "\n%s", description);
		this->fileHeader.Description[BKMDF_FILE_DESCR_LEN-1] = '\0';
	}
	

	// Initialize the free-text file header
	FillInTextHeader();
	return true;
}




void BkmDataFile::FillInTextHeader()
{


	/*    ----------------------------------------------
	 *   |        Fill in file header                   |
	 *    ----------------------------------------------
	 */
	char* nextWrPosTxtHdr= this->textHeader.freeText;
	char const * ptrEndTxtHdr = this->textHeader.freeText + sizeof(this->textHeader.freeText)-1;
	size_t remainChars = ptrEndTxtHdr - nextWrPosTxtHdr;
	
	this->fileHeader.ToCString(nextWrPosTxtHdr, remainChars);


  /*
   *   ---------------------------------------------------
   *  |     Fill in data header                           |
   *   ---------------------------------------------------
   */
	nextWrPosTxtHdr = this->textHeader.freeText + strlen(this->textHeader.freeText);
	remainChars = ptrEndTxtHdr - nextWrPosTxtHdr;
	assert(remainChars > 0);


	this->dataHeader.ToCString(nextWrPosTxtHdr, remainChars);

	/*      ------------------------------------------------
	 *     |   Mode Specific Header                         |
	 *      ------------------------------------------------
	 */

	nextWrPosTxtHdr = this->textHeader.freeText + strlen(this->textHeader.freeText);
	remainChars = ptrEndTxtHdr - nextWrPosTxtHdr;
	assert(remainChars > 0);

	this->modeHeader->ToCString(nextWrPosTxtHdr, remainChars);

	nextWrPosTxtHdr = this->textHeader.freeText + strlen(this->textHeader.freeText);
	remainChars = ptrEndTxtHdr - nextWrPosTxtHdr;
	assert(remainChars > 0);     // snprintf will always place a '\0'. ramainChars == 0 may be OK, but it also may indicate that 
	                             // there has not been enough room for the last of the headers. 
}





bool BkmDataFile::ReadHeaders()
{
	ResetHeaders();
	ResetErrMsgTxt();

	if (this->opened == false){
		return false;
	}

	fseek(this->fid, 0, SEEK_SET);
	size_t count = fread(&this->fileHeader, sizeof(this->fileHeader), 1, this->fid);

	if (count != 1){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, "Could not read fileHeader ( fileHeader )");
		return false;
	}
	

	fseek(this->fid, this->fileHeader.dataHeaderOffset, SEEK_SET);
	count = fread(&this->dataHeader, sizeof(this->dataHeader), 1, this->fid);

	if (count != 1){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN , "Could not read dataHeader ( Generic Data Header )");
		return false;
	}


	ModeSpecificHeader basicModeHeader;
	basicModeHeader.Reset();

	fseek(this->fid, this->fileHeader.modeHeaderOffset, SEEK_SET);
	count = fread(basicModeHeader.GetPtr(), basicModeHeader.len, 1, this->fid);
	
	//count = fread(this->modeHeader->GetPtr(), this->modeHeader->len, 1, this->fid );

	if (count != 1){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, "Could not read modeHeader ( Mode Specific Header ) ");
		return false;
	}

	if (this->modeHeader != NULL ){
		delete this->modeHeader;
	}

	this->modeHeader = ModeSpecificHeaderFactory::CreateModeSpecificHeader(basicModeHeader.dataInfo.modeType);


	// Read the header second time - this time with the right type.
	fseek(this->fid, this->fileHeader.modeHeaderOffset, SEEK_SET);
	count = fread(this->modeHeader->GetPtr(), this->modeHeader->len, 1, this->fid );

	if (count != 1){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, "Could not read modeHeader ( Mode Specific Header ) ");
		return false;
	}


	
	fseek(this->fid, this->fileHeader.textHeaderOffset, SEEK_SET);
	count = fread(&this->textHeader, sizeof( this->textHeader ), 1, this->fid );

	if (count != 1){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, "Could not read textHeader ( Free Text Header ) ");
		return false;
	}


	return true;
}



bool BkmDataFile::WriteHeaders()
{
	if (this->opened == false){
		return false;
	}
	fseek(this->fid, 0, SEEK_SET);   // Go to the beginning of file
	
	size_t count = 0;     
	count = fwrite(&this->fileHeader, 1, this->fileHeader.len, this->fid);

	if (count != static_cast<size_t>(this->fileHeader.len) ){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Could not write fileHeader ");
		return false;
	}

	count = fwrite(&this->textHeader, 1, sizeof(this->textHeader), this->fid);
	if (count != sizeof (this->textHeader) ){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Could not write textHeader ");
		return false;

	}

	count = fwrite(&this->dataHeader, 1, sizeof(this->dataHeader), this->fid);
	if (count != sizeof (this->dataHeader) ){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Could not write dataHeader ");
		return false;

	}

	count = fwrite(this->modeHeader->GetPtr(), 1, this->modeHeader->len, this->fid);
	if (count != static_cast<size_t>(this->modeHeader->len) ){
		snprintf(this->errMsgTxt, BKMDF_ERRMSGTXT_LEN, " Could not write modeHeader ");
		return false;
	}

	return true;
}



void BkmDataFile::ResetHeaders()
{
	this->fileHeader.Reset();
	this->dataHeader.Reset();
	if ( this->modeHeader != NULL ){
		this->modeHeader->Reset();
	}
	this->textHeader.Reset();
}



#ifdef _MSC_VER
#pragma warning(pop)
#endif
