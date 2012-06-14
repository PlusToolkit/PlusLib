#include "BkmDataFileFrameUtil.h"

#include <assert.h>


using namespace BkmDf;

namespace BkmDf
{

void BkmDf::SetCopyDimFromFrameSize(CopyDim* copyDim, const FrameSizeInfo& size)
{
	copyDim->numDim = size.numDim;

	for (int n = 0; n < size.numDim; n++)
	{
		copyDim->numElem[n] = size.dimInfo[n].numElem;
		copyDim->dataOffset[n] = size.dimInfo[n].headerLen;
	}

	FrameDimUtil::CalcPitch(copyDim->pitch, size.numDim, size);
	copyDim->lenDim0 = FrameDimUtil::CalcLenDim0(size);
}



void CopyFrame(uint8_t* dest, const CopyDim& destDim, const uint8_t *src, const CopyDim& srcDim, const int dimNum)
{

	assert(destDim.numDim == srcDim.numDim);
	assert(destDim.lenDim0 == srcDim.lenDim0);


	if (dimNum > 0)
	{
		for(int n = 0; n < destDim.numElem[dimNum]; n++ )
		{
			int destOffset = destDim.pitch[dimNum-1]*n + destDim.dataOffset[dimNum];
			int srcOffset = srcDim.pitch[dimNum-1]*n + srcDim.dataOffset[dimNum];

			CopyFrame( dest + destOffset, 
					   destDim,
				       src  + srcOffset, 
					   srcDim,
				       dimNum - 1);
		}
		return;
	}


	memcpy(dest + destDim.dataOffset[0], src + srcDim.dataOffset[0], destDim.lenDim0);

	return;
}



void CopyFrame(uint8_t* dest, const FrameSizeInfo& destSize, const uint8_t *src, const FrameSizeInfo& srcSize, const int dimNum)
{
	CopyDim destDim;
	CopyDim srcDim;

	SetCopyDimFromFrameSize(&destDim, destSize);
	SetCopyDimFromFrameSize(&srcDim, srcSize);

	CopyFrame(dest, destDim, src, srcDim, dimNum);

}



void CopyMatrixFrame(uint8_t* dest, const CopyDim& destDim, const uint8_t *src, const CopyDim& srcDim)
{
	
	assert(destDim.numDim == srcDim.numDim);
	assert(destDim.lenDim0 == srcDim.lenDim0);
	assert(destDim.numDim == 2);

	int destOffs = destDim.dataOffset[1] + destDim.dataOffset[0];
	int srcOffs = srcDim.dataOffset[1] + srcDim.dataOffset[0];

	for (int n = 0; n < destDim.numElem[1]; n++)
	{
		memcpy(dest + destDim.pitch[0]*n + destOffs, src +srcDim.pitch[0]*n + srcOffs, destDim.lenDim0);
	}

	return;
}


void CopyMatrixFrame(uint8_t* dest, const FrameSizeInfo& destSize, const uint8_t *src, const FrameSizeInfo& srcSize)
{
	CopyDim destDim;
	CopyDim srcDim;

	SetCopyDimFromFrameSize(&destDim, destSize);
	SetCopyDimFromFrameSize(&srcDim, srcSize);

	CopyMatrixFrame(dest, destDim, src, srcDim);
}


}