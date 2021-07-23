

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for USgfw2.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#include "ndr64types.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "USgfw2.h"

#define TYPE_FORMAT_STRING_SIZE   1889                              
#define PROC_FORMAT_STRING_SIZE   8673                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   4            

typedef struct _USgfw2_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } USgfw2_MIDL_TYPE_FORMAT_STRING;

typedef struct _USgfw2_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } USgfw2_MIDL_PROC_FORMAT_STRING;

typedef struct _USgfw2_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } USgfw2_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

static const RPC_SYNTAX_IDENTIFIER  _NDR64_RpcTransferSyntax = 
{{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}};



extern const USgfw2_MIDL_TYPE_FORMAT_STRING USgfw2__MIDL_TypeFormatString;
extern const USgfw2_MIDL_PROC_FORMAT_STRING USgfw2__MIDL_ProcFormatString;
extern const USgfw2_MIDL_EXPR_FORMAT_STRING USgfw2__MIDL_ExprFormatString;


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


extern const MIDL_SERVER_INFO IUsgScanLine2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanLine2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanLineProperties_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgScanLineProperties2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties2_ProxyInfo;


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


extern const MIDL_SERVER_INFO IUsgCineSink2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgCineSink2_ProxyInfo;


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


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgFrameROI_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgFrameROI_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgProbeElementsTest_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgProbeElementsTest_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IUsgTissueMotionDetector_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IUsgTissueMotionDetector_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const USgfw2_MIDL_PROC_FORMAT_STRING USgfw2__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure get_DetectionResult */


	/* Procedure get_TestResult */


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
/*  8 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

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


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pScanDepth */


	/* Parameter pMode */


	/* Parameter pVal */

/* 26 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 28 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
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


	/* Return value */


	/* Return value */

/* 32 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
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

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	NdrFcShort( 0x8 ),	/* 8 */
/* 52 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 54 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

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

/* 64 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 66 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 68 */	0x8,		/* FC_LONG */
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

/* 70 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 72 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rotate */


	/* Procedure get_DeviceState */


	/* Procedure get_DeviceState */


	/* Procedure get_ScanState */

/* 76 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 78 */	NdrFcLong( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x9 ),	/* 9 */
/* 84 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x24 ),	/* 36 */
/* 90 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 92 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 102 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 104 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 106 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 110 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rotate */


	/* Procedure put_ScanState */

/* 114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0xa ),	/* 10 */
/* 122 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 126 */	NdrFcShort( 0x8 ),	/* 8 */
/* 128 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 130 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 140 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 142 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 144 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 148 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanModes */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0xb ),	/* 11 */
/* 160 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 166 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 168 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 178 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 180 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 182 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Return value */

/* 184 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 186 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanModeObj */

/* 190 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 192 */	NdrFcLong( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0xc ),	/* 12 */
/* 198 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 206 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanMode */

/* 216 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pUsgScanMode */

/* 222 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 224 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 226 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 230 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FilgraphManager */

/* 234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0xd ),	/* 13 */
/* 242 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 250 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 260 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 262 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 264 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 268 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0xe ),	/* 14 */
/* 280 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 282 */	NdrFcShort( 0x54 ),	/* 84 */
/* 284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 286 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 288 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riidCtrl */

/* 298 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 300 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 302 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 304 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 306 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 312 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppObj */

/* 316 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 318 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 320 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 324 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateUsgfwFile */


	/* Procedure CreateUsgfwFile */

/* 328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0xf ),	/* 15 */
/* 336 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 344 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x1 ),	/* 1 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fileName */


	/* Parameter FileName */

/* 354 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 356 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 358 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Parameter createFlags */


	/* Parameter createFlags */

/* 360 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 362 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppFileObject */


	/* Parameter ppFileObject */

/* 366 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 368 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 370 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

	/* Return value */


	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 374 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMixerControl */

/* 378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x7 ),	/* 7 */
/* 386 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 388 */	NdrFcShort( 0x10 ),	/* 16 */
/* 390 */	NdrFcShort( 0x8 ),	/* 8 */
/* 392 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 394 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanMode */

/* 404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 406 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 412 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppMixerCtrl */

/* 416 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 418 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 420 */	NdrFcShort( 0xbc ),	/* Type Offset=188 */

	/* Return value */

/* 422 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 424 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 428 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 436 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 438 */	NdrFcShort( 0x54 ),	/* 84 */
/* 440 */	NdrFcShort( 0x8 ),	/* 8 */
/* 442 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 444 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riidCtrl */

/* 454 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 456 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 458 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 460 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 462 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 466 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 468 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppCtrl */

/* 472 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 474 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 476 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 478 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 480 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateCopy */

/* 484 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 486 */	NdrFcLong( 0x0 ),	/* 0 */
/* 490 */	NdrFcShort( 0x9 ),	/* 9 */
/* 492 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 498 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 500 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppNewScanMode */

/* 510 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 512 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 514 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 518 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */


	/* Procedure put_Contrast */


	/* Procedure Union */

/* 522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0xa ),	/* 10 */
/* 530 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 534 */	NdrFcShort( 0x8 ),	/* 8 */
/* 536 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 538 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter saveFlags */


	/* Parameter newVal */


	/* Parameter nScanMode */

/* 548 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 550 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 554 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 556 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StreamsCollection */

/* 560 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 562 */	NdrFcLong( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0xb ),	/* 11 */
/* 568 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 572 */	NdrFcShort( 0x8 ),	/* 8 */
/* 574 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 576 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 586 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 588 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 590 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */

/* 592 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 594 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanMode */

/* 598 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 600 */	NdrFcLong( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0xc ),	/* 12 */
/* 606 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x24 ),	/* 36 */
/* 612 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 614 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 624 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 626 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 630 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 632 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateClone */

/* 636 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 638 */	NdrFcLong( 0x0 ),	/* 0 */
/* 642 */	NdrFcShort( 0xd ),	/* 13 */
/* 644 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x8 ),	/* 8 */
/* 650 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 652 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppNewScanMode */

/* 662 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 664 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 666 */	NdrFcShort( 0x20 ),	/* Type Offset=32 */

	/* Return value */

/* 668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 670 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetControlObj */

/* 674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x8 ),	/* 8 */
/* 682 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 684 */	NdrFcShort( 0x44 ),	/* 68 */
/* 686 */	NdrFcShort( 0x8 ),	/* 8 */
/* 688 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 690 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riid */

/* 700 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 702 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 704 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter ppobj */

/* 706 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 708 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 710 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */

/* 712 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 714 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMixerControl */

/* 718 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 720 */	NdrFcLong( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x9 ),	/* 9 */
/* 726 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 730 */	NdrFcShort( 0x8 ),	/* 8 */
/* 732 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 734 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppMixerControl */

/* 744 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 746 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 748 */	NdrFcShort( 0xbc ),	/* Type Offset=188 */

	/* Return value */

/* 750 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 752 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_EnableDetection */


	/* Procedure get_BatteryLifeRemaining */


	/* Procedure get_TransparencyMode */


	/* Procedure get_Mute */


	/* Procedure get_Enabled */


	/* Procedure get_PriorityMode */


	/* Procedure get_SweepSpeed */


	/* Procedure get_Enabled */


	/* Procedure get_Enabled */


	/* Procedure get_StreamId */

/* 756 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 758 */	NdrFcLong( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0xa ),	/* 10 */
/* 764 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x24 ),	/* 36 */
/* 770 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 772 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */

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

/* 782 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 784 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 786 */	0x8,		/* FC_LONG */
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

/* 788 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 790 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_PowerLineStatus */


	/* Procedure get_ScanState */

/* 794 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 796 */	NdrFcLong( 0x0 ),	/* 0 */
/* 800 */	NdrFcShort( 0xb ),	/* 11 */
/* 802 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 806 */	NdrFcShort( 0x24 ),	/* 36 */
/* 808 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 810 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 818 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 820 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 822 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 824 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 826 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 828 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_ScanState */

/* 832 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 838 */	NdrFcShort( 0xc ),	/* 12 */
/* 840 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 844 */	NdrFcShort( 0x8 ),	/* 8 */
/* 846 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 848 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 856 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 858 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 860 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 862 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 864 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 866 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 868 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOutputWindow */

/* 870 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 872 */	NdrFcLong( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x7 ),	/* 7 */
/* 878 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 882 */	NdrFcShort( 0x8 ),	/* 8 */
/* 884 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 886 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 894 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hWnd */

/* 896 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 898 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 902 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 904 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAlphaBitmapParameters */

/* 908 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 910 */	NdrFcLong( 0x0 ),	/* 0 */
/* 914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 916 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x8 ),	/* 8 */
/* 922 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 924 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 926 */	NdrFcShort( 0x1 ),	/* 1 */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x0 ),	/* 0 */
/* 932 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 934 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 936 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 938 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Return value */

/* 940 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 942 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAlphaBitmap */

/* 946 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 948 */	NdrFcLong( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x9 ),	/* 9 */
/* 954 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 958 */	NdrFcShort( 0x8 ),	/* 8 */
/* 960 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 962 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 966 */	NdrFcShort( 0x1 ),	/* 1 */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 970 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 972 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 974 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 976 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Return value */

/* 978 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 980 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 982 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpdateAlphaBitmapParameters */

/* 984 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 986 */	NdrFcLong( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0xa ),	/* 10 */
/* 992 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x8 ),	/* 8 */
/* 998 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1000 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 1010 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1012 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1014 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Return value */

/* 1016 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1018 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOutputRect */

/* 1022 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1024 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0xb ),	/* 11 */
/* 1030 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1032 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1034 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1036 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1038 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRect */

/* 1048 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1050 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1052 */	NdrFcShort( 0x172 ),	/* Type Offset=370 */

	/* Return value */

/* 1054 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1056 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Capture */

/* 1060 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1062 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1066 */	NdrFcShort( 0xc ),	/* 12 */
/* 1068 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1072 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1074 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1076 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppCurrentImage */

/* 1086 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1088 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1090 */	NdrFcShort( 0x1a2 ),	/* Type Offset=418 */

	/* Return value */

/* 1092 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1094 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_AutoResolution */


	/* Procedure get_Negative */


	/* Procedure SwitchConnectorSupported */


	/* Procedure GetCurrentBitmap */

/* 1098 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1100 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1104 */	NdrFcShort( 0xd ),	/* 13 */
/* 1106 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1110 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1112 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1114 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter bSupported */


	/* Parameter handle */

/* 1124 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1126 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1130 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1132 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Repaint */

/* 1136 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1142 */	NdrFcShort( 0xe ),	/* 14 */
/* 1144 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1146 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1150 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1152 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1160 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hWindow */

/* 1162 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1164 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hDC */

/* 1168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1170 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1176 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanState */

/* 1180 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1186 */	NdrFcShort( 0xf ),	/* 15 */
/* 1188 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1194 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1196 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1206 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1208 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1210 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1212 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1214 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_ScanState */

/* 1218 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1220 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1224 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1226 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1232 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1234 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 1244 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1246 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1248 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 1250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1252 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BkColor */

/* 1256 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1262 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1264 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1268 */	NdrFcShort( 0x30 ),	/* 48 */
/* 1270 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1272 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1282 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1284 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1286 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Return value */

/* 1288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1290 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_BkColor */

/* 1294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1300 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1302 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1304 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1308 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1310 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1318 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 1320 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1322 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1324 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Return value */

/* 1326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1328 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOutputRect */

/* 1332 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1338 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1340 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1344 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1346 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1348 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1356 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRect */

/* 1358 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1360 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1362 */	NdrFcShort( 0x172 ),	/* Type Offset=370 */

	/* Return value */

/* 1364 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1366 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeArrive */

/* 1370 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1372 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1376 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1378 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1380 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1382 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1384 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1386 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1396 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1398 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1400 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter reserved */

/* 1402 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1404 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1410 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerArrive */

/* 1414 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1422 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1424 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1426 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1428 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1430 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1438 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1440 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1442 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1444 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter reserved */

/* 1446 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1448 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1452 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1454 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeRemove */

/* 1458 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1460 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1464 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1466 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1468 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1470 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1472 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1474 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1482 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1484 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1486 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1488 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter reserved */

/* 1490 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1492 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1496 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1498 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerRemove */

/* 1502 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1504 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1508 */	NdrFcShort( 0xa ),	/* 10 */
/* 1510 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1512 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1514 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1516 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1518 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1526 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1528 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1530 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1532 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter reserved */

/* 1534 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1536 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1538 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1540 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1542 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1544 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnProbeStateChanged */

/* 1546 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1548 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1552 */	NdrFcShort( 0xb ),	/* 11 */
/* 1554 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1556 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1558 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1560 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1562 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1570 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1572 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1574 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1576 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter reserved */

/* 1578 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1580 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1586 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeamformerStateChanged */

/* 1590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1596 */	NdrFcShort( 0xc ),	/* 12 */
/* 1598 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1600 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1602 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1604 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1606 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1614 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1616 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1618 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1620 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter reserved */

/* 1622 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1624 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1630 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanDepthRange */

/* 1634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1640 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1642 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1646 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1648 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1650 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1658 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pScanDepthMin */

/* 1660 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1662 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pScanDepthMax */

/* 1666 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1668 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1674 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ScanConverter */


	/* Procedure get_ProbesCollection */

/* 1678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1684 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1686 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1692 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1694 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1702 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 1704 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1706 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1708 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */


	/* Return value */

/* 1710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1712 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BeamformersCollection */

/* 1716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1722 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1724 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1730 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1732 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1742 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1744 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1746 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */

/* 1748 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1750 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataView */

/* 1754 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1756 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1760 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1762 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1766 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1768 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1770 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1778 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUnkSource */

/* 1780 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1782 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1784 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter ppUsgDataView */

/* 1786 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1788 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1790 */	NdrFcShort( 0x1ce ),	/* Type Offset=462 */

	/* Return value */

/* 1792 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1794 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Invalidate */

/* 1798 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1800 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1804 */	NdrFcShort( 0xa ),	/* 10 */
/* 1806 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1812 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1814 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1822 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1824 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1826 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeamformerNameFromCode */

/* 1830 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1832 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1836 */	NdrFcShort( 0xb ),	/* 11 */
/* 1838 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1840 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1844 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1846 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 1848 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1854 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Code */

/* 1856 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1858 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Name */

/* 1862 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1864 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1866 */	NdrFcShort( 0x1ec ),	/* Type Offset=492 */

	/* Return value */

/* 1868 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1870 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure BeamformerCodeFromName */

/* 1874 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1876 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1880 */	NdrFcShort( 0xc ),	/* 12 */
/* 1882 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1886 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1888 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1890 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1892 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1894 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1898 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Name */

/* 1900 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1902 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1904 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Parameter Code */

/* 1906 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1908 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1912 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1914 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_SerialNumber */


	/* Procedure ProbeNameFromCode */

/* 1918 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1920 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1924 */	NdrFcShort( 0xd ),	/* 13 */
/* 1926 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1928 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1932 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 1934 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 1936 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1942 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */


	/* Parameter Code */

/* 1944 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1946 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter Name */

/* 1950 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1952 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1954 */	NdrFcShort( 0x1ec ),	/* Type Offset=492 */

	/* Return value */


	/* Return value */

/* 1956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1958 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProbeCodeFromName */

/* 1962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1968 */	NdrFcShort( 0xe ),	/* 14 */
/* 1970 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1974 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1976 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1978 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1982 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1986 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Name */

/* 1988 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1990 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1992 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Parameter Code */

/* 1994 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1996 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2002 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 2006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2012 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2014 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2016 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2020 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2022 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lItem */

/* 2032 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2034 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppUnk */

/* 2038 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2040 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2042 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Return value */

/* 2044 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2046 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FileName */


	/* Procedure get_Name */


	/* Procedure get_Name */

/* 2050 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2056 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2058 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2062 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2064 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2066 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 2068 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2074 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 2076 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2078 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2080 */	NdrFcShort( 0x1ec ),	/* Type Offset=492 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 2082 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2084 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Probes */


	/* Procedure get_Beamformer */

/* 2088 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2090 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2094 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2096 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2102 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2104 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2114 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2116 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2118 */	NdrFcShort( 0x1f6 ),	/* Type Offset=502 */

	/* Return value */


	/* Return value */

/* 2120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2122 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DisplayName */


	/* Procedure get_DispalyName */

/* 2126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2132 */	NdrFcShort( 0xa ),	/* 10 */
/* 2134 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2140 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2142 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 2144 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2152 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2154 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2156 */	NdrFcShort( 0x1ec ),	/* Type Offset=492 */

	/* Return value */


	/* Return value */

/* 2158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2160 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_AutoProbeCenter */


	/* Procedure get_Gamma */


	/* Procedure get_Code */

/* 2164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2170 */	NdrFcShort( 0xb ),	/* 11 */
/* 2172 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2176 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2178 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2180 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2188 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 2190 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2192 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 2196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2198 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Type */

/* 2202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2208 */	NdrFcShort( 0xc ),	/* 12 */
/* 2210 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2214 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2216 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2218 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2226 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 2228 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2230 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2232 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 2234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2236 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDevice */

/* 2240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2246 */	NdrFcShort( 0xb ),	/* 11 */
/* 2248 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2254 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2256 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2268 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SwitchConnector */

/* 2272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2278 */	NdrFcShort( 0xc ),	/* 12 */
/* 2280 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2286 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 2288 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2296 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 2298 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2300 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CtlPointsCount */


	/* Procedure get_Code */

/* 2304 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2306 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2310 */	NdrFcShort( 0xe ),	/* 14 */
/* 2312 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2316 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2318 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2320 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2330 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2332 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2338 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 2342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2350 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2354 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2356 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2358 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2366 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lItem */

/* 2368 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2370 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pScanMode */

/* 2374 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2376 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2382 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2384 */	0x8,		/* FC_LONG */
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

/* 2386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2392 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2394 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2400 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 2402 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2410 */	NdrFcShort( 0x0 ),	/* 0 */

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

/* 2412 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2414 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2416 */	NdrFcShort( 0x20c ),	/* Type Offset=524 */

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

/* 2418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2420 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 2424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2432 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2436 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2438 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 2440 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 2442 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2448 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lItem */

/* 2450 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2452 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 2456 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 2458 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2460 */	NdrFcShort( 0x69a ),	/* Type Offset=1690 */

	/* Return value */

/* 2462 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2464 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_EnableDetection */


	/* Procedure put_TransparencyMode */


	/* Procedure put_Mute */


	/* Procedure put_Enabled */


	/* Procedure put_PriorityMode */


	/* Procedure put_SweepSpeed */


	/* Procedure put_Enabled */


	/* Procedure put_Enabled */

/* 2468 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2470 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2474 */	NdrFcShort( 0xb ),	/* 11 */
/* 2476 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2478 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2480 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2482 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 2484 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2492 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */


	/* Parameter newVal */

/* 2494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2496 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2502 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VisibleMin */


	/* Procedure get_VisibleMin */


	/* Procedure PositionToDepth */


	/* Procedure get_FocusSet */

/* 2506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2512 */	NdrFcShort( 0xa ),	/* 10 */
/* 2514 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2518 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2520 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2522 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter units */


	/* Parameter units */


	/* Parameter pos */


	/* Parameter mode */

/* 2532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2534 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter depth */


	/* Parameter pVal */

/* 2538 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2540 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2544 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2546 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FocusSet */

/* 2550 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2552 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2556 */	NdrFcShort( 0xb ),	/* 11 */
/* 2558 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2560 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2564 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2566 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2576 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2578 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter newVal */

/* 2582 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2584 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2586 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2588 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2590 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */


	/* Procedure get_FocusSetCount */

/* 2594 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2596 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2600 */	NdrFcShort( 0xc ),	/* 12 */
/* 2602 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2606 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2608 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 2610 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2618 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctlPoint */


	/* Parameter mode */

/* 2620 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2622 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2626 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2628 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 2632 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2634 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocalZonesCount */

/* 2638 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2640 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2644 */	NdrFcShort( 0xd ),	/* 13 */
/* 2646 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2648 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2650 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2652 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2654 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2662 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2664 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2666 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2670 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2672 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocalZones */

/* 2676 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2678 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2680 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2682 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2684 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2686 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocalZone */

/* 2688 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2690 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2694 */	NdrFcShort( 0xe ),	/* 14 */
/* 2696 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 2698 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2700 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2702 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 2704 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2712 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2714 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2716 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2720 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2722 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter zoneIndex */

/* 2726 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2728 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocalZone */

/* 2732 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2734 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2736 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */

/* 2738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2740 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocusPoint */

/* 2744 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2750 */	NdrFcShort( 0xf ),	/* 15 */
/* 2752 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 2754 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2756 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2758 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 2760 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2768 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2770 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2772 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2776 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2778 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter zoneIndex */

/* 2782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2784 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusPoint */

/* 2788 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2790 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2796 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFocusState */

/* 2800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2806 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2808 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 2810 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2814 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 2816 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 2818 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2824 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2826 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2828 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2832 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2834 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2838 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2840 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusState */

/* 2844 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 2846 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2848 */	NdrFcShort( 0x6b4 ),	/* Type Offset=1716 */

	/* Return value */

/* 2850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2852 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFocusState */

/* 2856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2862 */	NdrFcShort( 0x11 ),	/* 17 */
/* 2864 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 2866 */	NdrFcShort( 0x18 ),	/* 24 */
/* 2868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2870 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 2872 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 2874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2876 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2880 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 2882 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2884 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter focusSet */

/* 2888 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2890 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2894 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2896 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFocusState */

/* 2900 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2902 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2904 */	NdrFcShort( 0x6b4 ),	/* Type Offset=1716 */

	/* Return value */

/* 2906 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2908 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTgcEntries */

/* 2912 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2914 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2918 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2920 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2922 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2924 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2926 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2928 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2930 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2932 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2936 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 2938 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2940 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2944 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2946 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTgcData */

/* 2950 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2952 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 2954 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2958 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 2960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTgcEntries */

/* 2962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2968 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2970 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 2972 */	NdrFcShort( 0x2c ),	/* 44 */
/* 2974 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2976 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 2978 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2986 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 2988 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2990 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 2992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 2994 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2996 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 2998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTgcData */

/* 3000 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 3002 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3006 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3008 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3010 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryLifePercent */


	/* Procedure get_OutputPin */


	/* Procedure get_FramesDroppedInRenderer */


	/* Procedure get_Contrast */


	/* Procedure GetTgcSize */

/* 3012 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3014 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3018 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3020 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3024 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3026 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3028 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3036 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 3038 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3040 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 3044 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3046 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VisibleMax */


	/* Procedure get_VisibleMax */


	/* Procedure DepthToPosition */

/* 3050 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3056 */	NdrFcShort( 0xb ),	/* 11 */
/* 3058 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3060 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3062 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3064 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3066 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3074 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter units */


	/* Parameter units */


	/* Parameter depth */

/* 3076 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3078 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3080 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pos */

/* 3082 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3084 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 3088 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3090 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 3094 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3096 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3100 */	NdrFcShort( 0xd ),	/* 13 */
/* 3102 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3104 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3106 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3108 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3110 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3118 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctlPoint */

/* 3120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3122 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter newVal */

/* 3126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3128 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3134 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CtlPointsCount */

/* 3138 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3144 */	NdrFcShort( 0xf ),	/* 15 */
/* 3146 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3152 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3154 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3162 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 3164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3166 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3172 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CtlPointPos */

/* 3176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3182 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3184 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3188 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3190 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3192 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctlPoint */

/* 3202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3204 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 3208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3210 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3216 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ValuesTgc */

/* 3220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3226 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3228 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3234 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 3236 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3246 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3248 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3250 */	NdrFcShort( 0x20c ),	/* Type Offset=524 */

	/* Return value */

/* 3252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3254 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_AutoProbeCenter */


	/* Procedure put_Gamma */

/* 3258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3264 */	NdrFcShort( 0xc ),	/* 12 */
/* 3266 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3272 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3274 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3282 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 3284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3286 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3292 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_AutoResolution */


	/* Procedure put_Negative */

/* 3296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3302 */	NdrFcShort( 0xe ),	/* 14 */
/* 3304 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3310 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3312 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 3322 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3324 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3328 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3330 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BrightnessMin */

/* 3334 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3340 */	NdrFcShort( 0xf ),	/* 15 */
/* 3342 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3346 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3348 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3350 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3358 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3360 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3362 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3366 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3368 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BrightnessMax */

/* 3372 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3378 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3380 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3384 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3386 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3388 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3398 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3400 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3404 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3406 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ContrastMin */

/* 3410 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3412 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3416 */	NdrFcShort( 0x11 ),	/* 17 */
/* 3418 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3422 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3424 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3426 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3434 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3436 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3438 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3442 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3444 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ContrastMax */

/* 3448 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3450 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3454 */	NdrFcShort( 0x12 ),	/* 18 */
/* 3456 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3460 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3462 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3464 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3474 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3476 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3482 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCapabilities */


	/* Procedure get_GammaMin */

/* 3486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3492 */	NdrFcShort( 0x13 ),	/* 19 */
/* 3494 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3498 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3500 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3502 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3510 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCaps */


	/* Parameter pVal */

/* 3512 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3514 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3516 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3518 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3520 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_GammaMax */

/* 3524 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3526 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3530 */	NdrFcShort( 0x14 ),	/* 20 */
/* 3532 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3536 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3538 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3540 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3548 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3550 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3552 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3558 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetReferenceData */

/* 3562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3568 */	NdrFcShort( 0x15 ),	/* 21 */
/* 3570 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 3572 */	NdrFcShort( 0x2c ),	/* 44 */
/* 3574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3576 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3578 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nValueMax */

/* 3588 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3590 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nValuesCount */

/* 3594 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3596 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 3600 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 3602 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3604 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3606 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3608 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetReferenceData */

/* 3612 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3614 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3618 */	NdrFcShort( 0x16 ),	/* 22 */
/* 3620 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 3622 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3624 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3626 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3628 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3636 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pValueMax */

/* 3638 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3640 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValuesCount */

/* 3644 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3646 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 3650 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3652 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3656 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3658 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Calculate */

/* 3662 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3664 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3668 */	NdrFcShort( 0x17 ),	/* 23 */
/* 3670 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 3672 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3674 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3676 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3678 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3686 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 3688 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3690 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 3694 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3696 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 3700 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3702 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3704 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3708 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPaletteEntries */

/* 3712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3718 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3720 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 3722 */	NdrFcShort( 0x38 ),	/* 56 */
/* 3724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3726 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3728 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3736 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 3738 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3740 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 3744 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3746 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3748 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPalEntries */

/* 3750 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3752 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3754 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Return value */

/* 3756 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3758 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3760 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPaletteEntries */


	/* Procedure GetPaletteEntries */

/* 3762 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3764 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3770 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 3772 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3774 */	NdrFcShort( 0x30 ),	/* 48 */
/* 3776 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3778 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3786 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */


	/* Parameter startIndex */

/* 3788 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3790 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */


	/* Parameter entries */

/* 3794 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3796 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPalEntries */


	/* Parameter pPalEntries */

/* 3800 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3802 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3804 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Return value */


	/* Return value */

/* 3806 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3808 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOrigin */

/* 3812 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3818 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3820 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 3822 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3824 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3826 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 3828 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3830 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3836 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter point */

/* 3838 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3840 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3842 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Parameter pixelsOrigin */

/* 3844 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3846 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3848 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 3850 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3852 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3856 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3858 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetOrigin */

/* 3862 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3864 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3870 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3872 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3874 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3876 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3878 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3886 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter point */

/* 3888 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3890 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3892 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Parameter pixelsOrigin */

/* 3894 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3896 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3898 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 3900 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3902 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3904 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetResolution */

/* 3906 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3908 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3912 */	NdrFcShort( 0x9 ),	/* 9 */
/* 3914 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 3916 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3918 */	NdrFcShort( 0x34 ),	/* 52 */
/* 3920 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 3922 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter resolution */

/* 3932 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3934 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3936 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Parameter fPrefered */

/* 3938 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3940 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3944 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3946 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetResolution */

/* 3950 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3952 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3956 */	NdrFcShort( 0xa ),	/* 10 */
/* 3958 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 3960 */	NdrFcShort( 0x2c ),	/* 44 */
/* 3962 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3964 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 3966 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3974 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter resolution */

/* 3976 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3978 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 3980 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Return value */

/* 3982 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3984 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 3986 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PixelsToUnits */

/* 3988 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3990 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3994 */	NdrFcShort( 0xf ),	/* 15 */
/* 3996 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 3998 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4000 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4002 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 4004 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4012 */	NdrFcShort( 0x14 ),	/* 20 */

	/* Parameter Xpix */

/* 4014 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4016 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4018 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Ypix */

/* 4020 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4022 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4024 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Xunit */

/* 4026 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4028 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4030 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Yunit */

/* 4032 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4034 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4036 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pixOrigin */

/* 4038 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4040 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4042 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 4044 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4046 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnitsToPixels */

/* 4050 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4056 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4058 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4060 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4062 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4064 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 4066 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4074 */	NdrFcShort( 0x14 ),	/* 20 */

	/* Parameter Xunit */

/* 4076 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4078 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4080 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Yunit */

/* 4082 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4084 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4086 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter XPix */

/* 4088 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4090 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4092 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Ypix */

/* 4094 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4096 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4098 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter pixOrigin */

/* 4100 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4102 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4104 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 4106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4108 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetViewRect */

/* 4112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4118 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4120 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4122 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4126 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4128 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4136 */	NdrFcShort( 0x154 ),	/* 340 */

	/* Parameter left */

/* 4138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4140 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4142 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter top */

/* 4144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4146 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4148 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter right */

/* 4150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4152 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4154 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 4156 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4158 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4160 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4162 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4164 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetViewRect */

/* 4168 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4170 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4174 */	NdrFcShort( 0x12 ),	/* 18 */
/* 4176 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4180 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4182 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 4184 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4192 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter left */

/* 4194 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4196 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4198 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter top */

/* 4200 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4202 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4204 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter right */

/* 4206 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4208 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4210 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 4212 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4214 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4216 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 4218 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4220 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4226 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetZoom */

/* 4230 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4236 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4238 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4240 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4244 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4246 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4254 */	NdrFcShort( 0x14 ),	/* 20 */

	/* Parameter ZoomCenterX */

/* 4256 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4258 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4260 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomCenterY */

/* 4262 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4264 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4266 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomFactor */

/* 4268 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4270 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4274 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4276 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetZoom */

/* 4280 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4286 */	NdrFcShort( 0x14 ),	/* 20 */
/* 4288 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4292 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4294 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4296 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4304 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ZoomCenterX */

/* 4306 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4308 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4310 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZooomCenterY */

/* 4312 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4314 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4316 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ZoomFactor */

/* 4318 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4320 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fPrefered */

/* 4324 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4326 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4332 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */


	/* Procedure ControlChanged */

/* 4336 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4342 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4344 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4346 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4350 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 4352 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4360 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter dispId */

/* 4362 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4364 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter flags */

/* 4368 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4370 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4374 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4376 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnControlChanged */

/* 4380 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4382 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4386 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4388 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 4390 */	NdrFcShort( 0x64 ),	/* 100 */
/* 4392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4394 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 4396 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter riidCtrl */

/* 4406 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4408 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4410 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter scanMode */

/* 4412 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4414 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 4418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4420 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pControlObj */

/* 4424 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4426 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4428 */	NdrFcShort( 0x6da ),	/* Type Offset=1754 */

	/* Parameter dispId */

/* 4430 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4432 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter flags */

/* 4436 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4438 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4442 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4444 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnControlChangedBSTR */

/* 4448 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4450 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4454 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4456 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 4458 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4462 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 4464 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 4466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4468 */	NdrFcShort( 0x1 ),	/* 1 */
/* 4470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctrlGUID */

/* 4474 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 4476 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4478 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Parameter scanMode */

/* 4480 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4482 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter streamId */

/* 4486 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4488 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pControlObject */

/* 4492 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4494 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4496 */	NdrFcShort( 0x6da ),	/* Type Offset=1754 */

	/* Parameter dispId */

/* 4498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4500 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter flags */

/* 4504 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4506 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4512 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 4516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4522 */	NdrFcShort( 0xa ),	/* 10 */
/* 4524 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4526 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4528 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4530 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4532 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4540 */	NdrFcShort( 0x14 ),	/* 20 */

	/* Parameter x */

/* 4542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4544 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4546 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter y */

/* 4548 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4550 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4552 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4554 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4556 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4560 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4562 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 4566 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4568 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4572 */	NdrFcShort( 0xb ),	/* 11 */
/* 4574 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4578 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4580 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4582 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4588 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4590 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4592 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4594 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4598 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4600 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4602 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter x */

/* 4604 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4606 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4608 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter y */

/* 4610 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4612 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4614 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4616 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4618 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 4622 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4624 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4628 */	NdrFcShort( 0xc ),	/* 12 */
/* 4630 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 4632 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4634 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4636 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x8,		/* 8 */
/* 4638 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4646 */	NdrFcShort( 0x5000 ),	/* 20480 */

	/* Parameter scanLine */

/* 4648 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4650 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4652 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4654 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4656 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4658 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4660 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4662 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4664 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4666 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4668 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4670 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 4672 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4674 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4676 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4678 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4680 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4682 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4684 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4686 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4688 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4690 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4692 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 4694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine2 */

/* 4696 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4698 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4702 */	NdrFcShort( 0xd ),	/* 13 */
/* 4704 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4706 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4708 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4710 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4712 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4720 */	NdrFcShort( 0x50 ),	/* 80 */

	/* Parameter subFrame */

/* 4722 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4724 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X */

/* 4728 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4730 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4732 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4734 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4736 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4738 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4740 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4742 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4746 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4748 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates3 */

/* 4752 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4754 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4758 */	NdrFcShort( 0xe ),	/* 14 */
/* 4760 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4762 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4764 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4766 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 4768 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4776 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4778 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4780 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 4784 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4786 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4790 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4792 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4794 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 4796 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4798 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4800 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4802 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4804 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4806 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4808 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4810 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4812 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates4 */

/* 4814 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4816 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4820 */	NdrFcShort( 0xf ),	/* 15 */
/* 4822 */	NdrFcShort( 0x50 ),	/* X64 Stack size/offset = 80 */
/* 4824 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4826 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4828 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x9,		/* 9 */
/* 4830 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4838 */	NdrFcShort( 0x4000 ),	/* 16384 */

	/* Parameter scanLine */

/* 4840 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4842 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 4846 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4848 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4852 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4854 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4856 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4858 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4860 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4862 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4864 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4866 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4868 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 4870 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4872 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4874 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4876 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4878 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 4880 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4882 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4884 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 4886 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4888 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4890 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 4892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 4894 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4900 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4902 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 4904 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4906 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4908 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4910 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4916 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4918 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4920 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4922 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4926 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4928 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4930 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 4932 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4934 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4936 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4938 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4940 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4942 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4944 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4946 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 4948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 4950 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4952 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4956 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4958 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 4960 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4962 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4964 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x8,		/* 8 */
/* 4966 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4974 */	NdrFcShort( 0x5000 ),	/* 20480 */

	/* Parameter scanLine */

/* 4976 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4978 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 4980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4982 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4984 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 4986 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4988 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4990 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 4992 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4994 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4996 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 4998 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 5000 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5002 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5004 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 5006 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5008 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5010 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 5012 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5014 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 5016 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5018 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5020 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 5022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 5024 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5026 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5030 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5032 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5034 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5036 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5038 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 5040 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5048 */	NdrFcShort( 0x14 ),	/* 20 */

	/* Parameter X */

/* 5050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5052 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5054 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5058 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5060 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 5062 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5064 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5068 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5070 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLinesRange */

/* 5074 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5076 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5080 */	NdrFcShort( 0xa ),	/* 10 */
/* 5082 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5086 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5088 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5090 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5098 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */

/* 5100 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5102 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 5106 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5108 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5112 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5114 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointDepth */

/* 5118 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5120 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5124 */	NdrFcShort( 0xb ),	/* 11 */
/* 5126 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5128 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5130 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5132 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 5134 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5142 */	NdrFcShort( 0x14 ),	/* 20 */

	/* Parameter X */

/* 5144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5146 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5148 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5152 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5154 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 5156 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5158 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5160 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5162 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5164 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates3 */

/* 5168 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5170 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5174 */	NdrFcShort( 0xc ),	/* 12 */
/* 5176 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 5178 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5180 */	NdrFcShort( 0x5c ),	/* 92 */
/* 5182 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 5184 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5192 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 5194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5196 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 5200 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5202 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 5206 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5208 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5210 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 5212 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5214 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5216 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5218 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5220 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5222 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5226 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates4 */

/* 5230 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5236 */	NdrFcShort( 0xd ),	/* 13 */
/* 5238 */	NdrFcShort( 0x50 ),	/* X64 Stack size/offset = 80 */
/* 5240 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5242 */	NdrFcShort( 0x78 ),	/* 120 */
/* 5244 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x9,		/* 9 */
/* 5246 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5254 */	NdrFcShort( 0x4000 ),	/* 16384 */

	/* Parameter scanLine */

/* 5256 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5258 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 5262 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5264 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5266 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 5268 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5270 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5272 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 5274 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5276 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5278 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 5280 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5282 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5284 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 5286 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5288 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5290 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 5292 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5294 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 5296 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 5298 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5300 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 5302 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5306 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 5308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine2 */

/* 5310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5316 */	NdrFcShort( 0xe ),	/* 14 */
/* 5318 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5320 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5322 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5324 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5326 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5334 */	NdrFcShort( 0x50 ),	/* 80 */

	/* Parameter subFrame */

/* 5336 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5338 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X */

/* 5342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5344 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5346 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5348 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5350 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5352 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 5354 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5356 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5360 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5362 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointDepth2 */

/* 5366 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5368 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5372 */	NdrFcShort( 0xf ),	/* 15 */
/* 5374 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5376 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5378 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5380 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5382 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5390 */	NdrFcShort( 0x50 ),	/* 80 */

	/* Parameter subFrame */

/* 5392 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5394 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5396 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X */

/* 5398 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5400 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5402 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5406 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5408 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 5410 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5412 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5414 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5418 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryChargeStatus */


	/* Procedure get_Current */

/* 5422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5428 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5430 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5434 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5436 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5438 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5448 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5450 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5452 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5454 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5456 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 5460 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5462 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5468 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5470 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5474 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5476 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5484 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 5486 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5488 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5490 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 5492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5494 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Timeout */


	/* Procedure get_NumberOfElements */


	/* Procedure get_BatteryFullLifetime */


	/* Procedure get_InputPin */


	/* Procedure get_FramesDrawn */

/* 5498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5506 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5510 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5512 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5514 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5518 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5522 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5524 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5526 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 5530 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5532 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Preset */

/* 5536 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5542 */	NdrFcShort( 0xc ),	/* 12 */
/* 5544 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5546 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5548 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5550 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 5552 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 5554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5556 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5560 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cEntries */

/* 5562 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5564 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5566 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValues */

/* 5568 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5570 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5572 */	NdrFcShort( 0x6f0 ),	/* Type Offset=1776 */

	/* Parameter iCurrent */

/* 5574 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5576 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SweepTime */

/* 5580 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5582 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5588 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetColorMap */

/* 5592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5598 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5600 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5602 */	NdrFcShort( 0x60 ),	/* 96 */
/* 5604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5606 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5608 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5616 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTable1 */

/* 5618 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5620 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5622 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Parameter size1 */

/* 5624 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5626 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTable2 */

/* 5630 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5632 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5634 */	NdrFcShort( 0x1bc ),	/* Type Offset=444 */

	/* Parameter size2 */

/* 5636 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5638 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5640 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5644 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWindow */


	/* Procedure SetWindow */


	/* Procedure SetWindow */

/* 5648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5654 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5656 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5658 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5662 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5664 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5672 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5674 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5676 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5680 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5682 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5686 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5688 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5692 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5694 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5698 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5700 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindow */


	/* Procedure GetWindow */


	/* Procedure GetWindow */

/* 5704 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5710 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5712 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 5714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5716 */	NdrFcShort( 0x78 ),	/* 120 */
/* 5718 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5720 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5728 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5730 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5732 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5736 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5738 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5740 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5742 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5744 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5748 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5750 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5754 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5756 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 5758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWidth */


	/* Procedure SetWidth */


	/* Procedure SetWidth */

/* 5760 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5766 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5768 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5770 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5774 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5776 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5784 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5786 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5788 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5792 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5794 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5800 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */


	/* Procedure SetVolume */


	/* Procedure SetVolume */

/* 5804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5810 */	NdrFcShort( 0xa ),	/* 10 */
/* 5812 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5814 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5816 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5818 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5820 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5828 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5830 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5832 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5836 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5838 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5840 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5842 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5844 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWidth */


	/* Procedure GetWidth */


	/* Procedure GetWidth */

/* 5848 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5850 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5854 */	NdrFcShort( 0xb ),	/* 11 */
/* 5856 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5860 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5862 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5864 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5872 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5874 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5876 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5880 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5882 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5886 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5888 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */


	/* Procedure GetVolume */


	/* Procedure GetPhantomVolume */


	/* Procedure GetVolume */

/* 5892 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5894 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5898 */	NdrFcShort( 0xc ),	/* 12 */
/* 5900 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 5902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5904 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5906 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5908 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5916 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5918 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5920 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5924 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5926 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5928 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 5930 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5932 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_LinesRange */


	/* Procedure get_LinesRange */


	/* Procedure get_LinesRange */

/* 5936 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5938 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5942 */	NdrFcShort( 0xd ),	/* 13 */
/* 5944 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5948 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5950 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5952 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5960 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5962 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5964 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 5966 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5968 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5970 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 5972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_WidthRange */


	/* Procedure get_WidthRange */


	/* Procedure get_WidthRange */

/* 5974 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5976 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5980 */	NdrFcShort( 0xe ),	/* 14 */
/* 5982 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 5984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5986 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5988 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5990 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5998 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 6000 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 6002 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6004 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 6006 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6008 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6010 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */


	/* Procedure get_DepthsRange */


	/* Procedure get_DepthsRange */

/* 6012 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6014 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6018 */	NdrFcShort( 0xf ),	/* 15 */
/* 6020 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6024 */	NdrFcShort( 0x38 ),	/* 56 */
/* 6026 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6028 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6036 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 6038 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 6040 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6042 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 6044 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6046 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */


	/* Procedure get_VolumesRange */


	/* Procedure get_VolumesRange */

/* 6050 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6056 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6058 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6062 */	NdrFcShort( 0x38 ),	/* 56 */
/* 6064 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6066 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6070 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6072 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6074 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 6076 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 6078 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6080 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 6082 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6084 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CurrentPosition */


	/* Procedure get_CurrentPosition */

/* 6088 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6090 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6094 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6096 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6100 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6102 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6104 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 6114 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6116 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6118 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6122 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CurrentPosition */


	/* Procedure put_CurrentPosition */

/* 6126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6134 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6136 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6140 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6142 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 6152 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6154 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6156 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6160 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopPosition */


	/* Procedure get_Duration */

/* 6164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6170 */	NdrFcShort( 0x9 ),	/* 9 */
/* 6172 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6176 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6178 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6180 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6188 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 6190 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6192 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6194 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6198 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 6202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6208 */	NdrFcShort( 0xa ),	/* 10 */
/* 6210 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6214 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6216 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6218 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6226 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6228 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6230 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6232 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6236 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 6240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6246 */	NdrFcShort( 0xb ),	/* 11 */
/* 6248 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6254 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6256 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6264 */	NdrFcShort( 0x4 ),	/* 4 */

	/* Parameter newVal */

/* 6266 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6268 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6270 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6274 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopTime */

/* 6278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6284 */	NdrFcShort( 0xc ),	/* 12 */
/* 6286 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6290 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6292 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6294 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6302 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6304 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6306 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6308 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6310 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6312 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopTime */

/* 6316 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6318 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6322 */	NdrFcShort( 0xd ),	/* 13 */
/* 6324 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6326 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6328 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6330 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6332 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6340 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6344 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6346 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6350 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_State */


	/* Procedure get_State */

/* 6354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6360 */	NdrFcShort( 0xe ),	/* 14 */
/* 6362 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6366 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6368 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6370 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6378 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 6380 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6382 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6384 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6388 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_State */


	/* Procedure put_State */

/* 6392 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6398 */	NdrFcShort( 0xf ),	/* 15 */
/* 6400 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6406 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6408 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6416 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 6418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6420 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6422 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6424 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6426 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 6430 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6436 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6438 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6442 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6444 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6446 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6450 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6454 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStart */

/* 6456 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6458 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 6462 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6464 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6470 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPositions */

/* 6474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6480 */	NdrFcShort( 0x11 ),	/* 17 */
/* 6482 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6484 */	NdrFcShort( 0x1c ),	/* 28 */
/* 6486 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6488 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6490 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6498 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pPos */

/* 6500 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 6502 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 6506 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6508 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6512 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6514 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6516 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPositions */

/* 6518 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6520 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6524 */	NdrFcShort( 0x12 ),	/* 18 */
/* 6526 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6528 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6530 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6532 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6534 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6542 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter llStart */

/* 6544 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6546 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6548 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter llStop */

/* 6550 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6552 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6554 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6558 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CheckCapabilities */

/* 6562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6568 */	NdrFcShort( 0x14 ),	/* 20 */
/* 6570 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6572 */	NdrFcShort( 0x1c ),	/* 28 */
/* 6574 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6576 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6578 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6586 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCaps */

/* 6588 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 6590 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6594 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6596 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6598 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopPosition */

/* 6600 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6602 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6606 */	NdrFcShort( 0xa ),	/* 10 */
/* 6608 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6610 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6612 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6614 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6616 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6618 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6624 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6626 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6628 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6630 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6632 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6634 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Duration */

/* 6638 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6640 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6644 */	NdrFcShort( 0xb ),	/* 11 */
/* 6646 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6650 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6652 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6654 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6662 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6664 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6666 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6668 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6672 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 6676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6682 */	NdrFcShort( 0xc ),	/* 12 */
/* 6684 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6688 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6690 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6692 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6700 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6702 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6704 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6706 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6708 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6710 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 6714 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6720 */	NdrFcShort( 0xd ),	/* 13 */
/* 6722 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6726 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6728 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6730 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6738 */	NdrFcShort( 0x4 ),	/* 4 */

	/* Parameter newVal */

/* 6740 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6742 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6744 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6746 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6748 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_TimeFormat */

/* 6752 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6754 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6758 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6760 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6764 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6766 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6768 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6776 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6778 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6780 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6782 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6784 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6786 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TimeFormat */

/* 6790 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6796 */	NdrFcShort( 0x11 ),	/* 17 */
/* 6798 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6800 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6804 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6806 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6808 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6814 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6816 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6818 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6820 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6824 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ConvertTimeFormat */

/* 6828 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6834 */	NdrFcShort( 0x12 ),	/* 18 */
/* 6836 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 6838 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6840 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6842 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 6844 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6852 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter srcVal */

/* 6854 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6856 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6858 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter srcFormat */

/* 6860 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6862 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6864 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstFormat */

/* 6866 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6868 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6870 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstVal */

/* 6872 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6874 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6876 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6878 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6880 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 6882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 6884 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6886 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6890 */	NdrFcShort( 0x13 ),	/* 19 */
/* 6892 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6894 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6896 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6898 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6900 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6908 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lEarliest */

/* 6910 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6912 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6914 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter lLatest */

/* 6916 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6918 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6920 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6922 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6924 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6926 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetShiftPosition */

/* 6928 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6930 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6934 */	NdrFcShort( 0x14 ),	/* 20 */
/* 6936 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 6938 */	NdrFcShort( 0x28 ),	/* 40 */
/* 6940 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6942 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 6944 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6952 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6954 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6956 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 6958 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter shift */

/* 6960 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6962 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 6964 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter timeFormat */

/* 6966 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6968 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 6970 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter pos */

/* 6972 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6974 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6976 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6978 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6980 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 6982 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHoldRange */

/* 6984 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6986 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6990 */	NdrFcShort( 0x15 ),	/* 21 */
/* 6992 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 6994 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6996 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6998 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7000 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7008 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 7010 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7012 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7014 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 7016 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7018 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7020 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7022 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7024 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHoldRange */

/* 7028 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7030 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7034 */	NdrFcShort( 0x16 ),	/* 22 */
/* 7036 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7040 */	NdrFcShort( 0x50 ),	/* 80 */
/* 7042 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7044 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7050 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7052 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 7054 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7056 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7058 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 7060 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7062 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7064 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7066 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7068 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7070 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTimeFormatSupported */

/* 7072 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7074 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7078 */	NdrFcShort( 0x17 ),	/* 23 */
/* 7080 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7082 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7084 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7086 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7088 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7096 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter format */

/* 7098 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7100 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7102 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter support */

/* 7104 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7106 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7110 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7112 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContinuousInterval */

/* 7116 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7122 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7124 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7126 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7128 */	NdrFcShort( 0x50 ),	/* 80 */
/* 7130 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7132 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7140 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter position */

/* 7142 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7144 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7146 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter start */

/* 7148 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7150 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7152 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter end */

/* 7154 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7156 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7158 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7160 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7162 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSelection */

/* 7166 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7172 */	NdrFcShort( 0x19 ),	/* 25 */
/* 7174 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7176 */	NdrFcShort( 0x28 ),	/* 40 */
/* 7178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7180 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7182 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7190 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 7192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7194 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7196 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 7198 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7200 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7202 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter mode */

/* 7204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7206 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7208 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 7210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7212 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBufferSize */

/* 7216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7222 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7224 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7228 */	NdrFcShort( 0x5c ),	/* 92 */
/* 7230 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7232 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7240 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter total */

/* 7242 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7244 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter used */

/* 7248 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7250 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter writePos */

/* 7254 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7256 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7260 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7262 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */

/* 7266 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7274 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7278 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7280 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 7282 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7290 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 7292 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7294 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Timeout */


	/* Procedure SetBufferSize */

/* 7298 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7304 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7306 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7310 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7312 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7314 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7322 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter cbSize */

/* 7324 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7326 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 7330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7332 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveData */


	/* Procedure SaveData */

/* 7336 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7342 */	NdrFcShort( 0xa ),	/* 10 */
/* 7344 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7350 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 7352 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 7354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7356 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7360 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter strFileName */


	/* Parameter strFileName */

/* 7362 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7364 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7366 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Return value */


	/* Return value */

/* 7368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7370 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBufferSize */

/* 7374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7380 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7382 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7386 */	NdrFcShort( 0x74 ),	/* 116 */
/* 7388 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7390 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7398 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter total */

/* 7400 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7402 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7404 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter used */

/* 7406 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7408 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7410 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter writePos */

/* 7412 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7414 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7416 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7420 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetBufferSize */

/* 7424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7430 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7432 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7434 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7436 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7438 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7440 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7448 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cbSize */

/* 7450 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7452 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7454 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7458 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */

/* 7462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7468 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7470 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7474 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7476 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7478 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7486 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */

/* 7488 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7490 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 7494 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7496 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7502 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */

/* 7506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7512 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7514 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7518 */	NdrFcShort( 0x38 ),	/* 56 */
/* 7520 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7522 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 7532 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 7534 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7536 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */

/* 7538 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7540 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */

/* 7544 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7550 */	NdrFcShort( 0xa ),	/* 10 */
/* 7552 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7556 */	NdrFcShort( 0x38 ),	/* 56 */
/* 7558 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7560 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7568 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 7570 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 7572 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7574 */	NdrFcShort( 0x6a8 ),	/* Type Offset=1704 */

	/* Return value */

/* 7576 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7578 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSampleVolume */

/* 7582 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7584 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7588 */	NdrFcShort( 0xb ),	/* 11 */
/* 7590 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7592 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7594 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7596 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7598 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7606 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */

/* 7608 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7610 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 7614 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7616 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter line */

/* 7620 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7622 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7626 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7628 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsSupported */

/* 7632 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7634 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7638 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7640 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7644 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7646 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7648 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7656 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 7658 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7660 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7662 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter supported */

/* 7664 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7666 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7672 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIndex */

/* 7676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7682 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7684 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7686 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7688 */	NdrFcShort( 0x34 ),	/* 52 */
/* 7690 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7692 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7700 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 7702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7704 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7706 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter value */

/* 7708 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 7710 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7712 */	NdrFcShort( 0x5f6 ),	/* Type Offset=1526 */

	/* Return value */

/* 7714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7716 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */

/* 7720 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7722 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7726 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7728 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7732 */	NdrFcShort( 0x34 ),	/* 52 */
/* 7734 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7736 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7744 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 7746 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 7748 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7750 */	NdrFcShort( 0x70e ),	/* Type Offset=1806 */

	/* Return value */

/* 7752 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7754 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 7758 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7764 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7766 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7768 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7770 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7772 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7774 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7778 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7782 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 7784 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7786 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7788 */	NdrFcShort( 0x70e ),	/* Type Offset=1806 */

	/* Return value */

/* 7790 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7792 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7794 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FileName */

/* 7796 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7798 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7804 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7808 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7810 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 7812 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 7814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7816 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7820 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 7822 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7824 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7826 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Return value */

/* 7828 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7830 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 7834 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7836 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7840 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7842 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7848 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 7850 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7858 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 7860 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7862 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7864 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataKey */

/* 7866 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7868 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7872 */	NdrFcShort( 0xb ),	/* 11 */
/* 7874 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7876 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7878 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7880 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7882 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7890 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBaseKey */

/* 7892 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7894 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7896 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 7898 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7900 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 7904 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7906 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7908 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Return value */

/* 7910 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7912 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDataKey */

/* 7916 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7918 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7922 */	NdrFcShort( 0xc ),	/* 12 */
/* 7924 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 7926 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7928 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7930 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 7932 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7940 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter baseKey */

/* 7942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7944 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 7946 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 7948 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7950 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 7952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyIndex */

/* 7954 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7956 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 7960 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7962 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 7964 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Return value */

/* 7966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7968 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 7970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CloseDataKey */

/* 7972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7978 */	NdrFcShort( 0xd ),	/* 13 */
/* 7980 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 7982 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7986 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7988 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7996 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7998 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8000 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8002 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Return value */

/* 8004 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8006 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKeyInfo */

/* 8010 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8012 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8016 */	NdrFcShort( 0xe ),	/* 14 */
/* 8018 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8022 */	NdrFcShort( 0x40 ),	/* 64 */
/* 8024 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 8026 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8034 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 8036 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8038 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8040 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter subKeys */

/* 8042 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8044 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataValues */

/* 8048 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8050 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8054 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8056 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumValues */

/* 8060 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8062 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8066 */	NdrFcShort( 0xf ),	/* 15 */
/* 8068 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 8070 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8072 */	NdrFcShort( 0x40 ),	/* 64 */
/* 8074 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 8076 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8084 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 8086 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8088 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8090 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter ValueNum */

/* 8092 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8094 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 8098 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8100 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8102 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueSize */

/* 8104 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8106 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8110 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8112 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumSubkeys */

/* 8116 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8118 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8122 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8124 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8126 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8128 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8130 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 8132 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8140 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 8142 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8144 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8146 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter subkeyNum */

/* 8148 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8150 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyId */

/* 8154 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8156 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8160 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8162 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8164 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValue */

/* 8166 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8172 */	NdrFcShort( 0x11 ),	/* 17 */
/* 8174 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 8176 */	NdrFcShort( 0x20 ),	/* 32 */
/* 8178 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8180 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 8182 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 8184 */	NdrFcShort( 0x1 ),	/* 1 */
/* 8186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8190 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 8192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8194 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8196 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 8198 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8200 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 8204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8206 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8208 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cbBufferSize */

/* 8210 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8212 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 8216 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8218 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8220 */	NdrFcShort( 0x71e ),	/* Type Offset=1822 */

	/* Parameter dataSize */

/* 8222 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8224 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 8226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8230 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 8232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValue */

/* 8234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8240 */	NdrFcShort( 0x12 ),	/* 18 */
/* 8242 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 8244 */	NdrFcShort( 0x20 ),	/* 32 */
/* 8246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8248 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 8250 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 8252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8254 */	NdrFcShort( 0x1 ),	/* 1 */
/* 8256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8258 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 8260 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8262 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8264 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 8266 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8268 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 8272 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8274 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 8278 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 8280 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8282 */	NdrFcShort( 0x738 ),	/* Type Offset=1848 */

	/* Parameter dataSize */

/* 8284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8286 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8292 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 8294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateObjectsDump */

/* 8296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8302 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8304 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8306 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8310 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 8312 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 8314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8316 */	NdrFcShort( 0x1 ),	/* 1 */
/* 8318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fileName */

/* 8322 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 8324 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8326 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Parameter reserved1 */

/* 8328 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8330 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 8334 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8336 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8342 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPlugin */

/* 8346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8352 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8354 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8358 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8360 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8362 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8370 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pPlugin */

/* 8372 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8374 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8376 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter cookie */

/* 8378 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8380 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8386 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ScanModeCaps */

/* 8390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8396 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8398 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 8400 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8402 */	NdrFcShort( 0x40 ),	/* 64 */
/* 8404 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 8406 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8414 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Parameter */

/* 8416 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8418 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ScanMode */

/* 8422 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8424 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Capability */

/* 8428 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8430 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Support */

/* 8434 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8436 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8440 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8442 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurrent */

/* 8446 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8452 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8454 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8458 */	NdrFcShort( 0x48 ),	/* 72 */
/* 8460 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8462 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8470 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 8472 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8474 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8476 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter pVal */

/* 8478 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 8480 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8482 */	NdrFcShort( 0x752 ),	/* Type Offset=1874 */

	/* Return value */

/* 8484 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8486 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCurrent */

/* 8490 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8492 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8498 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8500 */	NdrFcShort( 0x40 ),	/* 64 */
/* 8502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8504 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8506 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8514 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 8516 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8518 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8520 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter pVal */

/* 8522 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 8524 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8526 */	NdrFcShort( 0x752 ),	/* Type Offset=1874 */

	/* Return value */

/* 8528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8530 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValues */

/* 8534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8540 */	NdrFcShort( 0x9 ),	/* 9 */
/* 8542 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8546 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8548 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8550 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8558 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 8560 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8562 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8564 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter pVal */

/* 8566 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8568 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8570 */	NdrFcShort( 0x20c ),	/* Type Offset=524 */

	/* Return value */

/* 8572 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8574 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FilterObj */

/* 8578 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8580 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8584 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8586 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8588 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8590 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8592 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 8594 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8598 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8602 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter FilterIndex */

/* 8604 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8606 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8608 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 8610 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8612 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8614 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */

/* 8616 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8618 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetElementsTestResult */

/* 8622 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8624 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8628 */	NdrFcShort( 0x9 ),	/* 9 */
/* 8630 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 8632 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8634 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8636 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 8638 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8646 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 8648 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8650 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 8652 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 8654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8656 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 8658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStates */

/* 8660 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8662 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 8664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8666 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8668 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 8670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const USgfw2_MIDL_TYPE_FORMAT_STRING USgfw2__MIDL_TypeFormatString =
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
/* 122 */	NdrFcShort( 0x18 ),	/* Offset= 24 (146) */
/* 124 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 126 */	NdrFcShort( 0x2 ),	/* 2 */
/* 128 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 130 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 132 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 134 */	0x0 , 
			0x0,		/* 0 */
/* 136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 144 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 146 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 150 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (124) */
/* 152 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 154 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 156 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x8 ),	/* 8 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (120) */
/* 166 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 168 */	NdrFcShort( 0x2 ),	/* Offset= 2 (170) */
/* 170 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 172 */	NdrFcLong( 0x4cf81935 ),	/* 1291327797 */
/* 176 */	NdrFcShort( 0xdcb3 ),	/* -9037 */
/* 178 */	NdrFcShort( 0x4c19 ),	/* 19481 */
/* 180 */	0x9d,		/* 157 */
			0xc9,		/* 201 */
/* 182 */	0x76,		/* 118 */
			0x53,		/* 83 */
/* 184 */	0x6,		/* 6 */
			0xa4,		/* 164 */
/* 186 */	0x71,		/* 113 */
			0x4f,		/* 79 */
/* 188 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 190 */	NdrFcShort( 0x2 ),	/* Offset= 2 (192) */
/* 192 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 194 */	NdrFcLong( 0xb7c94539 ),	/* -1211546311 */
/* 198 */	NdrFcShort( 0xa65a ),	/* -22950 */
/* 200 */	NdrFcShort( 0x42f3 ),	/* 17139 */
/* 202 */	0x8b,		/* 139 */
			0x65,		/* 101 */
/* 204 */	0xf8,		/* 248 */
			0x3d,		/* 61 */
/* 206 */	0x11,		/* 17 */
			0x4f,		/* 79 */
/* 208 */	0xf4,		/* 244 */
			0xc8,		/* 200 */
/* 210 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 212 */	NdrFcShort( 0x2 ),	/* Offset= 2 (214) */
/* 214 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 226 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 228 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 230 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 232 */	
			0x11, 0x0,	/* FC_RP */
/* 234 */	NdrFcShort( 0x92 ),	/* Offset= 146 (380) */
/* 236 */	
			0x13, 0x0,	/* FC_OP */
/* 238 */	NdrFcShort( 0x2 ),	/* Offset= 2 (240) */
/* 240 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 242 */	NdrFcShort( 0x4 ),	/* 4 */
/* 244 */	NdrFcShort( 0x2 ),	/* 2 */
/* 246 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 250 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 252 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 256 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 258 */	NdrFcShort( 0xffff ),	/* Offset= -1 (257) */
/* 260 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 262 */	NdrFcShort( 0x1 ),	/* 1 */
/* 264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 268 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (236) */
/* 270 */	
			0x13, 0x0,	/* FC_OP */
/* 272 */	NdrFcShort( 0x2 ),	/* Offset= 2 (274) */
/* 274 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 278 */	NdrFcShort( 0x3 ),	/* 3 */
/* 280 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 284 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 286 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 290 */	NdrFcShort( 0xa ),	/* Offset= 10 (300) */
/* 292 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 296 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 298 */	NdrFcShort( 0xffff ),	/* Offset= -1 (297) */
/* 300 */	
			0x13, 0x0,	/* FC_OP */
/* 302 */	NdrFcShort( 0x18 ),	/* Offset= 24 (326) */
/* 304 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 306 */	NdrFcShort( 0x1 ),	/* 1 */
/* 308 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 310 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 312 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 314 */	0x0 , 
			0x0,		/* 0 */
/* 316 */	NdrFcLong( 0x0 ),	/* 0 */
/* 320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 324 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 326 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 328 */	NdrFcShort( 0x18 ),	/* 24 */
/* 330 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (304) */
/* 332 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 334 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 336 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 338 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 340 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 342 */	NdrFcShort( 0x2 ),	/* 2 */
/* 344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0xffb2 ),	/* Offset= -78 (270) */
/* 350 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 352 */	NdrFcShort( 0x20 ),	/* 32 */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* Offset= 0 (356) */
/* 358 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 360 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 362 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 364 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 366 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (340) */
/* 368 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 370 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 372 */	NdrFcShort( 0x10 ),	/* 16 */
/* 374 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 376 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 378 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 380 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 382 */	NdrFcShort( 0x60 ),	/* 96 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x0 ),	/* Offset= 0 (386) */
/* 388 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 390 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 392 */	NdrFcShort( 0xff7c ),	/* Offset= -132 (260) */
/* 394 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 396 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (340) */
/* 398 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 400 */	NdrFcShort( 0xffce ),	/* Offset= -50 (350) */
/* 402 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 404 */	NdrFcShort( 0xffde ),	/* Offset= -34 (370) */
/* 406 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 408 */	NdrFcShort( 0xffda ),	/* Offset= -38 (370) */
/* 410 */	0xa,		/* FC_FLOAT */
			0x8,		/* FC_LONG */
/* 412 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 414 */	
			0x11, 0x0,	/* FC_RP */
/* 416 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (370) */
/* 418 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 420 */	NdrFcShort( 0x2 ),	/* Offset= 2 (422) */
/* 422 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 424 */	NdrFcLong( 0x7bf80981 ),	/* 2079852929 */
/* 428 */	NdrFcShort( 0xbf32 ),	/* -16590 */
/* 430 */	NdrFcShort( 0x101a ),	/* 4122 */
/* 432 */	0x8b,		/* 139 */
			0xbb,		/* 187 */
/* 434 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 436 */	0x0,		/* 0 */
			0x30,		/* 48 */
/* 438 */	0xc,		/* 12 */
			0xab,		/* 171 */
/* 440 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 442 */	NdrFcShort( 0x2 ),	/* Offset= 2 (444) */
/* 444 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 446 */	NdrFcShort( 0x4 ),	/* 4 */
/* 448 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 450 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 452 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 454 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 456 */	NdrFcShort( 0xffaa ),	/* Offset= -86 (370) */
/* 458 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 460 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 462 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 464 */	NdrFcShort( 0x2 ),	/* Offset= 2 (466) */
/* 466 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 468 */	NdrFcLong( 0xbfef8670 ),	/* -1074821520 */
/* 472 */	NdrFcShort( 0xdbc1 ),	/* -9279 */
/* 474 */	NdrFcShort( 0x4b47 ),	/* 19271 */
/* 476 */	0xbc,		/* 188 */
			0x1,		/* 1 */
/* 478 */	0xbb,		/* 187 */
			0xbc,		/* 188 */
/* 480 */	0xa1,		/* 161 */
			0x74,		/* 116 */
/* 482 */	0xdd,		/* 221 */
			0x31,		/* 49 */
/* 484 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 486 */	NdrFcShort( 0x6 ),	/* Offset= 6 (492) */
/* 488 */	
			0x13, 0x0,	/* FC_OP */
/* 490 */	NdrFcShort( 0xfea8 ),	/* Offset= -344 (146) */
/* 492 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 500 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (488) */
/* 502 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 504 */	NdrFcShort( 0x2 ),	/* Offset= 2 (506) */
/* 506 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 518 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 520 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 522 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 524 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 526 */	NdrFcShort( 0x2 ),	/* Offset= 2 (528) */
/* 528 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 530 */	NdrFcLong( 0x95d11d2b ),	/* -1781457621 */
/* 534 */	NdrFcShort( 0xec05 ),	/* -5115 */
/* 536 */	NdrFcShort( 0x4a2e ),	/* 18990 */
/* 538 */	0xb3,		/* 179 */
			0x1b,		/* 27 */
/* 540 */	0x13,		/* 19 */
			0x86,		/* 134 */
/* 542 */	0xc4,		/* 196 */
			0x84,		/* 132 */
/* 544 */	0xae,		/* 174 */
			0x16,		/* 22 */
/* 546 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 548 */	NdrFcShort( 0x476 ),	/* Offset= 1142 (1690) */
/* 550 */	
			0x13, 0x0,	/* FC_OP */
/* 552 */	NdrFcShort( 0x45e ),	/* Offset= 1118 (1670) */
/* 554 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 556 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 558 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 560 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 562 */	0x0 , 
			0x0,		/* 0 */
/* 564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 568 */	NdrFcLong( 0x0 ),	/* 0 */
/* 572 */	NdrFcShort( 0x2 ),	/* Offset= 2 (574) */
/* 574 */	NdrFcShort( 0x10 ),	/* 16 */
/* 576 */	NdrFcShort( 0x2f ),	/* 47 */
/* 578 */	NdrFcLong( 0x14 ),	/* 20 */
/* 582 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 584 */	NdrFcLong( 0x3 ),	/* 3 */
/* 588 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 590 */	NdrFcLong( 0x11 ),	/* 17 */
/* 594 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 596 */	NdrFcLong( 0x2 ),	/* 2 */
/* 600 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 602 */	NdrFcLong( 0x4 ),	/* 4 */
/* 606 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 608 */	NdrFcLong( 0x5 ),	/* 5 */
/* 612 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 614 */	NdrFcLong( 0xb ),	/* 11 */
/* 618 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 620 */	NdrFcLong( 0xa ),	/* 10 */
/* 624 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 626 */	NdrFcLong( 0x6 ),	/* 6 */
/* 630 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (862) */
/* 632 */	NdrFcLong( 0x7 ),	/* 7 */
/* 636 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 638 */	NdrFcLong( 0x8 ),	/* 8 */
/* 642 */	NdrFcShort( 0xff66 ),	/* Offset= -154 (488) */
/* 644 */	NdrFcLong( 0xd ),	/* 13 */
/* 648 */	NdrFcShort( 0xff72 ),	/* Offset= -142 (506) */
/* 650 */	NdrFcLong( 0x9 ),	/* 9 */
/* 654 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (868) */
/* 656 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 660 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (886) */
/* 662 */	NdrFcLong( 0x24 ),	/* 36 */
/* 666 */	NdrFcShort( 0x390 ),	/* Offset= 912 (1578) */
/* 668 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 672 */	NdrFcShort( 0x38a ),	/* Offset= 906 (1578) */
/* 674 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 678 */	NdrFcShort( 0x388 ),	/* Offset= 904 (1582) */
/* 680 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 684 */	NdrFcShort( 0x386 ),	/* Offset= 902 (1586) */
/* 686 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 690 */	NdrFcShort( 0x384 ),	/* Offset= 900 (1590) */
/* 692 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 696 */	NdrFcShort( 0x382 ),	/* Offset= 898 (1594) */
/* 698 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 702 */	NdrFcShort( 0x380 ),	/* Offset= 896 (1598) */
/* 704 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 708 */	NdrFcShort( 0x37e ),	/* Offset= 894 (1602) */
/* 710 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 714 */	NdrFcShort( 0x368 ),	/* Offset= 872 (1586) */
/* 716 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 720 */	NdrFcShort( 0x366 ),	/* Offset= 870 (1590) */
/* 722 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 726 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1606) */
/* 728 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 732 */	NdrFcShort( 0x366 ),	/* Offset= 870 (1602) */
/* 734 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 738 */	NdrFcShort( 0x368 ),	/* Offset= 872 (1610) */
/* 740 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 744 */	NdrFcShort( 0x366 ),	/* Offset= 870 (1614) */
/* 746 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 750 */	NdrFcShort( 0x364 ),	/* Offset= 868 (1618) */
/* 752 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 756 */	NdrFcShort( 0x374 ),	/* Offset= 884 (1640) */
/* 758 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 762 */	NdrFcShort( 0x372 ),	/* Offset= 882 (1644) */
/* 764 */	NdrFcLong( 0x10 ),	/* 16 */
/* 768 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 770 */	NdrFcLong( 0x12 ),	/* 18 */
/* 774 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 776 */	NdrFcLong( 0x13 ),	/* 19 */
/* 780 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 782 */	NdrFcLong( 0x15 ),	/* 21 */
/* 786 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 788 */	NdrFcLong( 0x16 ),	/* 22 */
/* 792 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 794 */	NdrFcLong( 0x17 ),	/* 23 */
/* 798 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 800 */	NdrFcLong( 0xe ),	/* 14 */
/* 804 */	NdrFcShort( 0x350 ),	/* Offset= 848 (1652) */
/* 806 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 810 */	NdrFcShort( 0x354 ),	/* Offset= 852 (1662) */
/* 812 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 816 */	NdrFcShort( 0x352 ),	/* Offset= 850 (1666) */
/* 818 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 822 */	NdrFcShort( 0x2fc ),	/* Offset= 764 (1586) */
/* 824 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 828 */	NdrFcShort( 0x2fa ),	/* Offset= 762 (1590) */
/* 830 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 834 */	NdrFcShort( 0x2f8 ),	/* Offset= 760 (1594) */
/* 836 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 840 */	NdrFcShort( 0x2ee ),	/* Offset= 750 (1590) */
/* 842 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 846 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (1590) */
/* 848 */	NdrFcLong( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x0 ),	/* Offset= 0 (852) */
/* 854 */	NdrFcLong( 0x1 ),	/* 1 */
/* 858 */	NdrFcShort( 0x0 ),	/* Offset= 0 (858) */
/* 860 */	NdrFcShort( 0xffff ),	/* Offset= -1 (859) */
/* 862 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 866 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 868 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 870 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 880 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 882 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 884 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 886 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 888 */	NdrFcShort( 0x2 ),	/* Offset= 2 (890) */
/* 890 */	
			0x13, 0x0,	/* FC_OP */
/* 892 */	NdrFcShort( 0x29c ),	/* Offset= 668 (1560) */
/* 894 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 896 */	NdrFcShort( 0x20 ),	/* 32 */
/* 898 */	NdrFcShort( 0xa ),	/* 10 */
/* 900 */	NdrFcLong( 0x8 ),	/* 8 */
/* 904 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1004) */
/* 906 */	NdrFcLong( 0xd ),	/* 13 */
/* 910 */	NdrFcShort( 0x98 ),	/* Offset= 152 (1062) */
/* 912 */	NdrFcLong( 0x9 ),	/* 9 */
/* 916 */	NdrFcShort( 0xcc ),	/* Offset= 204 (1120) */
/* 918 */	NdrFcLong( 0xc ),	/* 12 */
/* 922 */	NdrFcShort( 0x100 ),	/* Offset= 256 (1178) */
/* 924 */	NdrFcLong( 0x24 ),	/* 36 */
/* 928 */	NdrFcShort( 0x170 ),	/* Offset= 368 (1296) */
/* 930 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 934 */	NdrFcShort( 0x1a4 ),	/* Offset= 420 (1354) */
/* 936 */	NdrFcLong( 0x10 ),	/* 16 */
/* 940 */	NdrFcShort( 0x1c8 ),	/* Offset= 456 (1396) */
/* 942 */	NdrFcLong( 0x2 ),	/* 2 */
/* 946 */	NdrFcShort( 0x1e8 ),	/* Offset= 488 (1434) */
/* 948 */	NdrFcLong( 0x3 ),	/* 3 */
/* 952 */	NdrFcShort( 0x208 ),	/* Offset= 520 (1472) */
/* 954 */	NdrFcLong( 0x14 ),	/* 20 */
/* 958 */	NdrFcShort( 0x228 ),	/* Offset= 552 (1510) */
/* 960 */	NdrFcShort( 0xffff ),	/* Offset= -1 (959) */
/* 962 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 966 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 970 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 972 */	0x0 , 
			0x0,		/* 0 */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcLong( 0x0 ),	/* 0 */
/* 982 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 986 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 988 */	0x0 , 
			0x0,		/* 0 */
/* 990 */	NdrFcLong( 0x0 ),	/* 0 */
/* 994 */	NdrFcLong( 0x0 ),	/* 0 */
/* 998 */	
			0x13, 0x0,	/* FC_OP */
/* 1000 */	NdrFcShort( 0xfcaa ),	/* Offset= -854 (146) */
/* 1002 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1004 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1006 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1016) */
/* 1012 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1014 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1016 */	
			0x11, 0x0,	/* FC_RP */
/* 1018 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (962) */
/* 1020 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1022 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1024 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1030 */	0x0 , 
			0x0,		/* 0 */
/* 1032 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1036 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1040 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1044 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1046 */	0x0 , 
			0x0,		/* 0 */
/* 1048 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1056 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1058 */	NdrFcShort( 0xfdd8 ),	/* Offset= -552 (506) */
/* 1060 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1062 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1064 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1068 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1074) */
/* 1070 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1072 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1074 */	
			0x11, 0x0,	/* FC_RP */
/* 1076 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1020) */
/* 1078 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1082 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1088 */	0x0 , 
			0x0,		/* 0 */
/* 1090 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1098 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1102 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1104 */	0x0 , 
			0x0,		/* 0 */
/* 1106 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1114 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1116 */	NdrFcShort( 0xff08 ),	/* Offset= -248 (868) */
/* 1118 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1120 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1122 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1126 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1132) */
/* 1128 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1130 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1132 */	
			0x11, 0x0,	/* FC_RP */
/* 1134 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1078) */
/* 1136 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1146 */	0x0 , 
			0x0,		/* 0 */
/* 1148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1156 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1160 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1162 */	0x0 , 
			0x0,		/* 0 */
/* 1164 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1168 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1172 */	
			0x13, 0x0,	/* FC_OP */
/* 1174 */	NdrFcShort( 0x1f0 ),	/* Offset= 496 (1670) */
/* 1176 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1178 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1180 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1184 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1190) */
/* 1186 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1188 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1190 */	
			0x11, 0x0,	/* FC_RP */
/* 1192 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1136) */
/* 1194 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1196 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1206 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1208 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1210 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1212 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1214 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1216 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1220 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1222 */	0x0 , 
			0x0,		/* 0 */
/* 1224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1228 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1232 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1234 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1236 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1240 */	NdrFcShort( 0xa ),	/* Offset= 10 (1250) */
/* 1242 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1244 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1246 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (1194) */
/* 1248 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1250 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1252 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (1212) */
/* 1254 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1258 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1262 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1264 */	0x0 , 
			0x0,		/* 0 */
/* 1266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1270 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1274 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1278 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1280 */	0x0 , 
			0x0,		/* 0 */
/* 1282 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1290 */	
			0x13, 0x0,	/* FC_OP */
/* 1292 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (1234) */
/* 1294 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1296 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1298 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1308) */
/* 1304 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1306 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1308 */	
			0x11, 0x0,	/* FC_RP */
/* 1310 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1254) */
/* 1312 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1316 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1320 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1322 */	0x0 , 
			0x0,		/* 0 */
/* 1324 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1328 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1332 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1336 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1338 */	0x0 , 
			0x0,		/* 0 */
/* 1340 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1348 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1350 */	NdrFcShort( 0xfb90 ),	/* Offset= -1136 (214) */
/* 1352 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1354 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1356 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1360 */	NdrFcShort( 0xa ),	/* Offset= 10 (1370) */
/* 1362 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1364 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1366 */	0x0,		/* 0 */
			NdrFcShort( 0xfaff ),	/* Offset= -1281 (86) */
			0x5b,		/* FC_END */
/* 1370 */	
			0x11, 0x0,	/* FC_RP */
/* 1372 */	NdrFcShort( 0xffc4 ),	/* Offset= -60 (1312) */
/* 1374 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1376 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1378 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1382 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1384 */	0x0 , 
			0x0,		/* 0 */
/* 1386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1390 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1394 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1396 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1398 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1402 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1408) */
/* 1404 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1406 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1408 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1410 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1374) */
/* 1412 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1414 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1416 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1420 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1422 */	0x0 , 
			0x0,		/* 0 */
/* 1424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1428 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1432 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1434 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1436 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1440 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1446) */
/* 1442 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1444 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1446 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1448 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1412) */
/* 1450 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1452 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1454 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1458 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1460 */	0x0 , 
			0x0,		/* 0 */
/* 1462 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1466 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1470 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1472 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1474 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1484) */
/* 1480 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1482 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1484 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1486 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1450) */
/* 1488 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1492 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1498 */	0x0 , 
			0x0,		/* 0 */
/* 1500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1504 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1508 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1510 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1512 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1516 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1522) */
/* 1518 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1520 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1522 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1524 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1488) */
/* 1526 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1530 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1532 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1534 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1538 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1540 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 1542 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1544 */	0x0 , 
			0x0,		/* 0 */
/* 1546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1550 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1554 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1556 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1526) */
/* 1558 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1560 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1562 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1564 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1534) */
/* 1566 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1566) */
/* 1568 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1570 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1572 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1574 */	0x0,		/* 0 */
			NdrFcShort( 0xfd57 ),	/* Offset= -681 (894) */
			0x5b,		/* FC_END */
/* 1578 */	
			0x13, 0x0,	/* FC_OP */
/* 1580 */	NdrFcShort( 0xfea6 ),	/* Offset= -346 (1234) */
/* 1582 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1584 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1586 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1588 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1590 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1592 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1594 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1596 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1598 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1600 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1602 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1604 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1606 */	
			0x13, 0x0,	/* FC_OP */
/* 1608 */	NdrFcShort( 0xfd16 ),	/* Offset= -746 (862) */
/* 1610 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1612 */	NdrFcShort( 0xfb9c ),	/* Offset= -1124 (488) */
/* 1614 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1616 */	NdrFcShort( 0xfa86 ),	/* Offset= -1402 (214) */
/* 1618 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1620 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1622) */
/* 1622 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1624 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 1628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1632 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1634 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1636 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1638 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1640 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1642 */	NdrFcShort( 0xfd0c ),	/* Offset= -756 (886) */
/* 1644 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1646 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1648) */
/* 1648 */	
			0x13, 0x0,	/* FC_OP */
/* 1650 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1670) */
/* 1652 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1654 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1656 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1658 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1660 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1662 */	
			0x13, 0x0,	/* FC_OP */
/* 1664 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1652) */
/* 1666 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1668 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1670 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1672 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1676 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1676) */
/* 1678 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1680 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1682 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1684 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1686 */	NdrFcShort( 0xfb94 ),	/* Offset= -1132 (554) */
/* 1688 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1690 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1692 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1694 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1698 */	NdrFcShort( 0xfb84 ),	/* Offset= -1148 (550) */
/* 1700 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1702 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1704) */
/* 1704 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1706 */	NdrFcShort( 0xc ),	/* 12 */
/* 1708 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1710 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1712 */	
			0x11, 0x0,	/* FC_RP */
/* 1714 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1716) */
/* 1716 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1718 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1720 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1722 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1724 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1726 */	0x0 , 
			0x0,		/* 0 */
/* 1728 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1732 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1736 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1738 */	
			0x11, 0x0,	/* FC_RP */
/* 1740 */	NdrFcShort( 0xfaf0 ),	/* Offset= -1296 (444) */
/* 1742 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1744 */	NdrFcShort( 0xff26 ),	/* Offset= -218 (1526) */
/* 1746 */	
			0x11, 0x0,	/* FC_RP */
/* 1748 */	NdrFcShort( 0xff22 ),	/* Offset= -222 (1526) */
/* 1750 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1752 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1754 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1756 */	NdrFcLong( 0x5748ca80 ),	/* 1464388224 */
/* 1760 */	NdrFcShort( 0x1710 ),	/* 5904 */
/* 1762 */	NdrFcShort( 0x489f ),	/* 18591 */
/* 1764 */	0xbc,		/* 188 */
			0x13,		/* 19 */
/* 1766 */	0x28,		/* 40 */
			0xf2,		/* 242 */
/* 1768 */	0xc0,		/* 192 */
			0x12,		/* 18 */
/* 1770 */	0x2b,		/* 43 */
			0x49,		/* 73 */
/* 1772 */	
			0x11, 0x0,	/* FC_RP */
/* 1774 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1776) */
/* 1776 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1778 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1780 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1782 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1784 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1786 */	0x0 , 
			0x0,		/* 0 */
/* 1788 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1796 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1798 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1800 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1802 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1804 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1806) */
/* 1806 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1808 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1810 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 1812 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1814 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1816 */	0xb8,		/* FC_INT3264 */
			0x5c,		/* FC_PAD */
/* 1818 */	
			0x11, 0x0,	/* FC_RP */
/* 1820 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1822) */
/* 1822 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1824 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1826 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1828 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1830 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1832 */	0x0 , 
			0x0,		/* 0 */
/* 1834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1838 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1842 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1844 */	
			0x11, 0x0,	/* FC_RP */
/* 1846 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1848) */
/* 1848 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1850 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1852 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1854 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1856 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1858 */	0x0 , 
			0x0,		/* 0 */
/* 1860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1864 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1868 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1870 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1872 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1874) */
/* 1874 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1876 */	NdrFcShort( 0xc ),	/* 12 */
/* 1878 */	0xe,		/* FC_ENUM32 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1880 */	0x0,		/* 0 */
			NdrFcShort( 0xfe9d ),	/* Offset= -355 (1526) */
			0x5b,		/* FC_END */
/* 1884 */	
			0x11, 0x0,	/* FC_RP */
/* 1886 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1874) */

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



/* Standard interface: __MIDL_itf_USgfw2_0000_0000, ver. 0.0,
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
    38,
    76,
    114,
    152,
    190,
    234,
    272,
    328
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
    378,
    428,
    484,
    522,
    560,
    598,
    636
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
    674,
    718,
    756,
    794,
    832
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
    870,
    908,
    946,
    984,
    1022,
    1060,
    1098,
    1136,
    1180,
    1218,
    1256,
    1294,
    1332
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



/* Object interface: IUsgDeviceChangeSink, ver. 0.0,
   GUID={0x9717780E,0xAAAF,0x4FD2,{0x83,0x5A,0x80,0x91,0x0E,0x1E,0x80,0x3E}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgDeviceChangeSink_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1370,
    1414,
    1458,
    1502,
    1546,
    1590
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
    38,
    1634
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
    1678,
    1716,
    1754,
    1798,
    1830,
    1874,
    1918,
    1962,
    328
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
    2006
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
    2050,
    2088,
    76,
    2126,
    2164
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
    2050,
    2088,
    76,
    2126,
    2164,
    2202,
    1918
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
    2050,
    2088,
    76,
    2126,
    2240,
    2272,
    1098,
    2304
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
    2342
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
    38,
    2386
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
    2424
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    76,
    114
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
    38,
    2386,
    756,
    2468
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386,
    2506,
    2550,
    2594,
    2638,
    2688,
    2744,
    2800,
    2856
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
    2912,
    2962,
    3012,
    2506,
    3050,
    2594,
    3094,
    2304,
    3138,
    3176,
    3220
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
    38,
    2386,
    756,
    2468
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
    38,
    3012,
    522,
    2164,
    3258,
    1098,
    3296,
    3334,
    3372,
    3410,
    3448,
    3486,
    3524,
    3562,
    3612,
    3662
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
    3712,
    3762
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
    3812,
    3862,
    3906,
    3950,
    2164,
    3258,
    1098,
    3296,
    3988,
    4050,
    4112,
    4168,
    4230,
    4280
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
    4336
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
    4380,
    4448
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
    38,
    2386,
    4516,
    4566,
    4622
    };



/* Object interface: IUsgScanLine2, ver. 0.0,
   GUID={0xB521CD25,0xEC30,0x486b,{0xB1,0xB7,0xBE,0x22,0x97,0x35,0xAA,0xBD}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanLine2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    38,
    2386,
    4516,
    4566,
    4622,
    4696,
    4752,
    4814
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
    4894,
    4950,
    5024,
    5074,
    5118
    };



/* Object interface: IUsgScanLineProperties2, ver. 0.0,
   GUID={0xA4F7329D,0x51A7,0x4a61,{0xA9,0xA8,0xCF,0xE9,0x0A,0x90,0xC9,0x04}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanLineProperties2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4894,
    4950,
    5024,
    5074,
    5118,
    5168,
    5230,
    5310,
    5366
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
    38,
    2386
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
    5422,
    5460,
    2386
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
    5498,
    3012
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
    38,
    2386,
    756,
    2468,
    5536
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
    5592,
    3762
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
    38,
    2386,
    756,
    2468
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386,
    2506,
    3050
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
    5648,
    5704,
    5760,
    5804,
    5848,
    5892,
    5936,
    5974,
    6012,
    6050
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
    38,
    2386,
    2506,
    3050
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    6088,
    6126,
    6164,
    6202,
    6240,
    6278,
    6316,
    6354,
    6392,
    6430,
    6474,
    6518,
    3486,
    6562
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
    6088,
    6126,
    6164,
    6600,
    6638,
    6676,
    6714,
    6354,
    6392,
    6752,
    6790,
    6828,
    6884,
    6928,
    6984,
    7028,
    7072,
    7116,
    7166
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
    7216,
    7266,
    7298,
    7336
    };



/* Object interface: IUsgCineSink2, ver. 0.0,
   GUID={0x9EB34740,0x8AA5,0x4373,{0xA1,0xFF,0xAC,0x9A,0x86,0x4D,0x5A,0xBB}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCineSink2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    7374,
    7266,
    7424,
    7336
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
    38,
    2386,
    756,
    2468
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
    4336,
    7462,
    7506,
    7544,
    7582,
    5892
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
    38,
    2386
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
    1678
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
    1678,
    5498,
    3012
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
    38,
    2386
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
    38,
    2386,
    756,
    2468
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
    38,
    2386
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
    7632,
    7676
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
    7720,
    7758,
    2386
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
    38,
    2386,
    756,
    2468
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
    38,
    2386
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
    38,
    2386
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
    2050,
    7796,
    7834,
    522,
    7866,
    7916,
    7972,
    8010,
    8060,
    8116,
    8166,
    8234
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
    8296
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
    8346,
    38
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
    5422,
    5498,
    3012,
    756,
    794
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    38,
    2386
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
    8390
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
    8446,
    8490,
    8534
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
    8578
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
    5648,
    5704,
    5760,
    5804,
    5848,
    5892,
    5936,
    5974,
    6012,
    6050
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
    38
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
    38,
    2386
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
    38,
    2386
    };



/* Object interface: IUsgFrameROI, ver. 0.0,
   GUID={0x525055A7,0xB4AD,0x4A89,{0x85,0xE8,0x8F,0xCC,0xF5,0x1F,0x9D,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgFrameROI_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5648,
    5704,
    5760,
    5804,
    5848,
    5892,
    5936,
    5974,
    6012,
    6050
    };



/* Object interface: IUsgProbeElementsTest, ver. 0.0,
   GUID={0x06AF0C84,0xB1E3,0x411a,{0x93,0x63,0xE7,0x32,0xD1,0x65,0x9B,0xC0}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgProbeElementsTest_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    5498,
    8622
    };



/* Object interface: IUsgTissueMotionDetector, ver. 0.0,
   GUID={0x7C9EB8CB,0x04DF,0x4F02,{0xA2,0x54,0xE3,0xF6,0x3E,0xFE,0xD9,0xB5}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgTissueMotionDetector_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    5498,
    7298,
    756,
    2468
    };



#endif /* defined(_M_AMD64)*/



/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for USgfw2.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)



extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif


#include "ndr64types.h"
#include "pshpack8.h"


typedef 
NDR64_FORMAT_CHAR
__midl_frag2094_t;
extern const __midl_frag2094_t __midl_frag2094;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag2092_t;
extern const __midl_frag2092_t __midl_frag2092;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag2089_t;
extern const __midl_frag2089_t __midl_frag2089;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag2088_t;
extern const __midl_frag2088_t __midl_frag2088;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag2071_t;
extern const __midl_frag2071_t __midl_frag2071;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag2061_t;
extern const __midl_frag2061_t __midl_frag2061;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag2060_t;
extern const __midl_frag2060_t __midl_frag2060;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag2045_t;
extern const __midl_frag2045_t __midl_frag2045;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag2035_t;
extern const __midl_frag2035_t __midl_frag2035;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag2021_t;
extern const __midl_frag2021_t __midl_frag2021;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag2015_t;
extern const __midl_frag2015_t __midl_frag2015;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag2013_t;
extern const __midl_frag2013_t __midl_frag2013;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag2012_t;
extern const __midl_frag2012_t __midl_frag2012;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag2011_t;
extern const __midl_frag2011_t __midl_frag2011;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag2010_t;
extern const __midl_frag2010_t __midl_frag2010;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag1934_t;
extern const __midl_frag1934_t __midl_frag1934;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1933_t;
extern const __midl_frag1933_t __midl_frag1933;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1932_t;
extern const __midl_frag1932_t __midl_frag1932;

typedef 
NDR64_FORMAT_CHAR
__midl_frag1931_t;
extern const __midl_frag1931_t __midl_frag1931;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1930_t;
extern const __midl_frag1930_t __midl_frag1930;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1923_t;
extern const __midl_frag1923_t __midl_frag1923;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1921_t;
extern const __midl_frag1921_t __midl_frag1921;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1918_t;
extern const __midl_frag1918_t __midl_frag1918;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag1916_t;
extern const __midl_frag1916_t __midl_frag1916;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1912_t;
extern const __midl_frag1912_t __midl_frag1912;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1904_t;
extern const __midl_frag1904_t __midl_frag1904;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1815_t;
extern const __midl_frag1815_t __midl_frag1815;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1811_t;
extern const __midl_frag1811_t __midl_frag1811;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag1810_t;
extern const __midl_frag1810_t __midl_frag1810;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1809_t;
extern const __midl_frag1809_t __midl_frag1809;

typedef 
NDR64_FORMAT_CHAR
__midl_frag1806_t;
extern const __midl_frag1806_t __midl_frag1806;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag1805_t;
extern const __midl_frag1805_t __midl_frag1805;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag1804_t;
extern const __midl_frag1804_t __midl_frag1804;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1803_t;
extern const __midl_frag1803_t __midl_frag1803;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
}
__midl_frag1799_t;
extern const __midl_frag1799_t __midl_frag1799;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag1794_t;
extern const __midl_frag1794_t __midl_frag1794;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag1793_t;
extern const __midl_frag1793_t __midl_frag1793;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1792_t;
extern const __midl_frag1792_t __midl_frag1792;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
    struct _NDR64_PARAM_FORMAT frag8;
}
__midl_frag1787_t;
extern const __midl_frag1787_t __midl_frag1787;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1781_t;
extern const __midl_frag1781_t __midl_frag1781;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1773_t;
extern const __midl_frag1773_t __midl_frag1773;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1766_t;
extern const __midl_frag1766_t __midl_frag1766;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1763_t;
extern const __midl_frag1763_t __midl_frag1763;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1760_t;
extern const __midl_frag1760_t __midl_frag1760;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1756_t;
extern const __midl_frag1756_t __midl_frag1756;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1750_t;
extern const __midl_frag1750_t __midl_frag1750;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag1745_t;
extern const __midl_frag1745_t __midl_frag1745;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1741_t;
extern const __midl_frag1741_t __midl_frag1741;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1739_t;
extern const __midl_frag1739_t __midl_frag1739;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag1738_t;
extern const __midl_frag1738_t __midl_frag1738;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1737_t;
extern const __midl_frag1737_t __midl_frag1737;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1736_t;
extern const __midl_frag1736_t __midl_frag1736;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1686_t;
extern const __midl_frag1686_t __midl_frag1686;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag1684_t;
extern const __midl_frag1684_t __midl_frag1684;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1683_t;
extern const __midl_frag1683_t __midl_frag1683;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1682_t;
extern const __midl_frag1682_t __midl_frag1682;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1677_t;
extern const __midl_frag1677_t __midl_frag1677;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1672_t;
extern const __midl_frag1672_t __midl_frag1672;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1616_t;
extern const __midl_frag1616_t __midl_frag1616;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1593_t;
extern const __midl_frag1593_t __midl_frag1593;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1541_t;
extern const __midl_frag1541_t __midl_frag1541;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1524_t;
extern const __midl_frag1524_t __midl_frag1524;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1519_t;
extern const __midl_frag1519_t __midl_frag1519;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1512_t;
extern const __midl_frag1512_t __midl_frag1512;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1501_t;
extern const __midl_frag1501_t __midl_frag1501;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1497_t;
extern const __midl_frag1497_t __midl_frag1497;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1490_t;
extern const __midl_frag1490_t __midl_frag1490;

typedef 
NDR64_FORMAT_CHAR
__midl_frag1461_t;
extern const __midl_frag1461_t __midl_frag1461;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1460_t;
extern const __midl_frag1460_t __midl_frag1460;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1457_t;
extern const __midl_frag1457_t __midl_frag1457;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1456_t;
extern const __midl_frag1456_t __midl_frag1456;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1452_t;
extern const __midl_frag1452_t __midl_frag1452;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1435_t;
extern const __midl_frag1435_t __midl_frag1435;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag1434_t;
extern const __midl_frag1434_t __midl_frag1434;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag1420_t;
extern const __midl_frag1420_t __midl_frag1420;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1196_t;
extern const __midl_frag1196_t __midl_frag1196;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1193_t;
extern const __midl_frag1193_t __midl_frag1193;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1190_t;
extern const __midl_frag1190_t __midl_frag1190;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1187_t;
extern const __midl_frag1187_t __midl_frag1187;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag1182_t;
extern const __midl_frag1182_t __midl_frag1182;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag1181_t;
extern const __midl_frag1181_t __midl_frag1181;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag1180_t;
extern const __midl_frag1180_t __midl_frag1180;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1178_t;
extern const __midl_frag1178_t __midl_frag1178;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1116_t;
extern const __midl_frag1116_t __midl_frag1116;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1109_t;
extern const __midl_frag1109_t __midl_frag1109;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
    struct _NDR64_PARAM_FORMAT frag8;
    struct _NDR64_PARAM_FORMAT frag9;
    struct _NDR64_PARAM_FORMAT frag10;
}
__midl_frag1095_t;
extern const __midl_frag1095_t __midl_frag1095;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
}
__midl_frag1085_t;
extern const __midl_frag1085_t __midl_frag1085;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1079_t;
extern const __midl_frag1079_t __midl_frag1079;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag1067_t;
extern const __midl_frag1067_t __midl_frag1067;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
    struct _NDR64_PARAM_FORMAT frag8;
    struct _NDR64_PARAM_FORMAT frag9;
}
__midl_frag1054_t;
extern const __midl_frag1054_t __midl_frag1054;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag1045_t;
extern const __midl_frag1045_t __midl_frag1045;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag970_t;
extern const __midl_frag970_t __midl_frag970;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag969_t;
extern const __midl_frag969_t __midl_frag969;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
    struct _NDR64_PARAM_FORMAT frag8;
}
__midl_frag964_t;
extern const __midl_frag964_t __midl_frag964;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag956_t;
extern const __midl_frag956_t __midl_frag956;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
    struct _NDR64_PARAM_FORMAT frag8;
}
__midl_frag955_t;
extern const __midl_frag955_t __midl_frag955;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag942_t;
extern const __midl_frag942_t __midl_frag942;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag937_t;
extern const __midl_frag937_t __midl_frag937;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
}
__midl_frag926_t;
extern const __midl_frag926_t __midl_frag926;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag920_t;
extern const __midl_frag920_t __midl_frag920;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
}
__midl_frag911_t;
extern const __midl_frag911_t __midl_frag911;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag886_t;
extern const __midl_frag886_t __midl_frag886;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag885_t;
extern const __midl_frag885_t __midl_frag885;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag880_t;
extern const __midl_frag880_t __midl_frag880;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag876_t;
extern const __midl_frag876_t __midl_frag876;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag870_t;
extern const __midl_frag870_t __midl_frag870;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag860_t;
extern const __midl_frag860_t __midl_frag860;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag846_t;
extern const __midl_frag846_t __midl_frag846;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag840_t;
extern const __midl_frag840_t __midl_frag840;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag714_t;
extern const __midl_frag714_t __midl_frag714;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag713_t;
extern const __midl_frag713_t __midl_frag713;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag712_t;
extern const __midl_frag712_t __midl_frag712;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag708_t;
extern const __midl_frag708_t __midl_frag708;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag699_t;
extern const __midl_frag699_t __midl_frag699;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag692_t;
extern const __midl_frag692_t __midl_frag692;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag685_t;
extern const __midl_frag685_t __midl_frag685;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag521_t;
extern const __midl_frag521_t __midl_frag521;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag517_t;
extern const __midl_frag517_t __midl_frag517;

typedef 
NDR64_FORMAT_CHAR
__midl_frag514_t;
extern const __midl_frag514_t __midl_frag514;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag513_t;
extern const __midl_frag513_t __midl_frag513;

typedef 
NDR64_FORMAT_CHAR
__midl_frag512_t;
extern const __midl_frag512_t __midl_frag512;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag511_t;
extern const __midl_frag511_t __midl_frag511;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag510_t;
extern const __midl_frag510_t __midl_frag510;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag509_t;
extern const __midl_frag509_t __midl_frag509;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag502_t;
extern const __midl_frag502_t __midl_frag502;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag501_t;
extern const __midl_frag501_t __midl_frag501;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag500_t;
extern const __midl_frag500_t __midl_frag500;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag499_t;
extern const __midl_frag499_t __midl_frag499;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag498_t;
extern const __midl_frag498_t __midl_frag498;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag497_t;
extern const __midl_frag497_t __midl_frag497;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag496_t;
extern const __midl_frag496_t __midl_frag496;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag495_t;
extern const __midl_frag495_t __midl_frag495;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag492_t;
extern const __midl_frag492_t __midl_frag492;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag490_t;
extern const __midl_frag490_t __midl_frag490;

typedef 
NDR64_FORMAT_CHAR
__midl_frag489_t;
extern const __midl_frag489_t __midl_frag489;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag488_t;
extern const __midl_frag488_t __midl_frag488;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag487_t;
extern const __midl_frag487_t __midl_frag487;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag479_t;
extern const __midl_frag479_t __midl_frag479;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag471_t;
extern const __midl_frag471_t __midl_frag471;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag470_t;
extern const __midl_frag470_t __midl_frag470;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag469_t;
extern const __midl_frag469_t __midl_frag469;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag467_t;
extern const __midl_frag467_t __midl_frag467;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag466_t;
extern const __midl_frag466_t __midl_frag466;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag465_t;
extern const __midl_frag465_t __midl_frag465;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag464_t;
extern const __midl_frag464_t __midl_frag464;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag461_t;
extern const __midl_frag461_t __midl_frag461;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag460_t;
extern const __midl_frag460_t __midl_frag460;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag459_t;
extern const __midl_frag459_t __midl_frag459;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag456_t;
extern const __midl_frag456_t __midl_frag456;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag455_t;
extern const __midl_frag455_t __midl_frag455;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag454_t;
extern const __midl_frag454_t __midl_frag454;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag451_t;
extern const __midl_frag451_t __midl_frag451;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag450_t;
extern const __midl_frag450_t __midl_frag450;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag449_t;
extern const __midl_frag449_t __midl_frag449;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag448_t;
extern const __midl_frag448_t __midl_frag448;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag443_t;
extern const __midl_frag443_t __midl_frag443;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag5;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag6;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag7;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag8;
    } frag2;
}
__midl_frag442_t;
extern const __midl_frag442_t __midl_frag442;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag441_t;
extern const __midl_frag441_t __midl_frag441;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag438_t;
extern const __midl_frag438_t __midl_frag438;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag437_t;
extern const __midl_frag437_t __midl_frag437;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag436_t;
extern const __midl_frag436_t __midl_frag436;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_NO_REPEAT_FORMAT frag1;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag2;
        struct _NDR64_POINTER_FORMAT frag3;
        struct _NDR64_NO_REPEAT_FORMAT frag4;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag5;
        struct _NDR64_POINTER_FORMAT frag6;
        NDR64_FORMAT_CHAR frag7;
    } frag2;
}
__midl_frag435_t;
extern const __midl_frag435_t __midl_frag435;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag433_t;
extern const __midl_frag433_t __midl_frag433;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag432_t;
extern const __midl_frag432_t __midl_frag432;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag431_t;
extern const __midl_frag431_t __midl_frag431;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag428_t;
extern const __midl_frag428_t __midl_frag428;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag427_t;
extern const __midl_frag427_t __midl_frag427;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag426_t;
extern const __midl_frag426_t __midl_frag426;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag421_t;
extern const __midl_frag421_t __midl_frag421;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag420_t;
extern const __midl_frag420_t __midl_frag420;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag413_t;
extern const __midl_frag413_t __midl_frag413;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag412_t;
extern const __midl_frag412_t __midl_frag412;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag409_t;
extern const __midl_frag409_t __midl_frag409;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag408_t;
extern const __midl_frag408_t __midl_frag408;

typedef 
struct 
{
    struct _NDR64_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    struct _NDR64_UNION_ARM frag6;
    struct _NDR64_UNION_ARM frag7;
    struct _NDR64_UNION_ARM frag8;
    struct _NDR64_UNION_ARM frag9;
    struct _NDR64_UNION_ARM frag10;
    struct _NDR64_UNION_ARM frag11;
    struct _NDR64_UNION_ARM frag12;
    NDR64_UINT32 frag13;
}
__midl_frag407_t;
extern const __midl_frag407_t __midl_frag407;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag405_t;
extern const __midl_frag405_t __midl_frag405;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag404_t;
extern const __midl_frag404_t __midl_frag404;

typedef 
struct 
{
    struct _NDR64_CONF_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_REGION_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag403_t;
extern const __midl_frag403_t __midl_frag403;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag394_t;
extern const __midl_frag394_t __midl_frag394;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag385_t;
extern const __midl_frag385_t __midl_frag385;

typedef 
struct 
{
    struct _NDR64_NON_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    struct _NDR64_UNION_ARM frag6;
    struct _NDR64_UNION_ARM frag7;
    struct _NDR64_UNION_ARM frag8;
    struct _NDR64_UNION_ARM frag9;
    struct _NDR64_UNION_ARM frag10;
    struct _NDR64_UNION_ARM frag11;
    struct _NDR64_UNION_ARM frag12;
    struct _NDR64_UNION_ARM frag13;
    struct _NDR64_UNION_ARM frag14;
    struct _NDR64_UNION_ARM frag15;
    struct _NDR64_UNION_ARM frag16;
    struct _NDR64_UNION_ARM frag17;
    struct _NDR64_UNION_ARM frag18;
    struct _NDR64_UNION_ARM frag19;
    struct _NDR64_UNION_ARM frag20;
    struct _NDR64_UNION_ARM frag21;
    struct _NDR64_UNION_ARM frag22;
    struct _NDR64_UNION_ARM frag23;
    struct _NDR64_UNION_ARM frag24;
    struct _NDR64_UNION_ARM frag25;
    struct _NDR64_UNION_ARM frag26;
    struct _NDR64_UNION_ARM frag27;
    struct _NDR64_UNION_ARM frag28;
    struct _NDR64_UNION_ARM frag29;
    struct _NDR64_UNION_ARM frag30;
    struct _NDR64_UNION_ARM frag31;
    struct _NDR64_UNION_ARM frag32;
    struct _NDR64_UNION_ARM frag33;
    struct _NDR64_UNION_ARM frag34;
    struct _NDR64_UNION_ARM frag35;
    struct _NDR64_UNION_ARM frag36;
    struct _NDR64_UNION_ARM frag37;
    struct _NDR64_UNION_ARM frag38;
    struct _NDR64_UNION_ARM frag39;
    struct _NDR64_UNION_ARM frag40;
    struct _NDR64_UNION_ARM frag41;
    struct _NDR64_UNION_ARM frag42;
    struct _NDR64_UNION_ARM frag43;
    struct _NDR64_UNION_ARM frag44;
    struct _NDR64_UNION_ARM frag45;
    struct _NDR64_UNION_ARM frag46;
    struct _NDR64_UNION_ARM frag47;
    struct _NDR64_UNION_ARM frag48;
    struct _NDR64_UNION_ARM frag49;
    NDR64_UINT32 frag50;
}
__midl_frag384_t;
extern const __midl_frag384_t __midl_frag384;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_REGION_FORMAT frag1;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
    } frag2;
}
__midl_frag383_t;
extern const __midl_frag383_t __midl_frag383;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag381_t;
extern const __midl_frag381_t __midl_frag381;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag380_t;
extern const __midl_frag380_t __midl_frag380;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag378_t;
extern const __midl_frag378_t __midl_frag378;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag316_t;
extern const __midl_frag316_t __midl_frag316;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag283_t;
extern const __midl_frag283_t __midl_frag283;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag277_t;
extern const __midl_frag277_t __midl_frag277;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag276_t;
extern const __midl_frag276_t __midl_frag276;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag275_t;
extern const __midl_frag275_t __midl_frag275;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag271_t;
extern const __midl_frag271_t __midl_frag271;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag265_t;
extern const __midl_frag265_t __midl_frag265;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag243_t;
extern const __midl_frag243_t __midl_frag243;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag242_t;
extern const __midl_frag242_t __midl_frag242;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag241_t;
extern const __midl_frag241_t __midl_frag241;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag238_t;
extern const __midl_frag238_t __midl_frag238;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag209_t;
extern const __midl_frag209_t __midl_frag209;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag177_t;
extern const __midl_frag177_t __midl_frag177;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag176_t;
extern const __midl_frag176_t __midl_frag176;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag174_t;
extern const __midl_frag174_t __midl_frag174;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag172_t;
extern const __midl_frag172_t __midl_frag172;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag170_t;
extern const __midl_frag170_t __midl_frag170;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag153_t;
extern const __midl_frag153_t __midl_frag153;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag152_t;
extern const __midl_frag152_t __midl_frag152;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag151_t;
extern const __midl_frag151_t __midl_frag151;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag150_t;
extern const __midl_frag150_t __midl_frag150;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag148_t;
extern const __midl_frag148_t __midl_frag148;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag146_t;
extern const __midl_frag146_t __midl_frag146;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag144_t;
extern const __midl_frag144_t __midl_frag144;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag143_t;
extern const __midl_frag143_t __midl_frag143;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag138_t;
extern const __midl_frag138_t __midl_frag138;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag137_t;
extern const __midl_frag137_t __midl_frag137;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag134_t;
extern const __midl_frag134_t __midl_frag134;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag133_t;
extern const __midl_frag133_t __midl_frag133;

typedef 
struct 
{
    struct _NDR64_CONF_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag132_t;
extern const __midl_frag132_t __midl_frag132;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag131_t;
extern const __midl_frag131_t __midl_frag131;

typedef 
struct 
{
    struct _NDR64_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    NDR64_UINT32 frag6;
}
__midl_frag129_t;
extern const __midl_frag129_t __midl_frag129;

typedef 
struct 
{
    struct _NDR64_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    NDR64_UINT32 frag5;
}
__midl_frag124_t;
extern const __midl_frag124_t __midl_frag124;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag123_t;
extern const __midl_frag123_t __midl_frag123;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag122_t;
extern const __midl_frag122_t __midl_frag122;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag3;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag4;
        struct _NDR64_BUFFER_ALIGN_FORMAT frag5;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag6;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag7;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag8;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag9;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag10;
        struct _NDR64_MEMPAD_FORMAT frag11;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag12;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag13;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag14;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag15;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag16;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag17;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag18;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag19;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag20;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag21;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag22;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag23;
    } frag2;
}
__midl_frag121_t;
extern const __midl_frag121_t __midl_frag121;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag119_t;
extern const __midl_frag119_t __midl_frag119;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag103_t;
extern const __midl_frag103_t __midl_frag103;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag102_t;
extern const __midl_frag102_t __midl_frag102;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag101_t;
extern const __midl_frag101_t __midl_frag101;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag100_t;
extern const __midl_frag100_t __midl_frag100;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag94_t;
extern const __midl_frag94_t __midl_frag94;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag88_t;
extern const __midl_frag88_t __midl_frag88;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag87_t;
extern const __midl_frag87_t __midl_frag87;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag86_t;
extern const __midl_frag86_t __midl_frag86;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag85_t;
extern const __midl_frag85_t __midl_frag85;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag64_t;
extern const __midl_frag64_t __midl_frag64;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag60_t;
extern const __midl_frag60_t __midl_frag60;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag53_t;
extern const __midl_frag53_t __midl_frag53;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag45_t;
extern const __midl_frag45_t __midl_frag45;

typedef 
struct 
{
    struct _NDR64_CONF_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag44_t;
extern const __midl_frag44_t __midl_frag44;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag30_t;
extern const __midl_frag30_t __midl_frag30;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag29_t;
extern const __midl_frag29_t __midl_frag29;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag28_t;
extern const __midl_frag28_t __midl_frag28;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag27_t;
extern const __midl_frag27_t __midl_frag27;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag21_t;
extern const __midl_frag21_t __midl_frag21;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag19_t;
extern const __midl_frag19_t __midl_frag19;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag18_t;
extern const __midl_frag18_t __midl_frag18;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag17_t;
extern const __midl_frag17_t __midl_frag17;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag16_t;
extern const __midl_frag16_t __midl_frag16;

typedef 
NDR64_FORMAT_UINT32
__midl_frag1_t;
extern const __midl_frag1_t __midl_frag1;

static const __midl_frag2094_t __midl_frag2094 =
0x5    /* FC64_INT32 */;

static const __midl_frag2092_t __midl_frag2092 =
{ 
/* put_EnableDetection */
    { 
    /* put_EnableDetection */      /* procedure put_EnableDetection */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* newVal */      /* parameter newVal */
        &__midl_frag2094,
        { 
        /* newVal */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag2089_t __midl_frag2089 =
{ 
/* *BOOL */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag2094
};

static const __midl_frag2088_t __midl_frag2088 =
{ 
/* get_EnableDetection */
    { 
    /* get_EnableDetection */      /* procedure get_EnableDetection */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag2094,
        { 
        /* pVal */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag2071_t __midl_frag2071 =
{ 
/* GetElementsTestResult */
    { 
    /* GetElementsTestResult */      /* procedure GetElementsTestResult */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startIndex */      /* parameter startIndex */
        &__midl_frag2094,
        { 
        /* startIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* entries */      /* parameter entries */
        &__midl_frag2094,
        { 
        /* entries */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* pStates */      /* parameter pStates */
        &__midl_frag2094,
        { 
        /* pStates */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag2061_t __midl_frag2061 =
{ 
/* *VALUE_STEPPING */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1916
};

static const __midl_frag2060_t __midl_frag2060 =
{ 
/* get_VolumesRange */
    { 
    /* get_VolumesRange */      /* procedure get_VolumesRange */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 60 /* 0x3c */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1916,
        { 
        /* pVal */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag2045_t __midl_frag2045 =
{ 
/* GetVolume */
    { 
    /* GetVolume */      /* procedure GetVolume */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 72 /* 0x48 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag2094,
        { 
        /* startDepth */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag2094,
        { 
        /* endDepth */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag2035_t __midl_frag2035 =
{ 
/* SetVolume */
    { 
    /* SetVolume */      /* procedure SetVolume */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag2094,
        { 
        /* startDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag2094,
        { 
        /* endDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag2021_t __midl_frag2021 =
{ 
/* GetWindow */
    { 
    /* GetWindow */      /* procedure GetWindow */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 136 /* 0x88 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startLine */      /* parameter startLine */
        &__midl_frag2094,
        { 
        /* startLine */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* endLine */      /* parameter endLine */
        &__midl_frag2094,
        { 
        /* endLine */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag2094,
        { 
        /* startDepth */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag2094,
        { 
        /* endDepth */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag2015_t __midl_frag2015 =
{ 
/* SetWindow */
    { 
    /* SetWindow */      /* procedure SetWindow */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startLine */      /* parameter startLine */
        &__midl_frag2094,
        { 
        /* startLine */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* endLine */      /* parameter endLine */
        &__midl_frag2094,
        { 
        /* endLine */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag2094,
        { 
        /* startDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag2094,
        { 
        /* endDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag2013_t __midl_frag2013 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x95d11d2b,
        0xec05,
        0x4a2e,
        {0xb3, 0x1b, 0x13, 0x86, 0xc4, 0x84, 0xae, 0x16}
    }
};

static const __midl_frag2012_t __midl_frag2012 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag2013
};

static const __midl_frag2011_t __midl_frag2011 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag2012
};

static const __midl_frag2010_t __midl_frag2010 =
{ 
/* get_Values */
    { 
    /* get_Values */      /* procedure get_Values */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag2011,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1934_t __midl_frag1934 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00000000,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag1933_t __midl_frag1933 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1934
};

static const __midl_frag1932_t __midl_frag1932 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1933
};

static const __midl_frag1931_t __midl_frag1931 =
0x7    /* FC64_INT64 */;

static const __midl_frag1930_t __midl_frag1930 =
{ 
/* get_FilterObj */
    { 
    /* get_FilterObj */      /* procedure get_FilterObj */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* FilterIndex */      /* parameter FilterIndex */
        &__midl_frag1931,
        { 
        /* FilterIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1932,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1923_t __midl_frag1923 =
{ 
/* GetValues */
    { 
    /* GetValues */      /* procedure GetValues */
        (NDR64_UINT32) 917827 /* 0xe0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pControlObject */      /* parameter pControlObject */
        &__midl_frag1933,
        { 
        /* pControlObject */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag2011,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1921_t __midl_frag1921 =
{ 
/* *USG_UNIT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1916
};

static const __midl_frag1918_t __midl_frag1918 =
{ 
/* SetCurrent */
    { 
    /* SetCurrent */      /* procedure SetCurrent */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 52 /* 0x34 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pControlObject */      /* parameter pControlObject */
        &__midl_frag1933,
        { 
        /* pControlObject */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1916,
        { 
        /* pVal */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1916_t __midl_frag1916 =
{ 
/* USG_UNIT */
    { 
    /* USG_UNIT */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* USG_UNIT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 12 /* 0xc */
    }
};

static const __midl_frag1912_t __midl_frag1912 =
{ 
/* GetCurrent */
    { 
    /* GetCurrent */      /* procedure GetCurrent */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 60 /* 0x3c */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pControlObject */      /* parameter pControlObject */
        &__midl_frag1933,
        { 
        /* pControlObject */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1916,
        { 
        /* pVal */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1904_t __midl_frag1904 =
{ 
/* ScanModeCaps */
    { 
    /* ScanModeCaps */      /* procedure ScanModeCaps */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 72 /* 0x48 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* Parameter */      /* parameter Parameter */
        &__midl_frag2094,
        { 
        /* Parameter */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ScanMode */      /* parameter ScanMode */
        &__midl_frag2094,
        { 
        /* ScanMode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* Capability */      /* parameter Capability */
        &__midl_frag2094,
        { 
        /* Capability */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* Support */      /* parameter Support */
        &__midl_frag2094,
        { 
        /* Support */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1815_t __midl_frag1815 =
{ 
/* AddPlugin */
    { 
    /* AddPlugin */      /* procedure AddPlugin */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pPlugin */      /* parameter pPlugin */
        &__midl_frag1933,
        { 
        /* pPlugin */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* cookie */      /* parameter cookie */
        &__midl_frag2094,
        { 
        /* cookie */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1811_t __midl_frag1811 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag44
};

static const __midl_frag1810_t __midl_frag1810 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag1811
};

static const __midl_frag1809_t __midl_frag1809 =
{ 
/* CreateObjectsDump */
    { 
    /* CreateObjectsDump */      /* procedure CreateObjectsDump */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* fileName */      /* parameter fileName */
        &__midl_frag1810,
        { 
        /* fileName */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* reserved1 */      /* parameter reserved1 */
        &__midl_frag2094,
        { 
        /* reserved1 */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* reserved2 */      /* parameter reserved2 */
        &__midl_frag2094,
        { 
        /* reserved2 */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1806_t __midl_frag1806 =
0x2    /* FC64_INT8 */;

static const __midl_frag1805_t __midl_frag1805 =
{ 
/*  */
    (NDR64_UINT32) 0 /* 0x0 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */  /* Offset */
    }
};

static const __midl_frag1804_t __midl_frag1804 =
{ 
/* *BYTE */
    { 
    /* *BYTE */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *BYTE */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1805
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1806
    }
};

static const __midl_frag1803_t __midl_frag1803 =
{ 
/* *BYTE */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1804
};

static const __midl_frag1799_t __midl_frag1799 =
{ 
/* SetValue */
    { 
    /* SetValue */      /* procedure SetValue */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 56 /* 0x38 */ ,  /* Stack size */
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 6 /* 0x6 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* valueId */      /* parameter valueId */
        &__midl_frag2094,
        { 
        /* valueId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* valueIndex */      /* parameter valueIndex */
        &__midl_frag2094,
        { 
        /* valueIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* dataBuffer */      /* parameter dataBuffer */
        &__midl_frag1804,
        { 
        /* dataBuffer */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* dataSize */      /* parameter dataSize */
        &__midl_frag2094,
        { 
        /* dataSize */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    }
};

static const __midl_frag1794_t __midl_frag1794 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */  /* Offset */
    }
};

static const __midl_frag1793_t __midl_frag1793 =
{ 
/* *BYTE */
    { 
    /* *BYTE */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *BYTE */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1794
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1806
    }
};

static const __midl_frag1792_t __midl_frag1792 =
{ 
/* *BYTE */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1793
};

static const __midl_frag1787_t __midl_frag1787 =
{ 
/* GetValue */
    { 
    /* GetValue */      /* procedure GetValue */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 64 /* 0x40 */ ,  /* Stack size */
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* valueId */      /* parameter valueId */
        &__midl_frag2094,
        { 
        /* valueId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* valueIndex */      /* parameter valueIndex */
        &__midl_frag2094,
        { 
        /* valueIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* cbBufferSize */      /* parameter cbBufferSize */
        &__midl_frag2094,
        { 
        /* cbBufferSize */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* dataBuffer */      /* parameter dataBuffer */
        &__midl_frag1793,
        { 
        /* dataBuffer */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* dataSize */      /* parameter dataSize */
        &__midl_frag2094,
        { 
        /* dataSize */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        56 /* 0x38 */,   /* Stack offset */
    }
};

static const __midl_frag1781_t __midl_frag1781 =
{ 
/* EnumSubkeys */
    { 
    /* EnumSubkeys */      /* procedure EnumSubkeys */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* subkeyNum */      /* parameter subkeyNum */
        &__midl_frag2094,
        { 
        /* subkeyNum */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* subkeyId */      /* parameter subkeyId */
        &__midl_frag2094,
        { 
        /* subkeyId */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1773_t __midl_frag1773 =
{ 
/* EnumValues */
    { 
    /* EnumValues */      /* procedure EnumValues */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 72 /* 0x48 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ValueNum */      /* parameter ValueNum */
        &__midl_frag2094,
        { 
        /* ValueNum */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* valueId */      /* parameter valueId */
        &__midl_frag2094,
        { 
        /* valueId */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* valueSize */      /* parameter valueSize */
        &__midl_frag2094,
        { 
        /* valueSize */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1766_t __midl_frag1766 =
{ 
/* GetKeyInfo */
    { 
    /* GetKeyInfo */      /* procedure GetKeyInfo */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 72 /* 0x48 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* subKeys */      /* parameter subKeys */
        &__midl_frag2094,
        { 
        /* subKeys */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* dataValues */      /* parameter dataValues */
        &__midl_frag2094,
        { 
        /* dataValues */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1763_t __midl_frag1763 =
{ 
/* CloseDataKey */
    { 
    /* CloseDataKey */      /* procedure CloseDataKey */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1760_t __midl_frag1760 =
{ 
/* *LONG_PTR */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1931
};

static const __midl_frag1756_t __midl_frag1756 =
{ 
/* OpenDataKey */
    { 
    /* OpenDataKey */      /* procedure OpenDataKey */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 48 /* 0x30 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* baseKey */      /* parameter baseKey */
        &__midl_frag1931,
        { 
        /* baseKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* keyId */      /* parameter keyId */
        &__midl_frag2094,
        { 
        /* keyId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* keyIndex */      /* parameter keyIndex */
        &__midl_frag2094,
        { 
        /* keyIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1750_t __midl_frag1750 =
{ 
/* CreateDataKey */
    { 
    /* CreateDataKey */      /* procedure CreateDataKey */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 48 /* 0x30 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* hBaseKey */      /* parameter hBaseKey */
        &__midl_frag1931,
        { 
        /* hBaseKey */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* keyId */      /* parameter keyId */
        &__midl_frag2094,
        { 
        /* keyId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* dataKey */      /* parameter dataKey */
        &__midl_frag1931,
        { 
        /* dataKey */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1745_t __midl_frag1745 =
{ 
/* Load */
    { 
    /* Load */      /* procedure Load */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag1741_t __midl_frag1741 =
{ 
/* put_FileName */
    { 
    /* put_FileName */      /* procedure put_FileName */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* newVal */      /* parameter newVal */
        &__midl_frag1810,
        { 
        /* newVal */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1739_t __midl_frag1739 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag44
};

static const __midl_frag1738_t __midl_frag1738 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag1739
};

static const __midl_frag1737_t __midl_frag1737 =
{ 
/* *wireBSTR */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1738
};

static const __midl_frag1736_t __midl_frag1736 =
{ 
/* get_FileName */
    { 
    /* get_FileName */      /* procedure get_FileName */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1738,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1686_t __midl_frag1686 =
{ 
/* put_Current */
    { 
    /* put_Current */      /* procedure put_Current */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* newVal */      /* parameter newVal */
        &__midl_frag1684,
        { 
        /* newVal */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1684_t __midl_frag1684 =
{ 
/* FREQUENCY3 */
    { 
    /* FREQUENCY3 */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* FREQUENCY3 */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag1683_t __midl_frag1683 =
{ 
/* *FREQUENCY3 */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1684
};

static const __midl_frag1682_t __midl_frag1682 =
{ 
/* get_Current */
    { 
    /* get_Current */      /* procedure get_Current */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1684,
        { 
        /* pVal */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1677_t __midl_frag1677 =
{ 
/* GetIndex */
    { 
    /* GetIndex */      /* procedure GetIndex */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* nIndex */      /* parameter nIndex */
        &__midl_frag2094,
        { 
        /* nIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* value */      /* parameter value */
        &__midl_frag1684,
        { 
        /* value */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1672_t __midl_frag1672 =
{ 
/* IsSupported */
    { 
    /* IsSupported */      /* procedure IsSupported */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* nIndex */      /* parameter nIndex */
        &__midl_frag2094,
        { 
        /* nIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* supported */      /* parameter supported */
        &__midl_frag2094,
        { 
        /* supported */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1616_t __midl_frag1616 =
{ 
/* get_ScanConverter */
    { 
    /* get_ScanConverter */      /* procedure get_ScanConverter */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1932,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1593_t __midl_frag1593 =
{ 
/* SetSampleVolume */
    { 
    /* SetSampleVolume */      /* procedure SetSampleVolume */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag2094,
        { 
        /* startDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag2094,
        { 
        /* endDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* line */      /* parameter line */
        &__midl_frag2094,
        { 
        /* line */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1541_t __midl_frag1541 =
{ 
/* GetBufferSize */
    { 
    /* GetBufferSize */      /* procedure GetBufferSize */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 128 /* 0x80 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* total */      /* parameter total */
        &__midl_frag1931,
        { 
        /* total */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* used */      /* parameter used */
        &__midl_frag1931,
        { 
        /* used */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* writePos */      /* parameter writePos */
        &__midl_frag1931,
        { 
        /* writePos */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1524_t __midl_frag1524 =
{ 
/* GetBufferSize */
    { 
    /* GetBufferSize */      /* procedure GetBufferSize */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* total */      /* parameter total */
        &__midl_frag2094,
        { 
        /* total */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* used */      /* parameter used */
        &__midl_frag2094,
        { 
        /* used */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* writePos */      /* parameter writePos */
        &__midl_frag2094,
        { 
        /* writePos */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1519_t __midl_frag1519 =
{ 
/* SetSelection */
    { 
    /* SetSelection */      /* procedure SetSelection */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* from */      /* parameter from */
        &__midl_frag1931,
        { 
        /* from */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* to */      /* parameter to */
        &__midl_frag1931,
        { 
        /* to */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* mode */      /* parameter mode */
        &__midl_frag2094,
        { 
        /* mode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1512_t __midl_frag1512 =
{ 
/* GetContinuousInterval */
    { 
    /* GetContinuousInterval */      /* procedure GetContinuousInterval */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 88 /* 0x58 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* position */      /* parameter position */
        &__midl_frag1931,
        { 
        /* position */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* start */      /* parameter start */
        &__midl_frag1931,
        { 
        /* start */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* end */      /* parameter end */
        &__midl_frag1931,
        { 
        /* end */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1501_t __midl_frag1501 =
{ 
/* GetHoldRange */
    { 
    /* GetHoldRange */      /* procedure GetHoldRange */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 88 /* 0x58 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* from */      /* parameter from */
        &__midl_frag1931,
        { 
        /* from */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* to */      /* parameter to */
        &__midl_frag1931,
        { 
        /* to */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1497_t __midl_frag1497 =
{ 
/* SetHoldRange */
    { 
    /* SetHoldRange */      /* procedure SetHoldRange */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* from */      /* parameter from */
        &__midl_frag1931,
        { 
        /* from */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* to */      /* parameter to */
        &__midl_frag1931,
        { 
        /* to */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1490_t __midl_frag1490 =
{ 
/* GetShiftPosition */
    { 
    /* GetShiftPosition */      /* procedure GetShiftPosition */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 48 /* 0x30 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* from */      /* parameter from */
        &__midl_frag1931,
        { 
        /* from */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* shift */      /* parameter shift */
        &__midl_frag1931,
        { 
        /* shift */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* timeFormat */      /* parameter timeFormat */
        &__midl_frag2094,
        { 
        /* timeFormat */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pos */      /* parameter pos */
        &__midl_frag1931,
        { 
        /* pos */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1461_t __midl_frag1461 =
0xb    /* FC64_FLOAT32 */;

static const __midl_frag1460_t __midl_frag1460 =
{ 
/* put_Rate */
    { 
    /* put_Rate */      /* procedure put_Rate */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* newVal */      /* parameter newVal */
        &__midl_frag1461,
        { 
        /* newVal */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1457_t __midl_frag1457 =
{ 
/* *FLOAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1461
};

static const __midl_frag1456_t __midl_frag1456 =
{ 
/* get_Rate */
    { 
    /* get_Rate */      /* procedure get_Rate */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1461,
        { 
        /* pVal */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1452_t __midl_frag1452 =
{ 
/* get_Duration */
    { 
    /* get_Duration */      /* procedure get_Duration */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 48 /* 0x30 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1931,
        { 
        /* pVal */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1435_t __midl_frag1435 =
{ 
/* *LONG */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag2094
};

static const __midl_frag1434_t __midl_frag1434 =
{ 
/* CheckCapabilities */
    { 
    /* CheckCapabilities */      /* procedure CheckCapabilities */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pCaps */      /* parameter pCaps */
        &__midl_frag2094,
        { 
        /* pCaps */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1420_t __midl_frag1420 =
{ 
/* GetPositions */
    { 
    /* GetPositions */      /* procedure GetPositions */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pPos */      /* parameter pPos */
        &__midl_frag2094,
        { 
        /* pPos */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pStop */      /* parameter pStop */
        &__midl_frag2094,
        { 
        /* pStop */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag1196_t __midl_frag1196 =
{ 
/* *PALETTEENTRY */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag172
};

static const __midl_frag1193_t __midl_frag1193 =
{ 
/* GetPaletteEntries */
    { 
    /* GetPaletteEntries */      /* procedure GetPaletteEntries */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 52 /* 0x34 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startIndex */      /* parameter startIndex */
        &__midl_frag2094,
        { 
        /* startIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* entries */      /* parameter entries */
        &__midl_frag2094,
        { 
        /* entries */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* pPalEntries */      /* parameter pPalEntries */
        &__midl_frag172,
        { 
        /* pPalEntries */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1190_t __midl_frag1190 =
{ 
/* *PALETTEENTRY */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag172
};

static const __midl_frag1187_t __midl_frag1187 =
{ 
/* SetColorMap */
    { 
    /* SetColorMap */      /* procedure SetColorMap */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pTable1 */      /* parameter pTable1 */
        &__midl_frag172,
        { 
        /* pTable1 */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* size1 */      /* parameter size1 */
        &__midl_frag2094,
        { 
        /* size1 */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* pTable2 */      /* parameter pTable2 */
        &__midl_frag172,
        { 
        /* pTable2 */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* size2 */      /* parameter size2 */
        &__midl_frag2094,
        { 
        /* size2 */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1182_t __midl_frag1182 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x5,    /* FC64_INT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */  /* Offset */
    }
};

static const __midl_frag1181_t __midl_frag1181 =
{ 
/* *LONG */
    { 
    /* *LONG */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* *LONG */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag1182
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag2094
    }
};

static const __midl_frag1180_t __midl_frag1180 =
{ 
/* *LONG */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1181
};

static const __midl_frag1178_t __midl_frag1178 =
{ 
/* Preset */
    { 
    /* Preset */      /* procedure Preset */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cEntries */      /* parameter cEntries */
        &__midl_frag2094,
        { 
        /* cEntries */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pValues */      /* parameter pValues */
        &__midl_frag1181,
        { 
        /* pValues */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* iCurrent */      /* parameter iCurrent */
        &__midl_frag2094,
        { 
        /* iCurrent */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* SweepTime */      /* parameter SweepTime */
        &__midl_frag2094,
        { 
        /* SweepTime */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1116_t __midl_frag1116 =
{ 
/* GetPointDepth2 */
    { 
    /* GetPointDepth2 */      /* procedure GetPointDepth2 */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 80 /* 0x50 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* subFrame */      /* parameter subFrame */
        &__midl_frag2094,
        { 
        /* subFrame */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* X */      /* parameter X */
        &__midl_frag1461,
        { 
        /* X */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* Y */      /* parameter Y */
        &__midl_frag1461,
        { 
        /* Y */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* value */      /* parameter value */
        &__midl_frag1461,
        { 
        /* value */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1109_t __midl_frag1109 =
{ 
/* GetScanLine2 */
    { 
    /* GetScanLine2 */      /* procedure GetScanLine2 */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 80 /* 0x50 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* subFrame */      /* parameter subFrame */
        &__midl_frag2094,
        { 
        /* subFrame */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* X */      /* parameter X */
        &__midl_frag1461,
        { 
        /* X */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* Y */      /* parameter Y */
        &__midl_frag1461,
        { 
        /* Y */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* value */      /* parameter value */
        &__midl_frag2094,
        { 
        /* value */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag1095_t __midl_frag1095 =
{ 
/* GetCoordinates4 */
    { 
    /* GetCoordinates4 */      /* procedure GetCoordinates4 */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 80 /* 0x50 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 136 /* 0x88 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 16384 /* 0x4000 */,
        (NDR64_UINT16) 9 /* 0x9 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* scanLine */      /* parameter scanLine */
        &__midl_frag2094,
        { 
        /* scanLine */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* subFrame */      /* parameter subFrame */
        &__midl_frag2094,
        { 
        /* subFrame */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* X1 */      /* parameter X1 */
        &__midl_frag1461,
        { 
        /* X1 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* Y1 */      /* parameter Y1 */
        &__midl_frag1461,
        { 
        /* Y1 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* X2 */      /* parameter X2 */
        &__midl_frag1461,
        { 
        /* X2 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* Y2 */      /* parameter Y2 */
        &__midl_frag1461,
        { 
        /* Y2 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag1461,
        { 
        /* startDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        56 /* 0x38 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag1461,
        { 
        /* endDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        64 /* 0x40 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        72 /* 0x48 */,   /* Stack offset */
    }
};

static const __midl_frag1085_t __midl_frag1085 =
{ 
/* GetCoordinates3 */
    { 
    /* GetCoordinates3 */      /* procedure GetCoordinates3 */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 56 /* 0x38 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 6 /* 0x6 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* scanLine */      /* parameter scanLine */
        &__midl_frag2094,
        { 
        /* scanLine */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* subFrame */      /* parameter subFrame */
        &__midl_frag2094,
        { 
        /* subFrame */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* angle */      /* parameter angle */
        &__midl_frag1461,
        { 
        /* angle */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* X */      /* parameter X */
        &__midl_frag1461,
        { 
        /* X */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* Y */      /* parameter Y */
        &__midl_frag1461,
        { 
        /* Y */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    }
};

static const __midl_frag1079_t __midl_frag1079 =
{ 
/* GetPointDepth */
    { 
    /* GetPointDepth */      /* procedure GetPointDepth */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 20 /* 0x14 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* X */      /* parameter X */
        &__midl_frag1461,
        { 
        /* X */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* Y */      /* parameter Y */
        &__midl_frag1461,
        { 
        /* Y */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* value */      /* parameter value */
        &__midl_frag1461,
        { 
        /* value */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1067_t __midl_frag1067 =
{ 
/* GetScanLine */
    { 
    /* GetScanLine */      /* procedure GetScanLine */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 20 /* 0x14 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* X */      /* parameter X */
        &__midl_frag1461,
        { 
        /* X */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* Y */      /* parameter Y */
        &__midl_frag1461,
        { 
        /* Y */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* value */      /* parameter value */
        &__midl_frag2094,
        { 
        /* value */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1054_t __midl_frag1054 =
{ 
/* GetCoordinates2 */
    { 
    /* GetCoordinates2 */      /* procedure GetCoordinates2 */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 72 /* 0x48 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 136 /* 0x88 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 20480 /* 0x5000 */,
        (NDR64_UINT16) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* scanLine */      /* parameter scanLine */
        &__midl_frag2094,
        { 
        /* scanLine */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* X1 */      /* parameter X1 */
        &__midl_frag1461,
        { 
        /* X1 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* Y1 */      /* parameter Y1 */
        &__midl_frag1461,
        { 
        /* Y1 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* X2 */      /* parameter X2 */
        &__midl_frag1461,
        { 
        /* X2 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* Y2 */      /* parameter Y2 */
        &__midl_frag1461,
        { 
        /* Y2 */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* startDepth */      /* parameter startDepth */
        &__midl_frag1461,
        { 
        /* startDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    },
    { 
    /* endDepth */      /* parameter endDepth */
        &__midl_frag1461,
        { 
        /* endDepth */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        56 /* 0x38 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        64 /* 0x40 */,   /* Stack offset */
    }
};

static const __midl_frag1045_t __midl_frag1045 =
{ 
/* GetCoordinates */
    { 
    /* GetCoordinates */      /* procedure GetCoordinates */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* scanLine */      /* parameter scanLine */
        &__midl_frag2094,
        { 
        /* scanLine */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* angle */      /* parameter angle */
        &__midl_frag1461,
        { 
        /* angle */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* X */      /* parameter X */
        &__midl_frag1461,
        { 
        /* X */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* Y */      /* parameter Y */
        &__midl_frag1461,
        { 
        /* Y */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag970_t __midl_frag970 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x5748ca80,
        0x1710,
        0x489f,
        {0xbc, 0x13, 0x28, 0xf2, 0xc0, 0x12, 0x2b, 0x49}
    }
};

static const __midl_frag969_t __midl_frag969 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag970
};

static const __midl_frag964_t __midl_frag964 =
{ 
/* OnControlChangedBSTR */
    { 
    /* OnControlChangedBSTR */      /* procedure OnControlChangedBSTR */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 64 /* 0x40 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ctrlGUID */      /* parameter ctrlGUID */
        &__midl_frag1810,
        { 
        /* ctrlGUID */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* scanMode */      /* parameter scanMode */
        &__midl_frag2094,
        { 
        /* scanMode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* streamId */      /* parameter streamId */
        &__midl_frag2094,
        { 
        /* streamId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pControlObject */      /* parameter pControlObject */
        &__midl_frag969,
        { 
        /* pControlObject */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* dispId */      /* parameter dispId */
        &__midl_frag2094,
        { 
        /* dispId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* flags */      /* parameter flags */
        &__midl_frag2094,
        { 
        /* flags */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        56 /* 0x38 */,   /* Stack offset */
    }
};

static const __midl_frag956_t __midl_frag956 =
{ 
/* *IID */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag148
};

static const __midl_frag955_t __midl_frag955 =
{ 
/* OnControlChanged */
    { 
    /* OnControlChanged */      /* procedure OnControlChanged */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 64 /* 0x40 */ ,  /* Stack size */
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* riidCtrl */      /* parameter riidCtrl */
        &__midl_frag148,
        { 
        /* riidCtrl */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* scanMode */      /* parameter scanMode */
        &__midl_frag2094,
        { 
        /* scanMode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* streamId */      /* parameter streamId */
        &__midl_frag2094,
        { 
        /* streamId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pControlObj */      /* parameter pControlObj */
        &__midl_frag969,
        { 
        /* pControlObj */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* dispId */      /* parameter dispId */
        &__midl_frag2094,
        { 
        /* dispId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* flags */      /* parameter flags */
        &__midl_frag2094,
        { 
        /* flags */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        56 /* 0x38 */,   /* Stack offset */
    }
};

static const __midl_frag942_t __midl_frag942 =
{ 
/* GetZoom */
    { 
    /* GetZoom */      /* procedure GetZoom */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ZoomCenterX */      /* parameter ZoomCenterX */
        &__midl_frag1461,
        { 
        /* ZoomCenterX */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ZooomCenterY */      /* parameter ZooomCenterY */
        &__midl_frag1461,
        { 
        /* ZooomCenterY */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* ZoomFactor */      /* parameter ZoomFactor */
        &__midl_frag2094,
        { 
        /* ZoomFactor */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* fPrefered */      /* parameter fPrefered */
        &__midl_frag2094,
        { 
        /* fPrefered */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag937_t __midl_frag937 =
{ 
/* SetZoom */
    { 
    /* SetZoom */      /* procedure SetZoom */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 20 /* 0x14 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ZoomCenterX */      /* parameter ZoomCenterX */
        &__midl_frag1461,
        { 
        /* ZoomCenterX */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ZoomCenterY */      /* parameter ZoomCenterY */
        &__midl_frag1461,
        { 
        /* ZoomCenterY */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* ZoomFactor */      /* parameter ZoomFactor */
        &__midl_frag2094,
        { 
        /* ZoomFactor */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag926_t __midl_frag926 =
{ 
/* GetViewRect */
    { 
    /* GetViewRect */      /* procedure GetViewRect */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 56 /* 0x38 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 136 /* 0x88 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 6 /* 0x6 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* left */      /* parameter left */
        &__midl_frag1461,
        { 
        /* left */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* top */      /* parameter top */
        &__midl_frag1461,
        { 
        /* top */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* right */      /* parameter right */
        &__midl_frag1461,
        { 
        /* right */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* bottom */      /* parameter bottom */
        &__midl_frag1461,
        { 
        /* bottom */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* fPrefered */      /* parameter fPrefered */
        &__midl_frag2094,
        { 
        /* fPrefered */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    }
};

static const __midl_frag920_t __midl_frag920 =
{ 
/* SetViewRect */
    { 
    /* SetViewRect */      /* procedure SetViewRect */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 340 /* 0x154 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* left */      /* parameter left */
        &__midl_frag1461,
        { 
        /* left */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* top */      /* parameter top */
        &__midl_frag1461,
        { 
        /* top */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* right */      /* parameter right */
        &__midl_frag1461,
        { 
        /* right */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* bottom */      /* parameter bottom */
        &__midl_frag1461,
        { 
        /* bottom */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag911_t __midl_frag911 =
{ 
/* UnitsToPixels */
    { 
    /* UnitsToPixels */      /* procedure UnitsToPixels */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 56 /* 0x38 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 72 /* 0x48 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 20 /* 0x14 */,
        (NDR64_UINT16) 6 /* 0x6 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* Xunit */      /* parameter Xunit */
        &__midl_frag1461,
        { 
        /* Xunit */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* Yunit */      /* parameter Yunit */
        &__midl_frag1461,
        { 
        /* Yunit */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* XPix */      /* parameter XPix */
        &__midl_frag1461,
        { 
        /* XPix */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* Ypix */      /* parameter Ypix */
        &__midl_frag1461,
        { 
        /* Ypix */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* pixOrigin */      /* parameter pixOrigin */
        &__midl_frag2094,
        { 
        /* pixOrigin */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    }
};

static const __midl_frag886_t __midl_frag886 =
{ 
/* *IMAGE_RESOLUTION */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1684
};

static const __midl_frag885_t __midl_frag885 =
{ 
/* SetResolution */
    { 
    /* SetResolution */      /* procedure SetResolution */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 48 /* 0x30 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* resolution */      /* parameter resolution */
        &__midl_frag1684,
        { 
        /* resolution */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag880_t __midl_frag880 =
{ 
/* GetResolution */
    { 
    /* GetResolution */      /* procedure GetResolution */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* resolution */      /* parameter resolution */
        &__midl_frag1684,
        { 
        /* resolution */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* fPrefered */      /* parameter fPrefered */
        &__midl_frag2094,
        { 
        /* fPrefered */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag876_t __midl_frag876 =
{ 
/* SetOrigin */
    { 
    /* SetOrigin */      /* procedure SetOrigin */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 56 /* 0x38 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* point */      /* parameter point */
        &__midl_frag1684,
        { 
        /* point */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pixelsOrigin */      /* parameter pixelsOrigin */
        &__midl_frag2094,
        { 
        /* pixelsOrigin */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag870_t __midl_frag870 =
{ 
/* GetOrigin */
    { 
    /* GetOrigin */      /* procedure GetOrigin */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* point */      /* parameter point */
        &__midl_frag1684,
        { 
        /* point */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pixelsOrigin */      /* parameter pixelsOrigin */
        &__midl_frag2094,
        { 
        /* pixelsOrigin */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* fPrefered */      /* parameter fPrefered */
        &__midl_frag2094,
        { 
        /* fPrefered */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag860_t __midl_frag860 =
{ 
/* SetPaletteEntries */
    { 
    /* SetPaletteEntries */      /* procedure SetPaletteEntries */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 60 /* 0x3c */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* startIndex */      /* parameter startIndex */
        &__midl_frag2094,
        { 
        /* startIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* entries */      /* parameter entries */
        &__midl_frag2094,
        { 
        /* entries */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* pPalEntries */      /* parameter pPalEntries */
        &__midl_frag172,
        { 
        /* pPalEntries */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag846_t __midl_frag846 =
{ 
/* GetReferenceData */
    { 
    /* GetReferenceData */      /* procedure GetReferenceData */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 104 /* 0x68 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pValueMax */      /* parameter pValueMax */
        &__midl_frag2094,
        { 
        /* pValueMax */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pValuesCount */      /* parameter pValuesCount */
        &__midl_frag2094,
        { 
        /* pValuesCount */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* pData */      /* parameter pData */
        &__midl_frag2094,
        { 
        /* pData */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag840_t __midl_frag840 =
{ 
/* SetReferenceData */
    { 
    /* SetReferenceData */      /* procedure SetReferenceData */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 48 /* 0x30 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* nValueMax */      /* parameter nValueMax */
        &__midl_frag2094,
        { 
        /* nValueMax */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* nValuesCount */      /* parameter nValuesCount */
        &__midl_frag2094,
        { 
        /* nValuesCount */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* pData */      /* parameter pData */
        &__midl_frag2094,
        { 
        /* pData */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag714_t __midl_frag714 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 24 /* 0x18 */  /* Offset */
    }
};

static const __midl_frag713_t __midl_frag713 =
{ 
/* *LONG */
    { 
    /* *LONG */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* *LONG */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag714
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag2094
    }
};

static const __midl_frag712_t __midl_frag712 =
{ 
/* *LONG */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag713
};

static const __midl_frag708_t __midl_frag708 =
{ 
/* SetFocusState */
    { 
    /* SetFocusState */      /* procedure SetFocusState */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mode */      /* parameter mode */
        &__midl_frag2094,
        { 
        /* mode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* focusSet */      /* parameter focusSet */
        &__midl_frag2094,
        { 
        /* focusSet */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* entries */      /* parameter entries */
        &__midl_frag2094,
        { 
        /* entries */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pFocusState */      /* parameter pFocusState */
        &__midl_frag713,
        { 
        /* pFocusState */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag699_t __midl_frag699 =
{ 
/* GetFocusState */
    { 
    /* GetFocusState */      /* procedure GetFocusState */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mode */      /* parameter mode */
        &__midl_frag2094,
        { 
        /* mode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* focusSet */      /* parameter focusSet */
        &__midl_frag2094,
        { 
        /* focusSet */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* entries */      /* parameter entries */
        &__midl_frag2094,
        { 
        /* entries */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pFocusState */      /* parameter pFocusState */
        &__midl_frag713,
        { 
        /* pFocusState */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag692_t __midl_frag692 =
{ 
/* GetFocusPoint */
    { 
    /* GetFocusPoint */      /* procedure GetFocusPoint */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mode */      /* parameter mode */
        &__midl_frag2094,
        { 
        /* mode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* focusSet */      /* parameter focusSet */
        &__midl_frag2094,
        { 
        /* focusSet */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* zoneIndex */      /* parameter zoneIndex */
        &__midl_frag2094,
        { 
        /* zoneIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pFocusPoint */      /* parameter pFocusPoint */
        &__midl_frag2094,
        { 
        /* pFocusPoint */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag685_t __midl_frag685 =
{ 
/* GetFocalZone */
    { 
    /* GetFocalZone */      /* procedure GetFocalZone */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 60 /* 0x3c */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mode */      /* parameter mode */
        &__midl_frag2094,
        { 
        /* mode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* focusSet */      /* parameter focusSet */
        &__midl_frag2094,
        { 
        /* focusSet */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* zoneIndex */      /* parameter zoneIndex */
        &__midl_frag2094,
        { 
        /* zoneIndex */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pFocalZone */      /* parameter pFocalZone */
        &__midl_frag1916,
        { 
        /* pFocalZone */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag521_t __midl_frag521 =
{ 
/* *UINT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag2094
};

static const __midl_frag517_t __midl_frag517 =
{ 
/* *ULONGLONG */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1931
};

static const __midl_frag514_t __midl_frag514 =
0x4    /* FC64_INT16 */;

static const __midl_frag513_t __midl_frag513 =
{ 
/* *USHORT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag514
};

static const __midl_frag512_t __midl_frag512 =
0x10    /* FC64_CHAR */;

static const __midl_frag511_t __midl_frag511 =
{ 
/* *CHAR */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag512
};

static const __midl_frag510_t __midl_frag510 =
{ 
/* *DECIMAL */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag509
};

static const __midl_frag509_t __midl_frag509 =
{ 
/* DECIMAL */
    { 
    /* DECIMAL */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* DECIMAL */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag502_t __midl_frag502 =
{ 
/* *_wireVARIANT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag383
};

static const __midl_frag501_t __midl_frag501 =
{ 
/* **_wireVARIANT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag502
};

static const __midl_frag500_t __midl_frag500 =
{ 
/* *_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag403
};

static const __midl_frag499_t __midl_frag499 =
{ 
/* **_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag500
};

static const __midl_frag498_t __midl_frag498 =
{ 
/* ***_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag499
};

static const __midl_frag497_t __midl_frag497 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00020400,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag496_t __midl_frag496 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag497
};

static const __midl_frag495_t __midl_frag495 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag496
};

static const __midl_frag492_t __midl_frag492 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1933
};

static const __midl_frag490_t __midl_frag490 =
{ 
/* **FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1739
};

static const __midl_frag489_t __midl_frag489 =
0xc    /* FC64_FLOAT64 */;

static const __midl_frag488_t __midl_frag488 =
{ 
/* *DATE */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag489
};

static const __midl_frag487_t __midl_frag487 =
{ 
/* *CY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag394
};

static const __midl_frag479_t __midl_frag479 =
{ 
/* *FLOAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1461
};

static const __midl_frag471_t __midl_frag471 =
{ 
/* *BYTE */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag1806
};

static const __midl_frag470_t __midl_frag470 =
{ 
/* *_wireBRECORD */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag435
};

static const __midl_frag469_t __midl_frag469 =
{ 
/*  */
    { 
    /* *hyper */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag466
    }
};

static const __midl_frag467_t __midl_frag467 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */
    }
};

static const __midl_frag466_t __midl_frag466 =
{ 
/* *hyper */
    { 
    /* *hyper */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* *hyper */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag467
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag1931
    }
};

static const __midl_frag465_t __midl_frag465 =
{ 
/* HYPER_SIZEDARR */
    { 
    /* HYPER_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* HYPER_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag469,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag464_t __midl_frag464 =
{ 
/*  */
    { 
    /* *ULONG */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag461
    }
};

static const __midl_frag461_t __midl_frag461 =
{ 
/* *ULONG */
    { 
    /* *ULONG */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* *ULONG */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag467
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag2094
    }
};

static const __midl_frag460_t __midl_frag460 =
{ 
/* DWORD_SIZEDARR */
    { 
    /* DWORD_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* DWORD_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag464,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag459_t __midl_frag459 =
{ 
/*  */
    { 
    /* *short */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag456
    }
};

static const __midl_frag456_t __midl_frag456 =
{ 
/* *short */
    { 
    /* *short */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* *short */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag467
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag514
    }
};

static const __midl_frag455_t __midl_frag455 =
{ 
/* WORD_SIZEDARR */
    { 
    /* WORD_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* WORD_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag459,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag454_t __midl_frag454 =
{ 
/*  */
    { 
    /* *byte */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag451
    }
};

static const __midl_frag451_t __midl_frag451 =
{ 
/* *byte */
    { 
    /* *byte */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *byte */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag467
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1806
    }
};

static const __midl_frag450_t __midl_frag450 =
{ 
/* BYTE_SIZEDARR */
    { 
    /* BYTE_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* BYTE_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag454,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag449_t __midl_frag449 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag448_t __midl_frag448 =
{ 
/*  */
    { 
    /* **struct _NDR64_POINTER_FORMAT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag443
    }
};

static const __midl_frag443_t __midl_frag443 =
{ 
/* ** */
    { 
    /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag467
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *struct _NDR64_POINTER_FORMAT */
                0x24,    /* FC64_IP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag1934
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag1933
    }
};

static const __midl_frag442_t __midl_frag442 =
{ 
/* SAFEARR_HAVEIID */
    { 
    /* SAFEARR_HAVEIID */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_HAVEIID */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        0,
        0,
        &__midl_frag448,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag449
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag441_t __midl_frag441 =
{ 
/*  */
    { 
    /* **_wireBRECORD */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag433
    }
};

static const __midl_frag438_t __midl_frag438 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */
    }
};

static const __midl_frag437_t __midl_frag437 =
{ 
/* *byte */
    { 
    /* *byte */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *byte */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag438
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1806
    }
};

static const __midl_frag436_t __midl_frag436 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x0000002f,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag435_t __midl_frag435 =
{ 
/* _wireBRECORD */
    { 
    /* _wireBRECORD */
        0x31,    /* FC64_PSTRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireBRECORD */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 24 /* 0x18 */
    },
    { 
    /*  */
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *struct _NDR64_POINTER_FORMAT */
            0x24,    /* FC64_IP */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag436
        },
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 16 /* 0x10 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *byte */
            0x22,    /* FC64_OP */
            (NDR64_UINT8) 32 /* 0x20 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag437
        },
        0x93    /* FC64_END */
    }
};

static const __midl_frag433_t __midl_frag433 =
{ 
/* **_wireBRECORD */
    { 
    /* **_wireBRECORD */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **_wireBRECORD */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag467
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *_wireBRECORD */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag435
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag470
    }
};

static const __midl_frag432_t __midl_frag432 =
{ 
/* SAFEARR_BRECORD */
    { 
    /* SAFEARR_BRECORD */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_BRECORD */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag441,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag431_t __midl_frag431 =
{ 
/*  */
    { 
    /* **_wireVARIANT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag428
    }
};

static const __midl_frag428_t __midl_frag428 =
{ 
/* **_wireVARIANT */
    { 
    /* **_wireVARIANT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **_wireVARIANT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag467
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *_wireVARIANT */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag383
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag502
    }
};

static const __midl_frag427_t __midl_frag427 =
{ 
/* SAFEARR_VARIANT */
    { 
    /* SAFEARR_VARIANT */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_VARIANT */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag431,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag426_t __midl_frag426 =
{ 
/*  */
    { 
    /* **struct _NDR64_POINTER_FORMAT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag421
    }
};

static const __midl_frag421_t __midl_frag421 =
{ 
/* ** */
    { 
    /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag467
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *struct _NDR64_POINTER_FORMAT */
                0x24,    /* FC64_IP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag497
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag496
    }
};

static const __midl_frag420_t __midl_frag420 =
{ 
/* SAFEARR_DISPATCH */
    { 
    /* SAFEARR_DISPATCH */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_DISPATCH */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag426,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag413_t __midl_frag413 =
{ 
/* SAFEARR_UNKNOWN */
    { 
    /* SAFEARR_UNKNOWN */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_UNKNOWN */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag448,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag412_t __midl_frag412 =
{ 
/*  */
    { 
    /* **FLAGGED_WORD_BLOB */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag409
    }
};

static const __midl_frag409_t __midl_frag409 =
{ 
/* **FLAGGED_WORD_BLOB */
    { 
    /* **FLAGGED_WORD_BLOB */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **FLAGGED_WORD_BLOB */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag467
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *FLAGGED_WORD_BLOB */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag44
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag1739
    }
};

static const __midl_frag408_t __midl_frag408 =
{ 
/* SAFEARR_BSTR */
    { 
    /* SAFEARR_BSTR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_BSTR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag412,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag407_t __midl_frag407 =
{ 
/* SAFEARRAYUNION */
    { 
    /* SAFEARRAYUNION */
        0x50,    /* FC64_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 10 /* 0xa */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8 /* 0x8 */,
        &__midl_frag408,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 13 /* 0xd */,
        &__midl_frag413,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 9 /* 0x9 */,
        &__midl_frag420,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 12 /* 0xc */,
        &__midl_frag427,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 36 /* 0x24 */,
        &__midl_frag432,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 32781 /* 0x800d */,
        &__midl_frag442,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16 /* 0x10 */,
        &__midl_frag450,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 2 /* 0x2 */,
        &__midl_frag455,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 3 /* 0x3 */,
        &__midl_frag460,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 20 /* 0x14 */,
        &__midl_frag465,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag405_t __midl_frag405 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x3,    /* FC64_UINT16 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */
    }
};

static const __midl_frag404_t __midl_frag404 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag405
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag1684
    }
};

static const __midl_frag403_t __midl_frag403 =
{ 
/* _wireSAFEARRAY */
    { 
    /* _wireSAFEARRAY */
        0x36,    /* FC64_CONF_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireSAFEARRAY */
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 1 /* 0x1 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        0,
        0,
        0,
        &__midl_frag404,
    },
    { 
    /*  */
        { 
        /* _wireSAFEARRAY */
            0x30,    /* FC64_STRUCT */
            (NDR64_UINT8) 1 /* 0x1 */,
            (NDR64_UINT16) 12 /* 0xc */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag407
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag394_t __midl_frag394 =
{ 
/* CY */
    { 
    /* CY */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* CY */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag385_t __midl_frag385 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x3,    /* FC64_UINT16 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag384_t __midl_frag384 =
{ 
/* __MIDL_IOleAutomationTypes_0004 */
    { 
    /* __MIDL_IOleAutomationTypes_0004 */
        0x51,    /* FC64_NON_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag385,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 47 /* 0x2f */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 20 /* 0x14 */,
        &__midl_frag1931,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 3 /* 0x3 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 17 /* 0x11 */,
        &__midl_frag1806,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 2 /* 0x2 */,
        &__midl_frag514,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 4 /* 0x4 */,
        &__midl_frag1461,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 5 /* 0x5 */,
        &__midl_frag489,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 11 /* 0xb */,
        &__midl_frag514,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 10 /* 0xa */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 6 /* 0x6 */,
        &__midl_frag394,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 7 /* 0x7 */,
        &__midl_frag489,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8 /* 0x8 */,
        &__midl_frag1739,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 13 /* 0xd */,
        &__midl_frag1933,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 9 /* 0x9 */,
        &__midl_frag496,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8192 /* 0x2000 */,
        &__midl_frag499,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 36 /* 0x24 */,
        &__midl_frag470,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16420 /* 0x4024 */,
        &__midl_frag470,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16401 /* 0x4011 */,
        &__midl_frag471,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16386 /* 0x4002 */,
        &__midl_frag513,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16387 /* 0x4003 */,
        &__midl_frag521,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16404 /* 0x4014 */,
        &__midl_frag517,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16388 /* 0x4004 */,
        &__midl_frag479,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16389 /* 0x4005 */,
        &__midl_frag488,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16395 /* 0x400b */,
        &__midl_frag513,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16394 /* 0x400a */,
        &__midl_frag521,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16390 /* 0x4006 */,
        &__midl_frag487,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16391 /* 0x4007 */,
        &__midl_frag488,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16392 /* 0x4008 */,
        &__midl_frag490,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16397 /* 0x400d */,
        &__midl_frag492,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16393 /* 0x4009 */,
        &__midl_frag495,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 24576 /* 0x6000 */,
        &__midl_frag498,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16396 /* 0x400c */,
        &__midl_frag501,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16 /* 0x10 */,
        &__midl_frag512,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 18 /* 0x12 */,
        &__midl_frag514,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 19 /* 0x13 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 21 /* 0x15 */,
        &__midl_frag1931,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 22 /* 0x16 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 23 /* 0x17 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 14 /* 0xe */,
        &__midl_frag509,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16398 /* 0x400e */,
        &__midl_frag510,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16400 /* 0x4010 */,
        &__midl_frag511,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16402 /* 0x4012 */,
        &__midl_frag513,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16403 /* 0x4013 */,
        &__midl_frag521,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16405 /* 0x4015 */,
        &__midl_frag517,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16406 /* 0x4016 */,
        &__midl_frag521,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16407 /* 0x4017 */,
        &__midl_frag521,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 0 /* 0x0 */,
        0,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1 /* 0x1 */,
        0,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag383_t __midl_frag383 =
{ 
/* _wireVARIANT */
    { 
    /* _wireVARIANT */
        0x34,    /* FC64_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireVARIANT */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* _wireVARIANT */
            0x30,    /* FC64_STRUCT */
            (NDR64_UINT8) 3 /* 0x3 */,
            (NDR64_UINT16) 16 /* 0x10 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag384
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag381_t __midl_frag381 =
{ 
/* wireVARIANT */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 3 /* 0x3 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 24 /* 0x18 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag502
};

static const __midl_frag380_t __midl_frag380 =
{ 
/* *wireVARIANT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag381
};

static const __midl_frag378_t __midl_frag378 =
{ 
/* Item */
    { 
    /* Item */      /* procedure Item */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* lItem */      /* parameter lItem */
        &__midl_frag2094,
        { 
        /* lItem */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pValue */      /* parameter pValue */
        &__midl_frag381,
        { 
        /* pValue */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag316_t __midl_frag316 =
{ 
/* get_SerialNumber */
    { 
    /* get_SerialNumber */      /* procedure get_SerialNumber */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* index */      /* parameter index */
        &__midl_frag2094,
        { 
        /* index */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag1738,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag283_t __midl_frag283 =
{ 
/* Item */
    { 
    /* Item */      /* procedure Item */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* lItem */      /* parameter lItem */
        &__midl_frag2094,
        { 
        /* lItem */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ppUnk */      /* parameter ppUnk */
        &__midl_frag1932,
        { 
        /* ppUnk */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag277_t __midl_frag277 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x4cf81935,
        0xdcb3,
        0x4c19,
        {0x9d, 0xc9, 0x76, 0x53, 0x06, 0xa4, 0x71, 0x4f}
    }
};

static const __midl_frag276_t __midl_frag276 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag277
};

static const __midl_frag275_t __midl_frag275 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag276
};

static const __midl_frag271_t __midl_frag271 =
{ 
/* CreateUsgfwFile */
    { 
    /* CreateUsgfwFile */      /* procedure CreateUsgfwFile */
        (NDR64_UINT32) 3014979 /* 0x2e0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* fileName */      /* parameter fileName */
        &__midl_frag1810,
        { 
        /* fileName */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* createFlags */      /* parameter createFlags */
        &__midl_frag2094,
        { 
        /* createFlags */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* ppFileObject */      /* parameter ppFileObject */
        &__midl_frag275,
        { 
        /* ppFileObject */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag265_t __midl_frag265 =
{ 
/* ProbeCodeFromName */
    { 
    /* ProbeCodeFromName */      /* procedure ProbeCodeFromName */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* Name */      /* parameter Name */
        &__midl_frag1810,
        { 
        /* Name */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* Code */      /* parameter Code */
        &__midl_frag2094,
        { 
        /* Code */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag243_t __midl_frag243 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0xbfef8670,
        0xdbc1,
        0x4b47,
        {0xbc, 0x01, 0xbb, 0xbc, 0xa1, 0x74, 0xdd, 0x31}
    }
};

static const __midl_frag242_t __midl_frag242 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag243
};

static const __midl_frag241_t __midl_frag241 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag242
};

static const __midl_frag238_t __midl_frag238 =
{ 
/* CreateDataView */
    { 
    /* CreateDataView */      /* procedure CreateDataView */
        (NDR64_UINT32) 917827 /* 0xe0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pUnkSource */      /* parameter pUnkSource */
        &__midl_frag1933,
        { 
        /* pUnkSource */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ppUsgDataView */      /* parameter ppUsgDataView */
        &__midl_frag241,
        { 
        /* ppUsgDataView */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag209_t __midl_frag209 =
{ 
/* OnBeamformerStateChanged */
    { 
    /* OnBeamformerStateChanged */      /* procedure OnBeamformerStateChanged */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pUsgBeamformer */      /* parameter pUsgBeamformer */
        &__midl_frag1933,
        { 
        /* pUsgBeamformer */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* reserved */      /* parameter reserved */
        &__midl_frag2094,
        { 
        /* reserved */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag177_t __midl_frag177 =
{ 
/* *RECT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag148
};

static const __midl_frag176_t __midl_frag176 =
{ 
/* GetOutputRect */
    { 
    /* GetOutputRect */      /* procedure GetOutputRect */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 64 /* 0x40 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pRect */      /* parameter pRect */
        &__midl_frag148,
        { 
        /* pRect */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag174_t __midl_frag174 =
{ 
/* put_BkColor */
    { 
    /* put_BkColor */      /* procedure put_BkColor */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 20 /* 0x14 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* newVal */      /* parameter newVal */
        &__midl_frag172,
        { 
        /* newVal */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag172_t __midl_frag172 =
{ 
/* PALETTEENTRY */
    { 
    /* PALETTEENTRY */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* PALETTEENTRY */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */
    }
};

static const __midl_frag170_t __midl_frag170 =
{ 
/* get_BkColor */
    { 
    /* get_BkColor */      /* procedure get_BkColor */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 52 /* 0x34 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag172,
        { 
        /* pVal */
            0,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag153_t __midl_frag153 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x7bf80981,
        0xbf32,
        0x101a,
        {0x8b, 0xbb, 0x00, 0xaa, 0x00, 0x30, 0x0c, 0xab}
    }
};

static const __midl_frag152_t __midl_frag152 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag153
};

static const __midl_frag151_t __midl_frag151 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag152
};

static const __midl_frag150_t __midl_frag150 =
{ 
/* Capture */
    { 
    /* Capture */      /* procedure Capture */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ppCurrentImage */      /* parameter ppCurrentImage */
        &__midl_frag151,
        { 
        /* ppCurrentImage */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag148_t __midl_frag148 =
{ 
/* RECT */
    { 
    /* RECT */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* RECT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag146_t __midl_frag146 =
{ 
/* SetOutputRect */
    { 
    /* SetOutputRect */      /* procedure SetOutputRect */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 56 /* 0x38 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pRect */      /* parameter pRect */
        &__midl_frag148,
        { 
        /* pRect */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag144_t __midl_frag144 =
{ 
/* *USG_MIXING_BITMAP */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag121
};

static const __midl_frag143_t __midl_frag143 =
{ 
/* UpdateAlphaBitmapParameters */
    { 
    /* UpdateAlphaBitmapParameters */      /* procedure UpdateAlphaBitmapParameters */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pBmpParams */      /* parameter pBmpParams */
        &__midl_frag121,
        { 
        /* pBmpParams */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag138_t __midl_frag138 =
{ 
/* *userHBITMAP */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag129
};

static const __midl_frag137_t __midl_frag137 =
{ 
/* wireHBITMAP */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 2 /* 0x2 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag138
};

static const __midl_frag134_t __midl_frag134 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 20 /* 0x14 */
    }
};

static const __midl_frag133_t __midl_frag133 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag134
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag1806
    }
};

static const __midl_frag132_t __midl_frag132 =
{ 
/* userBITMAP */
    { 
    /* userBITMAP */
        0x32,    /* FC64_CONF_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* userBITMAP */
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 24 /* 0x18 */,
        &__midl_frag133
    }
};

static const __midl_frag131_t __midl_frag131 =
{ 
/* *userBITMAP */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag132
};

static const __midl_frag129_t __midl_frag129 =
{ 
/* userHBITMAP */
    { 
    /* userHBITMAP */
        0x50,    /* FC64_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x5,    /* FC64_INT32 */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 3 /* 0x3 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1215587415 /* 0x48746457 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1383359575 /* 0x52746457 */,
        &__midl_frag131,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1349805143 /* 0x50746457 */,
        &__midl_frag1931,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag124_t __midl_frag124 =
{ 
/* RemotableHandle */
    { 
    /* RemotableHandle */
        0x50,    /* FC64_ENCAPSULATED_UNION */
        (NDR64_UINT8) 3 /* 0x3 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x5,    /* FC64_INT32 */
        (NDR64_UINT32) 4 /* 0x4 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1215587415 /* 0x48746457 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1383359575 /* 0x52746457 */,
        &__midl_frag2094,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag123_t __midl_frag123 =
{ 
/* *RemotableHandle */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag124
};

static const __midl_frag122_t __midl_frag122 =
{ 
/* wireHDC */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 1 /* 0x1 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag123
};

static const __midl_frag121_t __midl_frag121 =
{ 
/* USG_MIXING_BITMAP */
    { 
    /* USG_MIXING_BITMAP */
        0x34,    /* FC64_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* USG_MIXING_BITMAP */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 96 /* 0x60 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag122
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag137
        },
        { 
        /* tagMIXBITMAP */
            0x92,    /* FC64_BUFFER_ALIGN */
            (NDR64_UINT8) 7 /* 0x7 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag137
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0xb,    /* FC64_FLOAT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag119_t __midl_frag119 =
{ 
/* GetAlphaBitmapParameters */
    { 
    /* GetAlphaBitmapParameters */      /* procedure GetAlphaBitmapParameters */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pBmpParams */      /* parameter pBmpParams */
        &__midl_frag121,
        { 
        /* pBmpParams */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag103_t __midl_frag103 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0xb7c94539,
        0xa65a,
        0x42f3,
        {0x8b, 0x65, 0xf8, 0x3d, 0x11, 0x4f, 0xf4, 0xc8}
    }
};

static const __midl_frag102_t __midl_frag102 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag103
};

static const __midl_frag101_t __midl_frag101 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag102
};

static const __midl_frag100_t __midl_frag100 =
{ 
/* GetMixerControl */
    { 
    /* GetMixerControl */      /* procedure GetMixerControl */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ppMixerControl */      /* parameter ppMixerControl */
        &__midl_frag101,
        { 
        /* ppMixerControl */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag94_t __midl_frag94 =
{ 
/* GetControlObj */
    { 
    /* GetControlObj */      /* procedure GetControlObj */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 72 /* 0x48 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* riid */      /* parameter riid */
        &__midl_frag148,
        { 
        /* riid */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ppobj */      /* parameter ppobj */
        &__midl_frag1932,
        { 
        /* ppobj */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag88_t __midl_frag88 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0xb8cba727,
        0xd104,
        0x416e,
        {0xb0, 0xcc, 0xc6, 0x2e, 0x9c, 0xf9, 0xb1, 0xf9}
    }
};

static const __midl_frag87_t __midl_frag87 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag88
};

static const __midl_frag86_t __midl_frag86 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag87
};

static const __midl_frag85_t __midl_frag85 =
{ 
/* CreateClone */
    { 
    /* CreateClone */      /* procedure CreateClone */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* ppNewScanMode */      /* parameter ppNewScanMode */
        &__midl_frag86,
        { 
        /* ppNewScanMode */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag64_t __midl_frag64 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag969
};

static const __midl_frag60_t __midl_frag60 =
{ 
/* GetControlObj */
    { 
    /* GetControlObj */      /* procedure GetControlObj */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 88 /* 0x58 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* riidCtrl */      /* parameter riidCtrl */
        &__midl_frag148,
        { 
        /* riidCtrl */
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* scanMode */      /* parameter scanMode */
        &__midl_frag2094,
        { 
        /* scanMode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* streamId */      /* parameter streamId */
        &__midl_frag2094,
        { 
        /* streamId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* ppCtrl */      /* parameter ppCtrl */
        &__midl_frag64,
        { 
        /* ppCtrl */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag53_t __midl_frag53 =
{ 
/* GetMixerControl */
    { 
    /* GetMixerControl */      /* procedure GetMixerControl */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* scanMode */      /* parameter scanMode */
        &__midl_frag2094,
        { 
        /* scanMode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* streamId */      /* parameter streamId */
        &__midl_frag2094,
        { 
        /* streamId */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* ppMixerCtrl */      /* parameter ppMixerCtrl */
        &__midl_frag101,
        { 
        /* ppMixerCtrl */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag45_t __midl_frag45 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag438
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag514
    }
};

static const __midl_frag44_t __midl_frag44 =
{ 
/* FLAGGED_WORD_BLOB */
    { 
    /* FLAGGED_WORD_BLOB */
        0x32,    /* FC64_CONF_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* FLAGGED_WORD_BLOB */
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag45
    }
};

static const __midl_frag30_t __midl_frag30 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x56a868b1,
        0x0ad4,
        0x11ce,
        {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
    }
};

static const __midl_frag29_t __midl_frag29 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag30
};

static const __midl_frag28_t __midl_frag28 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag29
};

static const __midl_frag27_t __midl_frag27 =
{ 
/* get_FilgraphManager */
    { 
    /* get_FilgraphManager */      /* procedure get_FilgraphManager */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag28,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag21_t __midl_frag21 =
{ 
/* GetScanModeObj */
    { 
    /* GetScanModeObj */      /* procedure GetScanModeObj */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* scanMode */      /* parameter scanMode */
        &__midl_frag2094,
        { 
        /* scanMode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* pUsgScanMode */      /* parameter pUsgScanMode */
        &__midl_frag86,
        { 
        /* pUsgScanMode */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag19_t __midl_frag19 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x60c480b7,
        0xf1e7,
        0x403c,
        {0x8a, 0xf3, 0x8d, 0xce, 0xd9, 0x9a, 0x25, 0x60}
    }
};

static const __midl_frag18_t __midl_frag18 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag19
};

static const __midl_frag17_t __midl_frag17 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag18
};

static const __midl_frag16_t __midl_frag16 =
{ 
/* get_ScanModes */
    { 
    /* get_ScanModes */      /* procedure get_ScanModes */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag17,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag2094,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1_t __midl_frag1 =
(NDR64_UINT32) 0 /* 0x0 */;


#include "poppack.h"


static const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize64
            ,BSTR_UserMarshal64
            ,BSTR_UserUnmarshal64
            ,BSTR_UserFree64
            },
            {
            HDC_UserSize64
            ,HDC_UserMarshal64
            ,HDC_UserUnmarshal64
            ,HDC_UserFree64
            },
            {
            HBITMAP_UserSize64
            ,HBITMAP_UserMarshal64
            ,HBITMAP_UserUnmarshal64
            ,HBITMAP_UserFree64
            },
            {
            VARIANT_UserSize64
            ,VARIANT_UserMarshal64
            ,VARIANT_UserUnmarshal64
            ,VARIANT_UserFree64
            }

        };



/* Standard interface: __MIDL_itf_USgfw2_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IUsgDataView, ver. 0.0,
   GUID={0xBFEF8670,0xDBC1,0x4B47,{0xBC,0x01,0xBB,0xBC,0xA1,0x74,0xDD,0x31}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgDataView_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag16,
    &__midl_frag21,
    &__midl_frag27,
    &__midl_frag60,
    &__midl_frag271
    };


static const MIDL_SYNTAX_INFO IUsgDataView_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataView_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDataView_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataView_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataView_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDataView_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDataView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDataView_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDataView_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgScanMode_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag53,
    &__midl_frag60,
    &__midl_frag85,
    &__midl_frag2092,
    &__midl_frag1616,
    &__midl_frag2088,
    &__midl_frag85
    };


static const MIDL_SYNTAX_INFO IUsgScanMode_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanMode_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanMode_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanMode_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanMode_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanMode_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanMode_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanMode_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDataStream_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag94,
    &__midl_frag100,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgDataStream_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataStream_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDataStream_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataStream_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataStream_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDataStream_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDataStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDataStream_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDataStream_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgMixerControl_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2092,
    &__midl_frag119,
    &__midl_frag143,
    &__midl_frag143,
    &__midl_frag146,
    &__midl_frag150,
    &__midl_frag2088,
    &__midl_frag2035,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag170,
    &__midl_frag174,
    &__midl_frag176
    };


static const MIDL_SYNTAX_INFO IUsgMixerControl_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgMixerControl_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgMixerControl_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgMixerControl_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgMixerControl_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgMixerControl_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgMixerControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgMixerControl_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgMixerControl_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgData_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    0
    };


static const MIDL_SYNTAX_INFO IUsgData_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgData_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgData_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgData_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgData_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgData_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgData_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgData_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgData_SyntaxInfo
    };
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
static const FormatInfoRef IUsgGraph_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    0
    };


static const MIDL_SYNTAX_INFO IUsgGraph_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgGraph_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgGraph_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgGraph_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgGraph_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgGraph_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgGraph_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgGraph_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgGraph_SyntaxInfo
    };
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
static const FormatInfoRef IUsgDeviceChangeSink_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag209,
    &__midl_frag209,
    &__midl_frag209,
    &__midl_frag209,
    &__midl_frag209,
    &__midl_frag209
    };


static const MIDL_SYNTAX_INFO IUsgDeviceChangeSink_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDeviceChangeSink_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceChangeSink_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDeviceChangeSink_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDeviceChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDeviceChangeSink_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IScanDepth_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2045
    };


static const MIDL_SYNTAX_INFO IScanDepth_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IScanDepth_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IScanDepth_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IScanDepth_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IScanDepth_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IScanDepth_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IScanDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IScanDepth_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IScanDepth_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgfw2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1616,
    &__midl_frag1616,
    &__midl_frag238,
    &__midl_frag1745,
    &__midl_frag316,
    &__midl_frag265,
    &__midl_frag316,
    &__midl_frag265,
    &__midl_frag271
    };


static const MIDL_SYNTAX_INFO IUsgfw2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgfw2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgfw2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgfw2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgfw2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgfw2_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IProbesCollection_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    0
    };


static const MIDL_SYNTAX_INFO IProbesCollection_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbesCollection_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IProbesCollection_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IProbesCollection_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbesCollection_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IProbesCollection_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IProbesCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IProbesCollection_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IProbesCollection_SyntaxInfo
    };
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
static const FormatInfoRef IUsgCollection_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag283
    };


static const MIDL_SYNTAX_INFO IUsgCollection_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCollection_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCollection_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCollection_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCollection_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCollection_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCollection_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCollection_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IProbe_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1736,
    &__midl_frag1616,
    &__midl_frag2088,
    &__midl_frag1736,
    &__midl_frag2088
    };


static const MIDL_SYNTAX_INFO IProbe_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbe_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IProbe_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IProbe_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbe_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IProbe_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IProbe_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IProbe_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IProbe_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IProbe2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1736,
    &__midl_frag1616,
    &__midl_frag2088,
    &__midl_frag1736,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag316
    };


static const MIDL_SYNTAX_INFO IProbe2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbe2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IProbe2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IProbe2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbe2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IProbe2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IProbe2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IProbe2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IProbe2_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IBeamformer_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1736,
    &__midl_frag1616,
    &__midl_frag2088,
    &__midl_frag1736,
    &__midl_frag1745,
    &__midl_frag1745,
    &__midl_frag2088,
    &__midl_frag2088
    };


static const MIDL_SYNTAX_INFO IBeamformer_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IBeamformer_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IBeamformer_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformer_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IBeamformer_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBeamformer_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IBeamformer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IBeamformer_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBeamformer_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IScanModes_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag1672
    };


static const MIDL_SYNTAX_INFO IScanModes_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IScanModes_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IScanModes_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IScanModes_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IScanModes_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IScanModes_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IScanModes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IScanModes_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IScanModes_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgControl_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    0
    };


static const MIDL_SYNTAX_INFO IUsgControl_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgControl_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgControl_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgControl_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgControl_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgControl_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgControl_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgControl_SyntaxInfo
    };
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
static const FormatInfoRef IUsgGain_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgGain_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgGain_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgGain_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgGain_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgGain_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgGain_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgGain_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgGain_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgGain_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgValues_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag378
    };


static const MIDL_SYNTAX_INFO IUsgValues_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgValues_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgValues_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgValues_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgValues_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgValues_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgValues_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgValues_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgValues_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgPower_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgPower_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPower_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgPower_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPower_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPower_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPower_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgPower_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgPower_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPower_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDynamicRange_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDynamicRange_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDynamicRange_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDynamicRange_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDynamicRange_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDynamicRange_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDynamicRange_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDynamicRange_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDynamicRange_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDynamicRange_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgFrameAvg_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgFrameAvg_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameAvg_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgFrameAvg_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFrameAvg_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameAvg_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFrameAvg_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgFrameAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgFrameAvg_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFrameAvg_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgRejection2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgRejection2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgRejection2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgRejection2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgRejection2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgRejection2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgRejection2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgRejection2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgRejection2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgRejection2_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgProbeFrequency2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgProbeFrequency2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgProbeFrequency2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgProbeFrequency2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgProbeFrequency2_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDepth_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDepth_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDepth_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDepth_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDepth_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDepth_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDepth_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDepth_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDepth_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgImageOrientation_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgImageOrientation_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageOrientation_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgImageOrientation_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageOrientation_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageOrientation_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgImageOrientation_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgImageOrientation_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgImageOrientation_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgImageOrientation_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgImageEnhancement_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgImageEnhancement_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgImageEnhancement_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageEnhancement_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgImageEnhancement_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgImageEnhancement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgImageEnhancement_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgViewArea_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgViewArea_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewArea_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgViewArea_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgViewArea_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewArea_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgViewArea_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgViewArea_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgViewArea_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgViewArea_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgLineDensity_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgLineDensity_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgLineDensity_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgLineDensity_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgLineDensity_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgLineDensity_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgLineDensity_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgLineDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgLineDensity_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgLineDensity_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgFocus_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag1672,
    &__midl_frag2035,
    &__midl_frag1672,
    &__midl_frag2071,
    &__midl_frag685,
    &__midl_frag692,
    &__midl_frag699,
    &__midl_frag708
    };


static const MIDL_SYNTAX_INFO IUsgFocus_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFocus_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgFocus_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFocus_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFocus_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFocus_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgFocus_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgFocus_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFocus_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgTgc_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2071,
    &__midl_frag840,
    &__midl_frag2088,
    &__midl_frag1672,
    &__midl_frag1672,
    &__midl_frag1672,
    &__midl_frag2035,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag1672,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgTgc_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTgc_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgTgc_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgTgc_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTgc_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgTgc_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgTgc_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgTgc_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgTgc_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgClearView_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgClearView_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgClearView_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgClearView_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgClearView_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgClearView_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgClearView_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgClearView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgClearView_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgClearView_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgPaletteCalculator_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag840,
    &__midl_frag846,
    &__midl_frag2071
    };


static const MIDL_SYNTAX_INFO IUsgPaletteCalculator_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgPaletteCalculator_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPaletteCalculator_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPaletteCalculator_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgPaletteCalculator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPaletteCalculator_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgPalette_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag860,
    &__midl_frag1193
    };


static const MIDL_SYNTAX_INFO IUsgPalette_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPalette_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgPalette_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPalette_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPalette_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPalette_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgPalette_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgPalette_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPalette_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgImageProperties_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag870,
    &__midl_frag876,
    &__midl_frag880,
    &__midl_frag885,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag911,
    &__midl_frag911,
    &__midl_frag920,
    &__midl_frag926,
    &__midl_frag937,
    &__midl_frag942
    };


static const MIDL_SYNTAX_INFO IUsgImageProperties_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageProperties_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgImageProperties_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgImageProperties_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageProperties_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgImageProperties_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgImageProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgImageProperties_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgImageProperties_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgControlChangeSink_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2035
    };


static const MIDL_SYNTAX_INFO IUsgControlChangeSink_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgControlChangeSink_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgControlChangeSink_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgControlChangeSink_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgControlChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgControlChangeSink_SyntaxInfo
    };
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
    NdrStubCall3
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
static const FormatInfoRef IUsgCtrlChangeCommon_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag955,
    &__midl_frag964
    };


static const MIDL_SYNTAX_INFO IUsgCtrlChangeCommon_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCtrlChangeCommon_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCtrlChangeCommon_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCtrlChangeCommon_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCtrlChangeCommon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCtrlChangeCommon_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgScanLine_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag1067,
    &__midl_frag1045,
    &__midl_frag1054
    };


static const MIDL_SYNTAX_INFO IUsgScanLine_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanLine_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLine_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLine_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanLine_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLine_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgScanLineStubVtbl =
{
    &IID_IUsgScanLine,
    &IUsgScanLine_ServerInfo,
    13,
    &IUsgScanLine_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLine2, ver. 0.0,
   GUID={0xB521CD25,0xEC30,0x486b,{0xB1,0xB7,0xBE,0x22,0x97,0x35,0xAA,0xBD}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgScanLine2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag1067,
    &__midl_frag1045,
    &__midl_frag1054,
    &__midl_frag1109,
    &__midl_frag1085,
    &__midl_frag1095
    };


static const MIDL_SYNTAX_INFO IUsgScanLine2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanLine2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLine2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLine2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanLine2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanLine2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLine2_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(16) _IUsgScanLine2ProxyVtbl = 
{
    &IUsgScanLine2_ProxyInfo,
    &IID_IUsgScanLine2,
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
    (void *) (INT_PTR) -1 /* IUsgScanLine::GetCoordinates2 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine2::GetScanLine2 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine2::GetCoordinates3 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLine2::GetCoordinates4 */
};


static const PRPC_STUB_FUNCTION IUsgScanLine2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgScanLine2StubVtbl =
{
    &IID_IUsgScanLine2,
    &IUsgScanLine2_ServerInfo,
    16,
    &IUsgScanLine2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLineProperties, ver. 0.0,
   GUID={0xEF1D6EDE,0x1AB3,0x429B,{0x83,0x48,0x6B,0xEA,0x1A,0x43,0x50,0x0E}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgScanLineProperties_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1045,
    &__midl_frag1054,
    &__midl_frag1067,
    &__midl_frag2045,
    &__midl_frag1079
    };


static const MIDL_SYNTAX_INFO IUsgScanLineProperties_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanLineProperties_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLineProperties_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanLineProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLineProperties_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgScanLinePropertiesStubVtbl =
{
    &IID_IUsgScanLineProperties,
    &IUsgScanLineProperties_ServerInfo,
    12,
    &IUsgScanLineProperties_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLineProperties2, ver. 0.0,
   GUID={0xA4F7329D,0x51A7,0x4a61,{0xA9,0xA8,0xCF,0xE9,0x0A,0x90,0xC9,0x04}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgScanLineProperties2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1045,
    &__midl_frag1054,
    &__midl_frag1067,
    &__midl_frag2045,
    &__midl_frag1079,
    &__midl_frag1085,
    &__midl_frag1095,
    &__midl_frag1109,
    &__midl_frag1116
    };


static const MIDL_SYNTAX_INFO IUsgScanLineProperties2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanLineProperties2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLineProperties2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanLineProperties2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanLineProperties2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLineProperties2_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(16) _IUsgScanLineProperties2ProxyVtbl = 
{
    &IUsgScanLineProperties2_ProxyInfo,
    &IID_IUsgScanLineProperties2,
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
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties::GetPointDepth */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties2::GetCoordinates3 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties2::GetCoordinates4 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties2::GetScanLine2 */ ,
    (void *) (INT_PTR) -1 /* IUsgScanLineProperties2::GetPointDepth2 */
};


static const PRPC_STUB_FUNCTION IUsgScanLineProperties2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgScanLineProperties2StubVtbl =
{
    &IID_IUsgScanLineProperties2,
    &IUsgScanLineProperties2_ServerInfo,
    16,
    &IUsgScanLineProperties2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgScanLineSelector, ver. 0.0,
   GUID={0x3BFE461D,0x4240,0x40AC,{0xB5,0xFF,0x29,0x2A,0x6C,0x25,0x3A,0x4C}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgScanLineSelector_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgScanLineSelector_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanLineSelector_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineSelector_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLineSelector_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanLineSelector_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanLineSelector_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgSweepMode_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgSweepMode_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepMode_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgSweepMode_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepMode_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepMode_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSweepMode_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgSweepMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgSweepMode_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSweepMode_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgQualProp_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088
    };


static const MIDL_SYNTAX_INFO IUsgQualProp_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgQualProp_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgQualProp_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgQualProp_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgQualProp_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgQualProp_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgQualProp_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgQualProp_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgQualProp_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgSweepSpeed_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag1178
    };


static const MIDL_SYNTAX_INFO IUsgSweepSpeed_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgSweepSpeed_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepSpeed_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSweepSpeed_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgSweepSpeed_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSweepSpeed_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerColorMap_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1187,
    &__midl_frag1193
    };


static const MIDL_SYNTAX_INFO IUsgDopplerColorMap_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerColorMap_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorMap_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorMap_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerColorMap_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorMap_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerColorPriority_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgDopplerColorPriority_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerColorPriority_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorPriority_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorPriority_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerColorPriority_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorPriority_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerSteerAngle_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerSteerAngle_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerSteerAngle_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSteerAngle_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSteerAngle_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerSteerAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSteerAngle_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerColorThreshold_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerColorThreshold_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerColorThreshold_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorThreshold_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorThreshold_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerColorThreshold_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorThreshold_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerBaseLine_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerBaseLine_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerBaseLine_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerBaseLine_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerBaseLine_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerBaseLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerBaseLine_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerPRF_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag1672,
    &__midl_frag1672
    };


static const MIDL_SYNTAX_INFO IUsgDopplerPRF_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerPRF_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPRF_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerPRF_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerPRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerPRF_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerWindow_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2015,
    &__midl_frag2021,
    &__midl_frag2035,
    &__midl_frag2035,
    &__midl_frag2045,
    &__midl_frag2045,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag2060
    };


static const MIDL_SYNTAX_INFO IUsgDopplerWindow_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerWindow_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWindow_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerWindow_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerWindow_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerWallFilter_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag1672,
    &__midl_frag1672
    };


static const MIDL_SYNTAX_INFO IUsgDopplerWallFilter_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerWallFilter_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWallFilter_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerWallFilter_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerWallFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerWallFilter_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerSignalScale_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerSignalScale_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerSignalScale_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalScale_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSignalScale_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalScale_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSignalScale_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerPacketSize_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerPacketSize_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerPacketSize_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerPacketSize_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerPacketSize_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerPacketSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerPacketSize_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgPulsesNumber_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgPulsesNumber_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgPulsesNumber_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPulsesNumber_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPulsesNumber_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgPulsesNumber_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPulsesNumber_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgCineloop_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1452,
    &__midl_frag1763,
    &__midl_frag1452,
    &__midl_frag1456,
    &__midl_frag1460,
    &__midl_frag1452,
    &__midl_frag1763,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2045,
    &__midl_frag1420,
    &__midl_frag1497,
    &__midl_frag2088,
    &__midl_frag1434
    };


static const MIDL_SYNTAX_INFO IUsgCineloop_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineloop_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCineloop_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineloop_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineloop_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineloop_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCineloop_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCineloop_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineloop_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgCineStream_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1452,
    &__midl_frag1763,
    &__midl_frag1452,
    &__midl_frag1763,
    &__midl_frag1452,
    &__midl_frag1456,
    &__midl_frag1460,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag1756,
    &__midl_frag1501,
    &__midl_frag1490,
    &__midl_frag1497,
    &__midl_frag1501,
    &__midl_frag1672,
    &__midl_frag1512,
    &__midl_frag1519
    };


static const MIDL_SYNTAX_INFO IUsgCineStream_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineStream_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCineStream_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineStream_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineStream_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineStream_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCineStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCineStream_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineStream_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgCineSink_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1524,
    &__midl_frag1745,
    &__midl_frag2092,
    &__midl_frag1741
    };


static const MIDL_SYNTAX_INFO IUsgCineSink_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCineSink_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineSink_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineSink_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCineSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCineSink_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineSink_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgCineSinkStubVtbl =
{
    &IID_IUsgCineSink,
    &IUsgCineSink_ServerInfo,
    11,
    &IUsgCineSink_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgCineSink2, ver. 0.0,
   GUID={0x9EB34740,0x8AA5,0x4373,{0xA1,0xFF,0xAC,0x9A,0x86,0x4D,0x5A,0xBB}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgCineSink2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1541,
    &__midl_frag1745,
    &__midl_frag1763,
    &__midl_frag1741
    };


static const MIDL_SYNTAX_INFO IUsgCineSink2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCineSink2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineSink2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineSink2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCineSink2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCineSink2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCineSink2_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(11) _IUsgCineSink2ProxyVtbl = 
{
    &IUsgCineSink2_ProxyInfo,
    &IID_IUsgCineSink2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink2::GetBufferSize */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink2::Reset */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink2::SetBufferSize */ ,
    (void *) (INT_PTR) -1 /* IUsgCineSink2::SaveData */
};


static const PRPC_STUB_FUNCTION IUsgCineSink2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgCineSink2StubVtbl =
{
    &IID_IUsgCineSink2,
    &IUsgCineSink2_ServerInfo,
    11,
    &IUsgCineSink2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgSpatialFilter, ver. 0.0,
   GUID={0x31BF1183,0x35D2,0x4EF3,{0xA1,0xB1,0xAD,0xEB,0xDE,0xDD,0x81,0xA0}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgSpatialFilter_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgSpatialFilter_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgSpatialFilter_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSpatialFilter_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSpatialFilter_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgSpatialFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSpatialFilter_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerSampleVolume_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2035,
    &__midl_frag2045,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag1593,
    &__midl_frag2045
    };


static const MIDL_SYNTAX_INFO IUsgDopplerSampleVolume_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerSampleVolume_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSampleVolume_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSampleVolume_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerSampleVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSampleVolume_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerCorrectionAngle_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerCorrectionAngle_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerCorrectionAngle_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerCorrectionAngle_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerCorrectionAngle_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerCorrectionAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerCorrectionAngle_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgScanConverterPlugin_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1616
    };


static const MIDL_SYNTAX_INFO IUsgScanConverterPlugin_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanConverterPlugin_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanConverterPlugin_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanConverterPlugin_SyntaxInfo
    };
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
    NdrStubCall3
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
static const FormatInfoRef IUsgScanConverterPlugin2_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1616,
    &__midl_frag2088,
    &__midl_frag2088
    };


static const MIDL_SYNTAX_INFO IUsgScanConverterPlugin2_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanConverterPlugin2_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanConverterPlugin2_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanConverterPlugin2_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerSignalSmooth_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerSignalSmooth_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerSignalSmooth_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSignalSmooth_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSignalSmooth_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalSmooth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSignalSmooth_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgAudioVolume_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgAudioVolume_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgAudioVolume_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgAudioVolume_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgAudioVolume_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgAudioVolume_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgAudioVolume_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgAudioVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgAudioVolume_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgAudioVolume_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerSpectralAvg_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDopplerSpectralAvg_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerSpectralAvg_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSpectralAvg_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSpectralAvg_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerSpectralAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerSpectralAvg_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgBioeffectsIndices_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1672,
    &__midl_frag1677
    };


static const MIDL_SYNTAX_INFO IUsgBioeffectsIndices_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgBioeffectsIndices_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgBioeffectsIndices_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgBioeffectsIndices_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgBioeffectsIndices_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgBioeffectsIndices_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgProbeFrequency3_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1682,
    &__midl_frag1686,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgProbeFrequency3_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgProbeFrequency3_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency3_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgProbeFrequency3_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency3_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgProbeFrequency3_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDopplerColorTransparency_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgDopplerColorTransparency_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDopplerColorTransparency_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorTransparency_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorTransparency_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDopplerColorTransparency_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDopplerColorTransparency_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsg3dVolumeSize_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsg3dVolumeSize_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsg3dVolumeSize_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeSize_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsg3dVolumeSize_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsg3dVolumeSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsg3dVolumeSize_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsg3dVolumeDensity_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsg3dVolumeDensity_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsg3dVolumeDensity_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsg3dVolumeDensity_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsg3dVolumeDensity_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsg3dVolumeDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsg3dVolumeDensity_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgFileStorage_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1736,
    &__midl_frag1741,
    &__midl_frag1745,
    &__midl_frag2092,
    &__midl_frag1750,
    &__midl_frag1756,
    &__midl_frag1763,
    &__midl_frag1766,
    &__midl_frag1773,
    &__midl_frag1781,
    &__midl_frag1787,
    &__midl_frag1799
    };


static const MIDL_SYNTAX_INFO IUsgFileStorage_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFileStorage_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgFileStorage_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFileStorage_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFileStorage_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFileStorage_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgFileStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgFileStorage_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFileStorage_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgfw2Debug_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1809
    };


static const MIDL_SYNTAX_INFO IUsgfw2Debug_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2Debug_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgfw2Debug_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2Debug_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2Debug_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgfw2Debug_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgfw2Debug_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgfw2Debug_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgfw2Debug_SyntaxInfo
    };
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
    NdrStubCall3
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
static const FormatInfoRef IUsgPlugin_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1815,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgPlugin_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPlugin_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgPlugin_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPlugin_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPlugin_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgPlugin_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgPlugin_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IBeamformerPowerState_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2088
    };


static const MIDL_SYNTAX_INFO IBeamformerPowerState_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IBeamformerPowerState_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IBeamformerPowerState_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformerPowerState_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IBeamformerPowerState_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBeamformerPowerState_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IBeamformerPowerState_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IBeamformerPowerState_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBeamformerPowerState_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgScanType_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgScanType_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanType_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgScanType_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanType_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanType_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanType_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgScanType_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgScanType_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgScanType_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgSteeringAngle_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgSteeringAngle_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgSteeringAngle_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSteeringAngle_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSteeringAngle_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgSteeringAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgSteeringAngle_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgViewAngle_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgViewAngle_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewAngle_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgViewAngle_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgViewAngle_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewAngle_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgViewAngle_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgViewAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgViewAngle_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgViewAngle_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgCompoundFrames_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgCompoundFrames_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCompoundFrames_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCompoundFrames_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCompoundFrames_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCompoundFrames_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCompoundFrames_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgCompoundAngle_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgCompoundAngle_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgCompoundAngle_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCompoundAngle_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCompoundAngle_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgCompoundAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgCompoundAngle_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDeviceCapabilities_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1904
    };


static const MIDL_SYNTAX_INFO IUsgDeviceCapabilities_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDeviceCapabilities_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceCapabilities_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDeviceCapabilities_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDeviceCapabilities_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDeviceCapabilities_SyntaxInfo
    };
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
    NdrStubCall3
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
static const FormatInfoRef IUsgUnits_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1912,
    &__midl_frag1918,
    &__midl_frag1923
    };


static const MIDL_SYNTAX_INFO IUsgUnits_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgUnits_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgUnits_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgUnits_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgUnits_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgUnits_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgUnits_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgUnits_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgUnits_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef ISampleGrabberFilter_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag1930
    };


static const MIDL_SYNTAX_INFO ISampleGrabberFilter_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &ISampleGrabberFilter_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &ISampleGrabberFilter_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO ISampleGrabberFilter_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &ISampleGrabberFilter_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ISampleGrabberFilter_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO ISampleGrabberFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &ISampleGrabberFilter_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ISampleGrabberFilter_SyntaxInfo
    };
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
    NdrStubCall3
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
static const FormatInfoRef IUsgWindowRF_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2015,
    &__midl_frag2021,
    &__midl_frag2035,
    &__midl_frag2035,
    &__midl_frag2045,
    &__midl_frag2045,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag2060
    };


static const MIDL_SYNTAX_INFO IUsgWindowRF_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgWindowRF_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgWindowRF_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgWindowRF_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgWindowRF_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgWindowRF_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgWindowRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgWindowRF_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgWindowRF_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgStreamEnable_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgStreamEnable_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgStreamEnable_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgStreamEnable_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgStreamEnable_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgStreamEnable_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgStreamEnable_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgStreamEnable_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgStreamEnable_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgStreamEnable_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgDataSourceRF_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgDataSourceRF_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataSourceRF_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgDataSourceRF_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDataSourceRF_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataSourceRF_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDataSourceRF_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgDataSourceRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgDataSourceRF_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgDataSourceRF_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IUsgMultiBeam_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2010
    };


static const MIDL_SYNTAX_INFO IUsgMultiBeam_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgMultiBeam_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgMultiBeam_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgMultiBeam_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgMultiBeam_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgMultiBeam_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgMultiBeam_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgMultiBeam_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgMultiBeam_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgMultiBeamStubVtbl =
{
    &IID_IUsgMultiBeam,
    &IUsgMultiBeam_ServerInfo,
    10,
    &IUsgMultiBeam_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgFrameROI, ver. 0.0,
   GUID={0x525055A7,0xB4AD,0x4A89,{0x85,0xE8,0x8F,0xCC,0xF5,0x1F,0x9D,0x38}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgFrameROI_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2015,
    &__midl_frag2021,
    &__midl_frag2035,
    &__midl_frag2035,
    &__midl_frag2045,
    &__midl_frag2045,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag2060,
    &__midl_frag2060
    };


static const MIDL_SYNTAX_INFO IUsgFrameROI_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameROI_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgFrameROI_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFrameROI_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameROI_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFrameROI_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgFrameROI_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgFrameROI_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgFrameROI_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(17) _IUsgFrameROIProxyVtbl = 
{
    &IUsgFrameROI_ProxyInfo,
    &IID_IUsgFrameROI,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::SetWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::GetWindow */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::SetWidth */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::SetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::GetWidth */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::GetVolume */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::get_LinesRange */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::get_WidthRange */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::get_DepthsRange */ ,
    (void *) (INT_PTR) -1 /* IUsgFrameROI::get_VolumesRange */
};


static const PRPC_STUB_FUNCTION IUsgFrameROI_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgFrameROIStubVtbl =
{
    &IID_IUsgFrameROI,
    &IUsgFrameROI_ServerInfo,
    17,
    &IUsgFrameROI_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgProbeElementsTest, ver. 0.0,
   GUID={0x06AF0C84,0xB1E3,0x411a,{0x93,0x63,0xE7,0x32,0xD1,0x65,0x9B,0xC0}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgProbeElementsTest_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2071
    };


static const MIDL_SYNTAX_INFO IUsgProbeElementsTest_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeElementsTest_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgProbeElementsTest_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeElementsTest_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeElementsTest_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgProbeElementsTest_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgProbeElementsTest_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgProbeElementsTest_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgProbeElementsTest_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(10) _IUsgProbeElementsTestProxyVtbl = 
{
    &IUsgProbeElementsTest_ProxyInfo,
    &IID_IUsgProbeElementsTest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeElementsTest::get_TestResult */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeElementsTest::get_NumberOfElements */ ,
    (void *) (INT_PTR) -1 /* IUsgProbeElementsTest::GetElementsTestResult */
};


static const PRPC_STUB_FUNCTION IUsgProbeElementsTest_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgProbeElementsTestStubVtbl =
{
    &IID_IUsgProbeElementsTest,
    &IUsgProbeElementsTest_ServerInfo,
    10,
    &IUsgProbeElementsTest_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IUsgTissueMotionDetector, ver. 0.0,
   GUID={0x7C9EB8CB,0x04DF,0x4F02,{0xA2,0x54,0xE3,0xF6,0x3E,0xFE,0xD9,0xB5}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IUsgTissueMotionDetector_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2088,
    &__midl_frag2088,
    &__midl_frag2092,
    &__midl_frag2088,
    &__midl_frag2092
    };


static const MIDL_SYNTAX_INFO IUsgTissueMotionDetector_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTissueMotionDetector_FormatStringOffsetTable[-3],
    USgfw2__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IUsgTissueMotionDetector_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgTissueMotionDetector_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTissueMotionDetector_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgTissueMotionDetector_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IUsgTissueMotionDetector_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    (unsigned short *) &IUsgTissueMotionDetector_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IUsgTissueMotionDetector_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(12) _IUsgTissueMotionDetectorProxyVtbl = 
{
    &IUsgTissueMotionDetector_ProxyInfo,
    &IID_IUsgTissueMotionDetector,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IUsgTissueMotionDetector::get_DetectionResult */ ,
    (void *) (INT_PTR) -1 /* IUsgTissueMotionDetector::get_Timeout */ ,
    (void *) (INT_PTR) -1 /* IUsgTissueMotionDetector::put_Timeout */ ,
    (void *) (INT_PTR) -1 /* IUsgTissueMotionDetector::get_EnableDetection */ ,
    (void *) (INT_PTR) -1 /* IUsgTissueMotionDetector::put_EnableDetection */
};


static const PRPC_STUB_FUNCTION IUsgTissueMotionDetector_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _IUsgTissueMotionDetectorStubVtbl =
{
    &IID_IUsgTissueMotionDetector,
    &IUsgTissueMotionDetector_ServerInfo,
    12,
    &IUsgTissueMotionDetector_table[-3],
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
    USgfw2__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x60001, /* Ndr library version */
    0,
    0x801026e, /* MIDL Version 8.1.622 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x2000001, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _USgfw2_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IUsgProbeFrequency3ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCompoundFramesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgCineStreamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDeviceChangeSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgImageEnhancementProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSpectralAvgProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerPRFProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLineSelectorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLine2ProxyVtbl,
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
    ( CInterfaceProxyVtbl *) &_IUsgCineSink2ProxyVtbl,
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
    ( CInterfaceProxyVtbl *) &_IUsgProbeElementsTestProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgControlChangeSinkProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerColorPriorityProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgMultiBeamProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerBaseLineProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgRejection2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDopplerSampleVolumeProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgStreamEnableProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgfw2DebugProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgScanLineProperties2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgDepthProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgPulsesNumberProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgClearViewProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgSweepSpeedProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgUnitsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgImagePropertiesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IUsgFrameROIProxyVtbl,
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
    ( CInterfaceProxyVtbl *) &_IUsgTissueMotionDetectorProxyVtbl,
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

const CInterfaceStubVtbl * const _USgfw2_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IUsgProbeFrequency3StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCompoundFramesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgCineStreamStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDeviceChangeSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgImageEnhancementStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSpectralAvgStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerPRFStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanLineSelectorStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanLine2StubVtbl,
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
    ( CInterfaceStubVtbl *) &_IUsgCineSink2StubVtbl,
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
    ( CInterfaceStubVtbl *) &_IUsgProbeElementsTestStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgControlChangeSinkStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerColorPriorityStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgMultiBeamStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerBaseLineStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgRejection2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDopplerSampleVolumeStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgStreamEnableStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgfw2DebugStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgScanLineProperties2StubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgDepthStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgPulsesNumberStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgClearViewStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgSweepSpeedStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgUnitsStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgImagePropertiesStubVtbl,
    ( CInterfaceStubVtbl *) &_IUsgFrameROIStubVtbl,
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
    ( CInterfaceStubVtbl *) &_IUsgTissueMotionDetectorStubVtbl,
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

PCInterfaceName const _USgfw2_InterfaceNamesList[] = 
{
    "IUsgProbeFrequency3",
    "IUsgCompoundFrames",
    "IUsgCineStream",
    "IUsgDeviceChangeSink",
    "IUsgImageEnhancement",
    "IUsgDopplerSpectralAvg",
    "IUsgDopplerPRF",
    "IUsgScanLineSelector",
    "IUsgScanLine2",
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
    "IUsgCineSink2",
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
    "IUsgProbeElementsTest",
    "IUsgControlChangeSink",
    "IUsgDopplerColorPriority",
    "IUsgMultiBeam",
    "IUsgDopplerBaseLine",
    "IUsgRejection2",
    "IUsgDopplerSampleVolume",
    "IUsgStreamEnable",
    "IUsgfw2Debug",
    "IUsgScanLineProperties2",
    "IUsgDepth",
    "IUsgPulsesNumber",
    "IUsgClearView",
    "IUsgSweepSpeed",
    "IUsgUnits",
    "IUsgImageProperties",
    "IUsgFrameROI",
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
    "IUsgTissueMotionDetector",
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

const IID *  const _USgfw2_BaseIIDList[] = 
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
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _USgfw2_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _USgfw2, pIID, n)

int __stdcall _USgfw2_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _USgfw2, 91, 64 )
    IID_BS_LOOKUP_NEXT_TEST( _USgfw2, 32 )
    IID_BS_LOOKUP_NEXT_TEST( _USgfw2, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _USgfw2, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _USgfw2, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _USgfw2, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _USgfw2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _USgfw2, 91, *pIndex )
    
}

const ExtendedProxyFileInfo USgfw2_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _USgfw2_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _USgfw2_StubVtblList,
    (const PCInterfaceName * ) & _USgfw2_InterfaceNamesList,
    (const IID ** ) & _USgfw2_BaseIIDList,
    & _USgfw2_IID_Lookup, 
    91,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

