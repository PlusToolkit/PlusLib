

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for Usgfw2.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "Usgfw2_h.h"

#define TYPE_FORMAT_STRING_SIZE   1663                              
#define PROC_FORMAT_STRING_SIZE   7753                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   4            

typedef struct _Usgfw2_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } Usgfw2_MIDL_TYPE_FORMAT_STRING;

typedef struct _Usgfw2_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } Usgfw2_MIDL_PROC_FORMAT_STRING;

typedef struct _Usgfw2_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } Usgfw2_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const Usgfw2_MIDL_TYPE_FORMAT_STRING Usgfw2__MIDL_TypeFormatString;
extern const Usgfw2_MIDL_PROC_FORMAT_STRING Usgfw2__MIDL_ProcFormatString;
extern const Usgfw2_MIDL_EXPR_FORMAT_STRING Usgfw2__MIDL_ExprFormatString;


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


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISampleGrabberFilter_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISampleGrabberFilter_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgWindowRF_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgWindowRF_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgStreamEnable_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgStreamEnable_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgDataSourceRF_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgDataSourceRF_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgMultiBeam_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgMultiBeam_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const Usgfw2_MIDL_PROC_FORMAT_STRING Usgfw2__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure get_Current */


	/* Procedure get_Current */


	/* Procedure get_Enable */


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
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

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


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pScanDepth */


	/* Parameter pMode */


	/* Parameter pVal */

/* 24 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
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


	/* Return value */


	/* Return value */


	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */


	/* Procedure put_Current */


	/* Procedure put_Enable */


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

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


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

/* 60 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	0x8,		/* FC_LONG */
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

/* 66 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rotate */


	/* Procedure get_DeviceState */


	/* Procedure get_DeviceState */


	/* Procedure get_ScanState */

/* 72 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 74 */	NdrFcLong( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x9 ),	/* 9 */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x24 ),	/* 36 */
/* 86 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 88 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 90 */	NdrFcShort( 0x0 ),	/* 0 */
/* 92 */	NdrFcShort( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 96 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 98 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 100 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rotate */


	/* Procedure put_ScanState */

/* 108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0xa ),	/* 10 */
/* 116 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 122 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 124 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 136 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanModes */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0xb ),	/* 11 */
/* 152 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 160 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 168 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Return value */

/* 174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanModeObj */

/* 180 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0xc ),	/* 12 */
/* 188 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 196 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanMode */

/* 204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 208 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pUsgScanMode */

/* 210 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 212 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 214 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FilgraphManager */

/* 222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0xd ),	/* 13 */
/* 230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 236 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 238 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 246 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 248 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 250 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0xe ),	/* 14 */
/* 266 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 268 */	NdrFcShort( 0x54 ),	/* 84 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 274 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riidCtrl */

/* 282 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 284 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 286 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 288 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 290 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 294 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 296 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 298 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppObj */

/* 300 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 302 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 304 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 308 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateUsgfwFile */


	/* Procedure CreateUsgfwFile */

/* 312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0xf ),	/* 15 */
/* 320 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 326 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 328 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x1 ),	/* 1 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fileName */


	/* Parameter FileName */

/* 336 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 340 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter createFlags */


	/* Parameter createFlags */

/* 342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppFileObject */


	/* Parameter ppFileObject */

/* 348 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 350 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 352 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Return value */


	/* Return value */

/* 354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 356 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMixerControl */

/* 360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x7 ),	/* 7 */
/* 368 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 370 */	NdrFcShort( 0x10 ),	/* 16 */
/* 372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 374 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 376 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanMode */

/* 384 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 390 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppMixerCtrl */

/* 396 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 398 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 400 */	NdrFcShort( 0xb2 ),	/* Type Offset=178 */

	/* Return value */

/* 402 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 404 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 408 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 410 */	NdrFcLong( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 418 */	NdrFcShort( 0x54 ),	/* 84 */
/* 420 */	NdrFcShort( 0x8 ),	/* 8 */
/* 422 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 424 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riidCtrl */

/* 432 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 434 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 436 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 438 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 440 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 444 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 446 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppCtrl */

/* 450 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 452 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 454 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 458 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateCopy */

/* 462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x9 ),	/* 9 */
/* 470 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x8 ),	/* 8 */
/* 476 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 478 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 484 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppNewScanMode */

/* 486 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 488 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 490 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 494 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */


	/* Procedure put_Contrast */


	/* Procedure Union */

/* 498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 504 */	NdrFcShort( 0xa ),	/* 10 */
/* 506 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 508 */	NdrFcShort( 0x8 ),	/* 8 */
/* 510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 512 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 514 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 520 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter saveFlags */


	/* Parameter newVal */


	/* Parameter nScanMode */

/* 522 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 524 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 530 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StreamsCollection */

/* 534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 540 */	NdrFcShort( 0xb ),	/* 11 */
/* 542 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x8 ),	/* 8 */
/* 548 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 550 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 558 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 562 */	NdrFcShort( 0xc8 ),	/* Type Offset=200 */

	/* Return value */

/* 564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanMode */

/* 570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0xc ),	/* 12 */
/* 578 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x24 ),	/* 36 */
/* 584 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 586 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 594 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 596 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 602 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateClone */

/* 606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0xd ),	/* 13 */
/* 614 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 620 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 622 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 628 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppNewScanMode */

/* 630 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 632 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 634 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 636 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 638 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 640 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 642 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 644 */	NdrFcLong( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x8 ),	/* 8 */
/* 650 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 652 */	NdrFcShort( 0x44 ),	/* 68 */
/* 654 */	NdrFcShort( 0x8 ),	/* 8 */
/* 656 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 658 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */

/* 666 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 668 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 670 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter ppobj */

/* 672 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 674 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 676 */	NdrFcShort( 0xc8 ),	/* Type Offset=200 */

	/* Return value */

/* 678 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 680 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMixerControl */

/* 684 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 686 */	NdrFcLong( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x9 ),	/* 9 */
/* 692 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0x8 ),	/* 8 */
/* 698 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 700 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppMixerControl */

/* 708 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 712 */	NdrFcShort( 0xb2 ),	/* Type Offset=178 */

	/* Return value */

/* 714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 718 */	0x8,		/* FC_LONG */
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

/* 720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 726 */	NdrFcShort( 0xa ),	/* 10 */
/* 728 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0x24 ),	/* 36 */
/* 734 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 736 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 744 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 746 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 748 */	0x8,		/* FC_LONG */
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

/* 750 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_PowerLineStatus */


	/* Procedure get_ScanState */

/* 756 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 758 */	NdrFcLong( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0xb ),	/* 11 */
/* 764 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x24 ),	/* 36 */
/* 770 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 772 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 780 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 782 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 784 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 786 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 788 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_ScanState */

/* 792 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 798 */	NdrFcShort( 0xc ),	/* 12 */
/* 800 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 804 */	NdrFcShort( 0x8 ),	/* 8 */
/* 806 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 808 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 816 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 818 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 820 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 824 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOutputWindow */

/* 828 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x7 ),	/* 7 */
/* 836 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 838 */	NdrFcShort( 0x8 ),	/* 8 */
/* 840 */	NdrFcShort( 0x8 ),	/* 8 */
/* 842 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 844 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hWnd */

/* 852 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 854 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 858 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 860 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAlphaBitmapParameters */

/* 864 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 866 */	NdrFcLong( 0x0 ),	/* 0 */
/* 870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 872 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x8 ),	/* 8 */
/* 878 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 880 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 882 */	NdrFcShort( 0x1 ),	/* 1 */
/* 884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 888 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 890 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 892 */	NdrFcShort( 0x166 ),	/* Type Offset=358 */

	/* Return value */

/* 894 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 896 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAlphaBitmap */

/* 900 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 902 */	NdrFcLong( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x9 ),	/* 9 */
/* 908 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 912 */	NdrFcShort( 0x8 ),	/* 8 */
/* 914 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 916 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x1 ),	/* 1 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 924 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 926 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 928 */	NdrFcShort( 0x166 ),	/* Type Offset=358 */

	/* Return value */

/* 930 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 932 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpdateAlphaBitmapParameters */

/* 936 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 938 */	NdrFcLong( 0x0 ),	/* 0 */
/* 942 */	NdrFcShort( 0xa ),	/* 10 */
/* 944 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 950 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 952 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x1 ),	/* 1 */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 960 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 964 */	NdrFcShort( 0x166 ),	/* Type Offset=358 */

	/* Return value */

/* 966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 968 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOutputRect */

/* 972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0xb ),	/* 11 */
/* 980 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 982 */	NdrFcShort( 0x34 ),	/* 52 */
/* 984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 986 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 988 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRect */

/* 996 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1000 */	NdrFcShort( 0x15c ),	/* Type Offset=348 */

	/* Return value */

/* 1002 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1004 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Capture */

/* 1008 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1010 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0xc ),	/* 12 */
/* 1016 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1022 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1024 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppCurrentImage */

/* 1032 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1034 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1036 */	NdrFcShort( 0x18a ),	/* Type Offset=394 */

	/* Return value */

/* 1038 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1040 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_AutoResolution */


	/* Procedure get_Negative */


	/* Procedure SwitchConnectorSupported */


	/* Procedure GetCurrentBitmap */

/* 1044 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1046 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1050 */	NdrFcShort( 0xd ),	/* 13 */
/* 1052 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1056 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1058 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1060 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter bSupported */


	/* Parameter handle */

/* 1068 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1070 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1074 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1076 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Repaint */

/* 1080 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1082 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0xe ),	/* 14 */
/* 1088 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1090 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1092 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1094 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1096 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1102 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hWindow */

/* 1104 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1106 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hDC */

/* 1110 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1112 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1116 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1118 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanState */

/* 1122 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1124 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1128 */	NdrFcShort( 0xf ),	/* 15 */
/* 1130 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1136 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1138 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1146 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1148 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1150 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1154 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_ScanState */

/* 1158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1164 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1166 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1172 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1174 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1180 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 1182 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1186 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1188 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1190 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BkColor */

/* 1194 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1196 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1200 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1202 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1208 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1210 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1216 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1218 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1220 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1222 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Return value */

/* 1224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1226 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_BkColor */

/* 1230 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1236 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1238 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1240 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1244 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1246 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 1254 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1256 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1258 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Return value */

/* 1260 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1262 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOutputRect */

/* 1266 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1272 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1274 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1278 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1280 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1282 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRect */

/* 1290 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1292 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1294 */	NdrFcShort( 0x15c ),	/* Type Offset=348 */

	/* Return value */

/* 1296 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1298 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeArrive */

/* 1302 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1304 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1310 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1312 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1314 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1316 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1318 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1324 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1326 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1328 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1330 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter reserved */

/* 1332 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1334 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1338 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1340 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1342 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerArrive */

/* 1344 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1346 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1350 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1352 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1354 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1356 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1358 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1360 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1368 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1370 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1372 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter reserved */

/* 1374 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1376 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1382 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeRemove */

/* 1386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1392 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1394 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1396 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1398 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1400 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1402 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1410 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1412 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1414 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter reserved */

/* 1416 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1418 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1422 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1424 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerRemove */

/* 1428 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1434 */	NdrFcShort( 0xa ),	/* 10 */
/* 1436 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1438 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1440 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1442 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1444 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1450 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1452 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1454 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1456 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter reserved */

/* 1458 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1464 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1466 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeStateChanged */

/* 1470 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1472 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1476 */	NdrFcShort( 0xb ),	/* 11 */
/* 1478 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1480 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1482 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1484 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1486 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1492 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1494 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1496 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1498 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter reserved */

/* 1500 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1502 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1508 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerStateChanged */

/* 1512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1518 */	NdrFcShort( 0xc ),	/* 12 */
/* 1520 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1522 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1524 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1526 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1528 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1536 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1538 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1540 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter reserved */

/* 1542 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1544 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1548 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1550 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanDepthRange */

/* 1554 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1560 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1562 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1568 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1570 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1576 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pScanDepthMin */

/* 1578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pScanDepthMax */

/* 1584 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1590 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1592 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ProbesCollection */

/* 1596 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1598 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1602 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1604 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1608 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1610 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1612 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1620 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1622 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1624 */	NdrFcShort( 0xc8 ),	/* Type Offset=200 */

	/* Return value */

/* 1626 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1628 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BeamformersCollection */

/* 1632 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1634 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1638 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1640 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1644 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1646 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1648 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1654 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1656 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1658 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1660 */	NdrFcShort( 0xc8 ),	/* Type Offset=200 */

	/* Return value */

/* 1662 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1664 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataView */

/* 1668 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1670 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1674 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1676 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1682 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1684 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1690 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUnkSource */

/* 1692 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1696 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter ppUsgDataView */

/* 1698 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1702 */	NdrFcShort( 0x1b6 ),	/* Type Offset=438 */

	/* Return value */

/* 1704 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1706 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1708 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Invalidate */

/* 1710 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1712 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0xa ),	/* 10 */
/* 1718 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1722 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1724 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1726 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1732 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1734 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1736 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeamformerNameFromCode */

/* 1740 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1742 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1746 */	NdrFcShort( 0xb ),	/* 11 */
/* 1748 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1750 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1754 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1756 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1758 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1762 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Code */

/* 1764 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1766 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Name */

/* 1770 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1772 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1774 */	NdrFcShort( 0x1d4 ),	/* Type Offset=468 */

	/* Return value */

/* 1776 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1778 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeamformerCodeFromName */

/* 1782 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1784 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1788 */	NdrFcShort( 0xc ),	/* 12 */
/* 1790 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1794 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1796 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1798 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1804 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Name */

/* 1806 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1808 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1810 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter Code */

/* 1812 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1814 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1818 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1820 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_SerialNumber */


	/* Procedure ProbeNameFromCode */

/* 1824 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1826 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1830 */	NdrFcShort( 0xd ),	/* 13 */
/* 1832 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1836 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1838 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1840 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1842 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1846 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */


	/* Parameter Code */

/* 1848 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1850 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1852 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter Name */

/* 1854 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1856 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1858 */	NdrFcShort( 0x1d4 ),	/* Type Offset=468 */

	/* Return value */


	/* Return value */

/* 1860 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1862 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1864 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProbeCodeFromName */

/* 1866 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1868 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1872 */	NdrFcShort( 0xe ),	/* 14 */
/* 1874 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1878 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1880 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1882 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1886 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1888 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Name */

/* 1890 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1892 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1894 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter Code */

/* 1896 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1898 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1904 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 1908 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1916 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1918 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1922 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1924 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lItem */

/* 1932 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppUnk */

/* 1938 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1940 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1942 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Return value */

/* 1944 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1946 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FileName */


	/* Procedure get_Name */


	/* Procedure get_Name */

/* 1950 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1952 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1956 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1958 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1962 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1964 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1966 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1968 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1972 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 1974 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1976 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1978 */	NdrFcShort( 0x1d4 ),	/* Type Offset=468 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 1980 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1982 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1984 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Probes */


	/* Procedure get_Beamformer */

/* 1986 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1988 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1992 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1994 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1998 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2000 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2002 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2008 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2010 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2012 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2014 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Return value */


	/* Return value */

/* 2016 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2018 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DisplayName */


	/* Procedure get_DispalyName */

/* 2022 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2024 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2028 */	NdrFcShort( 0xa ),	/* 10 */
/* 2030 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2034 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2036 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2038 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2040 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2044 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2046 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2048 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2050 */	NdrFcShort( 0x1d4 ),	/* Type Offset=468 */

	/* Return value */


	/* Return value */

/* 2052 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2054 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_AutoProbeCenter */


	/* Procedure get_Gamma */


	/* Procedure get_Code */

/* 2058 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2060 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2064 */	NdrFcShort( 0xb ),	/* 11 */
/* 2066 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2070 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2072 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2074 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2080 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 2082 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2084 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 2088 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2090 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Type */

/* 2094 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2096 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0xc ),	/* 12 */
/* 2102 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2106 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2108 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2110 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2116 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 2118 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2120 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2122 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 2124 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2126 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDevice */

/* 2130 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2132 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2136 */	NdrFcShort( 0xb ),	/* 11 */
/* 2138 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2142 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2144 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2146 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2152 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2154 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2156 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SwitchConnector */

/* 2160 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2162 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2166 */	NdrFcShort( 0xc ),	/* 12 */
/* 2168 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2172 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2174 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2176 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2184 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2186 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CtlPointsCount */


	/* Procedure get_Code */

/* 2190 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2192 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2196 */	NdrFcShort( 0xe ),	/* 14 */
/* 2198 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2202 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2204 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2206 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2212 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2214 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2216 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2220 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2222 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 2226 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2228 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2234 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2238 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2240 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2242 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2248 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lItem */

/* 2250 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2252 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pScanMode */

/* 2256 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2258 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2262 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2264 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2266 */	0x8,		/* FC_LONG */
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

/* 2268 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2270 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2274 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2276 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2280 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2282 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2284 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2290 */	NdrFcShort( 0x0 ),	/* 0 */

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

/* 2292 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2294 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2296 */	NdrFcShort( 0x1f4 ),	/* Type Offset=500 */

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

/* 2298 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2300 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 2304 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2306 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2310 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2312 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2316 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2318 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2320 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2322 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2326 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lItem */

/* 2328 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2330 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 2334 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 2336 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2338 */	NdrFcShort( 0x5e4 ),	/* Type Offset=1508 */

	/* Return value */

/* 2340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2342 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TransparencyMode */


	/* Procedure put_Mute */


	/* Procedure put_Enabled */


	/* Procedure put_PriorityMode */


	/* Procedure put_SweepSpeed */


	/* Procedure put_Enabled */


	/* Procedure put_Enabled */

/* 2346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2352 */	NdrFcShort( 0xb ),	/* 11 */
/* 2354 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2356 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2358 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2360 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2362 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2368 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */

/* 2370 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2372 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2376 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2378 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VisibleMin */


	/* Procedure get_VisibleMin */


	/* Procedure PositionToDepth */


	/* Procedure get_FocusSet */

/* 2382 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2384 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2388 */	NdrFcShort( 0xa ),	/* 10 */
/* 2390 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2394 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2396 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2398 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter units */


	/* Parameter units */


	/* Parameter pos */


	/* Parameter mode */

/* 2406 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter depth */


	/* Parameter pVal */

/* 2412 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2414 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2420 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FocusSet */

/* 2424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2430 */	NdrFcShort( 0xb ),	/* 11 */
/* 2432 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2434 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2436 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2438 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2440 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2448 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2450 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter newVal */

/* 2454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2456 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2460 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2462 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */


	/* Procedure get_FocusSetCount */

/* 2466 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2468 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2472 */	NdrFcShort( 0xc ),	/* 12 */
/* 2474 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2476 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2478 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2480 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2482 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2488 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctlPoint */


	/* Parameter mode */

/* 2490 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2492 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2496 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2498 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2502 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2504 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2506 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocalZonesCount */

/* 2508 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2510 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2514 */	NdrFcShort( 0xd ),	/* 13 */
/* 2516 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2518 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2520 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2522 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2524 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2540 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocalZones */

/* 2544 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2550 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2552 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocalZone */

/* 2556 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2558 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2562 */	NdrFcShort( 0xe ),	/* 14 */
/* 2564 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2566 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2568 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2570 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 2572 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2578 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2580 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2582 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2586 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2588 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter zoneIndex */

/* 2592 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2594 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocalZone */

/* 2598 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2600 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2602 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */

/* 2604 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2606 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2608 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocusPoint */

/* 2610 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2612 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2616 */	NdrFcShort( 0xf ),	/* 15 */
/* 2618 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2620 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2622 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2624 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 2626 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2632 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2634 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2636 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2640 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2642 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2644 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter zoneIndex */

/* 2646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2648 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusPoint */

/* 2652 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2654 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2658 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2660 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2662 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocusState */

/* 2664 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2666 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2670 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2672 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2674 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2676 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2678 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 2680 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 2682 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2686 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2688 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2690 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2694 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2696 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2700 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2702 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusState */

/* 2706 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2708 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2710 */	NdrFcShort( 0x5fe ),	/* Type Offset=1534 */

	/* Return value */

/* 2712 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2714 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFocusState */

/* 2718 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2720 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2724 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2726 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2728 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2732 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2734 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 2736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2738 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2740 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2742 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2744 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2748 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2750 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2754 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2756 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusState */

/* 2760 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2762 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2764 */	NdrFcShort( 0x5fe ),	/* Type Offset=1534 */

	/* Return value */

/* 2766 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2768 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTgcEntries */

/* 2772 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2774 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2778 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2780 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2782 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2784 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2786 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2788 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2794 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 2796 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2798 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2800 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2802 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2804 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2806 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTgcData */

/* 2808 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2810 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2812 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2814 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2816 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2818 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTgcEntries */

/* 2820 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2822 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2826 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2828 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2830 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2832 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2834 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2836 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2842 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 2844 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2846 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2850 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2852 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTgcData */

/* 2856 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 2858 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2864 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryLifePercent */


	/* Procedure get_OutputPin */


	/* Procedure get_FramesDroppedInRenderer */


	/* Procedure get_Contrast */


	/* Procedure GetTgcSize */

/* 2868 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2874 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2876 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2880 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2882 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2884 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2890 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 2892 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2894 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2896 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2898 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2900 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VisibleMax */


	/* Procedure get_VisibleMax */


	/* Procedure DepthToPosition */

/* 2904 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2906 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2910 */	NdrFcShort( 0xb ),	/* 11 */
/* 2912 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2916 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2918 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2920 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2926 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter units */


	/* Parameter units */


	/* Parameter depth */

/* 2928 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2930 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pos */

/* 2934 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2936 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 2940 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2942 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 2946 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2948 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2952 */	NdrFcShort( 0xd ),	/* 13 */
/* 2954 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2956 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2958 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2960 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2962 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 2964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2966 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2968 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctlPoint */

/* 2970 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2972 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter newVal */

/* 2976 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2978 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2982 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2984 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CtlPointsCount */

/* 2988 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2990 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2994 */	NdrFcShort( 0xf ),	/* 15 */
/* 2996 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2998 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3000 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3002 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3004 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3010 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 3012 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3014 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3016 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3018 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3020 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CtlPointPos */

/* 3024 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3026 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3030 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3032 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3034 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3036 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3038 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3040 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3046 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctlPoint */

/* 3048 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3050 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 3054 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3056 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3060 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3062 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ValuesTgc */

/* 3066 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3068 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3072 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3074 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3078 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3080 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3082 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3088 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3090 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3092 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3094 */	NdrFcShort( 0x1f4 ),	/* Type Offset=500 */

	/* Return value */

/* 3096 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3098 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_AutoProbeCenter */


	/* Procedure put_Gamma */

/* 3102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3108 */	NdrFcShort( 0xc ),	/* 12 */
/* 3110 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3116 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3118 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 3126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_AutoResolution */


	/* Procedure put_Negative */

/* 3138 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3144 */	NdrFcShort( 0xe ),	/* 14 */
/* 3146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3152 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3154 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3160 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 3162 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3164 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3170 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BrightnessMin */

/* 3174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3180 */	NdrFcShort( 0xf ),	/* 15 */
/* 3182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3186 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3188 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3190 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3196 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3198 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3200 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3204 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3206 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3208 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BrightnessMax */

/* 3210 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3212 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3216 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3218 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3222 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3224 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3226 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3232 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3234 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3236 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3240 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3242 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ContrastMin */

/* 3246 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3248 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3252 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3254 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3258 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3260 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3262 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3268 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3270 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3272 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3276 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3278 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ContrastMax */

/* 3282 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3288 */	NdrFcShort( 0x12 ),	/* 18 */
/* 3290 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3294 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3296 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3298 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3304 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3306 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3308 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3312 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3314 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCapabilities */


	/* Procedure get_GammaMin */

/* 3318 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3324 */	NdrFcShort( 0x13 ),	/* 19 */
/* 3326 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3330 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3332 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3334 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3340 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCaps */


	/* Parameter pVal */

/* 3342 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3344 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3350 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_GammaMax */

/* 3354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3360 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3362 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3366 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3368 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3370 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3376 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3378 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetReferenceData */

/* 3390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3396 */	NdrFcShort( 0x15 ),	/* 21 */
/* 3398 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3400 */	NdrFcShort( 0x2c ),	/* 44 */
/* 3402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3404 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3406 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3412 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nValueMax */

/* 3414 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3416 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nValuesCount */

/* 3420 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3422 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3424 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 3426 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 3428 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3432 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3434 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetReferenceData */

/* 3438 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3440 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3444 */	NdrFcShort( 0x16 ),	/* 22 */
/* 3446 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3448 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3450 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3452 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3454 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3460 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pValueMax */

/* 3462 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3464 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValuesCount */

/* 3468 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3470 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 3474 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3476 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3482 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Calculate */

/* 3486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3492 */	NdrFcShort( 0x17 ),	/* 23 */
/* 3494 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3496 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3498 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3500 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3502 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 3510 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3512 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 3516 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3518 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 3522 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3524 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3530 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPaletteEntries */

/* 3534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3540 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3542 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3544 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3546 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3548 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3550 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 3558 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 3564 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPalEntries */

/* 3570 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3574 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Return value */

/* 3576 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3578 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPaletteEntries */


	/* Procedure GetPaletteEntries */

/* 3582 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3584 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3588 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3590 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3592 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3594 */	NdrFcShort( 0x30 ),	/* 48 */
/* 3596 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3598 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3604 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */


	/* Parameter startIndex */

/* 3606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */


	/* Parameter entries */

/* 3612 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPalEntries */


	/* Parameter pPalEntries */

/* 3618 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3620 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3622 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Return value */


	/* Return value */

/* 3624 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3626 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOrigin */

/* 3630 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3632 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3636 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3638 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3640 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3642 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3644 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3646 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3652 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter point */

/* 3654 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3656 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3658 */	NdrFcShort( 0x55c ),	/* Type Offset=1372 */

	/* Parameter pixelsOrigin */

/* 3660 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3662 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3664 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 3666 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3668 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3674 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOrigin */

/* 3678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3686 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3688 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3692 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3694 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3700 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter point */

/* 3702 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3704 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3706 */	NdrFcShort( 0x55c ),	/* Type Offset=1372 */

	/* Parameter pixelsOrigin */

/* 3708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3710 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3712 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3716 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetResolution */

/* 3720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3726 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3728 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3732 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3734 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3736 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter resolution */

/* 3744 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3746 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3748 */	NdrFcShort( 0x55c ),	/* Type Offset=1372 */

	/* Parameter fPrefered */

/* 3750 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3756 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3758 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetResolution */

/* 3762 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3764 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3768 */	NdrFcShort( 0xa ),	/* 10 */
/* 3770 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3772 */	NdrFcShort( 0x2c ),	/* 44 */
/* 3774 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3776 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3778 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3784 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter resolution */

/* 3786 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3788 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3790 */	NdrFcShort( 0x55c ),	/* Type Offset=1372 */

	/* Return value */

/* 3792 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3794 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PixelsToUnits */

/* 3798 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3800 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3804 */	NdrFcShort( 0xf ),	/* 15 */
/* 3806 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3808 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3810 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3812 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 3814 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3820 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Xpix */

/* 3822 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3824 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3826 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Ypix */

/* 3828 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3830 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3832 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Xunit */

/* 3834 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3836 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3838 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Yunit */

/* 3840 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3842 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3844 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pixOrigin */

/* 3846 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3848 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3850 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3854 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnitsToPixels */

/* 3858 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3864 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3866 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3868 */	NdrFcShort( 0x18 ),	/* 24 */
/* 3870 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3872 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 3874 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3880 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Xunit */

/* 3882 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3884 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3886 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Yunit */

/* 3888 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3890 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3892 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter XPix */

/* 3894 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3896 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3898 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Ypix */

/* 3900 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3902 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3904 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pixOrigin */

/* 3906 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3908 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3910 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3912 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3914 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetViewRect */

/* 3918 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3920 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3924 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3926 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3928 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3932 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 3934 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3940 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter left */

/* 3942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3944 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3946 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter top */

/* 3948 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3950 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3952 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter right */

/* 3954 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3956 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3958 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 3960 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3962 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3964 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 3966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3968 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetViewRect */

/* 3972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3978 */	NdrFcShort( 0x12 ),	/* 18 */
/* 3980 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3982 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3984 */	NdrFcShort( 0x78 ),	/* 120 */
/* 3986 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 3988 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 3990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3994 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter left */

/* 3996 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4000 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter top */

/* 4002 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4004 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4006 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter right */

/* 4008 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4010 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4012 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 4014 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4016 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4018 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 4020 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4022 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4026 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4028 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetZoom */

/* 4032 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4034 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4038 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4040 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4042 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4044 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4046 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4048 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4050 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4054 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ZoomCenterX */

/* 4056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4058 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4060 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomCenterY */

/* 4062 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4064 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4066 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomFactor */

/* 4068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4070 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4074 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4076 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetZoom */

/* 4080 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4082 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4086 */	NdrFcShort( 0x14 ),	/* 20 */
/* 4088 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4090 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4092 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4094 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4096 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4102 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ZoomCenterX */

/* 4104 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4106 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4108 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZooomCenterY */

/* 4110 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4112 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4114 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomFactor */

/* 4116 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4118 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 4122 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4124 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4130 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */


	/* Procedure ControlChanged */

/* 4134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4140 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4142 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4144 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4148 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 4150 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4156 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter dispId */

/* 4158 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4160 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter flags */

/* 4164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4166 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4172 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnControlChanged */

/* 4176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4182 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4184 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4186 */	NdrFcShort( 0x64 ),	/* 100 */
/* 4188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4190 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 4192 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4198 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riidCtrl */

/* 4200 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4202 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4204 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 4206 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4208 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 4212 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4214 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pControlObj */

/* 4218 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4220 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4222 */	NdrFcShort( 0x61a ),	/* Type Offset=1562 */

	/* Parameter dispId */

/* 4224 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4226 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter flags */

/* 4230 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4232 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4236 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4238 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnControlChangedBSTR */

/* 4242 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4250 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4252 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4256 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 4258 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4262 */	NdrFcShort( 0x1 ),	/* 1 */
/* 4264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctrlGUID */

/* 4266 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 4268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4270 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter scanMode */

/* 4272 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4274 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 4278 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4280 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pControlObject */

/* 4284 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4286 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4288 */	NdrFcShort( 0x61a ),	/* Type Offset=1562 */

	/* Parameter dispId */

/* 4290 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4292 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter flags */

/* 4296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4298 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4304 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 4308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4310 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4314 */	NdrFcShort( 0xa ),	/* 10 */
/* 4316 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4318 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4320 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4324 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4330 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter x */

/* 4332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4334 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4336 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter y */

/* 4338 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4340 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4342 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4344 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4346 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4350 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4352 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 4356 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4358 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4362 */	NdrFcShort( 0xb ),	/* 11 */
/* 4364 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4368 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4370 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4372 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4378 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4380 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4382 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4386 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4388 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4390 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter x */

/* 4392 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4394 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4396 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter y */

/* 4398 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4400 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4402 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4404 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4406 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 4410 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4412 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4416 */	NdrFcShort( 0xc ),	/* 12 */
/* 4418 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 4420 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4422 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4424 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x8,		/* 8 */
/* 4426 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4432 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4434 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4436 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4440 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4442 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4444 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4446 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4448 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4450 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4452 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4454 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4456 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 4458 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4460 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4462 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4464 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4466 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4468 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4470 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4472 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4474 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4476 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4478 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4480 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 4482 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4488 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4490 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4492 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4494 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4496 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4498 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4504 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4506 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4508 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4512 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4514 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4516 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 4518 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4520 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4522 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4524 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4526 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4528 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4530 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4532 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 4536 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4542 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4544 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 4546 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4548 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4550 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x8,		/* 8 */
/* 4552 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4558 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4560 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4562 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4566 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4568 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4570 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4572 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4574 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4576 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4580 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4582 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 4584 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4586 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4588 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4590 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4592 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4594 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4596 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4598 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4600 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4602 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4604 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 4608 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4610 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4614 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4616 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4618 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4620 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4622 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4624 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4630 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter X */

/* 4632 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4634 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4636 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4638 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4640 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4642 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4644 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4646 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4650 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4652 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLinesRange */

/* 4656 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4662 */	NdrFcShort( 0xa ),	/* 10 */
/* 4664 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4668 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4670 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 4672 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4678 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */

/* 4680 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4682 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 4686 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4688 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4692 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4694 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointDepth */

/* 4698 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4700 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4704 */	NdrFcShort( 0xb ),	/* 11 */
/* 4706 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4708 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4710 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4712 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4714 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4720 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter X */

/* 4722 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4724 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4726 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4728 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4730 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4732 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4734 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4736 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4738 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4740 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4742 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryChargeStatus */


	/* Procedure get_Current */

/* 4746 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4748 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4752 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4754 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4758 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4760 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 4762 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4768 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 4770 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4772 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4774 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4776 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4778 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 4782 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4784 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4788 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4790 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4792 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4794 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4796 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 4798 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4804 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 4806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4808 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4810 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 4812 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4814 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryFullLifetime */


	/* Procedure get_InputPin */


	/* Procedure get_FramesDrawn */

/* 4818 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4820 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4826 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4830 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4832 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 4834 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4840 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 4842 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4844 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 4848 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4850 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4852 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Preset */

/* 4854 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4856 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4860 */	NdrFcShort( 0xc ),	/* 12 */
/* 4862 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4864 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4868 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 4870 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 4872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4874 */	NdrFcShort( 0x1 ),	/* 1 */
/* 4876 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cEntries */

/* 4878 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4880 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValues */

/* 4884 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 4886 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4888 */	NdrFcShort( 0x630 ),	/* Type Offset=1584 */

	/* Parameter iCurrent */

/* 4890 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4892 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SweepTime */

/* 4896 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4898 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4904 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetColorMap */

/* 4908 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4914 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4916 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4918 */	NdrFcShort( 0x60 ),	/* 96 */
/* 4920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4922 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4924 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTable1 */

/* 4932 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4936 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Parameter size1 */

/* 4938 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4940 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTable2 */

/* 4944 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4946 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4948 */	NdrFcShort( 0x1a4 ),	/* Type Offset=420 */

	/* Parameter size2 */

/* 4950 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4952 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4954 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4958 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWindow */


	/* Procedure SetWindow */

/* 4962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4968 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4970 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4972 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4974 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4976 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4978 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 4980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4984 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */

/* 4986 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4988 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4990 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */

/* 4992 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4994 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */

/* 4998 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5000 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5002 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5004 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5006 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5010 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5012 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindow */


	/* Procedure GetWindow */

/* 5016 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5018 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5022 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5024 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5028 */	NdrFcShort( 0x78 ),	/* 120 */
/* 5030 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5032 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5038 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */

/* 5040 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5042 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5044 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */

/* 5046 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5048 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5052 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5054 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5058 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5060 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5064 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5066 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5068 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWidth */


	/* Procedure SetWidth */

/* 5070 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5072 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5076 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5078 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5080 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5082 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5084 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5086 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5092 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */

/* 5094 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5096 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */

/* 5100 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5102 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5108 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */


	/* Procedure SetVolume */

/* 5112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5118 */	NdrFcShort( 0xa ),	/* 10 */
/* 5120 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5122 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5126 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5128 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5134 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5136 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5138 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5142 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5146 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5148 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5150 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWidth */


	/* Procedure GetWidth */

/* 5154 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5156 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5160 */	NdrFcShort( 0xb ),	/* 11 */
/* 5162 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5166 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5168 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5170 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */

/* 5178 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5180 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */

/* 5184 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5186 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5192 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */


	/* Procedure GetPhantomVolume */


	/* Procedure GetVolume */

/* 5196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5202 */	NdrFcShort( 0xc ),	/* 12 */
/* 5204 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5208 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5210 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5212 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5218 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5220 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5222 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5226 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5228 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5232 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5234 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_LinesRange */


	/* Procedure get_LinesRange */

/* 5238 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5240 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5244 */	NdrFcShort( 0xd ),	/* 13 */
/* 5246 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5250 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5252 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5254 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5260 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5262 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5264 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5266 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */


	/* Return value */

/* 5268 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5270 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_WidthRange */


	/* Procedure get_WidthRange */

/* 5274 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5276 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5280 */	NdrFcShort( 0xe ),	/* 14 */
/* 5282 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5286 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5288 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5290 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5296 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5298 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5300 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5302 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */


	/* Return value */

/* 5304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5306 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */


	/* Procedure get_DepthsRange */

/* 5310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5316 */	NdrFcShort( 0xf ),	/* 15 */
/* 5318 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5322 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5324 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5326 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5332 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5334 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5336 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5338 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */


	/* Return value */

/* 5340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5342 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */


	/* Procedure get_VolumesRange */

/* 5346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5352 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5354 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5358 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5360 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5362 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5368 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5370 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5372 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5374 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */


	/* Return value */

/* 5376 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5378 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CurrentPosition */


	/* Procedure get_CurrentPosition */

/* 5382 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5384 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5388 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5390 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5394 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5396 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5398 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5406 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5410 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5414 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CurrentPosition */


	/* Procedure put_CurrentPosition */

/* 5418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5424 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5426 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5428 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5432 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5434 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5440 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 5442 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5444 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5446 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5448 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5450 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopPosition */


	/* Procedure get_Duration */

/* 5454 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5460 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5462 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5466 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5468 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5470 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5476 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5478 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5480 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5482 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5484 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5486 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 5490 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5492 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5496 */	NdrFcShort( 0xa ),	/* 10 */
/* 5498 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5502 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5504 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5506 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5512 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 5514 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5516 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5518 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5520 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5522 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 5526 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5528 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5532 */	NdrFcShort( 0xb ),	/* 11 */
/* 5534 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5540 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5542 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5548 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 5550 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5552 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5554 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5558 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopTime */

/* 5562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5568 */	NdrFcShort( 0xc ),	/* 12 */
/* 5570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5574 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5576 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5578 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5584 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 5586 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5588 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5590 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5592 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5594 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopTime */

/* 5598 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5600 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5604 */	NdrFcShort( 0xd ),	/* 13 */
/* 5606 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5608 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5610 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5612 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5614 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5620 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 5622 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5624 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5626 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5630 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_State */


	/* Procedure get_State */

/* 5634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5640 */	NdrFcShort( 0xe ),	/* 14 */
/* 5642 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5646 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5648 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5650 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5656 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5658 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5660 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5662 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5664 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5666 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_State */


	/* Procedure put_State */

/* 5670 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5672 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5676 */	NdrFcShort( 0xf ),	/* 15 */
/* 5678 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5682 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5684 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5686 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5692 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 5694 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5696 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5698 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5700 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5702 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 5706 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5708 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5712 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5714 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5718 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5720 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5722 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5728 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStart */

/* 5730 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5732 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 5736 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5738 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5742 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5744 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPositions */

/* 5748 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5750 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5754 */	NdrFcShort( 0x11 ),	/* 17 */
/* 5756 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5758 */	NdrFcShort( 0x1c ),	/* 28 */
/* 5760 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5762 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5764 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5768 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5770 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pPos */

/* 5772 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 5774 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 5778 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5780 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5784 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5786 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPositions */

/* 5790 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5796 */	NdrFcShort( 0x12 ),	/* 18 */
/* 5798 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5800 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5804 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5806 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5812 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter llStart */

/* 5814 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5816 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5818 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter llStop */

/* 5820 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5822 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5824 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5826 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5828 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CheckCapabilities */

/* 5832 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5838 */	NdrFcShort( 0x14 ),	/* 20 */
/* 5840 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5842 */	NdrFcShort( 0x1c ),	/* 28 */
/* 5844 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5846 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5848 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5854 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCaps */

/* 5856 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 5858 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5864 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopPosition */

/* 5868 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5874 */	NdrFcShort( 0xa ),	/* 10 */
/* 5876 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5878 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5882 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5884 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5890 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 5892 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5894 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5896 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5898 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5900 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Duration */

/* 5904 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5906 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5910 */	NdrFcShort( 0xb ),	/* 11 */
/* 5912 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5916 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5918 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5920 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5926 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 5928 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5930 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5932 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5934 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5936 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 5940 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5942 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5946 */	NdrFcShort( 0xc ),	/* 12 */
/* 5948 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5952 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5954 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5956 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5962 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 5964 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5966 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5968 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5970 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5972 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 5976 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5978 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5982 */	NdrFcShort( 0xd ),	/* 13 */
/* 5984 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5988 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5990 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5992 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 5994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5998 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6000 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6002 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6004 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6006 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6008 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6010 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_TimeFormat */

/* 6012 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6014 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6018 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6020 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6024 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6026 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6028 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6034 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6036 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6038 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6040 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6042 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6044 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TimeFormat */

/* 6048 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6050 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6054 */	NdrFcShort( 0x11 ),	/* 17 */
/* 6056 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6058 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6060 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6062 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6064 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6070 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6072 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6074 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6076 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6078 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6080 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6082 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ConvertTimeFormat */

/* 6084 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6086 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6090 */	NdrFcShort( 0x12 ),	/* 18 */
/* 6092 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6094 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6096 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6098 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 6100 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter srcVal */

/* 6108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6112 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter srcFormat */

/* 6114 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6116 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6118 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstFormat */

/* 6120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6122 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6124 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstVal */

/* 6126 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6128 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6130 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6134 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 6138 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6144 */	NdrFcShort( 0x13 ),	/* 19 */
/* 6146 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6150 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6152 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6154 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6160 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lEarliest */

/* 6162 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6164 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6166 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter lLatest */

/* 6168 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6170 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6172 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6176 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetShiftPosition */

/* 6180 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6186 */	NdrFcShort( 0x14 ),	/* 20 */
/* 6188 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 6190 */	NdrFcShort( 0x28 ),	/* 40 */
/* 6192 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6194 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 6196 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6202 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6208 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter shift */

/* 6210 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6212 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6214 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter timeFormat */

/* 6216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6218 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6220 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter pos */

/* 6222 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6224 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6226 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6230 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHoldRange */

/* 6234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6240 */	NdrFcShort( 0x15 ),	/* 21 */
/* 6242 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6244 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6248 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6250 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6256 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6258 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6260 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6262 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 6264 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6266 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6268 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6270 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6272 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHoldRange */

/* 6276 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6282 */	NdrFcShort( 0x16 ),	/* 22 */
/* 6284 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6288 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6290 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6292 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6298 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6300 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6302 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6304 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 6306 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6308 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6310 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6312 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6314 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTimeFormatSupported */

/* 6318 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6324 */	NdrFcShort( 0x17 ),	/* 23 */
/* 6326 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6328 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6330 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6332 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6334 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6340 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter format */

/* 6342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6344 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6346 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter support */

/* 6348 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6350 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6356 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContinuousInterval */

/* 6360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6366 */	NdrFcShort( 0x18 ),	/* 24 */
/* 6368 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6370 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6372 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6374 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6376 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6382 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter position */

/* 6384 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6388 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter start */

/* 6390 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6392 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6394 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter end */

/* 6396 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6398 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6400 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6402 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6404 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSelection */

/* 6408 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6410 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6414 */	NdrFcShort( 0x19 ),	/* 25 */
/* 6416 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6418 */	NdrFcShort( 0x28 ),	/* 40 */
/* 6420 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6422 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6424 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6430 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6434 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6436 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 6438 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6440 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6442 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter mode */

/* 6444 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6446 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6448 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6450 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6452 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBufferSize */

/* 6456 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6458 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6462 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6464 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6468 */	NdrFcShort( 0x5c ),	/* 92 */
/* 6470 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6472 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6478 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter total */

/* 6480 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6482 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter used */

/* 6486 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6488 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter writePos */

/* 6492 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6494 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6498 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6500 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */

/* 6504 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6506 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6512 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6518 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 6520 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6526 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 6528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6530 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetBufferSize */

/* 6534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6540 */	NdrFcShort( 0x9 ),	/* 9 */
/* 6542 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6544 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6546 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6548 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6550 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cbSize */

/* 6558 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6564 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveData */

/* 6570 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6576 */	NdrFcShort( 0xa ),	/* 10 */
/* 6578 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6584 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 6586 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6590 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6592 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter strFileName */

/* 6594 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6596 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6598 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Return value */

/* 6600 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6602 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */

/* 6606 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6608 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6612 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6614 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6618 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6620 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6622 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6628 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */

/* 6630 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6632 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 6636 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6638 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6640 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6644 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */

/* 6648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6654 */	NdrFcShort( 0x9 ),	/* 9 */
/* 6656 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6660 */	NdrFcShort( 0x38 ),	/* 56 */
/* 6662 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6664 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6670 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6672 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 6674 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6676 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */

/* 6678 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6680 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */

/* 6684 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6686 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6690 */	NdrFcShort( 0xa ),	/* 10 */
/* 6692 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6696 */	NdrFcShort( 0x38 ),	/* 56 */
/* 6698 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6700 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6706 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6708 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 6710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6712 */	NdrFcShort( 0x5f2 ),	/* Type Offset=1522 */

	/* Return value */

/* 6714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSampleVolume */

/* 6720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6726 */	NdrFcShort( 0xb ),	/* 11 */
/* 6728 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6730 */	NdrFcShort( 0x18 ),	/* 24 */
/* 6732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6734 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6736 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */

/* 6744 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6746 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6748 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 6750 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter line */

/* 6756 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6758 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6762 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6764 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanConverter */

/* 6768 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6770 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6774 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6776 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6780 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6782 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 6784 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6790 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6792 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6794 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6796 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Return value */

/* 6798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6800 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsSupported */

/* 6804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6810 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6812 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6814 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6816 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6818 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6820 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6826 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 6828 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6830 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6832 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter supported */

/* 6834 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6836 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6838 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6840 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6842 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIndex */

/* 6846 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6848 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6852 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6854 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6856 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6858 */	NdrFcShort( 0x34 ),	/* 52 */
/* 6860 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6862 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6868 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 6870 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6872 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6874 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter value */

/* 6876 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 6878 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6880 */	NdrFcShort( 0x55c ),	/* Type Offset=1372 */

	/* Return value */

/* 6882 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6884 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */

/* 6888 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6894 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6896 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6898 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6900 */	NdrFcShort( 0x34 ),	/* 52 */
/* 6902 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6904 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6910 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6912 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 6914 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6916 */	NdrFcShort( 0x644 ),	/* Type Offset=1604 */

	/* Return value */

/* 6918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6920 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 6924 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6932 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6934 */	NdrFcShort( 0x18 ),	/* 24 */
/* 6936 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6938 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6940 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 6942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6944 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6946 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6948 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 6950 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6952 */	NdrFcShort( 0x644 ),	/* Type Offset=1604 */

	/* Return value */

/* 6954 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6956 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FileName */

/* 6960 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6962 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6966 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6968 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6972 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6974 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 6976 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 6978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6980 */	NdrFcShort( 0x1 ),	/* 1 */
/* 6982 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6984 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6986 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6988 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Return value */

/* 6990 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6992 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6994 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 6996 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6998 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7002 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7004 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7008 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7010 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 7012 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7018 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 7020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7022 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataKey */

/* 7026 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7032 */	NdrFcShort( 0xb ),	/* 11 */
/* 7034 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7036 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7038 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7040 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7042 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7048 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBaseKey */

/* 7050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7052 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7054 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 7056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7058 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7060 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 7062 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7064 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7068 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7070 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDataKey */

/* 7074 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7076 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7080 */	NdrFcShort( 0xc ),	/* 12 */
/* 7082 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7084 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7086 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7088 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 7090 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7096 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter baseKey */

/* 7098 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7100 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7102 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 7104 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7106 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyIndex */

/* 7110 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7112 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 7116 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7118 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7124 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CloseDataKey */

/* 7128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7134 */	NdrFcShort( 0xd ),	/* 13 */
/* 7136 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7142 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7144 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7152 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7154 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7160 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKeyInfo */

/* 7164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7170 */	NdrFcShort( 0xe ),	/* 14 */
/* 7172 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7176 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7178 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7180 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7186 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7188 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7190 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subKeys */

/* 7194 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7196 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataValues */

/* 7200 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7202 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7208 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumValues */

/* 7212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7218 */	NdrFcShort( 0xf ),	/* 15 */
/* 7220 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7222 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7224 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7226 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 7228 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7234 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7236 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7238 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ValueNum */

/* 7242 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7244 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 7248 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7250 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueSize */

/* 7254 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7256 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7260 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7262 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumSubkeys */

/* 7266 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7272 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7274 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7276 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7278 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7280 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7282 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7288 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7290 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7292 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyNum */

/* 7296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7298 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyId */

/* 7302 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7304 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7308 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7310 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValue */

/* 7314 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7316 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7320 */	NdrFcShort( 0x11 ),	/* 17 */
/* 7322 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 7324 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7326 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7328 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 7330 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 7332 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7336 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7338 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7340 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7342 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 7344 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7346 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 7350 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7352 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cbBufferSize */

/* 7356 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7358 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7360 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 7362 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 7364 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7366 */	NdrFcShort( 0x650 ),	/* Type Offset=1616 */

	/* Parameter dataSize */

/* 7368 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7370 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7374 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7376 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 7378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValue */

/* 7380 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7382 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7386 */	NdrFcShort( 0x12 ),	/* 18 */
/* 7388 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 7390 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7394 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 7396 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 7398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7400 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7402 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7406 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 7410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7412 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 7416 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7418 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 7422 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 7424 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7426 */	NdrFcShort( 0x660 ),	/* Type Offset=1632 */

	/* Parameter dataSize */

/* 7428 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7430 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7434 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7436 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateObjectsDump */

/* 7440 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7442 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7446 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7448 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7450 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7454 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 7456 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 7458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7460 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7462 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fileName */

/* 7464 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7466 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7468 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter reserved1 */

/* 7470 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7472 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7474 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 7476 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7478 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7480 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7482 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7484 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPlugin */

/* 7488 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7490 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7494 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7496 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7500 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7502 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 7504 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7510 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pPlugin */

/* 7512 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7514 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7516 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */

	/* Parameter cookie */

/* 7518 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7520 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7524 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7526 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ScanModeCaps */

/* 7530 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7532 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7536 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7538 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7540 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7542 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7544 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 7546 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7552 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Parameter */

/* 7554 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7556 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ScanMode */

/* 7560 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7562 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Capability */

/* 7566 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7568 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Support */

/* 7572 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7574 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7578 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7580 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurrent */

/* 7584 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7586 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7590 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7592 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7596 */	NdrFcShort( 0x48 ),	/* 72 */
/* 7598 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 7600 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7606 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 7608 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7610 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7612 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */

	/* Parameter pVal */

/* 7614 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 7616 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7618 */	NdrFcShort( 0x670 ),	/* Type Offset=1648 */

	/* Return value */

/* 7620 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7622 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCurrent */

/* 7626 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7628 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7632 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7634 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7636 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7638 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7640 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 7642 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7648 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 7650 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7652 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7654 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */

	/* Parameter pVal */

/* 7656 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 7658 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7660 */	NdrFcShort( 0x670 ),	/* Type Offset=1648 */

	/* Return value */

/* 7662 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7664 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7666 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValues */

/* 7668 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7670 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7674 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7676 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7682 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 7684 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7690 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 7692 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7696 */	NdrFcShort( 0x1e2 ),	/* Type Offset=482 */

	/* Parameter pVal */

/* 7698 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7702 */	NdrFcShort( 0x1f4 ),	/* Type Offset=500 */

	/* Return value */

/* 7704 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7706 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7708 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FilterObj */

/* 7710 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7712 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7716 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7718 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7720 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7722 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7724 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 7726 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 7728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7732 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter FilterIndex */

/* 7734 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7736 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 7740 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7742 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7744 */	NdrFcShort( 0x1de ),	/* Type Offset=478 */

	/* Return value */

/* 7746 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7748 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const Usgfw2_MIDL_TYPE_FORMAT_STRING Usgfw2__MIDL_TypeFormatString =
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
/* 122 */	NdrFcShort( 0xe ),	/* Offset= 14 (136) */
/* 124 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 126 */	NdrFcShort( 0x2 ),	/* 2 */
/* 128 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 130 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 132 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 134 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 136 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 140 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (124) */
/* 142 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 144 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 146 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x4 ),	/* 4 */
/* 152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 154 */	NdrFcShort( 0xffde ),	/* Offset= -34 (120) */
/* 156 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 158 */	NdrFcShort( 0x2 ),	/* Offset= 2 (160) */
/* 160 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 162 */	NdrFcLong( 0x4cf81935 ),	/* 1291327797 */
/* 166 */	NdrFcShort( 0xdcb3 ),	/* -9037 */
/* 168 */	NdrFcShort( 0x4c19 ),	/* 19481 */
/* 170 */	0x9d,		/* 157 */
			0xc9,		/* 201 */
/* 172 */	0x76,		/* 118 */
			0x53,		/* 83 */
/* 174 */	0x6,		/* 6 */
			0xa4,		/* 164 */
/* 176 */	0x71,		/* 113 */
			0x4f,		/* 79 */
/* 178 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 180 */	NdrFcShort( 0x2 ),	/* Offset= 2 (182) */
/* 182 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 184 */	NdrFcLong( 0xb7c94539 ),	/* -1211546311 */
/* 188 */	NdrFcShort( 0xa65a ),	/* -22950 */
/* 190 */	NdrFcShort( 0x42f3 ),	/* 17139 */
/* 192 */	0x8b,		/* 139 */
			0x65,		/* 101 */
/* 194 */	0xf8,		/* 248 */
			0x3d,		/* 61 */
/* 196 */	0x11,		/* 17 */
			0x4f,		/* 79 */
/* 198 */	0xf4,		/* 244 */
			0xc8,		/* 200 */
/* 200 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 202 */	NdrFcShort( 0x2 ),	/* Offset= 2 (204) */
/* 204 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 206 */	NdrFcLong( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 216 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 218 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 220 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 222 */	
			0x11, 0x0,	/* FC_RP */
/* 224 */	NdrFcShort( 0x86 ),	/* Offset= 134 (358) */
/* 226 */	
			0x13, 0x0,	/* FC_OP */
/* 228 */	NdrFcShort( 0x2 ),	/* Offset= 2 (230) */
/* 230 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 232 */	NdrFcShort( 0x4 ),	/* 4 */
/* 234 */	NdrFcShort( 0x2 ),	/* 2 */
/* 236 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 240 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 242 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 246 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 248 */	NdrFcShort( 0xffff ),	/* Offset= -1 (247) */
/* 250 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 252 */	NdrFcShort( 0x1 ),	/* 1 */
/* 254 */	NdrFcShort( 0x4 ),	/* 4 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (226) */
/* 260 */	
			0x13, 0x0,	/* FC_OP */
/* 262 */	NdrFcShort( 0x2 ),	/* Offset= 2 (264) */
/* 264 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 266 */	NdrFcShort( 0x8 ),	/* 8 */
/* 268 */	NdrFcShort( 0x3 ),	/* 3 */
/* 270 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 274 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 276 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 280 */	NdrFcShort( 0xa ),	/* Offset= 10 (290) */
/* 282 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 286 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 288 */	NdrFcShort( 0xffff ),	/* Offset= -1 (287) */
/* 290 */	
			0x13, 0x0,	/* FC_OP */
/* 292 */	NdrFcShort( 0xe ),	/* Offset= 14 (306) */
/* 294 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 296 */	NdrFcShort( 0x1 ),	/* 1 */
/* 298 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 300 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 302 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 304 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 306 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 308 */	NdrFcShort( 0x18 ),	/* 24 */
/* 310 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (294) */
/* 312 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 314 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 316 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 318 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 320 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 322 */	NdrFcShort( 0x2 ),	/* 2 */
/* 324 */	NdrFcShort( 0x4 ),	/* 4 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0xffbc ),	/* Offset= -68 (260) */
/* 330 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 332 */	NdrFcShort( 0x18 ),	/* 24 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x0 ),	/* Offset= 0 (336) */
/* 338 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 340 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 342 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 344 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (320) */
			0x5b,		/* FC_END */
/* 348 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 350 */	NdrFcShort( 0x10 ),	/* 16 */
/* 352 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 354 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 356 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 358 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 360 */	NdrFcShort( 0x4c ),	/* 76 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* Offset= 0 (364) */
/* 366 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 368 */	0x0,		/* 0 */
			NdrFcShort( 0xff89 ),	/* Offset= -119 (250) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 372 */	0x0,		/* 0 */
			NdrFcShort( 0xffcb ),	/* Offset= -53 (320) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 376 */	0x0,		/* 0 */
			NdrFcShort( 0xffd1 ),	/* Offset= -47 (330) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 380 */	0x0,		/* 0 */
			NdrFcShort( 0xffdf ),	/* Offset= -33 (348) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 384 */	0x0,		/* 0 */
			NdrFcShort( 0xffdb ),	/* Offset= -37 (348) */
			0xa,		/* FC_FLOAT */
/* 388 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 390 */	
			0x11, 0x0,	/* FC_RP */
/* 392 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (348) */
/* 394 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 396 */	NdrFcShort( 0x2 ),	/* Offset= 2 (398) */
/* 398 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 400 */	NdrFcLong( 0x7bf80981 ),	/* 2079852929 */
/* 404 */	NdrFcShort( 0xbf32 ),	/* -16590 */
/* 406 */	NdrFcShort( 0x101a ),	/* 4122 */
/* 408 */	0x8b,		/* 139 */
			0xbb,		/* 187 */
/* 410 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 412 */	0x0,		/* 0 */
			0x30,		/* 48 */
/* 414 */	0xc,		/* 12 */
			0xab,		/* 171 */
/* 416 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 418 */	NdrFcShort( 0x2 ),	/* Offset= 2 (420) */
/* 420 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 422 */	NdrFcShort( 0x4 ),	/* 4 */
/* 424 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 426 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 428 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 430 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 432 */	NdrFcShort( 0xffac ),	/* Offset= -84 (348) */
/* 434 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 436 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 438 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 440 */	NdrFcShort( 0x2 ),	/* Offset= 2 (442) */
/* 442 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 444 */	NdrFcLong( 0xbfef8670 ),	/* -1074821520 */
/* 448 */	NdrFcShort( 0xdbc1 ),	/* -9279 */
/* 450 */	NdrFcShort( 0x4b47 ),	/* 19271 */
/* 452 */	0xbc,		/* 188 */
			0x1,		/* 1 */
/* 454 */	0xbb,		/* 187 */
			0xbc,		/* 188 */
/* 456 */	0xa1,		/* 161 */
			0x74,		/* 116 */
/* 458 */	0xdd,		/* 221 */
			0x31,		/* 49 */
/* 460 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 462 */	NdrFcShort( 0x6 ),	/* Offset= 6 (468) */
/* 464 */	
			0x13, 0x0,	/* FC_OP */
/* 466 */	NdrFcShort( 0xfeb6 ),	/* Offset= -330 (136) */
/* 468 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x4 ),	/* 4 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (464) */
/* 478 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 480 */	NdrFcShort( 0x2 ),	/* Offset= 2 (482) */
/* 482 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 494 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 496 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 498 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 500 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 502 */	NdrFcShort( 0x2 ),	/* Offset= 2 (504) */
/* 504 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 506 */	NdrFcLong( 0x95d11d2b ),	/* -1781457621 */
/* 510 */	NdrFcShort( 0xec05 ),	/* -5115 */
/* 512 */	NdrFcShort( 0x4a2e ),	/* 18990 */
/* 514 */	0xb3,		/* 179 */
			0x1b,		/* 27 */
/* 516 */	0x13,		/* 19 */
			0x86,		/* 134 */
/* 518 */	0xc4,		/* 196 */
			0x84,		/* 132 */
/* 520 */	0xae,		/* 174 */
			0x16,		/* 22 */
/* 522 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 524 */	NdrFcShort( 0x3d8 ),	/* Offset= 984 (1508) */
/* 526 */	
			0x13, 0x0,	/* FC_OP */
/* 528 */	NdrFcShort( 0x3c0 ),	/* Offset= 960 (1488) */
/* 530 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 532 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 534 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 536 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 538 */	NdrFcShort( 0x2 ),	/* Offset= 2 (540) */
/* 540 */	NdrFcShort( 0x10 ),	/* 16 */
/* 542 */	NdrFcShort( 0x2f ),	/* 47 */
/* 544 */	NdrFcLong( 0x14 ),	/* 20 */
/* 548 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 550 */	NdrFcLong( 0x3 ),	/* 3 */
/* 554 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 556 */	NdrFcLong( 0x11 ),	/* 17 */
/* 560 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 562 */	NdrFcLong( 0x2 ),	/* 2 */
/* 566 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 568 */	NdrFcLong( 0x4 ),	/* 4 */
/* 572 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 574 */	NdrFcLong( 0x5 ),	/* 5 */
/* 578 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 580 */	NdrFcLong( 0xb ),	/* 11 */
/* 584 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 586 */	NdrFcLong( 0xa ),	/* 10 */
/* 590 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 592 */	NdrFcLong( 0x6 ),	/* 6 */
/* 596 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (828) */
/* 598 */	NdrFcLong( 0x7 ),	/* 7 */
/* 602 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 604 */	NdrFcLong( 0x8 ),	/* 8 */
/* 608 */	NdrFcShort( 0xff70 ),	/* Offset= -144 (464) */
/* 610 */	NdrFcLong( 0xd ),	/* 13 */
/* 614 */	NdrFcShort( 0xff7c ),	/* Offset= -132 (482) */
/* 616 */	NdrFcLong( 0x9 ),	/* 9 */
/* 620 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (834) */
/* 622 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 626 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (852) */
/* 628 */	NdrFcLong( 0x24 ),	/* 36 */
/* 632 */	NdrFcShort( 0x30e ),	/* Offset= 782 (1414) */
/* 634 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 638 */	NdrFcShort( 0x308 ),	/* Offset= 776 (1414) */
/* 640 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 644 */	NdrFcShort( 0x306 ),	/* Offset= 774 (1418) */
/* 646 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 650 */	NdrFcShort( 0x304 ),	/* Offset= 772 (1422) */
/* 652 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 656 */	NdrFcShort( 0x302 ),	/* Offset= 770 (1426) */
/* 658 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 662 */	NdrFcShort( 0x300 ),	/* Offset= 768 (1430) */
/* 664 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 668 */	NdrFcShort( 0x2fe ),	/* Offset= 766 (1434) */
/* 670 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 674 */	NdrFcShort( 0x2fc ),	/* Offset= 764 (1438) */
/* 676 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 680 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1422) */
/* 682 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 686 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1426) */
/* 688 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 692 */	NdrFcShort( 0x2ee ),	/* Offset= 750 (1442) */
/* 694 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 698 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1438) */
/* 700 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 704 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1446) */
/* 706 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 710 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1450) */
/* 712 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 716 */	NdrFcShort( 0x2e2 ),	/* Offset= 738 (1454) */
/* 718 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 722 */	NdrFcShort( 0x2e0 ),	/* Offset= 736 (1458) */
/* 724 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 728 */	NdrFcShort( 0x2de ),	/* Offset= 734 (1462) */
/* 730 */	NdrFcLong( 0x10 ),	/* 16 */
/* 734 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 736 */	NdrFcLong( 0x12 ),	/* 18 */
/* 740 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 742 */	NdrFcLong( 0x13 ),	/* 19 */
/* 746 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 748 */	NdrFcLong( 0x15 ),	/* 21 */
/* 752 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 754 */	NdrFcLong( 0x16 ),	/* 22 */
/* 758 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 760 */	NdrFcLong( 0x17 ),	/* 23 */
/* 764 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 766 */	NdrFcLong( 0xe ),	/* 14 */
/* 770 */	NdrFcShort( 0x2bc ),	/* Offset= 700 (1470) */
/* 772 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 776 */	NdrFcShort( 0x2c0 ),	/* Offset= 704 (1480) */
/* 778 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 782 */	NdrFcShort( 0x2be ),	/* Offset= 702 (1484) */
/* 784 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 788 */	NdrFcShort( 0x27a ),	/* Offset= 634 (1422) */
/* 790 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 794 */	NdrFcShort( 0x278 ),	/* Offset= 632 (1426) */
/* 796 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 800 */	NdrFcShort( 0x276 ),	/* Offset= 630 (1430) */
/* 802 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 806 */	NdrFcShort( 0x26c ),	/* Offset= 620 (1426) */
/* 808 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 812 */	NdrFcShort( 0x266 ),	/* Offset= 614 (1426) */
/* 814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 818 */	NdrFcShort( 0x0 ),	/* Offset= 0 (818) */
/* 820 */	NdrFcLong( 0x1 ),	/* 1 */
/* 824 */	NdrFcShort( 0x0 ),	/* Offset= 0 (824) */
/* 826 */	NdrFcShort( 0xffff ),	/* Offset= -1 (825) */
/* 828 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 832 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 834 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 836 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 844 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 846 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 848 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 850 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 852 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 854 */	NdrFcShort( 0x2 ),	/* Offset= 2 (856) */
/* 856 */	
			0x13, 0x0,	/* FC_OP */
/* 858 */	NdrFcShort( 0x21a ),	/* Offset= 538 (1396) */
/* 860 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 862 */	NdrFcShort( 0x18 ),	/* 24 */
/* 864 */	NdrFcShort( 0xa ),	/* 10 */
/* 866 */	NdrFcLong( 0x8 ),	/* 8 */
/* 870 */	NdrFcShort( 0x5a ),	/* Offset= 90 (960) */
/* 872 */	NdrFcLong( 0xd ),	/* 13 */
/* 876 */	NdrFcShort( 0x7e ),	/* Offset= 126 (1002) */
/* 878 */	NdrFcLong( 0x9 ),	/* 9 */
/* 882 */	NdrFcShort( 0x9e ),	/* Offset= 158 (1040) */
/* 884 */	NdrFcLong( 0xc ),	/* 12 */
/* 888 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (1088) */
/* 890 */	NdrFcLong( 0x24 ),	/* 36 */
/* 894 */	NdrFcShort( 0x124 ),	/* Offset= 292 (1186) */
/* 896 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 900 */	NdrFcShort( 0x144 ),	/* Offset= 324 (1224) */
/* 902 */	NdrFcLong( 0x10 ),	/* 16 */
/* 906 */	NdrFcShort( 0x15e ),	/* Offset= 350 (1256) */
/* 908 */	NdrFcLong( 0x2 ),	/* 2 */
/* 912 */	NdrFcShort( 0x178 ),	/* Offset= 376 (1288) */
/* 914 */	NdrFcLong( 0x3 ),	/* 3 */
/* 918 */	NdrFcShort( 0x192 ),	/* Offset= 402 (1320) */
/* 920 */	NdrFcLong( 0x14 ),	/* 20 */
/* 924 */	NdrFcShort( 0x1ac ),	/* Offset= 428 (1352) */
/* 926 */	NdrFcShort( 0xffff ),	/* Offset= -1 (925) */
/* 928 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 930 */	NdrFcShort( 0x4 ),	/* 4 */
/* 932 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 936 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 938 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 940 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 942 */	NdrFcShort( 0x4 ),	/* 4 */
/* 944 */	NdrFcShort( 0x0 ),	/* 0 */
/* 946 */	NdrFcShort( 0x1 ),	/* 1 */
/* 948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 952 */	0x13, 0x0,	/* FC_OP */
/* 954 */	NdrFcShort( 0xfcce ),	/* Offset= -818 (136) */
/* 956 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 958 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 960 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 962 */	NdrFcShort( 0x8 ),	/* 8 */
/* 964 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 966 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 968 */	NdrFcShort( 0x4 ),	/* 4 */
/* 970 */	NdrFcShort( 0x4 ),	/* 4 */
/* 972 */	0x11, 0x0,	/* FC_RP */
/* 974 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (928) */
/* 976 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 978 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 980 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 984 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 988 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 990 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 994 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 996 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 998 */	NdrFcShort( 0xfce6 ),	/* Offset= -794 (204) */
/* 1000 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1002 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1014) */
/* 1010 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1012 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1014 */	
			0x11, 0x0,	/* FC_RP */
/* 1016 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (980) */
/* 1018 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1020 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1022 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1028 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1032 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1034 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1036 */	NdrFcShort( 0xff36 ),	/* Offset= -202 (834) */
/* 1038 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1040 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1042 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1052) */
/* 1048 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1050 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1052 */	
			0x11, 0x0,	/* FC_RP */
/* 1054 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1018) */
/* 1056 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1058 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1060 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1066 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1068 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1070 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1074 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1076 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1080 */	0x13, 0x0,	/* FC_OP */
/* 1082 */	NdrFcShort( 0x196 ),	/* Offset= 406 (1488) */
/* 1084 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1086 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1088 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1090 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1094 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1100) */
/* 1096 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1098 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1100 */	
			0x11, 0x0,	/* FC_RP */
/* 1102 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (1056) */
/* 1104 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1106 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1114 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1116 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1118 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1120 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1122 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1124 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1126 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1128 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1130 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1132 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1134 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1136 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	NdrFcShort( 0xa ),	/* Offset= 10 (1150) */
/* 1142 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1144 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1146 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (1104) */
/* 1148 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1150 */	
			0x13, 0x0,	/* FC_OP */
/* 1152 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1122) */
/* 1154 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1156 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1158 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1162 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1164 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1166 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1168 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1172 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1178 */	0x13, 0x0,	/* FC_OP */
/* 1180 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (1134) */
/* 1182 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1184 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1186 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1198) */
/* 1194 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1196 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1198 */	
			0x11, 0x0,	/* FC_RP */
/* 1200 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (1154) */
/* 1202 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1206 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1210 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1212 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1216 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1218 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1220 */	NdrFcShort( 0xfd1e ),	/* Offset= -738 (482) */
/* 1222 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1224 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1226 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0xa ),	/* Offset= 10 (1240) */
/* 1232 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1234 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1236 */	NdrFcShort( 0xfb82 ),	/* Offset= -1150 (86) */
/* 1238 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1240 */	
			0x11, 0x0,	/* FC_RP */
/* 1242 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (1202) */
/* 1244 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1246 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1248 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1254 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1256 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1260 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1262 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1264 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1268 */	0x13, 0x0,	/* FC_OP */
/* 1270 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (1244) */
/* 1272 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1274 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1276 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1278 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1280 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1284 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1286 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1288 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1292 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1294 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1296 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1298 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1300 */	0x13, 0x0,	/* FC_OP */
/* 1302 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (1276) */
/* 1304 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1306 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1308 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1310 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1312 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1316 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1318 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1320 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1324 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1326 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1328 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1330 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1332 */	0x13, 0x0,	/* FC_OP */
/* 1334 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (1308) */
/* 1336 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1338 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1340 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1344 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1348 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1350 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1352 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1356 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1358 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1360 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1362 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1364 */	0x13, 0x0,	/* FC_OP */
/* 1366 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (1340) */
/* 1368 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1370 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1372 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1374 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1376 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1378 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1380 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1384 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1386 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1388 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1390 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1392 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1372) */
/* 1394 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1396 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1398 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1400 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1380) */
/* 1402 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1402) */
/* 1404 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1406 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1408 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1410 */	NdrFcShort( 0xfdda ),	/* Offset= -550 (860) */
/* 1412 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1414 */	
			0x13, 0x0,	/* FC_OP */
/* 1416 */	NdrFcShort( 0xfee6 ),	/* Offset= -282 (1134) */
/* 1418 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1420 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1422 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1424 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1426 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1428 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1430 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1432 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1434 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1436 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1438 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1440 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1442 */	
			0x13, 0x0,	/* FC_OP */
/* 1444 */	NdrFcShort( 0xfd98 ),	/* Offset= -616 (828) */
/* 1446 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1448 */	NdrFcShort( 0xfc28 ),	/* Offset= -984 (464) */
/* 1450 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1452 */	NdrFcShort( 0xfc36 ),	/* Offset= -970 (482) */
/* 1454 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1456 */	NdrFcShort( 0xfd92 ),	/* Offset= -622 (834) */
/* 1458 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1460 */	NdrFcShort( 0xfda0 ),	/* Offset= -608 (852) */
/* 1462 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1464 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1466) */
/* 1466 */	
			0x13, 0x0,	/* FC_OP */
/* 1468 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1488) */
/* 1470 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1472 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1474 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1476 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1478 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1480 */	
			0x13, 0x0,	/* FC_OP */
/* 1482 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1470) */
/* 1484 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1486 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1488 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1490 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1494 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1494) */
/* 1496 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1498 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1500 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1502 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1504 */	NdrFcShort( 0xfc32 ),	/* Offset= -974 (530) */
/* 1506 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1508 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1510 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1512 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1516 */	NdrFcShort( 0xfc22 ),	/* Offset= -990 (526) */
/* 1518 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1520 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1522) */
/* 1522 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1524 */	NdrFcShort( 0xc ),	/* 12 */
/* 1526 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1528 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1530 */	
			0x11, 0x0,	/* FC_RP */
/* 1532 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1534) */
/* 1534 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1536 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1538 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1540 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1542 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1544 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1546 */	
			0x11, 0x0,	/* FC_RP */
/* 1548 */	NdrFcShort( 0xfb98 ),	/* Offset= -1128 (420) */
/* 1550 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1552 */	NdrFcShort( 0xff4c ),	/* Offset= -180 (1372) */
/* 1554 */	
			0x11, 0x0,	/* FC_RP */
/* 1556 */	NdrFcShort( 0xff48 ),	/* Offset= -184 (1372) */
/* 1558 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1560 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1562 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1564 */	NdrFcLong( 0x5748ca80 ),	/* 1464388224 */
/* 1568 */	NdrFcShort( 0x1710 ),	/* 5904 */
/* 1570 */	NdrFcShort( 0x489f ),	/* 18591 */
/* 1572 */	0xbc,		/* 188 */
			0x13,		/* 19 */
/* 1574 */	0x28,		/* 40 */
			0xf2,		/* 242 */
/* 1576 */	0xc0,		/* 192 */
			0x12,		/* 18 */
/* 1578 */	0x2b,		/* 43 */
			0x49,		/* 73 */
/* 1580 */	
			0x11, 0x0,	/* FC_RP */
/* 1582 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1584) */
/* 1584 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1586 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1588 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1590 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1592 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1594 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1596 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1598 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1600 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1602 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1604) */
/* 1604 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1606 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1608 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 1610 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1612 */	
			0x11, 0x0,	/* FC_RP */
/* 1614 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1616) */
/* 1616 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1618 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1620 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1622 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1624 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1626 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1628 */	
			0x11, 0x0,	/* FC_RP */
/* 1630 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1632) */
/* 1632 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1634 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1636 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1638 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1640 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1642 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1644 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1646 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1648) */
/* 1648 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1650 */	NdrFcShort( 0xc ),	/* 12 */
/* 1652 */	0xe,		/* FC_ENUM32 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1654 */	0x0,		/* 0 */
			NdrFcShort( 0xfee5 ),	/* Offset= -283 (1372) */
			0x5b,		/* FC_END */
/* 1658 */	
			0x11, 0x0,	/* FC_RP */
/* 1660 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1648) */

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



/* Standard interface: __MIDL_itf_Usgfw2_0000_0000, ver. 0.0,
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
    36,
    72,
    108,
    144,
    180,
    222,
    258,
    312
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataView_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataView_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    360,
    408,
    462,
    498,
    534,
    570,
    606
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanMode_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanMode_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    642,
    684,
    720,
    756,
    792
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataStream_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    828,
    864,
    900,
    936,
    972,
    1008,
    1044,
    1080,
    1122,
    1158,
    1194,
    1230,
    1266
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgMixerControl_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgMixerControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgMixerControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgData_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgData_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgGraph_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgGraph_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1302,
    1344,
    1386,
    1428,
    1470,
    1512
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceChangeSink_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDeviceChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    1554
    };

static const MIDL_STUBLESS_PROXY_INFO IScanDepth_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IScanDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScanDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1596,
    1632,
    1668,
    1710,
    1740,
    1782,
    1824,
    1866,
    312
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgfw2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    Usgfw2__MIDL_ProcFormatString.Format,
    &IProbesCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbesCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1908
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCollection_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1950,
    1986,
    72,
    2022,
    2058
    };

static const MIDL_STUBLESS_PROXY_INFO IProbe_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IProbe_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbe_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1950,
    1986,
    72,
    2022,
    2058,
    2094,
    1824
    };

static const MIDL_STUBLESS_PROXY_INFO IProbe2_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IProbe2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbe2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1950,
    1986,
    72,
    2022,
    2130,
    2160,
    1044,
    2190
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformer_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IBeamformer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBeamformer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    2226
    };

static const MIDL_STUBLESS_PROXY_INFO IScanModes_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IScanModes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScanModes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgGain_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgGain_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgGain_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    2304
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgValues_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgValues_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgValues_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPower_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgPower_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPower_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDynamicRange_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDynamicRange_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDynamicRange_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFrameAvg_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFrameAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgRejection2_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgRejection2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgRejection2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency2_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDepth_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    72,
    108
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageOrientation_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageOrientation_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageOrientation_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageEnhancement_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageEnhancement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgViewArea_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewArea_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgViewArea_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgLineDensity_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgLineDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgLineDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    2382,
    2424,
    2466,
    2508,
    2556,
    2610,
    2664,
    2718
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFocus_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgFocus_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFocus_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    2772,
    2820,
    2868,
    2382,
    2904,
    2466,
    2946,
    2190,
    2988,
    3024,
    3066
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgTgc_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgTgc_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgTgc_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgClearView_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgClearView_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgClearView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2868,
    498,
    2058,
    3102,
    1044,
    3138,
    3174,
    3210,
    3246,
    3282,
    3318,
    3354,
    3390,
    3438,
    3486
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPaletteCalculator_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPaletteCalculator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    3534,
    3582
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPalette_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgPalette_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPalette_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    3630,
    3678,
    3720,
    3762,
    2058,
    3102,
    1044,
    3138,
    3798,
    3858,
    3918,
    3972,
    4032,
    4080
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageProperties_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4134
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgControlChangeSink_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgControlChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4176,
    4242
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCtrlChangeCommon_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCtrlChangeCommon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    4308,
    4356,
    4410
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLine_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4482,
    4536,
    4608,
    4656,
    4698
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineSelector_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineSelector_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4746,
    4782,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepMode_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepMode_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSweepMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4818,
    2868
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgQualProp_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgQualProp_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgQualProp_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346,
    4854
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepSpeed_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSweepSpeed_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4908,
    3582
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorMap_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorMap_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorPriority_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorPriority_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSteerAngle_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSteerAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorThreshold_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorThreshold_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerBaseLine_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerBaseLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    2382,
    2904
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPRF_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerPRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4962,
    5016,
    5070,
    5112,
    5154,
    5196,
    5238,
    5274,
    5310,
    5346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWindow_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    2382,
    2904
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWallFilter_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerWallFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalScale_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalScale_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPacketSize_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerPacketSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPulsesNumber_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPulsesNumber_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    5382,
    5418,
    5454,
    5490,
    5526,
    5562,
    5598,
    5634,
    5670,
    5706,
    5748,
    5790,
    3318,
    5832
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineloop_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineloop_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineloop_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    5382,
    5418,
    5454,
    5868,
    5904,
    5940,
    5976,
    5634,
    5670,
    6012,
    6048,
    6084,
    6138,
    6180,
    6234,
    6276,
    6318,
    6360,
    6408
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineStream_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    6456,
    6504,
    6534,
    6570
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineSink_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSpatialFilter_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSpatialFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4134,
    6606,
    6648,
    6684,
    6720,
    5196
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSampleVolume_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSampleVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerCorrectionAngle_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerCorrectionAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    6768
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    6768,
    4818,
    2868
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin2_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalSmooth_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalSmooth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgAudioVolume_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgAudioVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgAudioVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSpectralAvg_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSpectralAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    6804,
    6846
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgBioeffectsIndices_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgBioeffectsIndices_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    6888,
    6924,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency3_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency3_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorTransparency_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorTransparency_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeSize_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsg3dVolumeSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeDensity_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsg3dVolumeDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    1950,
    6960,
    6996,
    498,
    7026,
    7074,
    7128,
    7164,
    7212,
    7266,
    7314,
    7380
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFileStorage_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgFileStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFileStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    7440
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2Debug_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2Debug_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgfw2Debug_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    7488,
    36
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    4746,
    4818,
    2868,
    720,
    756
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformerPowerState_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IBeamformerPowerState_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBeamformerPowerState_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanType_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanType_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanType_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSteeringAngle_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSteeringAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgViewAngle_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgViewAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCompoundFrames_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCompoundFrames_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCompoundAngle_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCompoundAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    7530
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceCapabilities_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDeviceCapabilities_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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
    7584,
    7626,
    7668
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgUnits_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgUnits_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgUnits_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
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
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
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


/* Object interface: ISampleGrabberFilter, ver. 0.0,
   GUID={0x4591F5BF,0xFBB2,0x4D6E,{0xBD,0xAD,0xE6,0x29,0xE0,0x63,0x5F,0xFB}} */

#pragma code_seg(".orpc")
static const unsigned short ISampleGrabberFilter_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    7710
    };

static const MIDL_STUBLESS_PROXY_INFO ISampleGrabberFilter_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &ISampleGrabberFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISampleGrabberFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
    &ISampleGrabberFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _ISampleGrabberFilterProxyVtbl = 
{
    &ISampleGrabberFilter_ProxyInfo,
    &IID_ISampleGrabberFilter,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ISampleGrabberFilter::get_FilterObj */
};


static const PRPC_STUB_FUNCTION ISampleGrabberFilter_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2
};

CInterfaceStubVtbl _ISampleGrabberFilterStubVtbl =
{
    &IID_ISampleGrabberFilter,
    &ISampleGrabberFilter_ServerInfo,
    8,
    &ISampleGrabberFilter_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgWindowRF, ver. 0.0,
   GUID={0x5E2238EF,0x0DA8,0x48C4,{0x84,0xC5,0xE5,0xE6,0x11,0x94,0x04,0x5D}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgWindowRF_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4962,
    5016,
    5070,
    5112,
    5154,
    5196,
    5238,
    5274,
    5310,
    5346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgWindowRF_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgWindowRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgWindowRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgWindowRF_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IUsgWindowRFProxyVtbl = 
{
    &IUsgWindowRF_ProxyInfo,
    &IID_IUsgWindowRF,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::SetWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::SetWidth */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::SetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::GetWidth */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::GetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::get_LinesRange */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::get_WidthRange */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::get_DepthsRange */ ,
    (void *) (INT_PTR) -1 /* IUsgWindowRF::get_VolumesRange */
};


static const PRPC_STUB_FUNCTION IUsgWindowRF_table[] =
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

CInterfaceStubVtbl _IUsgWindowRFStubVtbl =
{
    &IID_IUsgWindowRF,
    &IUsgWindowRF_ServerInfo,
    17,
    &IUsgWindowRF_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgStreamEnable, ver. 0.0,
   GUID={0x33857397,0xF4BB,0x4B97,{0x97,0x76,0x39,0xC5,0x07,0x36,0xF8,0xBA}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgStreamEnable_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgStreamEnable_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgStreamEnable_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgStreamEnable_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgStreamEnable_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IUsgStreamEnableProxyVtbl = 
{
    &IUsgStreamEnable_ProxyInfo,
    &IID_IUsgStreamEnable,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgStreamEnable::get_Enable */ ,
    (void *) (INT_PTR) -1 /* IUsgStreamEnable::put_Enable */
};


static const PRPC_STUB_FUNCTION IUsgStreamEnable_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgStreamEnableStubVtbl =
{
    &IID_IUsgStreamEnable,
    &IUsgStreamEnable_ServerInfo,
    9,
    &IUsgStreamEnable_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgDataSourceRF, ver. 0.0,
   GUID={0x3D9B6FC0,0x2AB7,0x4CFE,{0x8B,0x04,0x32,0xD9,0x39,0xF8,0x0D,0xFE}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDataSourceRF_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataSourceRF_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataSourceRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataSourceRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataSourceRF_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgDataSourceRFProxyVtbl = 
{
    &IUsgDataSourceRF_ProxyInfo,
    &IID_IUsgDataSourceRF,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgDataSourceRF::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDataSourceRF::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgDataSourceRF::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgDataSourceRF_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgDataSourceRFStubVtbl =
{
    &IID_IUsgDataSourceRF,
    &IUsgDataSourceRF_ServerInfo,
    10,
    &IUsgDataSourceRF_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgMultiBeam, ver. 0.0,
   GUID={0x5431298D,0x7FDF,0x439F,{0x9D,0x23,0x1D,0xAF,0xDE,0x05,0x9F,0x19}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgMultiBeam_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgMultiBeam_ProxyInfo =
    {
    &Object_StubDesc,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgMultiBeam_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgMultiBeam_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Usgfw2__MIDL_ProcFormatString.Format,
    &IUsgMultiBeam_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IUsgMultiBeamProxyVtbl = 
{
    &IUsgMultiBeam_ProxyInfo,
    &IID_IUsgMultiBeam,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgMultiBeam::get_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgMultiBeam::put_Current */ ,
    (void *) (INT_PTR) -1 /* IUsgMultiBeam::get_Values */
};


static const PRPC_STUB_FUNCTION IUsgMultiBeam_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IUsgMultiBeamStubVtbl =
{
    &IID_IUsgMultiBeam,
    &IUsgMultiBeam_ServerInfo,
    10,
    &IUsgMultiBeam_table[-3],
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
    Usgfw2__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x801026e, /* MIDL Version 8.1.622 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _Usgfw2_ProxyVtblList[] = 
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
    ( CInterfaceProxyVtbl *) &_IUsgMultiBeamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerBaseLineProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgRejection2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSampleVolumeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgStreamEnableProxyVtbl,
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
    ( CInterfaceProxyVtbl *) &_ISampleGrabberFilterProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDataSourceRFProxyVtbl,
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
    ( CInterfaceProxyVtbl *) &_IUsgWindowRFProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanTypeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLineProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgLineDensityProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsg3dVolumeSizeProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _Usgfw2_StubVtblList[] = 
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
    ( CInterfaceStubVtbl *) &_IUsgMultiBeamStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerBaseLineStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgRejection2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSampleVolumeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgStreamEnableStubVtbl,
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
    ( CInterfaceStubVtbl *) &_ISampleGrabberFilterStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDataSourceRFStubVtbl,
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
    ( CInterfaceStubVtbl *) &_IUsgWindowRFStubVtbl,
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
    "IUsgMultiBeam",
    "IUsgDopplerBaseLine",
    "IUsgRejection2",
    "IUsgDopplerSampleVolume",
    "IUsgStreamEnable",
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
    "ISampleGrabberFilter",
    "IUsgDataSourceRF",
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
    "IUsgWindowRF",
    "IUsgScanType",
    "IUsgScanLine",
    "IUsgLineDensity",
    "IUsg3dVolumeSize",
    0
};

const IID *  const _Usgfw2_BaseIIDList[] = 
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

    IID_BS_LOOKUP_INITIAL_TEST( _Usgfw2, 85, 64 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 32 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _Usgfw2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _Usgfw2, 85, *pIndex )
    
}

const ExtendedProxyFileInfo Usgfw2_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _Usgfw2_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _Usgfw2_StubVtblList,
    (const PCInterfaceName * ) & _Usgfw2_InterfaceNamesList,
    (const IID ** ) & _Usgfw2_BaseIIDList,
    & _Usgfw2_IID_Lookup, 
    85,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

