/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSonixPortaVideoSource.cxx,v $

  Copyright (c) Elvis Chen, Queen's University,
                chene@cs.queensu.ca
                Sept 2008.
  All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/
#include "vtkSonixPortaVideoSource.h"

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnsignedCharArray.h"
#include "vtksys/SystemTools.hxx"

#include <ctype.h>

// because of warnings in windows header push and pop the warning level
#ifdef _MSC_VER
#pragma warning (push, 3)
#endif

#include <vector>
#include <string>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

vtkCxxRevisionMacro(vtkSonixPortaVideoSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSonixPortaVideoSource);

//----------------------------------------------------------------------------
vtkSonixPortaVideoSource* vtkSonixPortaVideoSource::Instance = 0;
vtkSonixPortaVideoSourceCleanup vtkSonixPortaVideoSource::Cleanup;

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#  define vtkGetWindowLong GetWindowLongPtr
#  define vtkSetWindowLong SetWindowLongPtr
#  define vtkGWL_USERDATA GWLP_USERDATA
#else // regular Visual studio
#  define vtkGetWindowLong GetWindowLong
#  define vtkSetWindowLong SetWindowLong
#  define vtkGWL_USERDATA GWL_USERDATA
#endif //

//----------------------------------------------------------------------------
vtkSonixPortaVideoSourceCleanup::vtkSonixPortaVideoSourceCleanup()
{
}

//----------------------------------------------------------------------------
vtkSonixPortaVideoSourceCleanup::~vtkSonixPortaVideoSourceCleanup()
{
  // Destroy any remaining output window.
  //vtkSonixPortaVideoSource::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkSonixPortaVideoSource::vtkSonixPortaVideoSource() 
{
  // porta instantiation
  this->PtrPorta = 0;
  this->PtrPorta = new porta;
  this->PortaBModeWidth = 640;       // defaults to BMode, 640x480
  this->PortaBModeHeight = 480;
  this->ImageBuffer = 0;
  this->ImageBuffer = new unsigned char [ this->PortaBModeWidth *
					  this->PortaBModeHeight * 4 ];
  if ( !this->ImageBuffer ) 
    {
      LOG_ERROR("vtkSonixPortaVideoSource constructor: not enough emory for ImageBuffer" );
    }
  
  this->PortaImageMode = (int)BMode;
  this->PortaMotorStartPosition = 0;
  this->PortaMotorPosition = 0;
  this->PortaProbeSelected = 0;
  this->PortaModeSelected = 0;
  this->PortaProbeName = 0;
  this->PortaSettingPath = 0;
  this->PortaFirmwarePath = 0;
  this->PortaLUTPath = 0;
  this->PortaCineSize = 256 * 1024 * 1024; // defaults to 245MB of Cine
  this->Initialized = 0;
  this->bIsPortaSettingPathSet = 0;
  this->bIsPortaFirmwarePathSet = 0;
  this->bIsPortaLUTPathSet = 0;
  
  // VTK
  this->FrameSize[0] = this->PortaBModeWidth;
  this->FrameSize[1] = this->PortaBModeHeight;
  this->FrameSize[2] = 1;

  // video output format
  this->OutputFormat = VTK_RGBA;
  this->NumberOfScalarComponents = 4;
  this->FrameBufferBitsPerPixel = 32;
  this->FlipFrames = 1;
  this->FrameBufferRowAlignment = 1;

  this->Initialized = 0;
}

vtkSonixPortaVideoSource::~vtkSonixPortaVideoSource() 
{
  // clean up porta related sources
  this->vtkSonixPortaVideoSource::ReleaseSystemResources();
  
  
  // finally, porta itself
  if ( this->PtrPorta ) 
    {
      delete PtrPorta;
      PtrPorta = 0;
    }

  // release all previously allocated memory
  if ( this->PortaProbeName ) 
    {
      delete [] this->PortaProbeName;
      this->PortaProbeName = 0;
    }

  if ( this->PortaSettingPath ) 
    {
      delete [] this->PortaSettingPath;
      this->PortaSettingPath = 0;
    }

  if ( this->PortaFirmwarePath ) 
    {
      delete [] this->PortaFirmwarePath;
      this->PortaFirmwarePath = 0;
    }

  if ( this->PortaLUTPath ) 
    {
      delete [] this->PortaLUTPath;
      this->PortaLUTPath = 0;
    }

  if ( this->ImageBuffer ) 
    {
      delete [] this->ImageBuffer;
      this->ImageBuffer = 0;
    }
  
  // finally, port itself
  if ( this->PtrPorta ) 
    {
      delete [] PtrPorta;
      PtrPorta = 0;
    }
}


//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::ReleaseSystemResources() 
{
  // if porta is instantiated, shut it down
  if ( this->PtrPorta ) 
    {
      this->PtrPorta->stopImage();
      this->PtrPorta->shutdown();
    }
}

//----------------------------------------------------------------------------
// up the reference count so it behaves like New
vtkSonixPortaVideoSource *vtkSonixPortaVideoSource::New() 
{
  vtkSonixPortaVideoSource *ret = vtkSonixPortaVideoSource::GetInstance();
  ret->Register( NULL );
  return( ret );
}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback( void *param, unsigned char *data, int cineBlock, int reserved ) {
    
  if ( !data ) 
    {
      // no actual data received
      return ( false );
    }
  
  vtkSonixPortaVideoSource::GetInstance()->LocalInternalGrab( param,
                                                              data,
                                                              cineBlock,
                                                              reserved );
  
  return ( true );
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
void vtkSonixPortaVideoSource::LocalInternalGrab( void *param,
						  unsigned char *data,
						  int cineBlock, int reserved ) 
{
  this->FrameBufferMutex->Lock();
  
  // frame buffer indices maintaince
  if ( this->AutoAdvance ) 
    {
      this->AdvanceFrameBuffer( 1 );
      
      if ( this->FrameIndex + 1 < this->FrameBufferSize ) 
	{
	  this->FrameIndex++;
	}
    }
  
  // time stamping
  int index = this->FrameBufferIndex % this->FrameBufferSize;
  while ( index < 0 )
    {
      index += this->FrameBufferSize;
    }
  
  this->FrameBufferTimeStamps[index] = vtkTimerLog::GetUniversalTime();
  
  if ( this->FrameCount++ == 0 ) 
    {
      this->StartTimeStamp = this->FrameBufferTimeStamps[ index ];
    }
  
  // decode the data according to the imaging parameters
  //
  // get the pointer to the actual incoming data onto a local pointer
  unsigned char *deviceDataPtr = static_cast<unsigned char*>( data );
  
  // get the pointer to the correct location of the frame buffer,
  // where this data needs to be copied
  unsigned char *frameBufferPtr = (unsigned char *)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetPointer(0));
  
  int outBytesPerRow = ((this->FrameBufferExtent[1]- this->FrameBufferExtent[0]+1)* this->FrameBufferBitsPerPixel + 7)/8;
  outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;
  
  int inBytesPerRow = this->FrameSize[0] * this->FrameBufferBitsPerPixel/8;
  
  int rows = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;
  
  // acquire an post-processed image
  this->PtrPorta->getBwImage( 0, this->ImageBuffer, true );
  
  // 4) copy the data to the local vtk frame buffer
  if ( outBytesPerRow == inBytesPerRow ) 
    {
      memcpy( frameBufferPtr,
	      this->ImageBuffer,
	      inBytesPerRow*rows );
    }
  else
    {
      while( --rows >= 0 ) 
	{
	  memcpy( frameBufferPtr,
		  deviceDataPtr,
		  outBytesPerRow );
	  frameBufferPtr += outBytesPerRow;
	  deviceDataPtr += inBytesPerRow;
	}
    }
  
  this->Modified();
  this->FrameBufferMutex->Unlock();
}


//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Initialize() 
{
  if ( this->Initialized ) 
    {
      return;
    }

  if ( bIsPortaSettingPathSet == 0 ||
       bIsPortaFirmwarePathSet == 0 ||
       bIsPortaFirmwarePathSet == 0 ) 
    {
      LOG_ERROR("One of the Porta paths has not been set" );
      return;
    }
  
  int code;
  probeInfo nfo;
  char name[80];
  int size = 256;
  char err[ 256 ];
  
  if ( !this->PtrPorta->init( this->PortaCineSize,
			      this->PortaFirmwarePath,
			      this->PortaSettingPath,
			      this->PortaLUTPath ) ) 
    {
      this->PtrPorta->getLastError( err, size );
      LOG_ERROR("Initialize: Porta could not be initialized: "
		     << "(" << err << ")" );
      return;
    }
  
  // test if the nameing scheme matches
  if ( this->PtrPorta->testParameterNames() != -1 ) 
    {
      LOG_ERROR("Porta naming scheme does not match" );
      this->ReleaseSystemResources();
      return;
    }
  
  // select the probe
  if ( this->PtrPorta->isConnected() ) 
    {
      code = (char)this->PtrPorta->getProbeID( 0 );
    }
   
  // select the code read and see if it is motorized
  if ( this->PtrPorta->selectProbe( code ) &&
       this->PtrPorta->getProbeInfo( nfo ) &&
       nfo.motorized ) 
    {
      
      // the 3D/4D probe is always connected to port 0
      this->PtrPorta->activateProbeConnector( 0 );
      this->PtrPorta->getProbeName( name, 80, code );
      
      // store the probe name
      if ( this->PortaProbeName ) 
	{
	  delete [] this->PortaProbeName;
	}
      this->PortaProbeName = new char [ 80 ];
      sprintf( this->PortaProbeName, "%s", name );
      
      if ( !this->PtrPorta->findMasterPreset( name, 80, code ) ) 
	{
	  LOG_ERROR("Initialize: master preset cannot be found" );
	  return;
	}
      
      if ( !this->PtrPorta->loadPreset( name ) ) 
	{
	  LOG_ERROR("Initialize: master preset could not be loaded" );
	  return;
	}

      // now we have successfully selected the probe
      this->PortaProbeSelected = 1;
    }
  
  // this is from propello
  if( !this->PtrPorta->initImagingMode( BMode ) ) 
    {
      LOG_ERROR("Initialize: cannot initialize imagingMode" );
      return;
    }
  else
    {
      this->PtrPorta->setDisplayDimensions( 0,
					    this->PortaBModeWidth,
					    this->PortaBModeHeight );
    }
  
  // successfully set to bmode
  this->PortaModeSelected = 1;
  
  // manual acquisition
  this->PtrPorta->setParam( prmMotorStatus, 0 );

  // finally, update all the parameters
  if ( !this->UpdateSonixPortaParams() ) 
    {
      LOG_ERROR("Initialize: cannot update sonix params" ); 
    }
  

  // set up the frame buffer
  this->FrameBufferMutex->Lock();
  // Frame buffer is also updated within DoFormatSetup
  this->DoFormatSetup();
  this->FrameBufferMutex->Unlock();

  // set up the callback function which is invocked upon arrival
  // of a new frame
  
  this->PtrPorta->setRawDataCallback( vtkSonixPortaVideoSourceNewFrameCallback,
				      (void*)this );

  this->Initialized = 1;
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::UpdateSonixPortaParams() 
{
  if ( !this->PtrPorta ) 
    {
      return( 0 );
    }
  
  bool bRunning = this->PtrPorta->isImaging();
  
  if ( bRunning ) 
    {
      this->PtrPorta->stopImage();
    }
  
  if ( !this->PtrPorta->updateImageParams() ) 
    {
      return( 0 );
    }
  
  if ( !this->PtrPorta->uploadData() ) 
    {
      return( 0 );
    }
  
  // update VTK FrameRate with SonixRP's hardware frame rate
  //
  // The reasons we update it here is because the SonixRP's hardware
  // frame rate is a function of several parameters, such as
  // bline density and image-depths.
  //
  this->FrameRate = (float)(this->PtrPorta->getFrameRate() );
  
  this->PtrPorta->updateDisplayParams();
  
  if ( bRunning ) 
    {
      this->PtrPorta->runImage();
    }
  
  this->Modified();
  
  return ( 1 );
}

//----------------------------------------------------------------------------
// check the current video format and set up the VTK video framebuffer to match
void vtkSonixPortaVideoSource::DoFormatSetup()
{
  // for now, assume bmode
  this->FrameSize[0] = this->PortaBModeWidth;
  this->FrameSize[1] = this->PortaBModeHeight;
  this->FrameSize[2] = 1;
  
  this->FrameBufferBitsPerPixel = 32;
  this->OutputFormat = VTK_RGBA;
  this->NumberOfScalarComponents = 4;
  
  this->UpdateFrameBuffer();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Grab() 
{
  // check this function:  why did Sid not do anything here?
  
  if ( this->Recording ) 
    {
      return;
    }
  
  // insure that the frame buffer is properly initialized
  this->Initialize();
  
  if ( !this->Initialized ) 
    {
      return;
    }
}


//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Record() 
{
  // ensure the hardware is initialized  
  this->Initialize();
  
  if ( !this->Initialized ) 
    {
      return;
    }
    
  if ( !this->Recording ) 
    {
      this->Recording = 1;
      this->FrameCount = 0;
      this->Modified();
      
      if ( !this->PtrPorta->isImaging() )
	this->PtrPorta->runImage();
    }
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Stop() 
{
  if ( this->Recording ) 
    {
      this->Recording = 0;
      this->Modified();
  
      if ( this->PtrPorta->isImaging() )
	{
	  this->PtrPorta->stopImage();
	}
    }
}


//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::RequestInformation(
						 vtkInformation * vtkNotUsed(request),
						 vtkInformationVector **vtkNotUsed(inputVector),
						 vtkInformationVector *outputVector) 
{
  
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  
  int i;
  int extent[6];

  // ensure that the hardware is initialized.
  this->Initialize();

  for ( i = 0; i < 3; i++ ) 
    {
      // initially set extent to the OutputWholeExtent
      extent[2*i] = this->OutputWholeExtent[2*i];
      extent[2*i+1] = this->OutputWholeExtent[2*i+1];
      // if 'flag' is set in output extent, use the FrameBufferExtent instead
      if ( extent[2*i+1] < extent[2*i] )
	{
	  extent[2*i] = 0;
	  extent[2*i+1] =						\
	    this->FrameBufferExtent[2*i+1] - this->FrameBufferExtent[2*i];
	}
      this->FrameOutputExtent[2*i] = extent[2*i];
      this->FrameOutputExtent[2*i+1] = extent[2*i+1];
    }
  
  
  int numFrames = this->NumberOfOutputFrames;
  if (numFrames < 1)
    {
      numFrames = 1;
    }
  if (numFrames > this->FrameBufferSize) 
    {
      numFrames = this->FrameBufferSize;
    }
  
  // multiply Z extent by number of frames to output
  extent[5] = extent[4] + (extent[5]-extent[4]+1) * numFrames - 1;
  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);
  
  // set the spacing
  outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);
  
  // set the origin.
  outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);
  

  /*
   *FIXME
   */
  // set the default data type
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR,
					      this->NumberOfScalarComponents);

  return( 1 );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::RequestData( vtkInformation *vtkNotUsed(request),
					   vtkInformationVector **vtkNotUsed(inputVector),
					   vtkInformationVector *vtkNotUsed(outputVector)) 
{
  vtkImageData *data = this->AllocateOutputData(this->GetOutput());
  int i,j;
  
  int outputExtent[6];     // will later be clipped in Z to a single frame
  int saveOutputExtent[6]; // will possibly contain multiple frames
  data->GetExtent(outputExtent);
  for (i = 0; i < 6; i++)
    {
      saveOutputExtent[i] = outputExtent[i];
    }
  // clip to extent to the Z size of one frame  
  outputExtent[4] = this->FrameOutputExtent[4]; 
  outputExtent[5] = this->FrameOutputExtent[5]; 
  
  int frameExtentX = this->FrameBufferExtent[1]-this->FrameBufferExtent[0]+1;
  int frameExtentY = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;
  int frameExtentZ = this->FrameBufferExtent[5]-this->FrameBufferExtent[4]+1;

  int extentX = outputExtent[1]-outputExtent[0]+1;
  int extentY = outputExtent[3]-outputExtent[2]+1;
  int extentZ = outputExtent[5]-outputExtent[4]+1;

  // if the output is more than a single frame,
  // then the output will cover a partial or full first frame,
  // several full frames, and a partial or full last frame

  // index and Z size of the first frame in the output extent
  int firstFrame = (saveOutputExtent[4]-outputExtent[4])/extentZ;
  int firstOutputExtent4 = saveOutputExtent[4] - extentZ*firstFrame;

  // index and Z size of the final frame in the output extent
  int finalFrame = (saveOutputExtent[5]-outputExtent[4])/extentZ;
  int finalOutputExtent5 = saveOutputExtent[5] - extentZ*finalFrame;

  char *outPtr = (char *)data->GetScalarPointer();
  char *outPtrTmp;

  int inIncY = (frameExtentX*this->FrameBufferBitsPerPixel + 7)/8;
  inIncY = ((inIncY + this->FrameBufferRowAlignment - 1)/
            this->FrameBufferRowAlignment)*this->FrameBufferRowAlignment;
  int inIncZ = inIncY*frameExtentY;

  int outIncX = this->NumberOfScalarComponents;
  int outIncY = outIncX*extentX;
  int outIncZ = outIncY*extentY;

  int inPadX = 0;
  int inPadY = 0;
  int inPadZ; // do inPadZ later

  int outPadX = -outputExtent[0];
  int outPadY = -outputExtent[2];
  int outPadZ;  // do outPadZ later

  if (outPadX < 0)
    {
      inPadX -= outPadX;
      outPadX = 0;
    }
  
  if (outPadY < 0)
    {
      inPadY -= outPadY;
      outPadY = 0;
    }
  
  int outX = frameExtentX - inPadX; 
  int outY = frameExtentY - inPadY; 
  int outZ; // do outZ later

  if (outX > extentX - outPadX)
    {
      outX = extentX - outPadX;
    }
  
  if (outY > extentY - outPadY)
    {
      outY = extentY - outPadY;
    }
  
  // if output extent has changed, need to initialize output to black
  for (i = 0; i < 3; i++)
    {
      if (saveOutputExtent[i] != this->LastOutputExtent[i])
	{
	  this->LastOutputExtent[i] = saveOutputExtent[i];
	  this->OutputNeedsInitialization = 1;
	}
    }
  
  // ditto for number of scalar components
  if (data->GetNumberOfScalarComponents() != 
      this->LastNumberOfScalarComponents)
    {
      this->LastNumberOfScalarComponents = data->GetNumberOfScalarComponents();
      this->OutputNeedsInitialization = 1;
    }

  // initialize output to zero only when necessary
  if (this->OutputNeedsInitialization)
    {
      memset(outPtr,0,
	     (saveOutputExtent[1]-saveOutputExtent[0]+1)*
	     (saveOutputExtent[3]-saveOutputExtent[2]+1)*
	     (saveOutputExtent[5]-saveOutputExtent[4]+1)*outIncX);
      this->OutputNeedsInitialization = 0;
    } 
  
  // we have to modify the outputExtent of the first frame,
  // because it might be complete (it will be restored after
  // the first frame has been copied to the output)
  int saveOutputExtent4 = outputExtent[4];
  outputExtent[4] = firstOutputExtent4;
  
  this->FrameBufferMutex->Lock();
  
  int index = this->FrameBufferIndex;
  this->FrameTimeStamp = 
    this->FrameBufferTimeStamps[index % this->FrameBufferSize];
  
  int frame;
  for (frame = firstFrame; frame <= finalFrame; frame++)
    {
      if (frame == finalFrame)
	{
	  outputExtent[5] = finalOutputExtent5;
	} 
      
      vtkDataArray *frameBuffer = reinterpret_cast<vtkDataArray *>(this->FrameBuffer[(index + frame) % this->FrameBufferSize]);
      
      char *inPtr = reinterpret_cast<char*>(frameBuffer->GetVoidPointer(0));
      char *inPtrTmp ;
      
      extentZ = outputExtent[5]-outputExtent[4]+1;
      inPadZ = 0;
      outPadZ = -outputExtent[4];
      
      if (outPadZ < 0)
	{
	  inPadZ -= outPadZ;
	  outPadZ = 0;
	}
      
      outZ = frameExtentZ - inPadZ;
      
      if (outZ > extentZ - outPadZ)
	{
	  outZ = extentZ - outPadZ;
	}
      
      if (this->FlipFrames)
	{ // apply a vertical flip while copying to output
	  outPtr += outIncZ*outPadZ+outIncY*outPadY+outIncX*outPadX;
	  inPtr += inIncZ*inPadZ+inIncY*(frameExtentY-inPadY-outY);
	  
	  for (i = 0; i < outZ; i++)
	    {
	      inPtrTmp = inPtr;
	      outPtrTmp = outPtr + outIncY*outY;
	      for (j = 0; j < outY; j++)
		{
		  outPtrTmp -= outIncY;
		  if (outX > 0)
		    {
		      this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX);
		    }
		  inPtrTmp += inIncY;
		}
	      outPtr += outIncZ;
	      inPtr += inIncZ;
	    }
	}
      else
	{ // don't apply a vertical flip
	  outPtr += outIncZ*outPadZ+outIncY*outPadY+outIncX*outPadX;
	  inPtr += inIncZ*inPadZ+inIncY*inPadY;
	  
	  for (i = 0; i < outZ; i++)
	    {
	      inPtrTmp = inPtr;
	      outPtrTmp = outPtr;
	      for (j = 0; j < outY; j++)
		{
		  if (outX > 0) 
		    {
		      this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX);
		    }
		  outPtrTmp += outIncY;
		  inPtrTmp += inIncY;
		}
	      outPtr += outIncZ;
	      inPtr += inIncZ;
	    }
	}
      // restore the output extent once the first frame is done
      outputExtent[4] = saveOutputExtent4;
    }
  
  this->FrameBufferMutex->Unlock();
  
  return 1;
  
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::UnpackRasterLine(char *outptr, char *inptr, 
						int start, int count) 
{
  // assume bpost mode for now
  char alpha = (char)(this->Opacity*255);
  
  inptr += start;

  memcpy( outptr, inptr, count*4 );
}


//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetOutputFormat(int format)
{  
  if (format == this->OutputFormat) 
    {
      return;
    }
  
  this->OutputFormat = format;
  
  // convert color format to number of scalar components
  int numComponents;
  
  switch (this->OutputFormat) 
    {
    case VTK_RGBA:
      numComponents = 4;
      break;
    case VTK_RGB:
      numComponents = 3;
      break;
    case VTK_LUMINANCE:
      numComponents = 1;
      break;
    default:
      numComponents = 0;
      LOG_ERROR("SetOutputFormat: Unrecognized color format.");
      break;
    }
  
  
  this->NumberOfScalarComponents = numComponents;
  
  if (this->FrameBufferBitsPerPixel != numComponents*8) 
    {
      this->FrameBufferMutex->Lock();
      this->FrameBufferBitsPerPixel = numComponents*8;
      if (this->Initialized) 
	{
	  // shouldn't have to call UpdateFrameBuffer
	  // as it is called within DoFormatSetup
	  // this->UpdateFrameBuffer();    
	  this->DoFormatSetup();
	}
      this->FrameBufferMutex->Unlock();
    }
  
  this->Modified();
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkSonixPortaVideoSource *vtkSonixPortaVideoSource::GetInstance() 
{
  
  if ( !vtkSonixPortaVideoSource::Instance ) 
    {
      // try the factory first
      vtkSonixPortaVideoSource::Instance = (vtkSonixPortaVideoSource *)vtkObjectFactory::CreateInstance( "vtkSonixPortaVideoSource" );
      
      if ( !vtkSonixPortaVideoSource::Instance ) 
	{
	  vtkSonixPortaVideoSource::Instance = new vtkSonixPortaVideoSource();
	}
      
      if ( !vtkSonixPortaVideoSource::Instance ) 
	{
	  int error = 0;
	}
    }
  
  return( vtkSonixPortaVideoSource::Instance );
}

void vtkSonixPortaVideoSource::SetInstance( vtkSonixPortaVideoSource *instance ) 
{
  if ( vtkSonixPortaVideoSource::Instance == instance ) 
    {
      return;
    }
  
  // preferably this will be NULL
  if ( vtkSonixPortaVideoSource::Instance ) 
    {
      vtkSonixPortaVideoSource::Instance->Delete();
    }
  
  vtkSonixPortaVideoSource::Instance = instance;
  
  if ( !instance ) 
    {
      return;
    }
  
  //user will call ->Delete() after setting instance
  instance->Register( NULL );
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetPortaFirmwarePath( char *path )
{
  if ( path ) 
    {
      if ( this->PortaFirmwarePath ) 
	{
	  delete [] PortaFirmwarePath;
	}
      this->PortaFirmwarePath = new char [256];
      sprintf( this->PortaFirmwarePath, "%s", path );
      this->bIsPortaFirmwarePathSet = 1;
    }
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetPortaSettingPath( char *path )
{
  if ( path ) 
    {
      if ( this->PortaSettingPath ) 
	{
	  delete [] PortaSettingPath;
	}
      this->PortaSettingPath = new char [256];
      sprintf( this->PortaSettingPath, "%s", path );
      this->bIsPortaSettingPathSet = 1;
    }
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetPortaLUTPath( char *path )
{
  if ( path ) 
    {
      if ( this->PortaLUTPath ) 
	{
	  delete [] PortaLUTPath;
	}
      this->PortaLUTPath = new char [256];
      sprintf( this->PortaLUTPath, "%s", path );
      this->bIsPortaLUTPathSet = 1;
    }
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetPortaCineSize( int size ) 
{
  this->PortaCineSize = size;
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::PrintSelf(ostream& os, vtkIndent indent) {
 
  this->Superclass::PrintSelf(os,indent);
  
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetImageDepth( int myDepth ) 
{
  if ( !this->PtrPorta->setParam( prmImageDepth, myDepth ) ) 
    {
      this->Depth = myDepth;
      LOG_ERROR("Problem setting depth manually" );
      return( 0 );
    }
  
  return( this->UpdateSonixPortaParams() );
}

  
//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetZoom( int percentage ) 
{
  // 100% means full height of the display area
  if ( !this->PtrPorta->setParam( prmZoom, percentage ) ) 
    {
      LOG_ERROR("SetZoom:  cannot set the zoom value" );
      return( 0 );
    }
  
  return( this->UpdateSonixPortaParams() );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetBLineDensity( int density ) {
  if ( !this->PtrPorta->setParam( prmBLineDensity, density ) ) 
    {
      LOG_ERROR("SetBLineDensity: cannot set the line density" );
      return( 0 );
    }
  
  return( this->UpdateSonixPortaParams() );
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetPortaMotorStartPosition( double v )
{
  this->PortaMotorStartPosition = v;
}

//----------------------------------------------------------------------------
double vtkSonixPortaVideoSource::GetPortaMotorStartPosition()
{
  return( this->PortaMotorStartPosition );
}

//----------------------------------------------------------------------------
// The actual angle being set may not be the same as the specified angle
// thus we return the actual angle that is being set.
double vtkSonixPortaVideoSource::GoToPortaMotorStartPosition()
{

  bool bRunning = this->PtrPorta->isImaging();
  
  if ( bRunning ) 
    {
      this->PtrPorta->stopImage();
    }
  
  this->PortaMotorPosition = this->PtrPorta->goToPosition( this->PortaMotorStartPosition );

  if ( bRunning ) 
    {
      this->PtrPorta->runImage();
    }
  
  return ( this->PortaMotorPosition );
}

//----------------------------------------------------------------------------
// The actual angle being set may not be the same as the specified angle
// thus we return the actual angle that is being set.
double vtkSonixPortaVideoSource::GoToPosition( double angle ) 
{
  bool bRunning = this->PtrPorta->isImaging();
  
  if ( bRunning ) 
    {
      this->PtrPorta->stopImage();
    }
  
  this->PortaMotorPosition = this->PtrPorta->goToPosition( angle );

  if ( bRunning ) 
    {
      this->PtrPorta->runImage();
    }
  
  return ( this->PortaMotorPosition );
}
//----------------------------------------------------------------------------
// The actual angle being set may not be the same as the specified angle
// thus we return the actual angle that is being set.
double vtkSonixPortaVideoSource::StepPortaMotor( bool cw, int steps )
{
  bool bRunning = this->PtrPorta->isImaging();
  
  if ( bRunning ) 
    {
      this->PtrPorta->stopImage();
    }

  this->PortaMotorPosition = this->PtrPorta->stepMotor( cw, steps );
  
  if ( bRunning ) 
    {
      this->PtrPorta->runImage();
    }
  
  return ( this->PortaMotorPosition );

}

//----------------------------------------------------------------------------
double vtkSonixPortaVideoSource::GetPortaMotorPosition()
{
  return( this->PortaMotorPosition );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::GetPortaFrameRate()
{
  return( this->PtrPorta->getFrameRate() );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetBModeFrameSize( int width, int height ) 
{
  this->PortaBModeWidth = width;
  this->PortaBModeHeight = height;
  
  // need to resize the ImageBuffer
  if ( this->ImageBuffer ) 
    {
      delete [] this->ImageBuffer;
      this->ImageBuffer = 0;
    }
  
  this->ImageBuffer = new unsigned char [ this->PortaBModeWidth *
					  this->PortaBModeHeight * 4 ];
  
  if ( !this->ImageBuffer ) 
    {
      LOG_ERROR("SetBModeFrameSize: Not enough memory" );
      return( 0 );
    }
  

  // VTK
  this->FrameSize[0] = this->PortaBModeWidth;
  this->FrameSize[1] = this->PortaBModeHeight;
  this->FrameSize[2] = 1;

  if ( this->Initialized ) 
    {
      this->FrameBufferMutex->Lock();
      this->DoFormatSetup();
      this->FrameBufferMutex->Unlock();
     
      this->PtrPorta->setDisplayDimensions( 0,
					    this->PortaBModeWidth,
					    this->PortaBModeHeight );

      //
      // not sure if needed, but won't hurt regardless
      //
      if ( !this->UpdateSonixPortaParams() ) 
	{
	  LOG_ERROR("SetBModeFrameSize:  unable to update param" );
	  return( 0 );
	}
    }
  
  return( 1 );
}
