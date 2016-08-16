/*
 * Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO LICENSEE:
 *
 * This source code and/or documentation ("Licensed Deliverables") are
 * subject to NVIDIA intellectual property rights under U.S. and
 * international Copyright laws.
 *
 * These Licensed Deliverables contained herein is PROPRIETARY and
 * CONFIDENTIAL to NVIDIA and is being provided under the terms and
 * conditions of a form of NVIDIA software license agreement by and
 * between NVIDIA and Licensee ("License Agreement") or electronically
 * accepted by Licensee.  Notwithstanding any terms or conditions to
 * the contrary in the License Agreement, reproduction or disclosure
 * of the Licensed Deliverables to any third party without the express
 * written consent of NVIDIA is prohibited.
 *
 * NOTWITHSTANDING ANY TERMS OR CONDITIONS TO THE CONTRARY IN THE
 * LICENSE AGREEMENT, NVIDIA MAKES NO REPRESENTATION ABOUT THE
 * SUITABILITY OF THESE LICENSED DELIVERABLES FOR ANY PURPOSE.  IT IS
 * PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.
 * NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THESE LICENSED
 * DELIVERABLES, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY,
 * NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * NOTWITHSTANDING ANY TERMS OR CONDITIONS TO THE CONTRARY IN THE
 * LICENSE AGREEMENT, IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY
 * SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THESE LICENSED DELIVERABLES.
 *
 * U.S. Government End Users.  These Licensed Deliverables are a
 * "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT
 * 1995), consisting of "commercial computer software" and "commercial
 * computer software documentation" as such terms are used in 48
 * C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government
 * only as a commercial end item.  Consistent with 48 C.F.R.12.212 and
 * 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995), all
 * U.S. Government End Users acquire the Licensed Deliverables with
 * only those rights set forth herein.
 *
 * Any use of the Licensed Deliverables in individual and commercial
 * software must include, in the user documentation and internal
 * comments to the code, the above Disclaimer and U.S. Government End
 * Users Notice.
 */

#ifndef __DVAPI_H_
#define __DVAPI_H_

#include <stdint.h>

#if defined(WIN32)
#define DVPAPI_INTERFACE  __declspec( dllexport ) DVPStatus
#if defined(DVP_STATIC)
// If using a static build, force the linker to include the
// DVP CRT call back, so the DVP can properly cleanup thread
// data. For dynamic library linkage, this is handled by Dllmain.
// Dynamic linkage is recommended.
  #ifdef _WIN64
  #pragma comment(linker, "/INCLUDE:_tls_used")
  #pragma comment(linker, "/INCLUDE:dvp_callback")
  
  #else  // _WIN64
  
  #pragma comment(linker, "/INCLUDE:__tls_used")
  #pragma comment(linker, "/INCLUDE:_dvp_callback")
  
  #endif  // _WIN64
#endif
#else
#define DVPAPI_INTERFACE  extern DVPStatus
#endif

#define DVP_MAJOR_VERSION  1
#define DVP_MINOR_VERSION  70

typedef uint64_t DVPBufferHandle;
typedef uint64_t DVPSyncObjectHandle;

typedef enum
{
    DVP_STATUS_OK                        =  0,
    DVP_STATUS_INVALID_PARAMETER         =  1,
    DVP_STATUS_UNSUPPORTED               =  2,
    DVP_STATUS_END_ENUMERATION           =  3,
    DVP_STATUS_INVALID_DEVICE            =  4,
    DVP_STATUS_OUT_OF_MEMORY             =  5,
    DVP_STATUS_INVALID_OPERATION         =  6,
    DVP_STATUS_TIMEOUT                   =  7,
    DVP_STATUS_INVALID_CONTEXT           =  8,
    DVP_STATUS_INVALID_RESOURCE_TYPE     =  9,
    DVP_STATUS_INVALID_FORMAT_OR_TYPE    =  10,
    DVP_STATUS_DEVICE_UNINITIALIZED      =  11,
    DVP_STATUS_UNSIGNALED                =  12,
    DVP_STATUS_SYNC_ERROR                =  13,
    DVP_STATUS_SYNC_STILL_BOUND          =  14,
    DVP_STATUS_ERROR                     = -1
} DVPStatus;

// Pixel component formats stored in the system memory buffer
// analogous to those defined in the OpenGL API, except for 
// DVP_BUFFER and the DVP_CUDA_* types. DVP_BUFFER provides 
// an unspecified format type to allow for general interpretation
// of the bytes at a later stage (in GPU shader). Note that not 
// all paths will achieve optimal speeds due to lack of HW support 
// for the transformation. The CUDA types are to be used when
// copying to/from a system memory buffer from-to a CUDA array, as the 
// CUDA array implies a memory layout that matches the array.
typedef enum
{
    DVP_BUFFER,                   // Buffer treated as a raw buffer 
                                  // and copied directly into GPU buffer
                                  // without any interpretation of the
                                  // stored bytes.
    DVP_DEPTH_COMPONENT,
    DVP_RGBA,
    DVP_BGRA,
    DVP_RED,
    DVP_GREEN,
    DVP_BLUE,
    DVP_ALPHA,
    DVP_RGB,
    DVP_BGR,
    DVP_LUMINANCE,
    DVP_LUMINANCE_ALPHA,
    DVP_CUDA_1_CHANNEL,
    DVP_CUDA_2_CHANNELS,
    DVP_CUDA_4_CHANNELS,
    DVP_RGBA_INTEGER,
    DVP_BGRA_INTEGER,
    DVP_RED_INTEGER,
    DVP_GREEN_INTEGER,
    DVP_BLUE_INTEGER,
    DVP_ALPHA_INTEGER,
    DVP_RGB_INTEGER,
    DVP_BGR_INTEGER,
    DVP_LUMINANCE_INTEGER,
    DVP_LUMINANCE_ALPHA_INTEGER
} DVPBufferFormats;

// Possible pixel component storage types for system memory buffers
typedef enum
{
    DVP_UNSIGNED_BYTE,
    DVP_BYTE,
    DVP_UNSIGNED_SHORT,
    DVP_SHORT,
    DVP_UNSIGNED_INT,
    DVP_INT,
    DVP_FLOAT,
    DVP_HALF_FLOAT,
    DVP_UNSIGNED_BYTE_3_3_2,
    DVP_UNSIGNED_BYTE_2_3_3_REV,
    DVP_UNSIGNED_SHORT_5_6_5,
    DVP_UNSIGNED_SHORT_5_6_5_REV,
    DVP_UNSIGNED_SHORT_4_4_4_4,
    DVP_UNSIGNED_SHORT_4_4_4_4_REV,
    DVP_UNSIGNED_SHORT_5_5_5_1,
    DVP_UNSIGNED_SHORT_1_5_5_5_REV,
    DVP_UNSIGNED_INT_8_8_8_8,
    DVP_UNSIGNED_INT_8_8_8_8_REV,
    DVP_UNSIGNED_INT_10_10_10_2,
    DVP_UNSIGNED_INT_2_10_10_10_REV
} DVPBufferTypes;

// System memory descriptor describing the size and storage formats
// of the buffer
typedef struct DVPSysmemBufferDescRec {
    uint32_t width;                     // Buffer Width
    uint32_t height;                    // Buffer Height
    uint32_t stride;                    // Stride
    uint32_t size;                      // Specifies the surface size if 
                                        // format == DVP_BUFFER
    DVPBufferFormats format;            // see enum above
    DVPBufferTypes type;                // see enum above
    void *bufAddr;                      // Buffer memory address
} DVPSysmemBufferDesc;

// Flags specified at sync object creation:
// ----------------------------------------
// Tells the implementation to use events wherever
// possible instead of software spin loops. Note if HW
// wait operations are supported by the implementation
// then events will not be used in the dvpMemcpy*
// functions. In such a case, events may still be used
// in dvpSyncObjClientWait* functions.
#define DVP_SYNC_OBJECT_FLAGS_USE_EVENTS      0x00000001

typedef struct DVPSyncObjectDescRec {
    uint32_t *sem;               // Location to write semaphore value
    uint32_t  flags;             // See above DVP_SYNC_OBJECT_FLAGS_* bits
    DVPStatus (*externalClientWaitFunc) (DVPSyncObjectHandle sync, 
                                         uint32_t value,
                                         bool GEQ, // If true then the function should wait for the sync value to be 
                                                   // greater than or equal to the value parameter. Otherwise just a
                                                   // straight forward equality comparison should be performed.
                                         uint64_t timeout);
                                         // If non-null, externalClientWaitFunc allows the DVP library
                                         // to call the application to wait for a sync object to be
                                         // released. This allows the application to create events, 
                                         // which can be triggered on device interrupts instead of
                                         // using spin loops inside the DVP library. Upon succeeding
                                         // the function must return DVP_STATUS_OK, non-zero for failure 
                                         // and DVP_STATUS_TIMEOUT on timeout. The externalClientWaitFunc should
                                         // not alter the current GL or CUDA context state
} DVPSyncObjectDesc;

// Time used when event timeouts should be ignored
#define DVP_TIMEOUT_IGNORED                   0xFFFFFFFFFFFFFFFFull

// Flags supplied to the dvpInit* functions:
//
// DVP_DEVICE_FLAGS_SHARE_APP_CONTEXT is only supported for OpenGL
// contexts and is the only supported flag for CUDA. It allows for 
// certain cases to be optimized by sharing the context 
// of the application for the DVP operations. This removes the
// need to do certain synchronizations. See issue 5 for parallel
// issues. When used, the app's GL context must be current for all calls 
// to the DVP library.
// the DVP library.
#define DVP_DEVICE_FLAGS_SHARE_APP_CONTEXT    0x000000001

//------------------------------------------------------------------------
// Function:      dvpInit*
//
//                To be called before any DVP resources are allocated.
//                This call allows for specification of flags that may
//                change the way DVP operations are performed. See above
//                for the list of flags.
//
//                If OpenGL or CUDA is used, the OpenGL/CUDA context
//                must be current at time of call.
//
// Parameters:    flags[IN]  - Buffer description structure
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER 
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpClose*
//
//                Function to be called when app closes to allow freeing
//                of any DVP library allocated resources.
//
//                If OpenGL or CUDA is used, the OpenGL/CUDA context
//                must be current at time of call.
//
// Parameters:    none
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER    
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpBegin 
//
// Description:   dvpBegin must be called before any combination of DVP
//                function calls dvpMemCpy*, dvpMapBufferWaitDVP,
//                dvpSyncObjClientWait*, and dvpMapBufferEndDVP. After 
//                the last of these functions has been called is dvpEnd
//                must be called. This allows for more efficient batched 
//                DVP operations.
//
// Parameters:    none
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpBegin();

//------------------------------------------------------------------------
// Function:      dvpEnd
//
// Description:   dvpEnd signals the end of a batch of DVP function calls
//                that began with dvpBegin
//
// Parameters:    none
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpEnd();

//------------------------------------------------------------------------
// Function:      dvpCreateBuffer
//
// Description:   Create a DVP buffer using system memory, wrapping a user
//                passed pointer. The pointer must be aligned 
//                to values returned by dvpGetRequiredAlignments*
//
// Parameters:    desc[IN]  - Buffer description structure
//                hBuf[OUT] - DVP Buffer handle
// 
// Returns:       DVP_STATUS_OK                
//                DVP_STATUS_INVALID_PARAMETER 
//                DVP_STATUS_ERROR           
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpCreateBuffer(DVPSysmemBufferDesc *desc,
                DVPBufferHandle *hBuf);

//------------------------------------------------------------------------
// Function:      dvpDestroyBuffer
//
// Description:   Destroy a previously created DVP buffer.
//
// Parameters:    hBuf[IN] - DVP Buffer handle
// 
// Returns:       DVP_STATUS_OK                
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR  
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpDestroyBuffer(DVPBufferHandle  hBuf);

//------------------------------------------------------------------------
// Function:      dvpBindTo*
//
// Description:   Bind a previously created system memory buffer to an 
//                OpenGL context, CUDA context or D3D[9,10,11] device. 
//                For OpenGL and CUDA the context must be current at 
//                the time of bind.
//
// Parameters:    hBuf[IN]      - DVP Buffer handle
//                device[IN]    - D3D device if used
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpUnbindFrom*
//
// Description:   Unbind a previously bound buffer from an API context. 
//                For OpenGL and CUDA the context must be current at 
//                the time of bind.
//
// Parameters:    hBuf[IN]   - DVP Buffer handle
//                device[IN] - D3D device if used
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpCreateGPU*
//
// Description:   dvpCreateGPU* creates a DVP buffer reference from a named
//                [buffer,texture,resource]. If OpenGL or CUDA is used, the 
//                OpenGL/CUDA context must be current at time of call.
//
//                For D3D resources, as soon as dvpCreateGPUD3D* is called
//                the resource is mapped for use by the DVP library.
//                dvpMapBufferWaitAPI must be called before the resource
//                is used.
//
// Parameters:    bufferID[IN]      - [buffer,texture,resource] to import
//                bufferHandle[OUT] - DVP Buffer handle
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER  
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpFreeBuffer
//
// Description:   dvpFreeBuffer frees the DVP buffer reference
//
// Parameters:    gpuBufferHandle[IN] - DVP Buffer handle
// 
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpFreeBuffer(DVPBufferHandle gpuBufferHandle);

//------------------------------------------------------------------------
// Function:      dvpGetRequiredConstants*
//
// Description:   dvpGetRequiredConstants* returns, for the associated 
//                API context or device:
//                - Alignments required for the start address of the
//                  buffer.
//                - An alignment recommendation for the stride (pitch)
//                  of the buffer, as the performance might vary based
//                  on the stride alignment. This is a conservative value
//                  so smaller values may still give optimal performance.
//                  Please refer to the SDK for more details.
//                - Semaphore Addr alignment
//                - Semaphore allocation size
//                - Semaphore payload offset
//                - Semaphore payload size in bytes
//
//                If OpenGL or CUDA is used, the OpenGL/CUDA context
//                must be current at time of call.
//
//                On Windows Vista and later dvpGetRequiredConstants* 
//                may return DVP_STATUS_OUT_OF_MEMORY when the system
//                limit for graphics contexts has been reached. To avoid
//                this, spurious graphics/compute contexts should be avoided.
//
// Parameters:    bufferAddrAlignment[OUT]
//                bufferGPUStrideAlignment[OUT]
//                semaphoreAddrAlignment[OUT]
//                semaphoreAllocSize[OUT]
//                semaphorePayloadOffset[OUT]
//                semaphorePayloadSize[OUT]
//                device[IN] - D3D device if used
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//                DVP_STATUS_OUT_OF_MEMORY
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpMemcpy* general comments that apply to all the the 
//                memcpy functions.
//
//                dvpMemcpyLined acquire the srcSync and then initiates 
//                a DMA transfer of the given amount of lines from the 
//                srcBuffer to the dstBuffer at a given line. 
//                Upon completion of a transfer, a release operation is 
//                performed on dstSync. The buffer is only usable once the 
//                release operation is complete.
//
//                For OpenGL and CUDA sources or destinations the correct
//                context must be current.
//
//                When possible the implementation will use HW based 
//                synchronization and in these cases the timeout parameter
//                is not supported, as the call will return immediately and
//                the HW will perform the wait. When SW synchronization is 
//                in use, the wait will timeout after the specified time. 
//                The actual implementation time resolution may differ from 
//                the nanoseconds resolution of the parameter. To ignore
//                the time out use the value DVP_TIMEOUT_IGNORED.
//
//                If the timeout is hit, the corresponding copy is not 
//                performed.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Function:      dvpMemcpyLined
//
// Description:   dvpMemcpyLined provides buffer copies between a
//                DVP sysmem buffer and a graphics API texture (as opposed to
//                a buffer type). Other buffer types (such
//                as graphics API buffers) return DVP_STATUS_INVALID_PARAMETER.
//
//                In addition, see "dvpMemcpy* general comments" above.
//
// Parameters:    srcBuffer[IN]        - src buffer handle
//                srcSync[IN]          - sync to acquire on before transfer
//                srcAcquireValue[IN]  - value to acquire on before transfer
//                timeout[IN]          - time out value in nanoseconds.
//                dstBuffer[IN]        - src buffer handle
//                dstSync[IN]          - sync to release on transfer completion
//                dstReleaseValue[IN]  - value to release on completion
//                startingLine[IN]     - starting line of buffer
//                numberOfLines[IN]    - number of lines to copy
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//
// GL state effected: The following GL state may be altered by this
//               function (not relevant if no GL source or destination
//               is used):
//                -GL_PACK_SKIP_ROWS, GL_PACK_SKIP_PIXELS, 
//                 GL_PACK_ROW_LENGTH
//                -The buffer bound to GL_PIXEL_PACK_BUFFER
//                -The current bound framebuffer (GL_FRAMEBUFFER_EXT)
//                -GL_UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_PIXELS,
//                 GL_UNPACK_ROW_LENGTH
//                -The buffer bound to GL_PIXEL_UNPACK_BUFFER
//                -The texture bound to GL_TEXTURE_2D
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMemcpyLined(DVPBufferHandle      srcBuffer,
                DVPSyncObjectHandle  srcSync,
                uint32_t             srcAcquireValue,
                uint64_t             timeout,
                DVPBufferHandle      dstBuffer,
                DVPSyncObjectHandle  dstSync,
                uint32_t             dstReleaseValue,
                uint32_t             startingLine,
                uint32_t             numberOfLines);

//------------------------------------------------------------------------
// Function:      dvpMemcpy
//
// Description:   dvpMemcpy provides buffer copies between a
//                DVP sysmem buffer and a graphics API pure buffer (as 
//                opposed to a texture type). Other buffer types (such
//                as graphics API textures) return 
//                DVP_STATUS_INVALID_PARAMETER.
//
//                The start address of the srcBuffer is given by srcOffset
//                and the dstBuffer start address is given by dstOffset.
//
//                In addition, see "dvpMemcpy* general comments" above.
//
// Parameters:    srcBuffer[IN]             - src buffer handle
//                srcSync[IN]               - sync to acquire on before transfer
//                srcAcquireValue[IN]       - value to acquire on before transfer
//                timeout[IN]               - time out value in nanoseconds.
//                dstBuffer[IN]             - src buffer handle
//                dstSync[IN]               - sync to release on completion
//                dstReleaseValue[IN]       - value to release on completion
//                uint32_t srcOffset[IN]    - byte offset of srcBuffer
//                uint32_t dstOffset[IN]    - byte offset of dstBuffer
//                uint32_t count[IN]        - number of bytes to copy
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//
// GL state effected: The following GL state may be altered by this
//               function (not relevant if no GL source or destination
//               is used):
//                 - The buffer bound to GL_COPY_WRITE_BUFFER
//                 - The buffer bound to GL_COPY_READ_BUFFER
// 
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMemcpy(DVPBufferHandle      srcBuffer,
            DVPSyncObjectHandle  srcSync,
            uint32_t             srcAcquireValue,
            uint64_t             timeout,
            DVPBufferHandle      dstBuffer,
            DVPSyncObjectHandle  dstSync,
            uint32_t             dstReleaseValue,
            uint32_t             srcOffset,
            uint32_t             dstOffset,
            uint32_t             count);

//------------------------------------------------------------------------
// Function:      dvpMemcpy2D
//
// Description:   dvpMemcpy2D provides buffer copies between a
//                DVP sysmem buffer and a graphics API texture (as opposed
//                to a buffer type). Other buffer types (such
//                as graphics API buffers) return 
//                DVP_STATUS_INVALID_PARAMETER.
//
//                The start address of the copy is calculated as 
//                startPtr = baseAddress + pitch*startY + startX*elementSize,
//                where the baseAdress and pitch are taken from the src 
//                and dst buffer properties.
//
//                In addition, see "dvpMemcpy* general comments" above.
//
// Parameters:    srcBuffer[IN]          - src buffer handle
//                srcSync[IN]            - sync to acquire before transfer
//                srcAcquireValue[IN]    - value to acquire before transfer
//                timeout[IN]            - time out value in nanoseconds.
//                dstBuffer[IN]          - src buffer handle
//                dstSync[IN]            - sync to release on completion
//                dstReleaseValue[IN]    - value to release on completion
//                uint32_t startY[IN]    - Y position of src rectangle
//                uint32_t startX[IN]    - starting X position in bytes
//                uint32_t height[IN]    - height of sub rectangle
//                uint32_t width[IN]     - width of sub rectangle in bytes
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//
// GL state effected: The following GL state may be altered by this
//               function (not relevant if no GL source or destination
//               is used):
//                -GL_PACK_SKIP_ROWS, GL_PACK_SKIP_PIXELS, 
//                 GL_PACK_ROW_LENGTH
//                -The buffer bound to GL_PIXEL_PACK_BUFFER
//                -The current bound framebuffer (GL_FRAMEBUFFER_EXT)
//                -GL_UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_PIXELS,
//                 GL_UNPACK_ROW_LENGTH
//                -The buffer bound to GL_PIXEL_UNPACK_BUFFER
//                -The texture bound to GL_TEXTURE_2D
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMemcpy2D(DVPBufferHandle      srcBuffer,
            DVPSyncObjectHandle  srcSync,
            uint32_t             srcAcquireValue,
            uint64_t             timeout,
            DVPBufferHandle      dstBuffer,
            DVPSyncObjectHandle  dstSync,
            uint32_t             dstReleaseValue,
            uint32_t             startY,
            uint32_t             startX,
            uint32_t             height,
            uint32_t             width);

//------------------------------------------------------------------------
// Function:      dvpMapBufferEndAPI
//
// Description:   Tells DVP to setup a signal for this buffer in the
//                callers API context or device. The signal follows all
//                previous API operations up to this point and, thus,
//                allows subsequent DVP calls to know when then this buffer
//                is ready for use within the DVP library. This function
//                would be followed by a call to dvpMapBufferWaitDVP to
//                synchronize rendering in the API stream and the DVP 
//                stream.
//
//                If OpenGL or CUDA is used, the OpenGL/CUDA context
//                must be current at time of call.
//
//                The use of dvpMapBufferEndAPI is NOT recommended for
//                CUDA synchronisation, as it is more optimal to use a
//                applcation CUDA stream in conjunction with 
//                dvpMapBufferEndCUDAStream. This allows the driver to 
//                do optimisations, such as parllelise the copy operations
//                and compute.
//
//                This must be called outside the dvpBegin/dvpEnd pair. In
//                addition, this call is not thread safe and must be called
//                from or fenced against the rendering thread associated with
//                the context or device.
//
// Parameters:    gpuBufferHandle[IN] - buffer to track
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//                DVP_STATUS_UNSIGNALED     - returned if the API is 
//                     unable to place a signal in the API context queue
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMapBufferEndAPI(DVPBufferHandle gpuBufferHandle);

//------------------------------------------------------------------------
// Function:      dvpMapBufferEndDVP
//
// Description:   Tells DVP to setup a signal for this buffer after
//                DVP operations are complete. The signal allows 
//                the API to know when then this buffer is 
//                ready for use within a API stream. This function would
//                be followed by a call to dvpMapBufferWaitAPI to
//                synchronize copies in the DVP stream and the API 
//                rendering stream.
//
//                This must be called inside the dvpBegin/dvpEnd pair.
//
// Parameters:    gpuBufferHandle[IN] - buffer to track
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMapBufferEndDVP(DVPBufferHandle gpuBufferHandle);

//------------------------------------------------------------------------
// Function:      dvpMapBufferWaitAPI
//
// Description:   Tells DVP to make the current API context or device to 
//                wait for a previous signal triggered by a 
//                dvpMapBufferEndDVP call.
//
//                The use of dvpMapBufferWaitCUDAStream is NOT recommended for
//                CUDA synchronisation, as it is more optimal to use a
//                applcation CUDA stream in conjunction with 
//                dvpMapBufferEndCUDAStream. This allows the driver to 
//                do optimisations, such as parllelise the copy operations
//                and compute.
//
//                If OpenGL or CUDA is used, the OpenGL/CUDA context
//                must be current at time of call.
//
//                This must be called outside the dvpBegin/dvpEnd pair. In
//                addition, this call is not thread safe and must be called
//                from or fenced against the rendering thread associated with
//                the context or device.
//
// Parameters:    gpuBufferHandle[IN] - buffer to track
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMapBufferWaitAPI(DVPBufferHandle gpuBufferHandle);

//------------------------------------------------------------------------
// Function:      dvpMapBufferWaitDVP
//
// Description:   Tells DVP to make the DVP stream wait for a previous 
//                signal triggered by a dvpMapBufferEndAPI call.
//
//                This must be called inside the dvpBegin/dvpEnd pair.
//
// Parameters:    gpuBufferHandle[IN] - buffer to track
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpMapBufferWaitDVP(DVPBufferHandle gpuBufferHandle);

//------------------------------------------------------------------------
// Function:      dvpSyncObjCompletion
//
// Description:   dvpSyncObjCompletion returns the time of completion
//                of the last completed GPU release operation. This time is
//                nanoseconds from and is in the same time domain as that
//                defined in GL_ARB_timer_query.
//
// Parameters:    syncObject[IN] - syncObject
//                timeStamp[OUT] - return time in nanoseconds
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpSyncObjCompletion(DVPSyncObjectHandle syncObject,
                        uint64_t *timeStamp);

//------------------------------------------------------------------------
// Function:      dvpImportSyncObject
//
// Description:   dvpImportSyncObject creates a DVPSyncObject from the 
//                DVPSyncObjectDesc. Note that a sync object is not 
//                supported for copy operations targeting different APIs.
//                This means, for example, it is illegal to call dvpMemCpy*
//                for source or target GL texture with sync object A and 
//                then later use that same sync object in dvpMemCpy* 
//                operation for a source or target CUDA buffer. The same
//                semaphore memory can still be used for two different sync
//                objects.
//
// Parameters:    desc[IN]        - data describing the sync object
//                syncObject[OUT] - handle to sync object
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpImportSyncObject(DVPSyncObjectDesc *desc, 
                    DVPSyncObjectHandle *syncObject);

//------------------------------------------------------------------------
// Function:      dvpFreeSyncObject
//
// Description:   dvpFreeSyncObject waits for any outstanding releases on 
//                this sync object before freeing the resources allocated for
//                the specified sync object. The application must make sure
//                any outstanding acquire operations have already been
//                completed.
//
//                If OpenGL is being used and the app's GL context is being
//                shared (via the DVP_DEVICE_FLAGS_SHARE_APP_CONTEXT flag),
//                then dvpFreeSyncObject needs to be called while each context,
//                on which the sync object was used, is current. If 
//                DVP_DEVICE_FLAGS_SHARE_APP_CONTEXT is used and there are out
//                standing contexts from which this sync object must be free'd
//                then dvpFreeSyncObject will return DVP_STATUS_SYNC_STILL_BOUND.
//
// Parameters:    syncObject[IN] - handle to sync object to be free'd
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//                DVP_STATUS_SYNC_STILL_BOUND
//------------------------------------------------------------------------

DVPAPI_INTERFACE
dvpFreeSyncObject(DVPSyncObjectHandle syncObject);

//------------------------------------------------------------------------
// Function:      dvpSyncObjClientWaitComplete
//
// Description:   dvpSyncObjClientWaitComplete blocks until the  
//                corresponding sync object's semaphore is greater than
//                or equal to the last release value issued by the 
//                DVP library. If the sync object has not yet been 
//                released, then the return value will be
//                DVP_STATUS_INVALID_OPERATION.
//
//                Function definition changes with library version 1.30:
//                This must be called inside a dvpBegin/dvpEnd pair.
//
// Parameters:    syncObject[IN] - handle to sync object
//                timeout[IN]    - time out value in nanoseconds.
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_INVALID_OPERATION
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpSyncObjClientWaitComplete(DVPSyncObjectHandle syncObject,
                                uint64_t timeout);

//------------------------------------------------------------------------
// Function:      dvpSyncObjClientWaitPartial
//
// Description:   dvpSyncObjClientWaitPartial blocks until the  
//                corresponding sync object's semaphore greater than
//                or equal to the supplied value.  If the sync object 
//                has not yet been released, then the return value 
//                will be DVP_STATUS_INVALID_OPERATION.
//
// Parameters:    syncObject[IN] - handle to sync object
//                value[IN]      - value to wait for
//                timeout[IN]    - time out value in nanoseconds.
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_INVALID_OPERATION
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpSyncObjClientWaitPartial(DVPSyncObjectHandle syncObject,
                            uint32_t value,
                            uint64_t timeout);

//------------------------------------------------------------------------
// Function:      dvpGetLibrayVersion
//
// Description:   Returns the current version of the library
//
// Parameters:    major[OUT]     - returned major version
//                minor[OUT]     - returned minor version
//
// Returns:       DVP_STATUS_OK
//------------------------------------------------------------------------
DVPAPI_INTERFACE
dvpGetLibrayVersion(uint32_t *major,
                    uint32_t *minor);

#endif // __DVAPI_H_

