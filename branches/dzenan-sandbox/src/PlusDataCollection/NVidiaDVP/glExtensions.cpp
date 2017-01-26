/***************************************************************************\
|*                                                                           *|
|*      Copyright 2007 NVIDIA Corporation.  All rights reserved.             *|
|*                                                                           *|
|*   NOTICE TO USER:                                                         *|
|*                                                                           *|
|*   This source code is subject to NVIDIA ownership rights under U.S.       *|
|*   and international Copyright laws.  Users and possessors of this         *|
|*   source code are hereby granted a nonexclusive, royalty-free             *|
|*   license to use this code in individual and commercial software.         *|
|*                                                                           *|
|*   NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE     *|
|*   CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR         *|
|*   IMPLIED WARRANTY OF ANY KIND. NVIDIA DISCLAIMS ALL WARRANTIES WITH      *|
|*   REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF         *|
|*   MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR          *|
|*   PURPOSE. IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL,            *|
|*   INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES          *|
|*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN      *|
|*   AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING     *|
|*   OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE      *|
|*   CODE.                                                                   *|
|*                                                                           *|
|*   U.S. Government End Users. This source code is a "commercial item"      *|
|*   as that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting       *|
|*   of "commercial computer  software" and "commercial computer software    *|
|*   documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995)   *|
|*   and is provided to the U.S. Government only as a commercial end item.   *|
|*   Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through        *|
|*   227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the       *|
|*   source code with only those rights set forth herein.                    *|
|*                                                                           *|
|*   Any use of this source code in individual and commercial software must  *|
|*   include, in the user documentation and internal comments to the code,   *|
|*   the above Disclaimer and U.S. Government End Users Notice.              *|
|*                                                                           *|
|*                                                                           *|
\***************************************************************************/

#if WIN32
#include <windows.h>
#endif

#include "nvConfigure.h"
#include <gl\glu.h>
#include <stdio.h>

// GL_EXT_framebuffer_object
PFNGLISRENDERBUFFEREXTPROC                              glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC                            glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC                         glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC                            glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC                         glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC                  glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC                               glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC                             glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC                          glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC                             glGenFramebuffersEXT = NULL;
PFNGLBLITFRAMEBUFFEREXTPROC                             glBlitFramebufferEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC                      glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC                        glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                        glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC                        glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC                     glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC         glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC                              glGenerateMipmapEXT = NULL;

//GL_EXT_timer_query
PFNGLGENQUERIESPROC                                     glGenQueries = NULL;
PFNGLDELETEQUERIESPROC                                  glDeleteQueries = NULL;
PFNGLBEGINQUERYPROC                                     glBeginQuery = NULL;
PFNGLENDQUERYPROC                                       glEndQuery = NULL;
PFNGLGETQUERYOBJECTUI64VEXTPROC                         glGetQueryObjectui64vEXT = NULL;
PFNGLGETINTEGER64VPROC                                  glGetInteger64v = NULL;
PFNGLGETQUERYOBJECTUIVPROC                              glGetQueryObjectuiv = NULL;
// WGL_gpu_affinity
PFNWGLENUMGPUSNVPROC                                    wglEnumGpusNV = NULL;
PFNWGLENUMGPUDEVICESNVPROC                              wglEnumGpuDevicesNV = NULL;
PFNWGLCREATEAFFINITYDCNVPROC                            wglCreateAffinityDCNV = NULL;
PFNWGLDELETEDCNVPROC                                    wglDeleteDCNV = NULL;
PFNWGLENUMGPUSFROMAFFINITYDCNVPROC                      wglEnumGpusFromAffinityDCNV = NULL;

// WGL_Swap_Interval
PFNWGLSWAPINTERVALEXTPROC                               wglSwapIntervalEXT = NULL;

//WGL_ARB_pixel_format
PFNWGLCHOOSEPIXELFORMATARBPROC                          wglChoosePixelFormat = NULL;

// WGL_NV_Copy_image
PFNWGLCOPYIMAGESUBDATANVPROC                            wglCopyImageSubDataNV = NULL;

//GL_ARB_copy_buffer
PFNGLCOPYBUFFERSUBDATAPROC                              glCopyBufferSubData = NULL;

// GL_ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC                                  glBindBuffer = NULL;
PFNGLBUFFERDATAARBPROC                                  glBufferData = NULL;
PFNGLBUFFERSUBDATAARBPROC                               glBufferSubData = NULL;
PFNGLDELETEBUFFERSARBPROC                               glDeleteBuffers = NULL;
PFNGLGENBUFFERSARBPROC                                  glGenBuffers = NULL;
PFNGLMAPBUFFERPROC                                      glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC                                    glUnmapBuffer = NULL;
PFNGLMAPBUFFERRANGEPROC                                 glMapBufferRange = NULL;
PFNGLGETBUFFERSUBDATAPROC                               glGetBufferSubData = NULL;

//NV_present_video
PFNWGLENUMERATEVIDEODEVICESNVPROC                       wglEnumerateVideoDevicesNV = NULL;
PFNWGLBINDVIDEODEVICENVPROC                             wglBindVideoDeviceNV = NULL;
PFNGLPRESENTFRAMEKEYEDNVPROC                            glPresentFrameKeyedNV = NULL;
PFNGLPRESENTFRAMEDUALFILLNVPROC                         glPresentFrameDualFillNV = NULL;
PFNGLGETVIDEOIVNVPROC                                   glGetVideoivNV = NULL;
PFNGLGETVIDEOUIVNVPROC                                  glGetVideouivNV = NULL;
PFNGLGETVIDEOI64VNVPROC                                 glGetVideoi64vNV = NULL;
PFNGLGETVIDEOUI64VNVPROC                                glGetVideoui64vNV = NULL;

//NV_Video_Capture
PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC                wglEnumerateVideoCaptureDevicesNV = NULL;
PFNWGLBINDVIDEOCAPTUREDEVICENVPROC                      wglBindVideoCaptureDeviceNV = NULL;
PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC                   wglReleaseVideoCaptureDeviceNV = NULL;
PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC                      wglLockVideoCaptureDeviceNV = NULL;
PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC                     wglQueryVideoCaptureDeviceNV = NULL;
PFNWGLQUERYCURRENTCONTEXTNVPROC                         wglQueryCurrentContextNV = NULL;
PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC                glVideoCaptureStreamParameterivNV = NULL;
PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC                glVideoCaptureStreamParameterfvNV = NULL;
PFNGLGETVIDEOCAPTURESTREAMIVNVPROC                      glGetVideoCaptureStreamivNV = NULL;
PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC                 glBindVideoCaptureStreamBufferNV = NULL;
PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC                glBindVideoCaptureStreamTextureNV = NULL;
PFNGLBEGINVIDEOCAPTURENVPROC                            glBeginVideoCaptureNV = NULL;
PFNGLENDVIDEOCAPTURENVPROC                              glEndVideoCaptureNV = NULL;
PFNGLVIDEOCAPTURENVPROC                                 glVideoCaptureNV = NULL;

PFNGLSHADERSOURCEARBPROC                                glShaderSource = NULL;
PFNGLCOMPILESHADERARBPROC                               glCompileShader = NULL;
PFNGLGETSHADERIVPROC                                    glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC                               glGetShaderInfoLog = NULL;
PFNGLUSEPROGRAMPROC                                     glUseProgram = NULL;
PFNGLCREATESHADERPROC                                   glCreateShader = NULL;
PFNGLDELETESHADERPROC                                   glDeleteShader = NULL;
PFNGLDELETEPROGRAMPROC                                  glDeleteProgram = NULL;
PFNGLCREATEPROGRAMPROC                                  glCreateProgram = NULL;
PFNGLATTACHSHADERPROC                                   glAttachShader = NULL;
PFNGLDETACHSHADERPROC                                   glDetachShader = NULL;
PFNGLLINKPROGRAMPROC                                    glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC                                   glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC                              glGetProgramInfoLog = NULL;
PFNGLGETUNIFORMLOCATIONPROC                             glGetUniformLocation = NULL;
PFNGLUNIFORM2IVARBPROC                                  glUniform2iv = NULL;

PFNGLFENCESYNCPROC                                      glFenceSync = NULL;           ///< ext function
PFNGLISSYNCPROC                                         glIsSync = NULL;              ///< ext function
PFNGLDELETESYNCPROC                                     glDeleteSync = NULL;          ///< ext function
PFNGLCLIENTWAITSYNCPROC                                 glClientWaitSync = NULL;      ///< ext function
PFNGLWAITSYNCPROC                                       glWaitSync = NULL;            ///< ext function
PFNGLGETSYNCIVPROC                                      glGetSynciv = NULL;           ///< ext function

//---------------------------------------------------------------------------
bool loadCopyImageExtension()
{
  wglCopyImageSubDataNV = ( PFNWGLCOPYIMAGESUBDATANVPROC ) wglGetProcAddress( "wglCopyImageSubDataNV" );

  bool loadOK = ( wglCopyImageSubDataNV != NULL );
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadAffinityExtension()
{
  wglEnumGpusNV = ( PFNWGLENUMGPUSNVPROC ) wglGetProcAddress( "wglEnumGpusNV" );
  wglEnumGpuDevicesNV = ( PFNWGLENUMGPUDEVICESNVPROC ) wglGetProcAddress( "wglEnumGpuDevicesNV" );
  wglCreateAffinityDCNV = ( PFNWGLCREATEAFFINITYDCNVPROC ) wglGetProcAddress( "wglCreateAffinityDCNV" );
  wglDeleteDCNV = ( PFNWGLDELETEDCNVPROC ) wglGetProcAddress( "wglDeleteDCNV" );
  wglEnumGpusFromAffinityDCNV = ( PFNWGLENUMGPUSFROMAFFINITYDCNVPROC ) wglGetProcAddress( "wglEnumGpusFromAffinityDCNV" );

  bool loadOK =
    ( wglEnumGpusNV != NULL ) &&
    ( wglEnumGpuDevicesNV != NULL ) &&
    ( wglCreateAffinityDCNV != NULL ) &&
    ( wglDeleteDCNV != NULL ) &&
    ( wglEnumGpusFromAffinityDCNV != NULL );

  return loadOK;
}

//---------------------------------------------------------------------------
bool loadFramebufferObjectExtension()
{
  glIsRenderbufferEXT = ( PFNGLISRENDERBUFFEREXTPROC ) wglGetProcAddress( "glIsRenderbufferEXT" );
  glBindRenderbufferEXT = ( PFNGLBINDRENDERBUFFEREXTPROC ) wglGetProcAddress( "glBindRenderbufferEXT" );
  glDeleteRenderbuffersEXT = ( PFNGLDELETERENDERBUFFERSEXTPROC ) wglGetProcAddress( "glDeleteRenderbuffersEXT" );
  glGenRenderbuffersEXT = ( PFNGLGENRENDERBUFFERSEXTPROC ) wglGetProcAddress( "glGenRenderbuffersEXT" );
  glRenderbufferStorageEXT = ( PFNGLRENDERBUFFERSTORAGEEXTPROC ) wglGetProcAddress( "glRenderbufferStorageEXT" );
  glGetRenderbufferParameterivEXT = ( PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC ) wglGetProcAddress( "glGetRenderbufferParameterivEXT" );
  glIsFramebufferEXT = ( PFNGLISFRAMEBUFFEREXTPROC ) wglGetProcAddress( "glIsFramebufferEXT" );
  glBindFramebufferEXT = ( PFNGLBINDFRAMEBUFFEREXTPROC ) wglGetProcAddress( "glBindFramebufferEXT" );
  glDeleteFramebuffersEXT = ( PFNGLDELETEFRAMEBUFFERSEXTPROC ) wglGetProcAddress( "glDeleteFramebuffersEXT" );
  glGenFramebuffersEXT = ( PFNGLGENFRAMEBUFFERSEXTPROC ) wglGetProcAddress( "glGenFramebuffersEXT" );
  glCheckFramebufferStatusEXT = ( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC ) wglGetProcAddress( "glCheckFramebufferStatusEXT" );
  glFramebufferTexture1DEXT = ( PFNGLFRAMEBUFFERTEXTURE1DEXTPROC ) wglGetProcAddress( "glFramebufferTexture1DEXT" );
  glFramebufferTexture2DEXT = ( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC ) wglGetProcAddress( "glFramebufferTexture2DEXT" );
  glFramebufferTexture3DEXT = ( PFNGLFRAMEBUFFERTEXTURE3DEXTPROC ) wglGetProcAddress( "glFramebufferTexture3DEXT" );
  glFramebufferRenderbufferEXT = ( PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC ) wglGetProcAddress( "glFramebufferRenderbufferEXT" );
  glGetFramebufferAttachmentParameterivEXT = ( PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC ) wglGetProcAddress( "glGetFramebufferAttachmentParameterivEXT" );
  glGenerateMipmapEXT = ( PFNGLGENERATEMIPMAPEXTPROC ) wglGetProcAddress( "glGenerateMipmapEXT" );
  glBlitFramebufferEXT = ( PFNGLBLITFRAMEBUFFEREXTPROC )wglGetProcAddress( "glBlitFramebufferEXT" );

  bool loadOK =
    ( glIsRenderbufferEXT != NULL ) &&
    ( glBindRenderbufferEXT != NULL ) &&
    ( glDeleteRenderbuffersEXT != NULL ) &&
    ( glGenRenderbuffersEXT != NULL ) &&
    ( glRenderbufferStorageEXT != NULL ) &&
    ( glGetRenderbufferParameterivEXT != NULL ) &&
    ( glIsFramebufferEXT != NULL ) &&
    ( glBindFramebufferEXT != NULL ) &&
    ( glDeleteFramebuffersEXT != NULL ) &&
    ( glGenFramebuffersEXT != NULL ) &&
    ( glCheckFramebufferStatusEXT != NULL ) &&
    ( glFramebufferTexture1DEXT != NULL ) &&
    ( glFramebufferTexture2DEXT != NULL ) &&
    ( glFramebufferTexture3DEXT != NULL ) &&
    ( glFramebufferRenderbufferEXT != NULL ) &&
    ( glGetFramebufferAttachmentParameterivEXT != NULL ) &&
    ( glGenerateMipmapEXT != NULL ) &&
    ( glBlitFramebufferEXT != NULL );
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadTimerQueryExtension()
{
  glGenQueries = ( PFNGLGENQUERIESPROC ) wglGetProcAddress( "glGenQueries" );
  glDeleteQueries = ( PFNGLDELETEQUERIESPROC ) wglGetProcAddress( "glDeleteQueries" );
  glBeginQuery = ( PFNGLBEGINQUERYPROC ) wglGetProcAddress( "glBeginQuery" );
  glEndQuery = ( PFNGLENDQUERYPROC ) wglGetProcAddress( "glEndQuery" );
  glGetQueryObjectui64vEXT = ( PFNGLGETQUERYOBJECTUI64VEXTPROC ) wglGetProcAddress( "glGetQueryObjectui64vEXT" );
  glGetInteger64v = ( PFNGLGETINTEGER64VPROC )wglGetProcAddress( "glGetInteger64v" );
  glGetQueryObjectuiv = ( PFNGLGETQUERYOBJECTUIVPROC )wglGetProcAddress( "glGetQueryObjectuiv" );

  bool loadOK  =
    ( glGenQueries != NULL ) &&
    ( glDeleteQueries != NULL ) &&
    ( glBeginQuery != NULL ) &&
    ( glEndQuery != NULL ) &&
    ( glGetQueryObjectui64vEXT != NULL ) &&
    ( glGetInteger64v != NULL ) && ( glGetQueryObjectuiv != NULL );

  return loadOK;
}

//---------------------------------------------------------------------------
bool loadSwapIntervalExtension()
{
  wglSwapIntervalEXT = ( PFNWGLSWAPINTERVALEXTPROC ) wglGetProcAddress( "wglSwapIntervalEXT" );

  bool loadOK = ( wglSwapIntervalEXT != NULL );
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadPixelFormatExtension()
{
  wglChoosePixelFormat = ( PFNWGLCHOOSEPIXELFORMATARBPROC ) wglGetProcAddress( "wglChoosePixelFormatARB" );

  bool loadOK = ( wglChoosePixelFormat != NULL );
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadBufferObjectExtension()
{
  glBindBuffer = ( PFNGLBINDBUFFERARBPROC ) wglGetProcAddress( "glBindBufferARB" );
  glBufferData = ( PFNGLBUFFERDATAARBPROC ) wglGetProcAddress( "glBufferDataARB" );
  glBufferSubData = ( PFNGLBUFFERSUBDATAARBPROC ) wglGetProcAddress( "glBufferSubDataARB" );
  glDeleteBuffers = ( PFNGLDELETEBUFFERSARBPROC ) wglGetProcAddress( "glDeleteBuffersARB" );
  glGenBuffers = ( PFNGLGENBUFFERSARBPROC ) wglGetProcAddress( "glGenBuffersARB" );
  glMapBuffer = ( PFNGLMAPBUFFERPROC ) wglGetProcAddress( "glMapBufferARB" );
  glMapBufferRange = ( PFNGLMAPBUFFERRANGEPROC ) wglGetProcAddress( "glMapBufferRange" );
  glUnmapBuffer = ( PFNGLUNMAPBUFFERPROC ) wglGetProcAddress( "glUnmapBufferARB" );
  glGetBufferSubData = ( PFNGLGETBUFFERSUBDATAPROC )wglGetProcAddress( "glGetBufferSubData" );

  bool loadOK =
    ( glBindBuffer != NULL ) &&
    ( glBufferData != NULL ) &&
    ( glBufferSubData !=  NULL ) &&
    ( glDeleteBuffers != NULL ) &&
    ( glGenBuffers != NULL ) &&
    ( glMapBuffer != NULL ) &&
    ( glUnmapBuffer != NULL ) &&
    ( glMapBufferRange != NULL ) &&
    ( glGetBufferSubData != NULL );
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadPresentVideoExtension()
{
  // Initialize video extension
  glPresentFrameKeyedNV = ( PFNGLPRESENTFRAMEKEYEDNVPROC ) wglGetProcAddress( "glPresentFrameKeyedNV" );
  glPresentFrameDualFillNV = ( PFNGLPRESENTFRAMEDUALFILLNVPROC ) wglGetProcAddress( "glPresentFrameDualFillNV" );
  wglEnumerateVideoDevicesNV = ( PFNWGLENUMERATEVIDEODEVICESNVPROC ) wglGetProcAddress( "wglEnumerateVideoDevicesNV" );
  wglBindVideoDeviceNV = ( PFNWGLBINDVIDEODEVICENVPROC ) wglGetProcAddress( "wglBindVideoDeviceNV" );
  glGetVideoivNV = ( PFNGLGETVIDEOIVNVPROC ) wglGetProcAddress( "glGetVideoivNV" );
  glGetVideouivNV = ( PFNGLGETVIDEOUIVNVPROC ) wglGetProcAddress( "glGetVideouivNV" );
  glGetVideoi64vNV = ( PFNGLGETVIDEOI64VNVPROC ) wglGetProcAddress( "glGetVideoi64vNV" );
  glGetVideoui64vNV = ( PFNGLGETVIDEOUI64VNVPROC ) wglGetProcAddress( "glGetVideoui64vNV" );

  bool loadOK = ( glPresentFrameKeyedNV != NULL ) && ( glPresentFrameDualFillNV != NULL ) &&
                ( wglEnumerateVideoDevicesNV != NULL ) && ( wglBindVideoDeviceNV != NULL ) &&
                ( glGetVideoivNV != NULL ) && ( glGetVideouivNV != NULL ) &&
                ( glGetVideoi64vNV != NULL ) && ( glGetVideoui64vNV != NULL );
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadCaptureVideoExtension()
{
  wglEnumerateVideoCaptureDevicesNV = ( PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC )wglGetProcAddress( "wglEnumerateVideoCaptureDevicesNV" ) ;
  wglBindVideoCaptureDeviceNV = ( PFNWGLBINDVIDEOCAPTUREDEVICENVPROC )wglGetProcAddress( "wglBindVideoCaptureDeviceNV" );
  wglReleaseVideoCaptureDeviceNV = ( PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC )wglGetProcAddress( "wglReleaseVideoCaptureDeviceNV" );
  wglLockVideoCaptureDeviceNV = ( PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC )wglGetProcAddress( "wglLockVideoCaptureDeviceNV" );
  wglQueryVideoCaptureDeviceNV = ( PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC )wglGetProcAddress( "wglQueryVideoCaptureDeviceNV" );
  wglQueryCurrentContextNV = ( PFNWGLQUERYCURRENTCONTEXTNVPROC )wglGetProcAddress( "wglQueryCurrentContextNV" );
  glVideoCaptureStreamParameterivNV = ( PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC )wglGetProcAddress( "glVideoCaptureStreamParameterivNV" );
  glVideoCaptureStreamParameterfvNV = ( PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC )wglGetProcAddress( "glVideoCaptureStreamParameterfvNV" );
  glGetVideoCaptureStreamivNV = ( PFNGLGETVIDEOCAPTURESTREAMIVNVPROC )wglGetProcAddress( "glGetVideoCaptureStreamivNV" );
  glBindVideoCaptureStreamBufferNV = ( PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC )wglGetProcAddress( "glBindVideoCaptureStreamBufferNV" );
  glBindVideoCaptureStreamTextureNV = ( PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC )wglGetProcAddress( "glBindVideoCaptureStreamTextureNV" );
  glBeginVideoCaptureNV = ( PFNGLBEGINVIDEOCAPTURENVPROC )wglGetProcAddress( "glBeginVideoCaptureNV" );
  glEndVideoCaptureNV = ( PFNGLENDVIDEOCAPTURENVPROC )wglGetProcAddress( "glEndVideoCaptureNV" );
  glVideoCaptureNV = ( PFNGLVIDEOCAPTURENVPROC )wglGetProcAddress( "glVideoCaptureNV" );

  bool loadOK( true );
  if ( !wglBindVideoCaptureDeviceNV || !wglQueryVideoCaptureDeviceNV ||
       !glVideoCaptureStreamParameterivNV ||
       !glVideoCaptureStreamParameterfvNV || !glGetVideoCaptureStreamivNV ||
       !glBindVideoCaptureStreamBufferNV || !glBindVideoCaptureStreamTextureNV ||
       !glBeginVideoCaptureNV || !glEndVideoCaptureNV || !glVideoCaptureNV )
  {
    loadOK = false;
  }
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadShaderObjectsExtension( void )
{
  glShaderSource = ( PFNGLSHADERSOURCEARBPROC )wglGetProcAddress( "glShaderSourceARB" );
  glCompileShader = ( PFNGLCOMPILESHADERARBPROC )wglGetProcAddress( "glCompileShaderARB" );
  glGetShaderiv = ( PFNGLGETSHADERIVPROC )wglGetProcAddress( "glGetShaderiv" );
  glGetShaderInfoLog = ( PFNGLGETSHADERINFOLOGPROC )wglGetProcAddress( "glGetShaderInfoLog" );
  glUseProgram = ( PFNGLUSEPROGRAMPROC )wglGetProcAddress( "glUseProgram" );
  glCreateShader = ( PFNGLCREATESHADERPROC )wglGetProcAddress( "glCreateShader" );
  glDeleteShader = ( PFNGLDELETESHADERPROC )wglGetProcAddress( "glDeleteShader" );
  glDeleteProgram = ( PFNGLDELETEPROGRAMPROC )wglGetProcAddress( "glDeleteProgram" );
  glCreateProgram = ( PFNGLCREATEPROGRAMPROC )wglGetProcAddress( "glCreateProgram" );
  glAttachShader = ( PFNGLATTACHSHADERPROC )wglGetProcAddress( "glAttachShader" );
  glDetachShader = ( PFNGLDETACHSHADERPROC )wglGetProcAddress( "glDetachShader" );
  glLinkProgram = ( PFNGLLINKPROGRAMPROC )wglGetProcAddress( "glLinkProgram" );
  glGetProgramiv = ( PFNGLGETPROGRAMIVPROC )wglGetProcAddress( "glGetProgramiv" );
  glGetProgramInfoLog = ( PFNGLGETPROGRAMINFOLOGPROC )wglGetProcAddress( "glGetProgramInfoLog" );
  glGetUniformLocation = ( PFNGLGETUNIFORMLOCATIONPROC )wglGetProcAddress( "glGetUniformLocation" );
  glUniform2iv = ( PFNGLUNIFORM2IVARBPROC )wglGetProcAddress( "glUniform2iv" );

  bool loadOK = true;
  if ( !glShaderSource || !glCompileShader ||
       !glGetShaderiv || !glGetShaderInfoLog || !glUseProgram || !glCreateShader ||
       !glDeleteShader || !glDeleteProgram || !glAttachShader || !glUniform2iv ||
       !glCreateProgram || !glDetachShader || !glGetUniformLocation ||
       !glLinkProgram || !glGetProgramiv || !glGetProgramInfoLog )
  {
    loadOK = false;
  }
  return loadOK;
}

//---------------------------------------------------------------------------
bool loadSyncExtension()
{
  glFenceSync = ( PFNGLFENCESYNCPROC ) wglGetProcAddress( "glFenceSync" );
  glIsSync = ( PFNGLISSYNCPROC ) wglGetProcAddress( "glIsSync" );
  glDeleteSync = ( PFNGLDELETESYNCPROC ) wglGetProcAddress( "glDeleteSync" );
  glClientWaitSync = ( PFNGLCLIENTWAITSYNCPROC ) wglGetProcAddress( "glClientWaitSync" );
  glWaitSync = ( PFNGLWAITSYNCPROC ) wglGetProcAddress( "glWaitSync" );
  glGetSynciv = ( PFNGLGETSYNCIVPROC ) wglGetProcAddress( "glGetSynciv" );

  bool loadOK =
    ( glFenceSync != NULL ) &&
    ( glIsSync != NULL ) &&
    ( glDeleteSync != NULL ) &&
    ( glClientWaitSync != NULL ) &&
    ( glWaitSync != NULL ) &&
    ( glGetSynciv != NULL );

  return loadOK;
}

//---------------------------------------------------------------------------
bool loadExtensionFunctions( void )
{
  bool fboOK, gpuAffinityOK, vboOK, timerQueryOK, swapOK, pixelFormatOK;

  fboOK = loadFramebufferObjectExtension();

  gpuAffinityOK = loadAffinityExtension();

  vboOK = loadBufferObjectExtension();

  timerQueryOK = loadTimerQueryExtension();

  swapOK = loadSwapIntervalExtension();

  pixelFormatOK = loadPixelFormatExtension();

  return fboOK && gpuAffinityOK && vboOK && swapOK && pixelFormatOK && timerQueryOK;
}