/****************************************************************************
 *
 * $Id: v2u_dshow.h 12436 2011-02-18 17:03:12Z monich $
 *
 * Copyright (C) 2009-2011 Epiphan Systems Inc. All rights reserved.
 *
 * VGA2USB driver for Windows.
 * Declaration of VGA2USB proprietary property set
 *
 ****************************************************************************/

#ifndef _VGA2USB_V2U_DSHOW_H_
#define _VGA2USB_V2U_DSHOW_H_ 1

/* {9B2C649F-CAE6-4745-8D09-413DF0562C4B} */
#define STATIC_PROPSETID_V2U_PROPSET \
    0x9b2c649fL, 0xcae6, 0x4745, 0x8d, 0x09, 0x41, 0x3d, 0xf0, 0x56, 0x2c, 0x4b

/*
 * Map between DirectShow PROP_GET properties and V2U properties
 */
#define V2U_DSHOW_PROP_MAP(m) \
    m( V2U_DSHOW_PROP_GET_DSFLAGS,          V2UKey_DirectShowFlags      ) \
    m( V2U_DSHOW_PROP_GET_DSFIXRES,         V2UKey_DirectShowFixRes     ) \
    m( V2U_DSHOW_PROP_GET_DSBITMAP,         V2UKey_DirectShowDefaultBmp ) \
    m( V2U_DSHOW_PROP_GET_DSSCALEMODE,      V2UKey_DirectShowScaleMode  ) \
    m( V2U_DSHOW_PROP_GET_DSMAXRATE,        V2UKey_DirectShowMaxFps     ) \
    m( V2U_DSHOW_PROP_GET_USERDATA,         V2UKey_UserData             ) \
    m( V2U_DSHOW_PROP_GET_PROD_ID,          V2UKey_UsbProductID         ) \
    m( V2U_DSHOW_PROP_GET_PROD_TYPE,        V2UKey_ProductType          ) \
    m( V2U_DSHOW_PROP_GET_PROD_NAME,        V2UKey_ProductName          ) \
    m( V2U_DSHOW_PROP_GET_VGAMODE_INFO,     V2UKey_ModeMeasurmentsDump  ) \
    m( V2U_DSHOW_PROP_GET_HW_COMPRESSION,   V2UKey_HardwareCompression  ) \
    m( V2U_DSHOW_PROP_GET_ADJ_RANGE,        V2UKey_AdjustmentsRange     ) \
    m( V2U_DSHOW_PROP_GET_VERSION,          V2UKey_Version              ) \
    m( V2U_DSHOW_PROP_GET_EDID,             V2UKey_EDID                 ) \
    m( V2U_DSHOW_PROP_GET_KVM_SUPPORT,      V2UKey_KVMCapable           ) \
    m( V2U_DSHOW_PROP_GET_VGAMODE_ENTRY,    V2UKey_VGAMode              ) \
    m( V2U_DSHOW_PROP_GET_VGAMODE,          V2UKey_CurrentVGAMode       ) \
    m( V2U_DSHOW_PROP_GET_MEASURE_INTERVAL, V2UKey_ModeMeasureInterval  ) \
    m( V2U_DSHOW_PROP_GET_EDID_SUPPORT,     V2UKey_EDIDSupport          ) \
    m( V2U_DSHOW_PROP_GET_TUNE_INTERVAL,    V2UKey_TuneInterval         ) \
    m( V2U_DSHOW_PROP_GET_SN,               V2UKey_SerialNumber         ) \
    m( V2U_DSHOW_PROP_GET_SIGNAL_TYPE,      V2UKey_InputSignalType      ) \
    m( V2U_DSHOW_PROP_GET_DVIMODE_DETECT,   V2UKey_DigitalModeDetect    ) \
    m( V2U_DSHOW_PROP_GET_NOISE_FILTER,     V2UKey_NoiseFilter          ) \
    m( V2U_DSHOW_PROP_GET_HSYNC_THRESHOLD,  V2UKey_HSyncThreshold       ) \
    m( V2U_DSHOW_PROP_GET_VSYNC_THRESHOLD,  V2UKey_VSyncThreshold       ) \
    m( V2U_DSHOW_PROP_GET_DEVICE_CAPS,      V2UKey_DeviceCaps           ) \
    m( V2U_DSHOW_PROP_GET_DSBITMAP2,        V2UKey_DirectShowDefaultBmp2) \
    m( V2U_DSHOW_PROP_GET_BUS_TYPE,         V2UKey_BusType              )

#define V2U_DSHOW_PROP_RESERVE(r) \
    r( V2U_DSHOW_PROP_RESERVED_1,  0 ) \
    r( V2U_DSHOW_PROP_RESERVED_2,  0 )

/**
 * Property IDs
 */
#undef V2U_DSHOW_PROP_DEFINE_ID
#define V2U_DSHOW_PROP_DEFINE_ID(id,key)    id,

typedef enum _V2U_DSHOW_PROP {
    V2U_DSHOW_PROP_DETECT_VIDEO_MODE,       /* V2U_VideoMode                */
    V2U_DSHOW_PROP_SET_PROPERTY,            /* Any property                 */

    V2U_DSHOW_PROP_MAP(V2U_DSHOW_PROP_DEFINE_ID)
    V2U_DSHOW_PROP_RESERVE(V2U_DSHOW_PROP_DEFINE_ID)

    V2U_DSHOW_PROP_GRAB_PARAMETERS,         /* V2U_GrabParameters           */
    V2U_DSHOW_PROP_COUNT                    /* Not a valid value!           */
} V2U_DSHOW_PROP;

#undef V2U_DSHOW_PROP_DEFINE_ID

#endif /* _VGA2USB_V2U_DSHOW_H_ */
