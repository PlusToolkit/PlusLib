/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __ICCapturingListener_h
#define __ICCapturingListener_h

#include <tisudshl.h>

class ICCapturingListener : public DShowLib::GrabberListener  
{
public:
	ICCapturingListener(); 
	~ICCapturingListener(); 

	// Overwrite the GrabberListener methods we need
	virtual void overlayCallback( DShowLib::Grabber& caller, smart_ptr<DShowLib::OverlayBitmap> pBitmap, const DShowLib::tsMediaSampleDesc& MediaSampleDesc ); 
	virtual void frameReady( DShowLib::Grabber& caller, smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD FrameNumber ); 

	// Save one image and mark it as saved
	void		saveImage( smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD currFrame );
	// Setup the buffersize. 
	void		setBufferSize( unsigned long NumBuffers );

	//! Callback function that is executed each time a frame is grabbed
	typedef bool (*ICCapturingSourceNewFramePtr)(unsigned char *imageData, unsigned long dataSize, unsigned long frameNumber);
    void SetICCapturingSourceNewFrameCallback(ICCapturingSourceNewFramePtr cb) { ICCapturingSourceNewFrameCallback = cb; } 

protected: 
	std::vector<bool>	m_BufferWritten;	// array of flags which buffers have been saved.
	
	//! Pointer to the callback function that is executed each time a frame is grabbed
    ICCapturingSourceNewFramePtr ICCapturingSourceNewFrameCallback;

};

#endif 
