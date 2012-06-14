#include "BkmDataFile.h"


namespace BkmDf 
{

/// <summary>	Redefinition of the Dimensions operation in a form suitable for copy operations.  </summary>
struct CopyDim 
{
	int numDim;                        ///< Number of dimensions in the data set 
	int numElem[BKMDF_MAX_NUM_DIM];    ///< Number of elements in every dimension
	int pitch[BKMDF_MAX_NUM_DIM];      ///< Distance bw the start of 2 elements in bytes
	int dataOffset[BKMDF_MAX_NUM_DIM]; ///< Offset of the first element in every dimension
	int lenDim0;                       ///< Length of dimension 0 in bytes
};

/// <summary> Take a structure with dimensions suitable for the Data Header and fill in a 
///    structure with dimensions suitable for a copy operation
/// </summary>
/// <param name="copyDim">       [out] The resulting structure with sizes used for a copy operation. </param>
/// <param name="framsSizeInfo">	Information describing the frams size. </param>
void SetCopyDimFromFrameSize(CopyDim* copyDim, const FrameSizeInfo& framsSizeInfo);  

/// <summary> Copies a n-dimensional frame of data. The function can be used to switch between 
///  two frames with different headers and padding, but with the same number of elements along
///  every dimension and the same data type. </summary>
/// <param name="dest">     [out] Destination for copy operation </param>
/// <param name="destDim">	Dimensions of the destination frame </param>
/// <param name="src">      Source frame for the copy operation </param>
/// <param name="srcDim"> 	Dimensions of the source frame. </param>
/// <param name="dimNum"> 	Number of dimensions in the frame. </param>
/// 
/// \note Use BkmDf::CopyMatrixFrame to copy 2D matrixes. It is faster.
///
void CopyFrame(uint8_t* dest, const CopyDim& destDim, const uint8_t *src, const CopyDim& srcDim, const int dimNum);


/// <summary> Copies a n-dimensional frame of data. The function can be used to switch between 
///  two frames with different headers and padding, but with the same number of elements along
///  every dimension and the same data type. </summary>
/// <param name="dest">     [out] Destination for copy operation </param>
/// <param name="destSize">	Size of the destination frame </param>
/// <param name="src">      Source frame for the copy operation </param>
/// <param name="srcSize"> 	Size of the source frame. </param>
/// <param name="dimNum"> 	Number of dimensions in the frame. </param>
/// 
/// \note Use BkmDf::CopyMatrixFrame to copy 2D matrixes. It is faster.
///

void CopyFrame(uint8_t* dest, const FrameSizeInfo& destSize, const uint8_t *src, const FrameSizeInfo& srcSize, const int dimNum);




/// <summary> Copy a frame, where the data represents a 2D matrix. The function can be used
/// for example to take out data without the headers, or to copy data to a new location where 
/// headers can be inserted.
/// </summary>
/// <param name="dest">    [out]  Destination buffer . </param>
/// <param name="destDim"> Dimensions of the destination frame. </param>
/// <param name="src">     Source buffer </param>
/// <param name="srcDim">  Dimensions for the source buffer </param>

void CopyMatrixFrame(uint8_t* dest, const CopyDim& destDim, const uint8_t *src, const CopyDim& srcDim);

/// <summary> Copy a frame, where the data represents a 2D matrix. The function can be used
/// for example to take out data without the headers, or to copy data to a new location where 
/// headers can be inserted.
/// </summary>
/// <param name="dest">    [out]  Destination buffer . </param>
/// <param name="destSize">	Size of the destination. </param>
/// <param name="src">      Source frame for the copy operation </param>
/// <param name="srcSize"> 	Size of the source. </param>
void CopyMatrixFrame(uint8_t* dest, const FrameSizeInfo& destSize, const uint8_t *src, const FrameSizeInfo& srcSize);



};   // The end of NameSpace BkmDf