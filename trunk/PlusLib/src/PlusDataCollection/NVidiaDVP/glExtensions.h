#ifndef NVGLEXT_H
#define NVGLEXT_H

#if WIN32
#include <windows.h>
#endif
#include "gl\gl.h"
#include "gl\glext.h"
#include "gl\wglext.h"

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