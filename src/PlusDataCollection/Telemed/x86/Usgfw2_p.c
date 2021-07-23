

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for USgfw2.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


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


#include "USgfw2.h"

#define TYPE_FORMAT_STRING_SIZE   1859                              
#define PROC_FORMAT_STRING_SIZE   8287                              
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


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif
#if !(TARGET_IS_NT60_OR_LATER)
#error You need Windows Vista or later to run this stub because it uses these features:
#error   forced complex structure or array, new range semantics, compiled for Windows Vista.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
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
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x1 ),	/* 1 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fileName */


	/* Parameter FileName */

/* 336 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 340 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

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
/* 352 */	NdrFcShort( 0xa6 ),	/* Type Offset=166 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 400 */	NdrFcShort( 0xbc ),	/* Type Offset=188 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 558 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 562 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 676 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppMixerControl */

/* 708 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 712 */	NdrFcShort( 0xbc ),	/* Type Offset=188 */

	/* Return value */

/* 714 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 718 */	0x8,		/* FC_LONG */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 882 */	NdrFcShort( 0x1 ),	/* 1 */
/* 884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 888 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 890 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 892 */	NdrFcShort( 0x17a ),	/* Type Offset=378 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x1 ),	/* 1 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 924 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 926 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 928 */	NdrFcShort( 0x17a ),	/* Type Offset=378 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x1 ),	/* 1 */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pBmpParams */

/* 960 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 964 */	NdrFcShort( 0x17a ),	/* Type Offset=378 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRect */

/* 996 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1000 */	NdrFcShort( 0x170 ),	/* Type Offset=368 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ppCurrentImage */

/* 1032 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1034 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1036 */	NdrFcShort( 0x19e ),	/* Type Offset=414 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1216 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1218 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 1220 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1222 */	NdrFcShort( 0x1b8 ),	/* Type Offset=440 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 1254 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 1256 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1258 */	NdrFcShort( 0x1b8 ),	/* Type Offset=440 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRect */

/* 1290 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1292 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1294 */	NdrFcShort( 0x170 ),	/* Type Offset=368 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1324 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1326 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1328 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1330 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1368 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1370 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1372 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1410 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1412 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1414 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1450 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1452 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1454 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1456 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1492 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgProbe */

/* 1494 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1496 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1498 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUsgBeamformer */

/* 1536 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1538 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1540 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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

	/* Procedure get_ScanConverter */


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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1618 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 1620 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1622 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1624 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */


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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1654 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 1656 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1658 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1660 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 1686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1690 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pUnkSource */

/* 1692 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1696 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter ppUsgDataView */

/* 1698 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1702 */	NdrFcShort( 0x1ca ),	/* Type Offset=458 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
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
/* 1774 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1804 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Name */

/* 1806 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1808 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1810 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
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
/* 1858 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1886 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1888 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Name */

/* 1890 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1892 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1894 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 1942 */	NdrFcShort( 0x1f2 ),	/* Type Offset=498 */

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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 1968 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1972 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 1974 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1976 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1978 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 2004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2008 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2010 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2012 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2014 */	NdrFcShort( 0x1f2 ),	/* Type Offset=498 */

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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 2040 */	NdrFcShort( 0x1 ),	/* 1 */
/* 2042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2044 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 2046 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2048 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2050 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 2296 */	NdrFcShort( 0x208 ),	/* Type Offset=520 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
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
/* 2338 */	NdrFcShort( 0x680 ),	/* Type Offset=1664 */

	/* Return value */

/* 2340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2342 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_EnableDetection */


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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 2602 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
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
/* 2710 */	NdrFcShort( 0x69a ),	/* Type Offset=1690 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
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
/* 2764 */	NdrFcShort( 0x69a ),	/* Type Offset=1690 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3088 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 3090 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3092 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3094 */	NdrFcShort( 0x208 ),	/* Type Offset=520 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 3574 */	NdrFcShort( 0x1b8 ),	/* Type Offset=440 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 3622 */	NdrFcShort( 0x1b8 ),	/* Type Offset=440 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3652 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter point */

/* 3654 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3656 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3658 */	NdrFcShort( 0x5dc ),	/* Type Offset=1500 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3700 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter point */

/* 3702 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3704 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3706 */	NdrFcShort( 0x5dc ),	/* Type Offset=1500 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3742 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter resolution */

/* 3744 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 3746 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3748 */	NdrFcShort( 0x5dc ),	/* Type Offset=1500 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 3780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3784 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter resolution */

/* 3786 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 3788 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3790 */	NdrFcShort( 0x5dc ),	/* Type Offset=1500 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
/* 4222 */	NdrFcShort( 0x6c0 ),	/* Type Offset=1728 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 4260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4262 */	NdrFcShort( 0x1 ),	/* 1 */
/* 4264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ctrlGUID */

/* 4266 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 4268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4270 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

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
/* 4288 */	NdrFcShort( 0x6c0 ),	/* Type Offset=1728 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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

	/* Procedure GetScanLine2 */

/* 4482 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4488 */	NdrFcShort( 0xd ),	/* 13 */
/* 4490 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4492 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4494 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4496 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4498 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4504 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter subFrame */

/* 4506 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4508 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X */

/* 4512 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4514 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4516 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4518 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4520 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4522 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4524 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4526 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4530 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4532 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates3 */

/* 4536 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4542 */	NdrFcShort( 0xe ),	/* 14 */
/* 4544 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4546 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4548 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4550 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 4552 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4558 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4560 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4562 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 4566 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4568 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4572 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4574 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4576 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 4578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4580 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4582 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4584 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4586 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4588 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4590 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4592 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates4 */

/* 4596 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4598 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4602 */	NdrFcShort( 0xf ),	/* 15 */
/* 4604 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 4606 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4608 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4610 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x9,		/* 9 */
/* 4612 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4618 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4620 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4622 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 4626 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4628 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4632 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4634 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4636 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4638 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4640 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4642 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4644 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4646 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4648 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 4650 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4652 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4654 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4656 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4658 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4660 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4662 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4664 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4666 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4670 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 4672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates */

/* 4674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4680 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4682 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4686 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4688 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 4690 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4696 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4698 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4700 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4704 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4706 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4708 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 4710 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4712 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4714 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4716 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4718 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4720 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4722 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4724 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates2 */

/* 4728 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4730 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4734 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4736 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 4738 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4740 */	NdrFcShort( 0x78 ),	/* 120 */
/* 4742 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x8,		/* 8 */
/* 4744 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4750 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4752 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4754 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 4758 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4760 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4762 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 4764 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4766 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4768 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 4770 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4772 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4774 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 4776 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4778 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4780 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 4782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4784 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4786 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 4788 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4790 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4792 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4796 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine */

/* 4800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4806 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4808 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4810 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4812 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4814 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4816 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4822 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter X */

/* 4824 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4826 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4828 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4830 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4832 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4834 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4836 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4838 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4840 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4842 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4844 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLinesRange */

/* 4848 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4850 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4854 */	NdrFcShort( 0xa ),	/* 10 */
/* 4856 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4858 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4860 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4862 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 4864 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4870 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */

/* 4872 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4874 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */

/* 4878 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4880 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4884 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4886 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointDepth */

/* 4890 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4892 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4896 */	NdrFcShort( 0xb ),	/* 11 */
/* 4898 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4900 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4902 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4904 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 4906 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4908 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4912 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter X */

/* 4914 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4916 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4918 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4920 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4922 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4924 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 4926 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4928 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4930 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4932 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4934 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates3 */

/* 4938 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4940 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4944 */	NdrFcShort( 0xc ),	/* 12 */
/* 4946 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4948 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4950 */	NdrFcShort( 0x5c ),	/* 92 */
/* 4952 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 4954 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 4956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4960 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 4962 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4964 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 4968 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4970 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter angle */

/* 4974 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4976 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4978 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X */

/* 4980 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4982 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4984 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 4986 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4988 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4990 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 4992 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4994 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCoordinates4 */

/* 4998 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5000 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5004 */	NdrFcShort( 0xd ),	/* 13 */
/* 5006 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 5008 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5010 */	NdrFcShort( 0x78 ),	/* 120 */
/* 5012 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x9,		/* 9 */
/* 5014 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5020 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter scanLine */

/* 5022 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5024 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subFrame */

/* 5028 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5030 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X1 */

/* 5034 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5036 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5038 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y1 */

/* 5040 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5042 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5044 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter X2 */

/* 5046 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5048 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5050 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y2 */

/* 5052 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5054 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5056 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter startDepth */

/* 5058 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5060 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5062 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 5064 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5066 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5068 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5072 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 5074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScanLine2 */

/* 5076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5082 */	NdrFcShort( 0xe ),	/* 14 */
/* 5084 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5086 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5088 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5090 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5092 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5098 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter subFrame */

/* 5100 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X */

/* 5106 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5110 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5114 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5116 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 5118 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5120 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5122 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5124 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5126 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointDepth2 */

/* 5130 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5132 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5136 */	NdrFcShort( 0xf ),	/* 15 */
/* 5138 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5140 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5142 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5144 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5146 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5152 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter subFrame */

/* 5154 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5156 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5158 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter X */

/* 5160 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5162 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5164 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter Y */

/* 5166 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5168 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5170 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter value */

/* 5172 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5174 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5176 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5180 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_BatteryChargeStatus */


	/* Procedure get_Current */

/* 5184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5190 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5192 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5196 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5198 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5200 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5206 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5210 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5212 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5216 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 5220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5228 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5234 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5236 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5242 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 5244 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5246 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5248 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 5250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5252 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Timeout */


	/* Procedure get_NumberOfElements */


	/* Procedure get_BatteryFullLifetime */


	/* Procedure get_InputPin */


	/* Procedure get_FramesDrawn */

/* 5256 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5262 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5264 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5268 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5270 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5272 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5278 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5280 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5282 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 5286 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5288 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5290 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Preset */

/* 5292 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5294 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5298 */	NdrFcShort( 0xc ),	/* 12 */
/* 5300 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5302 */	NdrFcShort( 0x18 ),	/* 24 */
/* 5304 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5306 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 5308 */	0x8,		/* 8 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 5310 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5312 */	NdrFcShort( 0x1 ),	/* 1 */
/* 5314 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cEntries */

/* 5316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5318 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5320 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValues */

/* 5322 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5324 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5326 */	NdrFcShort( 0x6d6 ),	/* Type Offset=1750 */

	/* Parameter iCurrent */

/* 5328 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5330 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter SweepTime */

/* 5334 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5336 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5340 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5342 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5344 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetColorMap */

/* 5346 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5352 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5354 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5356 */	NdrFcShort( 0x60 ),	/* 96 */
/* 5358 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5360 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5362 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5368 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pTable1 */

/* 5370 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5372 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5374 */	NdrFcShort( 0x1b8 ),	/* Type Offset=440 */

	/* Parameter size1 */

/* 5376 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5378 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTable2 */

/* 5382 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5384 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5386 */	NdrFcShort( 0x1b8 ),	/* Type Offset=440 */

	/* Parameter size2 */

/* 5388 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5390 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5392 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5394 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5396 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5398 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWindow */


	/* Procedure SetWindow */


	/* Procedure SetWindow */

/* 5400 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5402 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5406 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5408 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5410 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5412 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5414 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5416 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5418 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5422 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5424 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5426 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5430 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5432 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5436 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5438 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5442 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5444 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5446 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5448 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5450 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindow */


	/* Procedure GetWindow */


	/* Procedure GetWindow */

/* 5454 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5462 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5466 */	NdrFcShort( 0x78 ),	/* 120 */
/* 5468 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 5470 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5476 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5478 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5480 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5482 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5484 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5486 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5490 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5492 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5496 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5498 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5502 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5504 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5506 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetWidth */


	/* Procedure SetWidth */


	/* Procedure SetWidth */

/* 5508 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5510 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5514 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5516 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5518 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5520 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5522 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5524 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5532 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5540 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5544 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetVolume */


	/* Procedure SetVolume */


	/* Procedure SetVolume */

/* 5550 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5552 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5556 */	NdrFcShort( 0xa ),	/* 10 */
/* 5558 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5560 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5564 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5566 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5572 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5574 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5576 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5580 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5582 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5584 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5588 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWidth */


	/* Procedure GetWidth */


	/* Procedure GetWidth */

/* 5592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5598 */	NdrFcShort( 0xb ),	/* 11 */
/* 5600 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5604 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5606 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5608 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5614 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startLine */


	/* Parameter startLine */


	/* Parameter startLine */

/* 5616 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5618 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endLine */


	/* Parameter endLine */


	/* Parameter endLine */

/* 5622 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5624 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5628 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5630 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5632 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */


	/* Procedure GetVolume */


	/* Procedure GetPhantomVolume */


	/* Procedure GetVolume */

/* 5634 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5636 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5640 */	NdrFcShort( 0xc ),	/* 12 */
/* 5642 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5646 */	NdrFcShort( 0x40 ),	/* 64 */
/* 5648 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 5650 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5656 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */


	/* Parameter startDepth */

/* 5658 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5660 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5662 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */


	/* Parameter endDepth */

/* 5664 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5666 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5668 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 5670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5672 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_LinesRange */


	/* Procedure get_LinesRange */


	/* Procedure get_LinesRange */

/* 5676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5682 */	NdrFcShort( 0xd ),	/* 13 */
/* 5684 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5688 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5690 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5692 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5698 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5700 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5702 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5704 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5708 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_WidthRange */


	/* Procedure get_WidthRange */


	/* Procedure get_WidthRange */

/* 5712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5718 */	NdrFcShort( 0xe ),	/* 14 */
/* 5720 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5724 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5726 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5728 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5734 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5736 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5738 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5740 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5742 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5744 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */


	/* Procedure get_DepthsRange */


	/* Procedure get_DepthsRange */

/* 5748 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5750 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5754 */	NdrFcShort( 0xf ),	/* 15 */
/* 5756 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5760 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5762 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5764 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5768 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5770 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5772 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5774 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5776 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5778 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5780 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */


	/* Procedure get_VolumesRange */


	/* Procedure get_VolumesRange */

/* 5784 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5786 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5790 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5792 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5796 */	NdrFcShort( 0x38 ),	/* 56 */
/* 5798 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5800 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5806 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */


	/* Parameter pVal */

/* 5808 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 5810 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5812 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5814 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5816 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5818 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_CurrentPosition */


	/* Procedure get_CurrentPosition */

/* 5820 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5822 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5826 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5828 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5830 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5832 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5834 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5836 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5842 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5844 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5846 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5848 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5852 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_CurrentPosition */


	/* Procedure put_CurrentPosition */

/* 5856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5864 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5866 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5870 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5872 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5878 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 5880 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5882 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5884 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5886 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5888 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopPosition */


	/* Procedure get_Duration */

/* 5892 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5894 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5898 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5900 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5904 */	NdrFcShort( 0x2c ),	/* 44 */
/* 5906 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5908 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5910 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5914 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 5916 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5918 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5920 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5922 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5924 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5926 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 5928 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5930 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5934 */	NdrFcShort( 0xa ),	/* 10 */
/* 5936 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5940 */	NdrFcShort( 0x24 ),	/* 36 */
/* 5942 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5944 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5950 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 5952 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5954 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5956 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5958 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5960 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 5964 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5966 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5970 */	NdrFcShort( 0xb ),	/* 11 */
/* 5972 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5974 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5976 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5978 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 5980 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 5982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5986 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 5988 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5990 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5992 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 5994 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5996 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_StopTime */

/* 6000 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6002 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6006 */	NdrFcShort( 0xc ),	/* 12 */
/* 6008 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6012 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6014 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6016 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6020 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6022 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6024 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6026 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6028 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6030 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6032 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6034 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopTime */

/* 6036 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6038 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6042 */	NdrFcShort( 0xd ),	/* 13 */
/* 6044 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6046 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6048 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6050 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6052 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6058 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6060 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6062 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6064 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6066 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6068 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6070 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_State */


	/* Procedure get_State */

/* 6072 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6074 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6078 */	NdrFcShort( 0xe ),	/* 14 */
/* 6080 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6084 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6086 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6088 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6094 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */


	/* Parameter pVal */

/* 6096 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6098 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6100 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_State */


	/* Procedure put_State */

/* 6108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6114 */	NdrFcShort( 0xf ),	/* 15 */
/* 6116 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6122 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6124 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter newVal */

/* 6132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6136 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 6138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 6144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6150 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6152 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6156 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6158 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6160 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pStart */

/* 6168 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 6174 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPositions */

/* 6186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6192 */	NdrFcShort( 0x11 ),	/* 17 */
/* 6194 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6196 */	NdrFcShort( 0x1c ),	/* 28 */
/* 6198 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6200 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6202 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pPos */

/* 6210 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 6212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStop */

/* 6216 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6222 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6224 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPositions */

/* 6228 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6230 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6234 */	NdrFcShort( 0x12 ),	/* 18 */
/* 6236 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6238 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6240 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6242 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6244 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6250 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter llStart */

/* 6252 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6254 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6256 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter llStop */

/* 6258 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6260 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6262 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6266 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CheckCapabilities */

/* 6270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6276 */	NdrFcShort( 0x14 ),	/* 20 */
/* 6278 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6280 */	NdrFcShort( 0x1c ),	/* 28 */
/* 6282 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6284 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6286 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6292 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pCaps */

/* 6294 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 6296 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6298 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6302 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_StopPosition */

/* 6306 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6312 */	NdrFcShort( 0xa ),	/* 10 */
/* 6314 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6316 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6320 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6322 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6330 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6334 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6338 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Duration */

/* 6342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6348 */	NdrFcShort( 0xb ),	/* 11 */
/* 6350 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6354 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6356 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6358 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6366 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6368 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6370 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6374 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Rate */

/* 6378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6384 */	NdrFcShort( 0xc ),	/* 12 */
/* 6386 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6390 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6392 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6394 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6400 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6402 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6406 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Rate */

/* 6414 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6420 */	NdrFcShort( 0xd ),	/* 13 */
/* 6422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6424 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6426 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6428 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6430 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6436 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6438 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6440 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6442 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 6444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6446 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_TimeFormat */

/* 6450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6456 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6462 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6464 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6466 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 6474 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6478 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TimeFormat */

/* 6486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6492 */	NdrFcShort( 0x11 ),	/* 17 */
/* 6494 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6498 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6500 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6502 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 6510 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6512 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6514 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6518 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ConvertTimeFormat */

/* 6522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6528 */	NdrFcShort( 0x12 ),	/* 18 */
/* 6530 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6532 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6534 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6536 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 6538 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6544 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter srcVal */

/* 6546 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6548 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6550 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter srcFormat */

/* 6552 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6554 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6556 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstFormat */

/* 6558 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6560 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6562 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter dstVal */

/* 6564 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6566 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6568 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6570 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6572 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAvailable */

/* 6576 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6582 */	NdrFcShort( 0x13 ),	/* 19 */
/* 6584 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6588 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6590 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6592 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6598 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lEarliest */

/* 6600 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6602 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6604 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter lLatest */

/* 6606 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6608 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6610 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6612 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6614 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetShiftPosition */

/* 6618 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6620 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6624 */	NdrFcShort( 0x14 ),	/* 20 */
/* 6626 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 6628 */	NdrFcShort( 0x28 ),	/* 40 */
/* 6630 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6632 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 6634 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6636 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6640 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6642 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6644 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6646 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter shift */

/* 6648 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6650 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6652 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter timeFormat */

/* 6654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6656 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6658 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter pos */

/* 6660 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6662 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6664 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6666 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6668 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetHoldRange */

/* 6672 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6674 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6678 */	NdrFcShort( 0x15 ),	/* 21 */
/* 6680 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6682 */	NdrFcShort( 0x20 ),	/* 32 */
/* 6684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6686 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6688 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6694 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6696 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6698 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6700 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 6702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6704 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6706 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6708 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6710 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHoldRange */

/* 6714 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6720 */	NdrFcShort( 0x16 ),	/* 22 */
/* 6722 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6726 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6728 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6730 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6736 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6738 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6740 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6742 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 6744 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6746 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6748 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6750 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6752 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTimeFormatSupported */

/* 6756 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6758 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6762 */	NdrFcShort( 0x17 ),	/* 23 */
/* 6764 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6766 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6768 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6770 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 6772 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6778 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter format */

/* 6780 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6782 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6784 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter support */

/* 6786 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6788 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6792 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6794 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetContinuousInterval */

/* 6798 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6800 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6804 */	NdrFcShort( 0x18 ),	/* 24 */
/* 6806 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6808 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6810 */	NdrFcShort( 0x50 ),	/* 80 */
/* 6812 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6814 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6820 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter position */

/* 6822 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6824 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6826 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter start */

/* 6828 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6830 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6832 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter end */

/* 6834 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6836 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6838 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6840 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6842 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSelection */

/* 6846 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6848 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6852 */	NdrFcShort( 0x19 ),	/* 25 */
/* 6854 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6856 */	NdrFcShort( 0x28 ),	/* 40 */
/* 6858 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6860 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6862 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6868 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter from */

/* 6870 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6872 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6874 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter to */

/* 6876 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6878 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6880 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter mode */

/* 6882 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6884 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6886 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Return value */

/* 6888 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6890 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBufferSize */

/* 6894 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6900 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6902 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6906 */	NdrFcShort( 0x5c ),	/* 92 */
/* 6908 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 6910 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6916 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter total */

/* 6918 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter used */

/* 6924 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6926 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6928 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter writePos */

/* 6930 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6936 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6938 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */

/* 6942 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6944 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6948 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6950 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6956 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 6958 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6964 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 6966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6968 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Timeout */


	/* Procedure SetBufferSize */

/* 6972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6978 */	NdrFcShort( 0x9 ),	/* 9 */
/* 6980 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6982 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6986 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 6988 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 6990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6994 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */


	/* Parameter cbSize */

/* 6996 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 7002 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7004 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveData */


	/* Procedure SaveData */

/* 7008 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7010 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7014 */	NdrFcShort( 0xa ),	/* 10 */
/* 7016 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7022 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 7024 */	0x8,		/* 8 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 7026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7028 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7030 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter strFileName */


	/* Parameter strFileName */

/* 7032 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7034 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7036 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Return value */


	/* Return value */

/* 7038 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7040 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBufferSize */

/* 7044 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7046 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7050 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7052 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7056 */	NdrFcShort( 0x74 ),	/* 116 */
/* 7058 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7060 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7064 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7066 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter total */

/* 7068 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7070 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7072 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter used */

/* 7074 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7076 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7078 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter writePos */

/* 7080 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7082 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7084 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7088 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetBufferSize */

/* 7092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7098 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7100 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7102 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7106 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7108 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7114 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cbSize */

/* 7116 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7118 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7120 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 7122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7124 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVolume */

/* 7128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7134 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7136 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7140 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7142 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7144 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */

/* 7152 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7154 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 7158 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7160 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7166 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_DepthsRange */

/* 7170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7176 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7178 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7182 */	NdrFcShort( 0x38 ),	/* 56 */
/* 7184 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7186 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7192 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 7194 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 7196 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7198 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

	/* Return value */

/* 7200 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7202 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_VolumesRange */

/* 7206 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7212 */	NdrFcShort( 0xa ),	/* 10 */
/* 7214 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7218 */	NdrFcShort( 0x38 ),	/* 56 */
/* 7220 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7222 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7228 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 7230 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 7232 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7234 */	NdrFcShort( 0x68e ),	/* Type Offset=1678 */

	/* Return value */

/* 7236 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7238 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSampleVolume */

/* 7242 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7248 */	NdrFcShort( 0xb ),	/* 11 */
/* 7250 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7252 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7256 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7258 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startDepth */

/* 7266 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter endDepth */

/* 7272 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7274 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter line */

/* 7278 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7280 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7284 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7286 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsSupported */

/* 7290 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7292 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7296 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7298 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7302 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7304 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7306 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7310 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7312 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 7314 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7316 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7318 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter supported */

/* 7320 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7322 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7324 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7328 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIndex */

/* 7332 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7340 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7344 */	NdrFcShort( 0x34 ),	/* 52 */
/* 7346 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 7348 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7354 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 7356 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7358 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7360 */	0xe,		/* FC_ENUM32 */
			0x0,		/* 0 */

	/* Parameter value */

/* 7362 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 7364 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7366 */	NdrFcShort( 0x5dc ),	/* Type Offset=1500 */

	/* Return value */

/* 7368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7370 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Current */

/* 7374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7380 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7382 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7386 */	NdrFcShort( 0x34 ),	/* 52 */
/* 7388 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7390 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 7398 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 7400 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7402 */	NdrFcShort( 0x6f4 ),	/* Type Offset=1780 */

	/* Return value */

/* 7404 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7406 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Current */

/* 7410 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7412 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7416 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7418 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7420 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7422 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7424 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7426 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7432 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 7434 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7436 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7438 */	NdrFcShort( 0x6f4 ),	/* Type Offset=1780 */

	/* Return value */

/* 7440 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7442 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_FileName */

/* 7446 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7454 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7458 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7460 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 7462 */	0x8,		/* 8 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 7464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7466 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7468 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 7470 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7472 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7474 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Return value */

/* 7476 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7478 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7480 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 7482 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7484 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7488 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7490 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7494 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7496 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 7498 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7504 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 7506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7508 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateDataKey */

/* 7512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7518 */	NdrFcShort( 0xb ),	/* 11 */
/* 7520 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7522 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7524 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7526 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7528 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7534 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBaseKey */

/* 7536 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7538 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 7542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7544 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 7548 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7550 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7554 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7556 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDataKey */

/* 7560 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7562 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7566 */	NdrFcShort( 0xc ),	/* 12 */
/* 7568 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7570 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7572 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7574 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 7576 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7580 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7582 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter baseKey */

/* 7584 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7586 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyId */

/* 7590 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7592 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter keyIndex */

/* 7596 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7598 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataKey */

/* 7602 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7604 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7608 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7610 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CloseDataKey */

/* 7614 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7616 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7620 */	NdrFcShort( 0xd ),	/* 13 */
/* 7622 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7624 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7626 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7628 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 7630 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7632 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7634 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7636 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7638 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7640 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7644 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7646 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKeyInfo */

/* 7650 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7652 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7656 */	NdrFcShort( 0xe ),	/* 14 */
/* 7658 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7662 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7664 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7666 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7672 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7674 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7676 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subKeys */

/* 7680 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7682 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataValues */

/* 7686 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7688 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7692 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7694 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumValues */

/* 7698 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7700 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7704 */	NdrFcShort( 0xf ),	/* 15 */
/* 7706 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7708 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7710 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7712 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 7714 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7720 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7722 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7724 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ValueNum */

/* 7728 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7730 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 7734 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7736 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueSize */

/* 7740 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7742 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7746 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7748 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumSubkeys */

/* 7752 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7754 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7758 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7760 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7762 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7764 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7766 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 7768 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7772 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7774 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7776 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7778 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyNum */

/* 7782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7784 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter subkeyId */

/* 7788 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7790 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7796 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValue */

/* 7800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7806 */	NdrFcShort( 0x11 ),	/* 17 */
/* 7808 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 7810 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7812 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7814 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x7,		/* 7 */
/* 7816 */	0x8,		/* 8 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 7818 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7822 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7824 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7826 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 7830 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7832 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 7836 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7838 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7840 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cbBufferSize */

/* 7842 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7844 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7846 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 7848 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 7850 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7852 */	NdrFcShort( 0x700 ),	/* Type Offset=1792 */

	/* Parameter dataSize */

/* 7854 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7856 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7860 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7862 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 7864 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValue */

/* 7866 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7868 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7872 */	NdrFcShort( 0x12 ),	/* 18 */
/* 7874 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 7876 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7880 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 7882 */	0x8,		/* 8 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 7884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7886 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7888 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dataKey */

/* 7890 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7892 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueId */

/* 7896 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7898 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter valueIndex */

/* 7902 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7904 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7906 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dataBuffer */

/* 7908 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 7910 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7912 */	NdrFcShort( 0x71a ),	/* Type Offset=1818 */

	/* Parameter dataSize */

/* 7914 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7916 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7918 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7920 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7922 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateObjectsDump */

/* 7926 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7928 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7932 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7934 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7936 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7938 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7940 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 7942 */	0x8,		/* 8 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 7944 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7946 */	NdrFcShort( 0x1 ),	/* 1 */
/* 7948 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fileName */

/* 7950 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7952 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7954 */	NdrFcShort( 0x9c ),	/* Type Offset=156 */

	/* Parameter reserved1 */

/* 7956 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7958 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter reserved2 */

/* 7962 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7964 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7966 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7968 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7970 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPlugin */

/* 7974 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7976 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7980 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7982 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7986 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7988 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 7990 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 7992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7996 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pPlugin */

/* 7998 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8000 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8002 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter cookie */

/* 8004 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8006 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8010 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8012 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8014 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ScanModeCaps */

/* 8016 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8018 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8022 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8024 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 8026 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8028 */	NdrFcShort( 0x40 ),	/* 64 */
/* 8030 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 8032 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8038 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Parameter */

/* 8040 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8042 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8044 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ScanMode */

/* 8046 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8048 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Capability */

/* 8052 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8054 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Support */

/* 8058 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8060 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8064 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8066 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8068 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurrent */

/* 8070 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8072 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8076 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8078 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8082 */	NdrFcShort( 0x48 ),	/* 72 */
/* 8084 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8086 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8088 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8092 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 8094 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8096 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8098 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter pVal */

/* 8100 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 8102 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8104 */	NdrFcShort( 0x734 ),	/* Type Offset=1844 */

	/* Return value */

/* 8106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8108 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCurrent */

/* 8112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8120 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8122 */	NdrFcShort( 0x40 ),	/* 64 */
/* 8124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8126 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8128 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8134 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 8136 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8138 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8140 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter pVal */

/* 8142 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 8144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8146 */	NdrFcShort( 0x734 ),	/* Type Offset=1844 */

	/* Return value */

/* 8148 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8150 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8152 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetValues */

/* 8154 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8156 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8160 */	NdrFcShort( 0x9 ),	/* 9 */
/* 8162 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8166 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8168 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 8170 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pControlObject */

/* 8178 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8180 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8182 */	NdrFcShort( 0xd6 ),	/* Type Offset=214 */

	/* Parameter pVal */

/* 8184 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8186 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8188 */	NdrFcShort( 0x208 ),	/* Type Offset=520 */

	/* Return value */

/* 8190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8192 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_FilterObj */

/* 8196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8202 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8204 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8208 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8210 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 8212 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8218 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter FilterIndex */

/* 8220 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8222 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 8226 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8228 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8230 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */

/* 8232 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8234 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetElementsTestResult */

/* 8238 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8240 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8244 */	NdrFcShort( 0x9 ),	/* 9 */
/* 8246 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8248 */	NdrFcShort( 0x10 ),	/* 16 */
/* 8250 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8252 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 8254 */	0x8,		/* 8 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 8256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8260 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startIndex */

/* 8262 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8264 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8266 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter entries */

/* 8268 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8270 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pStates */

/* 8274 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8276 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8278 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8280 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8282 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8284 */	0x8,		/* FC_LONG */
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
/* 160 */	NdrFcShort( 0x4 ),	/* 4 */
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
/* 234 */	NdrFcShort( 0x90 ),	/* Offset= 144 (378) */
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
/* 264 */	NdrFcShort( 0x4 ),	/* 4 */
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
/* 344 */	NdrFcShort( 0x4 ),	/* 4 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0xffb2 ),	/* Offset= -78 (270) */
/* 350 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 352 */	NdrFcShort( 0x18 ),	/* 24 */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* Offset= 0 (356) */
/* 358 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 360 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 362 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 364 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (340) */
			0x5b,		/* FC_END */
/* 368 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 370 */	NdrFcShort( 0x10 ),	/* 16 */
/* 372 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 374 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 376 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 378 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 380 */	NdrFcShort( 0x4c ),	/* 76 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x0 ),	/* Offset= 0 (384) */
/* 386 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 388 */	0x0,		/* 0 */
			NdrFcShort( 0xff7f ),	/* Offset= -129 (260) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 392 */	0x0,		/* 0 */
			NdrFcShort( 0xffcb ),	/* Offset= -53 (340) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 396 */	0x0,		/* 0 */
			NdrFcShort( 0xffd1 ),	/* Offset= -47 (350) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 400 */	0x0,		/* 0 */
			NdrFcShort( 0xffdf ),	/* Offset= -33 (368) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 404 */	0x0,		/* 0 */
			NdrFcShort( 0xffdb ),	/* Offset= -37 (368) */
			0xa,		/* FC_FLOAT */
/* 408 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 410 */	
			0x11, 0x0,	/* FC_RP */
/* 412 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (368) */
/* 414 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 416 */	NdrFcShort( 0x2 ),	/* Offset= 2 (418) */
/* 418 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 420 */	NdrFcLong( 0x7bf80981 ),	/* 2079852929 */
/* 424 */	NdrFcShort( 0xbf32 ),	/* -16590 */
/* 426 */	NdrFcShort( 0x101a ),	/* 4122 */
/* 428 */	0x8b,		/* 139 */
			0xbb,		/* 187 */
/* 430 */	0x0,		/* 0 */
			0xaa,		/* 170 */
/* 432 */	0x0,		/* 0 */
			0x30,		/* 48 */
/* 434 */	0xc,		/* 12 */
			0xab,		/* 171 */
/* 436 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 438 */	NdrFcShort( 0x2 ),	/* Offset= 2 (440) */
/* 440 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 442 */	NdrFcShort( 0x4 ),	/* 4 */
/* 444 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 446 */	0x1,		/* FC_BYTE */
			0x1,		/* FC_BYTE */
/* 448 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 450 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 452 */	NdrFcShort( 0xffac ),	/* Offset= -84 (368) */
/* 454 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 456 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 458 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 460 */	NdrFcShort( 0x2 ),	/* Offset= 2 (462) */
/* 462 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 464 */	NdrFcLong( 0xbfef8670 ),	/* -1074821520 */
/* 468 */	NdrFcShort( 0xdbc1 ),	/* -9279 */
/* 470 */	NdrFcShort( 0x4b47 ),	/* 19271 */
/* 472 */	0xbc,		/* 188 */
			0x1,		/* 1 */
/* 474 */	0xbb,		/* 187 */
			0xbc,		/* 188 */
/* 476 */	0xa1,		/* 161 */
			0x74,		/* 116 */
/* 478 */	0xdd,		/* 221 */
			0x31,		/* 49 */
/* 480 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 482 */	NdrFcShort( 0x6 ),	/* Offset= 6 (488) */
/* 484 */	
			0x13, 0x0,	/* FC_OP */
/* 486 */	NdrFcShort( 0xfeac ),	/* Offset= -340 (146) */
/* 488 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x4 ),	/* 4 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (484) */
/* 498 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 500 */	NdrFcShort( 0x2 ),	/* Offset= 2 (502) */
/* 502 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 504 */	NdrFcLong( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 514 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 516 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 518 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 520 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 522 */	NdrFcShort( 0x2 ),	/* Offset= 2 (524) */
/* 524 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 526 */	NdrFcLong( 0x95d11d2b ),	/* -1781457621 */
/* 530 */	NdrFcShort( 0xec05 ),	/* -5115 */
/* 532 */	NdrFcShort( 0x4a2e ),	/* 18990 */
/* 534 */	0xb3,		/* 179 */
			0x1b,		/* 27 */
/* 536 */	0x13,		/* 19 */
			0x86,		/* 134 */
/* 538 */	0xc4,		/* 196 */
			0x84,		/* 132 */
/* 540 */	0xae,		/* 174 */
			0x16,		/* 22 */
/* 542 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 544 */	NdrFcShort( 0x460 ),	/* Offset= 1120 (1664) */
/* 546 */	
			0x13, 0x0,	/* FC_OP */
/* 548 */	NdrFcShort( 0x448 ),	/* Offset= 1096 (1644) */
/* 550 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 552 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 554 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 556 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 558 */	0x0 , 
			0x0,		/* 0 */
/* 560 */	NdrFcLong( 0x0 ),	/* 0 */
/* 564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x2 ),	/* Offset= 2 (570) */
/* 570 */	NdrFcShort( 0x10 ),	/* 16 */
/* 572 */	NdrFcShort( 0x2f ),	/* 47 */
/* 574 */	NdrFcLong( 0x14 ),	/* 20 */
/* 578 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 580 */	NdrFcLong( 0x3 ),	/* 3 */
/* 584 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 586 */	NdrFcLong( 0x11 ),	/* 17 */
/* 590 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 592 */	NdrFcLong( 0x2 ),	/* 2 */
/* 596 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 598 */	NdrFcLong( 0x4 ),	/* 4 */
/* 602 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 604 */	NdrFcLong( 0x5 ),	/* 5 */
/* 608 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 610 */	NdrFcLong( 0xb ),	/* 11 */
/* 614 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 616 */	NdrFcLong( 0xa ),	/* 10 */
/* 620 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 622 */	NdrFcLong( 0x6 ),	/* 6 */
/* 626 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (858) */
/* 628 */	NdrFcLong( 0x7 ),	/* 7 */
/* 632 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 634 */	NdrFcLong( 0x8 ),	/* 8 */
/* 638 */	NdrFcShort( 0xff66 ),	/* Offset= -154 (484) */
/* 640 */	NdrFcLong( 0xd ),	/* 13 */
/* 644 */	NdrFcShort( 0xff72 ),	/* Offset= -142 (502) */
/* 646 */	NdrFcLong( 0x9 ),	/* 9 */
/* 650 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (864) */
/* 652 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 656 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (882) */
/* 658 */	NdrFcLong( 0x24 ),	/* 36 */
/* 662 */	NdrFcShort( 0x37a ),	/* Offset= 890 (1552) */
/* 664 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 668 */	NdrFcShort( 0x374 ),	/* Offset= 884 (1552) */
/* 670 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 674 */	NdrFcShort( 0x372 ),	/* Offset= 882 (1556) */
/* 676 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 680 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1560) */
/* 682 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 686 */	NdrFcShort( 0x36e ),	/* Offset= 878 (1564) */
/* 688 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 692 */	NdrFcShort( 0x36c ),	/* Offset= 876 (1568) */
/* 694 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 698 */	NdrFcShort( 0x36a ),	/* Offset= 874 (1572) */
/* 700 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 704 */	NdrFcShort( 0x368 ),	/* Offset= 872 (1576) */
/* 706 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 710 */	NdrFcShort( 0x352 ),	/* Offset= 850 (1560) */
/* 712 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 716 */	NdrFcShort( 0x350 ),	/* Offset= 848 (1564) */
/* 718 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 722 */	NdrFcShort( 0x35a ),	/* Offset= 858 (1580) */
/* 724 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 728 */	NdrFcShort( 0x350 ),	/* Offset= 848 (1576) */
/* 730 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 734 */	NdrFcShort( 0x352 ),	/* Offset= 850 (1584) */
/* 736 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 740 */	NdrFcShort( 0x350 ),	/* Offset= 848 (1588) */
/* 742 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 746 */	NdrFcShort( 0x34e ),	/* Offset= 846 (1592) */
/* 748 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 752 */	NdrFcShort( 0x35e ),	/* Offset= 862 (1614) */
/* 754 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 758 */	NdrFcShort( 0x35c ),	/* Offset= 860 (1618) */
/* 760 */	NdrFcLong( 0x10 ),	/* 16 */
/* 764 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 766 */	NdrFcLong( 0x12 ),	/* 18 */
/* 770 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 772 */	NdrFcLong( 0x13 ),	/* 19 */
/* 776 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 778 */	NdrFcLong( 0x15 ),	/* 21 */
/* 782 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 784 */	NdrFcLong( 0x16 ),	/* 22 */
/* 788 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 790 */	NdrFcLong( 0x17 ),	/* 23 */
/* 794 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 796 */	NdrFcLong( 0xe ),	/* 14 */
/* 800 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1626) */
/* 802 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 806 */	NdrFcShort( 0x33e ),	/* Offset= 830 (1636) */
/* 808 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 812 */	NdrFcShort( 0x33c ),	/* Offset= 828 (1640) */
/* 814 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 818 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1560) */
/* 820 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 824 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1564) */
/* 826 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 830 */	NdrFcShort( 0x2e2 ),	/* Offset= 738 (1568) */
/* 832 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 836 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (1564) */
/* 838 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 842 */	NdrFcShort( 0x2d2 ),	/* Offset= 722 (1564) */
/* 844 */	NdrFcLong( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x0 ),	/* Offset= 0 (848) */
/* 850 */	NdrFcLong( 0x1 ),	/* 1 */
/* 854 */	NdrFcShort( 0x0 ),	/* Offset= 0 (854) */
/* 856 */	NdrFcShort( 0xffff ),	/* Offset= -1 (855) */
/* 858 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 860 */	NdrFcShort( 0x8 ),	/* 8 */
/* 862 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 864 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 866 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 876 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 878 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 880 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 882 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 884 */	NdrFcShort( 0x2 ),	/* Offset= 2 (886) */
/* 886 */	
			0x13, 0x0,	/* FC_OP */
/* 888 */	NdrFcShort( 0x286 ),	/* Offset= 646 (1534) */
/* 890 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 892 */	NdrFcShort( 0x18 ),	/* 24 */
/* 894 */	NdrFcShort( 0xa ),	/* 10 */
/* 896 */	NdrFcLong( 0x8 ),	/* 8 */
/* 900 */	NdrFcShort( 0x64 ),	/* Offset= 100 (1000) */
/* 902 */	NdrFcLong( 0xd ),	/* 13 */
/* 906 */	NdrFcShort( 0x9c ),	/* Offset= 156 (1062) */
/* 908 */	NdrFcLong( 0x9 ),	/* 9 */
/* 912 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (1120) */
/* 914 */	NdrFcLong( 0xc ),	/* 12 */
/* 918 */	NdrFcShort( 0x104 ),	/* Offset= 260 (1178) */
/* 920 */	NdrFcLong( 0x24 ),	/* 36 */
/* 924 */	NdrFcShort( 0x174 ),	/* Offset= 372 (1296) */
/* 926 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 930 */	NdrFcShort( 0x17e ),	/* Offset= 382 (1312) */
/* 932 */	NdrFcLong( 0x10 ),	/* 16 */
/* 936 */	NdrFcShort( 0x1a2 ),	/* Offset= 418 (1354) */
/* 938 */	NdrFcLong( 0x2 ),	/* 2 */
/* 942 */	NdrFcShort( 0x1c6 ),	/* Offset= 454 (1396) */
/* 944 */	NdrFcLong( 0x3 ),	/* 3 */
/* 948 */	NdrFcShort( 0x1ea ),	/* Offset= 490 (1438) */
/* 950 */	NdrFcLong( 0x14 ),	/* 20 */
/* 954 */	NdrFcShort( 0x20e ),	/* Offset= 526 (1480) */
/* 956 */	NdrFcShort( 0xffff ),	/* Offset= -1 (955) */
/* 958 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 960 */	NdrFcShort( 0x4 ),	/* 4 */
/* 962 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 966 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 968 */	0x0 , 
			0x0,		/* 0 */
/* 970 */	NdrFcLong( 0x0 ),	/* 0 */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 980 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 982 */	NdrFcShort( 0x4 ),	/* 4 */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	NdrFcShort( 0x1 ),	/* 1 */
/* 988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 992 */	0x13, 0x0,	/* FC_OP */
/* 994 */	NdrFcShort( 0xfcb0 ),	/* Offset= -848 (146) */
/* 996 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 998 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1000 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1002 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1004 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1006 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1008 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1010 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1012 */	0x11, 0x0,	/* FC_RP */
/* 1014 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (958) */
/* 1016 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1018 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
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
/* 1058 */	NdrFcShort( 0xfcb4 ),	/* Offset= -844 (214) */
/* 1060 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1062 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1064 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1068 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1074) */
/* 1070 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1072 */	0x5c,		/* FC_PAD */
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
/* 1116 */	NdrFcShort( 0xff04 ),	/* Offset= -252 (864) */
/* 1118 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1120 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1122 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1126 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1132) */
/* 1128 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1130 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1132 */	
			0x11, 0x0,	/* FC_RP */
/* 1134 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1078) */
/* 1136 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1138 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1140 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1146 */	0x0 , 
			0x0,		/* 0 */
/* 1148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1156 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1158 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1160 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1164 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1170 */	0x13, 0x0,	/* FC_OP */
/* 1172 */	NdrFcShort( 0x1d8 ),	/* Offset= 472 (1644) */
/* 1174 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1176 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1178 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1184 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1190) */
/* 1186 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1188 */	0x5c,		/* FC_PAD */
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
/* 1236 */	NdrFcShort( 0x10 ),	/* 16 */
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
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1256 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1258 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1262 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1264 */	0x0 , 
			0x0,		/* 0 */
/* 1266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1270 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1274 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1276 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1278 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1282 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1288 */	0x13, 0x0,	/* FC_OP */
/* 1290 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1234) */
/* 1292 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1294 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1296 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1308) */
/* 1304 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1306 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1308 */	
			0x11, 0x0,	/* FC_RP */
/* 1310 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1254) */
/* 1312 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1314 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1318 */	NdrFcShort( 0xa ),	/* Offset= 10 (1328) */
/* 1320 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1322 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1324 */	NdrFcShort( 0xfb2a ),	/* Offset= -1238 (86) */
/* 1326 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1328 */	
			0x11, 0x0,	/* FC_RP */
/* 1330 */	NdrFcShort( 0xfeca ),	/* Offset= -310 (1020) */
/* 1332 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1334 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1336 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1342 */	0x0 , 
			0x0,		/* 0 */
/* 1344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1348 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1352 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1354 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1356 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1358 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1360 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1362 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1364 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1366 */	0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1368 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1332) */
/* 1370 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1372 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1374 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1376 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1378 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1382 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1384 */	0x0 , 
			0x0,		/* 0 */
/* 1386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1390 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1394 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1396 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1400 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1402 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1404 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1406 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1408 */	0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1410 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1374) */
/* 1412 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1414 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1416 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1418 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1420 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1424 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1426 */	0x0 , 
			0x0,		/* 0 */
/* 1428 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1436 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1438 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1440 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1442 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1444 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1446 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1448 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1450 */	0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1452 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1416) */
/* 1454 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1456 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1458 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1462 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1466 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1468 */	0x0 , 
			0x0,		/* 0 */
/* 1470 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1478 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1480 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1482 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1484 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1486 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1488 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1490 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1492 */	0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1494 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1458) */
/* 1496 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1498 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1500 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1504 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1506 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1508 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1510 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1512 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1514 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1516 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1518 */	0x0 , 
			0x0,		/* 0 */
/* 1520 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1528 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1530 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1500) */
/* 1532 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1534 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1536 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1538 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1508) */
/* 1540 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1540) */
/* 1542 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1544 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1546 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1548 */	NdrFcShort( 0xfd6e ),	/* Offset= -658 (890) */
/* 1550 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1552 */	
			0x13, 0x0,	/* FC_OP */
/* 1554 */	NdrFcShort( 0xfec0 ),	/* Offset= -320 (1234) */
/* 1556 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1558 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1560 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1562 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1564 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1566 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1568 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1570 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1572 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1574 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1576 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1578 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1580 */	
			0x13, 0x0,	/* FC_OP */
/* 1582 */	NdrFcShort( 0xfd2c ),	/* Offset= -724 (858) */
/* 1584 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1586 */	NdrFcShort( 0xfbb2 ),	/* Offset= -1102 (484) */
/* 1588 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1590 */	NdrFcShort( 0xfaa0 ),	/* Offset= -1376 (214) */
/* 1592 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1594 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1596) */
/* 1596 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1598 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 1602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1606 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1608 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1610 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1612 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1614 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1616 */	NdrFcShort( 0xfd22 ),	/* Offset= -734 (882) */
/* 1618 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1620 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1622) */
/* 1622 */	
			0x13, 0x0,	/* FC_OP */
/* 1624 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1644) */
/* 1626 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1628 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1630 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1632 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1634 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1636 */	
			0x13, 0x0,	/* FC_OP */
/* 1638 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1626) */
/* 1640 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1642 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1644 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1646 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1650 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1650) */
/* 1652 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1654 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1656 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1658 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1660 */	NdrFcShort( 0xfbaa ),	/* Offset= -1110 (550) */
/* 1662 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1664 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1666 */	NdrFcShort( 0x3 ),	/* 3 */
/* 1668 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1672 */	NdrFcShort( 0xfb9a ),	/* Offset= -1126 (546) */
/* 1674 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1676 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1678) */
/* 1678 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1680 */	NdrFcShort( 0xc ),	/* 12 */
/* 1682 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1684 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1686 */	
			0x11, 0x0,	/* FC_RP */
/* 1688 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1690) */
/* 1690 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1692 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1694 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1696 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1698 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1700 */	0x0 , 
			0x0,		/* 0 */
/* 1702 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1710 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1712 */	
			0x11, 0x0,	/* FC_RP */
/* 1714 */	NdrFcShort( 0xfb06 ),	/* Offset= -1274 (440) */
/* 1716 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1718 */	NdrFcShort( 0xff26 ),	/* Offset= -218 (1500) */
/* 1720 */	
			0x11, 0x0,	/* FC_RP */
/* 1722 */	NdrFcShort( 0xff22 ),	/* Offset= -222 (1500) */
/* 1724 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1726 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1728 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1730 */	NdrFcLong( 0x5748ca80 ),	/* 1464388224 */
/* 1734 */	NdrFcShort( 0x1710 ),	/* 5904 */
/* 1736 */	NdrFcShort( 0x489f ),	/* 18591 */
/* 1738 */	0xbc,		/* 188 */
			0x13,		/* 19 */
/* 1740 */	0x28,		/* 40 */
			0xf2,		/* 242 */
/* 1742 */	0xc0,		/* 192 */
			0x12,		/* 18 */
/* 1744 */	0x2b,		/* 43 */
			0x49,		/* 73 */
/* 1746 */	
			0x11, 0x0,	/* FC_RP */
/* 1748 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1750) */
/* 1750 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1752 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1754 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1756 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1758 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1760 */	0x0 , 
			0x0,		/* 0 */
/* 1762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1766 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1770 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1772 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1774 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1776 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1778 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1780) */
/* 1780 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1782 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1784 */	0x8,		/* FC_LONG */
			0xe,		/* FC_ENUM32 */
/* 1786 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1788 */	
			0x11, 0x0,	/* FC_RP */
/* 1790 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1792) */
/* 1792 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1794 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1796 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1798 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1800 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1802 */	0x0 , 
			0x0,		/* 0 */
/* 1804 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1808 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1812 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1814 */	
			0x11, 0x0,	/* FC_RP */
/* 1816 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1818) */
/* 1818 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1820 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1822 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 1824 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1826 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 1828 */	0x0 , 
			0x0,		/* 0 */
/* 1830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1838 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1840 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1842 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1844) */
/* 1844 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1846 */	NdrFcShort( 0xc ),	/* 12 */
/* 1848 */	0xe,		/* FC_ENUM32 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1850 */	0x0,		/* 0 */
			NdrFcShort( 0xfea1 ),	/* Offset= -351 (1500) */
			0x5b,		/* FC_END */
/* 1854 */	
			0x11, 0x0,	/* FC_RP */
/* 1856 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1844) */

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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataView_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanMode_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgMixerControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgMixerControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgData_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgData_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgGraph_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgGraph_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDeviceChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IScanDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScanDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgfw2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbesCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbesCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCollection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbe_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbe_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IProbe2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProbe2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IBeamformer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBeamformer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IScanModes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScanModes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgGain_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgGain_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgValues_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgValues_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPower_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPower_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDynamicRange_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDynamicRange_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFrameAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgRejection2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgRejection2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDepth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDepth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageOrientation_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageOrientation_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageEnhancement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageEnhancement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewArea_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgViewArea_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgLineDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgLineDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFocus_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFocus_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTgc_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgTgc_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgClearView_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgClearView_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPaletteCalculator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPaletteCalculator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPalette_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPalette_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgImageProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgImageProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgControlChangeSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgControlChangeSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCtrlChangeCommon_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCtrlChangeCommon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    36,
    2268,
    4308,
    4356,
    4410,
    4482,
    4536,
    4596
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLine2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLine2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLine2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
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
static const unsigned short IUsgScanLineProperties_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4674,
    4728,
    4800,
    4848,
    4890
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineProperties_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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


/* Object interface: IUsgScanLineProperties2, ver. 0.0,
   GUID={0xA4F7329D,0x51A7,0x4a61,{0xA9,0xA8,0xCF,0xE9,0x0A,0x90,0xC9,0x04}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgScanLineProperties2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    4674,
    4728,
    4800,
    4848,
    4890,
    4938,
    4998,
    5076,
    5130
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanLineProperties2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineProperties2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineProperties2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanLineSelector_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanLineSelector_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5184,
    5220,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepMode_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepMode_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSweepMode_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5256,
    2868
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgQualProp_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgQualProp_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgQualProp_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5292
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgSweepSpeed_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSweepSpeed_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSweepSpeed_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5346,
    3582
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerColorMap_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorMap_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorMap_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorPriority_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorPriority_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSteerAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSteerAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorThreshold_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorThreshold_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerBaseLine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerBaseLine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerPRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5400,
    5454,
    5508,
    5550,
    5592,
    5634,
    5676,
    5712,
    5748,
    5784
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerWindow_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWindow_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerWindow_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerWallFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerWallFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalScale_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalScale_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerPacketSize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerPacketSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPulsesNumber_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPulsesNumber_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5820,
    5856,
    5892,
    5928,
    5964,
    6000,
    6036,
    6072,
    6108,
    6144,
    6186,
    6228,
    3318,
    6270
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineloop_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineloop_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineloop_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5820,
    5856,
    5892,
    6306,
    6342,
    6378,
    6414,
    6072,
    6108,
    6450,
    6486,
    6522,
    6576,
    6618,
    6672,
    6714,
    6756,
    6798,
    6846
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineStream_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineStream_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineStream_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    6894,
    6942,
    6972,
    7008
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineSink_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineSink_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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


/* Object interface: IUsgCineSink2, ver. 0.0,
   GUID={0x9EB34740,0x8AA5,0x4373,{0xA1,0xFF,0xAC,0x9A,0x86,0x4D,0x5A,0xBB}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgCineSink2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    7044,
    6942,
    7092,
    7008
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgCineSink2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCineSink2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCineSink2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
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
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSpatialFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSpatialFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    7128,
    7170,
    7206,
    7242,
    5634
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDopplerSampleVolume_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSampleVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSampleVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerCorrectionAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerCorrectionAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    1596
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    1596,
    5256,
    2868
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgScanConverterPlugin2_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanConverterPlugin2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanConverterPlugin2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSignalSmooth_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSignalSmooth_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgAudioVolume_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgAudioVolume_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerSpectralAvg_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerSpectralAvg_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    7290,
    7332
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgBioeffectsIndices_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgBioeffectsIndices_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgBioeffectsIndices_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    7374,
    7410,
    2268
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeFrequency3_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeFrequency3_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeFrequency3_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDopplerColorTransparency_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDopplerColorTransparency_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeSize_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsg3dVolumeSize_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsg3dVolumeDensity_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsg3dVolumeDensity_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    7446,
    7482,
    498,
    7512,
    7560,
    7614,
    7650,
    7698,
    7752,
    7800,
    7866
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFileStorage_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFileStorage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFileStorage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    7926
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgfw2Debug_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgfw2Debug_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgfw2Debug_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    7974,
    36
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgPlugin_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgPlugin_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgPlugin_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5184,
    5256,
    2868,
    720,
    756
    };

static const MIDL_STUBLESS_PROXY_INFO IBeamformerPowerState_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IBeamformerPowerState_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBeamformerPowerState_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgScanType_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgScanType_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgSteeringAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgSteeringAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgViewAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgViewAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundFrames_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCompoundFrames_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgCompoundAngle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgCompoundAngle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    8016
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgDeviceCapabilities_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDeviceCapabilities_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDeviceCapabilities_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    8070,
    8112,
    8154
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgUnits_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgUnits_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgUnits_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    8196
    };

static const MIDL_STUBLESS_PROXY_INFO ISampleGrabberFilter_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &ISampleGrabberFilter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISampleGrabberFilter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    5400,
    5454,
    5508,
    5550,
    5592,
    5634,
    5676,
    5712,
    5748,
    5784
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgWindowRF_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgWindowRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgWindowRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgStreamEnable_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgStreamEnable_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgDataSourceRF_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgDataSourceRF_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgMultiBeam_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgMultiBeam_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
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


/* Object interface: IUsgFrameROI, ver. 0.0,
   GUID={0x525055A7,0xB4AD,0x4A89,{0x85,0xE8,0x8F,0xCC,0xF5,0x1F,0x9D,0x38}} */

#pragma code_seg(".orpc")
static const unsigned short IUsgFrameROI_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    5400,
    5454,
    5508,
    5550,
    5592,
    5634,
    5676,
    5712,
    5748,
    5784
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgFrameROI_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameROI_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgFrameROI_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgFrameROI_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
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
static const unsigned short IUsgProbeElementsTest_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    5256,
    8238
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgProbeElementsTest_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeElementsTest_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgProbeElementsTest_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgProbeElementsTest_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
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
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
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
static const unsigned short IUsgTissueMotionDetector_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    5256,
    6972,
    720,
    2346
    };

static const MIDL_STUBLESS_PROXY_INFO IUsgTissueMotionDetector_ProxyInfo =
    {
    &Object_StubDesc,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTissueMotionDetector_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IUsgTissueMotionDetector_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    USgfw2__MIDL_ProcFormatString.Format,
    &IUsgTissueMotionDetector_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
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
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
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
    0x1, /* MIDL flag */
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
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

