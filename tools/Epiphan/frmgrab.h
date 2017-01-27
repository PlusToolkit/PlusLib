/****************************************************************************
 *
 * $Id: frmgrab.h 14478 2011-10-17 20:49:08Z monich $
 *
 * Copyright (C) 2008-2011 Epiphan Systems Inc. All rights reserved.
 *
 * Header file for the Epiphan frame grabber library
 *
 ****************************************************************************/

#ifndef _EPIPHAN_FRMGRAB_H_
#define _EPIPHAN_FRMGRAB_H_ 1

#include "v2u_defs.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _FrmGrabber FrmGrabber;
struct sockaddr_in;
    
/**
 * Initialize/deinitialize frmgrab library. Both functions may be invoked
 * several times, but there must be FrmGrab_Deinit call per each FrmGrab_Init.
 */
void FrmGrab_Init(void);
void FrmGrab_Deinit(void);
void FrmGrabNet_Init(void);
void FrmGrabNet_Deinit(void);

/**
 * Functions specific to local frame grabbers.
 */
FrmGrabber* FrmGrabLocal_Open(void);
FrmGrabber* FrmGrabLocal_OpenSN(const char* sn);
FrmGrabber* FrmGrabLocal_OpenIndex(int i);
int FrmGrabLocal_Count(void);
int FrmGrabLocal_OpenAll(FrmGrabber* grabbers[], int maxcount);

/**
 * VGA2Ethernet authentication callback. Username buffer pointer is NULL
 * if username is not required. Maximum sizes of username and password
 * are defined below. Non-ASCII username and passwords must be UTF-8
 * encoded. Note that FG_USERNAME_SIZE and FG_PASSWORD_SIZE define
 * maximum number of bytes (including NULL terminator), not characters.
 * In case of UTF-8 encoding it's not the same thing.
 */
typedef V2U_BOOL (*FrmGrabAuthProc)(char* user, char* pass, void* param);
#define FG_USERNAME_SIZE 32
#define FG_PASSWORD_SIZE 64

/**
 * Connection status returned by FrmGrabNet_OpenAddress2 
 */
typedef enum _FrmGrabConnectStatus {
    FrmGrabConnectOK,           /* Connection OK, authentication not needed */
    FrmGrabConnectAuthOK,       /* Connection OK, authentication successful */
    FrmGrabConnectError,        /* Connection failed */
    FrmGrabConnectAuthFail,     /* Authentication failure */
    FrmGrabConnectAuthCancel    /* Authentication was cancelled */
} FrmGrabConnectStatus;

/**
 * VGA2Ethernet specific functions. IP address is in the host byte order.
 */
FrmGrabber* FrmGrabNet_Open(void);
FrmGrabber* FrmGrabNet_OpenSN(const char* sn);
FrmGrabber* FrmGrabNet_OpenLocation(const char* location);
FrmGrabber* FrmGrabNet_OpenAddress(V2U_UINT32 ipaddr, V2U_UINT16 port);
FrmGrabber* FrmGrabNet_OpenAddress2(V2U_UINT32 ipaddr, V2U_UINT16 port,
    FrmGrabAuthProc authproc, void* param, FrmGrabConnectStatus* status);

/**
 * VGA2Ethernet authentication. These functions fail (but don't crash) on
 * local frame grabbers.
 */
V2U_BOOL FrmGrabNet_IsProtected(FrmGrabber* fg);
FrmGrabConnectStatus FrmGrabNet_Auth(FrmGrabber* fg, FrmGrabAuthProc authproc,
    void* param);

/**
 * Simple non-interactive username/password authentication. VGA2Ethernet
 * grabbers normally don't use usernames for viewer authentication, you
 * should pass NULL as the second parameter.
 *
 * Available since version 3.26.0.15
 */
FrmGrabConnectStatus FrmGrabNet_Auth2(FrmGrabber* fg, const char* user,
    const char* pass);

/**
 * Network statistics for a network frame grabber. These functions return
 * V2U_FALSE if invoked on a local frame grabber.
 */
typedef struct _FrmGrabNetStat {
    V2U_UINT64 bytesSent;       /* Total bytes sent over the network */
    V2U_UINT64 bytesReceived;   /* Total bytes received from the network */
} FrmGrabNetStat;

V2U_BOOL FrmGrabNet_GetStat(FrmGrabber* fg, FrmGrabNetStat* netstat);
V2U_BOOL FrmGrabNet_GetRemoteAddr(FrmGrabber* fg, struct sockaddr_in* addr);

/* Other network-specific functions */
V2U_BOOL FrmGrabNet_IsAlive(FrmGrabber* fg);
V2U_BOOL FrmGrabNet_SetAutoReconnect(FrmGrabber* fg, V2U_BOOL enable);

/** Generic functions that work with all Frame Grabbers */

/**
 * Open frame grabber. The URL-like location parameter can be one of the
 * following:
 *
 * local:[SERIAL]
 *
 *   Opens a local frame grabber. Optionally, the serial number can be
 *   specified.
 *
 * net:[ADDRESS[:PORT]]
 *
 *   Opens a network frame grabber at the specified address/port. If no
 *   address is specified, opens a random network frame grabber.
 *
 * sn:SERIAL
 *
 *   Opens a local or network frame grabber with the specified serial number.
 *   Checks the local frame grabbers first then goes to the network.
 *
 * id:INDEX
 *
 *   Opens a local frame grabber with the specified index.
 *
 */
FrmGrabber* FrmGrab_Open(const char* location);

/**
 * Duplicates handle to the frame grabber. Returns a new independent
 * FrmGrabber instance pointing to the same piece of hardware.
 */
FrmGrabber* FrmGrab_Dup(FrmGrabber* fg);

/**
 * Returns frame grabber's serial number.
 */
const char* FrmGrab_GetSN(FrmGrabber* fg);

/**
 * Returns the unique product id. Includes product type bits OR'ed with
 * type-specific product id.
 */
int FrmGrab_GetProductId(FrmGrabber* fg);

#define PRODUCT_ID_MASK     0x0000ffff
#define PRODUCT_TYPE_MASK   0x00ff0000
#define PRODUCT_TYPE_USB    0x00010000  /* Local USB grabber */
#define PRODUCT_TYPE_NET    0x00020000  /* Network grabber */
#define PRODUCT_TYPE_NETAPP 0x00030000  /* Network appliance, not a grabber */
#define PRODUCT_TYPE_FILE   0x00040000  /* Reserved */
#define PRODUCT_TYPE_PCI    0x00050000  /* Local PCI grabber */

#define FrmGrab_IsNetGrabber(fg) \
    ((FrmGrab_GetProductId(fg) & PRODUCT_TYPE_MASK) == PRODUCT_TYPE_NET)

#define FrmGrab_IsUsbGrabber(fg) \
    ((FrmGrab_GetProductId(fg) & PRODUCT_TYPE_MASK) == PRODUCT_TYPE_USB)

#define FrmGrab_IsPciGrabber(fg) \
    ((FrmGrab_GetProductId(fg) & PRODUCT_TYPE_MASK) == PRODUCT_TYPE_PCI)

#define FrmGrab_IsLocalGrabber(fg) \
    (FrmGrab_IsUsbGrabber(fg) || \
     FrmGrab_IsPciGrabber(fg))

/**
 * Returns the product description ("VGA2USB", "VGA2Ethernet", etc)
 */
const char* FrmGrab_GetProductName(FrmGrabber* fg);

/**
 * Returns a string that describes the location of the grabber ("USB,
 * "192.168.0.122", etc)
 */
const char* FrmGrab_GetLocation(FrmGrabber* fg);

/**
 * Returns device capabilities (V2U_CAPS_* flags defined in v2u_defs.h).
 * Returns zero if an error occurs.
 */
V2U_UINT32 FrmGrab_GetCaps(FrmGrabber* fg);

/**
 * Returns the last known video mode, detecting is if necessary.
 * Unlike FrmGrab_DetectVideoMode, this function doesn't force the
 * video mode detection, but most of the time it's accurate enough.
 * Note that video mode detection is a relatively expensive operation.
 */
void FrmGrab_GetVideoMode(FrmGrabber* fg, V2U_VideoMode* vm);

/**
 * Detects current video mode. If no signal is detected, the output
 * V2U_VideoMode structure is zeroed.
 */
V2U_BOOL FrmGrab_DetectVideoMode(FrmGrabber* fg, V2U_VideoMode* vm);

/**
 * Queries current VGA capture parameters.
 */
V2U_BOOL FrmGrab_GetGrabParams(FrmGrabber* fg, V2U_GrabParameters* gp);

/**
 * Queries current VGA capture parameters and adjustment ranges in one shot.
 * More efficient for network grabbers. FrmGrab_GetGrabParams2(fg,gp,NULL)
 * is equivalent to FrmGrab_GetGrabParams(fg,gp)
 */
V2U_BOOL FrmGrab_GetGrabParams2(FrmGrabber* fg, V2U_GrabParameters* gp,
    V2UAdjRange* range);

/**
 * Sets VGA capture parameters.
 */
V2U_BOOL FrmGrab_SetGrabParams(FrmGrabber* fg, const V2U_GrabParameters* gp);

/**
 * Queries the device property.
 */
V2U_BOOL FrmGrab_GetProperty(FrmGrabber* fg, V2U_Property* prop);

/**
 * Sets the device property.
 */
V2U_BOOL FrmGrab_SetProperty(FrmGrabber* fg, const V2U_Property* prop);

/**
 * VGA mode table can be queried or modified with a bunch of GetProperty
 * or SetProperty calls, but getting and settinng the entire VGA mode table
 * with one call is more efficient, especially for network frame grabbers.
 * Number of custom modes doesn't exceed V2U_CUSTOM_VIDEOMODE_COUNT. The
 * whole FrmGrabVgaModes structure returned by FrmGrab_GetVGAModes is
 * allocated as a single memory block. The caller is responsible for
 * deallocating it with with a single FrmGrab_Free call.
 */
typedef struct _FrmGrabVgaModes {
    V2UVideoModeDescr* customModes; /* Custom modes */
    V2UVideoModeDescr* stdModes;    /* Standard modes */
    int numCustomModes;             /* Number of custom modes */
    int numStdModes;                /* Number of standard modes */
} FrmGrabVgaModes;

/**
 * Gets VGA mode table. The returned pointer must be deallocated
 * with FrmGrab_Free()
 */
FrmGrabVgaModes* FrmGrab_GetVGAModes(FrmGrabber* fg);

/**
 * Sets VGA mode table. Custom modes can be modified, standard modes can only
 * enabled or disabled (VIDEOMODE_TYPE_ENABLED flag).
 */
V2U_BOOL FrmGrab_SetVGAModes(FrmGrabber* fg, const FrmGrabVgaModes* vgaModes);

/**
 * Sends PS/2 events to the frame grabber (KVM capable products only).
 */
V2U_BOOL FrmGrab_SendPS2(FrmGrabber* fg, const V2U_SendPS2* ps2);

/**
 * Sets intended frame rate limit (average number of FrmGrab_Frame calls per
 * second). The frame grabber may use this hint to reduce resource usage,
 * especially in low fps case.
 */
V2U_BOOL FrmGrab_SetMaxFps(FrmGrabber* fg, double maxFps);

/**
 * Signals the grabber to prepare for capturing frames with maximum
 * frame rate. While it currently doesn't matter for local grabbers, it's
 * really important for network grabbers (it turns streaming on, otherwise
 * FrmGrab_Frame will have to work on request/response basis, which is
 * much slower). Returns V2U_TRUE if streaming is supported, or V2U_FALSE
 * if this call has no effect.
 */
V2U_BOOL FrmGrab_Start(FrmGrabber* fg);

/**
 * Signals the grabber to stop capturing frames with maximum frame rate.
 */
void FrmGrab_Stop(FrmGrabber* fg);

/**
 * Grabs one frame. The caller doesn't have to call FrmGrab_Start first,
 * but it's recommended in order to get maximum frame rate. 
 *
 * The second parameter is the capture format, i.e. one of
 * V2U_GRABFRAME_FORMAT_* constants defined in v2u_defs.h
 *
 * The last parameter is a pointer to the requested crop rectangle. Pass
 * NULL if you need the whole frame.
 */
V2U_GrabFrame2* FrmGrab_Frame(FrmGrabber* fg, V2U_UINT32 format,
    const V2URect* crop);

/**
 * Releases the frame previously returned by FrmGrab_Frame
 */
void FrmGrab_Release(FrmGrabber* fg, V2U_GrabFrame2* frame);

/**
 * Closes the frame grabber and invalidates the handle.
 */
void FrmGrab_Close(FrmGrabber* fg);

/**
 * Deallocates memory allocated by frmgrab.
 */
void FrmGrab_Free(void* ptr);

/** Memory allocation callbacks */
typedef void* (*FrmGrabMemAlloc)(void* param, V2U_UINT32 len);
typedef void (*FrmGrabMemFree)(void* param, void* ptr);

typedef struct _FrmGrabMemCB {
    FrmGrabMemAlloc alloc;
    FrmGrabMemFree free;
} FrmGrabMemCB;

/**
 * Sets functions for allocating and deallocating memory for the frame
 * buffers. NOTE that these callbacks are ONLY invoked to allocate memory
 * for the frame buffer. For all other allocations, frmgrab library uses
 * its own allocator. The allocator and deallocator callbacks may be invoked
 * in arbitrary thread context, not necessarily the thread that invokes
 * FrmGrab_Frame. The contents of FrmGrabMemCB is copied to internal storage.
 */
void FrmGrab_SetAlloc(FrmGrabber* fg, const FrmGrabMemCB* memcb, void* param);
    
#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _EPIPHAN_FRMGRAB_H_ */

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
