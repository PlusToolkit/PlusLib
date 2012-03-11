/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// ICCapturingListener.cpp: implementation of the ICCapturingListener class.
//----------------------------------------------------------------------------

#include <iostream>

#include "ICCapturingListener.h"

using namespace DShowLib;

//----------------------------------------------------------------------------
ICCapturingListener::ICCapturingListener()
{
  this->ICCapturingSourceNewFrameCallback = NULL; 
}

//----------------------------------------------------------------------------
ICCapturingListener::~ICCapturingListener()
{
}

//----------------------------------------------------------------------------
/*! The overlayCallback() method draws the number of the current frame. The
  frame count is a member of the tsMediaSampleDesc structure that is passed
  to overlayCallback() by the Grabber.
*/
void ICCapturingListener::overlayCallback( Grabber& caller, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc& MediaSampleDesc)
{
  char szText[25];
  if( pBitmap->getEnable() == true ) // Draw only, if the overlay bitmap is enabled.
  {
    sprintf( szText,"%04d ", MediaSampleDesc.FrameNumber);
    pBitmap->drawText( RGB(255,0,0), 0, 0, szText );
  }
}

//----------------------------------------------------------------------------
/*! The frameReady() method calls the saveImage method to save the image buffer to disk.
*/
void ICCapturingListener::frameReady( Grabber& caller, smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
{
  if (this->ICCapturingSourceNewFrameCallback != NULL)
  {
    pBuffer->lock(); 
    (*ICCapturingSourceNewFrameCallback)( pBuffer->getPtr(), pBuffer->getBufferSize(), currFrame );
    pBuffer->unlock(); 
  }
}

//----------------------------------------------------------------------------
/*! Initialize the array of bools that is used to memorize, which buffers were processed in 
  the frameReady() method. The size of the array is specified by the parameter NumBuffers.
  It should be equal to the number of buffers in the FrameHandlerSink.
  All members of m_BufferWritten are initialized to false.
  This means that no buffers have been processed.
*/
void ICCapturingListener::setBufferSize( unsigned long NumBuffers )
{
  m_BufferWritten.resize( NumBuffers, false );
}

//----------------------------------------------------------------------------
/*! The image passed by the MemBuffer pointer is saved to a BMP file.
*/
void ICCapturingListener::saveImage( smart_ptr<MemBuffer> pBuffer, DWORD currFrame)
{
  char filename[MAX_PATH];
  if( currFrame < m_BufferWritten.size() )
  {
    sprintf( filename, "image%02i.bmp", currFrame );

    saveToFileBMP( *pBuffer, filename );

    m_BufferWritten.at( currFrame ) = true;
  }
}
