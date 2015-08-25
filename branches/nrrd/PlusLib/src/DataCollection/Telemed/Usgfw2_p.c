

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Apr 19 15:36:15 2012
 */
/* Compiler settings for ..\..\..\include\USGFWSDK\include\Usgfw2.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "Usgfw2_h.h"

#define TYPE_FORMAT_STRING_SIZE   1601                              
#define PROC_FORMAT_STRING_SIZE   6247                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   4            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDataView_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDataView_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanMode_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanMode_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDataStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDataStream_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgMixerControl_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgMixerControl_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgData_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgData_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgGraph_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgGraph_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDeviceChangeSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDeviceChangeSink_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IScanDepth_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IScanDepth_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgfw2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgfw2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IProbesCollection_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IProbesCollection_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCollection_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCollection_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IProbe_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IProbe_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IProbe2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IProbe2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBeamformer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBeamformer_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IScanModes_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IScanModes_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgControl_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgControl_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgGain_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgGain_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgValues_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgValues_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgPower_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgPower_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDynamicRange_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDynamicRange_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgFrameAvg_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgFrameAvg_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgRejection2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgRejection2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgProbeFrequency2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDepth_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDepth_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgImageOrientation_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgImageOrientation_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgImageEnhancement_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgImageEnhancement_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgViewArea_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgViewArea_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgLineDensity_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgLineDensity_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgFocus_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgFocus_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgTgc_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgTgc_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgClearView_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgClearView_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgPaletteCalculator_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgPaletteCalculator_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgPalette_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgPalette_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgImageProperties_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgImageProperties_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgControlChangeSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgControlChangeSink_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCtrlChangeCommon_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCtrlChangeCommon_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanLine_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanLine_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanLineProperties_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanLineSelector_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanLineSelector_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgSweepMode_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgSweepMode_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgQualProp_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgQualProp_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgSweepSpeed_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgSweepSpeed_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerColorMap_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorMap_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerColorPriority_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorPriority_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerSteerAngle_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSteerAngle_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerColorThreshold_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorThreshold_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerBaseLine_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerBaseLine_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerPRF_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPRF_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerWindow_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWindow_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerWallFilter_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWallFilter_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerSignalScale_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalScale_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerPacketSize_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPacketSize_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgPulsesNumber_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgPulsesNumber_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCineloop_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCineloop_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCineStream_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCineStream_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCineSink_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCineSink_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgSpatialFilter_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgSpatialFilter_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerSampleVolume_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSampleVolume_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerCorrectionAngle_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerCorrectionAngle_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanConverterPlugin_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanConverterPlugin2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerSignalSmooth_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalSmooth_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgAudioVolume_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgAudioVolume_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerSpectralAvg_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSpectralAvg_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgBioeffectsIndices_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgBioeffectsIndices_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgProbeFrequency3_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency3_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDopplerColorTransparency_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorTransparency_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsg3dVolumeSize_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeSize_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsg3dVolumeDensity_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeDensity_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgFileStorage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgFileStorage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgfw2Debug_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgfw2Debug_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgPlugin_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgPlugin_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBeamformerPowerState_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBeamformerPowerState_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanType_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanType_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgSteeringAngle_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgSteeringAngle_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgViewAngle_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgViewAngle_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCompoundFrames_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCompoundFrames_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgCompoundAngle_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCompoundAngle_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDeviceCapabilities_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDeviceCapabilities_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgUnits_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgUnits_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, float, double or hyper in -Oif or -Oicf.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_AvgFrameRate */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Brightness */


	/* Procedure get_Current */


	/* Procedure get_CurrentMode */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Mirror */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Count */


	/* Procedure get_Current */


	/* Procedure get_Count */


	/* Procedure get_Count */


	/* Procedure GetScanDepth */


	/* Procedure GetStreamMode */


	/* Procedure get_ScanMode */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pScanDepth */


	/* Parameter pMode */


	/* Parameter pVal */

/* 16 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure RemovePlugin */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Brightness */


	/* Procedure put_Current */


	/* Procedure put_CurrentMode */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Mirror */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure SetScanDepth */


	/* Procedure put_ScanMode */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 38 */	NdrFcShort( 0x8 ),	/* 8 */
/* 40 */	NdrFcShort( 0x8 ),	/* 8 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter cookie */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter nScanDepth */


	/* Parameter newVal */

/* 44 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 46 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rotate */


	/* Procedure get_DeviceState */


	/* Procedure get_DeviceState */


	/* Procedure get_ScanState */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x9 ),	/* 9 */
/* 64 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x24 ),	/* 36 */
/* 70 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 72 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rotate */


	/* Procedure put_ScanState */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0xa ),	/* 10 */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	NdrFcShort( 0x8 ),	/* 8 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 100 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 104 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanModes */

/* 112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0xb ),	/* 11 */
/* 120 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 126 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 128 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 130 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 132 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Return value */

/* 134 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 136 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanModeObj */

/* 140 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 142 */	NdrFcLong( 0x0 ),	/* 0 */
/* 146 */	NdrFcShort( 0xc ),	/* 12 */
/* 148 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 152 */	NdrFcShort( 0x8 ),	/* 8 */
/* 154 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter scanMode */

/* 156 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 158 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pUsgScanMode */

/* 162 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 164 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 166 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 170 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FilgraphManager */

/* 174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0xd ),	/* 13 */
/* 182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 188 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 190 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 192 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 194 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 198 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0xe ),	/* 14 */
/* 210 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 212 */	NdrFcShort( 0x54 ),	/* 84 */
/* 214 */	NdrFcShort( 0x8 ),	/* 8 */
/* 216 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter riidCtrl */

/* 218 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 220 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 222 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 224 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 226 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 230 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 232 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppObj */

/* 236 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 238 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 240 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 244 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateUsgfwFile */


	/* Procedure CreateUsgfwFile */

/* 248 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0xf ),	/* 15 */
/* 256 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 260 */	NdrFcShort( 0x8 ),	/* 8 */
/* 262 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter fileName */


	/* Parameter FileName */

/* 264 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 266 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 268 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Parameter createFlags */


	/* Parameter createFlags */

/* 270 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 272 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppFileObject */


	/* Parameter ppFileObject */

/* 276 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 278 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 280 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */


	/* Return value */

/* 282 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 284 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMixerControl */

/* 288 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 290 */	NdrFcLong( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x7 ),	/* 7 */
/* 296 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 298 */	NdrFcShort( 0x10 ),	/* 16 */
/* 300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 302 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter scanMode */

/* 304 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 306 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 312 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppMixerCtrl */

/* 316 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 318 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 320 */	NdrFcShort( 0xb0 ),	/* Type Offset=176 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 324 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x8 ),	/* 8 */
/* 336 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 338 */	NdrFcShort( 0x54 ),	/* 84 */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter riidCtrl */

/* 344 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 346 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 348 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 350 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 352 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 356 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 358 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppCtrl */

/* 362 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 364 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 366 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 370 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateCopy */

/* 374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x9 ),	/* 9 */
/* 382 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 388 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppNewScanMode */

/* 390 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 392 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 394 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 398 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */


	/* Procedure put_Contrast */


	/* Procedure Union */

/* 402 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0xa ),	/* 10 */
/* 410 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 412 */	NdrFcShort( 0x8 ),	/* 8 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter saveFlags */


	/* Parameter newVal */


	/* Parameter nScanMode */

/* 418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 420 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 424 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 426 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StreamsCollection */

/* 430 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0xb ),	/* 11 */
/* 438 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x8 ),	/* 8 */
/* 444 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 446 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 448 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 450 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */

	/* Return value */

/* 452 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 454 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanMode */

/* 458 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 460 */	NdrFcLong( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0xc ),	/* 12 */
/* 466 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x24 ),	/* 36 */
/* 472 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 474 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateClone */

/* 486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0xd ),	/* 13 */
/* 494 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x8 ),	/* 8 */
/* 500 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppNewScanMode */

/* 502 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 504 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 506 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 508 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 510 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 514 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 516 */	NdrFcLong( 0x0 ),	/* 0 */
/* 520 */	NdrFcShort( 0x8 ),	/* 8 */
/* 522 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 524 */	NdrFcShort( 0x44 ),	/* 68 */
/* 526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 528 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter riid */

/* 530 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 532 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 534 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter ppobj */

/* 536 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 538 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 540 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */

	/* Return value */

/* 542 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 544 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMixerControl */

/* 548 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 550 */	NdrFcLong( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x9 ),	/* 9 */
/* 556 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 562 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMixerControl */

/* 564 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 566 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 568 */	NdrFcShort( 0xb0 ),	/* Type Offset=176 */

	/* Return value */

/* 570 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 572 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryLifeRemaining */


	/* Procedure get_TransparencyMode */


	/* Procedure get_Mute */


	/* Procedure get_Enabled */


	/* Procedure get_PriorityMode */


	/* Procedure get_SweepSpeed */


	/* Procedure get_Enabled */


	/* Procedure get_Enabled */


	/* Procedure get_StreamId */

/* 576 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0xa ),	/* 10 */
/* 584 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x24 ),	/* 36 */
/* 590 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 592 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 594 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 598 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 600 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 602 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_PowerLineStatus */


	/* Procedure get_ScanState */

/* 604 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 606 */	NdrFcLong( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0xb ),	/* 11 */
/* 612 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 616 */	NdrFcShort( 0x24 ),	/* 36 */
/* 618 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 620 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 622 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 624 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 626 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 628 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_ScanState */

/* 632 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 634 */	NdrFcLong( 0x0 ),	/* 0 */
/* 638 */	NdrFcShort( 0xc ),	/* 12 */
/* 640 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	NdrFcShort( 0x8 ),	/* 8 */
/* 646 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 648 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 650 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 652 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 654 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 656 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOutputWindow */

/* 660 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 662 */	NdrFcLong( 0x0 ),	/* 0 */
/* 666 */	NdrFcShort( 0x7 ),	/* 7 */
/* 668 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 670 */	NdrFcShort( 0x8 ),	/* 8 */
/* 672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 674 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter hWnd */

/* 676 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 678 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 680 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 682 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 684 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 686 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAlphaBitmapParameters */

/* 688 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 690 */	NdrFcLong( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 696 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x8 ),	/* 8 */
/* 702 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pBmpParams */

/* 704 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 706 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 708 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Return value */

/* 710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 712 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAlphaBitmap */

/* 716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0x9 ),	/* 9 */
/* 724 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 730 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pBmpParams */

/* 732 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 734 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 736 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Return value */

/* 738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 740 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpdateAlphaBitmapParameters */

/* 744 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0xa ),	/* 10 */
/* 752 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 758 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pBmpParams */

/* 760 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 762 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 764 */	NdrFcShort( 0x162 ),	/* Type Offset=354 */

	/* Return value */

/* 766 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 768 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOutputRect */

/* 772 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 774 */	NdrFcLong( 0x0 ),	/* 0 */
/* 778 */	NdrFcShort( 0xb ),	/* 11 */
/* 780 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 782 */	NdrFcShort( 0x34 ),	/* 52 */
/* 784 */	NdrFcShort( 0x8 ),	/* 8 */
/* 786 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pRect */

/* 788 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 790 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 792 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 796 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Capture */

/* 800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 806 */	NdrFcShort( 0xc ),	/* 12 */
/* 808 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 814 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppCurrentImage */

/* 816 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 818 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 820 */	NdrFcShort( 0x186 ),	/* Type Offset=390 */

	/* Return value */

/* 822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 824 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_AutoResolution */


	/* Procedure get_Negative */


	/* Procedure SwitchConnectorSupported */


	/* Procedure GetCurrentBitmap */

/* 828 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0xd ),	/* 13 */
/* 836 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0x24 ),	/* 36 */
/* 842 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter bSupported */


	/* Parameter handle */

/* 844 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 846 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 852 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Repaint */

/* 856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 862 */	NdrFcShort( 0xe ),	/* 14 */
/* 864 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 866 */	NdrFcShort( 0x10 ),	/* 16 */
/* 868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 870 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter hWindow */

/* 872 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 874 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hDC */

/* 878 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 880 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 884 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 886 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanState */

/* 890 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 892 */	NdrFcLong( 0x0 ),	/* 0 */
/* 896 */	NdrFcShort( 0xf ),	/* 15 */
/* 898 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 902 */	NdrFcShort( 0x24 ),	/* 36 */
/* 904 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 906 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 908 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 910 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 912 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 914 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_ScanState */

/* 918 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 920 */	NdrFcLong( 0x0 ),	/* 0 */
/* 924 */	NdrFcShort( 0x10 ),	/* 16 */
/* 926 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 928 */	NdrFcShort( 0x8 ),	/* 8 */
/* 930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 932 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 934 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 936 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 938 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 940 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 942 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BkColor */

/* 946 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 948 */	NdrFcLong( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x11 ),	/* 17 */
/* 954 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 958 */	NdrFcShort( 0x30 ),	/* 48 */
/* 960 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 962 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 964 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 966 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Return value */

/* 968 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 970 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_BkColor */

/* 974 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 976 */	NdrFcLong( 0x0 ),	/* 0 */
/* 980 */	NdrFcShort( 0x12 ),	/* 18 */
/* 982 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 984 */	NdrFcShort( 0x14 ),	/* 20 */
/* 986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 988 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 990 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 992 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 994 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Return value */

/* 996 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 998 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOutputRect */

/* 1002 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1004 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1010 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1016 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pRect */

/* 1018 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1020 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1022 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 1024 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1026 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1028 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeArrive */

/* 1030 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1032 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1036 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1038 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1040 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1042 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1044 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUsgProbe */

/* 1046 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1048 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1050 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter reserved */

/* 1052 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1054 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1058 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1060 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerArrive */

/* 1064 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1066 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1072 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1074 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1076 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1078 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUsgBeamformer */

/* 1080 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1082 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1084 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter reserved */

/* 1086 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1088 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1092 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1094 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeRemove */

/* 1098 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1100 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1104 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1106 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1108 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1110 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1112 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUsgProbe */

/* 1114 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1116 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1118 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter reserved */

/* 1120 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1122 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1126 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerRemove */

/* 1132 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1134 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0xa ),	/* 10 */
/* 1140 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1142 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1144 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1146 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUsgBeamformer */

/* 1148 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1150 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1152 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter reserved */

/* 1154 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1156 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1160 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1162 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeStateChanged */

/* 1166 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1172 */	NdrFcShort( 0xb ),	/* 11 */
/* 1174 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1176 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1178 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1180 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUsgProbe */

/* 1182 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1186 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter reserved */

/* 1188 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1190 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1194 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1196 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerStateChanged */

/* 1200 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1202 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0xc ),	/* 12 */
/* 1208 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1210 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1212 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1214 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUsgBeamformer */

/* 1216 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1218 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1220 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter reserved */

/* 1222 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1224 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanDepthRange */

/* 1234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1240 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1242 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1246 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1248 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pScanDepthMin */

/* 1250 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1252 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pScanDepthMax */

/* 1256 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1258 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1262 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1264 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1266 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ProbesCollection */

/* 1268 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1270 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1274 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1276 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1280 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1282 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 1284 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1286 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1288 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */

	/* Return value */

/* 1290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1292 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BeamformersCollection */

/* 1296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1304 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1310 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 1312 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1314 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1316 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */

	/* Return value */

/* 1318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1320 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataView */

/* 1324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1330 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1332 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1338 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pUnkSource */

/* 1340 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1342 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1344 */	NdrFcShort( 0xca ),	/* Type Offset=202 */

	/* Parameter ppUsgDataView */

/* 1346 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1348 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1350 */	NdrFcShort( 0x1b2 ),	/* Type Offset=434 */

	/* Return value */

/* 1352 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1354 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Invalidate */

/* 1358 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0xa ),	/* 10 */
/* 1366 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1370 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1372 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 1374 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1376 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeamformerNameFromCode */

/* 1380 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1382 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1386 */	NdrFcShort( 0xb ),	/* 11 */
/* 1388 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1390 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1394 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter Code */

/* 1396 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1398 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Name */

/* 1402 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1404 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1406 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Return value */

/* 1408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1410 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeamformerCodeFromName */

/* 1414 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0xc ),	/* 12 */
/* 1422 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1426 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1428 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter Name */

/* 1430 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1432 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1434 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Parameter Code */

/* 1436 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1438 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1442 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1444 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_SerialNumber */


	/* Procedure ProbeNameFromCode */

/* 1448 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1450 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1454 */	NdrFcShort( 0xd ),	/* 13 */
/* 1456 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1458 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1462 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter index */


	/* Parameter Code */

/* 1464 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1466 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter Name */

/* 1470 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1472 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1474 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Return value */


	/* Return value */

/* 1476 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1478 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1480 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProbeCodeFromName */

/* 1482 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1488 */	NdrFcShort( 0xe ),	/* 14 */
/* 1490 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1494 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1496 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter Name */

/* 1498 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1500 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1502 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Parameter Code */

/* 1504 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1506 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1512 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 1516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1522 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1524 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1530 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter lItem */

/* 1532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppUnk */

/* 1538 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1540 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1542 */	NdrFcShort( 0x1da ),	/* Type Offset=474 */

	/* Return value */

/* 1544 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FileName */


	/* Procedure get_Name */


	/* Procedure get_Name */

/* 1550 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1552 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1556 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1558 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1564 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 1566 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1568 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1570 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 1572 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1574 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Probes */


	/* Procedure get_Beamformer */

/* 1578 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1580 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1586 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1590 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1592 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 1594 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1596 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1598 */	NdrFcShort( 0x1da ),	/* Type Offset=474 */

	/* Return value */


	/* Return value */

/* 1600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1602 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DisplayName */


	/* Procedure get_DispalyName */

/* 1606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0xa ),	/* 10 */
/* 1614 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1620 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 1622 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1624 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1626 */	NdrFcShort( 0x1d0 ),	/* Type Offset=464 */

	/* Return value */


	/* Return value */

/* 1628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1630 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_AutoProbeCenter */


	/* Procedure get_Gamma */


	/* Procedure get_Code */

/* 1634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1640 */	NdrFcShort( 0xb ),	/* 11 */
/* 1642 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1646 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1648 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 1650 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1652 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 1656 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1658 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Type */

/* 1662 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1664 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1668 */	NdrFcShort( 0xc ),	/* 12 */
/* 1670 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1674 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1676 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 1678 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1680 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1682 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1684 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1686 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1688 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDevice */

/* 1690 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1692 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1696 */	NdrFcShort( 0xb ),	/* 11 */
/* 1698 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1702 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1704 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 1706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1708 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SwitchConnector */

/* 1712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1718 */	NdrFcShort( 0xc ),	/* 12 */
/* 1720 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1726 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 1728 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1730 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CtlPointsCount */


	/* Procedure get_Code */

/* 1734 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1736 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0xe ),	/* 14 */
/* 1742 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1744 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1746 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1748 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 1750 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1752 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1756 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1758 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 1762 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1764 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1770 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1774 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1776 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter lItem */

/* 1778 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1780 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pScanMode */

/* 1784 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1786 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1790 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1792 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1794 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_ValuesMode */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */


	/* Procedure get_Values */

/* 1796 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1798 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1804 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1808 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1810 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 1812 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1814 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1816 */	NdrFcShort( 0x1f0 ),	/* Type Offset=496 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1818 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1820 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 1824 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1826 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1832 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1836 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1838 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter lItem */

/* 1840 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1842 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 1846 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 1848 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1850 */	NdrFcShort( 0x5ae ),	/* Type Offset=1454 */

	/* Return value */

/* 1852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1854 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TransparencyMode */


	/* Procedure put_Mute */


	/* Procedure put_Enabled */


	/* Procedure put_PriorityMode */


	/* Procedure put_SweepSpeed */


	/* Procedure put_Enabled */


	/* Procedure put_Enabled */

/* 1858 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1864 */	NdrFcShort( 0xb ),	/* 11 */
/* 1866 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1872 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */

/* 1874 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1876 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1880 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1882 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VisibleMin */


	/* Procedure get_VisibleMin */


	/* Procedure PositionToDepth */


	/* Procedure get_FocusSet */

/* 1886 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1888 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1892 */	NdrFcShort( 0xa ),	/* 10 */
/* 1894 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1896 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1898 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1900 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter units */


	/* Parameter units */


	/* Parameter pos */


	/* Parameter mode */

/* 1902 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1904 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter depth */


	/* Parameter pVal */

/* 1908 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1910 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1914 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1916 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1918 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FocusSet */

/* 1920 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1922 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1926 */	NdrFcShort( 0xb ),	/* 11 */
/* 1928 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1930 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1934 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter mode */

/* 1936 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1938 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter newVal */

/* 1942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1944 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1948 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1950 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */


	/* Procedure get_FocusSetCount */

/* 1954 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1956 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1960 */	NdrFcShort( 0xc ),	/* 12 */
/* 1962 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1964 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1966 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1968 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter ctlPoint */


	/* Parameter mode */

/* 1970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1972 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 1976 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1978 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 1982 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1984 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocalZonesCount */

/* 1988 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1990 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1994 */	NdrFcShort( 0xd ),	/* 13 */
/* 1996 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1998 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2000 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2002 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter mode */

/* 2004 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2006 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2010 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2012 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocalZones */

/* 2016 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2018 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2022 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2024 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocalZone */

/* 2028 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2030 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2034 */	NdrFcShort( 0xe ),	/* 14 */
/* 2036 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2038 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2040 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2042 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter mode */

/* 2044 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2046 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2052 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter zoneIndex */

/* 2056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2058 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocalZone */

/* 2062 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2064 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2066 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 2068 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2070 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocusPoint */

/* 2074 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2076 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2080 */	NdrFcShort( 0xf ),	/* 15 */
/* 2082 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2084 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2086 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2088 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter mode */

/* 2090 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2092 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2094 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2096 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2098 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter zoneIndex */

/* 2102 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2104 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusPoint */

/* 2108 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2110 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2116 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocusState */

/* 2120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2126 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2128 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2130 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2134 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter mode */

/* 2136 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2138 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2142 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2146 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2148 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2150 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusState */

/* 2154 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2156 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2158 */	NdrFcShort( 0x5c8 ),	/* Type Offset=1480 */

	/* Return value */

/* 2160 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2162 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFocusState */

/* 2166 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2172 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2174 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2176 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2180 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter mode */

/* 2182 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2188 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2190 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2196 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusState */

/* 2200 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2202 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2204 */	NdrFcShort( 0x5c8 ),	/* Type Offset=1480 */

	/* Return value */

/* 2206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2208 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTgcEntries */

/* 2212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2218 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2220 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2222 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2224 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2226 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter startIndex */

/* 2228 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2230 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2234 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2236 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTgcData */

/* 2240 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2242 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2248 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTgcEntries */

/* 2252 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2260 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2262 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2266 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter startIndex */

/* 2268 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2270 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2274 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2276 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTgcData */

/* 2280 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 2282 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2286 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2288 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2290 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryLifePercent */


	/* Procedure get_OutputPin */


	/* Procedure get_FramesDroppedInRenderer */


	/* Procedure get_Contrast */


	/* Procedure GetTgcSize */

/* 2292 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2294 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2298 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2300 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2304 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2306 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 2308 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2310 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2314 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2316 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VisibleMax */


	/* Procedure get_VisibleMax */


	/* Procedure DepthToPosition */

/* 2320 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2322 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2326 */	NdrFcShort( 0xb ),	/* 11 */
/* 2328 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2332 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2334 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter units */


	/* Parameter units */


	/* Parameter depth */

/* 2336 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pos */

/* 2342 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 2348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2350 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 2354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2360 */	NdrFcShort( 0xd ),	/* 13 */
/* 2362 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2364 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2368 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter ctlPoint */

/* 2370 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2372 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter newVal */

/* 2376 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2378 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2382 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2384 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2386 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CtlPointsCount */

/* 2388 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2390 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2394 */	NdrFcShort( 0xf ),	/* 15 */
/* 2396 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2402 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 2404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2406 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2410 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2412 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CtlPointPos */

/* 2416 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2422 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2424 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2426 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2428 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2430 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter ctlPoint */

/* 2432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2434 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 2438 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2440 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2446 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ValuesTgc */

/* 2450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2456 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2464 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 2466 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2468 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2470 */	NdrFcShort( 0x1f0 ),	/* Type Offset=496 */

	/* Return value */

/* 2472 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2474 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_AutoProbeCenter */


	/* Procedure put_Gamma */

/* 2478 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2480 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2484 */	NdrFcShort( 0xc ),	/* 12 */
/* 2486 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2492 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 2494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2496 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2502 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_AutoResolution */


	/* Procedure put_Negative */

/* 2506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2512 */	NdrFcShort( 0xe ),	/* 14 */
/* 2514 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2520 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 2522 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2524 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2530 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BrightnessMin */

/* 2534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2540 */	NdrFcShort( 0xf ),	/* 15 */
/* 2542 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2546 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2548 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 2550 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2552 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2558 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BrightnessMax */

/* 2562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2568 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2576 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 2578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ContrastMin */

/* 2590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2596 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2598 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2602 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2604 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 2606 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2612 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ContrastMax */

/* 2618 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2620 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2624 */	NdrFcShort( 0x12 ),	/* 18 */
/* 2626 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2630 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2632 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 2634 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2636 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2640 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2642 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2644 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCapabilities */


	/* Procedure get_GammaMin */

/* 2646 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2648 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2652 */	NdrFcShort( 0x13 ),	/* 19 */
/* 2654 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2658 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2660 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pCaps */


	/* Parameter pVal */

/* 2662 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2664 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2670 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_GammaMax */

/* 2674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2680 */	NdrFcShort( 0x14 ),	/* 20 */
/* 2682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2686 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2688 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 2690 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2692 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2696 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2698 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetReferenceData */

/* 2702 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2704 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2708 */	NdrFcShort( 0x15 ),	/* 21 */
/* 2710 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2712 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2714 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2716 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter nValueMax */

/* 2718 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2720 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2722 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nValuesCount */

/* 2724 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2726 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2728 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 2730 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 2732 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2736 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2738 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetReferenceData */

/* 2742 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2744 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2748 */	NdrFcShort( 0x16 ),	/* 22 */
/* 2750 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2752 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2754 */	NdrFcShort( 0x5c ),	/* 92 */
/* 2756 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter pValueMax */

/* 2758 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2760 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2762 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValuesCount */

/* 2764 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 2766 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 2770 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2772 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2776 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2778 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Calculate */

/* 2782 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2784 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2788 */	NdrFcShort( 0x17 ),	/* 23 */
/* 2790 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2792 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2794 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2796 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter startIndex */

/* 2798 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2800 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2804 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2806 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2808 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 2810 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2812 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2816 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2818 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPaletteEntries */

/* 2822 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2824 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2828 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2830 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2832 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2836 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter startIndex */

/* 2838 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2840 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2844 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2846 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPalEntries */

/* 2850 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2852 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2854 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Return value */

/* 2856 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2858 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPaletteEntries */


	/* Procedure GetPaletteEntries */

/* 2862 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2864 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2870 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2872 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2874 */	NdrFcShort( 0x30 ),	/* 48 */
/* 2876 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter startIndex */


	/* Parameter startIndex */

/* 2878 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2880 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */


	/* Parameter entries */

/* 2884 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2886 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPalEntries */


	/* Parameter pPalEntries */

/* 2890 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2892 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2894 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Return value */


	/* Return value */

/* 2896 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2898 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOrigin */

/* 2902 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2904 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2908 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2910 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2912 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2914 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2916 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter point */

/* 2918 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2922 */	NdrFcShort( 0x528 ),	/* Type Offset=1320 */

	/* Parameter pixelsOrigin */

/* 2924 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2926 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2928 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 2930 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2936 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2938 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOrigin */

/* 2942 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2944 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2950 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2952 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2956 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter point */

/* 2958 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2960 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2962 */	NdrFcShort( 0x528 ),	/* Type Offset=1320 */

	/* Parameter pixelsOrigin */

/* 2964 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2966 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2968 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 2970 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2972 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetResolution */

/* 2976 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2978 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2982 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2984 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2988 */	NdrFcShort( 0x34 ),	/* 52 */
/* 2990 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter resolution */

/* 2992 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 2994 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2996 */	NdrFcShort( 0x528 ),	/* Type Offset=1320 */

	/* Parameter fPrefered */

/* 2998 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3000 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3002 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3004 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3006 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetResolution */

/* 3010 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3012 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3016 */	NdrFcShort( 0xa ),	/* 10 */
/* 3018 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3020 */	NdrFcShort( 0x2c ),	/* 44 */
/* 3022 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3024 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter resolution */

/* 3026 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3028 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3030 */	NdrFcShort( 0x528 ),	/* Type Offset=1320 */

	/* Return value */

/* 3032 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3034 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PixelsToUnits */

/* 3038 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3044 */	NdrFcShort( 0xf ),	/* 15 */
/* 3046 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3048 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3050 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3052 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter Xpix */

/* 3054 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3056 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3058 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Ypix */

/* 3060 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3062 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3064 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Xunit */

/* 3066 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3068 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3070 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Yunit */

/* 3072 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3074 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3076 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pixOrigin */

/* 3078 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3080 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3082 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3084 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3086 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3088 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnitsToPixels */

/* 3090 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3092 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3096 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3098 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3100 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3102 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3104 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter Xunit */

/* 3106 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3110 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Yunit */

/* 3112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3116 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter XPix */

/* 3118 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3120 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3122 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Ypix */

/* 3124 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3126 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3128 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pixOrigin */

/* 3130 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3132 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3134 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3136 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3138 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetViewRect */

/* 3142 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3144 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3148 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3150 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3152 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3154 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3156 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter left */

/* 3158 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3160 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3162 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter top */

/* 3164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3166 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3168 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter right */

/* 3170 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3172 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3174 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 3176 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3178 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3180 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3182 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3184 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetViewRect */

/* 3188 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3190 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3194 */	NdrFcShort( 0x12 ),	/* 18 */
/* 3196 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3200 */	NdrFcShort( 0x78 ),	/* 120 */
/* 3202 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter left */

/* 3204 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3208 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter top */

/* 3210 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3212 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3214 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter right */

/* 3216 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3218 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3220 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 3222 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3224 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3226 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 3228 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3230 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3236 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetZoom */

/* 3240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3246 */	NdrFcShort( 0x13 ),	/* 19 */
/* 3248 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3250 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3254 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter ZoomCenterX */

/* 3256 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3258 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3260 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomCenterY */

/* 3262 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3264 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3266 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomFactor */

/* 3268 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3270 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3274 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3276 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetZoom */

/* 3280 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3286 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3288 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3292 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3294 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter ZoomCenterX */

/* 3296 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3298 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3300 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZooomCenterY */

/* 3302 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3304 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3306 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomFactor */

/* 3308 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3310 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 3314 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3316 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3320 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3322 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */


	/* Procedure ControlChanged */

/* 3326 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3332 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3334 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3336 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3340 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startDepth */


	/* Parameter dispId */

/* 3342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3344 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter flags */

/* 3348 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3350 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3356 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnControlChanged */

/* 3360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3366 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3368 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3370 */	NdrFcShort( 0x64 ),	/* 100 */
/* 3372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3374 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter riidCtrl */

/* 3376 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3378 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3380 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 3382 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3384 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3386 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 3388 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3390 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3392 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pControlObj */

/* 3394 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3396 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3398 */	NdrFcShort( 0x5e2 ),	/* Type Offset=1506 */

	/* Parameter dispId */

/* 3400 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3402 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3404 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter flags */

/* 3406 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3408 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3414 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnControlChangedBSTR */

/* 3418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3424 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3426 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3428 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3432 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter ctrlGUID */

/* 3434 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 3436 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3438 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Parameter scanMode */

/* 3440 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3442 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 3446 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3448 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pControlObject */

/* 3452 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3454 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3456 */	NdrFcShort( 0x5e2 ),	/* Type Offset=1506 */

	/* Parameter dispId */

/* 3458 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3460 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter flags */

/* 3464 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3466 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3470 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3472 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3474 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 3476 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3478 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3482 */	NdrFcShort( 0xa ),	/* 10 */
/* 3484 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3486 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3488 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3490 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter x */

/* 3492 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3494 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3496 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter y */

/* 3498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3500 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3502 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 3504 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3506 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3512 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 3516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3522 */	NdrFcShort( 0xb ),	/* 11 */
/* 3524 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3528 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3530 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter scanLine */

/* 3532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 3538 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3540 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3542 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter x */

/* 3544 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3548 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter y */

/* 3550 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3552 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3554 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3558 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 3562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3568 */	NdrFcShort( 0xc ),	/* 12 */
/* 3570 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3572 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3574 */	NdrFcShort( 0x78 ),	/* 120 */
/* 3576 */	0x4,		/* Oi2 Flags:  has return, */
			0x8,		/* 8 */

	/* Parameter scanLine */

/* 3578 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 3584 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3588 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 3590 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3592 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3594 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 3596 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3598 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3600 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 3602 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3604 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3606 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 3608 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3610 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3612 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 3614 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3616 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3618 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3620 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3622 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 3626 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3628 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3632 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3634 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3636 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3638 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3640 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter scanLine */

/* 3642 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3644 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 3648 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3650 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3652 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 3654 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3656 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3658 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 3660 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3662 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3664 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3666 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3668 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 3672 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3674 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3678 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3680 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 3682 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3684 */	NdrFcShort( 0x78 ),	/* 120 */
/* 3686 */	0x4,		/* Oi2 Flags:  has return, */
			0x8,		/* 8 */

	/* Parameter scanLine */

/* 3688 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3690 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 3694 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3696 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3698 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 3700 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3702 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3704 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 3706 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3708 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3710 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 3712 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3714 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3716 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 3718 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3720 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3722 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 3724 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3726 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3728 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3730 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3732 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 3734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 3736 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3738 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3742 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3744 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3746 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3748 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3750 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter X */

/* 3752 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3754 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3756 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 3758 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3760 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3762 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 3764 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3766 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3770 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3772 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLinesRange */

/* 3776 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3778 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3782 */	NdrFcShort( 0xa ),	/* 10 */
/* 3784 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3788 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3790 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startLine */

/* 3792 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3794 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 3798 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3800 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3804 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3806 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3808 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointDepth */

/* 3810 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3812 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3816 */	NdrFcShort( 0xb ),	/* 11 */
/* 3818 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3820 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3822 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3824 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter X */

/* 3826 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3828 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3830 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 3832 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3834 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3836 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 3838 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3840 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3842 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3846 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryChargeStatus */


	/* Procedure get_Current */

/* 3850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3856 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3858 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3860 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3862 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3864 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 3866 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3868 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3870 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3872 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3874 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 3878 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3880 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3884 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3886 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3888 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3890 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3892 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 3894 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3896 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3898 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3900 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3902 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3904 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryFullLifetime */


	/* Procedure get_InputPin */


	/* Procedure get_FramesDrawn */

/* 3906 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3908 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3912 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3914 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3916 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3918 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3920 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 3922 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3924 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3926 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 3928 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3930 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Preset */

/* 3934 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3936 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3940 */	NdrFcShort( 0xc ),	/* 12 */
/* 3942 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3944 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3946 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3948 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter cEntries */

/* 3950 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3952 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3954 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValues */

/* 3956 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3958 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3960 */	NdrFcShort( 0x5f8 ),	/* Type Offset=1528 */

	/* Parameter iCurrent */

/* 3962 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3964 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SweepTime */

/* 3968 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3970 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3974 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3976 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3978 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetColorMap */

/* 3980 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3982 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3986 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3988 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3990 */	NdrFcShort( 0x60 ),	/* 96 */
/* 3992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3994 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter pTable1 */

/* 3996 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4000 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Parameter size1 */

/* 4002 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4004 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTable2 */

/* 4008 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4010 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4012 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Parameter size2 */

/* 4014 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4016 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4022 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWindow */

/* 4026 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4032 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4034 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4036 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4038 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4040 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter startLine */

/* 4042 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4044 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 4048 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4050 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4054 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4056 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4060 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4062 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4066 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4068 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4070 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindow */

/* 4072 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4074 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4078 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4080 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4084 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4086 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter startLine */

/* 4088 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4090 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 4094 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4096 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4100 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4102 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4106 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4108 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4112 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4114 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWidth */

/* 4118 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4120 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4124 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4126 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4128 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4130 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4132 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startLine */

/* 4134 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4136 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 4140 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4142 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4148 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */

/* 4152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4158 */	NdrFcShort( 0xa ),	/* 10 */
/* 4160 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4162 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4166 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startDepth */

/* 4168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4174 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWidth */

/* 4186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4192 */	NdrFcShort( 0xb ),	/* 11 */
/* 4194 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4198 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4200 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startLine */

/* 4202 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4204 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 4208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4210 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4216 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPhantomVolume */


	/* Procedure GetVolume */

/* 4220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4226 */	NdrFcShort( 0xc ),	/* 12 */
/* 4228 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4232 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4234 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startDepth */


	/* Parameter startDepth */

/* 4236 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4238 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */

/* 4242 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4244 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4248 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4250 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_LinesRange */

/* 4254 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4256 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4260 */	NdrFcShort( 0xd ),	/* 13 */
/* 4262 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4266 */	NdrFcShort( 0x38 ),	/* 56 */
/* 4268 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4270 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 4272 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4274 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 4276 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4278 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_WidthRange */

/* 4282 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4288 */	NdrFcShort( 0xe ),	/* 14 */
/* 4290 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4294 */	NdrFcShort( 0x38 ),	/* 56 */
/* 4296 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4298 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 4300 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4302 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 4304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4306 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */

/* 4310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4316 */	NdrFcShort( 0xf ),	/* 15 */
/* 4318 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4322 */	NdrFcShort( 0x38 ),	/* 56 */
/* 4324 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4326 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 4328 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4330 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 4332 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4334 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */

/* 4338 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4340 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4344 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4346 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4350 */	NdrFcShort( 0x38 ),	/* 56 */
/* 4352 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4354 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 4356 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4358 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 4360 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4362 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CurrentPosition */


	/* Procedure get_CurrentPosition */

/* 4366 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4368 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4372 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4374 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4378 */	NdrFcShort( 0x2c ),	/* 44 */
/* 4380 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 4382 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4384 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4386 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4388 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4390 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4392 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CurrentPosition */


	/* Procedure put_CurrentPosition */

/* 4394 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4396 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4402 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4404 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4406 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4408 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 4410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4412 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4414 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4418 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopPosition */


	/* Procedure get_Duration */

/* 4422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4428 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4430 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4434 */	NdrFcShort( 0x2c ),	/* 44 */
/* 4436 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 4438 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4440 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4442 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4446 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 4450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4456 */	NdrFcShort( 0xa ),	/* 10 */
/* 4458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4462 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4464 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4466 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4468 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4470 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4472 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4474 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 4478 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4480 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4484 */	NdrFcShort( 0xb ),	/* 11 */
/* 4486 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4492 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 4494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4496 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4498 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4502 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopTime */

/* 4506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4512 */	NdrFcShort( 0xc ),	/* 12 */
/* 4514 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4518 */	NdrFcShort( 0x2c ),	/* 44 */
/* 4520 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4522 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4524 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4526 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4530 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopTime */

/* 4534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4540 */	NdrFcShort( 0xd ),	/* 13 */
/* 4542 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4544 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4546 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4548 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 4550 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4552 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4554 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4558 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_State */


	/* Procedure get_State */

/* 4562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4568 */	NdrFcShort( 0xe ),	/* 14 */
/* 4570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4574 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4576 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 4578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4582 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_State */


	/* Procedure put_State */

/* 4590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4596 */	NdrFcShort( 0xf ),	/* 15 */
/* 4598 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4600 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4604 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 4606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4610 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4612 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 4618 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4620 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4624 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4626 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4630 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4632 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pStart */

/* 4634 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4636 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 4640 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4642 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4644 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4646 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4648 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPositions */

/* 4652 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4654 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4658 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4660 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4662 */	NdrFcShort( 0x1c ),	/* 28 */
/* 4664 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4666 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pPos */

/* 4668 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 4670 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 4674 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4676 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4680 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPositions */

/* 4686 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4688 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4692 */	NdrFcShort( 0x12 ),	/* 18 */
/* 4694 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4696 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4698 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4700 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter llStart */

/* 4702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4704 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4706 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter llStop */

/* 4708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4710 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4712 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4716 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CheckCapabilities */

/* 4720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4726 */	NdrFcShort( 0x14 ),	/* 20 */
/* 4728 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4730 */	NdrFcShort( 0x1c ),	/* 28 */
/* 4732 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4734 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pCaps */

/* 4736 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 4738 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4742 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4744 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopPosition */

/* 4748 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4750 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4754 */	NdrFcShort( 0xa ),	/* 10 */
/* 4756 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4758 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4762 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 4764 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4766 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4768 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4770 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4772 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Duration */

/* 4776 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4778 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4782 */	NdrFcShort( 0xb ),	/* 11 */
/* 4784 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4788 */	NdrFcShort( 0x2c ),	/* 44 */
/* 4790 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4792 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4794 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4796 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4800 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 4804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4810 */	NdrFcShort( 0xc ),	/* 12 */
/* 4812 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4816 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4818 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4820 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4822 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4824 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4826 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4828 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 4832 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4838 */	NdrFcShort( 0xd ),	/* 13 */
/* 4840 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4844 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4846 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 4848 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4850 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4852 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4854 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4856 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_TimeFormat */

/* 4860 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4862 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4866 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4868 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4872 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4874 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 4876 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4878 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4880 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 4882 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4884 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TimeFormat */

/* 4888 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4894 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4896 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4902 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 4904 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4906 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4908 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 4910 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4912 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ConvertTimeFormat */

/* 4916 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4918 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4922 */	NdrFcShort( 0x12 ),	/* 18 */
/* 4924 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4926 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4928 */	NdrFcShort( 0x2c ),	/* 44 */
/* 4930 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter srcVal */

/* 4932 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4936 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter srcFormat */

/* 4938 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4940 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4942 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstFormat */

/* 4944 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4946 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4948 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstVal */

/* 4950 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4952 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4954 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4958 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 4962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4968 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4970 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4974 */	NdrFcShort( 0x50 ),	/* 80 */
/* 4976 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter lEarliest */

/* 4978 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4980 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4982 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter lLatest */

/* 4984 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4986 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4988 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 4990 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4992 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4994 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetShiftPosition */

/* 4996 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4998 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5002 */	NdrFcShort( 0x14 ),	/* 20 */
/* 5004 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5006 */	NdrFcShort( 0x28 ),	/* 40 */
/* 5008 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5010 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter from */

/* 5012 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5014 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5016 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter shift */

/* 5018 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5020 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5022 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter timeFormat */

/* 5024 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5026 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5028 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter pos */

/* 5030 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5032 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5034 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5036 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5038 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5040 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHoldRange */

/* 5042 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5044 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5048 */	NdrFcShort( 0x15 ),	/* 21 */
/* 5050 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5052 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5054 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5056 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter from */

/* 5058 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5060 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5062 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 5064 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5066 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5068 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5072 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHoldRange */

/* 5076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5082 */	NdrFcShort( 0x16 ),	/* 22 */
/* 5084 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5088 */	NdrFcShort( 0x50 ),	/* 80 */
/* 5090 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter from */

/* 5092 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5094 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5096 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 5098 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5100 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5102 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5104 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5106 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTimeFormatSupported */

/* 5110 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5112 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5116 */	NdrFcShort( 0x17 ),	/* 23 */
/* 5118 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5122 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5124 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter format */

/* 5126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5130 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter support */

/* 5132 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContinuousInterval */

/* 5144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5150 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5152 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5154 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5156 */	NdrFcShort( 0x50 ),	/* 80 */
/* 5158 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter position */

/* 5160 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5162 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5164 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter start */

/* 5166 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5168 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5170 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter end */

/* 5172 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5174 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5176 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5180 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSelection */

/* 5184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5190 */	NdrFcShort( 0x19 ),	/* 25 */
/* 5192 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5194 */	NdrFcShort( 0x28 ),	/* 40 */
/* 5196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5198 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter from */

/* 5200 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5202 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5204 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 5206 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5208 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5210 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter mode */

/* 5212 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5214 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5216 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 5218 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5220 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBufferSize */

/* 5224 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5226 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5230 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5232 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5236 */	NdrFcShort( 0x5c ),	/* 92 */
/* 5238 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter total */

/* 5240 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5242 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter used */

/* 5246 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5248 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter writePos */

/* 5252 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5254 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5258 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5260 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */

/* 5264 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5272 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5278 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 5280 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5282 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetBufferSize */

/* 5286 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5288 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5292 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5294 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5300 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter cbSize */

/* 5302 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5304 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5308 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5310 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveData */

/* 5314 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5316 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5320 */	NdrFcShort( 0xa ),	/* 10 */
/* 5322 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5326 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5328 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strFileName */

/* 5330 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 5332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5334 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Return value */

/* 5336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5338 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */

/* 5342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5350 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5354 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5356 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter startDepth */

/* 5358 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5360 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5362 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 5364 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5366 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5370 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5372 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */

/* 5376 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5378 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5382 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5384 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5388 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5390 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 5392 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5394 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5396 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 5398 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5400 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */

/* 5404 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5406 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5410 */	NdrFcShort( 0xa ),	/* 10 */
/* 5412 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5416 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5418 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 5420 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5422 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5424 */	NdrFcShort( 0x5bc ),	/* Type Offset=1468 */

	/* Return value */

/* 5426 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5428 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSampleVolume */

/* 5432 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5434 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5438 */	NdrFcShort( 0xb ),	/* 11 */
/* 5440 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5442 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5444 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5446 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter startDepth */

/* 5448 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5450 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 5454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5456 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter line */

/* 5460 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5462 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5468 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanConverter */

/* 5472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5478 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5480 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5486 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 5488 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5490 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5492 */	NdrFcShort( 0x1da ),	/* Type Offset=474 */

	/* Return value */

/* 5494 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5496 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsSupported */

/* 5500 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5506 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5508 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5512 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5514 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter nIndex */

/* 5516 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5518 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5520 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter supported */

/* 5522 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5524 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5530 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIndex */

/* 5534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5540 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5542 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5544 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5546 */	NdrFcShort( 0x34 ),	/* 52 */
/* 5548 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter nIndex */

/* 5550 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5552 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5554 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter value */

/* 5556 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 5558 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5560 */	NdrFcShort( 0x528 ),	/* Type Offset=1320 */

	/* Return value */

/* 5562 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5564 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5566 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */

/* 5568 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5570 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5574 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5576 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5580 */	NdrFcShort( 0x34 ),	/* 52 */
/* 5582 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 5584 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 5586 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5588 */	NdrFcShort( 0x60a ),	/* Type Offset=1546 */

	/* Return value */

/* 5590 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5592 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 5596 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5598 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5604 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5606 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5608 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5610 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 5612 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 5614 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5616 */	NdrFcShort( 0x60a ),	/* Type Offset=1546 */

	/* Return value */

/* 5618 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5620 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FileName */

/* 5624 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5626 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5630 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5632 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5636 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5638 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 5640 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 5642 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5644 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Return value */

/* 5646 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5648 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 5652 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5654 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5658 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5660 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5664 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5666 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 5668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5670 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataKey */

/* 5674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5680 */	NdrFcShort( 0xb ),	/* 11 */
/* 5682 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5684 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5686 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5688 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter hBaseKey */

/* 5690 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5692 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 5696 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5698 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 5702 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5704 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5706 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5708 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5710 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDataKey */

/* 5714 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5720 */	NdrFcShort( 0xc ),	/* 12 */
/* 5722 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5724 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5726 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5728 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter baseKey */

/* 5730 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5732 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 5736 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5738 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyIndex */

/* 5742 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5744 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 5748 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5750 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5754 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5756 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CloseDataKey */

/* 5760 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5766 */	NdrFcShort( 0xd ),	/* 13 */
/* 5768 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5770 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5774 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter dataKey */

/* 5776 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5778 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5782 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5784 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKeyInfo */

/* 5788 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5790 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5794 */	NdrFcShort( 0xe ),	/* 14 */
/* 5796 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5798 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5800 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5802 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter dataKey */

/* 5804 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5806 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5808 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subKeys */

/* 5810 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5812 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataValues */

/* 5816 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5818 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5824 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumValues */

/* 5828 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5834 */	NdrFcShort( 0xf ),	/* 15 */
/* 5836 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5838 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5840 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5842 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter dataKey */

/* 5844 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5846 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ValueNum */

/* 5850 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5852 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 5856 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5858 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueSize */

/* 5862 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5864 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5868 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5870 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumSubkeys */

/* 5874 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5876 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5880 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5882 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5884 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5886 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5888 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter dataKey */

/* 5890 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5892 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyNum */

/* 5896 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5898 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyId */

/* 5902 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5904 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5908 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5910 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValue */

/* 5914 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5916 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5920 */	NdrFcShort( 0x11 ),	/* 17 */
/* 5922 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5924 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5926 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5928 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x7,		/* 7 */

	/* Parameter dataKey */

/* 5930 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5932 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 5936 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5938 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 5942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5944 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cbBufferSize */

/* 5948 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5950 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 5954 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5956 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5958 */	NdrFcShort( 0x616 ),	/* Type Offset=1558 */

	/* Parameter dataSize */

/* 5960 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5962 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5968 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValue */

/* 5972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5978 */	NdrFcShort( 0x12 ),	/* 18 */
/* 5980 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5982 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5986 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter dataKey */

/* 5988 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5990 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 5994 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5996 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 6000 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6002 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 6006 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6008 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6010 */	NdrFcShort( 0x624 ),	/* Type Offset=1572 */

	/* Parameter dataSize */

/* 6012 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6014 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6016 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6018 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6020 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateObjectsDump */

/* 6024 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6026 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6030 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6032 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6034 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6038 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter fileName */

/* 6040 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6042 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6044 */	NdrFcShort( 0x90 ),	/* Type Offset=144 */

	/* Parameter reserved1 */

/* 6046 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6048 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 6052 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6054 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6058 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6060 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPlugin */

/* 6064 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6066 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6070 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6072 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6076 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6078 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pPlugin */

/* 6080 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6082 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6084 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Parameter cookie */

/* 6086 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6088 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6092 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6094 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ScanModeCaps */

/* 6098 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6100 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6104 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6106 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6108 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6110 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6112 */	0x4,		/* Oi2 Flags:  has return, */
			0x5,		/* 5 */

	/* Parameter Parameter */

/* 6114 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6116 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ScanMode */

/* 6120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6122 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Capability */

/* 6126 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Support */

/* 6132 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6134 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6140 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurrent */

/* 6144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6150 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6152 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6156 */	NdrFcShort( 0x48 ),	/* 72 */
/* 6158 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pControlObject */

/* 6160 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6162 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6164 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Parameter pVal */

/* 6166 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 6168 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6170 */	NdrFcShort( 0x632 ),	/* Type Offset=1586 */

	/* Return value */

/* 6172 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6174 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6176 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCurrent */

/* 6178 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6180 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6184 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6186 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6188 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6192 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pControlObject */

/* 6194 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6196 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6198 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Parameter pVal */

/* 6200 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 6202 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6204 */	NdrFcShort( 0x632 ),	/* Type Offset=1586 */

	/* Return value */

/* 6206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6208 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValues */

/* 6212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6218 */	NdrFcShort( 0x9 ),	/* 9 */
/* 6220 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6224 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6226 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pControlObject */

/* 6228 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6230 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6232 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Parameter pVal */

/* 6234 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6236 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6238 */	NdrFcShort( 0x1f0 ),	/* Type Offset=496 */

	/* Return value */

/* 6240 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6242 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  8 */	0xe,		/* FC_ENUM32 */
			0x5c,		/* FC_PAD */
/* 10 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 16 */	NdrFcLong( 0x60c480b7 ),	/* 1623490743 */
/* 20 */	NdrFcShort( 0xf1e7 ),	/* -3609 */
/* 22 */	NdrFcShort( 0x403c ),	/* 16444 */
/* 24 */	0x8a,		/* 138 */
			0xf3,		/* 243 */
/* 26 */	0x8d,		/* 141 */
			0xce,		/* 206 */
/* 28 */	0xd9,		/* 217 */
			0x9a,		/* 154 */
/* 30 */	0x25,		/* 37 */
			0x60,		/* 96 */
/* 32 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 34 */	NdrFcShort( 0x2 ),	/* Offset= 2 (36) */
/* 36 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 38 */	NdrFcLong( 0xb8cba727 ),	/* -1194612953 */
/* 42 */	NdrFcShort( 0xd104 ),	/* -12028 */
/* 44 */	NdrFcShort( 0x416e ),	/* 16750 */
/* 46 */	0xb0,		/* 176 */
			0xcc,		/* 204 */
/* 48 */	0xc6,		/* 198 */
			0x2e,		/* 46 */
/* 50 */	0x9c,		/* 156 */
			0xf9,		/* 249 */
/* 52 */	0xb1,		/* 177 */
			0xf9,		/* 249 */
/* 54 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 56 */	NdrFcShort( 0x2 ),	/* Offset= 2 (58) */
/* 58 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 60 */	NdrFcLong( 0x56a868b1 ),	/* 1453877425 */
/* 64 */	NdrFcShort( 0xad4 ),	/* 2772 */
/* 66 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 68 */	0xb0,		/* 176 */
			0x3a,		/* 58 */
/* 70 */	0x0,		/* 0 */
			0x20,		/* 32 */
/* 72 */	0xaf,		/* 175 */
			0xb,		/* 11 */
/* 74 */	0xa7,		/* 167 */
			0x70,		/* 112 */
/* 76 */	
			0x11, 0x0,	/* FC_RP */
/* 78 */	NdrFcShort( 0x8 ),	/* Offset= 8 (86) */
/* 80 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 82 */	NdrFcShort( 0x8 ),	/* 8 */
/* 84 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 86 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 88 */	NdrFcShort( 0x10 ),	/* 16 */
/* 90 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 92 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 94 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (80) */
			0x5b,		/* FC_END */
/* 98 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 100 */	NdrFcShort( 0x2 ),	/* Offset= 2 (102) */
/* 102 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 104 */	NdrFcLong( 0x5748ca80 ),	/* 1464388224 */
/* 108 */	NdrFcShort( 0x1710 ),	/* 5904 */
/* 110 */	NdrFcShort( 0x489f ),	/* 18591 */
/* 112 */	0xbc,		/* 188 */
			0x13,		/* 19 */
/* 114 */	0x28,		/* 40 */
			0xf2,		/* 242 */
/* 116 */	0xc0,		/* 192 */
			0x12,		/* 18 */
/* 118 */	0x2b,		/* 43 */
			0x49,		/* 73 */
/* 120 */	
			0x12, 0x0,	/* FC_UP */
/* 122 */	NdrFcShort( 0xc ),	/* Offset= 12 (134) */
/* 124 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 126 */	NdrFcShort( 0x2 ),	/* 2 */
/* 128 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 130 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 132 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 134 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 136 */	NdrFcShort( 0x8 ),	/* 8 */
/* 138 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (124) */
/* 140 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 142 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 144 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0x4 ),	/* 4 */
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 152 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (120) */
/* 154 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 156 */	NdrFcShort( 0x2 ),	/* Offset= 2 (158) */
/* 158 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 160 */	NdrFcLong( 0x4cf81935 ),	/* 1291327797 */
/* 164 */	NdrFcShort( 0xdcb3 ),	/* -9037 */
/* 166 */	NdrFcShort( 0x4c19 ),	/* 19481 */
/* 168 */	0x9d,		/* 157 */
			0xc9,		/* 201 */
/* 170 */	0x76,		/* 118 */
			0x53,		/* 83 */
/* 172 */	0x6,		/* 6 */
			0xa4,		/* 164 */
/* 174 */	0x71,		/* 113 */
			0x4f,		/* 79 */
/* 176 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 178 */	NdrFcShort( 0x2 ),	/* Offset= 2 (180) */
/* 180 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 182 */	NdrFcLong( 0xb7c94539 ),	/* -1211546311 */
/* 186 */	NdrFcShort( 0xa65a ),	/* -22950 */
/* 188 */	NdrFcShort( 0x42f3 ),	/* 17139 */
/* 190 */	0x8b,		/* 139 */
			0x65,		/* 101 */
/* 192 */	0xf8,		/* 248 */
			0x3d,		/* 61 */
/* 194 */	0x11,		/* 17 */
			0x4f,		/* 79 */
/* 196 */	0xf4,		/* 244 */
			0xc8,		/* 200 */
/* 198 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 200 */	NdrFcShort( 0x2 ),	/* Offset= 2 (202) */
/* 202 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 214 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 216 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 218 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 220 */	
			0x11, 0x0,	/* FC_RP */
/* 222 */	NdrFcShort( 0x84 ),	/* Offset= 132 (354) */
/* 224 */	
			0x13, 0x0,	/* FC_OP */
/* 226 */	NdrFcShort( 0x2 ),	/* Offset= 2 (228) */
/* 228 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 230 */	NdrFcShort( 0x4 ),	/* 4 */
/* 232 */	NdrFcShort( 0x2 ),	/* 2 */
/* 234 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 238 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 240 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 244 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 246 */	NdrFcShort( 0xffff ),	/* Offset= -1 (245) */
/* 248 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 250 */	NdrFcShort( 0x1 ),	/* 1 */
/* 252 */	NdrFcShort( 0x4 ),	/* 4 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (224) */
/* 258 */	
			0x13, 0x0,	/* FC_OP */
/* 260 */	NdrFcShort( 0x2 ),	/* Offset= 2 (262) */
/* 262 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 266 */	NdrFcShort( 0x3 ),	/* 3 */
/* 268 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 272 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 274 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 278 */	NdrFcShort( 0xa ),	/* Offset= 10 (288) */
/* 280 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 284 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 286 */	NdrFcShort( 0xffff ),	/* Offset= -1 (285) */
/* 288 */	
			0x13, 0x0,	/* FC_OP */
/* 290 */	NdrFcShort( 0xc ),	/* Offset= 12 (302) */
/* 292 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 294 */	NdrFcShort( 0x1 ),	/* 1 */
/* 296 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 298 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 300 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 302 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 304 */	NdrFcShort( 0x18 ),	/* 24 */
/* 306 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (292) */
/* 308 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 310 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 312 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 314 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 316 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 318 */	NdrFcShort( 0x2 ),	/* 2 */
/* 320 */	NdrFcShort( 0x4 ),	/* 4 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0xffbe ),	/* Offset= -66 (258) */
/* 326 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 328 */	NdrFcShort( 0x18 ),	/* 24 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x0 ),	/* Offset= 0 (332) */
/* 334 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 336 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 338 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 340 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (316) */
			0x5b,		/* FC_END */
/* 344 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 346 */	NdrFcShort( 0x10 ),	/* 16 */
/* 348 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 350 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 352 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 354 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 356 */	NdrFcShort( 0x4c ),	/* 76 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* Offset= 0 (360) */
/* 362 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 364 */	0x0,		/* 0 */
			NdrFcShort( 0xff8b ),	/* Offset= -117 (248) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 368 */	0x0,		/* 0 */
			NdrFcShort( 0xffcb ),	/* Offset= -53 (316) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 372 */	0x0,		/* 0 */
			NdrFcShort( 0xffd1 ),	/* Offset= -47 (326) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 376 */	0x0,		/* 0 */
			NdrFcShort( 0xffdf ),	/* Offset= -33 (344) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 380 */	0x0,		/* 0 */
			NdrFcShort( 0xffdb ),	/* Offset= -37 (344) */
			0xa,		/* FC_FLOAT */
/* 384 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 386 */	
			0x11, 0x0,	/* FC_RP */
/* 388 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (344) */
/* 390 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 392 */	NdrFcShort( 0x2 ),	/* Offset= 2 (394) */
/* 394 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 396 */	NdrFcLong( 0x7bf80981 ),	/* 2079852929 */
/* 400 */	NdrFcShort( 0xbf32 ),	/* -16590 */
/* 402 */	NdrFcShort( 0x101a ),	/* 4122 */
/* 404 */	0x8b,		/* 139 */
			0xbb,		/* 187 */
/* 406 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 408 */	0x0,		/* 0 */
			0x30,		/* 48 */
/* 410 */	0xc,		/* 12 */
			0xab,		/* 171 */
/* 412 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 414 */	NdrFcShort( 0x2 ),	/* Offset= 2 (416) */
/* 416 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 418 */	NdrFcShort( 0x4 ),	/* 4 */
/* 420 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 422 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 424 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 426 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 428 */	NdrFcShort( 0xffac ),	/* Offset= -84 (344) */
/* 430 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 432 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 434 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 436 */	NdrFcShort( 0x2 ),	/* Offset= 2 (438) */
/* 438 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 440 */	NdrFcLong( 0xbfef8670 ),	/* -1074821520 */
/* 444 */	NdrFcShort( 0xdbc1 ),	/* -9279 */
/* 446 */	NdrFcShort( 0x4b47 ),	/* 19271 */
/* 448 */	0xbc,		/* 188 */
			0x1,		/* 1 */
/* 450 */	0xbb,		/* 187 */
			0xbc,		/* 188 */
/* 452 */	0xa1,		/* 161 */
			0x74,		/* 116 */
/* 454 */	0xdd,		/* 221 */
			0x31,		/* 49 */
/* 456 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 458 */	NdrFcShort( 0x6 ),	/* Offset= 6 (464) */
/* 460 */	
			0x13, 0x0,	/* FC_OP */
/* 462 */	NdrFcShort( 0xfeb8 ),	/* Offset= -328 (134) */
/* 464 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x4 ),	/* 4 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (460) */
/* 474 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 476 */	NdrFcShort( 0x2 ),	/* Offset= 2 (478) */
/* 478 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 480 */	NdrFcLong( 0x0 ),	/* 0 */
/* 484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 488 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 490 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 492 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 494 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 496 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 498 */	NdrFcShort( 0x2 ),	/* Offset= 2 (500) */
/* 500 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 502 */	NdrFcLong( 0x95d11d2b ),	/* -1781457621 */
/* 506 */	NdrFcShort( 0xec05 ),	/* -5115 */
/* 508 */	NdrFcShort( 0x4a2e ),	/* 18990 */
/* 510 */	0xb3,		/* 179 */
			0x1b,		/* 27 */
/* 512 */	0x13,		/* 19 */
			0x86,		/* 134 */
/* 514 */	0xc4,		/* 196 */
			0x84,		/* 132 */
/* 516 */	0xae,		/* 174 */
			0x16,		/* 22 */
/* 518 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 520 */	NdrFcShort( 0x3a6 ),	/* Offset= 934 (1454) */
/* 522 */	
			0x13, 0x0,	/* FC_OP */
/* 524 */	NdrFcShort( 0x38e ),	/* Offset= 910 (1434) */
/* 526 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 528 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 530 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 532 */	NdrFcShort( 0x2 ),	/* Offset= 2 (534) */
/* 534 */	NdrFcShort( 0x10 ),	/* 16 */
/* 536 */	NdrFcShort( 0x2f ),	/* 47 */
/* 538 */	NdrFcLong( 0x14 ),	/* 20 */
/* 542 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 544 */	NdrFcLong( 0x3 ),	/* 3 */
/* 548 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 550 */	NdrFcLong( 0x11 ),	/* 17 */
/* 554 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 556 */	NdrFcLong( 0x2 ),	/* 2 */
/* 560 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 562 */	NdrFcLong( 0x4 ),	/* 4 */
/* 566 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 568 */	NdrFcLong( 0x5 ),	/* 5 */
/* 572 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 574 */	NdrFcLong( 0xb ),	/* 11 */
/* 578 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 580 */	NdrFcLong( 0xa ),	/* 10 */
/* 584 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 586 */	NdrFcLong( 0x6 ),	/* 6 */
/* 590 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (822) */
/* 592 */	NdrFcLong( 0x7 ),	/* 7 */
/* 596 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 598 */	NdrFcLong( 0x8 ),	/* 8 */
/* 602 */	NdrFcShort( 0xff72 ),	/* Offset= -142 (460) */
/* 604 */	NdrFcLong( 0xd ),	/* 13 */
/* 608 */	NdrFcShort( 0xff7e ),	/* Offset= -130 (478) */
/* 610 */	NdrFcLong( 0x9 ),	/* 9 */
/* 614 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (828) */
/* 616 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 620 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (846) */
/* 622 */	NdrFcLong( 0x24 ),	/* 36 */
/* 626 */	NdrFcShort( 0x2de ),	/* Offset= 734 (1360) */
/* 628 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 632 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (1360) */
/* 634 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 638 */	NdrFcShort( 0x2d6 ),	/* Offset= 726 (1364) */
/* 640 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 644 */	NdrFcShort( 0x2d4 ),	/* Offset= 724 (1368) */
/* 646 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 650 */	NdrFcShort( 0x2d2 ),	/* Offset= 722 (1372) */
/* 652 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 656 */	NdrFcShort( 0x2d0 ),	/* Offset= 720 (1376) */
/* 658 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 662 */	NdrFcShort( 0x2ce ),	/* Offset= 718 (1380) */
/* 664 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 668 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (1384) */
/* 670 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 674 */	NdrFcShort( 0x2b6 ),	/* Offset= 694 (1368) */
/* 676 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 680 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (1372) */
/* 682 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 686 */	NdrFcShort( 0x2be ),	/* Offset= 702 (1388) */
/* 688 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 692 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (1384) */
/* 694 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 698 */	NdrFcShort( 0x2b6 ),	/* Offset= 694 (1392) */
/* 700 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 704 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (1396) */
/* 706 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 710 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (1400) */
/* 712 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 716 */	NdrFcShort( 0x2b0 ),	/* Offset= 688 (1404) */
/* 718 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 722 */	NdrFcShort( 0x2ae ),	/* Offset= 686 (1408) */
/* 724 */	NdrFcLong( 0x10 ),	/* 16 */
/* 728 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 730 */	NdrFcLong( 0x12 ),	/* 18 */
/* 734 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 736 */	NdrFcLong( 0x13 ),	/* 19 */
/* 740 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 742 */	NdrFcLong( 0x15 ),	/* 21 */
/* 746 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 748 */	NdrFcLong( 0x16 ),	/* 22 */
/* 752 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 754 */	NdrFcLong( 0x17 ),	/* 23 */
/* 758 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 760 */	NdrFcLong( 0xe ),	/* 14 */
/* 764 */	NdrFcShort( 0x28c ),	/* Offset= 652 (1416) */
/* 766 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 770 */	NdrFcShort( 0x290 ),	/* Offset= 656 (1426) */
/* 772 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 776 */	NdrFcShort( 0x28e ),	/* Offset= 654 (1430) */
/* 778 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 782 */	NdrFcShort( 0x24a ),	/* Offset= 586 (1368) */
/* 784 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 788 */	NdrFcShort( 0x248 ),	/* Offset= 584 (1372) */
/* 790 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 794 */	NdrFcShort( 0x246 ),	/* Offset= 582 (1376) */
/* 796 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 800 */	NdrFcShort( 0x23c ),	/* Offset= 572 (1372) */
/* 802 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 806 */	NdrFcShort( 0x236 ),	/* Offset= 566 (1372) */
/* 808 */	NdrFcLong( 0x0 ),	/* 0 */
/* 812 */	NdrFcShort( 0x0 ),	/* Offset= 0 (812) */
/* 814 */	NdrFcLong( 0x1 ),	/* 1 */
/* 818 */	NdrFcShort( 0x0 ),	/* Offset= 0 (818) */
/* 820 */	NdrFcShort( 0xffff ),	/* Offset= -1 (819) */
/* 822 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 826 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 828 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 830 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 838 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 840 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 842 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 844 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 846 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 848 */	NdrFcShort( 0x2 ),	/* Offset= 2 (850) */
/* 850 */	
			0x13, 0x0,	/* FC_OP */
/* 852 */	NdrFcShort( 0x1ea ),	/* Offset= 490 (1342) */
/* 854 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 856 */	NdrFcShort( 0x18 ),	/* 24 */
/* 858 */	NdrFcShort( 0xa ),	/* 10 */
/* 860 */	NdrFcLong( 0x8 ),	/* 8 */
/* 864 */	NdrFcShort( 0x58 ),	/* Offset= 88 (952) */
/* 866 */	NdrFcLong( 0xd ),	/* 13 */
/* 870 */	NdrFcShort( 0x78 ),	/* Offset= 120 (990) */
/* 872 */	NdrFcLong( 0x9 ),	/* 9 */
/* 876 */	NdrFcShort( 0x94 ),	/* Offset= 148 (1024) */
/* 878 */	NdrFcLong( 0xc ),	/* 12 */
/* 882 */	NdrFcShort( 0xbc ),	/* Offset= 188 (1070) */
/* 884 */	NdrFcLong( 0x24 ),	/* 36 */
/* 888 */	NdrFcShort( 0x114 ),	/* Offset= 276 (1164) */
/* 890 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 894 */	NdrFcShort( 0x11e ),	/* Offset= 286 (1180) */
/* 896 */	NdrFcLong( 0x10 ),	/* 16 */
/* 900 */	NdrFcShort( 0x136 ),	/* Offset= 310 (1210) */
/* 902 */	NdrFcLong( 0x2 ),	/* 2 */
/* 906 */	NdrFcShort( 0x14e ),	/* Offset= 334 (1240) */
/* 908 */	NdrFcLong( 0x3 ),	/* 3 */
/* 912 */	NdrFcShort( 0x166 ),	/* Offset= 358 (1270) */
/* 914 */	NdrFcLong( 0x14 ),	/* 20 */
/* 918 */	NdrFcShort( 0x17e ),	/* Offset= 382 (1300) */
/* 920 */	NdrFcShort( 0xffff ),	/* Offset= -1 (919) */
/* 922 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 924 */	NdrFcShort( 0x4 ),	/* 4 */
/* 926 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 932 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 934 */	NdrFcShort( 0x4 ),	/* 4 */
/* 936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 938 */	NdrFcShort( 0x1 ),	/* 1 */
/* 940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 944 */	0x13, 0x0,	/* FC_OP */
/* 946 */	NdrFcShort( 0xfcd4 ),	/* Offset= -812 (134) */
/* 948 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 950 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 952 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 956 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 958 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 960 */	NdrFcShort( 0x4 ),	/* 4 */
/* 962 */	NdrFcShort( 0x4 ),	/* 4 */
/* 964 */	0x11, 0x0,	/* FC_RP */
/* 966 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (922) */
/* 968 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 970 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 972 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 980 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 984 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 986 */	NdrFcShort( 0xfe04 ),	/* Offset= -508 (478) */
/* 988 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 990 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1002) */
/* 998 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1000 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1002 */	
			0x11, 0x0,	/* FC_RP */
/* 1004 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (972) */
/* 1006 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1014 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1018 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1020 */	NdrFcShort( 0xff40 ),	/* Offset= -192 (828) */
/* 1022 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1024 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1026 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1036) */
/* 1032 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1034 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1036 */	
			0x11, 0x0,	/* FC_RP */
/* 1038 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (1006) */
/* 1040 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1042 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1044 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1048 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1050 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1052 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1056 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1058 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1062 */	0x13, 0x0,	/* FC_OP */
/* 1064 */	NdrFcShort( 0x172 ),	/* Offset= 370 (1434) */
/* 1066 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1068 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1070 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1072 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1076 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1082) */
/* 1078 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1080 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1082 */	
			0x11, 0x0,	/* FC_RP */
/* 1084 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (1040) */
/* 1086 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1088 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1096 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1098 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1100 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1102 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1104 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1106 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1108 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1110 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1112 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1114 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1116 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1120 */	NdrFcShort( 0xa ),	/* Offset= 10 (1130) */
/* 1122 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1124 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1126 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (1086) */
/* 1128 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1130 */	
			0x13, 0x0,	/* FC_OP */
/* 1132 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (1104) */
/* 1134 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1136 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1138 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1142 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1144 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1146 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1150 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1156 */	0x13, 0x0,	/* FC_OP */
/* 1158 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (1114) */
/* 1160 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1162 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1164 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1176) */
/* 1172 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1174 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1176 */	
			0x11, 0x0,	/* FC_RP */
/* 1178 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (1134) */
/* 1180 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1182 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1186 */	NdrFcShort( 0xa ),	/* Offset= 10 (1196) */
/* 1188 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1190 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1192 */	NdrFcShort( 0xfbae ),	/* Offset= -1106 (86) */
/* 1194 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1196 */	
			0x11, 0x0,	/* FC_RP */
/* 1198 */	NdrFcShort( 0xff1e ),	/* Offset= -226 (972) */
/* 1200 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1202 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1204 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1208 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1210 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1214 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1216 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1220 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1222 */	0x13, 0x0,	/* FC_OP */
/* 1224 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1200) */
/* 1226 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1228 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1230 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1232 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1234 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1238 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1240 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1244 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1246 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1248 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1250 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1252 */	0x13, 0x0,	/* FC_OP */
/* 1254 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1230) */
/* 1256 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1258 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1260 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1262 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1264 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1268 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1270 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1274 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1276 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1278 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1280 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1282 */	0x13, 0x0,	/* FC_OP */
/* 1284 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1260) */
/* 1286 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1288 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1290 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1292 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1294 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1298 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1300 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1302 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1304 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1306 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1308 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1310 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1312 */	0x13, 0x0,	/* FC_OP */
/* 1314 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1290) */
/* 1316 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1318 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1320 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1324 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1326 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1328 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1332 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1334 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1336 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1338 */	NdrFcShort( 0xffee ),	/* Offset= -18 (1320) */
/* 1340 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1342 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1344 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1346 */	NdrFcShort( 0xffee ),	/* Offset= -18 (1328) */
/* 1348 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1348) */
/* 1350 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1352 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1354 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1356 */	NdrFcShort( 0xfe0a ),	/* Offset= -502 (854) */
/* 1358 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1360 */	
			0x13, 0x0,	/* FC_OP */
/* 1362 */	NdrFcShort( 0xff08 ),	/* Offset= -248 (1114) */
/* 1364 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1366 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1368 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1370 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1372 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1374 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1376 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1378 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1380 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1382 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1384 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1386 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1388 */	
			0x13, 0x0,	/* FC_OP */
/* 1390 */	NdrFcShort( 0xfdc8 ),	/* Offset= -568 (822) */
/* 1392 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1394 */	NdrFcShort( 0xfc5a ),	/* Offset= -934 (460) */
/* 1396 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1398 */	NdrFcShort( 0xfc68 ),	/* Offset= -920 (478) */
/* 1400 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1402 */	NdrFcShort( 0xfdc2 ),	/* Offset= -574 (828) */
/* 1404 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1406 */	NdrFcShort( 0xfdd0 ),	/* Offset= -560 (846) */
/* 1408 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1410 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1412) */
/* 1412 */	
			0x13, 0x0,	/* FC_OP */
/* 1414 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1434) */
/* 1416 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1418 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1420 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1422 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1424 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1426 */	
			0x13, 0x0,	/* FC_OP */
/* 1428 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1416) */
/* 1430 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1432 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1434 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1436 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1440) */
/* 1442 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1444 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1446 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1448 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1450 */	NdrFcShort( 0xfc64 ),	/* Offset= -924 (526) */
/* 1452 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1454 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1456 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1458 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1462 */	NdrFcShort( 0xfc54 ),	/* Offset= -940 (522) */
/* 1464 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1466 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1468) */
/* 1468 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1470 */	NdrFcShort( 0xc ),	/* 12 */
/* 1472 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1474 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1476 */	
			0x11, 0x0,	/* FC_RP */
/* 1478 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1480) */
/* 1480 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1482 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1484 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1486 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1488 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1490 */	
			0x11, 0x0,	/* FC_RP */
/* 1492 */	NdrFcShort( 0xfbcc ),	/* Offset= -1076 (416) */
/* 1494 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1496 */	NdrFcShort( 0xff50 ),	/* Offset= -176 (1320) */
/* 1498 */	
			0x11, 0x0,	/* FC_RP */
/* 1500 */	NdrFcShort( 0xff4c ),	/* Offset= -180 (1320) */
/* 1502 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1504 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1506 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1508 */	NdrFcLong( 0x5748ca80 ),	/* 1464388224 */
/* 1512 */	NdrFcShort( 0x1710 ),	/* 5904 */
/* 1514 */	NdrFcShort( 0x489f ),	/* 18591 */
/* 1516 */	0xbc,		/* 188 */
			0x13,		/* 19 */
/* 1518 */	0x28,		/* 40 */
			0xf2,		/* 242 */
/* 1520 */	0xc0,		/* 192 */
			0x12,		/* 18 */
/* 1522 */	0x2b,		/* 43 */
			0x49,		/* 73 */
/* 1524 */	
			0x11, 0x0,	/* FC_RP */
/* 1526 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1528) */
/* 1528 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1530 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1532 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1536 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1538 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1540 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1542 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1544 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1546) */
/* 1546 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1548 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1550 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 1552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1554 */	
			0x11, 0x0,	/* FC_RP */
/* 1556 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1558) */
/* 1558 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1560 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1562 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1564 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1566 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1568 */	
			0x11, 0x0,	/* FC_RP */
/* 1570 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1572) */
/* 1572 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1574 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1576 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1578 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1580 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1582 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1584 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1586) */
/* 1586 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1588 */	NdrFcShort( 0xc ),	/* 12 */
/* 1590 */	0xe,		/* FC_ENUM32 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1592 */	0x0,		/* 0 */
			NdrFcShort( 0xfeef ),	/* Offset= -273 (1320) */
			0x5b,		/* FC_END */
/* 1596 */	
			0x11, 0x0,	/* FC_RP */
/* 1598 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1586) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            HDC_UserSize
            ,HDC_UserMarshal
            ,HDC_UserUnmarshal
            ,HDC_UserFree
            },
            {
            HBITMAP_UserSize
            ,HBITMAP_UserMarshal
            ,HBITMAP_UserUnmarshal
            ,HBITMAP_UserFree
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Standard interface: __MIDL_itf_Usgfw2_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IUsgDataView, ver. 0.0,
   GUID={0xBFEF8670,0xDBC1,0x4B47,{0xBC,0x01,0xBB,0xBC,0xA1,0x74,0xDD,0x31}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDataView_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    56,
    84,
    112,
    140,
    174,
    202,
    248
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataView_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDataView_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDataView_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(16) _IUsgDataViewProxyVtbl = 
{
    &IUsgDataView_ProxyInfo,
    &IID_IUsgDataView,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::get_ScanMode */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::put_ScanMode */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::get_ScanState */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::put_ScanState */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::get_ScanModes */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::GetScanModeObj */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::get_FilgraphManager */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::GetControlObj */ ,
    (void *) (INT_PTR) -1 /* IUsgDataView::CreateUsgfwFile */
};


static const PRPC_STUB_FUNCTION IUsgDataView_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDataViewStubVtbl =
{
    &IID_IUsgDataView,
    &IUsgDataView_ServerInfo,
    16,
    &IUsgDataView_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanMode, ver. 0.0,
   GUID={0xB8CBA727,0xD104,0x416E,{0xB0,0xCC,0xC6,0x2E,0x9C,0xF9,0xB1,0xF9}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanMode_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    288,
    328,
    374,
    402,
    430,
    458,
    486
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanMode_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanMode_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanMode_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IUsgScanModeProxyVtbl = 
{
    &IUsgScanMode_ProxyInfo,
    &IID_IUsgScanMode,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::GetMixerControl */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::GetControlObj */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::CreateCopy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::Union */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::get_StreamsCollection */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::get_ScanMode */ ,
    (void *) (INT_PTR) -1 /* IUsgScanMode::CreateClone */
};


static const PRPC_STUB_FUNCTION IUsgScanMode_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanModeStubVtbl =
{
    &IID_IUsgScanMode,
    &IUsgScanMode_ServerInfo,
    14,
    &IUsgScanMode_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDataStream, ver. 0.0,
   GUID={0xEDF488C3,0x5BE4,0x4D32,{0xB9,0x6F,0xD9,0xBD,0xDF,0xC7,0x7C,0x43}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDataStream_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    514,
    548,
    576,
    604,
    632
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDataStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDataStream_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IUsgDataStreamProxyVtbl = 
{
    &IUsgDataStream_ProxyInfo,
    &IID_IUsgDataStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDataStream::GetStreamMode */ ,
    (void *) (INT_PTR) -1 /* IUsgDataStream::GetControlObj */ ,
    (void *) (INT_PTR) -1 /* IUsgDataStream::GetMixerControl */ ,
    (void *) (INT_PTR) -1 /* IUsgDataStream::get_StreamId */ ,
    (void *) (INT_PTR) -1 /* IUsgDataStream::get_ScanState */ ,
    (void *) (INT_PTR) -1 /* IUsgDataStream::put_ScanState */
};


static const PRPC_STUB_FUNCTION IUsgDataStream_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDataStreamStubVtbl =
{
    &IID_IUsgDataStream,
    &IUsgDataStream_ServerInfo,
    13,
    &IUsgDataStream_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgMixerControl, ver. 0.0,
   GUID={0xB7C94539,0xA65A,0x42F3,{0x8B,0x65,0xF8,0x3D,0x11,0x4F,0xF4,0xC8}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgMixerControl_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    660,
    688,
    716,
    744,
    772,
    800,
    828,
    856,
    890,
    918,
    946,
    974,
    1002
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgMixerControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgMixerControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgMixerControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgMixerControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(20) _IUsgMixerControlProxyVtbl = 
{
    &IUsgMixerControl_ProxyInfo,
    &IID_IUsgMixerControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::SetOutputWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::GetAlphaBitmapParameters */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::SetAlphaBitmap */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::UpdateAlphaBitmapParameters */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::SetOutputRect */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::Capture */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::GetCurrentBitmap */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::Repaint */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::get_ScanState */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::put_ScanState */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::get_BkColor */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::put_BkColor */ ,
    (void *) (INT_PTR) -1 /* IUsgMixerControl::GetOutputRect */
};


static const PRPC_STUB_FUNCTION IUsgMixerControl_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgMixerControlStubVtbl =
{
    &IID_IUsgMixerControl,
    &IUsgMixerControl_ServerInfo,
    20,
    &IUsgMixerControl_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgData, ver. 0.0,
   GUID={0x4363F1DB,0x1261,0x4BD6,{0x99,0xE5,0xB4,0x83,0xEC,0xB3,0x52,0x18}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgData_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgData_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgData_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgData_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgData_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IUsgDataProxyVtbl = 
{
    0,
    &IID_IUsgData,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IUsgData_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IUsgDataStubVtbl =
{
    &IID_IUsgData,
    &IUsgData_ServerInfo,
    7,
    &IUsgData_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgGraph, ver. 0.0,
   GUID={0x739FDDCE,0x29FF,0x44D9,{0x9C,0x3C,0x3E,0x68,0x13,0xA6,0x79,0x69}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgGraph_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgGraph_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgGraph_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgGraph_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgGraph_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IUsgGraphProxyVtbl = 
{
    0,
    &IID_IUsgGraph,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IUsgGraph_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IUsgGraphStubVtbl =
{
    &IID_IUsgGraph,
    &IUsgGraph_ServerInfo,
    7,
    &IUsgGraph_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDeviceChangeSink, ver. 0.0,
   GUID={0x9717780E,0xAAAF,0x4FD2,{0x83,0x5A,0x80,0x91,0x0E,0x1E,0x80,0x3E}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDeviceChangeSink_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1030,
    1064,
    1098,
    1132,
    1166,
    1200
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceChangeSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDeviceChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IUsgDeviceChangeSinkProxyVtbl = 
{
    &IUsgDeviceChangeSink_ProxyInfo,
    &IID_IUsgDeviceChangeSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceChangeSink::OnProbeArrive */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceChangeSink::OnBeamformerArrive */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceChangeSink::OnProbeRemove */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceChangeSink::OnBeamformerRemove */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceChangeSink::OnProbeStateChanged */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceChangeSink::OnBeamformerStateChanged */
};


static const PRPC_STUB_FUNCTION IUsgDeviceChangeSink_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDeviceChangeSinkStubVtbl =
{
    &IID_IUsgDeviceChangeSink,
    &IUsgDeviceChangeSink_ServerInfo,
    13,
    &IUsgDeviceChangeSink_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IScanDepth, ver. 0.0,
   GUID={0x7391AEBB,0x13BB,0x4ffe,{0xAE,0x84,0x48,0xCD,0x63,0xB5,0x23,0xA0}} */

#pragma code_seg(".orpc")
static const unsigned short IScanDepth_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1234
    };

static const MIDL_STUBLESS_PROXY_INFO IScanDepth_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IScanDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScanDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IScanDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IScanDepthProxyVtbl = 
{
    &IScanDepth_ProxyInfo,
    &IID_IScanDepth,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IScanDepth::GetScanDepth */ ,
    (void *) (INT_PTR) -1 /* IScanDepth::SetScanDepth */ ,
    (void *) (INT_PTR) -1 /* IScanDepth::GetScanDepthRange */
};


static const PRPC_STUB_FUNCTION IScanDepth_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IScanDepthStubVtbl =
{
    &IID_IScanDepth,
    &IScanDepth_ServerInfo,
    10,
    &IScanDepth_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgfw2, ver. 0.0,
   GUID={0xAAE0C833,0xBFE6,0x4594,{0x98,0x4E,0x8B,0x9F,0xD4,0x8C,0xA4,0x87}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgfw2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1268,
    1296,
    1324,
    1358,
    1380,
    1414,
    1448,
    1482,
    248
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgfw2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgfw2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgfw2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(16) _IUsgfw2ProxyVtbl = 
{
    &IUsgfw2_ProxyInfo,
    &IID_IUsgfw2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::get_ProbesCollection */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::get_BeamformersCollection */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::CreateDataView */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::Invalidate */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::BeamformerNameFromCode */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::BeamformerCodeFromName */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::ProbeNameFromCode */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::ProbeCodeFromName */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2::CreateUsgfwFile */
};


static const PRPC_STUB_FUNCTION IUsgfw2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgfw2StubVtbl =
{
    &IID_IUsgfw2,
    &IUsgfw2_ServerInfo,
    16,
    &IUsgfw2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IProbesCollection, ver. 0.0,
   GUID={0x1C3AF9E8,0x2597,0x4A1C,{0xAD,0xEA,0x6F,0x9A,0x17,0x64,0x5A,0x16}} */

#pragma code_seg(".orpc")
static const unsigned short IProbesCollection_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IProbesCollection_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IProbesCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbesCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IProbesCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IProbesCollectionProxyVtbl = 
{
    0,
    &IID_IProbesCollection,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IProbesCollection_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IProbesCollectionStubVtbl =
{
    &IID_IProbesCollection,
    &IProbesCollection_ServerInfo,
    7,
    &IProbesCollection_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCollection, ver. 0.0,
   GUID={0xEAA864EC,0xF0B8,0x49EF,{0xBF,0x78,0x09,0xB8,0x37,0x9D,0x0D,0x62}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCollection_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    1516
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCollection_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgCollectionProxyVtbl = 
{
    &IUsgCollection_ProxyInfo,
    &IID_IUsgCollection,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCollection::get_Count */ ,
    (void *) (INT_PTR) -1 /* IUsgCollection::Item */
};


static const PRPC_STUB_FUNCTION IUsgCollection_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCollectionStubVtbl =
{
    &IID_IUsgCollection,
    &IUsgCollection_ServerInfo,
    9,
    &IUsgCollection_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IProbe, ver. 0.0,
   GUID={0x264096B1,0x8393,0x4060,{0x90,0x7B,0x91,0x7C,0x39,0x5F,0xF9,0x7C}} */

#pragma code_seg(".orpc")
static const unsigned short IProbe_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1550,
    1578,
    56,
    1606,
    1634
    };

static const MIDL_STUBLESS_PROXY_INFO IProbe_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IProbe_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbe_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IProbe_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IProbeProxyVtbl = 
{
    &IProbe_ProxyInfo,
    &IID_IProbe,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_Name */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_Beamformer */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_DeviceState */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_DispalyName */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_Code */
};


static const PRPC_STUB_FUNCTION IProbe_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IProbeStubVtbl =
{
    &IID_IProbe,
    &IProbe_ServerInfo,
    12,
    &IProbe_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IProbe2, ver. 0.0,
   GUID={0x7500FEC3,0xE775,0x4d0c,{0x91,0xD8,0x59,0xDF,0x9C,0x3E,0xD7,0xEB}} */

#pragma code_seg(".orpc")
static const unsigned short IProbe2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1550,
    1578,
    56,
    1606,
    1634,
    1662,
    1448
    };

static const MIDL_STUBLESS_PROXY_INFO IProbe2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IProbe2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbe2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IProbe2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IProbe2ProxyVtbl = 
{
    &IProbe2_ProxyInfo,
    &IID_IProbe2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_Name */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_Beamformer */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_DeviceState */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_DispalyName */ ,
    (void *) (INT_PTR) -1 /* IProbe::get_Code */ ,
    (void *) (INT_PTR) -1 /* IProbe2::get_Type */ ,
    (void *) (INT_PTR) -1 /* IProbe2::get_SerialNumber */
};


static const PRPC_STUB_FUNCTION IProbe2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IProbe2StubVtbl =
{
    &IID_IProbe2,
    &IProbe2_ServerInfo,
    14,
    &IProbe2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IBeamformer, ver. 0.0,
   GUID={0x1AF2973E,0x1991,0x4A7A,{0x86,0xAF,0x7E,0xA0,0x15,0x0C,0x69,0x25}} */

#pragma code_seg(".orpc")
static const unsigned short IBeamformer_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1550,
    1578,
    56,
    1606,
    1690,
    1712,
    828,
    1734
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformer_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBeamformer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBeamformer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBeamformer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(15) _IBeamformerProxyVtbl = 
{
    &IBeamformer_ProxyInfo,
    &IID_IBeamformer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::get_Name */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::get_Probes */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::get_DeviceState */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::get_DisplayName */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::OpenDevice */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::SwitchConnector */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::SwitchConnectorSupported */ ,
    (void *) (INT_PTR) -1 /* IBeamformer::get_Code */
};


static const PRPC_STUB_FUNCTION IBeamformer_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IBeamformerStubVtbl =
{
    &IID_IBeamformer,
    &IBeamformer_ServerInfo,
    15,
    &IBeamformer_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IScanModes, ver. 0.0,
   GUID={0x60C480B7,0xF1E7,0x403C,{0x8A,0xF3,0x8D,0xCE,0xD9,0x9A,0x25,0x60}} */

#pragma code_seg(".orpc")
static const unsigned short IScanModes_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    1762
    };

static const MIDL_STUBLESS_PROXY_INFO IScanModes_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IScanModes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScanModes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IScanModes_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IScanModesProxyVtbl = 
{
    &IScanModes_ProxyInfo,
    &IID_IScanModes,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IScanModes::get_Count */ ,
    (void *) (INT_PTR) -1 /* IScanModes::Item */
};


static const PRPC_STUB_FUNCTION IScanModes_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IScanModesStubVtbl =
{
    &IID_IScanModes,
    &IScanModes_ServerInfo,
    9,
    &IScanModes_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgControl, ver. 0.0,
   GUID={0x5748CA80,0x1710,0x489F,{0xBC,0x13,0x28,0xF2,0xC0,0x12,0x2B,0x49}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgControl_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IUsgControlProxyVtbl = 
{
    0,
    &IID_IUsgControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IUsgControl_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IUsgControlStubVtbl =
{
    &IID_IUsgControl,
    &IUsgControl_ServerInfo,
    7,
    &IUsgControl_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgGain, ver. 0.0,
   GUID={0xA18F0D3F,0xDD69,0x4BDE,{0x8F,0x26,0x4F,0x54,0xD6,0x7B,0x57,0xD0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgGain_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgGain_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgGain_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgGain_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgGain_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgGainProxyVtbl = 
{
    &IUsgGain_ProxyInfo,
    &IID_IUsgGain,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgGain::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgGain::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgGain::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgGain_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgGainStubVtbl =
{
    &IID_IUsgGain,
    &IUsgGain_ServerInfo,
    10,
    &IUsgGain_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgValues, ver. 0.0,
   GUID={0x95D11D2B,0xEC05,0x4A2E,{0xB3,0x1B,0x13,0x86,0xC4,0x84,0xAE,0x16}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgValues_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    1824
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgValues_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgValues_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgValues_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgValues_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgValuesProxyVtbl = 
{
    &IUsgValues_ProxyInfo,
    &IID_IUsgValues,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgValues::get_Count */ ,
    (void *) (INT_PTR) -1 /* IUsgValues::Item */
};


static const PRPC_STUB_FUNCTION IUsgValues_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgValuesStubVtbl =
{
    &IID_IUsgValues,
    &IUsgValues_ServerInfo,
    9,
    &IUsgValues_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgPower, ver. 0.0,
   GUID={0xF23DC92E,0x60CB,0x4EAE,{0x8C,0xD1,0xBD,0x72,0x9E,0x8D,0x78,0x5C}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgPower_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPower_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgPower_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPower_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgPower_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgPowerProxyVtbl = 
{
    &IUsgPower_ProxyInfo,
    &IID_IUsgPower,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgPower::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgPower::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgPower::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgPower_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgPowerStubVtbl =
{
    &IID_IUsgPower,
    &IUsgPower_ServerInfo,
    10,
    &IUsgPower_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDynamicRange, ver. 0.0,
   GUID={0xB3194B41,0x4E87,0x4787,{0x8E,0x79,0x25,0x63,0x3A,0x75,0x96,0xD9}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDynamicRange_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDynamicRange_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDynamicRange_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDynamicRange_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDynamicRange_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDynamicRangeProxyVtbl = 
{
    &IUsgDynamicRange_ProxyInfo,
    &IID_IUsgDynamicRange,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDynamicRange::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDynamicRange::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDynamicRange::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDynamicRange_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDynamicRangeStubVtbl =
{
    &IID_IUsgDynamicRange,
    &IUsgDynamicRange_ServerInfo,
    10,
    &IUsgDynamicRange_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgFrameAvg, ver. 0.0,
   GUID={0x3FD666AF,0xC2B5,0x4A5D,{0xAF,0xCA,0x30,0xED,0xE0,0x34,0x27,0xE2}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgFrameAvg_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFrameAvg_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgFrameAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFrameAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgFrameAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgFrameAvgProxyVtbl = 
{
    &IUsgFrameAvg_ProxyInfo,
    &IID_IUsgFrameAvg,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameAvg::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameAvg::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameAvg::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgFrameAvg_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgFrameAvgStubVtbl =
{
    &IID_IUsgFrameAvg,
    &IUsgFrameAvg_ServerInfo,
    10,
    &IUsgFrameAvg_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgRejection2, ver. 0.0,
   GUID={0x3FE7E792,0x42A5,0x45D1,{0xB0,0x54,0x7B,0xF4,0x7C,0x67,0xDB,0xFB}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgRejection2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgRejection2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgRejection2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgRejection2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgRejection2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgRejection2ProxyVtbl = 
{
    &IUsgRejection2_ProxyInfo,
    &IID_IUsgRejection2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgRejection2::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgRejection2::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgRejection2::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgRejection2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgRejection2StubVtbl =
{
    &IID_IUsgRejection2,
    &IUsgRejection2_ServerInfo,
    10,
    &IUsgRejection2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgProbeFrequency2, ver. 0.0,
   GUID={0x53FCF15D,0x3C94,0x4AB3,{0x9B,0x8E,0x0C,0xD6,0x7D,0x73,0x3A,0x24}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgProbeFrequency2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgProbeFrequency2ProxyVtbl = 
{
    &IUsgProbeFrequency2_ProxyInfo,
    &IID_IUsgProbeFrequency2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeFrequency2::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeFrequency2::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeFrequency2::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgProbeFrequency2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgProbeFrequency2StubVtbl =
{
    &IID_IUsgProbeFrequency2,
    &IUsgProbeFrequency2_ServerInfo,
    10,
    &IUsgProbeFrequency2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDepth, ver. 0.0,
   GUID={0x812D829E,0x9D55,0x406A,{0xB8,0x9D,0x31,0xA4,0x10,0x83,0x9F,0x87}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDepth_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDepth_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDepthProxyVtbl = 
{
    &IUsgDepth_ProxyInfo,
    &IID_IUsgDepth,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDepth::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDepth::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDepth::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDepth_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDepthStubVtbl =
{
    &IID_IUsgDepth,
    &IUsgDepth_ServerInfo,
    10,
    &IUsgDepth_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgImageOrientation, ver. 0.0,
   GUID={0x859BCBDB,0x015C,0x4439,{0x97,0x02,0xF0,0xCB,0x0F,0xDF,0x80,0x59}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgImageOrientation_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    56,
    84
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageOrientation_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgImageOrientation_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageOrientation_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgImageOrientation_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _IUsgImageOrientationProxyVtbl = 
{
    &IUsgImageOrientation_ProxyInfo,
    &IID_IUsgImageOrientation,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgImageOrientation::get_Mirror */ ,
    (void *) (INT_PTR) -1 /* IUsgImageOrientation::put_Mirror */ ,
    (void *) (INT_PTR) -1 /* IUsgImageOrientation::get_Rotate */ ,
    (void *) (INT_PTR) -1 /* IUsgImageOrientation::put_Rotate */
};


static const PRPC_STUB_FUNCTION IUsgImageOrientation_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgImageOrientationStubVtbl =
{
    &IID_IUsgImageOrientation,
    &IUsgImageOrientation_ServerInfo,
    11,
    &IUsgImageOrientation_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgImageEnhancement, ver. 0.0,
   GUID={0x90C02711,0x657D,0x436C,{0xB8,0x65,0xDA,0x76,0xE7,0xB5,0xEA,0x76}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgImageEnhancement_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageEnhancement_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageEnhancement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgImageEnhancementProxyVtbl = 
{
    &IUsgImageEnhancement_ProxyInfo,
    &IID_IUsgImageEnhancement,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgImageEnhancement::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgImageEnhancement::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgImageEnhancement::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgImageEnhancement::get_Enabled */ ,
    (void *) (INT_PTR) -1 /* IUsgImageEnhancement::put_Enabled */
};


static const PRPC_STUB_FUNCTION IUsgImageEnhancement_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgImageEnhancementStubVtbl =
{
    &IID_IUsgImageEnhancement,
    &IUsgImageEnhancement_ServerInfo,
    12,
    &IUsgImageEnhancement_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgViewArea, ver. 0.0,
   GUID={0x0F5EAEE8,0x9C4E,0x4714,{0x8F,0x85,0x17,0xD3,0x1C,0xD2,0x5F,0xC6}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgViewArea_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgViewArea_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgViewArea_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgViewArea_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgViewArea_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgViewAreaProxyVtbl = 
{
    &IUsgViewArea_ProxyInfo,
    &IID_IUsgViewArea,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgViewArea::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgViewArea::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgViewArea::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgViewArea_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgViewAreaStubVtbl =
{
    &IID_IUsgViewArea,
    &IUsgViewArea_ServerInfo,
    10,
    &IUsgViewArea_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgLineDensity, ver. 0.0,
   GUID={0x118427F8,0xBAAC,0x4F29,{0xB8,0x5C,0xDC,0xFC,0xD6,0x35,0x73,0xFE}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgLineDensity_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgLineDensity_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgLineDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgLineDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgLineDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgLineDensityProxyVtbl = 
{
    &IUsgLineDensity_ProxyInfo,
    &IID_IUsgLineDensity,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgLineDensity::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgLineDensity::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgLineDensity::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgLineDensity_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgLineDensityStubVtbl =
{
    &IID_IUsgLineDensity,
    &IUsgLineDensity_ServerInfo,
    10,
    &IUsgLineDensity_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgFocus, ver. 0.0,
   GUID={0x53502AB7,0xC0FB,0x4B31,{0xA4,0xEB,0x23,0xC0,0x92,0xD6,0xD1,0x3A}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgFocus_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    1886,
    1920,
    1954,
    1988,
    2028,
    2074,
    2120,
    2166
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFocus_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgFocus_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFocus_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgFocus_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IUsgFocusProxyVtbl = 
{
    &IUsgFocus_ProxyInfo,
    &IID_IUsgFocus,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::get_CurrentMode */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::put_CurrentMode */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::get_ValuesMode */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::get_FocusSet */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::put_FocusSet */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::get_FocusSetCount */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::GetFocalZonesCount */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::GetFocalZone */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::GetFocusPoint */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::GetFocusState */ ,
    (void *) (INT_PTR) -1 /* IUsgFocus::SetFocusState */
};


static const PRPC_STUB_FUNCTION IUsgFocus_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgFocusStubVtbl =
{
    &IID_IUsgFocus,
    &IUsgFocus_ServerInfo,
    18,
    &IUsgFocus_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgTgc, ver. 0.0,
   GUID={0x5AA2CBAF,0x30F9,0x4F20,{0xA7,0xF8,0xBB,0x77,0xA7,0xC8,0x6D,0x71}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgTgc_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    2212,
    2252,
    2292,
    1886,
    2320,
    1954,
    2354,
    1734,
    2388,
    2416,
    2450
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgTgc_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgTgc_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgTgc_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgTgc_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IUsgTgcProxyVtbl = 
{
    &IUsgTgc_ProxyInfo,
    &IID_IUsgTgc,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::GetTgcEntries */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::SetTgcEntries */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::GetTgcSize */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::PositionToDepth */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::DepthToPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::get_CtlPointsCount */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::put_CtlPointsCount */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::get_CtlPointPos */ ,
    (void *) (INT_PTR) -1 /* IUsgTgc::get_ValuesTgc */
};


static const PRPC_STUB_FUNCTION IUsgTgc_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgTgcStubVtbl =
{
    &IID_IUsgTgc,
    &IUsgTgc_ServerInfo,
    18,
    &IUsgTgc_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgClearView, ver. 0.0,
   GUID={0x5B07F59F,0xE2B9,0x4045,{0x9C,0x23,0xBB,0xAA,0x62,0x88,0x60,0x78}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgClearView_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgClearView_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgClearView_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgClearView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgClearView_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgClearViewProxyVtbl = 
{
    &IUsgClearView_ProxyInfo,
    &IID_IUsgClearView,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgClearView::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgClearView::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgClearView::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgClearView::get_Enabled */ ,
    (void *) (INT_PTR) -1 /* IUsgClearView::put_Enabled */
};


static const PRPC_STUB_FUNCTION IUsgClearView_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgClearViewStubVtbl =
{
    &IID_IUsgClearView,
    &IUsgClearView_ServerInfo,
    12,
    &IUsgClearView_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgPaletteCalculator, ver. 0.0,
   GUID={0x537B5EA9,0x246B,0x4AF5,{0xA1,0x99,0x58,0x93,0xED,0x41,0x62,0x0E}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgPaletteCalculator_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    2292,
    402,
    1634,
    2478,
    828,
    2506,
    2534,
    2562,
    2590,
    2618,
    2646,
    2674,
    2702,
    2742,
    2782
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPaletteCalculator_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPaletteCalculator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(24) _IUsgPaletteCalculatorProxyVtbl = 
{
    &IUsgPaletteCalculator_ProxyInfo,
    &IID_IUsgPaletteCalculator,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_Brightness */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::put_Brightness */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_Contrast */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::put_Contrast */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_Gamma */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::put_Gamma */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_Negative */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::put_Negative */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_BrightnessMin */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_BrightnessMax */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_ContrastMin */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_ContrastMax */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_GammaMin */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::get_GammaMax */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::SetReferenceData */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::GetReferenceData */ ,
    (void *) (INT_PTR) -1 /* IUsgPaletteCalculator::Calculate */
};


static const PRPC_STUB_FUNCTION IUsgPaletteCalculator_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgPaletteCalculatorStubVtbl =
{
    &IID_IUsgPaletteCalculator,
    &IUsgPaletteCalculator_ServerInfo,
    24,
    &IUsgPaletteCalculator_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgPalette, ver. 0.0,
   GUID={0x39F0DB4B,0x5197,0x4E11,{0xAB,0xB6,0x7C,0x87,0x35,0xE6,0xB7,0xAE}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgPalette_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    2822,
    2862
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPalette_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgPalette_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPalette_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgPalette_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgPaletteProxyVtbl = 
{
    &IUsgPalette_ProxyInfo,
    &IID_IUsgPalette,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgPalette::SetPaletteEntries */ ,
    (void *) (INT_PTR) -1 /* IUsgPalette::GetPaletteEntries */
};


static const PRPC_STUB_FUNCTION IUsgPalette_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgPaletteStubVtbl =
{
    &IID_IUsgPalette,
    &IUsgPalette_ServerInfo,
    9,
    &IUsgPalette_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgImageProperties, ver. 0.0,
   GUID={0x27C0A0A4,0x475B,0x423C,{0xBF,0x8B,0x82,0xFC,0x56,0xAD,0x75,0x73}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgImageProperties_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    2902,
    2942,
    2976,
    3010,
    1634,
    2478,
    828,
    2506,
    3038,
    3090,
    3142,
    3188,
    3240,
    3280
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageProperties_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgImageProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgImageProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(21) _IUsgImagePropertiesProxyVtbl = 
{
    &IUsgImageProperties_ProxyInfo,
    &IID_IUsgImageProperties,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::GetOrigin */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::SetOrigin */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::GetResolution */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::SetResolution */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::get_AutoProbeCenter */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::put_AutoProbeCenter */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::get_AutoResolution */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::put_AutoResolution */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::PixelsToUnits */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::UnitsToPixels */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::SetViewRect */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::GetViewRect */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::SetZoom */ ,
    (void *) (INT_PTR) -1 /* IUsgImageProperties::GetZoom */
};


static const PRPC_STUB_FUNCTION IUsgImageProperties_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgImagePropertiesStubVtbl =
{
    &IID_IUsgImageProperties,
    &IUsgImageProperties_ServerInfo,
    21,
    &IUsgImageProperties_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgControlChangeSink, ver. 0.0,
   GUID={0x9E38438A,0x733B,0x4784,{0x8C,0x68,0x60,0x24,0x1E,0xD4,0x98,0x59}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgControlChangeSink_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3326
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgControlChangeSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgControlChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IUsgControlChangeSinkProxyVtbl = 
{
    &IUsgControlChangeSink_ProxyInfo,
    &IID_IUsgControlChangeSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgControlChangeSink::ControlChanged */
};


static const PRPC_STUB_FUNCTION IUsgControlChangeSink_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgControlChangeSinkStubVtbl =
{
    &IID_IUsgControlChangeSink,
    &IUsgControlChangeSink_ServerInfo,
    8,
    &IUsgControlChangeSink_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCtrlChangeCommon, ver. 0.0,
   GUID={0xAFA64E76,0x249A,0x4606,{0x8E,0xEF,0xE4,0xFD,0x80,0x2A,0xE9,0xEF}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCtrlChangeCommon_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3360,
    3418
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCtrlChangeCommon_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCtrlChangeCommon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgCtrlChangeCommonProxyVtbl = 
{
    &IUsgCtrlChangeCommon_ProxyInfo,
    &IID_IUsgCtrlChangeCommon,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCtrlChangeCommon::OnControlChanged */ ,
    (void *) (INT_PTR) -1 /* IUsgCtrlChangeCommon::OnControlChangedBSTR */
};


static const PRPC_STUB_FUNCTION IUsgCtrlChangeCommon_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCtrlChangeCommonStubVtbl =
{
    &IID_IUsgCtrlChangeCommon,
    &IUsgCtrlChangeCommon_ServerInfo,
    9,
    &IUsgCtrlChangeCommon_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLine, ver. 0.0,
   GUID={0xAC0CEFF6,0x21E9,0x472F,{0xB1,0xA3,0xFA,0xF1,0x85,0x57,0xA0,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanLine_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    3476,
    3516,
    3562
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLine_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanLine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanLine_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IUsgScanLineProxyVtbl = 
{
    &IUsgScanLine_ProxyInfo,
    &IID_IUsgScanLine,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine::GetScanLine */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine::GetCoordinates */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine::GetCoordinates2 */
};


static const PRPC_STUB_FUNCTION IUsgScanLine_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanLineStubVtbl =
{
    &IID_IUsgScanLine,
    &IUsgScanLine_ServerInfo,
    13,
    &IUsgScanLine_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLineProperties, ver. 0.0,
   GUID={0xEF1D6EDE,0x1AB3,0x429B,{0x83,0x48,0x6B,0xEA,0x1A,0x43,0x50,0x0E}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanLineProperties_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3626,
    3672,
    3736,
    3776,
    3810
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgScanLinePropertiesProxyVtbl = 
{
    &IUsgScanLineProperties_ProxyInfo,
    &IID_IUsgScanLineProperties,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties::GetCoordinates */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties::GetCoordinates2 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties::GetScanLine */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties::GetScanLinesRange */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties::GetPointDepth */
};


static const PRPC_STUB_FUNCTION IUsgScanLineProperties_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanLinePropertiesStubVtbl =
{
    &IID_IUsgScanLineProperties,
    &IUsgScanLineProperties_ServerInfo,
    12,
    &IUsgScanLineProperties_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLineSelector, ver. 0.0,
   GUID={0x3BFE461D,0x4240,0x40AC,{0xB5,0xFF,0x29,0x2A,0x6C,0x25,0x3A,0x4C}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanLineSelector_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineSelector_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineSelector_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgScanLineSelectorProxyVtbl = 
{
    &IUsgScanLineSelector_ProxyInfo,
    &IID_IUsgScanLineSelector,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineSelector::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineSelector::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineSelector::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgScanLineSelector_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanLineSelectorStubVtbl =
{
    &IID_IUsgScanLineSelector,
    &IUsgScanLineSelector_ServerInfo,
    10,
    &IUsgScanLineSelector_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgSweepMode, ver. 0.0,
   GUID={0x4DD0E32D,0x23BF,0x4591,{0xB7,0x6D,0x2C,0x97,0x1B,0xB8,0x95,0x07}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgSweepMode_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3850,
    3878,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepMode_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgSweepMode_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSweepMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgSweepMode_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgSweepModeProxyVtbl = 
{
    &IUsgSweepMode_ProxyInfo,
    &IID_IUsgSweepMode,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepMode::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepMode::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepMode::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgSweepMode_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgSweepModeStubVtbl =
{
    &IID_IUsgSweepMode,
    &IUsgSweepMode_ServerInfo,
    10,
    &IUsgSweepMode_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgQualProp, ver. 0.0,
   GUID={0x0DF080D7,0x1180,0x4c94,{0x90,0x03,0x16,0x81,0x74,0xCD,0x3A,0xCD}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgQualProp_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    3906,
    2292
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgQualProp_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgQualProp_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgQualProp_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgQualProp_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgQualPropProxyVtbl = 
{
    &IUsgQualProp_ProxyInfo,
    &IID_IUsgQualProp,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgQualProp::get_AvgFrameRate */ ,
    (void *) (INT_PTR) -1 /* IUsgQualProp::get_FramesDrawn */ ,
    (void *) (INT_PTR) -1 /* IUsgQualProp::get_FramesDroppedInRenderer */
};


static const PRPC_STUB_FUNCTION IUsgQualProp_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgQualPropStubVtbl =
{
    &IID_IUsgQualProp,
    &IUsgQualProp_ServerInfo,
    10,
    &IUsgQualProp_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgSweepSpeed, ver. 0.0,
   GUID={0x481482A0,0xB0E1,0x460E,{0x92,0xB9,0x31,0x59,0xED,0x9E,0x27,0x48}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgSweepSpeed_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858,
    3934
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepSpeed_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSweepSpeed_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IUsgSweepSpeedProxyVtbl = 
{
    &IUsgSweepSpeed_ProxyInfo,
    &IID_IUsgSweepSpeed,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepSpeed::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepSpeed::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepSpeed::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepSpeed::get_SweepSpeed */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepSpeed::put_SweepSpeed */ ,
    (void *) (INT_PTR) -1 /* IUsgSweepSpeed::Preset */
};


static const PRPC_STUB_FUNCTION IUsgSweepSpeed_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgSweepSpeedStubVtbl =
{
    &IID_IUsgSweepSpeed,
    &IUsgSweepSpeed_ServerInfo,
    13,
    &IUsgSweepSpeed_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerColorMap, ver. 0.0,
   GUID={0x39B7413A,0x07E4,0x492D,{0x8A,0x38,0x5A,0x5E,0x78,0x8E,0x30,0xD1}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerColorMap_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3980,
    2862
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorMap_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorMap_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgDopplerColorMapProxyVtbl = 
{
    &IUsgDopplerColorMap_ProxyInfo,
    &IID_IUsgDopplerColorMap,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorMap::SetColorMap */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorMap::GetPaletteEntries */
};


static const PRPC_STUB_FUNCTION IUsgDopplerColorMap_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerColorMapStubVtbl =
{
    &IID_IUsgDopplerColorMap,
    &IUsgDopplerColorMap_ServerInfo,
    9,
    &IUsgDopplerColorMap_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerColorPriority, ver. 0.0,
   GUID={0x5134078D,0x9B3D,0x4DB4,{0xB7,0xF6,0xBA,0xF5,0xC6,0x01,0x80,0xE0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerColorPriority_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorPriority_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorPriority_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgDopplerColorPriorityProxyVtbl = 
{
    &IUsgDopplerColorPriority_ProxyInfo,
    &IID_IUsgDopplerColorPriority,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorPriority::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorPriority::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorPriority::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorPriority::get_PriorityMode */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorPriority::put_PriorityMode */
};


static const PRPC_STUB_FUNCTION IUsgDopplerColorPriority_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerColorPriorityStubVtbl =
{
    &IID_IUsgDopplerColorPriority,
    &IUsgDopplerColorPriority_ServerInfo,
    12,
    &IUsgDopplerColorPriority_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerSteerAngle, ver. 0.0,
   GUID={0x3373936B,0x1232,0x4E94,{0x9B,0xA9,0x45,0x65,0xD7,0x36,0x63,0x5D}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerSteerAngle_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSteerAngle_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSteerAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerSteerAngleProxyVtbl = 
{
    &IUsgDopplerSteerAngle_ProxyInfo,
    &IID_IUsgDopplerSteerAngle,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSteerAngle::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSteerAngle::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSteerAngle::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerSteerAngle_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerSteerAngleStubVtbl =
{
    &IID_IUsgDopplerSteerAngle,
    &IUsgDopplerSteerAngle_ServerInfo,
    10,
    &IUsgDopplerSteerAngle_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerColorThreshold, ver. 0.0,
   GUID={0xCD490C38,0x98B9,0x487A,{0x9B,0x91,0x65,0x3C,0x80,0x6F,0xFD,0x21}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerColorThreshold_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorThreshold_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorThreshold_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerColorThresholdProxyVtbl = 
{
    &IUsgDopplerColorThreshold_ProxyInfo,
    &IID_IUsgDopplerColorThreshold,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorThreshold::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorThreshold::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorThreshold::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerColorThreshold_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerColorThresholdStubVtbl =
{
    &IID_IUsgDopplerColorThreshold,
    &IUsgDopplerColorThreshold_ServerInfo,
    10,
    &IUsgDopplerColorThreshold_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerBaseLine, ver. 0.0,
   GUID={0xF78B3D8F,0xF0D9,0x4129,{0xA0,0xC1,0xB9,0x97,0x75,0x77,0xBE,0xA2}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerBaseLine_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerBaseLine_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerBaseLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerBaseLineProxyVtbl = 
{
    &IUsgDopplerBaseLine_ProxyInfo,
    &IID_IUsgDopplerBaseLine,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerBaseLine::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerBaseLine::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerBaseLine::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerBaseLine_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerBaseLineStubVtbl =
{
    &IID_IUsgDopplerBaseLine,
    &IUsgDopplerBaseLine_ServerInfo,
    10,
    &IUsgDopplerBaseLine_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerPRF, ver. 0.0,
   GUID={0xD37B8F18,0x417C,0x406B,{0x8E,0x5D,0xBA,0xEC,0x11,0x62,0x34,0x28}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerPRF_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    1886,
    2320
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPRF_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerPRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgDopplerPRFProxyVtbl = 
{
    &IUsgDopplerPRF_ProxyInfo,
    &IID_IUsgDopplerPRF,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPRF::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPRF::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPRF::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPRF::get_VisibleMin */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPRF::get_VisibleMax */
};


static const PRPC_STUB_FUNCTION IUsgDopplerPRF_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerPRFStubVtbl =
{
    &IID_IUsgDopplerPRF,
    &IUsgDopplerPRF_ServerInfo,
    12,
    &IUsgDopplerPRF_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerWindow, ver. 0.0,
   GUID={0x7D485326,0x1EAD,0x43C7,{0xBC,0x9A,0xC5,0x7C,0xF2,0x51,0xD4,0xD3}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerWindow_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4026,
    4072,
    4118,
    4152,
    4186,
    4220,
    4254,
    4282,
    4310,
    4338
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWindow_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IUsgDopplerWindowProxyVtbl = 
{
    &IUsgDopplerWindow_ProxyInfo,
    &IID_IUsgDopplerWindow,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::SetWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::SetWidth */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::SetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::GetWidth */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::GetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::get_LinesRange */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::get_WidthRange */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::get_DepthsRange */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWindow::get_VolumesRange */
};


static const PRPC_STUB_FUNCTION IUsgDopplerWindow_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerWindowStubVtbl =
{
    &IID_IUsgDopplerWindow,
    &IUsgDopplerWindow_ServerInfo,
    17,
    &IUsgDopplerWindow_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerWallFilter, ver. 0.0,
   GUID={0x6A62BE4A,0x23C3,0x4262,{0xB1,0xC6,0xC2,0x09,0x33,0x61,0x5E,0x90}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerWallFilter_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    1886,
    2320
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWallFilter_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerWallFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgDopplerWallFilterProxyVtbl = 
{
    &IUsgDopplerWallFilter_ProxyInfo,
    &IID_IUsgDopplerWallFilter,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWallFilter::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWallFilter::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWallFilter::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWallFilter::get_VisibleMin */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerWallFilter::get_VisibleMax */
};


static const PRPC_STUB_FUNCTION IUsgDopplerWallFilter_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerWallFilterStubVtbl =
{
    &IID_IUsgDopplerWallFilter,
    &IUsgDopplerWallFilter_ServerInfo,
    12,
    &IUsgDopplerWallFilter_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerSignalScale, ver. 0.0,
   GUID={0x507BA161,0xF30F,0x4B86,{0x9D,0xB2,0x10,0x7B,0x89,0x84,0x1A,0x0B}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerSignalScale_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalScale_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalScale_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerSignalScaleProxyVtbl = 
{
    &IUsgDopplerSignalScale_ProxyInfo,
    &IID_IUsgDopplerSignalScale,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSignalScale::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSignalScale::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSignalScale::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerSignalScale_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerSignalScaleStubVtbl =
{
    &IID_IUsgDopplerSignalScale,
    &IUsgDopplerSignalScale_ServerInfo,
    10,
    &IUsgDopplerSignalScale_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerPacketSize, ver. 0.0,
   GUID={0xC500DFDD,0xACA3,0x4594,{0xA0,0xEE,0x75,0xC0,0x89,0xB3,0x98,0x0C}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerPacketSize_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPacketSize_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerPacketSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerPacketSizeProxyVtbl = 
{
    &IUsgDopplerPacketSize_ProxyInfo,
    &IID_IUsgDopplerPacketSize,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPacketSize::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPacketSize::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerPacketSize::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerPacketSize_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerPacketSizeStubVtbl =
{
    &IID_IUsgDopplerPacketSize,
    &IUsgDopplerPacketSize_ServerInfo,
    10,
    &IUsgDopplerPacketSize_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgPulsesNumber, ver. 0.0,
   GUID={0x629FA89F,0x7BDB,0x4B79,{0xB3,0xF3,0xA5,0x5A,0xEA,0x07,0x8B,0xC0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgPulsesNumber_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPulsesNumber_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPulsesNumber_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgPulsesNumberProxyVtbl = 
{
    &IUsgPulsesNumber_ProxyInfo,
    &IID_IUsgPulsesNumber,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgPulsesNumber::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgPulsesNumber::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgPulsesNumber::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgPulsesNumber_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgPulsesNumberStubVtbl =
{
    &IID_IUsgPulsesNumber,
    &IUsgPulsesNumber_ServerInfo,
    10,
    &IUsgPulsesNumber_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCineloop, ver. 0.0,
   GUID={0xA2986CE3,0x3F1A,0x4361,{0x89,0x0D,0x94,0x81,0x6E,0xD1,0xCC,0xF7}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCineloop_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4366,
    4394,
    4422,
    4450,
    4478,
    4506,
    4534,
    4562,
    4590,
    4618,
    4652,
    4686,
    2646,
    4720
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineloop_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCineloop_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineloop_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCineloop_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(21) _IUsgCineloopProxyVtbl = 
{
    &IUsgCineloop_ProxyInfo,
    &IID_IUsgCineloop,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::get_CurrentPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::put_CurrentPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::get_Duration */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::get_Rate */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::put_Rate */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::get_StopTime */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::put_StopTime */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::get_State */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::put_State */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::GetAvailable */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::GetPositions */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::SetPositions */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::GetCapabilities */ ,
    (void *) (INT_PTR) -1 /* IUsgCineloop::CheckCapabilities */
};


static const PRPC_STUB_FUNCTION IUsgCineloop_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCineloopStubVtbl =
{
    &IID_IUsgCineloop,
    &IUsgCineloop_ServerInfo,
    21,
    &IUsgCineloop_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCineStream, ver. 0.0,
   GUID={0x5071C20D,0x306B,0x4EC2,{0xAB,0xA0,0x1E,0x20,0xE7,0xD5,0xEA,0x7E}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCineStream_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4366,
    4394,
    4422,
    4748,
    4776,
    4804,
    4832,
    4562,
    4590,
    4860,
    4888,
    4916,
    4962,
    4996,
    5042,
    5076,
    5110,
    5144,
    5184
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineStream_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCineStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCineStream_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(26) _IUsgCineStreamProxyVtbl = 
{
    &IUsgCineStream_ProxyInfo,
    &IID_IUsgCineStream,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::get_CurrentPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::put_CurrentPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::get_StopPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::put_StopPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::get_Duration */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::get_Rate */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::put_Rate */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::get_State */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::put_State */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::get_TimeFormat */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::put_TimeFormat */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::ConvertTimeFormat */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::GetAvailable */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::GetShiftPosition */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::SetHoldRange */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::GetHoldRange */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::IsTimeFormatSupported */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::GetContinuousInterval */ ,
    (void *) (INT_PTR) -1 /* IUsgCineStream::SetSelection */
};


static const PRPC_STUB_FUNCTION IUsgCineStream_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCineStreamStubVtbl =
{
    &IID_IUsgCineStream,
    &IUsgCineStream_ServerInfo,
    26,
    &IUsgCineStream_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCineSink, ver. 0.0,
   GUID={0x04D658E8,0x6691,0x4034,{0xAD,0xFD,0x39,0xEB,0xE7,0xA0,0x30,0xDA}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCineSink_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5224,
    5264,
    5286,
    5314
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineSink_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCineSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCineSink_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _IUsgCineSinkProxyVtbl = 
{
    &IUsgCineSink_ProxyInfo,
    &IID_IUsgCineSink,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink::GetBufferSize */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink::Reset */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink::SetBufferSize */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink::SaveData */
};


static const PRPC_STUB_FUNCTION IUsgCineSink_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCineSinkStubVtbl =
{
    &IID_IUsgCineSink,
    &IUsgCineSink_ServerInfo,
    11,
    &IUsgCineSink_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgSpatialFilter, ver. 0.0,
   GUID={0x31BF1183,0x35D2,0x4EF3,{0xA1,0xB1,0xAD,0xEB,0xDE,0xDD,0x81,0xA0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgSpatialFilter_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSpatialFilter_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSpatialFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgSpatialFilterProxyVtbl = 
{
    &IUsgSpatialFilter_ProxyInfo,
    &IID_IUsgSpatialFilter,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgSpatialFilter::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSpatialFilter::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSpatialFilter::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgSpatialFilter::get_Enabled */ ,
    (void *) (INT_PTR) -1 /* IUsgSpatialFilter::put_Enabled */
};


static const PRPC_STUB_FUNCTION IUsgSpatialFilter_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgSpatialFilterStubVtbl =
{
    &IID_IUsgSpatialFilter,
    &IUsgSpatialFilter_ServerInfo,
    12,
    &IUsgSpatialFilter_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerSampleVolume, ver. 0.0,
   GUID={0xF81C0C94,0xB620,0x46EA,{0xB6,0xC3,0x5D,0x10,0xE4,0xD7,0xF0,0x6C}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerSampleVolume_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3326,
    5342,
    5376,
    5404,
    5432,
    4220
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSampleVolume_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSampleVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IUsgDopplerSampleVolumeProxyVtbl = 
{
    &IUsgDopplerSampleVolume_ProxyInfo,
    &IID_IUsgDopplerSampleVolume,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSampleVolume::SetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSampleVolume::GetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSampleVolume::get_DepthsRange */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSampleVolume::get_VolumesRange */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSampleVolume::SetSampleVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSampleVolume::GetPhantomVolume */
};


static const PRPC_STUB_FUNCTION IUsgDopplerSampleVolume_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerSampleVolumeStubVtbl =
{
    &IID_IUsgDopplerSampleVolume,
    &IUsgDopplerSampleVolume_ServerInfo,
    13,
    &IUsgDopplerSampleVolume_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerCorrectionAngle, ver. 0.0,
   GUID={0x2BFE46DC,0xDD1A,0x42C8,{0x8B,0xDB,0xD0,0x34,0xD5,0xFA,0x04,0xDB}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerCorrectionAngle_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerCorrectionAngle_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerCorrectionAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerCorrectionAngleProxyVtbl = 
{
    &IUsgDopplerCorrectionAngle_ProxyInfo,
    &IID_IUsgDopplerCorrectionAngle,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerCorrectionAngle::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerCorrectionAngle::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerCorrectionAngle::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerCorrectionAngle_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerCorrectionAngleStubVtbl =
{
    &IID_IUsgDopplerCorrectionAngle,
    &IUsgDopplerCorrectionAngle_ServerInfo,
    10,
    &IUsgDopplerCorrectionAngle_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanConverterPlugin, ver. 0.0,
   GUID={0xDD280DD5,0x674A,0x4837,{0x9F,0x03,0x9F,0xDE,0x77,0x59,0x65,0x99}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanConverterPlugin_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5472
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IUsgScanConverterPluginProxyVtbl = 
{
    &IUsgScanConverterPlugin_ProxyInfo,
    &IID_IUsgScanConverterPlugin,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanConverterPlugin::get_ScanConverter */
};


static const PRPC_STUB_FUNCTION IUsgScanConverterPlugin_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanConverterPluginStubVtbl =
{
    &IID_IUsgScanConverterPlugin,
    &IUsgScanConverterPlugin_ServerInfo,
    8,
    &IUsgScanConverterPlugin_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanConverterPlugin2, ver. 0.0,
   GUID={0x2F84D02F,0xE381,0x4b94,{0xAB,0x10,0x52,0xD6,0x40,0x13,0xAF,0x4C}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanConverterPlugin2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5472,
    3906,
    2292
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgScanConverterPlugin2ProxyVtbl = 
{
    &IUsgScanConverterPlugin2_ProxyInfo,
    &IID_IUsgScanConverterPlugin2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanConverterPlugin::get_ScanConverter */ ,
    (void *) (INT_PTR) -1 /* IUsgScanConverterPlugin2::get_InputPin */ ,
    (void *) (INT_PTR) -1 /* IUsgScanConverterPlugin2::get_OutputPin */
};


static const PRPC_STUB_FUNCTION IUsgScanConverterPlugin2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanConverterPlugin2StubVtbl =
{
    &IID_IUsgScanConverterPlugin2,
    &IUsgScanConverterPlugin2_ServerInfo,
    10,
    &IUsgScanConverterPlugin2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerSignalSmooth, ver. 0.0,
   GUID={0x4D2095BD,0xD9D0,0x421F,{0x87,0x87,0xA0,0xC7,0x64,0xBE,0xD7,0x35}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerSignalSmooth_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalSmooth_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalSmooth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerSignalSmoothProxyVtbl = 
{
    &IUsgDopplerSignalSmooth_ProxyInfo,
    &IID_IUsgDopplerSignalSmooth,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSignalSmooth::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSignalSmooth::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSignalSmooth::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerSignalSmooth_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerSignalSmoothStubVtbl =
{
    &IID_IUsgDopplerSignalSmooth,
    &IUsgDopplerSignalSmooth_ServerInfo,
    10,
    &IUsgDopplerSignalSmooth_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgAudioVolume, ver. 0.0,
   GUID={0x17EBD173,0x3BBD,0x427C,{0x98,0x44,0xF2,0x74,0x09,0x9B,0x05,0x62}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgAudioVolume_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgAudioVolume_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgAudioVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgAudioVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgAudioVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgAudioVolumeProxyVtbl = 
{
    &IUsgAudioVolume_ProxyInfo,
    &IID_IUsgAudioVolume,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgAudioVolume::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgAudioVolume::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgAudioVolume::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgAudioVolume::get_Mute */ ,
    (void *) (INT_PTR) -1 /* IUsgAudioVolume::put_Mute */
};


static const PRPC_STUB_FUNCTION IUsgAudioVolume_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgAudioVolumeStubVtbl =
{
    &IID_IUsgAudioVolume,
    &IUsgAudioVolume_ServerInfo,
    12,
    &IUsgAudioVolume_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerSpectralAvg, ver. 0.0,
   GUID={0x7BAAC412,0x6FF0,0x42C0,{0x85,0xA0,0x9D,0x67,0x9E,0x91,0xFF,0x6B}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerSpectralAvg_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSpectralAvg_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSpectralAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDopplerSpectralAvgProxyVtbl = 
{
    &IUsgDopplerSpectralAvg_ProxyInfo,
    &IID_IUsgDopplerSpectralAvg,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSpectralAvg::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSpectralAvg::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerSpectralAvg::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDopplerSpectralAvg_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerSpectralAvgStubVtbl =
{
    &IID_IUsgDopplerSpectralAvg,
    &IUsgDopplerSpectralAvg_ServerInfo,
    10,
    &IUsgDopplerSpectralAvg_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgBioeffectsIndices, ver. 0.0,
   GUID={0x533907B0,0x42A7,0x474D,{0xAB,0x97,0x34,0x2E,0xEB,0xDD,0x9A,0xA0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgBioeffectsIndices_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5500,
    5534
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgBioeffectsIndices_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgBioeffectsIndices_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgBioeffectsIndicesProxyVtbl = 
{
    &IUsgBioeffectsIndices_ProxyInfo,
    &IID_IUsgBioeffectsIndices,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgBioeffectsIndices::IsSupported */ ,
    (void *) (INT_PTR) -1 /* IUsgBioeffectsIndices::GetIndex */
};


static const PRPC_STUB_FUNCTION IUsgBioeffectsIndices_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgBioeffectsIndicesStubVtbl =
{
    &IID_IUsgBioeffectsIndices,
    &IUsgBioeffectsIndices_ServerInfo,
    9,
    &IUsgBioeffectsIndices_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgProbeFrequency3, ver. 0.0,
   GUID={0x491CFD05,0x2F69,0x42F4,{0x95,0x14,0xAE,0x47,0x89,0x0B,0x6E,0x1E}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgProbeFrequency3_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5568,
    5596,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency3_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency3_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgProbeFrequency3ProxyVtbl = 
{
    &IUsgProbeFrequency3_ProxyInfo,
    &IID_IUsgProbeFrequency3,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeFrequency3::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeFrequency3::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeFrequency3::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgProbeFrequency3_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgProbeFrequency3StubVtbl =
{
    &IID_IUsgProbeFrequency3,
    &IUsgProbeFrequency3_ServerInfo,
    10,
    &IUsgProbeFrequency3_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDopplerColorTransparency, ver. 0.0,
   GUID={0xB27183A9,0x33AF,0x40AC,{0xA1,0x32,0x11,0xF0,0x34,0xDE,0x78,0x17}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDopplerColorTransparency_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796,
    576,
    1858
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorTransparency_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorTransparency_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IUsgDopplerColorTransparencyProxyVtbl = 
{
    &IUsgDopplerColorTransparency_ProxyInfo,
    &IID_IUsgDopplerColorTransparency,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorTransparency::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorTransparency::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorTransparency::get_Values */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorTransparency::get_TransparencyMode */ ,
    (void *) (INT_PTR) -1 /* IUsgDopplerColorTransparency::put_TransparencyMode */
};


static const PRPC_STUB_FUNCTION IUsgDopplerColorTransparency_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDopplerColorTransparencyStubVtbl =
{
    &IID_IUsgDopplerColorTransparency,
    &IUsgDopplerColorTransparency_ServerInfo,
    12,
    &IUsgDopplerColorTransparency_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsg3dVolumeSize, ver. 0.0,
   GUID={0x145E5DFC,0x246E,0x4B55,{0x8E,0xB1,0x4C,0x95,0x6E,0x98,0x85,0x4F}} */

#pragma code_seg(".orpc")
static const unsigned short IUsg3dVolumeSize_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeSize_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsg3dVolumeSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsg3dVolumeSizeProxyVtbl = 
{
    &IUsg3dVolumeSize_ProxyInfo,
    &IID_IUsg3dVolumeSize,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsg3dVolumeSize::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsg3dVolumeSize::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsg3dVolumeSize::get_Values */
};


static const PRPC_STUB_FUNCTION IUsg3dVolumeSize_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsg3dVolumeSizeStubVtbl =
{
    &IID_IUsg3dVolumeSize,
    &IUsg3dVolumeSize_ServerInfo,
    10,
    &IUsg3dVolumeSize_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsg3dVolumeDensity, ver. 0.0,
   GUID={0x42C2A978,0xC31B,0x4235,{0x92,0x92,0xE5,0x37,0x26,0xE5,0xA6,0x1C}} */

#pragma code_seg(".orpc")
static const unsigned short IUsg3dVolumeDensity_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeDensity_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsg3dVolumeDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsg3dVolumeDensityProxyVtbl = 
{
    &IUsg3dVolumeDensity_ProxyInfo,
    &IID_IUsg3dVolumeDensity,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsg3dVolumeDensity::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsg3dVolumeDensity::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsg3dVolumeDensity::get_Values */
};


static const PRPC_STUB_FUNCTION IUsg3dVolumeDensity_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsg3dVolumeDensityStubVtbl =
{
    &IID_IUsg3dVolumeDensity,
    &IUsg3dVolumeDensity_ServerInfo,
    10,
    &IUsg3dVolumeDensity_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgFileStorage, ver. 0.0,
   GUID={0x4CF81935,0xDCB3,0x4C19,{0x9D,0xC9,0x76,0x53,0x06,0xA4,0x71,0x4F}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgFileStorage_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1550,
    5624,
    5652,
    402,
    5674,
    5714,
    5760,
    5788,
    5828,
    5874,
    5914,
    5972
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFileStorage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgFileStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFileStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgFileStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(19) _IUsgFileStorageProxyVtbl = 
{
    &IUsgFileStorage_ProxyInfo,
    &IID_IUsgFileStorage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::get_FileName */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::put_FileName */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::Load */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::Save */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::CreateDataKey */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::OpenDataKey */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::CloseDataKey */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::GetKeyInfo */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::EnumValues */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::EnumSubkeys */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::GetValue */ ,
    (void *) (INT_PTR) -1 /* IUsgFileStorage::SetValue */
};


static const PRPC_STUB_FUNCTION IUsgFileStorage_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgFileStorageStubVtbl =
{
    &IID_IUsgFileStorage,
    &IUsgFileStorage_ServerInfo,
    19,
    &IUsgFileStorage_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgfw2Debug, ver. 0.0,
   GUID={0x1E181F99,0x13FB,0x4570,{0x99,0x34,0x09,0x4D,0x6D,0x52,0x74,0xA9}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgfw2Debug_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    6024
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2Debug_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgfw2Debug_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgfw2Debug_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgfw2Debug_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IUsgfw2DebugProxyVtbl = 
{
    &IUsgfw2Debug_ProxyInfo,
    &IID_IUsgfw2Debug,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgfw2Debug::CreateObjectsDump */
};


static const PRPC_STUB_FUNCTION IUsgfw2Debug_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgfw2DebugStubVtbl =
{
    &IID_IUsgfw2Debug,
    &IUsgfw2Debug_ServerInfo,
    8,
    &IUsgfw2Debug_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgPlugin, ver. 0.0,
   GUID={0x19E2FD36,0x9D47,0x4A86,{0xBF,0x5E,0xCD,0x92,0xA0,0x0F,0x16,0xD5}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgPlugin_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    6064,
    28
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgPluginProxyVtbl = 
{
    &IUsgPlugin_ProxyInfo,
    &IID_IUsgPlugin,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgPlugin::AddPlugin */ ,
    (void *) (INT_PTR) -1 /* IUsgPlugin::RemovePlugin */
};


static const PRPC_STUB_FUNCTION IUsgPlugin_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgPluginStubVtbl =
{
    &IID_IUsgPlugin,
    &IUsgPlugin_ServerInfo,
    9,
    &IUsgPlugin_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IBeamformerPowerState, ver. 0.0,
   GUID={0x9D1D0EB0,0xC497,0x42EE,{0xBB,0x75,0xEB,0x0B,0xA5,0xF7,0x74,0xDC}} */

#pragma code_seg(".orpc")
static const unsigned short IBeamformerPowerState_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    3850,
    3906,
    2292,
    576,
    604
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformerPowerState_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IBeamformerPowerState_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBeamformerPowerState_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IBeamformerPowerState_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IBeamformerPowerStateProxyVtbl = 
{
    &IBeamformerPowerState_ProxyInfo,
    &IID_IBeamformerPowerState,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IBeamformerPowerState::get_BatteryChargeStatus */ ,
    (void *) (INT_PTR) -1 /* IBeamformerPowerState::get_BatteryFullLifetime */ ,
    (void *) (INT_PTR) -1 /* IBeamformerPowerState::get_BatteryLifePercent */ ,
    (void *) (INT_PTR) -1 /* IBeamformerPowerState::get_BatteryLifeRemaining */ ,
    (void *) (INT_PTR) -1 /* IBeamformerPowerState::get_PowerLineStatus */
};


static const PRPC_STUB_FUNCTION IBeamformerPowerState_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IBeamformerPowerStateStubVtbl =
{
    &IID_IBeamformerPowerState,
    &IBeamformerPowerState_ServerInfo,
    12,
    &IBeamformerPowerState_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanType, ver. 0.0,
   GUID={0xEF4959EF,0xAE06,0x414B,{0xB2,0x90,0x67,0x51,0x27,0xF0,0x03,0xD0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanType_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanType_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgScanType_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanType_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgScanType_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgScanTypeProxyVtbl = 
{
    &IUsgScanType_ProxyInfo,
    &IID_IUsgScanType,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgScanType::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgScanType::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgScanType::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgScanType_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgScanTypeStubVtbl =
{
    &IID_IUsgScanType,
    &IUsgScanType_ServerInfo,
    10,
    &IUsgScanType_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgSteeringAngle, ver. 0.0,
   GUID={0xA0D966E1,0x6C45,0x44E3,{0x98,0x87,0x14,0x2D,0xE3,0x07,0x68,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgSteeringAngle_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSteeringAngle_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSteeringAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgSteeringAngleProxyVtbl = 
{
    &IUsgSteeringAngle_ProxyInfo,
    &IID_IUsgSteeringAngle,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgSteeringAngle::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSteeringAngle::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgSteeringAngle::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgSteeringAngle_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgSteeringAngleStubVtbl =
{
    &IID_IUsgSteeringAngle,
    &IUsgSteeringAngle_ServerInfo,
    10,
    &IUsgSteeringAngle_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgViewAngle, ver. 0.0,
   GUID={0xD3CAA86B,0x8D04,0x4FFD,{0x8F,0x4E,0xF9,0xE1,0x58,0x05,0x1D,0x5B}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgViewAngle_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgViewAngle_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgViewAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgViewAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgViewAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgViewAngleProxyVtbl = 
{
    &IUsgViewAngle_ProxyInfo,
    &IID_IUsgViewAngle,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgViewAngle::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgViewAngle::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgViewAngle::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgViewAngle_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgViewAngleStubVtbl =
{
    &IID_IUsgViewAngle,
    &IUsgViewAngle_ServerInfo,
    10,
    &IUsgViewAngle_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCompoundFrames, ver. 0.0,
   GUID={0x2CB1500C,0x8196,0x47FF,{0xBE,0xCF,0x61,0x94,0x7E,0x01,0x92,0xC5}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCompoundFrames_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCompoundFrames_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCompoundFrames_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgCompoundFramesProxyVtbl = 
{
    &IUsgCompoundFrames_ProxyInfo,
    &IID_IUsgCompoundFrames,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCompoundFrames::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgCompoundFrames::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgCompoundFrames::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgCompoundFrames_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCompoundFramesStubVtbl =
{
    &IID_IUsgCompoundFrames,
    &IUsgCompoundFrames_ServerInfo,
    10,
    &IUsgCompoundFrames_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCompoundAngle, ver. 0.0,
   GUID={0xEEA419CB,0x8B31,0x47A3,{0xA0,0x70,0xA6,0x8C,0xEE,0x24,0xF3,0xF1}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCompoundAngle_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    1796
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCompoundAngle_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCompoundAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgCompoundAngleProxyVtbl = 
{
    &IUsgCompoundAngle_ProxyInfo,
    &IID_IUsgCompoundAngle,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCompoundAngle::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgCompoundAngle::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgCompoundAngle::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgCompoundAngle_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgCompoundAngleStubVtbl =
{
    &IID_IUsgCompoundAngle,
    &IUsgCompoundAngle_ServerInfo,
    10,
    &IUsgCompoundAngle_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDeviceCapabilities, ver. 0.0,
   GUID={0x360D17D2,0xA12A,0x4bd0,{0x80,0x51,0xDA,0x3C,0xCB,0xFB,0xB9,0xB7}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDeviceCapabilities_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    6098
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceCapabilities_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDeviceCapabilities_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IUsgDeviceCapabilitiesProxyVtbl = 
{
    &IUsgDeviceCapabilities_ProxyInfo,
    &IID_IUsgDeviceCapabilities,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDeviceCapabilities::ScanModeCaps */
};


static const PRPC_STUB_FUNCTION IUsgDeviceCapabilities_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDeviceCapabilitiesStubVtbl =
{
    &IID_IUsgDeviceCapabilities,
    &IUsgDeviceCapabilities_ServerInfo,
    8,
    &IUsgDeviceCapabilities_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgUnits, ver. 0.0,
   GUID={0xE06602A1,0xEBE0,0x4E20,{0xB3,0x9C,0xB9,0x38,0x06,0x04,0x34,0xBE}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgUnits_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    6144,
    6178,
    6212
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgUnits_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IUsgUnits_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgUnits_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IUsgUnits_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgUnitsProxyVtbl = 
{
    &IUsgUnits_ProxyInfo,
    &IID_IUsgUnits,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgUnits::GetCurrent */ ,
    (void *) (INT_PTR) -1 /* IUsgUnits::SetCurrent */ ,
    (void *) (INT_PTR) -1 /* IUsgUnits::GetValues */
};


static const PRPC_STUB_FUNCTION IUsgUnits_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgUnitsStubVtbl =
{
    &IID_IUsgUnits,
    &IUsgUnits_ServerInfo,
    10,
    &IUsgUnits_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x600016e, /* MIDL Version 6.0.366 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _Usgfw2_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IUsgProbeFrequency3ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCompoundFramesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCineStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDeviceChangeSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgImageEnhancementProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSpectralAvgProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerPRFProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLineSelectorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerWindowProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanModeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgValuesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgSweepModeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgPowerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanConverterPlugin2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgfw2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgFileStorageProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgPluginProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerColorThresholdProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgMixerControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerColorMapProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBeamformerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgGainProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDynamicRangeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerWallFilterProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgPaletteProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgProbeFrequency2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSignalScaleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSteerAngleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgViewAngleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDataViewProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgAudioVolumeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCtrlChangeCommonProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsg3dVolumeDensityProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgControlProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgSpatialFilterProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgControlChangeSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerColorPriorityProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerBaseLineProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgRejection2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSampleVolumeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgfw2DebugProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDepthProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgPulsesNumberProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgClearViewProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgSweepSpeedProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgUnitsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgImagePropertiesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgPaletteCalculatorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerColorTransparencyProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgFrameAvgProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgTgcProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgBioeffectsIndicesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBeamformerPowerStateProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IProbeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgFocusProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IScanModesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IScanDepthProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSignalSmoothProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDataStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IProbe2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCompoundAngleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgGraphProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDeviceCapabilitiesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanConverterPluginProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgQualPropProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgImageOrientationProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDataProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerCorrectionAngleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerPacketSizeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLinePropertiesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgSteeringAngleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCineloopProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCineSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgViewAreaProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IProbesCollectionProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCollectionProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanTypeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLineProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgLineDensityProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsg3dVolumeSizeProxyVtbl,
    0
};

const CInterfaceStubVtbl * _Usgfw2_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IUsgProbeFrequency3StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCompoundFramesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCineStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDeviceChangeSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgImageEnhancementStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSpectralAvgStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerPRFStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanLineSelectorStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerWindowStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanModeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgValuesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgSweepModeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgPowerStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanConverterPlugin2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgfw2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgFileStorageStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgPluginStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerColorThresholdStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgMixerControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerColorMapStubVtbl,
    ( CInterfaceStubVtbl *) &_IBeamformerStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgGainStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDynamicRangeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerWallFilterStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgPaletteStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgProbeFrequency2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSignalScaleStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSteerAngleStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgViewAngleStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDataViewStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgAudioVolumeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCtrlChangeCommonStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsg3dVolumeDensityStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgControlStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgSpatialFilterStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgControlChangeSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerColorPriorityStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerBaseLineStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgRejection2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSampleVolumeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgfw2DebugStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDepthStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgPulsesNumberStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgClearViewStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgSweepSpeedStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgUnitsStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgImagePropertiesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgPaletteCalculatorStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerColorTransparencyStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgFrameAvgStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgTgcStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgBioeffectsIndicesStubVtbl,
    ( CInterfaceStubVtbl *) &_IBeamformerPowerStateStubVtbl,
    ( CInterfaceStubVtbl *) &_IProbeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgFocusStubVtbl,
    ( CInterfaceStubVtbl *) &_IScanModesStubVtbl,
    ( CInterfaceStubVtbl *) &_IScanDepthStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSignalSmoothStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDataStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IProbe2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCompoundAngleStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgGraphStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDeviceCapabilitiesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanConverterPluginStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgQualPropStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgImageOrientationStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDataStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerCorrectionAngleStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerPacketSizeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanLinePropertiesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgSteeringAngleStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCineloopStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCineSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgViewAreaStubVtbl,
    ( CInterfaceStubVtbl *) &_IProbesCollectionStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCollectionStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanTypeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanLineStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgLineDensityStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsg3dVolumeSizeStubVtbl,
    0
};

PCInterfaceName const _Usgfw2_InterfaceNamesList[] = 
{
    "IUsgProbeFrequency3",
    "IUsgCompoundFrames",
    "IUsgCineStream",
    "IUsgDeviceChangeSink",
    "IUsgImageEnhancement",
    "IUsgDopplerSpectralAvg",
    "IUsgDopplerPRF",
    "IUsgScanLineSelector",
    "IUsgDopplerWindow",
    "IUsgScanMode",
    "IUsgValues",
    "IUsgSweepMode",
    "IUsgPower",
    "IUsgScanConverterPlugin2",
    "IUsgfw2",
    "IUsgFileStorage",
    "IUsgPlugin",
    "IUsgDopplerColorThreshold",
    "IUsgMixerControl",
    "IUsgDopplerColorMap",
    "IBeamformer",
    "IUsgGain",
    "IUsgDynamicRange",
    "IUsgDopplerWallFilter",
    "IUsgPalette",
    "IUsgProbeFrequency2",
    "IUsgDopplerSignalScale",
    "IUsgDopplerSteerAngle",
    "IUsgViewAngle",
    "IUsgDataView",
    "IUsgAudioVolume",
    "IUsgCtrlChangeCommon",
    "IUsg3dVolumeDensity",
    "IUsgControl",
    "IUsgSpatialFilter",
    "IUsgControlChangeSink",
    "IUsgDopplerColorPriority",
    "IUsgDopplerBaseLine",
    "IUsgRejection2",
    "IUsgDopplerSampleVolume",
    "IUsgfw2Debug",
    "IUsgDepth",
    "IUsgPulsesNumber",
    "IUsgClearView",
    "IUsgSweepSpeed",
    "IUsgUnits",
    "IUsgImageProperties",
    "IUsgPaletteCalculator",
    "IUsgDopplerColorTransparency",
    "IUsgFrameAvg",
    "IUsgTgc",
    "IUsgBioeffectsIndices",
    "IBeamformerPowerState",
    "IProbe",
    "IUsgFocus",
    "IScanModes",
    "IScanDepth",
    "IUsgDopplerSignalSmooth",
    "IUsgDataStream",
    "IProbe2",
    "IUsgCompoundAngle",
    "IUsgGraph",
    "IUsgDeviceCapabilities",
    "IUsgScanConverterPlugin",
    "IUsgQualProp",
    "IUsgImageOrientation",
    "IUsgData",
    "IUsgDopplerCorrectionAngle",
    "IUsgDopplerPacketSize",
    "IUsgScanLineProperties",
    "IUsgSteeringAngle",
    "IUsgCineloop",
    "IUsgCineSink",
    "IUsgViewArea",
    "IProbesCollection",
    "IUsgCollection",
    "IUsgScanType",
    "IUsgScanLine",
    "IUsgLineDensity",
    "IUsg3dVolumeSize",
    0
};

const IID *  _Usgfw2_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _Usgfw2_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _Usgfw2, pIID, n)

int __stdcall _Usgfw2_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _Usgfw2, 80, 64 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 32 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _Usgfw2, 80, *pIndex )
    
}

const ExtendedProxyFileInfo Usgfw2_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _Usgfw2_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _Usgfw2_StubVtblList,
    (const PCInterfaceName * ) & _Usgfw2_InterfaceNamesList,
    (const IID ** ) & _Usgfw2_BaseIIDList,
    & _Usgfw2_IID_Lookup, 
    80,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

