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

#ifndef NVGLEXT_H
#define NVGLEXT_H

#if WIN32
#include <windows.h>
#endif
#include "nvConfigure.h"

extern bool loadExtensionFunctions( void );
extern bool loadAffinityExtension();
extern bool loadFramebufferObjectExtension();
extern bool loadTimerQueryExtension();
extern bool loadSwapIntervalExtension();
extern bool loadPixelFormatExtension();
extern bool loadBufferObjectExtension();
extern bool loadPresentVideoExtension();
extern bool loadCaptureVideoExtension();
extern bool loadShaderObjectsExtension( void );
extern bool loadCopyImageExtension();
extern bool loadSyncExtension();

// WGL_NV_Copy_image
extern PFNWGLCOPYIMAGESUBDATANVPROC                       wglCopyImageSubDataNV;

// GL_EXT_framebuffer_object
extern PFNGLISRENDERBUFFEREXTPROC                         glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC                       glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC                    glDeleteRenderbuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC                       glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC                    glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC             glGetRenderBufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC                          glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC                        glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC                     glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC                        glGenFramebuffersEXT;
extern PFNGLBLITFRAMEBUFFEREXTPROC                        glBlitFramebufferEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC                 glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC                   glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                   glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC                   glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC                glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC    glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC                         glGenerateMipmapEXT;


//GL_EXT_timer_query
extern PFNGLGENQUERIESPROC                                glGenQueries;
extern PFNGLDELETEQUERIESPROC                             glDeleteQueries;
extern PFNGLBEGINQUERYPROC                                glBeginQuery;
extern PFNGLENDQUERYPROC                                  glEndQuery;
extern PFNGLGETQUERYOBJECTUI64VEXTPROC                    glGetQueryObjectui64vEXT;
extern PFNGLGETINTEGER64VPROC                             glGetInteger64v;
extern PFNGLGETQUERYOBJECTUIVPROC                         glGetQueryObjectuiv;

// GL_ARB_vertex_program
extern PFNGLPROGRAMSTRINGARBPROC                          glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC                            glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC                         glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC                            glGenProgramsARB;

// WGL_gpu_affinity
extern PFNWGLENUMGPUSNVPROC                               wglEnumGpusNV;
extern PFNWGLENUMGPUDEVICESNVPROC                         wglEnumGpuDevicesNV;
extern PFNWGLCREATEAFFINITYDCNVPROC                       wglCreateAffinityDCNV;
extern PFNWGLDELETEDCNVPROC                               wglDeleteDCNV;
extern PFNWGLENUMGPUSFROMAFFINITYDCNVPROC                 wglEnumGpusFromAffinityDCNV;

// WGL_Swap_Interval
extern PFNWGLSWAPINTERVALEXTPROC                          wglSwapIntervalEXT;

//WGL_ARB_pixel_format
extern PFNWGLCHOOSEPIXELFORMATARBPROC                     wglChoosePixelFormat;

// GL_ARB_vertex_buffer_object
extern PFNGLBINDBUFFERARBPROC                             glBindBuffer;
extern PFNGLBUFFERDATAARBPROC                             glBufferData;
extern PFNGLBUFFERSUBDATAARBPROC                          glBufferSubData;
extern PFNGLDELETEBUFFERSARBPROC                          glDeleteBuffers;
extern PFNGLGENBUFFERSARBPROC                             glGenBuffers;
extern PFNGLMAPBUFFERPROC                                 glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                               glUnmapBuffer;
extern PFNGLMAPBUFFERRANGEPROC                            glMapBufferRange;
extern PFNGLGETBUFFERSUBDATAPROC                          glGetBufferSubData;

//NV_present_video
extern PFNWGLENUMERATEVIDEODEVICESNVPROC                  wglEnumerateVideoDevicesNV;
extern PFNWGLBINDVIDEODEVICENVPROC                        wglBindVideoDeviceNV;
extern PFNGLPRESENTFRAMEKEYEDNVPROC                       glPresentFrameKeyedNV;
extern PFNGLPRESENTFRAMEDUALFILLNVPROC                    glPresentFrameDualFillNV;

extern PFNGLGETVIDEOIVNVPROC                              glGetVideoivNV;
extern PFNGLGETVIDEOUIVNVPROC                             glGetVideouivNV;
extern PFNGLGETVIDEOI64VNVPROC                            glGetVideoi64vNV;
extern PFNGLGETVIDEOUI64VNVPROC                           glGetVideoui64vNV;

//GL_NV_video_capture
extern PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC           wglEnumerateVideoCaptureDevicesNV;
extern PFNWGLBINDVIDEOCAPTUREDEVICENVPROC                 wglBindVideoCaptureDeviceNV;
extern PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC              wglReleaseVideoCaptureDeviceNV;
extern PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC                 wglLockVideoCaptureDeviceNV;
extern PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC                wglQueryVideoCaptureDeviceNV;
extern PFNWGLQUERYCURRENTCONTEXTNVPROC                    wglQueryCurrentContextNV;

extern PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC           glVideoCaptureStreamParameterivNV;
extern PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC           glVideoCaptureStreamParameterfvNV;
extern PFNGLGETVIDEOCAPTURESTREAMIVNVPROC                 glGetVideoCaptureStreamivNV;
extern PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC            glBindVideoCaptureStreamBufferNV;
extern PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC           glBindVideoCaptureStreamTextureNV;
extern PFNGLBEGINVIDEOCAPTURENVPROC                       glBeginVideoCaptureNV;
extern PFNGLENDVIDEOCAPTURENVPROC                         glEndVideoCaptureNV;
extern PFNGLVIDEOCAPTURENVPROC                            glVideoCaptureNV;

/// Shader objects
extern     PFNGLSHADERSOURCEARBPROC                       glShaderSource;
extern     PFNGLCOMPILESHADERARBPROC                      glCompileShader;
extern     PFNGLGETSHADERIVPROC                           glGetShaderiv;
extern     PFNGLGETSHADERINFOLOGPROC                      glGetShaderInfoLog;
extern     PFNGLUSEPROGRAMPROC                            glUseProgram;
extern     PFNGLCREATESHADERPROC                          glCreateShader;
extern     PFNGLDELETESHADERPROC                          glDeleteShader;
extern     PFNGLDELETEPROGRAMPROC                         glDeleteProgram;
extern     PFNGLCREATEPROGRAMPROC                         glCreateProgram;
extern     PFNGLATTACHSHADERPROC                          glAttachShader;
extern     PFNGLDETACHSHADERPROC                          glDetachShader;
extern     PFNGLLINKPROGRAMPROC                           glLinkProgram;
extern     PFNGLGETPROGRAMIVPROC                          glGetProgramiv;
extern     PFNGLGETPROGRAMINFOLOGPROC                     glGetProgramInfoLog;
extern     PFNGLGETUNIFORMLOCATIONPROC                    glGetUniformLocation;
extern     PFNGLUNIFORM2IVARBPROC                         glUniform2iv;

extern     PFNGLFENCESYNCPROC                             glFenceSync;            ///< ext function
extern     PFNGLISSYNCPROC                                glIsSync;               ///< ext function
extern     PFNGLDELETESYNCPROC                            glDeleteSync;           ///< ext function
extern     PFNGLCLIENTWAITSYNCPROC                        glClientWaitSync;       ///< ext function
extern     PFNGLWAITSYNCPROC                              glWaitSync;             ///< ext function
extern     PFNGLGETSYNCIVPROC                             glGetSynciv;            ///< ext function

#endif