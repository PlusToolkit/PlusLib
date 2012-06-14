/** \mainpage BK Medical Data File Format
 *  
 * \section SecIntroduction Introduction
 *  The BK Medical Data File is intended for storage of ultrasound data grabbed
 *  at different points along the signal processing path - from the beamformer 
 *  to the display.  The storage is implemented in the class BkDataFile.
 * 
 *  See also sections:
 *  \li \ref SecFileFormat
 *  \li \ref SecFrameLayout
 *  \li \ref DetailedHeaders
 *  \li \ref SecAddNewFunc
 *  
 *  \subsection SecDesignConsiderations Design Considerations
 *
 *  The data format and representation of samples changes along the data path of 
 *  ultrasound signals - from real 16-bit channel data to 32-bit real beam sum, to 
 *  16-bit complex samples IQ data to 32-bit floating point strain and flow
 *  data, to finally 8-bit image data. The file format should therefore support all of these
 *  different types of data, and the list of supported sample formats is enumerated in BkmDf::SampleFormat
 *  and in BkmDf::SampleFormatInfo
 *
 *  The data can have different dimensions, from 1-dimensional A-mode vector, or 5-dimensional
 *  IQ samples used as input to Vector Flow Imaging (the reader is encouraged to find the 5 dimensions). Furthermore
 *  headers may or may not be present at different locations. These headers can be attached to any
 *  of the dimensions - for example on a per-line basis, or on a per frame basis. 
 *
 *  The structures BkmDf::FrameSizeInfo and  BkmDf::DimInfo encapsulate the description of the data 
 *  dimensionality. 
 *
 *   And finally, to facilitate grabbing and subsequent injection back of data, and of off-line signal processing, 
 *   a description  of the imaging mode and the physical location of buffer stored in the file is needed.
 *   This description varies from imaging mode to imaging. 
 *   
 *   The minimum needed information is encapsulated in the data structure BkmDf::UltrasoundDataInfo
 *   
 *   All this information must be stored in the file. \b Four headers are implemented.
 *   \li BkmDf::FileHeader - Stores offset to different parts in the file. Described in section \ref SecFileHeader
 *   \li BkmDf::GenericDataHeader - Encapsulates BkmDf::FrameSizeInfo. Used to determine the size of the frames. Described in section \ref SecGenericDataHeader
 *   \li BkmDf::ModeSpecificHeader - Encapsulates BkmDf::UltrasoundDataInfo. Contains information needed to inject back data and to process data off-line. <b> Can be extended </b>. Described in section \ref SecModeSpecificHeader
 *   \li BkmDf::FreeTextHeader - Contains a human-readable version of the other headers. Used to inspect data files using a normal text editor. Described in section \ref SecFreeTextHeader
 *
 *  
 *   \li <b> File Format is described in Section \ref SecFileFormat  </b>
 *
 * \section SecExampleOfUse Example of Use
 *
 * \code
 * uint8_t  frame1[HEIGHT][WIDTH];   
 * uint8_t  frame2[HEIGHT][WIDTH];
 *
 * char txtBuf[2048];     // Text buffer, where headers will output their c-string representations.
 *
 *
 * int m, n;
 *
 * for (m = 0; m<HEIGHT; m++){
 *     for (n = 0; n < WIDTH; n++){
 *         frame1[m][n] = ( ((m-256)*(m-256) + (n - 256)*(n-256)) < 50*50)? 255:128;
 *         frame2[m][n] = ( (m > 200 && m < 300) && (n > 200 && n < 300) )? 0:192;
 *      }
 * }
 *
 *
 * BkmDf::UltrasoundDataInfo ultrasDataInfo;
 * BkmDf::FrameSizeInfo frameSizeInfo;
 *
 * frameSizeInfo.SetSampleFormat(SAMPLE_FORMAT_UINT8);
 * int dims[2] = {WIDTH, HEIGHT};
 * frameSizeInfo.SetDimNoHeaderNoPad(2, dims);    // Set the dimensions. No headers or dimensions
 * 
 * BkmDf::BkmDataFile bkmFile;
 * bkmFile.OpenCreate("bitmap8.env", frameSizeInfo, ultrasDataInfo, "Two simple bitmaps ");
 * bkmFile.AppendRawFrame(frame1);
 * bkmFile.AppendRawFrame(frame2);
 * bkmFile.Close();
 *
 * \endcode
 *
 * 
 * The resulting headers are given below:
 *
 *  \code
 *
 * File Header
 * 
 *               len : 284 bytes (Header Length) 
 *         numFrames : 2
 *  textHeaderOffset : 284 bytes
 *  dataHeaderOffset : 4384 bytes
 *  modeHeaderOffset : 4532 bytes
 *        dataOffset : 4552 bytes
 *
 *
 * 
 * Generic Data Header
 *
 *           len : 148 bytes (Header length)
 *           ver : 1 
 *      frameLen : 262144 bytes
 * Sample format : ( 1 x 1 ) bytes
 *        numDim : 2 (Num Dims of Data set)
 * 
 * dim[  0 ]
 *     headerLen : 0 bytes
 *       numElem : 512 
 *        padLen : 0 bytes
 * 
 * dim[  1 ]
 *     headerLen : 0 bytes
 *       numElem : 512 
 *        padLen : 0 bytes
 *
 * Mode Specific Header
 *
 *        len : 20 (bytes) (Len of mode-specific-header)
 *         ver : 1 
 *    modeType : 1 ( B-mode ) 
 * physBuffLoc : 5 ( PC, Review Buf)
 *      buffID : 0 
 *
 *  \endcode
 *
 * \page PageFileFormat File Format
 * 
 * \section SecFileFormat File Format
 *  You should also inspect sections
 *  \li \ref SecDetailedHeaders
 *  \li \ref SecFrameLayout
 *
 *  \image html file_structure.png
 *  \image latex file_structure.pdf 
 * 
 *  As it can be seen the data is preceded by the 4 headers described above. 
 * 
 *  
 * \page DetailedHeaders Detailed Header Format
 *
 * \section SecDetailedHeaders Detailed Header Format
 *
 * \subsection SecFileHeader File Header
 *
 *  \image html headersFileHeader.png 
 *  \image latex headersFileHeader.pdf width=16cm
 *
 *  \subsection SecFreeTextHeader Free Text Header
 *
 *  \image html headersFreeTextHeader.png
 *  \image latex headersFreeTextHeader.pdf
 *
 *  \subsection SecGenericDataHeader Generic Data Header
 *
 *  \image html headersGenericDataHeader.png
 *  \image latex headersGenericDataHeader.pdf width=16cm
 *
 *  \subsection SecModeSpecificHeader Mode Specific Header
 *  
 *  \image html headersModeSpecificHeader.png
 *  \image latex headersModeSpecificHeader.pdf width=16cm
 *
 *
 * \page PageFrameLayoyt Frame Layout 
 * \section SecFrameLayout Layout of data in a frame
 *
 *  A frame is described as P-dimensional data set. The dimensions are simply nested inside each
 *  other. The layout in the memory is simple (see BkmDf::FrameSizeInfo):
 *  \code
 *  function SaveDataSet(const BkmDf::FrameSizeInfo& frameSize, int dim)
 *  {
 *      write header[dim]
 *      if (dim == 0) then
 *         for n = 0 to frameSize[dim].numElem
 *             write dataElement[n]
 *      else
 *         for n = 0 to frameSize.dimInfo[dim].numElem
 *              call SaveDataSet(frameSize, dim-1)
 *      endif
 *      write padding[dim]
 *  }
 *  \endcode 
 *  
 *   An example layoyt is shown in the image below:
 *   \image html framesMemoryLayout.png
 *   \image latex frameMemoryLayout.pdf
 *
 *
 * 
 * \page PageAddNewFunc How to Add New Functionality
 * 
 * \section SecAddNewFunc Add new functionality
 *  
 * \subsection SecNewModeSpecificHeader New Mode Specific Header
 *  Adding a new Mode Specific Header requires the following steps
 *  \li Extend the class ModeSpecificHeader. For example,
 *   \code
 *      struct CfmSpecificHeader: ModeSpecificHeader{
 *            int32_t numShotsPerEstimate;
 *            int32_t interleaveFactor;
 *            int32_t multiBeamFactor;
 *            int32_t SmpFreq;
 *            int32_t CenterFreq;
 *      }
 *   \endcode
 *  
 *   \li Override the virtual functions
 *   \code
 *    	char * BkmDf::ModeSpecificHeader::ToCString(char* str, size_t strLen);
 *      void BkmDf::ModeSpecificHeader::Reset();
 *      void BkmDf::ModeSpecificHeader::InitializeFrom(const UltrasoundDataInfo& ultrasDataInfo);
 *   \endcode
 *   Notice that every header has a length. All headers are implemented in C++ as structures.
 *   The function sizeof() returns the actual length of the structure. ModeSpecificHeader is the
 *   only structure with virtual functions. The compiler adds an actual field with a pointer
 *   to the structure (a real \c this pointer). To fill in the length of the new header, use the following
 *   construction:
 *   \code
 *     CfmSpecificHeader::Reset()
 *     {
 *        this->len = sizeof(CfmSpecificHeader) - sizeof(CfmSpecificHeader*);
 *        ... 
 *     }
 *   \endcode
 *
 *   \li Modify  BkmDf::ModeSpecificHeaderFactory::CreateModeSpecificHeader function and add
 *       your custom header
 *    \code
 *   BkmDf::ModeSpecificHeaderFactory::CreateModeSpecificHeader(const ImgModeType& imgModeType)
 *   {
 *    ModeSpecificHeader *header = NULL;
 *    switch(imgModeType){
 *        ...
 *    case MODE_TYPE_CFM:
 *         header = new CfmSpecificHeader;
 *         break;
 *   }
 *  \endcode
 *
 *   \li If you want to add a new mode altogether, you should do so by inserting a new enumerated value in the BkmDf::ImgModeType,
 *       just before NUM_MODE_TYPES. <b> Never change the order of existing enumerated values, since this will render the new
 *       code incompatible with previous versions.</b>
 *
 *  \subsection  SecAddNewUtil Add new physical location and new initialization for BkmDf::UltrasoundDataInfo
 *   The idea with the class BkmDf::UltrasoundDataUtil is to allow the easy addition of new utility functions
 *   without the need to change the interface to the BkmDf::UltrasoundDataInfo.  It contains functions to validate
 *   whether parameters are within range and to set default values for specific scan situations.
 *  
 *  
 */