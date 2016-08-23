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

#ifndef __DVAPI_CUDA_H_
#define __DVAPI_CUDA_H_

#include <cuda.h>
#include "DVPAPI.h"

DVPAPI_INTERFACE
dvpInitCUDAContext(uint32_t flags);

DVPAPI_INTERFACE
dvpCloseCUDAContext();

DVPAPI_INTERFACE
dvpBindToCUDACtx(DVPBufferHandle hBuf);

DVPAPI_INTERFACE
dvpUnbindFromCUDACtx(DVPBufferHandle hBuf);

DVPAPI_INTERFACE
dvpCreateGPUCUDAArray(CUarray array, 
                      DVPBufferHandle *bufferHandle);

DVPAPI_INTERFACE
dvpCreateGPUCUDADevicePtr(CUdeviceptr devPtr, 
                          DVPBufferHandle *bufferHandle);

//------------------------------------------------------------------------
// Function:      dvpMapBufferWaitCUDAStream
//
// Description:   dvpMapBufferWaitCUDAStream peforms the same function as
//                dvpMapBufferWaitAPI, but allows the specification of which
//                stream will next use the buffer. This allows for more
//                efficient driver optimisations. The NULL stream can also
//                be specified in which case DVP will synchronise with that
//                stream, but my be less than optimal performance.
//
// Parameters:    gpuBufferHandle[IN]       - buffer to synchorise with DVP
//                stream[IN]                - stream in which buffer was used
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//------------------------------------------------------------------------

DVPAPI_INTERFACE
dvpMapBufferWaitCUDAStream(DVPBufferHandle gpuBufferHandle,
                           CUstream stream);

//------------------------------------------------------------------------
// Function:      dvpMapBufferEndCUDAStream
//
// Description:   dvpMapBufferEndCUDAStream peforms the same function as
//                dvpMapBufferEndAPI, but allows the specification of which
//                stream last used the buffer. This allows for more
//                efficient driver optimisations. The NULL stream can also
//                be specified in which case DVP will synchronise with that
//                stream, but my be less than optimal performance.
//
// Parameters:    gpuBufferHandle[IN]       - buffer to synchorise with DVP
//                stream[IN]                - stream in which buffer will
//                                            be used.
//
// Returns:       DVP_STATUS_OK
//                DVP_STATUS_INVALID_PARAMETER
//                DVP_STATUS_ERROR
//                DVP_STATUS_UNSIGNALED     - returned if the API is 
//                      unable to place a signal in the API compute stream
//------------------------------------------------------------------------

DVPAPI_INTERFACE
dvpMapBufferEndCUDAStream(DVPBufferHandle gpuBufferHandle,
                          CUstream stream);

DVPAPI_INTERFACE
dvpGetRequiredConstantsCUDACtx(uint32_t *bufferAddrAlignment,
                               uint32_t *bufferGPUStrideAlignment,
                               uint32_t *semaphoreAddrAlignment,
                               uint32_t *semaphoreAllocSize,
                               uint32_t *semaphorePayloadOffset,
                               uint32_t *semaphorePayloadSize);

#endif
