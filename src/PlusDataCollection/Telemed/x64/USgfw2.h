

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __USgfw2_h__
#define __USgfw2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUsgDataView_FWD_DEFINED__
#define __IUsgDataView_FWD_DEFINED__
typedef interface IUsgDataView IUsgDataView;

#endif 	/* __IUsgDataView_FWD_DEFINED__ */


#ifndef __IUsgScanMode_FWD_DEFINED__
#define __IUsgScanMode_FWD_DEFINED__
typedef interface IUsgScanMode IUsgScanMode;

#endif 	/* __IUsgScanMode_FWD_DEFINED__ */


#ifndef __IUsgDataStream_FWD_DEFINED__
#define __IUsgDataStream_FWD_DEFINED__
typedef interface IUsgDataStream IUsgDataStream;

#endif 	/* __IUsgDataStream_FWD_DEFINED__ */


#ifndef __IUsgMixerControl_FWD_DEFINED__
#define __IUsgMixerControl_FWD_DEFINED__
typedef interface IUsgMixerControl IUsgMixerControl;

#endif 	/* __IUsgMixerControl_FWD_DEFINED__ */


#ifndef __IUsgData_FWD_DEFINED__
#define __IUsgData_FWD_DEFINED__
typedef interface IUsgData IUsgData;

#endif 	/* __IUsgData_FWD_DEFINED__ */


#ifndef __IUsgGraph_FWD_DEFINED__
#define __IUsgGraph_FWD_DEFINED__
typedef interface IUsgGraph IUsgGraph;

#endif 	/* __IUsgGraph_FWD_DEFINED__ */


#ifndef __IUsgDeviceChangeSink_FWD_DEFINED__
#define __IUsgDeviceChangeSink_FWD_DEFINED__
typedef interface IUsgDeviceChangeSink IUsgDeviceChangeSink;

#endif 	/* __IUsgDeviceChangeSink_FWD_DEFINED__ */


#ifndef __IScanDepth_FWD_DEFINED__
#define __IScanDepth_FWD_DEFINED__
typedef interface IScanDepth IScanDepth;

#endif 	/* __IScanDepth_FWD_DEFINED__ */


#ifndef __IUsgfw2_FWD_DEFINED__
#define __IUsgfw2_FWD_DEFINED__
typedef interface IUsgfw2 IUsgfw2;

#endif 	/* __IUsgfw2_FWD_DEFINED__ */


#ifndef __IProbesCollection_FWD_DEFINED__
#define __IProbesCollection_FWD_DEFINED__
typedef interface IProbesCollection IProbesCollection;

#endif 	/* __IProbesCollection_FWD_DEFINED__ */


#ifndef __IUsgCollection_FWD_DEFINED__
#define __IUsgCollection_FWD_DEFINED__
typedef interface IUsgCollection IUsgCollection;

#endif 	/* __IUsgCollection_FWD_DEFINED__ */


#ifndef __IProbe_FWD_DEFINED__
#define __IProbe_FWD_DEFINED__
typedef interface IProbe IProbe;

#endif 	/* __IProbe_FWD_DEFINED__ */


#ifndef __IProbe2_FWD_DEFINED__
#define __IProbe2_FWD_DEFINED__
typedef interface IProbe2 IProbe2;

#endif 	/* __IProbe2_FWD_DEFINED__ */


#ifndef __IBeamformer_FWD_DEFINED__
#define __IBeamformer_FWD_DEFINED__
typedef interface IBeamformer IBeamformer;

#endif 	/* __IBeamformer_FWD_DEFINED__ */


#ifndef __IScanModes_FWD_DEFINED__
#define __IScanModes_FWD_DEFINED__
typedef interface IScanModes IScanModes;

#endif 	/* __IScanModes_FWD_DEFINED__ */


#ifndef __IUsgControl_FWD_DEFINED__
#define __IUsgControl_FWD_DEFINED__
typedef interface IUsgControl IUsgControl;

#endif 	/* __IUsgControl_FWD_DEFINED__ */


#ifndef __IUsgGain_FWD_DEFINED__
#define __IUsgGain_FWD_DEFINED__
typedef interface IUsgGain IUsgGain;

#endif 	/* __IUsgGain_FWD_DEFINED__ */


#ifndef __IUsgValues_FWD_DEFINED__
#define __IUsgValues_FWD_DEFINED__
typedef interface IUsgValues IUsgValues;

#endif 	/* __IUsgValues_FWD_DEFINED__ */


#ifndef __IUsgPower_FWD_DEFINED__
#define __IUsgPower_FWD_DEFINED__
typedef interface IUsgPower IUsgPower;

#endif 	/* __IUsgPower_FWD_DEFINED__ */


#ifndef __IUsgDynamicRange_FWD_DEFINED__
#define __IUsgDynamicRange_FWD_DEFINED__
typedef interface IUsgDynamicRange IUsgDynamicRange;

#endif 	/* __IUsgDynamicRange_FWD_DEFINED__ */


#ifndef __IUsgFrameAvg_FWD_DEFINED__
#define __IUsgFrameAvg_FWD_DEFINED__
typedef interface IUsgFrameAvg IUsgFrameAvg;

#endif 	/* __IUsgFrameAvg_FWD_DEFINED__ */


#ifndef __IUsgRejection2_FWD_DEFINED__
#define __IUsgRejection2_FWD_DEFINED__
typedef interface IUsgRejection2 IUsgRejection2;

#endif 	/* __IUsgRejection2_FWD_DEFINED__ */


#ifndef __IUsgProbeFrequency2_FWD_DEFINED__
#define __IUsgProbeFrequency2_FWD_DEFINED__
typedef interface IUsgProbeFrequency2 IUsgProbeFrequency2;

#endif 	/* __IUsgProbeFrequency2_FWD_DEFINED__ */


#ifndef __IUsgDepth_FWD_DEFINED__
#define __IUsgDepth_FWD_DEFINED__
typedef interface IUsgDepth IUsgDepth;

#endif 	/* __IUsgDepth_FWD_DEFINED__ */


#ifndef __IUsgImageOrientation_FWD_DEFINED__
#define __IUsgImageOrientation_FWD_DEFINED__
typedef interface IUsgImageOrientation IUsgImageOrientation;

#endif 	/* __IUsgImageOrientation_FWD_DEFINED__ */


#ifndef __IUsgImageEnhancement_FWD_DEFINED__
#define __IUsgImageEnhancement_FWD_DEFINED__
typedef interface IUsgImageEnhancement IUsgImageEnhancement;

#endif 	/* __IUsgImageEnhancement_FWD_DEFINED__ */


#ifndef __IUsgViewArea_FWD_DEFINED__
#define __IUsgViewArea_FWD_DEFINED__
typedef interface IUsgViewArea IUsgViewArea;

#endif 	/* __IUsgViewArea_FWD_DEFINED__ */


#ifndef __IUsgLineDensity_FWD_DEFINED__
#define __IUsgLineDensity_FWD_DEFINED__
typedef interface IUsgLineDensity IUsgLineDensity;

#endif 	/* __IUsgLineDensity_FWD_DEFINED__ */


#ifndef __IUsgFocus_FWD_DEFINED__
#define __IUsgFocus_FWD_DEFINED__
typedef interface IUsgFocus IUsgFocus;

#endif 	/* __IUsgFocus_FWD_DEFINED__ */


#ifndef __IUsgTgc_FWD_DEFINED__
#define __IUsgTgc_FWD_DEFINED__
typedef interface IUsgTgc IUsgTgc;

#endif 	/* __IUsgTgc_FWD_DEFINED__ */


#ifndef __IUsgClearView_FWD_DEFINED__
#define __IUsgClearView_FWD_DEFINED__
typedef interface IUsgClearView IUsgClearView;

#endif 	/* __IUsgClearView_FWD_DEFINED__ */


#ifndef __IUsgPaletteCalculator_FWD_DEFINED__
#define __IUsgPaletteCalculator_FWD_DEFINED__
typedef interface IUsgPaletteCalculator IUsgPaletteCalculator;

#endif 	/* __IUsgPaletteCalculator_FWD_DEFINED__ */


#ifndef __IUsgPalette_FWD_DEFINED__
#define __IUsgPalette_FWD_DEFINED__
typedef interface IUsgPalette IUsgPalette;

#endif 	/* __IUsgPalette_FWD_DEFINED__ */


#ifndef __IUsgImageProperties_FWD_DEFINED__
#define __IUsgImageProperties_FWD_DEFINED__
typedef interface IUsgImageProperties IUsgImageProperties;

#endif 	/* __IUsgImageProperties_FWD_DEFINED__ */


#ifndef __IUsgControlChangeSink_FWD_DEFINED__
#define __IUsgControlChangeSink_FWD_DEFINED__
typedef interface IUsgControlChangeSink IUsgControlChangeSink;

#endif 	/* __IUsgControlChangeSink_FWD_DEFINED__ */


#ifndef __IUsgCtrlChangeCommon_FWD_DEFINED__
#define __IUsgCtrlChangeCommon_FWD_DEFINED__
typedef interface IUsgCtrlChangeCommon IUsgCtrlChangeCommon;

#endif 	/* __IUsgCtrlChangeCommon_FWD_DEFINED__ */


#ifndef __IUsgScanLine_FWD_DEFINED__
#define __IUsgScanLine_FWD_DEFINED__
typedef interface IUsgScanLine IUsgScanLine;

#endif 	/* __IUsgScanLine_FWD_DEFINED__ */


#ifndef __IUsgScanLine2_FWD_DEFINED__
#define __IUsgScanLine2_FWD_DEFINED__
typedef interface IUsgScanLine2 IUsgScanLine2;

#endif 	/* __IUsgScanLine2_FWD_DEFINED__ */


#ifndef __IUsgScanLineProperties_FWD_DEFINED__
#define __IUsgScanLineProperties_FWD_DEFINED__
typedef interface IUsgScanLineProperties IUsgScanLineProperties;

#endif 	/* __IUsgScanLineProperties_FWD_DEFINED__ */


#ifndef __IUsgScanLineProperties2_FWD_DEFINED__
#define __IUsgScanLineProperties2_FWD_DEFINED__
typedef interface IUsgScanLineProperties2 IUsgScanLineProperties2;

#endif 	/* __IUsgScanLineProperties2_FWD_DEFINED__ */


#ifndef __IUsgScanLineSelector_FWD_DEFINED__
#define __IUsgScanLineSelector_FWD_DEFINED__
typedef interface IUsgScanLineSelector IUsgScanLineSelector;

#endif 	/* __IUsgScanLineSelector_FWD_DEFINED__ */


#ifndef __IUsgSweepMode_FWD_DEFINED__
#define __IUsgSweepMode_FWD_DEFINED__
typedef interface IUsgSweepMode IUsgSweepMode;

#endif 	/* __IUsgSweepMode_FWD_DEFINED__ */


#ifndef __IUsgQualProp_FWD_DEFINED__
#define __IUsgQualProp_FWD_DEFINED__
typedef interface IUsgQualProp IUsgQualProp;

#endif 	/* __IUsgQualProp_FWD_DEFINED__ */


#ifndef __IUsgSweepSpeed_FWD_DEFINED__
#define __IUsgSweepSpeed_FWD_DEFINED__
typedef interface IUsgSweepSpeed IUsgSweepSpeed;

#endif 	/* __IUsgSweepSpeed_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorMap_FWD_DEFINED__
#define __IUsgDopplerColorMap_FWD_DEFINED__
typedef interface IUsgDopplerColorMap IUsgDopplerColorMap;

#endif 	/* __IUsgDopplerColorMap_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorPriority_FWD_DEFINED__
#define __IUsgDopplerColorPriority_FWD_DEFINED__
typedef interface IUsgDopplerColorPriority IUsgDopplerColorPriority;

#endif 	/* __IUsgDopplerColorPriority_FWD_DEFINED__ */


#ifndef __IUsgDopplerSteerAngle_FWD_DEFINED__
#define __IUsgDopplerSteerAngle_FWD_DEFINED__
typedef interface IUsgDopplerSteerAngle IUsgDopplerSteerAngle;

#endif 	/* __IUsgDopplerSteerAngle_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorThreshold_FWD_DEFINED__
#define __IUsgDopplerColorThreshold_FWD_DEFINED__
typedef interface IUsgDopplerColorThreshold IUsgDopplerColorThreshold;

#endif 	/* __IUsgDopplerColorThreshold_FWD_DEFINED__ */


#ifndef __IUsgDopplerBaseLine_FWD_DEFINED__
#define __IUsgDopplerBaseLine_FWD_DEFINED__
typedef interface IUsgDopplerBaseLine IUsgDopplerBaseLine;

#endif 	/* __IUsgDopplerBaseLine_FWD_DEFINED__ */


#ifndef __IUsgDopplerPRF_FWD_DEFINED__
#define __IUsgDopplerPRF_FWD_DEFINED__
typedef interface IUsgDopplerPRF IUsgDopplerPRF;

#endif 	/* __IUsgDopplerPRF_FWD_DEFINED__ */


#ifndef __IUsgDopplerWindow_FWD_DEFINED__
#define __IUsgDopplerWindow_FWD_DEFINED__
typedef interface IUsgDopplerWindow IUsgDopplerWindow;

#endif 	/* __IUsgDopplerWindow_FWD_DEFINED__ */


#ifndef __IUsgDopplerWallFilter_FWD_DEFINED__
#define __IUsgDopplerWallFilter_FWD_DEFINED__
typedef interface IUsgDopplerWallFilter IUsgDopplerWallFilter;

#endif 	/* __IUsgDopplerWallFilter_FWD_DEFINED__ */


#ifndef __IUsgDopplerSignalScale_FWD_DEFINED__
#define __IUsgDopplerSignalScale_FWD_DEFINED__
typedef interface IUsgDopplerSignalScale IUsgDopplerSignalScale;

#endif 	/* __IUsgDopplerSignalScale_FWD_DEFINED__ */


#ifndef __IUsgDopplerPacketSize_FWD_DEFINED__
#define __IUsgDopplerPacketSize_FWD_DEFINED__
typedef interface IUsgDopplerPacketSize IUsgDopplerPacketSize;

#endif 	/* __IUsgDopplerPacketSize_FWD_DEFINED__ */


#ifndef __IUsgPulsesNumber_FWD_DEFINED__
#define __IUsgPulsesNumber_FWD_DEFINED__
typedef interface IUsgPulsesNumber IUsgPulsesNumber;

#endif 	/* __IUsgPulsesNumber_FWD_DEFINED__ */


#ifndef __IUsgCineloop_FWD_DEFINED__
#define __IUsgCineloop_FWD_DEFINED__
typedef interface IUsgCineloop IUsgCineloop;

#endif 	/* __IUsgCineloop_FWD_DEFINED__ */


#ifndef __IUsgCineStream_FWD_DEFINED__
#define __IUsgCineStream_FWD_DEFINED__
typedef interface IUsgCineStream IUsgCineStream;

#endif 	/* __IUsgCineStream_FWD_DEFINED__ */


#ifndef __IUsgCineSink_FWD_DEFINED__
#define __IUsgCineSink_FWD_DEFINED__
typedef interface IUsgCineSink IUsgCineSink;

#endif 	/* __IUsgCineSink_FWD_DEFINED__ */


#ifndef __IUsgCineSink2_FWD_DEFINED__
#define __IUsgCineSink2_FWD_DEFINED__
typedef interface IUsgCineSink2 IUsgCineSink2;

#endif 	/* __IUsgCineSink2_FWD_DEFINED__ */


#ifndef __IUsgSpatialFilter_FWD_DEFINED__
#define __IUsgSpatialFilter_FWD_DEFINED__
typedef interface IUsgSpatialFilter IUsgSpatialFilter;

#endif 	/* __IUsgSpatialFilter_FWD_DEFINED__ */


#ifndef __IUsgDopplerSampleVolume_FWD_DEFINED__
#define __IUsgDopplerSampleVolume_FWD_DEFINED__
typedef interface IUsgDopplerSampleVolume IUsgDopplerSampleVolume;

#endif 	/* __IUsgDopplerSampleVolume_FWD_DEFINED__ */


#ifndef __IUsgDopplerCorrectionAngle_FWD_DEFINED__
#define __IUsgDopplerCorrectionAngle_FWD_DEFINED__
typedef interface IUsgDopplerCorrectionAngle IUsgDopplerCorrectionAngle;

#endif 	/* __IUsgDopplerCorrectionAngle_FWD_DEFINED__ */


#ifndef __IUsgScanConverterPlugin_FWD_DEFINED__
#define __IUsgScanConverterPlugin_FWD_DEFINED__
typedef interface IUsgScanConverterPlugin IUsgScanConverterPlugin;

#endif 	/* __IUsgScanConverterPlugin_FWD_DEFINED__ */


#ifndef __IUsgScanConverterPlugin2_FWD_DEFINED__
#define __IUsgScanConverterPlugin2_FWD_DEFINED__
typedef interface IUsgScanConverterPlugin2 IUsgScanConverterPlugin2;

#endif 	/* __IUsgScanConverterPlugin2_FWD_DEFINED__ */


#ifndef __IUsgDopplerSignalSmooth_FWD_DEFINED__
#define __IUsgDopplerSignalSmooth_FWD_DEFINED__
typedef interface IUsgDopplerSignalSmooth IUsgDopplerSignalSmooth;

#endif 	/* __IUsgDopplerSignalSmooth_FWD_DEFINED__ */


#ifndef __IUsgAudioVolume_FWD_DEFINED__
#define __IUsgAudioVolume_FWD_DEFINED__
typedef interface IUsgAudioVolume IUsgAudioVolume;

#endif 	/* __IUsgAudioVolume_FWD_DEFINED__ */


#ifndef __IUsgDopplerSpectralAvg_FWD_DEFINED__
#define __IUsgDopplerSpectralAvg_FWD_DEFINED__
typedef interface IUsgDopplerSpectralAvg IUsgDopplerSpectralAvg;

#endif 	/* __IUsgDopplerSpectralAvg_FWD_DEFINED__ */


#ifndef __IUsgBioeffectsIndices_FWD_DEFINED__
#define __IUsgBioeffectsIndices_FWD_DEFINED__
typedef interface IUsgBioeffectsIndices IUsgBioeffectsIndices;

#endif 	/* __IUsgBioeffectsIndices_FWD_DEFINED__ */


#ifndef __IUsgProbeFrequency3_FWD_DEFINED__
#define __IUsgProbeFrequency3_FWD_DEFINED__
typedef interface IUsgProbeFrequency3 IUsgProbeFrequency3;

#endif 	/* __IUsgProbeFrequency3_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorTransparency_FWD_DEFINED__
#define __IUsgDopplerColorTransparency_FWD_DEFINED__
typedef interface IUsgDopplerColorTransparency IUsgDopplerColorTransparency;

#endif 	/* __IUsgDopplerColorTransparency_FWD_DEFINED__ */


#ifndef __IUsg3dVolumeSize_FWD_DEFINED__
#define __IUsg3dVolumeSize_FWD_DEFINED__
typedef interface IUsg3dVolumeSize IUsg3dVolumeSize;

#endif 	/* __IUsg3dVolumeSize_FWD_DEFINED__ */


#ifndef __IUsg3dVolumeDensity_FWD_DEFINED__
#define __IUsg3dVolumeDensity_FWD_DEFINED__
typedef interface IUsg3dVolumeDensity IUsg3dVolumeDensity;

#endif 	/* __IUsg3dVolumeDensity_FWD_DEFINED__ */


#ifndef __IUsgFileStorage_FWD_DEFINED__
#define __IUsgFileStorage_FWD_DEFINED__
typedef interface IUsgFileStorage IUsgFileStorage;

#endif 	/* __IUsgFileStorage_FWD_DEFINED__ */


#ifndef __IUsgfw2Debug_FWD_DEFINED__
#define __IUsgfw2Debug_FWD_DEFINED__
typedef interface IUsgfw2Debug IUsgfw2Debug;

#endif 	/* __IUsgfw2Debug_FWD_DEFINED__ */


#ifndef __IUsgPlugin_FWD_DEFINED__
#define __IUsgPlugin_FWD_DEFINED__
typedef interface IUsgPlugin IUsgPlugin;

#endif 	/* __IUsgPlugin_FWD_DEFINED__ */


#ifndef __IBeamformerPowerState_FWD_DEFINED__
#define __IBeamformerPowerState_FWD_DEFINED__
typedef interface IBeamformerPowerState IBeamformerPowerState;

#endif 	/* __IBeamformerPowerState_FWD_DEFINED__ */


#ifndef __IUsgScanType_FWD_DEFINED__
#define __IUsgScanType_FWD_DEFINED__
typedef interface IUsgScanType IUsgScanType;

#endif 	/* __IUsgScanType_FWD_DEFINED__ */


#ifndef __IUsgSteeringAngle_FWD_DEFINED__
#define __IUsgSteeringAngle_FWD_DEFINED__
typedef interface IUsgSteeringAngle IUsgSteeringAngle;

#endif 	/* __IUsgSteeringAngle_FWD_DEFINED__ */


#ifndef __IUsgViewAngle_FWD_DEFINED__
#define __IUsgViewAngle_FWD_DEFINED__
typedef interface IUsgViewAngle IUsgViewAngle;

#endif 	/* __IUsgViewAngle_FWD_DEFINED__ */


#ifndef __IUsgCompoundFrames_FWD_DEFINED__
#define __IUsgCompoundFrames_FWD_DEFINED__
typedef interface IUsgCompoundFrames IUsgCompoundFrames;

#endif 	/* __IUsgCompoundFrames_FWD_DEFINED__ */


#ifndef __IUsgCompoundAngle_FWD_DEFINED__
#define __IUsgCompoundAngle_FWD_DEFINED__
typedef interface IUsgCompoundAngle IUsgCompoundAngle;

#endif 	/* __IUsgCompoundAngle_FWD_DEFINED__ */


#ifndef __IUsgDeviceCapabilities_FWD_DEFINED__
#define __IUsgDeviceCapabilities_FWD_DEFINED__
typedef interface IUsgDeviceCapabilities IUsgDeviceCapabilities;

#endif 	/* __IUsgDeviceCapabilities_FWD_DEFINED__ */


#ifndef __IUsgUnits_FWD_DEFINED__
#define __IUsgUnits_FWD_DEFINED__
typedef interface IUsgUnits IUsgUnits;

#endif 	/* __IUsgUnits_FWD_DEFINED__ */


#ifndef __ISampleGrabberFilter_FWD_DEFINED__
#define __ISampleGrabberFilter_FWD_DEFINED__
typedef interface ISampleGrabberFilter ISampleGrabberFilter;

#endif 	/* __ISampleGrabberFilter_FWD_DEFINED__ */


#ifndef __IUsgWindowRF_FWD_DEFINED__
#define __IUsgWindowRF_FWD_DEFINED__
typedef interface IUsgWindowRF IUsgWindowRF;

#endif 	/* __IUsgWindowRF_FWD_DEFINED__ */


#ifndef __IUsgStreamEnable_FWD_DEFINED__
#define __IUsgStreamEnable_FWD_DEFINED__
typedef interface IUsgStreamEnable IUsgStreamEnable;

#endif 	/* __IUsgStreamEnable_FWD_DEFINED__ */


#ifndef __IUsgDataSourceRF_FWD_DEFINED__
#define __IUsgDataSourceRF_FWD_DEFINED__
typedef interface IUsgDataSourceRF IUsgDataSourceRF;

#endif 	/* __IUsgDataSourceRF_FWD_DEFINED__ */


#ifndef __IUsgMultiBeam_FWD_DEFINED__
#define __IUsgMultiBeam_FWD_DEFINED__
typedef interface IUsgMultiBeam IUsgMultiBeam;

#endif 	/* __IUsgMultiBeam_FWD_DEFINED__ */


#ifndef __IUsgFrameROI_FWD_DEFINED__
#define __IUsgFrameROI_FWD_DEFINED__
typedef interface IUsgFrameROI IUsgFrameROI;

#endif 	/* __IUsgFrameROI_FWD_DEFINED__ */


#ifndef __IUsgProbeElementsTest_FWD_DEFINED__
#define __IUsgProbeElementsTest_FWD_DEFINED__
typedef interface IUsgProbeElementsTest IUsgProbeElementsTest;

#endif 	/* __IUsgProbeElementsTest_FWD_DEFINED__ */


#ifndef __IUsgTissueMotionDetector_FWD_DEFINED__
#define __IUsgTissueMotionDetector_FWD_DEFINED__
typedef interface IUsgTissueMotionDetector IUsgTissueMotionDetector;

#endif 	/* __IUsgTissueMotionDetector_FWD_DEFINED__ */


#ifndef __IUsgFocus_FWD_DEFINED__
#define __IUsgFocus_FWD_DEFINED__
typedef interface IUsgFocus IUsgFocus;

#endif 	/* __IUsgFocus_FWD_DEFINED__ */


#ifndef __IScanModes_FWD_DEFINED__
#define __IScanModes_FWD_DEFINED__
typedef interface IScanModes IScanModes;

#endif 	/* __IScanModes_FWD_DEFINED__ */


#ifndef __IUsgDataView_FWD_DEFINED__
#define __IUsgDataView_FWD_DEFINED__
typedef interface IUsgDataView IUsgDataView;

#endif 	/* __IUsgDataView_FWD_DEFINED__ */


#ifndef __IUsgScanMode_FWD_DEFINED__
#define __IUsgScanMode_FWD_DEFINED__
typedef interface IUsgScanMode IUsgScanMode;

#endif 	/* __IUsgScanMode_FWD_DEFINED__ */


#ifndef __IUsgDataStream_FWD_DEFINED__
#define __IUsgDataStream_FWD_DEFINED__
typedef interface IUsgDataStream IUsgDataStream;

#endif 	/* __IUsgDataStream_FWD_DEFINED__ */


#ifndef __IProbe_FWD_DEFINED__
#define __IProbe_FWD_DEFINED__
typedef interface IProbe IProbe;

#endif 	/* __IProbe_FWD_DEFINED__ */


#ifndef __IProbe2_FWD_DEFINED__
#define __IProbe2_FWD_DEFINED__
typedef interface IProbe2 IProbe2;

#endif 	/* __IProbe2_FWD_DEFINED__ */


#ifndef __IBeamformer_FWD_DEFINED__
#define __IBeamformer_FWD_DEFINED__
typedef interface IBeamformer IBeamformer;

#endif 	/* __IBeamformer_FWD_DEFINED__ */


#ifndef __IUsgCollection_FWD_DEFINED__
#define __IUsgCollection_FWD_DEFINED__
typedef interface IUsgCollection IUsgCollection;

#endif 	/* __IUsgCollection_FWD_DEFINED__ */


#ifndef __IUsgControl_FWD_DEFINED__
#define __IUsgControl_FWD_DEFINED__
typedef interface IUsgControl IUsgControl;

#endif 	/* __IUsgControl_FWD_DEFINED__ */


#ifndef __IUsgMixerControl_FWD_DEFINED__
#define __IUsgMixerControl_FWD_DEFINED__
typedef interface IUsgMixerControl IUsgMixerControl;

#endif 	/* __IUsgMixerControl_FWD_DEFINED__ */


#ifndef __IUsgGain_FWD_DEFINED__
#define __IUsgGain_FWD_DEFINED__
typedef interface IUsgGain IUsgGain;

#endif 	/* __IUsgGain_FWD_DEFINED__ */


#ifndef __IUsgValues_FWD_DEFINED__
#define __IUsgValues_FWD_DEFINED__
typedef interface IUsgValues IUsgValues;

#endif 	/* __IUsgValues_FWD_DEFINED__ */


#ifndef __IUsgPower_FWD_DEFINED__
#define __IUsgPower_FWD_DEFINED__
typedef interface IUsgPower IUsgPower;

#endif 	/* __IUsgPower_FWD_DEFINED__ */


#ifndef __IUsgDynamicRange_FWD_DEFINED__
#define __IUsgDynamicRange_FWD_DEFINED__
typedef interface IUsgDynamicRange IUsgDynamicRange;

#endif 	/* __IUsgDynamicRange_FWD_DEFINED__ */


#ifndef __IUsgFrameAvg_FWD_DEFINED__
#define __IUsgFrameAvg_FWD_DEFINED__
typedef interface IUsgFrameAvg IUsgFrameAvg;

#endif 	/* __IUsgFrameAvg_FWD_DEFINED__ */


#ifndef __IUsgRejection2_FWD_DEFINED__
#define __IUsgRejection2_FWD_DEFINED__
typedef interface IUsgRejection2 IUsgRejection2;

#endif 	/* __IUsgRejection2_FWD_DEFINED__ */


#ifndef __IUsgProbeFrequency2_FWD_DEFINED__
#define __IUsgProbeFrequency2_FWD_DEFINED__
typedef interface IUsgProbeFrequency2 IUsgProbeFrequency2;

#endif 	/* __IUsgProbeFrequency2_FWD_DEFINED__ */


#ifndef __IUsgProbeFrequency3_FWD_DEFINED__
#define __IUsgProbeFrequency3_FWD_DEFINED__
typedef interface IUsgProbeFrequency3 IUsgProbeFrequency3;

#endif 	/* __IUsgProbeFrequency3_FWD_DEFINED__ */


#ifndef __IUsgDepth_FWD_DEFINED__
#define __IUsgDepth_FWD_DEFINED__
typedef interface IUsgDepth IUsgDepth;

#endif 	/* __IUsgDepth_FWD_DEFINED__ */


#ifndef __IUsgImageOrientation_FWD_DEFINED__
#define __IUsgImageOrientation_FWD_DEFINED__
typedef interface IUsgImageOrientation IUsgImageOrientation;

#endif 	/* __IUsgImageOrientation_FWD_DEFINED__ */


#ifndef __IUsgImageEnhancement_FWD_DEFINED__
#define __IUsgImageEnhancement_FWD_DEFINED__
typedef interface IUsgImageEnhancement IUsgImageEnhancement;

#endif 	/* __IUsgImageEnhancement_FWD_DEFINED__ */


#ifndef __IUsgViewArea_FWD_DEFINED__
#define __IUsgViewArea_FWD_DEFINED__
typedef interface IUsgViewArea IUsgViewArea;

#endif 	/* __IUsgViewArea_FWD_DEFINED__ */


#ifndef __IUsgLineDensity_FWD_DEFINED__
#define __IUsgLineDensity_FWD_DEFINED__
typedef interface IUsgLineDensity IUsgLineDensity;

#endif 	/* __IUsgLineDensity_FWD_DEFINED__ */


#ifndef __IUsgTgc_FWD_DEFINED__
#define __IUsgTgc_FWD_DEFINED__
typedef interface IUsgTgc IUsgTgc;

#endif 	/* __IUsgTgc_FWD_DEFINED__ */


#ifndef __IUsgClearView_FWD_DEFINED__
#define __IUsgClearView_FWD_DEFINED__
typedef interface IUsgClearView IUsgClearView;

#endif 	/* __IUsgClearView_FWD_DEFINED__ */


#ifndef __IUsgPalette_FWD_DEFINED__
#define __IUsgPalette_FWD_DEFINED__
typedef interface IUsgPalette IUsgPalette;

#endif 	/* __IUsgPalette_FWD_DEFINED__ */


#ifndef __IUsgImageProperties_FWD_DEFINED__
#define __IUsgImageProperties_FWD_DEFINED__
typedef interface IUsgImageProperties IUsgImageProperties;

#endif 	/* __IUsgImageProperties_FWD_DEFINED__ */


#ifndef __IUsgScanLine_FWD_DEFINED__
#define __IUsgScanLine_FWD_DEFINED__
typedef interface IUsgScanLine IUsgScanLine;

#endif 	/* __IUsgScanLine_FWD_DEFINED__ */


#ifndef __IUsgScanLine2_FWD_DEFINED__
#define __IUsgScanLine2_FWD_DEFINED__
typedef interface IUsgScanLine2 IUsgScanLine2;

#endif 	/* __IUsgScanLine2_FWD_DEFINED__ */


#ifndef __IUsgScanLineProperties_FWD_DEFINED__
#define __IUsgScanLineProperties_FWD_DEFINED__
typedef interface IUsgScanLineProperties IUsgScanLineProperties;

#endif 	/* __IUsgScanLineProperties_FWD_DEFINED__ */


#ifndef __IUsgScanLineProperties2_FWD_DEFINED__
#define __IUsgScanLineProperties2_FWD_DEFINED__
typedef interface IUsgScanLineProperties2 IUsgScanLineProperties2;

#endif 	/* __IUsgScanLineProperties2_FWD_DEFINED__ */


#ifndef __IUsgScanLineSelector_FWD_DEFINED__
#define __IUsgScanLineSelector_FWD_DEFINED__
typedef interface IUsgScanLineSelector IUsgScanLineSelector;

#endif 	/* __IUsgScanLineSelector_FWD_DEFINED__ */


#ifndef __IUsgSweepMode_FWD_DEFINED__
#define __IUsgSweepMode_FWD_DEFINED__
typedef interface IUsgSweepMode IUsgSweepMode;

#endif 	/* __IUsgSweepMode_FWD_DEFINED__ */


#ifndef __IUsgQualProp_FWD_DEFINED__
#define __IUsgQualProp_FWD_DEFINED__
typedef interface IUsgQualProp IUsgQualProp;

#endif 	/* __IUsgQualProp_FWD_DEFINED__ */


#ifndef __IUsgSweepSpeed_FWD_DEFINED__
#define __IUsgSweepSpeed_FWD_DEFINED__
typedef interface IUsgSweepSpeed IUsgSweepSpeed;

#endif 	/* __IUsgSweepSpeed_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorMap_FWD_DEFINED__
#define __IUsgDopplerColorMap_FWD_DEFINED__
typedef interface IUsgDopplerColorMap IUsgDopplerColorMap;

#endif 	/* __IUsgDopplerColorMap_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorPriority_FWD_DEFINED__
#define __IUsgDopplerColorPriority_FWD_DEFINED__
typedef interface IUsgDopplerColorPriority IUsgDopplerColorPriority;

#endif 	/* __IUsgDopplerColorPriority_FWD_DEFINED__ */


#ifndef __IUsgDopplerSteerAngle_FWD_DEFINED__
#define __IUsgDopplerSteerAngle_FWD_DEFINED__
typedef interface IUsgDopplerSteerAngle IUsgDopplerSteerAngle;

#endif 	/* __IUsgDopplerSteerAngle_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorThreshold_FWD_DEFINED__
#define __IUsgDopplerColorThreshold_FWD_DEFINED__
typedef interface IUsgDopplerColorThreshold IUsgDopplerColorThreshold;

#endif 	/* __IUsgDopplerColorThreshold_FWD_DEFINED__ */


#ifndef __IUsgDopplerBaseLine_FWD_DEFINED__
#define __IUsgDopplerBaseLine_FWD_DEFINED__
typedef interface IUsgDopplerBaseLine IUsgDopplerBaseLine;

#endif 	/* __IUsgDopplerBaseLine_FWD_DEFINED__ */


#ifndef __IUsgDopplerPRF_FWD_DEFINED__
#define __IUsgDopplerPRF_FWD_DEFINED__
typedef interface IUsgDopplerPRF IUsgDopplerPRF;

#endif 	/* __IUsgDopplerPRF_FWD_DEFINED__ */


#ifndef __IUsgDopplerWindow_FWD_DEFINED__
#define __IUsgDopplerWindow_FWD_DEFINED__
typedef interface IUsgDopplerWindow IUsgDopplerWindow;

#endif 	/* __IUsgDopplerWindow_FWD_DEFINED__ */


#ifndef __IUsgDopplerWallFilter_FWD_DEFINED__
#define __IUsgDopplerWallFilter_FWD_DEFINED__
typedef interface IUsgDopplerWallFilter IUsgDopplerWallFilter;

#endif 	/* __IUsgDopplerWallFilter_FWD_DEFINED__ */


#ifndef __IUsgDopplerSignalScale_FWD_DEFINED__
#define __IUsgDopplerSignalScale_FWD_DEFINED__
typedef interface IUsgDopplerSignalScale IUsgDopplerSignalScale;

#endif 	/* __IUsgDopplerSignalScale_FWD_DEFINED__ */


#ifndef __IUsgDopplerSignalSmooth_FWD_DEFINED__
#define __IUsgDopplerSignalSmooth_FWD_DEFINED__
typedef interface IUsgDopplerSignalSmooth IUsgDopplerSignalSmooth;

#endif 	/* __IUsgDopplerSignalSmooth_FWD_DEFINED__ */


#ifndef __IUsgDopplerPacketSize_FWD_DEFINED__
#define __IUsgDopplerPacketSize_FWD_DEFINED__
typedef interface IUsgDopplerPacketSize IUsgDopplerPacketSize;

#endif 	/* __IUsgDopplerPacketSize_FWD_DEFINED__ */


#ifndef __IUsgDopplerSpectralAvg_FWD_DEFINED__
#define __IUsgDopplerSpectralAvg_FWD_DEFINED__
typedef interface IUsgDopplerSpectralAvg IUsgDopplerSpectralAvg;

#endif 	/* __IUsgDopplerSpectralAvg_FWD_DEFINED__ */


#ifndef __IUsgPulsesNumber_FWD_DEFINED__
#define __IUsgPulsesNumber_FWD_DEFINED__
typedef interface IUsgPulsesNumber IUsgPulsesNumber;

#endif 	/* __IUsgPulsesNumber_FWD_DEFINED__ */


#ifndef __IUsgCineStream_FWD_DEFINED__
#define __IUsgCineStream_FWD_DEFINED__
typedef interface IUsgCineStream IUsgCineStream;

#endif 	/* __IUsgCineStream_FWD_DEFINED__ */


#ifndef __IUsgCineSink_FWD_DEFINED__
#define __IUsgCineSink_FWD_DEFINED__
typedef interface IUsgCineSink IUsgCineSink;

#endif 	/* __IUsgCineSink_FWD_DEFINED__ */


#ifndef __IUsgCineSink2_FWD_DEFINED__
#define __IUsgCineSink2_FWD_DEFINED__
typedef interface IUsgCineSink2 IUsgCineSink2;

#endif 	/* __IUsgCineSink2_FWD_DEFINED__ */


#ifndef __IUsgSpatialFilter_FWD_DEFINED__
#define __IUsgSpatialFilter_FWD_DEFINED__
typedef interface IUsgSpatialFilter IUsgSpatialFilter;

#endif 	/* __IUsgSpatialFilter_FWD_DEFINED__ */


#ifndef __IUsgDopplerSampleVolume_FWD_DEFINED__
#define __IUsgDopplerSampleVolume_FWD_DEFINED__
typedef interface IUsgDopplerSampleVolume IUsgDopplerSampleVolume;

#endif 	/* __IUsgDopplerSampleVolume_FWD_DEFINED__ */


#ifndef __IUsgDopplerCorrectionAngle_FWD_DEFINED__
#define __IUsgDopplerCorrectionAngle_FWD_DEFINED__
typedef interface IUsgDopplerCorrectionAngle IUsgDopplerCorrectionAngle;

#endif 	/* __IUsgDopplerCorrectionAngle_FWD_DEFINED__ */


#ifndef __IUsgScanConverterPlugin_FWD_DEFINED__
#define __IUsgScanConverterPlugin_FWD_DEFINED__
typedef interface IUsgScanConverterPlugin IUsgScanConverterPlugin;

#endif 	/* __IUsgScanConverterPlugin_FWD_DEFINED__ */


#ifndef __IUsgScanConverterPlugin2_FWD_DEFINED__
#define __IUsgScanConverterPlugin2_FWD_DEFINED__
typedef interface IUsgScanConverterPlugin2 IUsgScanConverterPlugin2;

#endif 	/* __IUsgScanConverterPlugin2_FWD_DEFINED__ */


#ifndef __IUsgAudioVolume_FWD_DEFINED__
#define __IUsgAudioVolume_FWD_DEFINED__
typedef interface IUsgAudioVolume IUsgAudioVolume;

#endif 	/* __IUsgAudioVolume_FWD_DEFINED__ */


#ifndef __IUsgBioeffectsIndices_FWD_DEFINED__
#define __IUsgBioeffectsIndices_FWD_DEFINED__
typedef interface IUsgBioeffectsIndices IUsgBioeffectsIndices;

#endif 	/* __IUsgBioeffectsIndices_FWD_DEFINED__ */


#ifndef __IUsgDopplerColorTransparency_FWD_DEFINED__
#define __IUsgDopplerColorTransparency_FWD_DEFINED__
typedef interface IUsgDopplerColorTransparency IUsgDopplerColorTransparency;

#endif 	/* __IUsgDopplerColorTransparency_FWD_DEFINED__ */


#ifndef __IUsg3dVolumeSize_FWD_DEFINED__
#define __IUsg3dVolumeSize_FWD_DEFINED__
typedef interface IUsg3dVolumeSize IUsg3dVolumeSize;

#endif 	/* __IUsg3dVolumeSize_FWD_DEFINED__ */


#ifndef __IUsg3dVolumeDensity_FWD_DEFINED__
#define __IUsg3dVolumeDensity_FWD_DEFINED__
typedef interface IUsg3dVolumeDensity IUsg3dVolumeDensity;

#endif 	/* __IUsg3dVolumeDensity_FWD_DEFINED__ */


#ifndef __IUsgFileStorage_FWD_DEFINED__
#define __IUsgFileStorage_FWD_DEFINED__
typedef interface IUsgFileStorage IUsgFileStorage;

#endif 	/* __IUsgFileStorage_FWD_DEFINED__ */


#ifndef __IUsgPlugin_FWD_DEFINED__
#define __IUsgPlugin_FWD_DEFINED__
typedef interface IUsgPlugin IUsgPlugin;

#endif 	/* __IUsgPlugin_FWD_DEFINED__ */


#ifndef __IBeamformerPowerState_FWD_DEFINED__
#define __IBeamformerPowerState_FWD_DEFINED__
typedef interface IBeamformerPowerState IBeamformerPowerState;

#endif 	/* __IBeamformerPowerState_FWD_DEFINED__ */


#ifndef __IUsgScanType_FWD_DEFINED__
#define __IUsgScanType_FWD_DEFINED__
typedef interface IUsgScanType IUsgScanType;

#endif 	/* __IUsgScanType_FWD_DEFINED__ */


#ifndef __IUsgSteeringAngle_FWD_DEFINED__
#define __IUsgSteeringAngle_FWD_DEFINED__
typedef interface IUsgSteeringAngle IUsgSteeringAngle;

#endif 	/* __IUsgSteeringAngle_FWD_DEFINED__ */


#ifndef __IUsgViewAngle_FWD_DEFINED__
#define __IUsgViewAngle_FWD_DEFINED__
typedef interface IUsgViewAngle IUsgViewAngle;

#endif 	/* __IUsgViewAngle_FWD_DEFINED__ */


#ifndef __IUsgCompoundFrames_FWD_DEFINED__
#define __IUsgCompoundFrames_FWD_DEFINED__
typedef interface IUsgCompoundFrames IUsgCompoundFrames;

#endif 	/* __IUsgCompoundFrames_FWD_DEFINED__ */


#ifndef __IUsgCompoundAngle_FWD_DEFINED__
#define __IUsgCompoundAngle_FWD_DEFINED__
typedef interface IUsgCompoundAngle IUsgCompoundAngle;

#endif 	/* __IUsgCompoundAngle_FWD_DEFINED__ */


#ifndef __IUsgDeviceCapabilities_FWD_DEFINED__
#define __IUsgDeviceCapabilities_FWD_DEFINED__
typedef interface IUsgDeviceCapabilities IUsgDeviceCapabilities;

#endif 	/* __IUsgDeviceCapabilities_FWD_DEFINED__ */


#ifndef __IUsgUnits_FWD_DEFINED__
#define __IUsgUnits_FWD_DEFINED__
typedef interface IUsgUnits IUsgUnits;

#endif 	/* __IUsgUnits_FWD_DEFINED__ */


#ifndef __ISampleGrabberFilter_FWD_DEFINED__
#define __ISampleGrabberFilter_FWD_DEFINED__
typedef interface ISampleGrabberFilter ISampleGrabberFilter;

#endif 	/* __ISampleGrabberFilter_FWD_DEFINED__ */


#ifndef __IUsgDataSourceRF_FWD_DEFINED__
#define __IUsgDataSourceRF_FWD_DEFINED__
typedef interface IUsgDataSourceRF IUsgDataSourceRF;

#endif 	/* __IUsgDataSourceRF_FWD_DEFINED__ */


#ifndef __IUsgStreamEnable_FWD_DEFINED__
#define __IUsgStreamEnable_FWD_DEFINED__
typedef interface IUsgStreamEnable IUsgStreamEnable;

#endif 	/* __IUsgStreamEnable_FWD_DEFINED__ */


#ifndef __IUsgWindowRF_FWD_DEFINED__
#define __IUsgWindowRF_FWD_DEFINED__
typedef interface IUsgWindowRF IUsgWindowRF;

#endif 	/* __IUsgWindowRF_FWD_DEFINED__ */


#ifndef __IUsgMultiBeam_FWD_DEFINED__
#define __IUsgMultiBeam_FWD_DEFINED__
typedef interface IUsgMultiBeam IUsgMultiBeam;

#endif 	/* __IUsgMultiBeam_FWD_DEFINED__ */


#ifndef __IUsgFrameROI_FWD_DEFINED__
#define __IUsgFrameROI_FWD_DEFINED__
typedef interface IUsgFrameROI IUsgFrameROI;

#endif 	/* __IUsgFrameROI_FWD_DEFINED__ */


#ifndef __IUsgProbeElementsTest_FWD_DEFINED__
#define __IUsgProbeElementsTest_FWD_DEFINED__
typedef interface IUsgProbeElementsTest IUsgProbeElementsTest;

#endif 	/* __IUsgProbeElementsTest_FWD_DEFINED__ */


#ifndef __IUsgTissueMotionDetector_FWD_DEFINED__
#define __IUsgTissueMotionDetector_FWD_DEFINED__
typedef interface IUsgTissueMotionDetector IUsgTissueMotionDetector;

#endif 	/* __IUsgTissueMotionDetector_FWD_DEFINED__ */


#ifndef __Usgfw2_FWD_DEFINED__
#define __Usgfw2_FWD_DEFINED__

#ifdef __cplusplus
typedef class Usgfw2 Usgfw2;
#else
typedef struct Usgfw2 Usgfw2;
#endif /* __cplusplus */

#endif 	/* __Usgfw2_FWD_DEFINED__ */


#ifndef __UsgPaletteCalculator_FWD_DEFINED__
#define __UsgPaletteCalculator_FWD_DEFINED__

#ifdef __cplusplus
typedef class UsgPaletteCalculator UsgPaletteCalculator;
#else
typedef struct UsgPaletteCalculator UsgPaletteCalculator;
#endif /* __cplusplus */

#endif 	/* __UsgPaletteCalculator_FWD_DEFINED__ */


#ifndef __UsgControl_FWD_DEFINED__
#define __UsgControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class UsgControl UsgControl;
#else
typedef struct UsgControl UsgControl;
#endif /* __cplusplus */

#endif 	/* __UsgControl_FWD_DEFINED__ */


#ifndef __UsgDataView_FWD_DEFINED__
#define __UsgDataView_FWD_DEFINED__

#ifdef __cplusplus
typedef class UsgDataView UsgDataView;
#else
typedef struct UsgDataView UsgDataView;
#endif /* __cplusplus */

#endif 	/* __UsgDataView_FWD_DEFINED__ */


#ifndef __Usgfw2Debug_FWD_DEFINED__
#define __Usgfw2Debug_FWD_DEFINED__

#ifdef __cplusplus
typedef class Usgfw2Debug Usgfw2Debug;
#else
typedef struct Usgfw2Debug Usgfw2Debug;
#endif /* __cplusplus */

#endif 	/* __Usgfw2Debug_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"
#include "oaidl.h"
#include "ocidl.h"
#include "shtypes.h"
#include "strmif.h"
#include "control.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_USgfw2_0000_0000 */
/* [local] */ 

#ifndef VIDEO_SOURCE_TAG
#define VIDEO_SOURCE_TAG 0x12345
#endif














typedef /* [uuid] */  DECLSPEC_UUID("39D124EC-D60A-342A-8F92-6CBD6982BF63") struct tagMIXBITMAP
    {
    LONG bmWidth;
    LONG bmHeight;
    LONG bmWidthBytes;
    LONG bmPlanes;
    LONG bmBitsPixel;
    HBITMAP bmBits;
    } 	tagMIXBITMAP;

typedef tagMIXBITMAP MIXBITMAP;

typedef tagMIXBITMAP *PMIXBITMAP;

typedef /* [uuid][public] */  DECLSPEC_UUID("25440CAA-5582-3311-973B-FCE9158AA3C2") struct tagUsgMixingBitmap
    {
    ULONG dwFlags;
    HDC hdc;
    HBITMAP hBitmap;
    tagMIXBITMAP mixBitmap;
    RECT rcSrc;
    RECT rcDest;
    FLOAT fAlpha;
    COLORREF clrSrcKey;
    } 	tagUsgMixingBitmap;

typedef tagUsgMixingBitmap USG_MIXING_BITMAP;

typedef tagUsgMixingBitmap *PUSG_MIXING_BITMAP;

typedef /* [v1_enum][uuid] */  DECLSPEC_UUID("15769630-1FAD-3965-B717-5B190F98218D") 
enum tagThiMode
    {
        THI_MODE1	= 1,
        THI_MODE2	= 2,
        THI_MODE2_ITHI	= 0x102
    } 	tagThiMode;

typedef tagThiMode THI_MODE;

/* [uuid] */ struct  DECLSPEC_UUID("C07ED72B-4EA2-4cb1-9AF2-798B6192D2CC") FREQUENCY3
    {
    /* [helpstring] */ int nFrequency;
    /* [helpstring] */ tagThiMode nThiMode;
    } ;
typedef /* [uuid][v1_enum][public] */  DECLSPEC_UUID("56ECA81C-FCA2-37D3-ABEE-F9C2F30D9627") 
enum tagScanState
    {
        SCAN_STATE_STOP	= 0,
        SCAN_STATE_ACQUIRE	= 1,
        SCAN_STATE_FREEZE	= 2,
        SCAN_STATE_RUN	= 3
    } 	tagScanState;

typedef tagScanState SCAN_STATE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("AA6FACB8-3D48-3FA7-A672-FCAAC8DC9F86") 
enum tagCineState
    {
        CINE_STATE_STOP	= 0,
        CINE_STATE_PLAY	= 1
    } 	tagCineState;

typedef tagCineState CINE_STATE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("170DF8D4-2EF9-3FE6-9E0C-942BF4375161") 
enum tagCineTimeFormat
    {
        CINE_FORMAT_REFERENCE_TIME	= 0,
        CINE_FORMAT_FRAME	= 1,
        CINE_FORMAT_SAMPLE	= 2
    } 	tagCineTimeFormat;

typedef tagCineTimeFormat CINE_TIME_FORMAT;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("11A822F6-5A10-3968-9ACE-FDD22C8038D4") 
enum tagCineSelectionMode
    {
        SM_SELECT	= 0,
        SM_DESELECT	= 1
    } 	tagCineSelectionMode;

typedef tagCineSelectionMode SELECTION_MODE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("0FA281EC-361B-3C46-92BD-34A64771AE97") 
enum tagImageRotate
    {
        IMAGE_ROTATE_NONE	= 0,
        IMAGE_ROTATE_90CCW	= 1,
        IMAGE_ROTATE_180CCW	= 2,
        IMAGE_ROTATE_270CCW	= 3
    } 	tagImageRotate;

typedef tagImageRotate IMAGE_ROTATE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("C9474F1A-21D9-39CF-BDEC-155DF00FDEE6") 
enum tagPixelsOrigin
    {
        PIXELS_ORIGIN_WINDOW	= 0,
        PIXELS_ORIGIN_OUTPUTRECT	= 1
    } 	tagPixelsOrigin;

typedef tagPixelsOrigin PIXELS_ORIGIN;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("52C9AB6E-421A-3EE0-95FD-D3BC50EC0670") 
enum tagSweepMode
    {
        SWEEP_MODE_CYCLE	= 0,
        SWEEP_MODE_SCROLL	= 1
    } 	tagSweepMode;

typedef tagSweepMode SWEEP_MODE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("D6923CDD-205B-30A6-A520-02BA4E12608E") 
enum tagDeviceState
    {
        DEVICE_STATE_CLOSED	= 0,
        DEVICE_STATE_OPENED	= 1,
        DEVICE_STATE_INUSE	= 2,
        DEVICE_STATE_HARDWARE_ERROR	= 3,
        DEVICE_STATE_WRONG_POWER	= 4,
        DEVICE_STATE_REMOVED	= 5,
        DEVICE_STATE_UNINITIALIZED	= 6,
        DEVICE_STATE_REVISION_MISMATCH	= 7
    } 	tagDeviceState;

typedef tagDeviceState DEVICE_STATE;

typedef /* [v1_enum] */ 
enum tagBeamformerType
    {
        BEAMFORMER_EchoBlaster64	= 0,
        BEAMFORMER_EchoBlaster128	= ( BEAMFORMER_EchoBlaster64 + 1 ) ,
        BEAMFORMER_EchoBlaster128RevC	= ( BEAMFORMER_EchoBlaster128 + 1 ) ,
        BEAMFORMER_LogicScan128	= ( BEAMFORMER_EchoBlaster128RevC + 1 ) 
    } 	BEAMFORMER_TYPE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("75D16B6D-E7F7-3434-9657-E0A1AB945E22") 
enum tagBioeffectIndex
    {
        INDEX_MI	= 0,
        INDEX_TI	= ( INDEX_MI + 1 ) ,
        INDEX_TIS	= ( INDEX_TI + 1 ) ,
        INDEX_TIB	= ( INDEX_TIS + 1 ) ,
        INDEX_TIC	= ( INDEX_TIB + 1 ) 
    } 	tagBioeffectIndex;

typedef tagBioeffectIndex BIOEFFECT_INDEX;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("6EEF4791-1B29-3A68-8856-5AB53FFFB853") 
enum tagBatteryChargeStatus
    {
        Charging	= 1,
        Critical	= 2,
        High	= 4,
        Low	= 8,
        NoBattery	= 0x10,
        ChargeUnknown	= 0xff
    } 	tagBatteryChargeStatus;

typedef tagBatteryChargeStatus BATTERY_CHARGE_STATUS;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("04AEB099-EE7F-3DCC-9753-FFBC0A1D75FF") 
enum tagPowerLineStatus
    {
        Offline	= 0,
        Online	= ( Offline + 1 ) ,
        LineUnknown	= ( Online + 1 ) 
    } 	tagPowerLineStatus;

typedef tagPowerLineStatus POWER_LINE_STATUS;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("4E01FFE8-8021-3560-8225-53B8FDC690D0") 
enum tagProbeType
    {
        PROBETYPE_Undefined	= 0,
        PROBETYPE_Linear	= ( PROBETYPE_Undefined + 1 ) ,
        PROBETYPE_Convex	= ( PROBETYPE_Linear + 1 ) ,
        PROBETYPE_PhasedArray	= ( PROBETYPE_Convex + 1 ) ,
        PROBETYPE_TProbe	= ( PROBETYPE_PhasedArray + 1 ) ,
        PROBETYPE_Convex3D	= ( PROBETYPE_TProbe + 1 ) ,
        PROBETYPE_ProbeDuo	= ( PROBETYPE_Convex3D + 1 ) 
    } 	tagProbeType;

typedef tagProbeType PROBE_TYPE;

typedef /* [uuid][public] */  DECLSPEC_UUID("95894D68-3D0F-3471-9A17-AC56F3D5C96D") struct tagFocalZone
    {
    LONG nFocusPoint;
    LONG nZoneBegin;
    LONG nZoneEnd;
    } 	tagFocalZone;

typedef tagFocalZone FOCAL_ZONE;

typedef tagFocalZone *PFOCAL_ZONE;

typedef /* [uuid][public] */  DECLSPEC_UUID("BD8E4326-286C-37AD-98F0-0BEC6E05C703") struct tagImageResolution
    {
    LONG nXPelsPerUnit;
    LONG nYPelsPerUnit;
    } 	tagImageResolution;

typedef tagImageResolution IMAGE_RESOLUTION;

typedef tagImageResolution *PIMAGE_RESOLUTION;

typedef /* [uuid][public] */  DECLSPEC_UUID("F8F2BD08-DA7F-3DA3-9D20-D63FF966F489") struct tagValueBounds
    {
    LONG minimum;
    LONG maximum;
    } 	tagValueBounds;

typedef tagValueBounds VALUE_BOUNDS;

typedef tagValueBounds *PVALUE_BOUNDS;

typedef /* [uuid][public] */  DECLSPEC_UUID("EC51071F-5032-3002-B185-CC51B8FE646E") struct tagValueStepping
    {
    LONG stepping;
    LONG minimum;
    LONG maximum;
    } 	tagValueStepping;

typedef tagValueStepping VALUE_STEPPING;

typedef tagValueStepping *PVALUE_STEPPING;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("C09BD155-805C-3548-B401-6B9341F3E002") 
enum tagUnitBase
    {
        Code	= 0,
        Second	= ( Code + 1 ) ,
        Meter	= ( Second + 1 ) ,
        Bell	= ( Meter + 1 ) ,
        Hertz	= ( Bell + 1 ) ,
        Gramm	= ( Hertz + 1 ) ,
        Percent	= ( Gramm + 1 ) ,
        Volt	= ( Percent + 1 ) 
    } 	tagUnitBase;

typedef tagUnitBase UNIT_BASE;

typedef /* [uuid][public] */  DECLSPEC_UUID("44E6135C-9A22-374B-84BA-B4180713E350") struct tagScaleFactor
    {
    LONG Numerator;
    LONG Denominator;
    } 	tagScaleFactor;

typedef tagScaleFactor SCALE_FACTOR;

typedef tagScaleFactor *PSCALE_FACTOR;

typedef /* [uuid][public] */  DECLSPEC_UUID("D1053E8E-BDAE-3A0B-9FF9-1EC22F504AEE") struct tagUsgUnit
    {
    tagUnitBase UnitBase;
    SCALE_FACTOR ScaleFactor;
    } 	tagUsgUnit;

typedef tagUsgUnit USG_UNIT;

typedef tagUsgUnit *PUSG_UNIT;



extern RPC_IF_HANDLE __MIDL_itf_USgfw2_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_USgfw2_0000_0000_v0_0_s_ifspec;

#ifndef __IUsgDataView_INTERFACE_DEFINED__
#define __IUsgDataView_INTERFACE_DEFINED__

/* interface IUsgDataView */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDataView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFEF8670-DBC1-4B47-BC01-BBBCA174DD31")
    IUsgDataView : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanMode( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScanMode( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanState( 
            /* [retval][out] */ SCAN_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScanState( 
            /* [in] */ SCAN_STATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanModes( 
            /* [retval][out] */ IScanModes **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanModeObj( 
            /* [in] */ ULONG scanMode,
            /* [out] */ IUsgScanMode **pUsgScanMode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FilgraphManager( 
            /* [retval][out] */ IMediaControl **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetControlObj( 
            /* [in] */ const IID *riidCtrl,
            /* [in] */ ULONG scanMode,
            /* [in] */ ULONG streamId,
            /* [out] */ IUsgControl **ppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateUsgfwFile( 
            /* [in] */ BSTR FileName,
            /* [in] */ LONG createFlags,
            /* [retval][out] */ IUsgFileStorage **ppFileObject) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDataViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDataView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDataView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDataView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDataView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDataView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDataView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDataView * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanMode )( 
            IUsgDataView * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ScanMode )( 
            IUsgDataView * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanState )( 
            IUsgDataView * This,
            /* [retval][out] */ SCAN_STATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ScanState )( 
            IUsgDataView * This,
            /* [in] */ SCAN_STATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanModes )( 
            IUsgDataView * This,
            /* [retval][out] */ IScanModes **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanModeObj )( 
            IUsgDataView * This,
            /* [in] */ ULONG scanMode,
            /* [out] */ IUsgScanMode **pUsgScanMode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FilgraphManager )( 
            IUsgDataView * This,
            /* [retval][out] */ IMediaControl **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetControlObj )( 
            IUsgDataView * This,
            /* [in] */ const IID *riidCtrl,
            /* [in] */ ULONG scanMode,
            /* [in] */ ULONG streamId,
            /* [out] */ IUsgControl **ppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateUsgfwFile )( 
            IUsgDataView * This,
            /* [in] */ BSTR FileName,
            /* [in] */ LONG createFlags,
            /* [retval][out] */ IUsgFileStorage **ppFileObject);
        
        END_INTERFACE
    } IUsgDataViewVtbl;

    interface IUsgDataView
    {
        CONST_VTBL struct IUsgDataViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDataView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDataView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDataView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDataView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDataView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDataView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDataView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDataView_get_ScanMode(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanMode(This,pVal) ) 

#define IUsgDataView_put_ScanMode(This,newVal)	\
    ( (This)->lpVtbl -> put_ScanMode(This,newVal) ) 

#define IUsgDataView_get_ScanState(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanState(This,pVal) ) 

#define IUsgDataView_put_ScanState(This,newVal)	\
    ( (This)->lpVtbl -> put_ScanState(This,newVal) ) 

#define IUsgDataView_get_ScanModes(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanModes(This,pVal) ) 

#define IUsgDataView_GetScanModeObj(This,scanMode,pUsgScanMode)	\
    ( (This)->lpVtbl -> GetScanModeObj(This,scanMode,pUsgScanMode) ) 

#define IUsgDataView_get_FilgraphManager(This,pVal)	\
    ( (This)->lpVtbl -> get_FilgraphManager(This,pVal) ) 

#define IUsgDataView_GetControlObj(This,riidCtrl,scanMode,streamId,ppObj)	\
    ( (This)->lpVtbl -> GetControlObj(This,riidCtrl,scanMode,streamId,ppObj) ) 

#define IUsgDataView_CreateUsgfwFile(This,FileName,createFlags,ppFileObject)	\
    ( (This)->lpVtbl -> CreateUsgfwFile(This,FileName,createFlags,ppFileObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDataView_INTERFACE_DEFINED__ */


#ifndef __IUsgScanMode_INTERFACE_DEFINED__
#define __IUsgScanMode_INTERFACE_DEFINED__

/* interface IUsgScanMode */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanMode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B8CBA727-D104-416E-B0CC-C62E9CF9B1F9")
    IUsgScanMode : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMixerControl( 
            /* [in] */ ULONG scanMode,
            /* [in] */ ULONG streamId,
            /* [out] */ IUsgMixerControl **ppMixerCtrl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetControlObj( 
            /* [in] */ const IID *riidCtrl,
            /* [in] */ LONG scanMode,
            /* [in] */ ULONG streamId,
            /* [out] */ IUsgControl **ppCtrl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateCopy( 
            /* [retval][out] */ IUsgScanMode **ppNewScanMode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Union( 
            /* [in] */ ULONG nScanMode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamsCollection( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanMode( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateClone( 
            /* [retval][out] */ IUsgScanMode **ppNewScanMode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanModeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanMode * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanMode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanMode * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanMode * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanMode * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanMode * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMixerControl )( 
            IUsgScanMode * This,
            /* [in] */ ULONG scanMode,
            /* [in] */ ULONG streamId,
            /* [out] */ IUsgMixerControl **ppMixerCtrl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetControlObj )( 
            IUsgScanMode * This,
            /* [in] */ const IID *riidCtrl,
            /* [in] */ LONG scanMode,
            /* [in] */ ULONG streamId,
            /* [out] */ IUsgControl **ppCtrl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateCopy )( 
            IUsgScanMode * This,
            /* [retval][out] */ IUsgScanMode **ppNewScanMode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Union )( 
            IUsgScanMode * This,
            /* [in] */ ULONG nScanMode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StreamsCollection )( 
            IUsgScanMode * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanMode )( 
            IUsgScanMode * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateClone )( 
            IUsgScanMode * This,
            /* [retval][out] */ IUsgScanMode **ppNewScanMode);
        
        END_INTERFACE
    } IUsgScanModeVtbl;

    interface IUsgScanMode
    {
        CONST_VTBL struct IUsgScanModeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanMode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanMode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanMode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanMode_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanMode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanMode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanMode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanMode_GetMixerControl(This,scanMode,streamId,ppMixerCtrl)	\
    ( (This)->lpVtbl -> GetMixerControl(This,scanMode,streamId,ppMixerCtrl) ) 

#define IUsgScanMode_GetControlObj(This,riidCtrl,scanMode,streamId,ppCtrl)	\
    ( (This)->lpVtbl -> GetControlObj(This,riidCtrl,scanMode,streamId,ppCtrl) ) 

#define IUsgScanMode_CreateCopy(This,ppNewScanMode)	\
    ( (This)->lpVtbl -> CreateCopy(This,ppNewScanMode) ) 

#define IUsgScanMode_Union(This,nScanMode)	\
    ( (This)->lpVtbl -> Union(This,nScanMode) ) 

#define IUsgScanMode_get_StreamsCollection(This,pVal)	\
    ( (This)->lpVtbl -> get_StreamsCollection(This,pVal) ) 

#define IUsgScanMode_get_ScanMode(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanMode(This,pVal) ) 

#define IUsgScanMode_CreateClone(This,ppNewScanMode)	\
    ( (This)->lpVtbl -> CreateClone(This,ppNewScanMode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanMode_INTERFACE_DEFINED__ */


#ifndef __IUsgDataStream_INTERFACE_DEFINED__
#define __IUsgDataStream_INTERFACE_DEFINED__

/* interface IUsgDataStream */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDataStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EDF488C3-5BE4-4D32-B96F-D9BDDFC77C43")
    IUsgDataStream : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStreamMode( 
            /* [retval][out] */ ULONG *pMode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetControlObj( 
            /* [in] */ REFIID riid,
            /* [retval][out] */ IUnknown **ppobj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMixerControl( 
            /* [retval][out] */ IUsgMixerControl **ppMixerControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamId( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanState( 
            /* [retval][out] */ SCAN_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScanState( 
            /* [in] */ SCAN_STATE newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDataStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDataStream * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDataStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDataStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDataStream * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDataStream * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDataStream * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDataStream * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStreamMode )( 
            IUsgDataStream * This,
            /* [retval][out] */ ULONG *pMode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetControlObj )( 
            IUsgDataStream * This,
            /* [in] */ REFIID riid,
            /* [retval][out] */ IUnknown **ppobj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMixerControl )( 
            IUsgDataStream * This,
            /* [retval][out] */ IUsgMixerControl **ppMixerControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StreamId )( 
            IUsgDataStream * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanState )( 
            IUsgDataStream * This,
            /* [retval][out] */ SCAN_STATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ScanState )( 
            IUsgDataStream * This,
            /* [in] */ SCAN_STATE newVal);
        
        END_INTERFACE
    } IUsgDataStreamVtbl;

    interface IUsgDataStream
    {
        CONST_VTBL struct IUsgDataStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDataStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDataStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDataStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDataStream_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDataStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDataStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDataStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDataStream_GetStreamMode(This,pMode)	\
    ( (This)->lpVtbl -> GetStreamMode(This,pMode) ) 

#define IUsgDataStream_GetControlObj(This,riid,ppobj)	\
    ( (This)->lpVtbl -> GetControlObj(This,riid,ppobj) ) 

#define IUsgDataStream_GetMixerControl(This,ppMixerControl)	\
    ( (This)->lpVtbl -> GetMixerControl(This,ppMixerControl) ) 

#define IUsgDataStream_get_StreamId(This,pVal)	\
    ( (This)->lpVtbl -> get_StreamId(This,pVal) ) 

#define IUsgDataStream_get_ScanState(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanState(This,pVal) ) 

#define IUsgDataStream_put_ScanState(This,newVal)	\
    ( (This)->lpVtbl -> put_ScanState(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDataStream_INTERFACE_DEFINED__ */


#ifndef __IUsgMixerControl_INTERFACE_DEFINED__
#define __IUsgMixerControl_INTERFACE_DEFINED__

/* interface IUsgMixerControl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgMixerControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B7C94539-A65A-42F3-8B65-F83D114FF4C8")
    IUsgMixerControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOutputWindow( 
            /* [in] */ LONG hWnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlphaBitmapParameters( 
            /* [out] */ USG_MIXING_BITMAP *pBmpParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAlphaBitmap( 
            /* [in] */ USG_MIXING_BITMAP *pBmpParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateAlphaBitmapParameters( 
            /* [in] */ USG_MIXING_BITMAP *pBmpParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOutputRect( 
            /* [in] */ RECT *pRect) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Capture( 
            /* [out] */ IPictureDisp **ppCurrentImage) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentBitmap( 
            /* [retval][out] */ LONG *handle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Repaint( 
            LONG hWindow,
            LONG hDC) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanState( 
            /* [retval][out] */ SCAN_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScanState( 
            /* [in] */ SCAN_STATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BkColor( 
            /* [retval][out] */ PALETTEENTRY *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BkColor( 
            /* [in] */ PALETTEENTRY newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOutputRect( 
            /* [out] */ RECT *pRect) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgMixerControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgMixerControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgMixerControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgMixerControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgMixerControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgMixerControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgMixerControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgMixerControl * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOutputWindow )( 
            IUsgMixerControl * This,
            /* [in] */ LONG hWnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlphaBitmapParameters )( 
            IUsgMixerControl * This,
            /* [out] */ USG_MIXING_BITMAP *pBmpParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAlphaBitmap )( 
            IUsgMixerControl * This,
            /* [in] */ USG_MIXING_BITMAP *pBmpParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateAlphaBitmapParameters )( 
            IUsgMixerControl * This,
            /* [in] */ USG_MIXING_BITMAP *pBmpParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOutputRect )( 
            IUsgMixerControl * This,
            /* [in] */ RECT *pRect);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Capture )( 
            IUsgMixerControl * This,
            /* [out] */ IPictureDisp **ppCurrentImage);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentBitmap )( 
            IUsgMixerControl * This,
            /* [retval][out] */ LONG *handle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Repaint )( 
            IUsgMixerControl * This,
            LONG hWindow,
            LONG hDC);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanState )( 
            IUsgMixerControl * This,
            /* [retval][out] */ SCAN_STATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ScanState )( 
            IUsgMixerControl * This,
            /* [in] */ SCAN_STATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BkColor )( 
            IUsgMixerControl * This,
            /* [retval][out] */ PALETTEENTRY *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BkColor )( 
            IUsgMixerControl * This,
            /* [in] */ PALETTEENTRY newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOutputRect )( 
            IUsgMixerControl * This,
            /* [out] */ RECT *pRect);
        
        END_INTERFACE
    } IUsgMixerControlVtbl;

    interface IUsgMixerControl
    {
        CONST_VTBL struct IUsgMixerControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgMixerControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgMixerControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgMixerControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgMixerControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgMixerControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgMixerControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgMixerControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgMixerControl_SetOutputWindow(This,hWnd)	\
    ( (This)->lpVtbl -> SetOutputWindow(This,hWnd) ) 

#define IUsgMixerControl_GetAlphaBitmapParameters(This,pBmpParams)	\
    ( (This)->lpVtbl -> GetAlphaBitmapParameters(This,pBmpParams) ) 

#define IUsgMixerControl_SetAlphaBitmap(This,pBmpParams)	\
    ( (This)->lpVtbl -> SetAlphaBitmap(This,pBmpParams) ) 

#define IUsgMixerControl_UpdateAlphaBitmapParameters(This,pBmpParams)	\
    ( (This)->lpVtbl -> UpdateAlphaBitmapParameters(This,pBmpParams) ) 

#define IUsgMixerControl_SetOutputRect(This,pRect)	\
    ( (This)->lpVtbl -> SetOutputRect(This,pRect) ) 

#define IUsgMixerControl_Capture(This,ppCurrentImage)	\
    ( (This)->lpVtbl -> Capture(This,ppCurrentImage) ) 

#define IUsgMixerControl_GetCurrentBitmap(This,handle)	\
    ( (This)->lpVtbl -> GetCurrentBitmap(This,handle) ) 

#define IUsgMixerControl_Repaint(This,hWindow,hDC)	\
    ( (This)->lpVtbl -> Repaint(This,hWindow,hDC) ) 

#define IUsgMixerControl_get_ScanState(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanState(This,pVal) ) 

#define IUsgMixerControl_put_ScanState(This,newVal)	\
    ( (This)->lpVtbl -> put_ScanState(This,newVal) ) 

#define IUsgMixerControl_get_BkColor(This,pVal)	\
    ( (This)->lpVtbl -> get_BkColor(This,pVal) ) 

#define IUsgMixerControl_put_BkColor(This,newVal)	\
    ( (This)->lpVtbl -> put_BkColor(This,newVal) ) 

#define IUsgMixerControl_GetOutputRect(This,pRect)	\
    ( (This)->lpVtbl -> GetOutputRect(This,pRect) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgMixerControl_INTERFACE_DEFINED__ */


#ifndef __IUsgData_INTERFACE_DEFINED__
#define __IUsgData_INTERFACE_DEFINED__

/* interface IUsgData */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4363F1DB-1261-4BD6-99E5-B483ECB35218")
    IUsgData : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgData * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgData * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgData * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgData * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgData * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IUsgDataVtbl;

    interface IUsgData
    {
        CONST_VTBL struct IUsgDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgData_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgData_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgData_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgData_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgData_INTERFACE_DEFINED__ */


#ifndef __IUsgGraph_INTERFACE_DEFINED__
#define __IUsgGraph_INTERFACE_DEFINED__

/* interface IUsgGraph */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgGraph;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("739FDDCE-29FF-44D9-9C3C-3E6813A67969")
    IUsgGraph : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgGraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgGraph * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgGraph * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgGraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgGraph * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgGraph * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgGraph * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgGraph * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IUsgGraphVtbl;

    interface IUsgGraph
    {
        CONST_VTBL struct IUsgGraphVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgGraph_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgGraph_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgGraph_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgGraph_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgGraph_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgGraph_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgGraph_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgGraph_INTERFACE_DEFINED__ */


#ifndef __IUsgDeviceChangeSink_INTERFACE_DEFINED__
#define __IUsgDeviceChangeSink_INTERFACE_DEFINED__

/* interface IUsgDeviceChangeSink */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDeviceChangeSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9717780E-AAAF-4FD2-835A-80910E1E803E")
    IUsgDeviceChangeSink : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnProbeArrive( 
            /* [in] */ IUnknown *pUsgProbe,
            /* [out][in] */ ULONG *reserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnBeamformerArrive( 
            /* [in] */ IUnknown *pUsgBeamformer,
            /* [out][in] */ ULONG *reserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnProbeRemove( 
            /* [in] */ IUnknown *pUsgProbe,
            /* [out][in] */ ULONG *reserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnBeamformerRemove( 
            /* [in] */ IUnknown *pUsgBeamformer,
            /* [out][in] */ ULONG *reserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnProbeStateChanged( 
            /* [in] */ IUnknown *pUsgProbe,
            /* [out][in] */ ULONG *reserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnBeamformerStateChanged( 
            /* [in] */ IUnknown *pUsgBeamformer,
            /* [out][in] */ ULONG *reserved) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDeviceChangeSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDeviceChangeSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDeviceChangeSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDeviceChangeSink * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDeviceChangeSink * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnProbeArrive )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ IUnknown *pUsgProbe,
            /* [out][in] */ ULONG *reserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnBeamformerArrive )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ IUnknown *pUsgBeamformer,
            /* [out][in] */ ULONG *reserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnProbeRemove )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ IUnknown *pUsgProbe,
            /* [out][in] */ ULONG *reserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnBeamformerRemove )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ IUnknown *pUsgBeamformer,
            /* [out][in] */ ULONG *reserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnProbeStateChanged )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ IUnknown *pUsgProbe,
            /* [out][in] */ ULONG *reserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnBeamformerStateChanged )( 
            IUsgDeviceChangeSink * This,
            /* [in] */ IUnknown *pUsgBeamformer,
            /* [out][in] */ ULONG *reserved);
        
        END_INTERFACE
    } IUsgDeviceChangeSinkVtbl;

    interface IUsgDeviceChangeSink
    {
        CONST_VTBL struct IUsgDeviceChangeSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDeviceChangeSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDeviceChangeSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDeviceChangeSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDeviceChangeSink_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDeviceChangeSink_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDeviceChangeSink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDeviceChangeSink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDeviceChangeSink_OnProbeArrive(This,pUsgProbe,reserved)	\
    ( (This)->lpVtbl -> OnProbeArrive(This,pUsgProbe,reserved) ) 

#define IUsgDeviceChangeSink_OnBeamformerArrive(This,pUsgBeamformer,reserved)	\
    ( (This)->lpVtbl -> OnBeamformerArrive(This,pUsgBeamformer,reserved) ) 

#define IUsgDeviceChangeSink_OnProbeRemove(This,pUsgProbe,reserved)	\
    ( (This)->lpVtbl -> OnProbeRemove(This,pUsgProbe,reserved) ) 

#define IUsgDeviceChangeSink_OnBeamformerRemove(This,pUsgBeamformer,reserved)	\
    ( (This)->lpVtbl -> OnBeamformerRemove(This,pUsgBeamformer,reserved) ) 

#define IUsgDeviceChangeSink_OnProbeStateChanged(This,pUsgProbe,reserved)	\
    ( (This)->lpVtbl -> OnProbeStateChanged(This,pUsgProbe,reserved) ) 

#define IUsgDeviceChangeSink_OnBeamformerStateChanged(This,pUsgBeamformer,reserved)	\
    ( (This)->lpVtbl -> OnBeamformerStateChanged(This,pUsgBeamformer,reserved) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDeviceChangeSink_INTERFACE_DEFINED__ */


#ifndef __IScanDepth_INTERFACE_DEFINED__
#define __IScanDepth_INTERFACE_DEFINED__

/* interface IScanDepth */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScanDepth;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7391AEBB-13BB-4ffe-AE84-48CD63B523A0")
    IScanDepth : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanDepth( 
            /* [retval][out] */ LONG *pScanDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScanDepth( 
            /* [in] */ LONG nScanDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanDepthRange( 
            /* [out] */ LONG *pScanDepthMin,
            /* [out] */ LONG *pScanDepthMax) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScanDepthVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScanDepth * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScanDepth * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScanDepth * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScanDepth * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScanDepth * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScanDepth * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScanDepth * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanDepth )( 
            IScanDepth * This,
            /* [retval][out] */ LONG *pScanDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetScanDepth )( 
            IScanDepth * This,
            /* [in] */ LONG nScanDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanDepthRange )( 
            IScanDepth * This,
            /* [out] */ LONG *pScanDepthMin,
            /* [out] */ LONG *pScanDepthMax);
        
        END_INTERFACE
    } IScanDepthVtbl;

    interface IScanDepth
    {
        CONST_VTBL struct IScanDepthVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScanDepth_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScanDepth_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScanDepth_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScanDepth_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IScanDepth_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IScanDepth_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IScanDepth_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IScanDepth_GetScanDepth(This,pScanDepth)	\
    ( (This)->lpVtbl -> GetScanDepth(This,pScanDepth) ) 

#define IScanDepth_SetScanDepth(This,nScanDepth)	\
    ( (This)->lpVtbl -> SetScanDepth(This,nScanDepth) ) 

#define IScanDepth_GetScanDepthRange(This,pScanDepthMin,pScanDepthMax)	\
    ( (This)->lpVtbl -> GetScanDepthRange(This,pScanDepthMin,pScanDepthMax) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScanDepth_INTERFACE_DEFINED__ */


#ifndef __IUsgfw2_INTERFACE_DEFINED__
#define __IUsgfw2_INTERFACE_DEFINED__

/* interface IUsgfw2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgfw2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AAE0C833-BFE6-4594-984E-8B9FD48CA487")
    IUsgfw2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProbesCollection( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BeamformersCollection( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateDataView( 
            /* [in] */ IUnknown *pUnkSource,
            /* [retval][out] */ IUsgDataView **ppUsgDataView) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Invalidate( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeamformerNameFromCode( 
            /* [in] */ LONG Code,
            /* [retval][out] */ BSTR *Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeamformerCodeFromName( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ LONG *Code) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProbeNameFromCode( 
            /* [in] */ LONG Code,
            /* [retval][out] */ BSTR *Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProbeCodeFromName( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ LONG *Code) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateUsgfwFile( 
            /* [in] */ BSTR fileName,
            /* [in] */ LONG createFlags,
            /* [retval][out] */ IUsgFileStorage **ppFileObject) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgfw2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgfw2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgfw2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgfw2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgfw2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgfw2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgfw2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgfw2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProbesCollection )( 
            IUsgfw2 * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BeamformersCollection )( 
            IUsgfw2 * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateDataView )( 
            IUsgfw2 * This,
            /* [in] */ IUnknown *pUnkSource,
            /* [retval][out] */ IUsgDataView **ppUsgDataView);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Invalidate )( 
            IUsgfw2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeamformerNameFromCode )( 
            IUsgfw2 * This,
            /* [in] */ LONG Code,
            /* [retval][out] */ BSTR *Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeamformerCodeFromName )( 
            IUsgfw2 * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ LONG *Code);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProbeNameFromCode )( 
            IUsgfw2 * This,
            /* [in] */ LONG Code,
            /* [retval][out] */ BSTR *Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProbeCodeFromName )( 
            IUsgfw2 * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ LONG *Code);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateUsgfwFile )( 
            IUsgfw2 * This,
            /* [in] */ BSTR fileName,
            /* [in] */ LONG createFlags,
            /* [retval][out] */ IUsgFileStorage **ppFileObject);
        
        END_INTERFACE
    } IUsgfw2Vtbl;

    interface IUsgfw2
    {
        CONST_VTBL struct IUsgfw2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgfw2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgfw2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgfw2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgfw2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgfw2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgfw2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgfw2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgfw2_get_ProbesCollection(This,pVal)	\
    ( (This)->lpVtbl -> get_ProbesCollection(This,pVal) ) 

#define IUsgfw2_get_BeamformersCollection(This,pVal)	\
    ( (This)->lpVtbl -> get_BeamformersCollection(This,pVal) ) 

#define IUsgfw2_CreateDataView(This,pUnkSource,ppUsgDataView)	\
    ( (This)->lpVtbl -> CreateDataView(This,pUnkSource,ppUsgDataView) ) 

#define IUsgfw2_Invalidate(This)	\
    ( (This)->lpVtbl -> Invalidate(This) ) 

#define IUsgfw2_BeamformerNameFromCode(This,Code,Name)	\
    ( (This)->lpVtbl -> BeamformerNameFromCode(This,Code,Name) ) 

#define IUsgfw2_BeamformerCodeFromName(This,Name,Code)	\
    ( (This)->lpVtbl -> BeamformerCodeFromName(This,Name,Code) ) 

#define IUsgfw2_ProbeNameFromCode(This,Code,Name)	\
    ( (This)->lpVtbl -> ProbeNameFromCode(This,Code,Name) ) 

#define IUsgfw2_ProbeCodeFromName(This,Name,Code)	\
    ( (This)->lpVtbl -> ProbeCodeFromName(This,Name,Code) ) 

#define IUsgfw2_CreateUsgfwFile(This,fileName,createFlags,ppFileObject)	\
    ( (This)->lpVtbl -> CreateUsgfwFile(This,fileName,createFlags,ppFileObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgfw2_INTERFACE_DEFINED__ */


#ifndef __IProbesCollection_INTERFACE_DEFINED__
#define __IProbesCollection_INTERFACE_DEFINED__

/* interface IProbesCollection */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IProbesCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1C3AF9E8-2597-4A1C-ADEA-6F9A17645A16")
    IProbesCollection : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IProbesCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProbesCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProbesCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProbesCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProbesCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProbesCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProbesCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProbesCollection * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IProbesCollectionVtbl;

    interface IProbesCollection
    {
        CONST_VTBL struct IProbesCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProbesCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProbesCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProbesCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProbesCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProbesCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProbesCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProbesCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProbesCollection_INTERFACE_DEFINED__ */


#ifndef __IUsgCollection_INTERFACE_DEFINED__
#define __IUsgCollection_INTERFACE_DEFINED__

/* interface IUsgCollection */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EAA864EC-F0B8-49EF-BF78-09B8379D0D62")
    IUsgCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ LONG lItem,
            /* [out] */ IUnknown **ppUnk) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCollection * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IUsgCollection * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IUsgCollection * This,
            /* [in] */ LONG lItem,
            /* [out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } IUsgCollectionVtbl;

    interface IUsgCollection
    {
        CONST_VTBL struct IUsgCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCollection_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IUsgCollection_Item(This,lItem,ppUnk)	\
    ( (This)->lpVtbl -> Item(This,lItem,ppUnk) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCollection_INTERFACE_DEFINED__ */


#ifndef __IProbe_INTERFACE_DEFINED__
#define __IProbe_INTERFACE_DEFINED__

/* interface IProbe */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IProbe;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("264096B1-8393-4060-907B-917C395FF97C")
    IProbe : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Beamformer( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeviceState( 
            /* [retval][out] */ DEVICE_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DispalyName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Code( 
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProbeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProbe * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProbe * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProbe * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProbe * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProbe * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProbe * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProbe * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IProbe * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Beamformer )( 
            IProbe * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviceState )( 
            IProbe * This,
            /* [retval][out] */ DEVICE_STATE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DispalyName )( 
            IProbe * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Code )( 
            IProbe * This,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } IProbeVtbl;

    interface IProbe
    {
        CONST_VTBL struct IProbeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProbe_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProbe_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProbe_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProbe_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProbe_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProbe_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProbe_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IProbe_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IProbe_get_Beamformer(This,pVal)	\
    ( (This)->lpVtbl -> get_Beamformer(This,pVal) ) 

#define IProbe_get_DeviceState(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviceState(This,pVal) ) 

#define IProbe_get_DispalyName(This,pVal)	\
    ( (This)->lpVtbl -> get_DispalyName(This,pVal) ) 

#define IProbe_get_Code(This,pVal)	\
    ( (This)->lpVtbl -> get_Code(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProbe_INTERFACE_DEFINED__ */


#ifndef __IProbe2_INTERFACE_DEFINED__
#define __IProbe2_INTERFACE_DEFINED__

/* interface IProbe2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IProbe2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7500FEC3-E775-4d0c-91D8-59DF9C3ED7EB")
    IProbe2 : public IProbe
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ PROBE_TYPE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SerialNumber( 
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProbe2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProbe2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProbe2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProbe2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProbe2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProbe2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProbe2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProbe2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IProbe2 * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Beamformer )( 
            IProbe2 * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviceState )( 
            IProbe2 * This,
            /* [retval][out] */ DEVICE_STATE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DispalyName )( 
            IProbe2 * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Code )( 
            IProbe2 * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IProbe2 * This,
            /* [retval][out] */ PROBE_TYPE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SerialNumber )( 
            IProbe2 * This,
            /* [in] */ LONG index,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IProbe2Vtbl;

    interface IProbe2
    {
        CONST_VTBL struct IProbe2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProbe2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProbe2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProbe2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProbe2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProbe2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProbe2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProbe2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IProbe2_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IProbe2_get_Beamformer(This,pVal)	\
    ( (This)->lpVtbl -> get_Beamformer(This,pVal) ) 

#define IProbe2_get_DeviceState(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviceState(This,pVal) ) 

#define IProbe2_get_DispalyName(This,pVal)	\
    ( (This)->lpVtbl -> get_DispalyName(This,pVal) ) 

#define IProbe2_get_Code(This,pVal)	\
    ( (This)->lpVtbl -> get_Code(This,pVal) ) 


#define IProbe2_get_Type(This,pVal)	\
    ( (This)->lpVtbl -> get_Type(This,pVal) ) 

#define IProbe2_get_SerialNumber(This,index,pVal)	\
    ( (This)->lpVtbl -> get_SerialNumber(This,index,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProbe2_INTERFACE_DEFINED__ */


#ifndef __IBeamformer_INTERFACE_DEFINED__
#define __IBeamformer_INTERFACE_DEFINED__

/* interface IBeamformer */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBeamformer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1AF2973E-1991-4A7A-86AF-7EA0150C6925")
    IBeamformer : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Probes( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeviceState( 
            /* [retval][out] */ DEVICE_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenDevice( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SwitchConnector( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SwitchConnectorSupported( 
            /* [retval][out] */ LONG *bSupported) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Code( 
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBeamformerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBeamformer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBeamformer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBeamformer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBeamformer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBeamformer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBeamformer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBeamformer * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IBeamformer * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Probes )( 
            IBeamformer * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviceState )( 
            IBeamformer * This,
            /* [retval][out] */ DEVICE_STATE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DisplayName )( 
            IBeamformer * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenDevice )( 
            IBeamformer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SwitchConnector )( 
            IBeamformer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SwitchConnectorSupported )( 
            IBeamformer * This,
            /* [retval][out] */ LONG *bSupported);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Code )( 
            IBeamformer * This,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } IBeamformerVtbl;

    interface IBeamformer
    {
        CONST_VTBL struct IBeamformerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBeamformer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBeamformer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBeamformer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBeamformer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBeamformer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBeamformer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBeamformer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBeamformer_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IBeamformer_get_Probes(This,pVal)	\
    ( (This)->lpVtbl -> get_Probes(This,pVal) ) 

#define IBeamformer_get_DeviceState(This,pVal)	\
    ( (This)->lpVtbl -> get_DeviceState(This,pVal) ) 

#define IBeamformer_get_DisplayName(This,pVal)	\
    ( (This)->lpVtbl -> get_DisplayName(This,pVal) ) 

#define IBeamformer_OpenDevice(This)	\
    ( (This)->lpVtbl -> OpenDevice(This) ) 

#define IBeamformer_SwitchConnector(This)	\
    ( (This)->lpVtbl -> SwitchConnector(This) ) 

#define IBeamformer_SwitchConnectorSupported(This,bSupported)	\
    ( (This)->lpVtbl -> SwitchConnectorSupported(This,bSupported) ) 

#define IBeamformer_get_Code(This,pVal)	\
    ( (This)->lpVtbl -> get_Code(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBeamformer_INTERFACE_DEFINED__ */


#ifndef __IScanModes_INTERFACE_DEFINED__
#define __IScanModes_INTERFACE_DEFINED__

/* interface IScanModes */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScanModes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("60C480B7-F1E7-403C-8AF3-8DCED99A2560")
    IScanModes : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ LONG lItem,
            /* [out] */ ULONG *pScanMode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IScanModesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScanModes * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScanModes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScanModes * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScanModes * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScanModes * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScanModes * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScanModes * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScanModes * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IScanModes * This,
            /* [in] */ LONG lItem,
            /* [out] */ ULONG *pScanMode);
        
        END_INTERFACE
    } IScanModesVtbl;

    interface IScanModes
    {
        CONST_VTBL struct IScanModesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScanModes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IScanModes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IScanModes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IScanModes_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IScanModes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IScanModes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IScanModes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IScanModes_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IScanModes_Item(This,lItem,pScanMode)	\
    ( (This)->lpVtbl -> Item(This,lItem,pScanMode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScanModes_INTERFACE_DEFINED__ */


#ifndef __IUsgControl_INTERFACE_DEFINED__
#define __IUsgControl_INTERFACE_DEFINED__

/* interface IUsgControl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5748CA80-1710-489F-BC13-28F2C0122B49")
    IUsgControl : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgControl * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IUsgControlVtbl;

    interface IUsgControl
    {
        CONST_VTBL struct IUsgControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgControl_INTERFACE_DEFINED__ */


#ifndef __IUsgGain_INTERFACE_DEFINED__
#define __IUsgGain_INTERFACE_DEFINED__

/* interface IUsgGain */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgGain;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A18F0D3F-DD69-4BDE-8F26-4F54D67B57D0")
    IUsgGain : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgGainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgGain * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgGain * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgGain * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgGain * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgGain * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgGain * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgGain * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgGain * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgGain * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgGain * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgGainVtbl;

    interface IUsgGain
    {
        CONST_VTBL struct IUsgGainVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgGain_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgGain_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgGain_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgGain_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgGain_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgGain_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgGain_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgGain_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgGain_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgGain_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgGain_INTERFACE_DEFINED__ */


#ifndef __IUsgValues_INTERFACE_DEFINED__
#define __IUsgValues_INTERFACE_DEFINED__

/* interface IUsgValues */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgValues;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("95D11D2B-EC05-4A2E-B31B-1386C484AE16")
    IUsgValues : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ LONG lItem,
            /* [retval][out] */ VARIANT *pValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgValuesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgValues * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgValues * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgValues * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgValues * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgValues * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgValues * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgValues * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IUsgValues * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IUsgValues * This,
            /* [in] */ LONG lItem,
            /* [retval][out] */ VARIANT *pValue);
        
        END_INTERFACE
    } IUsgValuesVtbl;

    interface IUsgValues
    {
        CONST_VTBL struct IUsgValuesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgValues_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgValues_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgValues_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgValues_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgValues_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgValues_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgValues_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgValues_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IUsgValues_Item(This,lItem,pValue)	\
    ( (This)->lpVtbl -> Item(This,lItem,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgValues_INTERFACE_DEFINED__ */


#ifndef __IUsgPower_INTERFACE_DEFINED__
#define __IUsgPower_INTERFACE_DEFINED__

/* interface IUsgPower */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgPower;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F23DC92E-60CB-4EAE-8CD1-BD729E8D785C")
    IUsgPower : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgPowerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgPower * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgPower * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgPower * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgPower * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgPower * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgPower * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgPower * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgPower * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgPower * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgPower * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgPowerVtbl;

    interface IUsgPower
    {
        CONST_VTBL struct IUsgPowerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgPower_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgPower_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgPower_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgPower_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgPower_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgPower_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgPower_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgPower_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgPower_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgPower_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgPower_INTERFACE_DEFINED__ */


#ifndef __IUsgDynamicRange_INTERFACE_DEFINED__
#define __IUsgDynamicRange_INTERFACE_DEFINED__

/* interface IUsgDynamicRange */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDynamicRange;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B3194B41-4E87-4787-8E79-25633A7596D9")
    IUsgDynamicRange : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDynamicRangeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDynamicRange * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDynamicRange * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDynamicRange * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDynamicRange * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDynamicRange * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDynamicRange * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDynamicRange * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDynamicRange * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDynamicRange * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDynamicRange * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDynamicRangeVtbl;

    interface IUsgDynamicRange
    {
        CONST_VTBL struct IUsgDynamicRangeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDynamicRange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDynamicRange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDynamicRange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDynamicRange_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDynamicRange_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDynamicRange_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDynamicRange_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDynamicRange_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDynamicRange_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDynamicRange_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDynamicRange_INTERFACE_DEFINED__ */


#ifndef __IUsgFrameAvg_INTERFACE_DEFINED__
#define __IUsgFrameAvg_INTERFACE_DEFINED__

/* interface IUsgFrameAvg */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgFrameAvg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3FD666AF-C2B5-4A5D-AFCA-30EDE03427E2")
    IUsgFrameAvg : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgFrameAvgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgFrameAvg * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgFrameAvg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgFrameAvg * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgFrameAvg * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgFrameAvg * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgFrameAvg * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgFrameAvg * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgFrameAvg * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgFrameAvg * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgFrameAvg * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgFrameAvgVtbl;

    interface IUsgFrameAvg
    {
        CONST_VTBL struct IUsgFrameAvgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgFrameAvg_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgFrameAvg_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgFrameAvg_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgFrameAvg_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgFrameAvg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgFrameAvg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgFrameAvg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgFrameAvg_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgFrameAvg_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgFrameAvg_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgFrameAvg_INTERFACE_DEFINED__ */


#ifndef __IUsgRejection2_INTERFACE_DEFINED__
#define __IUsgRejection2_INTERFACE_DEFINED__

/* interface IUsgRejection2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgRejection2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3FE7E792-42A5-45D1-B054-7BF47C67DBFB")
    IUsgRejection2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgRejection2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgRejection2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgRejection2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgRejection2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgRejection2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgRejection2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgRejection2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgRejection2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgRejection2 * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgRejection2 * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgRejection2 * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgRejection2Vtbl;

    interface IUsgRejection2
    {
        CONST_VTBL struct IUsgRejection2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgRejection2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgRejection2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgRejection2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgRejection2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgRejection2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgRejection2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgRejection2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgRejection2_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgRejection2_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgRejection2_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgRejection2_INTERFACE_DEFINED__ */


#ifndef __IUsgProbeFrequency2_INTERFACE_DEFINED__
#define __IUsgProbeFrequency2_INTERFACE_DEFINED__

/* interface IUsgProbeFrequency2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgProbeFrequency2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53FCF15D-3C94-4AB3-9B8E-0CD67D733A24")
    IUsgProbeFrequency2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgProbeFrequency2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgProbeFrequency2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgProbeFrequency2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgProbeFrequency2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgProbeFrequency2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgProbeFrequency2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgProbeFrequency2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgProbeFrequency2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgProbeFrequency2 * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgProbeFrequency2 * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgProbeFrequency2 * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgProbeFrequency2Vtbl;

    interface IUsgProbeFrequency2
    {
        CONST_VTBL struct IUsgProbeFrequency2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgProbeFrequency2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgProbeFrequency2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgProbeFrequency2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgProbeFrequency2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgProbeFrequency2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgProbeFrequency2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgProbeFrequency2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgProbeFrequency2_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgProbeFrequency2_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgProbeFrequency2_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgProbeFrequency2_INTERFACE_DEFINED__ */


#ifndef __IUsgDepth_INTERFACE_DEFINED__
#define __IUsgDepth_INTERFACE_DEFINED__

/* interface IUsgDepth */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDepth;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("812D829E-9D55-406A-B89D-31A410839F87")
    IUsgDepth : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDepthVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDepth * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDepth * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDepth * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDepth * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDepth * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDepth * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDepth * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDepth * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDepth * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDepth * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDepthVtbl;

    interface IUsgDepth
    {
        CONST_VTBL struct IUsgDepthVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDepth_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDepth_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDepth_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDepth_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDepth_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDepth_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDepth_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDepth_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDepth_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDepth_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDepth_INTERFACE_DEFINED__ */


#ifndef __IUsgImageOrientation_INTERFACE_DEFINED__
#define __IUsgImageOrientation_INTERFACE_DEFINED__

/* interface IUsgImageOrientation */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgImageOrientation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("859BCBDB-015C-4439-9702-F0CB0FDF8059")
    IUsgImageOrientation : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mirror( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mirror( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rotate( 
            /* [retval][out] */ IMAGE_ROTATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Rotate( 
            /* [in] */ IMAGE_ROTATE newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgImageOrientationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgImageOrientation * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgImageOrientation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgImageOrientation * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgImageOrientation * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgImageOrientation * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgImageOrientation * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgImageOrientation * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Mirror )( 
            IUsgImageOrientation * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Mirror )( 
            IUsgImageOrientation * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rotate )( 
            IUsgImageOrientation * This,
            /* [retval][out] */ IMAGE_ROTATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Rotate )( 
            IUsgImageOrientation * This,
            /* [in] */ IMAGE_ROTATE newVal);
        
        END_INTERFACE
    } IUsgImageOrientationVtbl;

    interface IUsgImageOrientation
    {
        CONST_VTBL struct IUsgImageOrientationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgImageOrientation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgImageOrientation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgImageOrientation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgImageOrientation_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgImageOrientation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgImageOrientation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgImageOrientation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgImageOrientation_get_Mirror(This,pVal)	\
    ( (This)->lpVtbl -> get_Mirror(This,pVal) ) 

#define IUsgImageOrientation_put_Mirror(This,newVal)	\
    ( (This)->lpVtbl -> put_Mirror(This,newVal) ) 

#define IUsgImageOrientation_get_Rotate(This,pVal)	\
    ( (This)->lpVtbl -> get_Rotate(This,pVal) ) 

#define IUsgImageOrientation_put_Rotate(This,newVal)	\
    ( (This)->lpVtbl -> put_Rotate(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgImageOrientation_INTERFACE_DEFINED__ */


#ifndef __IUsgImageEnhancement_INTERFACE_DEFINED__
#define __IUsgImageEnhancement_INTERFACE_DEFINED__

/* interface IUsgImageEnhancement */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgImageEnhancement;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("90C02711-657D-436C-B865-DA76E7B5EA76")
    IUsgImageEnhancement : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgImageEnhancementVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgImageEnhancement * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgImageEnhancement * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgImageEnhancement * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgImageEnhancement * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgImageEnhancement * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgImageEnhancement * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgImageEnhancement * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgImageEnhancement * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgImageEnhancement * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgImageEnhancement * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IUsgImageEnhancement * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IUsgImageEnhancement * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IUsgImageEnhancementVtbl;

    interface IUsgImageEnhancement
    {
        CONST_VTBL struct IUsgImageEnhancementVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgImageEnhancement_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgImageEnhancement_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgImageEnhancement_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgImageEnhancement_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgImageEnhancement_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgImageEnhancement_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgImageEnhancement_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgImageEnhancement_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgImageEnhancement_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgImageEnhancement_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgImageEnhancement_get_Enabled(This,pVal)	\
    ( (This)->lpVtbl -> get_Enabled(This,pVal) ) 

#define IUsgImageEnhancement_put_Enabled(This,newVal)	\
    ( (This)->lpVtbl -> put_Enabled(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgImageEnhancement_INTERFACE_DEFINED__ */


#ifndef __IUsgViewArea_INTERFACE_DEFINED__
#define __IUsgViewArea_INTERFACE_DEFINED__

/* interface IUsgViewArea */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgViewArea;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0F5EAEE8-9C4E-4714-8F85-17D31CD25FC6")
    IUsgViewArea : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgViewAreaVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgViewArea * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgViewArea * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgViewArea * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgViewArea * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgViewArea * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgViewArea * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgViewArea * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgViewArea * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgViewArea * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgViewArea * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgViewAreaVtbl;

    interface IUsgViewArea
    {
        CONST_VTBL struct IUsgViewAreaVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgViewArea_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgViewArea_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgViewArea_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgViewArea_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgViewArea_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgViewArea_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgViewArea_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgViewArea_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgViewArea_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgViewArea_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgViewArea_INTERFACE_DEFINED__ */


#ifndef __IUsgLineDensity_INTERFACE_DEFINED__
#define __IUsgLineDensity_INTERFACE_DEFINED__

/* interface IUsgLineDensity */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgLineDensity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("118427F8-BAAC-4F29-B85C-DCFCD63573FE")
    IUsgLineDensity : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgLineDensityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgLineDensity * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgLineDensity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgLineDensity * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgLineDensity * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgLineDensity * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgLineDensity * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgLineDensity * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgLineDensity * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgLineDensity * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgLineDensity * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgLineDensityVtbl;

    interface IUsgLineDensity
    {
        CONST_VTBL struct IUsgLineDensityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgLineDensity_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgLineDensity_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgLineDensity_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgLineDensity_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgLineDensity_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgLineDensity_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgLineDensity_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgLineDensity_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgLineDensity_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgLineDensity_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgLineDensity_INTERFACE_DEFINED__ */


#ifndef __IUsgFocus_INTERFACE_DEFINED__
#define __IUsgFocus_INTERFACE_DEFINED__

/* interface IUsgFocus */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgFocus;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53502AB7-C0FB-4B31-A4EB-23C092D6D13A")
    IUsgFocus : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentMode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentMode( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ValuesMode( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FocusSet( 
            /* [in] */ LONG mode,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FocusSet( 
            /* [in] */ LONG mode,
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FocusSetCount( 
            /* [in] */ LONG mode,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFocalZonesCount( 
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [retval][out] */ LONG *pFocalZones) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFocalZone( 
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ LONG zoneIndex,
            /* [out] */ FOCAL_ZONE *pFocalZone) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFocusPoint( 
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ LONG zoneIndex,
            /* [retval][out] */ LONG *pFocusPoint) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFocusState( 
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ ULONG entries,
            /* [size_is][out] */ LONG *pFocusState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFocusState( 
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ ULONG entries,
            /* [size_is][in] */ LONG *pFocusState) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgFocusVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgFocus * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgFocus * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgFocus * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgFocus * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgFocus * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgFocus * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgFocus * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentMode )( 
            IUsgFocus * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CurrentMode )( 
            IUsgFocus * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ValuesMode )( 
            IUsgFocus * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FocusSet )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FocusSet )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FocusSetCount )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFocalZonesCount )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [retval][out] */ LONG *pFocalZones);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFocalZone )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ LONG zoneIndex,
            /* [out] */ FOCAL_ZONE *pFocalZone);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFocusPoint )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ LONG zoneIndex,
            /* [retval][out] */ LONG *pFocusPoint);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFocusState )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ ULONG entries,
            /* [size_is][out] */ LONG *pFocusState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFocusState )( 
            IUsgFocus * This,
            /* [in] */ LONG mode,
            /* [in] */ LONG focusSet,
            /* [in] */ ULONG entries,
            /* [size_is][in] */ LONG *pFocusState);
        
        END_INTERFACE
    } IUsgFocusVtbl;

    interface IUsgFocus
    {
        CONST_VTBL struct IUsgFocusVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgFocus_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgFocus_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgFocus_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgFocus_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgFocus_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgFocus_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgFocus_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgFocus_get_CurrentMode(This,pVal)	\
    ( (This)->lpVtbl -> get_CurrentMode(This,pVal) ) 

#define IUsgFocus_put_CurrentMode(This,newVal)	\
    ( (This)->lpVtbl -> put_CurrentMode(This,newVal) ) 

#define IUsgFocus_get_ValuesMode(This,pVal)	\
    ( (This)->lpVtbl -> get_ValuesMode(This,pVal) ) 

#define IUsgFocus_get_FocusSet(This,mode,pVal)	\
    ( (This)->lpVtbl -> get_FocusSet(This,mode,pVal) ) 

#define IUsgFocus_put_FocusSet(This,mode,newVal)	\
    ( (This)->lpVtbl -> put_FocusSet(This,mode,newVal) ) 

#define IUsgFocus_get_FocusSetCount(This,mode,pVal)	\
    ( (This)->lpVtbl -> get_FocusSetCount(This,mode,pVal) ) 

#define IUsgFocus_GetFocalZonesCount(This,mode,focusSet,pFocalZones)	\
    ( (This)->lpVtbl -> GetFocalZonesCount(This,mode,focusSet,pFocalZones) ) 

#define IUsgFocus_GetFocalZone(This,mode,focusSet,zoneIndex,pFocalZone)	\
    ( (This)->lpVtbl -> GetFocalZone(This,mode,focusSet,zoneIndex,pFocalZone) ) 

#define IUsgFocus_GetFocusPoint(This,mode,focusSet,zoneIndex,pFocusPoint)	\
    ( (This)->lpVtbl -> GetFocusPoint(This,mode,focusSet,zoneIndex,pFocusPoint) ) 

#define IUsgFocus_GetFocusState(This,mode,focusSet,entries,pFocusState)	\
    ( (This)->lpVtbl -> GetFocusState(This,mode,focusSet,entries,pFocusState) ) 

#define IUsgFocus_SetFocusState(This,mode,focusSet,entries,pFocusState)	\
    ( (This)->lpVtbl -> SetFocusState(This,mode,focusSet,entries,pFocusState) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgFocus_INTERFACE_DEFINED__ */


#ifndef __IUsgTgc_INTERFACE_DEFINED__
#define __IUsgTgc_INTERFACE_DEFINED__

/* interface IUsgTgc */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgTgc;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5AA2CBAF-30F9-4F20-A7F8-BB77A7C86D71")
    IUsgTgc : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTgcEntries( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ LONG *pTgcData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTgcEntries( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [in] */ LONG *pTgcData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTgcSize( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PositionToDepth( 
            /* [in] */ LONG pos,
            /* [retval][out] */ LONG *depth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DepthToPosition( 
            /* [in] */ LONG depth,
            /* [retval][out] */ LONG *pos) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [in] */ LONG ctlPoint,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG ctlPoint,
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CtlPointsCount( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CtlPointsCount( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CtlPointPos( 
            /* [in] */ LONG ctlPoint,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ValuesTgc( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgTgcVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgTgc * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgTgc * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgTgc * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgTgc * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgTgc * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgTgc * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgTgc * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTgcEntries )( 
            IUsgTgc * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ LONG *pTgcData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTgcEntries )( 
            IUsgTgc * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [in] */ LONG *pTgcData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTgcSize )( 
            IUsgTgc * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PositionToDepth )( 
            IUsgTgc * This,
            /* [in] */ LONG pos,
            /* [retval][out] */ LONG *depth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DepthToPosition )( 
            IUsgTgc * This,
            /* [in] */ LONG depth,
            /* [retval][out] */ LONG *pos);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgTgc * This,
            /* [in] */ LONG ctlPoint,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgTgc * This,
            /* [in] */ LONG ctlPoint,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CtlPointsCount )( 
            IUsgTgc * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CtlPointsCount )( 
            IUsgTgc * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CtlPointPos )( 
            IUsgTgc * This,
            /* [in] */ LONG ctlPoint,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ValuesTgc )( 
            IUsgTgc * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgTgcVtbl;

    interface IUsgTgc
    {
        CONST_VTBL struct IUsgTgcVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgTgc_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgTgc_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgTgc_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgTgc_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgTgc_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgTgc_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgTgc_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgTgc_GetTgcEntries(This,startIndex,entries,pTgcData)	\
    ( (This)->lpVtbl -> GetTgcEntries(This,startIndex,entries,pTgcData) ) 

#define IUsgTgc_SetTgcEntries(This,startIndex,entries,pTgcData)	\
    ( (This)->lpVtbl -> SetTgcEntries(This,startIndex,entries,pTgcData) ) 

#define IUsgTgc_GetTgcSize(This,pVal)	\
    ( (This)->lpVtbl -> GetTgcSize(This,pVal) ) 

#define IUsgTgc_PositionToDepth(This,pos,depth)	\
    ( (This)->lpVtbl -> PositionToDepth(This,pos,depth) ) 

#define IUsgTgc_DepthToPosition(This,depth,pos)	\
    ( (This)->lpVtbl -> DepthToPosition(This,depth,pos) ) 

#define IUsgTgc_get_Current(This,ctlPoint,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,ctlPoint,pVal) ) 

#define IUsgTgc_put_Current(This,ctlPoint,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,ctlPoint,newVal) ) 

#define IUsgTgc_get_CtlPointsCount(This,pVal)	\
    ( (This)->lpVtbl -> get_CtlPointsCount(This,pVal) ) 

#define IUsgTgc_put_CtlPointsCount(This,newVal)	\
    ( (This)->lpVtbl -> put_CtlPointsCount(This,newVal) ) 

#define IUsgTgc_get_CtlPointPos(This,ctlPoint,pVal)	\
    ( (This)->lpVtbl -> get_CtlPointPos(This,ctlPoint,pVal) ) 

#define IUsgTgc_get_ValuesTgc(This,pVal)	\
    ( (This)->lpVtbl -> get_ValuesTgc(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgTgc_INTERFACE_DEFINED__ */


#ifndef __IUsgClearView_INTERFACE_DEFINED__
#define __IUsgClearView_INTERFACE_DEFINED__

/* interface IUsgClearView */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgClearView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5B07F59F-E2B9-4045-9C23-BBAA62886078")
    IUsgClearView : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgClearViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgClearView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgClearView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgClearView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgClearView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgClearView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgClearView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgClearView * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgClearView * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgClearView * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgClearView * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IUsgClearView * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IUsgClearView * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IUsgClearViewVtbl;

    interface IUsgClearView
    {
        CONST_VTBL struct IUsgClearViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgClearView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgClearView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgClearView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgClearView_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgClearView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgClearView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgClearView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgClearView_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgClearView_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgClearView_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgClearView_get_Enabled(This,pVal)	\
    ( (This)->lpVtbl -> get_Enabled(This,pVal) ) 

#define IUsgClearView_put_Enabled(This,newVal)	\
    ( (This)->lpVtbl -> put_Enabled(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgClearView_INTERFACE_DEFINED__ */


#ifndef __IUsgPaletteCalculator_INTERFACE_DEFINED__
#define __IUsgPaletteCalculator_INTERFACE_DEFINED__

/* interface IUsgPaletteCalculator */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgPaletteCalculator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("537B5EA9-246B-4AF5-A199-5893ED41620E")
    IUsgPaletteCalculator : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Brightness( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Brightness( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Contrast( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Contrast( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gamma( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Gamma( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Negative( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Negative( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BrightnessMin( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BrightnessMax( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ContrastMin( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ContrastMax( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GammaMin( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GammaMax( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetReferenceData( 
            /* [in] */ LONG nValueMax,
            /* [in] */ LONG nValuesCount,
            /* [in] */ LONG *pData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReferenceData( 
            /* [out] */ LONG *pValueMax,
            /* [out][in] */ LONG *pValuesCount,
            /* [out] */ LONG *pData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Calculate( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ LONG *pData) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgPaletteCalculatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgPaletteCalculator * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgPaletteCalculator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgPaletteCalculator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgPaletteCalculator * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgPaletteCalculator * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgPaletteCalculator * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgPaletteCalculator * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Brightness )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Brightness )( 
            IUsgPaletteCalculator * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Contrast )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Contrast )( 
            IUsgPaletteCalculator * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Gamma )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Gamma )( 
            IUsgPaletteCalculator * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Negative )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Negative )( 
            IUsgPaletteCalculator * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BrightnessMin )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BrightnessMax )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ContrastMin )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ContrastMax )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GammaMin )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GammaMax )( 
            IUsgPaletteCalculator * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetReferenceData )( 
            IUsgPaletteCalculator * This,
            /* [in] */ LONG nValueMax,
            /* [in] */ LONG nValuesCount,
            /* [in] */ LONG *pData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReferenceData )( 
            IUsgPaletteCalculator * This,
            /* [out] */ LONG *pValueMax,
            /* [out][in] */ LONG *pValuesCount,
            /* [out] */ LONG *pData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Calculate )( 
            IUsgPaletteCalculator * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ LONG *pData);
        
        END_INTERFACE
    } IUsgPaletteCalculatorVtbl;

    interface IUsgPaletteCalculator
    {
        CONST_VTBL struct IUsgPaletteCalculatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgPaletteCalculator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgPaletteCalculator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgPaletteCalculator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgPaletteCalculator_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgPaletteCalculator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgPaletteCalculator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgPaletteCalculator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgPaletteCalculator_get_Brightness(This,pVal)	\
    ( (This)->lpVtbl -> get_Brightness(This,pVal) ) 

#define IUsgPaletteCalculator_put_Brightness(This,newVal)	\
    ( (This)->lpVtbl -> put_Brightness(This,newVal) ) 

#define IUsgPaletteCalculator_get_Contrast(This,pVal)	\
    ( (This)->lpVtbl -> get_Contrast(This,pVal) ) 

#define IUsgPaletteCalculator_put_Contrast(This,newVal)	\
    ( (This)->lpVtbl -> put_Contrast(This,newVal) ) 

#define IUsgPaletteCalculator_get_Gamma(This,pVal)	\
    ( (This)->lpVtbl -> get_Gamma(This,pVal) ) 

#define IUsgPaletteCalculator_put_Gamma(This,newVal)	\
    ( (This)->lpVtbl -> put_Gamma(This,newVal) ) 

#define IUsgPaletteCalculator_get_Negative(This,pVal)	\
    ( (This)->lpVtbl -> get_Negative(This,pVal) ) 

#define IUsgPaletteCalculator_put_Negative(This,newVal)	\
    ( (This)->lpVtbl -> put_Negative(This,newVal) ) 

#define IUsgPaletteCalculator_get_BrightnessMin(This,pVal)	\
    ( (This)->lpVtbl -> get_BrightnessMin(This,pVal) ) 

#define IUsgPaletteCalculator_get_BrightnessMax(This,pVal)	\
    ( (This)->lpVtbl -> get_BrightnessMax(This,pVal) ) 

#define IUsgPaletteCalculator_get_ContrastMin(This,pVal)	\
    ( (This)->lpVtbl -> get_ContrastMin(This,pVal) ) 

#define IUsgPaletteCalculator_get_ContrastMax(This,pVal)	\
    ( (This)->lpVtbl -> get_ContrastMax(This,pVal) ) 

#define IUsgPaletteCalculator_get_GammaMin(This,pVal)	\
    ( (This)->lpVtbl -> get_GammaMin(This,pVal) ) 

#define IUsgPaletteCalculator_get_GammaMax(This,pVal)	\
    ( (This)->lpVtbl -> get_GammaMax(This,pVal) ) 

#define IUsgPaletteCalculator_SetReferenceData(This,nValueMax,nValuesCount,pData)	\
    ( (This)->lpVtbl -> SetReferenceData(This,nValueMax,nValuesCount,pData) ) 

#define IUsgPaletteCalculator_GetReferenceData(This,pValueMax,pValuesCount,pData)	\
    ( (This)->lpVtbl -> GetReferenceData(This,pValueMax,pValuesCount,pData) ) 

#define IUsgPaletteCalculator_Calculate(This,startIndex,entries,pData)	\
    ( (This)->lpVtbl -> Calculate(This,startIndex,entries,pData) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgPaletteCalculator_INTERFACE_DEFINED__ */


#ifndef __IUsgPalette_INTERFACE_DEFINED__
#define __IUsgPalette_INTERFACE_DEFINED__

/* interface IUsgPalette */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgPalette;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39F0DB4B-5197-4E11-ABB6-7C8735E6B7AE")
    IUsgPalette : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPaletteEntries( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [in] */ PALETTEENTRY *pPalEntries) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPaletteEntries( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ PALETTEENTRY *pPalEntries) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgPaletteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgPalette * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgPalette * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgPalette * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgPalette * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgPalette * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgPalette * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgPalette * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPaletteEntries )( 
            IUsgPalette * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [in] */ PALETTEENTRY *pPalEntries);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPaletteEntries )( 
            IUsgPalette * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ PALETTEENTRY *pPalEntries);
        
        END_INTERFACE
    } IUsgPaletteVtbl;

    interface IUsgPalette
    {
        CONST_VTBL struct IUsgPaletteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgPalette_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgPalette_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgPalette_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgPalette_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgPalette_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgPalette_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgPalette_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgPalette_SetPaletteEntries(This,startIndex,entries,pPalEntries)	\
    ( (This)->lpVtbl -> SetPaletteEntries(This,startIndex,entries,pPalEntries) ) 

#define IUsgPalette_GetPaletteEntries(This,startIndex,entries,pPalEntries)	\
    ( (This)->lpVtbl -> GetPaletteEntries(This,startIndex,entries,pPalEntries) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgPalette_INTERFACE_DEFINED__ */


#ifndef __IUsgImageProperties_INTERFACE_DEFINED__
#define __IUsgImageProperties_INTERFACE_DEFINED__

/* interface IUsgImageProperties */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgImageProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("27C0A0A4-475B-423C-BF8B-82FC56AD7573")
    IUsgImageProperties : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOrigin( 
            /* [out] */ POINT *point,
            PIXELS_ORIGIN pixelsOrigin,
            /* [in] */ BOOL fPrefered) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOrigin( 
            /* [in] */ POINT *point,
            PIXELS_ORIGIN pixelsOrigin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResolution( 
            /* [out] */ IMAGE_RESOLUTION *resolution,
            /* [in] */ BOOL fPrefered) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetResolution( 
            /* [in] */ IMAGE_RESOLUTION *resolution) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoProbeCenter( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoProbeCenter( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoResolution( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoResolution( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PixelsToUnits( 
            /* [in] */ FLOAT Xpix,
            /* [in] */ FLOAT Ypix,
            /* [out] */ FLOAT *Xunit,
            /* [out] */ FLOAT *Yunit,
            /* [in] */ PIXELS_ORIGIN pixOrigin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnitsToPixels( 
            /* [in] */ FLOAT Xunit,
            /* [in] */ FLOAT Yunit,
            /* [out] */ FLOAT *XPix,
            /* [out] */ FLOAT *Ypix,
            /* [in] */ PIXELS_ORIGIN pixOrigin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetViewRect( 
            /* [in] */ FLOAT left,
            /* [in] */ FLOAT top,
            /* [in] */ FLOAT right,
            /* [in] */ FLOAT bottom) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetViewRect( 
            /* [out] */ FLOAT *left,
            /* [out] */ FLOAT *top,
            /* [out] */ FLOAT *right,
            /* [out] */ FLOAT *bottom,
            /* [in] */ BOOL fPrefered) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetZoom( 
            /* [in] */ FLOAT ZoomCenterX,
            /* [in] */ FLOAT ZoomCenterY,
            /* [in] */ LONG ZoomFactor) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetZoom( 
            /* [out] */ FLOAT *ZoomCenterX,
            /* [out] */ FLOAT *ZooomCenterY,
            /* [out] */ LONG *ZoomFactor,
            /* [in] */ BOOL fPrefered) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgImagePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgImageProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgImageProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgImageProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgImageProperties * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgImageProperties * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgImageProperties * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgImageProperties * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOrigin )( 
            IUsgImageProperties * This,
            /* [out] */ POINT *point,
            PIXELS_ORIGIN pixelsOrigin,
            /* [in] */ BOOL fPrefered);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOrigin )( 
            IUsgImageProperties * This,
            /* [in] */ POINT *point,
            PIXELS_ORIGIN pixelsOrigin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            IUsgImageProperties * This,
            /* [out] */ IMAGE_RESOLUTION *resolution,
            /* [in] */ BOOL fPrefered);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetResolution )( 
            IUsgImageProperties * This,
            /* [in] */ IMAGE_RESOLUTION *resolution);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoProbeCenter )( 
            IUsgImageProperties * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoProbeCenter )( 
            IUsgImageProperties * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoResolution )( 
            IUsgImageProperties * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoResolution )( 
            IUsgImageProperties * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PixelsToUnits )( 
            IUsgImageProperties * This,
            /* [in] */ FLOAT Xpix,
            /* [in] */ FLOAT Ypix,
            /* [out] */ FLOAT *Xunit,
            /* [out] */ FLOAT *Yunit,
            /* [in] */ PIXELS_ORIGIN pixOrigin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnitsToPixels )( 
            IUsgImageProperties * This,
            /* [in] */ FLOAT Xunit,
            /* [in] */ FLOAT Yunit,
            /* [out] */ FLOAT *XPix,
            /* [out] */ FLOAT *Ypix,
            /* [in] */ PIXELS_ORIGIN pixOrigin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetViewRect )( 
            IUsgImageProperties * This,
            /* [in] */ FLOAT left,
            /* [in] */ FLOAT top,
            /* [in] */ FLOAT right,
            /* [in] */ FLOAT bottom);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetViewRect )( 
            IUsgImageProperties * This,
            /* [out] */ FLOAT *left,
            /* [out] */ FLOAT *top,
            /* [out] */ FLOAT *right,
            /* [out] */ FLOAT *bottom,
            /* [in] */ BOOL fPrefered);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetZoom )( 
            IUsgImageProperties * This,
            /* [in] */ FLOAT ZoomCenterX,
            /* [in] */ FLOAT ZoomCenterY,
            /* [in] */ LONG ZoomFactor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetZoom )( 
            IUsgImageProperties * This,
            /* [out] */ FLOAT *ZoomCenterX,
            /* [out] */ FLOAT *ZooomCenterY,
            /* [out] */ LONG *ZoomFactor,
            /* [in] */ BOOL fPrefered);
        
        END_INTERFACE
    } IUsgImagePropertiesVtbl;

    interface IUsgImageProperties
    {
        CONST_VTBL struct IUsgImagePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgImageProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgImageProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgImageProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgImageProperties_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgImageProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgImageProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgImageProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgImageProperties_GetOrigin(This,point,pixelsOrigin,fPrefered)	\
    ( (This)->lpVtbl -> GetOrigin(This,point,pixelsOrigin,fPrefered) ) 

#define IUsgImageProperties_SetOrigin(This,point,pixelsOrigin)	\
    ( (This)->lpVtbl -> SetOrigin(This,point,pixelsOrigin) ) 

#define IUsgImageProperties_GetResolution(This,resolution,fPrefered)	\
    ( (This)->lpVtbl -> GetResolution(This,resolution,fPrefered) ) 

#define IUsgImageProperties_SetResolution(This,resolution)	\
    ( (This)->lpVtbl -> SetResolution(This,resolution) ) 

#define IUsgImageProperties_get_AutoProbeCenter(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoProbeCenter(This,pVal) ) 

#define IUsgImageProperties_put_AutoProbeCenter(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoProbeCenter(This,newVal) ) 

#define IUsgImageProperties_get_AutoResolution(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoResolution(This,pVal) ) 

#define IUsgImageProperties_put_AutoResolution(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoResolution(This,newVal) ) 

#define IUsgImageProperties_PixelsToUnits(This,Xpix,Ypix,Xunit,Yunit,pixOrigin)	\
    ( (This)->lpVtbl -> PixelsToUnits(This,Xpix,Ypix,Xunit,Yunit,pixOrigin) ) 

#define IUsgImageProperties_UnitsToPixels(This,Xunit,Yunit,XPix,Ypix,pixOrigin)	\
    ( (This)->lpVtbl -> UnitsToPixels(This,Xunit,Yunit,XPix,Ypix,pixOrigin) ) 

#define IUsgImageProperties_SetViewRect(This,left,top,right,bottom)	\
    ( (This)->lpVtbl -> SetViewRect(This,left,top,right,bottom) ) 

#define IUsgImageProperties_GetViewRect(This,left,top,right,bottom,fPrefered)	\
    ( (This)->lpVtbl -> GetViewRect(This,left,top,right,bottom,fPrefered) ) 

#define IUsgImageProperties_SetZoom(This,ZoomCenterX,ZoomCenterY,ZoomFactor)	\
    ( (This)->lpVtbl -> SetZoom(This,ZoomCenterX,ZoomCenterY,ZoomFactor) ) 

#define IUsgImageProperties_GetZoom(This,ZoomCenterX,ZooomCenterY,ZoomFactor,fPrefered)	\
    ( (This)->lpVtbl -> GetZoom(This,ZoomCenterX,ZooomCenterY,ZoomFactor,fPrefered) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgImageProperties_INTERFACE_DEFINED__ */


#ifndef __IUsgControlChangeSink_INTERFACE_DEFINED__
#define __IUsgControlChangeSink_INTERFACE_DEFINED__

/* interface IUsgControlChangeSink */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgControlChangeSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9E38438A-733B-4784-8C68-60241ED49859")
    IUsgControlChangeSink : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ControlChanged( 
            /* [in] */ LONG dispId,
            /* [in] */ LONG flags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgControlChangeSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgControlChangeSink * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgControlChangeSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgControlChangeSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgControlChangeSink * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgControlChangeSink * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgControlChangeSink * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgControlChangeSink * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ControlChanged )( 
            IUsgControlChangeSink * This,
            /* [in] */ LONG dispId,
            /* [in] */ LONG flags);
        
        END_INTERFACE
    } IUsgControlChangeSinkVtbl;

    interface IUsgControlChangeSink
    {
        CONST_VTBL struct IUsgControlChangeSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgControlChangeSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgControlChangeSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgControlChangeSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgControlChangeSink_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgControlChangeSink_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgControlChangeSink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgControlChangeSink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgControlChangeSink_ControlChanged(This,dispId,flags)	\
    ( (This)->lpVtbl -> ControlChanged(This,dispId,flags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgControlChangeSink_INTERFACE_DEFINED__ */


#ifndef __IUsgCtrlChangeCommon_INTERFACE_DEFINED__
#define __IUsgCtrlChangeCommon_INTERFACE_DEFINED__

/* interface IUsgCtrlChangeCommon */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCtrlChangeCommon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA64E76-249A-4606-8EEF-E4FD802AE9EF")
    IUsgCtrlChangeCommon : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnControlChanged( 
            /* [in] */ REFIID riidCtrl,
            /* [in] */ ULONG scanMode,
            /* [in] */ LONG streamId,
            /* [in] */ IUsgControl *pControlObj,
            /* [in] */ LONG dispId,
            /* [in] */ LONG flags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnControlChangedBSTR( 
            /* [in] */ BSTR ctrlGUID,
            /* [in] */ LONG scanMode,
            /* [in] */ LONG streamId,
            /* [in] */ IUsgControl *pControlObject,
            /* [in] */ LONG dispId,
            /* [in] */ LONG flags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCtrlChangeCommonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCtrlChangeCommon * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCtrlChangeCommon * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCtrlChangeCommon * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCtrlChangeCommon * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCtrlChangeCommon * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCtrlChangeCommon * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCtrlChangeCommon * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnControlChanged )( 
            IUsgCtrlChangeCommon * This,
            /* [in] */ REFIID riidCtrl,
            /* [in] */ ULONG scanMode,
            /* [in] */ LONG streamId,
            /* [in] */ IUsgControl *pControlObj,
            /* [in] */ LONG dispId,
            /* [in] */ LONG flags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnControlChangedBSTR )( 
            IUsgCtrlChangeCommon * This,
            /* [in] */ BSTR ctrlGUID,
            /* [in] */ LONG scanMode,
            /* [in] */ LONG streamId,
            /* [in] */ IUsgControl *pControlObject,
            /* [in] */ LONG dispId,
            /* [in] */ LONG flags);
        
        END_INTERFACE
    } IUsgCtrlChangeCommonVtbl;

    interface IUsgCtrlChangeCommon
    {
        CONST_VTBL struct IUsgCtrlChangeCommonVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCtrlChangeCommon_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCtrlChangeCommon_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCtrlChangeCommon_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCtrlChangeCommon_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCtrlChangeCommon_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCtrlChangeCommon_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCtrlChangeCommon_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCtrlChangeCommon_OnControlChanged(This,riidCtrl,scanMode,streamId,pControlObj,dispId,flags)	\
    ( (This)->lpVtbl -> OnControlChanged(This,riidCtrl,scanMode,streamId,pControlObj,dispId,flags) ) 

#define IUsgCtrlChangeCommon_OnControlChangedBSTR(This,ctrlGUID,scanMode,streamId,pControlObject,dispId,flags)	\
    ( (This)->lpVtbl -> OnControlChangedBSTR(This,ctrlGUID,scanMode,streamId,pControlObject,dispId,flags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCtrlChangeCommon_INTERFACE_DEFINED__ */


#ifndef __IUsgScanLine_INTERFACE_DEFINED__
#define __IUsgScanLine_INTERFACE_DEFINED__

/* interface IUsgScanLine */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanLine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AC0CEFF6-21E9-472F-B1A3-FAF18557A037")
    IUsgScanLine : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanLine( 
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y,
            /* [retval][out] */ LONG *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates( 
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *x,
            /* [out] */ FLOAT *y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates2( 
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth = 0,
            /* [defaultvalue][in] */ FLOAT endDepth = 0) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanLineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanLine * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanLine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanLine * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanLine * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanLine * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanLine * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanLine * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgScanLine * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgScanLine * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgScanLine * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLine )( 
            IUsgScanLine * This,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y,
            /* [retval][out] */ LONG *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates )( 
            IUsgScanLine * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *x,
            /* [out] */ FLOAT *y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates2 )( 
            IUsgScanLine * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth,
            /* [defaultvalue][in] */ FLOAT endDepth);
        
        END_INTERFACE
    } IUsgScanLineVtbl;

    interface IUsgScanLine
    {
        CONST_VTBL struct IUsgScanLineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanLine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanLine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanLine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanLine_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanLine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanLine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanLine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanLine_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgScanLine_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgScanLine_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgScanLine_GetScanLine(This,x,y,value)	\
    ( (This)->lpVtbl -> GetScanLine(This,x,y,value) ) 

#define IUsgScanLine_GetCoordinates(This,scanLine,angle,x,y)	\
    ( (This)->lpVtbl -> GetCoordinates(This,scanLine,angle,x,y) ) 

#define IUsgScanLine_GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanLine_INTERFACE_DEFINED__ */


#ifndef __IUsgScanLine2_INTERFACE_DEFINED__
#define __IUsgScanLine2_INTERFACE_DEFINED__

/* interface IUsgScanLine2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanLine2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B521CD25-EC30-486b-B1B7-BE229735AABD")
    IUsgScanLine2 : public IUsgScanLine
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanLine2( 
            /* [in] */ LONG subFrame,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates3( 
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates4( 
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth = 0,
            /* [defaultvalue][in] */ FLOAT endDepth = 0) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanLine2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanLine2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanLine2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanLine2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanLine2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanLine2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanLine2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanLine2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgScanLine2 * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgScanLine2 * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgScanLine2 * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLine )( 
            IUsgScanLine2 * This,
            /* [in] */ FLOAT x,
            /* [in] */ FLOAT y,
            /* [retval][out] */ LONG *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates )( 
            IUsgScanLine2 * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *x,
            /* [out] */ FLOAT *y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates2 )( 
            IUsgScanLine2 * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth,
            /* [defaultvalue][in] */ FLOAT endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLine2 )( 
            IUsgScanLine2 * This,
            /* [in] */ LONG subFrame,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates3 )( 
            IUsgScanLine2 * This,
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates4 )( 
            IUsgScanLine2 * This,
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth,
            /* [defaultvalue][in] */ FLOAT endDepth);
        
        END_INTERFACE
    } IUsgScanLine2Vtbl;

    interface IUsgScanLine2
    {
        CONST_VTBL struct IUsgScanLine2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanLine2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanLine2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanLine2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanLine2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanLine2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanLine2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanLine2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanLine2_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgScanLine2_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgScanLine2_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgScanLine2_GetScanLine(This,x,y,value)	\
    ( (This)->lpVtbl -> GetScanLine(This,x,y,value) ) 

#define IUsgScanLine2_GetCoordinates(This,scanLine,angle,x,y)	\
    ( (This)->lpVtbl -> GetCoordinates(This,scanLine,angle,x,y) ) 

#define IUsgScanLine2_GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth) ) 


#define IUsgScanLine2_GetScanLine2(This,subFrame,X,Y,value)	\
    ( (This)->lpVtbl -> GetScanLine2(This,subFrame,X,Y,value) ) 

#define IUsgScanLine2_GetCoordinates3(This,scanLine,subFrame,angle,X,Y)	\
    ( (This)->lpVtbl -> GetCoordinates3(This,scanLine,subFrame,angle,X,Y) ) 

#define IUsgScanLine2_GetCoordinates4(This,scanLine,subFrame,X1,Y1,X2,Y2,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetCoordinates4(This,scanLine,subFrame,X1,Y1,X2,Y2,startDepth,endDepth) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanLine2_INTERFACE_DEFINED__ */


#ifndef __IUsgScanLineProperties_INTERFACE_DEFINED__
#define __IUsgScanLineProperties_INTERFACE_DEFINED__

/* interface IUsgScanLineProperties */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanLineProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF1D6EDE-1AB3-429B-8348-6BEA1A43500E")
    IUsgScanLineProperties : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates( 
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates2( 
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth = 0,
            /* [defaultvalue][in] */ FLOAT endDepth = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanLine( 
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanLinesRange( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPointDepth( 
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ FLOAT *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanLinePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanLineProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanLineProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanLineProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanLineProperties * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanLineProperties * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanLineProperties * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanLineProperties * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates )( 
            IUsgScanLineProperties * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates2 )( 
            IUsgScanLineProperties * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth,
            /* [defaultvalue][in] */ FLOAT endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLine )( 
            IUsgScanLineProperties * This,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLinesRange )( 
            IUsgScanLineProperties * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPointDepth )( 
            IUsgScanLineProperties * This,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ FLOAT *value);
        
        END_INTERFACE
    } IUsgScanLinePropertiesVtbl;

    interface IUsgScanLineProperties
    {
        CONST_VTBL struct IUsgScanLinePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanLineProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanLineProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanLineProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanLineProperties_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanLineProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanLineProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanLineProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanLineProperties_GetCoordinates(This,scanLine,angle,X,Y)	\
    ( (This)->lpVtbl -> GetCoordinates(This,scanLine,angle,X,Y) ) 

#define IUsgScanLineProperties_GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth) ) 

#define IUsgScanLineProperties_GetScanLine(This,X,Y,value)	\
    ( (This)->lpVtbl -> GetScanLine(This,X,Y,value) ) 

#define IUsgScanLineProperties_GetScanLinesRange(This,startLine,endLine)	\
    ( (This)->lpVtbl -> GetScanLinesRange(This,startLine,endLine) ) 

#define IUsgScanLineProperties_GetPointDepth(This,X,Y,value)	\
    ( (This)->lpVtbl -> GetPointDepth(This,X,Y,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanLineProperties_INTERFACE_DEFINED__ */


#ifndef __IUsgScanLineProperties2_INTERFACE_DEFINED__
#define __IUsgScanLineProperties2_INTERFACE_DEFINED__

/* interface IUsgScanLineProperties2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanLineProperties2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A4F7329D-51A7-4a61-A9A8-CFE90A90C904")
    IUsgScanLineProperties2 : public IUsgScanLineProperties
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates3( 
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCoordinates4( 
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth = 0,
            /* [defaultvalue][in] */ FLOAT endDepth = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScanLine2( 
            /* [in] */ LONG subFrame,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPointDepth2( 
            /* [in] */ LONG subFrame,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ FLOAT *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanLineProperties2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanLineProperties2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanLineProperties2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanLineProperties2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanLineProperties2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates2 )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ LONG scanLine,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth,
            /* [defaultvalue][in] */ FLOAT endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLine )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLinesRange )( 
            IUsgScanLineProperties2 * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPointDepth )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ FLOAT *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates3 )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *angle,
            /* [out] */ FLOAT *X,
            /* [out] */ FLOAT *Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCoordinates4 )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ LONG scanLine,
            /* [in] */ LONG subFrame,
            /* [out] */ FLOAT *X1,
            /* [out] */ FLOAT *Y1,
            /* [out] */ FLOAT *X2,
            /* [out] */ FLOAT *Y2,
            /* [defaultvalue][in] */ FLOAT startDepth,
            /* [defaultvalue][in] */ FLOAT endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScanLine2 )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ LONG subFrame,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ LONG *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPointDepth2 )( 
            IUsgScanLineProperties2 * This,
            /* [in] */ LONG subFrame,
            /* [in] */ FLOAT X,
            /* [in] */ FLOAT Y,
            /* [retval][out] */ FLOAT *value);
        
        END_INTERFACE
    } IUsgScanLineProperties2Vtbl;

    interface IUsgScanLineProperties2
    {
        CONST_VTBL struct IUsgScanLineProperties2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanLineProperties2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanLineProperties2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanLineProperties2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanLineProperties2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanLineProperties2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanLineProperties2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanLineProperties2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanLineProperties2_GetCoordinates(This,scanLine,angle,X,Y)	\
    ( (This)->lpVtbl -> GetCoordinates(This,scanLine,angle,X,Y) ) 

#define IUsgScanLineProperties2_GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetCoordinates2(This,scanLine,X1,Y1,X2,Y2,startDepth,endDepth) ) 

#define IUsgScanLineProperties2_GetScanLine(This,X,Y,value)	\
    ( (This)->lpVtbl -> GetScanLine(This,X,Y,value) ) 

#define IUsgScanLineProperties2_GetScanLinesRange(This,startLine,endLine)	\
    ( (This)->lpVtbl -> GetScanLinesRange(This,startLine,endLine) ) 

#define IUsgScanLineProperties2_GetPointDepth(This,X,Y,value)	\
    ( (This)->lpVtbl -> GetPointDepth(This,X,Y,value) ) 


#define IUsgScanLineProperties2_GetCoordinates3(This,scanLine,subFrame,angle,X,Y)	\
    ( (This)->lpVtbl -> GetCoordinates3(This,scanLine,subFrame,angle,X,Y) ) 

#define IUsgScanLineProperties2_GetCoordinates4(This,scanLine,subFrame,X1,Y1,X2,Y2,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetCoordinates4(This,scanLine,subFrame,X1,Y1,X2,Y2,startDepth,endDepth) ) 

#define IUsgScanLineProperties2_GetScanLine2(This,subFrame,X,Y,value)	\
    ( (This)->lpVtbl -> GetScanLine2(This,subFrame,X,Y,value) ) 

#define IUsgScanLineProperties2_GetPointDepth2(This,subFrame,X,Y,value)	\
    ( (This)->lpVtbl -> GetPointDepth2(This,subFrame,X,Y,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanLineProperties2_INTERFACE_DEFINED__ */


#ifndef __IUsgScanLineSelector_INTERFACE_DEFINED__
#define __IUsgScanLineSelector_INTERFACE_DEFINED__

/* interface IUsgScanLineSelector */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanLineSelector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3BFE461D-4240-40AC-B5FF-292A6C253A4C")
    IUsgScanLineSelector : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanLineSelectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanLineSelector * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanLineSelector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanLineSelector * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanLineSelector * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanLineSelector * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanLineSelector * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanLineSelector * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgScanLineSelector * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgScanLineSelector * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgScanLineSelector * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgScanLineSelectorVtbl;

    interface IUsgScanLineSelector
    {
        CONST_VTBL struct IUsgScanLineSelectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanLineSelector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanLineSelector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanLineSelector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanLineSelector_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanLineSelector_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanLineSelector_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanLineSelector_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanLineSelector_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgScanLineSelector_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgScanLineSelector_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanLineSelector_INTERFACE_DEFINED__ */


#ifndef __IUsgSweepMode_INTERFACE_DEFINED__
#define __IUsgSweepMode_INTERFACE_DEFINED__

/* interface IUsgSweepMode */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgSweepMode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4DD0E32D-23BF-4591-B76D-2C971BB89507")
    IUsgSweepMode : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ SWEEP_MODE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ SWEEP_MODE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgSweepModeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgSweepMode * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgSweepMode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgSweepMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgSweepMode * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgSweepMode * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgSweepMode * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgSweepMode * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgSweepMode * This,
            /* [retval][out] */ SWEEP_MODE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgSweepMode * This,
            /* [in] */ SWEEP_MODE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgSweepMode * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgSweepModeVtbl;

    interface IUsgSweepMode
    {
        CONST_VTBL struct IUsgSweepModeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgSweepMode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgSweepMode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgSweepMode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgSweepMode_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgSweepMode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgSweepMode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgSweepMode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgSweepMode_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgSweepMode_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgSweepMode_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgSweepMode_INTERFACE_DEFINED__ */


#ifndef __IUsgQualProp_INTERFACE_DEFINED__
#define __IUsgQualProp_INTERFACE_DEFINED__

/* interface IUsgQualProp */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgQualProp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0DF080D7-1180-4c94-9003-168174CD3ACD")
    IUsgQualProp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AvgFrameRate( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FramesDrawn( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FramesDroppedInRenderer( 
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgQualPropVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgQualProp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgQualProp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgQualProp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgQualProp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgQualProp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgQualProp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgQualProp * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AvgFrameRate )( 
            IUsgQualProp * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FramesDrawn )( 
            IUsgQualProp * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FramesDroppedInRenderer )( 
            IUsgQualProp * This,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } IUsgQualPropVtbl;

    interface IUsgQualProp
    {
        CONST_VTBL struct IUsgQualPropVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgQualProp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgQualProp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgQualProp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgQualProp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgQualProp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgQualProp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgQualProp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgQualProp_get_AvgFrameRate(This,pVal)	\
    ( (This)->lpVtbl -> get_AvgFrameRate(This,pVal) ) 

#define IUsgQualProp_get_FramesDrawn(This,pVal)	\
    ( (This)->lpVtbl -> get_FramesDrawn(This,pVal) ) 

#define IUsgQualProp_get_FramesDroppedInRenderer(This,pVal)	\
    ( (This)->lpVtbl -> get_FramesDroppedInRenderer(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgQualProp_INTERFACE_DEFINED__ */


#ifndef __IUsgSweepSpeed_INTERFACE_DEFINED__
#define __IUsgSweepSpeed_INTERFACE_DEFINED__

/* interface IUsgSweepSpeed */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgSweepSpeed;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("481482A0-B0E1-460E-92B9-3159ED9E2748")
    IUsgSweepSpeed : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SweepSpeed( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SweepSpeed( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Preset( 
            /* [in] */ LONG cEntries,
            /* [size_is][in] */ LONG *pValues,
            /* [in] */ LONG iCurrent,
            /* [in] */ BOOL SweepTime) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgSweepSpeedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgSweepSpeed * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgSweepSpeed * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgSweepSpeed * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgSweepSpeed * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgSweepSpeed * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgSweepSpeed * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgSweepSpeed * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgSweepSpeed * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgSweepSpeed * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgSweepSpeed * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SweepSpeed )( 
            IUsgSweepSpeed * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SweepSpeed )( 
            IUsgSweepSpeed * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Preset )( 
            IUsgSweepSpeed * This,
            /* [in] */ LONG cEntries,
            /* [size_is][in] */ LONG *pValues,
            /* [in] */ LONG iCurrent,
            /* [in] */ BOOL SweepTime);
        
        END_INTERFACE
    } IUsgSweepSpeedVtbl;

    interface IUsgSweepSpeed
    {
        CONST_VTBL struct IUsgSweepSpeedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgSweepSpeed_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgSweepSpeed_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgSweepSpeed_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgSweepSpeed_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgSweepSpeed_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgSweepSpeed_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgSweepSpeed_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgSweepSpeed_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgSweepSpeed_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgSweepSpeed_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgSweepSpeed_get_SweepSpeed(This,pVal)	\
    ( (This)->lpVtbl -> get_SweepSpeed(This,pVal) ) 

#define IUsgSweepSpeed_put_SweepSpeed(This,newVal)	\
    ( (This)->lpVtbl -> put_SweepSpeed(This,newVal) ) 

#define IUsgSweepSpeed_Preset(This,cEntries,pValues,iCurrent,SweepTime)	\
    ( (This)->lpVtbl -> Preset(This,cEntries,pValues,iCurrent,SweepTime) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgSweepSpeed_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerColorMap_INTERFACE_DEFINED__
#define __IUsgDopplerColorMap_INTERFACE_DEFINED__

/* interface IUsgDopplerColorMap */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerColorMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39B7413A-07E4-492D-8A38-5A5E788E30D1")
    IUsgDopplerColorMap : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetColorMap( 
            /* [in] */ PALETTEENTRY *pTable1,
            /* [in] */ LONG size1,
            /* [in] */ PALETTEENTRY *pTable2,
            /* [in] */ LONG size2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPaletteEntries( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ PALETTEENTRY *pPalEntries) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerColorMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerColorMap * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerColorMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerColorMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerColorMap * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerColorMap * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerColorMap * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerColorMap * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetColorMap )( 
            IUsgDopplerColorMap * This,
            /* [in] */ PALETTEENTRY *pTable1,
            /* [in] */ LONG size1,
            /* [in] */ PALETTEENTRY *pTable2,
            /* [in] */ LONG size2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPaletteEntries )( 
            IUsgDopplerColorMap * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ PALETTEENTRY *pPalEntries);
        
        END_INTERFACE
    } IUsgDopplerColorMapVtbl;

    interface IUsgDopplerColorMap
    {
        CONST_VTBL struct IUsgDopplerColorMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerColorMap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerColorMap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerColorMap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerColorMap_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerColorMap_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerColorMap_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerColorMap_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerColorMap_SetColorMap(This,pTable1,size1,pTable2,size2)	\
    ( (This)->lpVtbl -> SetColorMap(This,pTable1,size1,pTable2,size2) ) 

#define IUsgDopplerColorMap_GetPaletteEntries(This,startIndex,entries,pPalEntries)	\
    ( (This)->lpVtbl -> GetPaletteEntries(This,startIndex,entries,pPalEntries) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerColorMap_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerColorPriority_INTERFACE_DEFINED__
#define __IUsgDopplerColorPriority_INTERFACE_DEFINED__

/* interface IUsgDopplerColorPriority */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerColorPriority;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5134078D-9B3D-4DB4-B7F6-BAF5C60180E0")
    IUsgDopplerColorPriority : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PriorityMode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PriorityMode( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerColorPriorityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerColorPriority * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerColorPriority * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerColorPriority * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerColorPriority * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerColorPriority * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerColorPriority * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerColorPriority * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerColorPriority * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerColorPriority * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerColorPriority * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PriorityMode )( 
            IUsgDopplerColorPriority * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PriorityMode )( 
            IUsgDopplerColorPriority * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IUsgDopplerColorPriorityVtbl;

    interface IUsgDopplerColorPriority
    {
        CONST_VTBL struct IUsgDopplerColorPriorityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerColorPriority_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerColorPriority_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerColorPriority_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerColorPriority_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerColorPriority_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerColorPriority_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerColorPriority_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerColorPriority_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerColorPriority_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerColorPriority_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgDopplerColorPriority_get_PriorityMode(This,pVal)	\
    ( (This)->lpVtbl -> get_PriorityMode(This,pVal) ) 

#define IUsgDopplerColorPriority_put_PriorityMode(This,newVal)	\
    ( (This)->lpVtbl -> put_PriorityMode(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerColorPriority_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerSteerAngle_INTERFACE_DEFINED__
#define __IUsgDopplerSteerAngle_INTERFACE_DEFINED__

/* interface IUsgDopplerSteerAngle */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerSteerAngle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3373936B-1232-4E94-9BA9-4565D736635D")
    IUsgDopplerSteerAngle : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerSteerAngleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerSteerAngle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerSteerAngle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerSteerAngle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerSteerAngle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerSteerAngle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerSteerAngle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerSteerAngle * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerSteerAngle * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerSteerAngle * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerSteerAngle * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerSteerAngleVtbl;

    interface IUsgDopplerSteerAngle
    {
        CONST_VTBL struct IUsgDopplerSteerAngleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerSteerAngle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerSteerAngle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerSteerAngle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerSteerAngle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerSteerAngle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerSteerAngle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerSteerAngle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerSteerAngle_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerSteerAngle_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerSteerAngle_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerSteerAngle_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerColorThreshold_INTERFACE_DEFINED__
#define __IUsgDopplerColorThreshold_INTERFACE_DEFINED__

/* interface IUsgDopplerColorThreshold */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerColorThreshold;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CD490C38-98B9-487A-9B91-653C806FFD21")
    IUsgDopplerColorThreshold : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerColorThresholdVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerColorThreshold * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerColorThreshold * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerColorThreshold * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerColorThreshold * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerColorThreshold * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerColorThreshold * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerColorThreshold * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerColorThreshold * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerColorThreshold * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerColorThreshold * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerColorThresholdVtbl;

    interface IUsgDopplerColorThreshold
    {
        CONST_VTBL struct IUsgDopplerColorThresholdVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerColorThreshold_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerColorThreshold_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerColorThreshold_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerColorThreshold_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerColorThreshold_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerColorThreshold_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerColorThreshold_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerColorThreshold_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerColorThreshold_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerColorThreshold_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerColorThreshold_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerBaseLine_INTERFACE_DEFINED__
#define __IUsgDopplerBaseLine_INTERFACE_DEFINED__

/* interface IUsgDopplerBaseLine */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerBaseLine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F78B3D8F-F0D9-4129-A0C1-B9977577BEA2")
    IUsgDopplerBaseLine : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerBaseLineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerBaseLine * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerBaseLine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerBaseLine * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerBaseLine * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerBaseLine * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerBaseLine * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerBaseLine * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerBaseLine * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerBaseLine * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerBaseLine * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerBaseLineVtbl;

    interface IUsgDopplerBaseLine
    {
        CONST_VTBL struct IUsgDopplerBaseLineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerBaseLine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerBaseLine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerBaseLine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerBaseLine_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerBaseLine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerBaseLine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerBaseLine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerBaseLine_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerBaseLine_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerBaseLine_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerBaseLine_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerPRF_INTERFACE_DEFINED__
#define __IUsgDopplerPRF_INTERFACE_DEFINED__

/* interface IUsgDopplerPRF */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerPRF;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D37B8F18-417C-406B-8E5D-BAEC11623428")
    IUsgDopplerPRF : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VisibleMin( 
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VisibleMax( 
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerPRFVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerPRF * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerPRF * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerPRF * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerPRF * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerPRF * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerPRF * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerPRF * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerPRF * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerPRF * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerPRF * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VisibleMin )( 
            IUsgDopplerPRF * This,
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VisibleMax )( 
            IUsgDopplerPRF * This,
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } IUsgDopplerPRFVtbl;

    interface IUsgDopplerPRF
    {
        CONST_VTBL struct IUsgDopplerPRFVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerPRF_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerPRF_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerPRF_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerPRF_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerPRF_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerPRF_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerPRF_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerPRF_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerPRF_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerPRF_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgDopplerPRF_get_VisibleMin(This,units,pVal)	\
    ( (This)->lpVtbl -> get_VisibleMin(This,units,pVal) ) 

#define IUsgDopplerPRF_get_VisibleMax(This,units,pVal)	\
    ( (This)->lpVtbl -> get_VisibleMax(This,units,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerPRF_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerWindow_INTERFACE_DEFINED__
#define __IUsgDopplerWindow_INTERFACE_DEFINED__

/* interface IUsgDopplerWindow */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7D485326-1EAD-43C7-BC9A-C57CF251D4D3")
    IUsgDopplerWindow : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWindow( 
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVolume( 
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVolume( 
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LinesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WidthRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DepthsRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VolumesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerWindow * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerWindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerWindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerWindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerWindow * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWindow )( 
            IUsgDopplerWindow * This,
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IUsgDopplerWindow * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IUsgDopplerWindow * This,
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVolume )( 
            IUsgDopplerWindow * This,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IUsgDopplerWindow * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVolume )( 
            IUsgDopplerWindow * This,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LinesRange )( 
            IUsgDopplerWindow * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WidthRange )( 
            IUsgDopplerWindow * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DepthsRange )( 
            IUsgDopplerWindow * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VolumesRange )( 
            IUsgDopplerWindow * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        END_INTERFACE
    } IUsgDopplerWindowVtbl;

    interface IUsgDopplerWindow
    {
        CONST_VTBL struct IUsgDopplerWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerWindow_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerWindow_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerWindow_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerWindow_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerWindow_SetWindow(This,startLine,endLine,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetWindow(This,startLine,endLine,startDepth,endDepth) ) 

#define IUsgDopplerWindow_GetWindow(This,startLine,endLine,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetWindow(This,startLine,endLine,startDepth,endDepth) ) 

#define IUsgDopplerWindow_SetWidth(This,startLine,endLine)	\
    ( (This)->lpVtbl -> SetWidth(This,startLine,endLine) ) 

#define IUsgDopplerWindow_SetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetVolume(This,startDepth,endDepth) ) 

#define IUsgDopplerWindow_GetWidth(This,startLine,endLine)	\
    ( (This)->lpVtbl -> GetWidth(This,startLine,endLine) ) 

#define IUsgDopplerWindow_GetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetVolume(This,startDepth,endDepth) ) 

#define IUsgDopplerWindow_get_LinesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_LinesRange(This,pVal) ) 

#define IUsgDopplerWindow_get_WidthRange(This,pVal)	\
    ( (This)->lpVtbl -> get_WidthRange(This,pVal) ) 

#define IUsgDopplerWindow_get_DepthsRange(This,pVal)	\
    ( (This)->lpVtbl -> get_DepthsRange(This,pVal) ) 

#define IUsgDopplerWindow_get_VolumesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_VolumesRange(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerWindow_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerWallFilter_INTERFACE_DEFINED__
#define __IUsgDopplerWallFilter_INTERFACE_DEFINED__

/* interface IUsgDopplerWallFilter */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerWallFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6A62BE4A-23C3-4262-B1C6-C20933615E90")
    IUsgDopplerWallFilter : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VisibleMin( 
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VisibleMax( 
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerWallFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerWallFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerWallFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerWallFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerWallFilter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerWallFilter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerWallFilter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerWallFilter * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerWallFilter * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerWallFilter * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerWallFilter * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VisibleMin )( 
            IUsgDopplerWallFilter * This,
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VisibleMax )( 
            IUsgDopplerWallFilter * This,
            /* [in] */ LONG units,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } IUsgDopplerWallFilterVtbl;

    interface IUsgDopplerWallFilter
    {
        CONST_VTBL struct IUsgDopplerWallFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerWallFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerWallFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerWallFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerWallFilter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerWallFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerWallFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerWallFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerWallFilter_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerWallFilter_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerWallFilter_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgDopplerWallFilter_get_VisibleMin(This,units,pVal)	\
    ( (This)->lpVtbl -> get_VisibleMin(This,units,pVal) ) 

#define IUsgDopplerWallFilter_get_VisibleMax(This,units,pVal)	\
    ( (This)->lpVtbl -> get_VisibleMax(This,units,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerWallFilter_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerSignalScale_INTERFACE_DEFINED__
#define __IUsgDopplerSignalScale_INTERFACE_DEFINED__

/* interface IUsgDopplerSignalScale */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerSignalScale;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("507BA161-F30F-4B86-9DB2-107B89841A0B")
    IUsgDopplerSignalScale : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerSignalScaleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerSignalScale * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerSignalScale * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerSignalScale * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerSignalScale * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerSignalScale * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerSignalScale * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerSignalScale * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerSignalScale * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerSignalScale * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerSignalScale * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerSignalScaleVtbl;

    interface IUsgDopplerSignalScale
    {
        CONST_VTBL struct IUsgDopplerSignalScaleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerSignalScale_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerSignalScale_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerSignalScale_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerSignalScale_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerSignalScale_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerSignalScale_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerSignalScale_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerSignalScale_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerSignalScale_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerSignalScale_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerSignalScale_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerPacketSize_INTERFACE_DEFINED__
#define __IUsgDopplerPacketSize_INTERFACE_DEFINED__

/* interface IUsgDopplerPacketSize */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerPacketSize;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C500DFDD-ACA3-4594-A0EE-75C089B3980C")
    IUsgDopplerPacketSize : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerPacketSizeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerPacketSize * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerPacketSize * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerPacketSize * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerPacketSize * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerPacketSize * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerPacketSize * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerPacketSize * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerPacketSize * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerPacketSize * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerPacketSize * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerPacketSizeVtbl;

    interface IUsgDopplerPacketSize
    {
        CONST_VTBL struct IUsgDopplerPacketSizeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerPacketSize_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerPacketSize_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerPacketSize_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerPacketSize_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerPacketSize_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerPacketSize_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerPacketSize_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerPacketSize_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerPacketSize_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerPacketSize_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerPacketSize_INTERFACE_DEFINED__ */


#ifndef __IUsgPulsesNumber_INTERFACE_DEFINED__
#define __IUsgPulsesNumber_INTERFACE_DEFINED__

/* interface IUsgPulsesNumber */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgPulsesNumber;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("629FA89F-7BDB-4B79-B3F3-A55AEA078BC0")
    IUsgPulsesNumber : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgPulsesNumberVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgPulsesNumber * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgPulsesNumber * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgPulsesNumber * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgPulsesNumber * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgPulsesNumber * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgPulsesNumber * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgPulsesNumber * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgPulsesNumber * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgPulsesNumber * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgPulsesNumber * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgPulsesNumberVtbl;

    interface IUsgPulsesNumber
    {
        CONST_VTBL struct IUsgPulsesNumberVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgPulsesNumber_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgPulsesNumber_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgPulsesNumber_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgPulsesNumber_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgPulsesNumber_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgPulsesNumber_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgPulsesNumber_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgPulsesNumber_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgPulsesNumber_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgPulsesNumber_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgPulsesNumber_INTERFACE_DEFINED__ */


#ifndef __IUsgCineloop_INTERFACE_DEFINED__
#define __IUsgCineloop_INTERFACE_DEFINED__

/* interface IUsgCineloop */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCineloop;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A2986CE3-3F1A-4361-890D-94816ED1CCF7")
    IUsgCineloop : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
            /* [retval][out] */ LONGLONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
            /* [in] */ LONGLONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ LONGLONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ FLOAT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StopTime( 
            /* [retval][out] */ LONGLONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StopTime( 
            /* [in] */ LONGLONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ CINE_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_State( 
            /* [in] */ CINE_STATE newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAvailable( 
            /* [out] */ LONG *pStart,
            /* [out] */ LONG *pStop) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPositions( 
            LONG *pPos,
            /* [out] */ LONG *pStop) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPositions( 
            /* [in] */ LONGLONG llStart,
            /* [in] */ LONGLONG llStop) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCapabilities( 
            /* [retval][out] */ LONG *pCaps) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckCapabilities( 
            /* [out][in] */ LONG *pCaps) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCineloopVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCineloop * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCineloop * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCineloop * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCineloop * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCineloop * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCineloop * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCineloop * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentPosition )( 
            IUsgCineloop * This,
            /* [retval][out] */ LONGLONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CurrentPosition )( 
            IUsgCineloop * This,
            /* [in] */ LONGLONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Duration )( 
            IUsgCineloop * This,
            /* [retval][out] */ LONGLONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rate )( 
            IUsgCineloop * This,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Rate )( 
            IUsgCineloop * This,
            /* [in] */ FLOAT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StopTime )( 
            IUsgCineloop * This,
            /* [retval][out] */ LONGLONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StopTime )( 
            IUsgCineloop * This,
            /* [in] */ LONGLONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_State )( 
            IUsgCineloop * This,
            /* [retval][out] */ CINE_STATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_State )( 
            IUsgCineloop * This,
            /* [in] */ CINE_STATE newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAvailable )( 
            IUsgCineloop * This,
            /* [out] */ LONG *pStart,
            /* [out] */ LONG *pStop);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPositions )( 
            IUsgCineloop * This,
            LONG *pPos,
            /* [out] */ LONG *pStop);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPositions )( 
            IUsgCineloop * This,
            /* [in] */ LONGLONG llStart,
            /* [in] */ LONGLONG llStop);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCapabilities )( 
            IUsgCineloop * This,
            /* [retval][out] */ LONG *pCaps);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckCapabilities )( 
            IUsgCineloop * This,
            /* [out][in] */ LONG *pCaps);
        
        END_INTERFACE
    } IUsgCineloopVtbl;

    interface IUsgCineloop
    {
        CONST_VTBL struct IUsgCineloopVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCineloop_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCineloop_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCineloop_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCineloop_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCineloop_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCineloop_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCineloop_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCineloop_get_CurrentPosition(This,pVal)	\
    ( (This)->lpVtbl -> get_CurrentPosition(This,pVal) ) 

#define IUsgCineloop_put_CurrentPosition(This,newVal)	\
    ( (This)->lpVtbl -> put_CurrentPosition(This,newVal) ) 

#define IUsgCineloop_get_Duration(This,pVal)	\
    ( (This)->lpVtbl -> get_Duration(This,pVal) ) 

#define IUsgCineloop_get_Rate(This,pVal)	\
    ( (This)->lpVtbl -> get_Rate(This,pVal) ) 

#define IUsgCineloop_put_Rate(This,newVal)	\
    ( (This)->lpVtbl -> put_Rate(This,newVal) ) 

#define IUsgCineloop_get_StopTime(This,pVal)	\
    ( (This)->lpVtbl -> get_StopTime(This,pVal) ) 

#define IUsgCineloop_put_StopTime(This,newVal)	\
    ( (This)->lpVtbl -> put_StopTime(This,newVal) ) 

#define IUsgCineloop_get_State(This,pVal)	\
    ( (This)->lpVtbl -> get_State(This,pVal) ) 

#define IUsgCineloop_put_State(This,newVal)	\
    ( (This)->lpVtbl -> put_State(This,newVal) ) 

#define IUsgCineloop_GetAvailable(This,pStart,pStop)	\
    ( (This)->lpVtbl -> GetAvailable(This,pStart,pStop) ) 

#define IUsgCineloop_GetPositions(This,pPos,pStop)	\
    ( (This)->lpVtbl -> GetPositions(This,pPos,pStop) ) 

#define IUsgCineloop_SetPositions(This,llStart,llStop)	\
    ( (This)->lpVtbl -> SetPositions(This,llStart,llStop) ) 

#define IUsgCineloop_GetCapabilities(This,pCaps)	\
    ( (This)->lpVtbl -> GetCapabilities(This,pCaps) ) 

#define IUsgCineloop_CheckCapabilities(This,pCaps)	\
    ( (This)->lpVtbl -> CheckCapabilities(This,pCaps) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCineloop_INTERFACE_DEFINED__ */


#ifndef __IUsgCineStream_INTERFACE_DEFINED__
#define __IUsgCineStream_INTERFACE_DEFINED__

/* interface IUsgCineStream */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCineStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5071C20D-306B-4EC2-ABA0-1E20E7D5EA7E")
    IUsgCineStream : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
            /* [retval][out] */ LONGLONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
            /* [in] */ LONGLONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StopPosition( 
            /* [retval][out] */ LONGLONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StopPosition( 
            /* [in] */ LONGLONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ LONGLONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ FLOAT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ FLOAT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ CINE_STATE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_State( 
            /* [in] */ CINE_STATE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeFormat( 
            /* [retval][out] */ CINE_TIME_FORMAT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeFormat( 
            /* [in] */ CINE_TIME_FORMAT newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConvertTimeFormat( 
            /* [in] */ LONGLONG srcVal,
            /* [in] */ CINE_TIME_FORMAT srcFormat,
            /* [in] */ CINE_TIME_FORMAT dstFormat,
            /* [retval][out] */ LONGLONG *dstVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAvailable( 
            /* [out] */ LONGLONG *lEarliest,
            /* [out] */ LONGLONG *lLatest) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetShiftPosition( 
            /* [in] */ LONGLONG from,
            /* [in] */ LONGLONG shift,
            /* [in] */ CINE_TIME_FORMAT timeFormat,
            /* [retval][out] */ LONGLONG *pos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHoldRange( 
            /* [in] */ LONGLONG from,
            /* [in] */ LONGLONG to) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHoldRange( 
            /* [out] */ LONGLONG *from,
            /* [out] */ LONGLONG *to) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsTimeFormatSupported( 
            /* [in] */ CINE_TIME_FORMAT format,
            /* [retval][out] */ BOOL *support) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetContinuousInterval( 
            /* [in] */ LONGLONG position,
            /* [out] */ LONGLONG *start,
            /* [out] */ LONGLONG *end) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSelection( 
            /* [in] */ LONGLONG from,
            /* [in] */ LONGLONG to,
            /* [in] */ SELECTION_MODE mode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCineStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCineStream * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCineStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCineStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCineStream * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCineStream * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCineStream * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCineStream * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentPosition )( 
            IUsgCineStream * This,
            /* [retval][out] */ LONGLONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CurrentPosition )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StopPosition )( 
            IUsgCineStream * This,
            /* [retval][out] */ LONGLONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StopPosition )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Duration )( 
            IUsgCineStream * This,
            /* [retval][out] */ LONGLONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rate )( 
            IUsgCineStream * This,
            /* [retval][out] */ FLOAT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Rate )( 
            IUsgCineStream * This,
            /* [in] */ FLOAT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_State )( 
            IUsgCineStream * This,
            /* [retval][out] */ CINE_STATE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_State )( 
            IUsgCineStream * This,
            /* [in] */ CINE_STATE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeFormat )( 
            IUsgCineStream * This,
            /* [retval][out] */ CINE_TIME_FORMAT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeFormat )( 
            IUsgCineStream * This,
            /* [in] */ CINE_TIME_FORMAT newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertTimeFormat )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG srcVal,
            /* [in] */ CINE_TIME_FORMAT srcFormat,
            /* [in] */ CINE_TIME_FORMAT dstFormat,
            /* [retval][out] */ LONGLONG *dstVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAvailable )( 
            IUsgCineStream * This,
            /* [out] */ LONGLONG *lEarliest,
            /* [out] */ LONGLONG *lLatest);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetShiftPosition )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG from,
            /* [in] */ LONGLONG shift,
            /* [in] */ CINE_TIME_FORMAT timeFormat,
            /* [retval][out] */ LONGLONG *pos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHoldRange )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG from,
            /* [in] */ LONGLONG to);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHoldRange )( 
            IUsgCineStream * This,
            /* [out] */ LONGLONG *from,
            /* [out] */ LONGLONG *to);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsTimeFormatSupported )( 
            IUsgCineStream * This,
            /* [in] */ CINE_TIME_FORMAT format,
            /* [retval][out] */ BOOL *support);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetContinuousInterval )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG position,
            /* [out] */ LONGLONG *start,
            /* [out] */ LONGLONG *end);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSelection )( 
            IUsgCineStream * This,
            /* [in] */ LONGLONG from,
            /* [in] */ LONGLONG to,
            /* [in] */ SELECTION_MODE mode);
        
        END_INTERFACE
    } IUsgCineStreamVtbl;

    interface IUsgCineStream
    {
        CONST_VTBL struct IUsgCineStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCineStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCineStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCineStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCineStream_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCineStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCineStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCineStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCineStream_get_CurrentPosition(This,pVal)	\
    ( (This)->lpVtbl -> get_CurrentPosition(This,pVal) ) 

#define IUsgCineStream_put_CurrentPosition(This,newVal)	\
    ( (This)->lpVtbl -> put_CurrentPosition(This,newVal) ) 

#define IUsgCineStream_get_StopPosition(This,pVal)	\
    ( (This)->lpVtbl -> get_StopPosition(This,pVal) ) 

#define IUsgCineStream_put_StopPosition(This,newVal)	\
    ( (This)->lpVtbl -> put_StopPosition(This,newVal) ) 

#define IUsgCineStream_get_Duration(This,pVal)	\
    ( (This)->lpVtbl -> get_Duration(This,pVal) ) 

#define IUsgCineStream_get_Rate(This,pVal)	\
    ( (This)->lpVtbl -> get_Rate(This,pVal) ) 

#define IUsgCineStream_put_Rate(This,newVal)	\
    ( (This)->lpVtbl -> put_Rate(This,newVal) ) 

#define IUsgCineStream_get_State(This,pVal)	\
    ( (This)->lpVtbl -> get_State(This,pVal) ) 

#define IUsgCineStream_put_State(This,newVal)	\
    ( (This)->lpVtbl -> put_State(This,newVal) ) 

#define IUsgCineStream_get_TimeFormat(This,pVal)	\
    ( (This)->lpVtbl -> get_TimeFormat(This,pVal) ) 

#define IUsgCineStream_put_TimeFormat(This,newVal)	\
    ( (This)->lpVtbl -> put_TimeFormat(This,newVal) ) 

#define IUsgCineStream_ConvertTimeFormat(This,srcVal,srcFormat,dstFormat,dstVal)	\
    ( (This)->lpVtbl -> ConvertTimeFormat(This,srcVal,srcFormat,dstFormat,dstVal) ) 

#define IUsgCineStream_GetAvailable(This,lEarliest,lLatest)	\
    ( (This)->lpVtbl -> GetAvailable(This,lEarliest,lLatest) ) 

#define IUsgCineStream_GetShiftPosition(This,from,shift,timeFormat,pos)	\
    ( (This)->lpVtbl -> GetShiftPosition(This,from,shift,timeFormat,pos) ) 

#define IUsgCineStream_SetHoldRange(This,from,to)	\
    ( (This)->lpVtbl -> SetHoldRange(This,from,to) ) 

#define IUsgCineStream_GetHoldRange(This,from,to)	\
    ( (This)->lpVtbl -> GetHoldRange(This,from,to) ) 

#define IUsgCineStream_IsTimeFormatSupported(This,format,support)	\
    ( (This)->lpVtbl -> IsTimeFormatSupported(This,format,support) ) 

#define IUsgCineStream_GetContinuousInterval(This,position,start,end)	\
    ( (This)->lpVtbl -> GetContinuousInterval(This,position,start,end) ) 

#define IUsgCineStream_SetSelection(This,from,to,mode)	\
    ( (This)->lpVtbl -> SetSelection(This,from,to,mode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCineStream_INTERFACE_DEFINED__ */


#ifndef __IUsgCineSink_INTERFACE_DEFINED__
#define __IUsgCineSink_INTERFACE_DEFINED__

/* interface IUsgCineSink */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCineSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("04D658E8-6691-4034-ADFD-39EBE7A030DA")
    IUsgCineSink : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBufferSize( 
            /* [out] */ LONG *total,
            /* [out] */ LONG *used,
            /* [out] */ LONG *writePos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBufferSize( 
            /* [in] */ LONG cbSize) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveData( 
            /* [in] */ BSTR strFileName) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCineSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCineSink * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCineSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCineSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCineSink * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCineSink * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCineSink * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCineSink * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBufferSize )( 
            IUsgCineSink * This,
            /* [out] */ LONG *total,
            /* [out] */ LONG *used,
            /* [out] */ LONG *writePos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUsgCineSink * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBufferSize )( 
            IUsgCineSink * This,
            /* [in] */ LONG cbSize);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveData )( 
            IUsgCineSink * This,
            /* [in] */ BSTR strFileName);
        
        END_INTERFACE
    } IUsgCineSinkVtbl;

    interface IUsgCineSink
    {
        CONST_VTBL struct IUsgCineSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCineSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCineSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCineSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCineSink_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCineSink_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCineSink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCineSink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCineSink_GetBufferSize(This,total,used,writePos)	\
    ( (This)->lpVtbl -> GetBufferSize(This,total,used,writePos) ) 

#define IUsgCineSink_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUsgCineSink_SetBufferSize(This,cbSize)	\
    ( (This)->lpVtbl -> SetBufferSize(This,cbSize) ) 

#define IUsgCineSink_SaveData(This,strFileName)	\
    ( (This)->lpVtbl -> SaveData(This,strFileName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCineSink_INTERFACE_DEFINED__ */


#ifndef __IUsgCineSink2_INTERFACE_DEFINED__
#define __IUsgCineSink2_INTERFACE_DEFINED__

/* interface IUsgCineSink2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCineSink2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9EB34740-8AA5-4373-A1FF-AC9A864D5ABB")
    IUsgCineSink2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBufferSize( 
            /* [out] */ LONG64 *total,
            /* [out] */ LONG64 *used,
            /* [out] */ LONG64 *writePos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBufferSize( 
            /* [in] */ LONG64 cbSize) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveData( 
            /* [in] */ BSTR strFileName) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCineSink2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCineSink2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCineSink2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCineSink2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCineSink2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCineSink2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCineSink2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCineSink2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBufferSize )( 
            IUsgCineSink2 * This,
            /* [out] */ LONG64 *total,
            /* [out] */ LONG64 *used,
            /* [out] */ LONG64 *writePos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUsgCineSink2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBufferSize )( 
            IUsgCineSink2 * This,
            /* [in] */ LONG64 cbSize);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveData )( 
            IUsgCineSink2 * This,
            /* [in] */ BSTR strFileName);
        
        END_INTERFACE
    } IUsgCineSink2Vtbl;

    interface IUsgCineSink2
    {
        CONST_VTBL struct IUsgCineSink2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCineSink2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCineSink2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCineSink2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCineSink2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCineSink2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCineSink2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCineSink2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCineSink2_GetBufferSize(This,total,used,writePos)	\
    ( (This)->lpVtbl -> GetBufferSize(This,total,used,writePos) ) 

#define IUsgCineSink2_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUsgCineSink2_SetBufferSize(This,cbSize)	\
    ( (This)->lpVtbl -> SetBufferSize(This,cbSize) ) 

#define IUsgCineSink2_SaveData(This,strFileName)	\
    ( (This)->lpVtbl -> SaveData(This,strFileName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCineSink2_INTERFACE_DEFINED__ */


#ifndef __IUsgSpatialFilter_INTERFACE_DEFINED__
#define __IUsgSpatialFilter_INTERFACE_DEFINED__

/* interface IUsgSpatialFilter */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgSpatialFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("31BF1183-35D2-4EF3-A1B1-ADEBDEDD81A0")
    IUsgSpatialFilter : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgSpatialFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgSpatialFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgSpatialFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgSpatialFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgSpatialFilter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgSpatialFilter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgSpatialFilter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgSpatialFilter * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgSpatialFilter * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgSpatialFilter * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgSpatialFilter * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IUsgSpatialFilter * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IUsgSpatialFilter * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IUsgSpatialFilterVtbl;

    interface IUsgSpatialFilter
    {
        CONST_VTBL struct IUsgSpatialFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgSpatialFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgSpatialFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgSpatialFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgSpatialFilter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgSpatialFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgSpatialFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgSpatialFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgSpatialFilter_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgSpatialFilter_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgSpatialFilter_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgSpatialFilter_get_Enabled(This,pVal)	\
    ( (This)->lpVtbl -> get_Enabled(This,pVal) ) 

#define IUsgSpatialFilter_put_Enabled(This,newVal)	\
    ( (This)->lpVtbl -> put_Enabled(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgSpatialFilter_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerSampleVolume_INTERFACE_DEFINED__
#define __IUsgDopplerSampleVolume_INTERFACE_DEFINED__

/* interface IUsgDopplerSampleVolume */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerSampleVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F81C0C94-B620-46EA-B6C3-5D10E4D7F06C")
    IUsgDopplerSampleVolume : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVolume( 
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVolume( 
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DepthsRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VolumesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSampleVolume( 
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth,
            /* [in] */ LONG line) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPhantomVolume( 
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerSampleVolumeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerSampleVolume * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerSampleVolume * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerSampleVolume * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerSampleVolume * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerSampleVolume * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerSampleVolume * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerSampleVolume * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVolume )( 
            IUsgDopplerSampleVolume * This,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVolume )( 
            IUsgDopplerSampleVolume * This,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DepthsRange )( 
            IUsgDopplerSampleVolume * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VolumesRange )( 
            IUsgDopplerSampleVolume * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSampleVolume )( 
            IUsgDopplerSampleVolume * This,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth,
            /* [in] */ LONG line);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPhantomVolume )( 
            IUsgDopplerSampleVolume * This,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        END_INTERFACE
    } IUsgDopplerSampleVolumeVtbl;

    interface IUsgDopplerSampleVolume
    {
        CONST_VTBL struct IUsgDopplerSampleVolumeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerSampleVolume_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerSampleVolume_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerSampleVolume_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerSampleVolume_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerSampleVolume_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerSampleVolume_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerSampleVolume_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerSampleVolume_SetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetVolume(This,startDepth,endDepth) ) 

#define IUsgDopplerSampleVolume_GetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetVolume(This,startDepth,endDepth) ) 

#define IUsgDopplerSampleVolume_get_DepthsRange(This,pVal)	\
    ( (This)->lpVtbl -> get_DepthsRange(This,pVal) ) 

#define IUsgDopplerSampleVolume_get_VolumesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_VolumesRange(This,pVal) ) 

#define IUsgDopplerSampleVolume_SetSampleVolume(This,startDepth,endDepth,line)	\
    ( (This)->lpVtbl -> SetSampleVolume(This,startDepth,endDepth,line) ) 

#define IUsgDopplerSampleVolume_GetPhantomVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetPhantomVolume(This,startDepth,endDepth) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerSampleVolume_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerCorrectionAngle_INTERFACE_DEFINED__
#define __IUsgDopplerCorrectionAngle_INTERFACE_DEFINED__

/* interface IUsgDopplerCorrectionAngle */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerCorrectionAngle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2BFE46DC-DD1A-42C8-8BDB-D034D5FA04DB")
    IUsgDopplerCorrectionAngle : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerCorrectionAngleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerCorrectionAngle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerCorrectionAngle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerCorrectionAngle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerCorrectionAngle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerCorrectionAngle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerCorrectionAngle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerCorrectionAngle * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerCorrectionAngle * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerCorrectionAngle * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerCorrectionAngle * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerCorrectionAngleVtbl;

    interface IUsgDopplerCorrectionAngle
    {
        CONST_VTBL struct IUsgDopplerCorrectionAngleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerCorrectionAngle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerCorrectionAngle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerCorrectionAngle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerCorrectionAngle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerCorrectionAngle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerCorrectionAngle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerCorrectionAngle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerCorrectionAngle_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerCorrectionAngle_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerCorrectionAngle_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerCorrectionAngle_INTERFACE_DEFINED__ */


#ifndef __IUsgScanConverterPlugin_INTERFACE_DEFINED__
#define __IUsgScanConverterPlugin_INTERFACE_DEFINED__

/* interface IUsgScanConverterPlugin */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanConverterPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DD280DD5-674A-4837-9F03-9FDE77596599")
    IUsgScanConverterPlugin : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScanConverter( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanConverterPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanConverterPlugin * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanConverterPlugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanConverterPlugin * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanConverterPlugin * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanConverterPlugin * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanConverterPlugin * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanConverterPlugin * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanConverter )( 
            IUsgScanConverterPlugin * This,
            /* [retval][out] */ IUnknown **pVal);
        
        END_INTERFACE
    } IUsgScanConverterPluginVtbl;

    interface IUsgScanConverterPlugin
    {
        CONST_VTBL struct IUsgScanConverterPluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanConverterPlugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanConverterPlugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanConverterPlugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanConverterPlugin_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanConverterPlugin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanConverterPlugin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanConverterPlugin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanConverterPlugin_get_ScanConverter(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanConverter(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanConverterPlugin_INTERFACE_DEFINED__ */


#ifndef __IUsgScanConverterPlugin2_INTERFACE_DEFINED__
#define __IUsgScanConverterPlugin2_INTERFACE_DEFINED__

/* interface IUsgScanConverterPlugin2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanConverterPlugin2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2F84D02F-E381-4b94-AB10-52D64013AF4C")
    IUsgScanConverterPlugin2 : public IUsgScanConverterPlugin
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InputPin( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputPin( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanConverterPlugin2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanConverterPlugin2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanConverterPlugin2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanConverterPlugin2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanConverterPlugin2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanConverterPlugin2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanConverterPlugin2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanConverterPlugin2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ScanConverter )( 
            IUsgScanConverterPlugin2 * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InputPin )( 
            IUsgScanConverterPlugin2 * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OutputPin )( 
            IUsgScanConverterPlugin2 * This,
            /* [retval][out] */ ULONG *pVal);
        
        END_INTERFACE
    } IUsgScanConverterPlugin2Vtbl;

    interface IUsgScanConverterPlugin2
    {
        CONST_VTBL struct IUsgScanConverterPlugin2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanConverterPlugin2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanConverterPlugin2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanConverterPlugin2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanConverterPlugin2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanConverterPlugin2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanConverterPlugin2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanConverterPlugin2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanConverterPlugin2_get_ScanConverter(This,pVal)	\
    ( (This)->lpVtbl -> get_ScanConverter(This,pVal) ) 


#define IUsgScanConverterPlugin2_get_InputPin(This,pVal)	\
    ( (This)->lpVtbl -> get_InputPin(This,pVal) ) 

#define IUsgScanConverterPlugin2_get_OutputPin(This,pVal)	\
    ( (This)->lpVtbl -> get_OutputPin(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanConverterPlugin2_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerSignalSmooth_INTERFACE_DEFINED__
#define __IUsgDopplerSignalSmooth_INTERFACE_DEFINED__

/* interface IUsgDopplerSignalSmooth */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerSignalSmooth;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D2095BD-D9D0-421F-8787-A0C764BED735")
    IUsgDopplerSignalSmooth : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerSignalSmoothVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerSignalSmooth * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerSignalSmooth * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerSignalSmooth * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerSignalSmooth * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerSignalSmooth * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerSignalSmooth * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerSignalSmooth * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerSignalSmooth * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerSignalSmooth * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerSignalSmooth * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerSignalSmoothVtbl;

    interface IUsgDopplerSignalSmooth
    {
        CONST_VTBL struct IUsgDopplerSignalSmoothVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerSignalSmooth_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerSignalSmooth_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerSignalSmooth_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerSignalSmooth_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerSignalSmooth_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerSignalSmooth_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerSignalSmooth_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerSignalSmooth_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerSignalSmooth_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerSignalSmooth_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerSignalSmooth_INTERFACE_DEFINED__ */


#ifndef __IUsgAudioVolume_INTERFACE_DEFINED__
#define __IUsgAudioVolume_INTERFACE_DEFINED__

/* interface IUsgAudioVolume */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgAudioVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("17EBD173-3BBD-427C-9844-F274099B0562")
    IUsgAudioVolume : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mute( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mute( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgAudioVolumeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgAudioVolume * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgAudioVolume * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgAudioVolume * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgAudioVolume * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgAudioVolume * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgAudioVolume * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgAudioVolume * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgAudioVolume * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgAudioVolume * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgAudioVolume * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Mute )( 
            IUsgAudioVolume * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Mute )( 
            IUsgAudioVolume * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } IUsgAudioVolumeVtbl;

    interface IUsgAudioVolume
    {
        CONST_VTBL struct IUsgAudioVolumeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgAudioVolume_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgAudioVolume_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgAudioVolume_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgAudioVolume_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgAudioVolume_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgAudioVolume_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgAudioVolume_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgAudioVolume_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgAudioVolume_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgAudioVolume_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgAudioVolume_get_Mute(This,pVal)	\
    ( (This)->lpVtbl -> get_Mute(This,pVal) ) 

#define IUsgAudioVolume_put_Mute(This,newVal)	\
    ( (This)->lpVtbl -> put_Mute(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgAudioVolume_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerSpectralAvg_INTERFACE_DEFINED__
#define __IUsgDopplerSpectralAvg_INTERFACE_DEFINED__

/* interface IUsgDopplerSpectralAvg */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerSpectralAvg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7BAAC412-6FF0-42C0-85A0-9D679E91FF6B")
    IUsgDopplerSpectralAvg : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerSpectralAvgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerSpectralAvg * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerSpectralAvg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerSpectralAvg * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerSpectralAvg * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerSpectralAvg * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerSpectralAvg * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerSpectralAvg * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerSpectralAvg * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerSpectralAvg * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerSpectralAvg * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDopplerSpectralAvgVtbl;

    interface IUsgDopplerSpectralAvg
    {
        CONST_VTBL struct IUsgDopplerSpectralAvgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerSpectralAvg_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerSpectralAvg_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerSpectralAvg_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerSpectralAvg_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerSpectralAvg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerSpectralAvg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerSpectralAvg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerSpectralAvg_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerSpectralAvg_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerSpectralAvg_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerSpectralAvg_INTERFACE_DEFINED__ */


#ifndef __IUsgBioeffectsIndices_INTERFACE_DEFINED__
#define __IUsgBioeffectsIndices_INTERFACE_DEFINED__

/* interface IUsgBioeffectsIndices */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgBioeffectsIndices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("533907B0-42A7-474D-AB97-342EEBDD9AA0")
    IUsgBioeffectsIndices : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsSupported( 
            /* [in] */ BIOEFFECT_INDEX nIndex,
            /* [retval][out] */ LONG *supported) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIndex( 
            /* [in] */ BIOEFFECT_INDEX nIndex,
            /* [retval][out] */ VALUE_BOUNDS *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgBioeffectsIndicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgBioeffectsIndices * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgBioeffectsIndices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgBioeffectsIndices * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgBioeffectsIndices * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgBioeffectsIndices * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgBioeffectsIndices * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgBioeffectsIndices * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsSupported )( 
            IUsgBioeffectsIndices * This,
            /* [in] */ BIOEFFECT_INDEX nIndex,
            /* [retval][out] */ LONG *supported);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetIndex )( 
            IUsgBioeffectsIndices * This,
            /* [in] */ BIOEFFECT_INDEX nIndex,
            /* [retval][out] */ VALUE_BOUNDS *value);
        
        END_INTERFACE
    } IUsgBioeffectsIndicesVtbl;

    interface IUsgBioeffectsIndices
    {
        CONST_VTBL struct IUsgBioeffectsIndicesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgBioeffectsIndices_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgBioeffectsIndices_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgBioeffectsIndices_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgBioeffectsIndices_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgBioeffectsIndices_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgBioeffectsIndices_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgBioeffectsIndices_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgBioeffectsIndices_IsSupported(This,nIndex,supported)	\
    ( (This)->lpVtbl -> IsSupported(This,nIndex,supported) ) 

#define IUsgBioeffectsIndices_GetIndex(This,nIndex,value)	\
    ( (This)->lpVtbl -> GetIndex(This,nIndex,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgBioeffectsIndices_INTERFACE_DEFINED__ */


#ifndef __IUsgProbeFrequency3_INTERFACE_DEFINED__
#define __IUsgProbeFrequency3_INTERFACE_DEFINED__

/* interface IUsgProbeFrequency3 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgProbeFrequency3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("491CFD05-2F69-42F4-9514-AE47890B6E1E")
    IUsgProbeFrequency3 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ struct FREQUENCY3 *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ struct FREQUENCY3 newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgProbeFrequency3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgProbeFrequency3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgProbeFrequency3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgProbeFrequency3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgProbeFrequency3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgProbeFrequency3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgProbeFrequency3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgProbeFrequency3 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgProbeFrequency3 * This,
            /* [retval][out] */ struct FREQUENCY3 *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgProbeFrequency3 * This,
            /* [in] */ struct FREQUENCY3 newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgProbeFrequency3 * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgProbeFrequency3Vtbl;

    interface IUsgProbeFrequency3
    {
        CONST_VTBL struct IUsgProbeFrequency3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgProbeFrequency3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgProbeFrequency3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgProbeFrequency3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgProbeFrequency3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgProbeFrequency3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgProbeFrequency3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgProbeFrequency3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgProbeFrequency3_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgProbeFrequency3_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgProbeFrequency3_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgProbeFrequency3_INTERFACE_DEFINED__ */


#ifndef __IUsgDopplerColorTransparency_INTERFACE_DEFINED__
#define __IUsgDopplerColorTransparency_INTERFACE_DEFINED__

/* interface IUsgDopplerColorTransparency */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDopplerColorTransparency;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B27183A9-33AF-40AC-A132-11F034DE7817")
    IUsgDopplerColorTransparency : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TransparencyMode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TransparencyMode( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDopplerColorTransparencyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDopplerColorTransparency * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDopplerColorTransparency * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDopplerColorTransparency * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDopplerColorTransparency * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDopplerColorTransparency * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDopplerColorTransparency * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDopplerColorTransparency * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDopplerColorTransparency * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDopplerColorTransparency * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDopplerColorTransparency * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TransparencyMode )( 
            IUsgDopplerColorTransparency * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TransparencyMode )( 
            IUsgDopplerColorTransparency * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IUsgDopplerColorTransparencyVtbl;

    interface IUsgDopplerColorTransparency
    {
        CONST_VTBL struct IUsgDopplerColorTransparencyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDopplerColorTransparency_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDopplerColorTransparency_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDopplerColorTransparency_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDopplerColorTransparency_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDopplerColorTransparency_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDopplerColorTransparency_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDopplerColorTransparency_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDopplerColorTransparency_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDopplerColorTransparency_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDopplerColorTransparency_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#define IUsgDopplerColorTransparency_get_TransparencyMode(This,pVal)	\
    ( (This)->lpVtbl -> get_TransparencyMode(This,pVal) ) 

#define IUsgDopplerColorTransparency_put_TransparencyMode(This,newVal)	\
    ( (This)->lpVtbl -> put_TransparencyMode(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDopplerColorTransparency_INTERFACE_DEFINED__ */


#ifndef __IUsg3dVolumeSize_INTERFACE_DEFINED__
#define __IUsg3dVolumeSize_INTERFACE_DEFINED__

/* interface IUsg3dVolumeSize */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsg3dVolumeSize;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("145E5DFC-246E-4B55-8EB1-4C956E98854F")
    IUsg3dVolumeSize : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsg3dVolumeSizeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsg3dVolumeSize * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsg3dVolumeSize * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsg3dVolumeSize * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsg3dVolumeSize * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsg3dVolumeSize * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsg3dVolumeSize * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsg3dVolumeSize * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsg3dVolumeSize * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsg3dVolumeSize * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsg3dVolumeSize * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsg3dVolumeSizeVtbl;

    interface IUsg3dVolumeSize
    {
        CONST_VTBL struct IUsg3dVolumeSizeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsg3dVolumeSize_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsg3dVolumeSize_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsg3dVolumeSize_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsg3dVolumeSize_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsg3dVolumeSize_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsg3dVolumeSize_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsg3dVolumeSize_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsg3dVolumeSize_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsg3dVolumeSize_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsg3dVolumeSize_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsg3dVolumeSize_INTERFACE_DEFINED__ */


#ifndef __IUsg3dVolumeDensity_INTERFACE_DEFINED__
#define __IUsg3dVolumeDensity_INTERFACE_DEFINED__

/* interface IUsg3dVolumeDensity */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsg3dVolumeDensity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("42C2A978-C31B-4235-9292-E53726E5A61C")
    IUsg3dVolumeDensity : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsg3dVolumeDensityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsg3dVolumeDensity * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsg3dVolumeDensity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsg3dVolumeDensity * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsg3dVolumeDensity * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsg3dVolumeDensity * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsg3dVolumeDensity * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsg3dVolumeDensity * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsg3dVolumeDensity * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsg3dVolumeDensity * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsg3dVolumeDensity * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsg3dVolumeDensityVtbl;

    interface IUsg3dVolumeDensity
    {
        CONST_VTBL struct IUsg3dVolumeDensityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsg3dVolumeDensity_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsg3dVolumeDensity_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsg3dVolumeDensity_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsg3dVolumeDensity_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsg3dVolumeDensity_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsg3dVolumeDensity_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsg3dVolumeDensity_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsg3dVolumeDensity_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsg3dVolumeDensity_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsg3dVolumeDensity_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsg3dVolumeDensity_INTERFACE_DEFINED__ */


#ifndef __IUsgFileStorage_INTERFACE_DEFINED__
#define __IUsgFileStorage_INTERFACE_DEFINED__

/* interface IUsgFileStorage */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgFileStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CF81935-DCB3-4C19-9DC9-765306A4714F")
    IUsgFileStorage : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FileName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FileName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ LONG saveFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateDataKey( 
            /* [in] */ LONG_PTR hBaseKey,
            /* [in] */ LONG keyId,
            /* [retval][out] */ LONG_PTR *dataKey) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenDataKey( 
            /* [in] */ LONG_PTR baseKey,
            /* [in] */ LONG keyId,
            /* [in] */ LONG keyIndex,
            /* [retval][out] */ LONG_PTR *dataKey) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseDataKey( 
            /* [in] */ LONG_PTR dataKey) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKeyInfo( 
            /* [in] */ LONG_PTR dataKey,
            /* [out] */ LONG *subKeys,
            /* [out] */ LONG *dataValues) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumValues( 
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG ValueNum,
            /* [out] */ LONG *valueId,
            /* [out] */ LONG *valueSize) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumSubkeys( 
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG subkeyNum,
            /* [out] */ LONG *subkeyId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG valueId,
            /* [in] */ LONG valueIndex,
            /* [in] */ LONG cbBufferSize,
            /* [size_is][out] */ BYTE *dataBuffer,
            /* [out] */ LONG *dataSize) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG valueId,
            /* [in] */ LONG valueIndex,
            /* [size_is][in] */ BYTE *dataBuffer,
            /* [in] */ LONG dataSize) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgFileStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgFileStorage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgFileStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgFileStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgFileStorage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgFileStorage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgFileStorage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgFileStorage * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileName )( 
            IUsgFileStorage * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FileName )( 
            IUsgFileStorage * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            IUsgFileStorage * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IUsgFileStorage * This,
            /* [in] */ LONG saveFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateDataKey )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR hBaseKey,
            /* [in] */ LONG keyId,
            /* [retval][out] */ LONG_PTR *dataKey);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenDataKey )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR baseKey,
            /* [in] */ LONG keyId,
            /* [in] */ LONG keyIndex,
            /* [retval][out] */ LONG_PTR *dataKey);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseDataKey )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR dataKey);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyInfo )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR dataKey,
            /* [out] */ LONG *subKeys,
            /* [out] */ LONG *dataValues);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnumValues )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG ValueNum,
            /* [out] */ LONG *valueId,
            /* [out] */ LONG *valueSize);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnumSubkeys )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG subkeyNum,
            /* [out] */ LONG *subkeyId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG valueId,
            /* [in] */ LONG valueIndex,
            /* [in] */ LONG cbBufferSize,
            /* [size_is][out] */ BYTE *dataBuffer,
            /* [out] */ LONG *dataSize);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IUsgFileStorage * This,
            /* [in] */ LONG_PTR dataKey,
            /* [in] */ LONG valueId,
            /* [in] */ LONG valueIndex,
            /* [size_is][in] */ BYTE *dataBuffer,
            /* [in] */ LONG dataSize);
        
        END_INTERFACE
    } IUsgFileStorageVtbl;

    interface IUsgFileStorage
    {
        CONST_VTBL struct IUsgFileStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgFileStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgFileStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgFileStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgFileStorage_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgFileStorage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgFileStorage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgFileStorage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgFileStorage_get_FileName(This,pVal)	\
    ( (This)->lpVtbl -> get_FileName(This,pVal) ) 

#define IUsgFileStorage_put_FileName(This,newVal)	\
    ( (This)->lpVtbl -> put_FileName(This,newVal) ) 

#define IUsgFileStorage_Load(This)	\
    ( (This)->lpVtbl -> Load(This) ) 

#define IUsgFileStorage_Save(This,saveFlags)	\
    ( (This)->lpVtbl -> Save(This,saveFlags) ) 

#define IUsgFileStorage_CreateDataKey(This,hBaseKey,keyId,dataKey)	\
    ( (This)->lpVtbl -> CreateDataKey(This,hBaseKey,keyId,dataKey) ) 

#define IUsgFileStorage_OpenDataKey(This,baseKey,keyId,keyIndex,dataKey)	\
    ( (This)->lpVtbl -> OpenDataKey(This,baseKey,keyId,keyIndex,dataKey) ) 

#define IUsgFileStorage_CloseDataKey(This,dataKey)	\
    ( (This)->lpVtbl -> CloseDataKey(This,dataKey) ) 

#define IUsgFileStorage_GetKeyInfo(This,dataKey,subKeys,dataValues)	\
    ( (This)->lpVtbl -> GetKeyInfo(This,dataKey,subKeys,dataValues) ) 

#define IUsgFileStorage_EnumValues(This,dataKey,ValueNum,valueId,valueSize)	\
    ( (This)->lpVtbl -> EnumValues(This,dataKey,ValueNum,valueId,valueSize) ) 

#define IUsgFileStorage_EnumSubkeys(This,dataKey,subkeyNum,subkeyId)	\
    ( (This)->lpVtbl -> EnumSubkeys(This,dataKey,subkeyNum,subkeyId) ) 

#define IUsgFileStorage_GetValue(This,dataKey,valueId,valueIndex,cbBufferSize,dataBuffer,dataSize)	\
    ( (This)->lpVtbl -> GetValue(This,dataKey,valueId,valueIndex,cbBufferSize,dataBuffer,dataSize) ) 

#define IUsgFileStorage_SetValue(This,dataKey,valueId,valueIndex,dataBuffer,dataSize)	\
    ( (This)->lpVtbl -> SetValue(This,dataKey,valueId,valueIndex,dataBuffer,dataSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgFileStorage_INTERFACE_DEFINED__ */


#ifndef __IUsgfw2Debug_INTERFACE_DEFINED__
#define __IUsgfw2Debug_INTERFACE_DEFINED__

/* interface IUsgfw2Debug */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgfw2Debug;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1E181F99-13FB-4570-9934-094D6D5274A9")
    IUsgfw2Debug : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateObjectsDump( 
            /* [in] */ BSTR fileName,
            /* [in] */ LONG reserved1,
            /* [in] */ LONG reserved2) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgfw2DebugVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgfw2Debug * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgfw2Debug * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgfw2Debug * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgfw2Debug * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgfw2Debug * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgfw2Debug * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgfw2Debug * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateObjectsDump )( 
            IUsgfw2Debug * This,
            /* [in] */ BSTR fileName,
            /* [in] */ LONG reserved1,
            /* [in] */ LONG reserved2);
        
        END_INTERFACE
    } IUsgfw2DebugVtbl;

    interface IUsgfw2Debug
    {
        CONST_VTBL struct IUsgfw2DebugVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgfw2Debug_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgfw2Debug_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgfw2Debug_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgfw2Debug_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgfw2Debug_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgfw2Debug_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgfw2Debug_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgfw2Debug_CreateObjectsDump(This,fileName,reserved1,reserved2)	\
    ( (This)->lpVtbl -> CreateObjectsDump(This,fileName,reserved1,reserved2) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgfw2Debug_INTERFACE_DEFINED__ */


#ifndef __IUsgPlugin_INTERFACE_DEFINED__
#define __IUsgPlugin_INTERFACE_DEFINED__

/* interface IUsgPlugin */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("19E2FD36-9D47-4A86-BF5E-CD92A00F16D5")
    IUsgPlugin : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddPlugin( 
            /* [in] */ IUnknown *pPlugin,
            /* [retval][out] */ LONG *cookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePlugin( 
            /* [in] */ LONG cookie) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgPlugin * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgPlugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgPlugin * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgPlugin * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgPlugin * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgPlugin * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgPlugin * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddPlugin )( 
            IUsgPlugin * This,
            /* [in] */ IUnknown *pPlugin,
            /* [retval][out] */ LONG *cookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemovePlugin )( 
            IUsgPlugin * This,
            /* [in] */ LONG cookie);
        
        END_INTERFACE
    } IUsgPluginVtbl;

    interface IUsgPlugin
    {
        CONST_VTBL struct IUsgPluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgPlugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgPlugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgPlugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgPlugin_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgPlugin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgPlugin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgPlugin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgPlugin_AddPlugin(This,pPlugin,cookie)	\
    ( (This)->lpVtbl -> AddPlugin(This,pPlugin,cookie) ) 

#define IUsgPlugin_RemovePlugin(This,cookie)	\
    ( (This)->lpVtbl -> RemovePlugin(This,cookie) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgPlugin_INTERFACE_DEFINED__ */


#ifndef __IBeamformerPowerState_INTERFACE_DEFINED__
#define __IBeamformerPowerState_INTERFACE_DEFINED__

/* interface IBeamformerPowerState */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBeamformerPowerState;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9D1D0EB0-C497-42EE-BB75-EB0BA5F774DC")
    IBeamformerPowerState : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BatteryChargeStatus( 
            /* [retval][out] */ BATTERY_CHARGE_STATUS *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BatteryFullLifetime( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BatteryLifePercent( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BatteryLifeRemaining( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PowerLineStatus( 
            /* [retval][out] */ POWER_LINE_STATUS *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBeamformerPowerStateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBeamformerPowerState * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBeamformerPowerState * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBeamformerPowerState * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBeamformerPowerState * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBeamformerPowerState * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBeamformerPowerState * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBeamformerPowerState * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BatteryChargeStatus )( 
            IBeamformerPowerState * This,
            /* [retval][out] */ BATTERY_CHARGE_STATUS *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BatteryFullLifetime )( 
            IBeamformerPowerState * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BatteryLifePercent )( 
            IBeamformerPowerState * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BatteryLifeRemaining )( 
            IBeamformerPowerState * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PowerLineStatus )( 
            IBeamformerPowerState * This,
            /* [retval][out] */ POWER_LINE_STATUS *pVal);
        
        END_INTERFACE
    } IBeamformerPowerStateVtbl;

    interface IBeamformerPowerState
    {
        CONST_VTBL struct IBeamformerPowerStateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBeamformerPowerState_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBeamformerPowerState_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBeamformerPowerState_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBeamformerPowerState_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBeamformerPowerState_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBeamformerPowerState_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBeamformerPowerState_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBeamformerPowerState_get_BatteryChargeStatus(This,pVal)	\
    ( (This)->lpVtbl -> get_BatteryChargeStatus(This,pVal) ) 

#define IBeamformerPowerState_get_BatteryFullLifetime(This,pVal)	\
    ( (This)->lpVtbl -> get_BatteryFullLifetime(This,pVal) ) 

#define IBeamformerPowerState_get_BatteryLifePercent(This,pVal)	\
    ( (This)->lpVtbl -> get_BatteryLifePercent(This,pVal) ) 

#define IBeamformerPowerState_get_BatteryLifeRemaining(This,pVal)	\
    ( (This)->lpVtbl -> get_BatteryLifeRemaining(This,pVal) ) 

#define IBeamformerPowerState_get_PowerLineStatus(This,pVal)	\
    ( (This)->lpVtbl -> get_PowerLineStatus(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBeamformerPowerState_INTERFACE_DEFINED__ */


#ifndef __IUsgScanType_INTERFACE_DEFINED__
#define __IUsgScanType_INTERFACE_DEFINED__

/* interface IUsgScanType */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgScanType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF4959EF-AE06-414B-B290-675127F003D0")
    IUsgScanType : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgScanTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgScanType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgScanType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgScanType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgScanType * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgScanType * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgScanType * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgScanType * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgScanType * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgScanType * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgScanType * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgScanTypeVtbl;

    interface IUsgScanType
    {
        CONST_VTBL struct IUsgScanTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgScanType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgScanType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgScanType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgScanType_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgScanType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgScanType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgScanType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgScanType_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgScanType_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgScanType_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgScanType_INTERFACE_DEFINED__ */


#ifndef __IUsgSteeringAngle_INTERFACE_DEFINED__
#define __IUsgSteeringAngle_INTERFACE_DEFINED__

/* interface IUsgSteeringAngle */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgSteeringAngle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A0D966E1-6C45-44E3-9887-142DE307689A")
    IUsgSteeringAngle : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgSteeringAngleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgSteeringAngle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgSteeringAngle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgSteeringAngle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgSteeringAngle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgSteeringAngle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgSteeringAngle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgSteeringAngle * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgSteeringAngle * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgSteeringAngle * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgSteeringAngle * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgSteeringAngleVtbl;

    interface IUsgSteeringAngle
    {
        CONST_VTBL struct IUsgSteeringAngleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgSteeringAngle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgSteeringAngle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgSteeringAngle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgSteeringAngle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgSteeringAngle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgSteeringAngle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgSteeringAngle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgSteeringAngle_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgSteeringAngle_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgSteeringAngle_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgSteeringAngle_INTERFACE_DEFINED__ */


#ifndef __IUsgViewAngle_INTERFACE_DEFINED__
#define __IUsgViewAngle_INTERFACE_DEFINED__

/* interface IUsgViewAngle */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgViewAngle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D3CAA86B-8D04-4FFD-8F4E-F9E158051D5B")
    IUsgViewAngle : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgViewAngleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgViewAngle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgViewAngle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgViewAngle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgViewAngle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgViewAngle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgViewAngle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgViewAngle * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgViewAngle * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgViewAngle * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgViewAngle * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgViewAngleVtbl;

    interface IUsgViewAngle
    {
        CONST_VTBL struct IUsgViewAngleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgViewAngle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgViewAngle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgViewAngle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgViewAngle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgViewAngle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgViewAngle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgViewAngle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgViewAngle_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgViewAngle_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgViewAngle_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgViewAngle_INTERFACE_DEFINED__ */


#ifndef __IUsgCompoundFrames_INTERFACE_DEFINED__
#define __IUsgCompoundFrames_INTERFACE_DEFINED__

/* interface IUsgCompoundFrames */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCompoundFrames;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2CB1500C-8196-47FF-BECF-61947E0192C5")
    IUsgCompoundFrames : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCompoundFramesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCompoundFrames * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCompoundFrames * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCompoundFrames * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCompoundFrames * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCompoundFrames * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCompoundFrames * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCompoundFrames * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgCompoundFrames * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgCompoundFrames * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgCompoundFrames * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgCompoundFramesVtbl;

    interface IUsgCompoundFrames
    {
        CONST_VTBL struct IUsgCompoundFramesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCompoundFrames_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCompoundFrames_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCompoundFrames_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCompoundFrames_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCompoundFrames_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCompoundFrames_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCompoundFrames_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCompoundFrames_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgCompoundFrames_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgCompoundFrames_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCompoundFrames_INTERFACE_DEFINED__ */


#ifndef __IUsgCompoundAngle_INTERFACE_DEFINED__
#define __IUsgCompoundAngle_INTERFACE_DEFINED__

/* interface IUsgCompoundAngle */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgCompoundAngle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EEA419CB-8B31-47A3-A070-A68CEE24F3F1")
    IUsgCompoundAngle : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgCompoundAngleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgCompoundAngle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgCompoundAngle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgCompoundAngle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgCompoundAngle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgCompoundAngle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgCompoundAngle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgCompoundAngle * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgCompoundAngle * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgCompoundAngle * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgCompoundAngle * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgCompoundAngleVtbl;

    interface IUsgCompoundAngle
    {
        CONST_VTBL struct IUsgCompoundAngleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgCompoundAngle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgCompoundAngle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgCompoundAngle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgCompoundAngle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgCompoundAngle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgCompoundAngle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgCompoundAngle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgCompoundAngle_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgCompoundAngle_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgCompoundAngle_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgCompoundAngle_INTERFACE_DEFINED__ */


#ifndef __IUsgDeviceCapabilities_INTERFACE_DEFINED__
#define __IUsgDeviceCapabilities_INTERFACE_DEFINED__

/* interface IUsgDeviceCapabilities */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDeviceCapabilities;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("360D17D2-A12A-4bd0-8051-DA3CCBFBB9B7")
    IUsgDeviceCapabilities : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScanModeCaps( 
            /* [in] */ LONG Parameter,
            /* [in] */ ULONG ScanMode,
            /* [out] */ LONG *Capability,
            /* [retval][out] */ LONG *Support) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDeviceCapabilitiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDeviceCapabilities * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDeviceCapabilities * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDeviceCapabilities * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDeviceCapabilities * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDeviceCapabilities * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDeviceCapabilities * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDeviceCapabilities * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ScanModeCaps )( 
            IUsgDeviceCapabilities * This,
            /* [in] */ LONG Parameter,
            /* [in] */ ULONG ScanMode,
            /* [out] */ LONG *Capability,
            /* [retval][out] */ LONG *Support);
        
        END_INTERFACE
    } IUsgDeviceCapabilitiesVtbl;

    interface IUsgDeviceCapabilities
    {
        CONST_VTBL struct IUsgDeviceCapabilitiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDeviceCapabilities_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDeviceCapabilities_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDeviceCapabilities_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDeviceCapabilities_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDeviceCapabilities_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDeviceCapabilities_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDeviceCapabilities_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDeviceCapabilities_ScanModeCaps(This,Parameter,ScanMode,Capability,Support)	\
    ( (This)->lpVtbl -> ScanModeCaps(This,Parameter,ScanMode,Capability,Support) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDeviceCapabilities_INTERFACE_DEFINED__ */


#ifndef __IUsgUnits_INTERFACE_DEFINED__
#define __IUsgUnits_INTERFACE_DEFINED__

/* interface IUsgUnits */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgUnits;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E06602A1-EBE0-4E20-B39C-B938060434BE")
    IUsgUnits : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrent( 
            /* [in] */ IUnknown *pControlObject,
            /* [retval][out] */ USG_UNIT *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCurrent( 
            /* [in] */ IUnknown *pControlObject,
            /* [in] */ USG_UNIT *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValues( 
            /* [in] */ IUnknown *pControlObject,
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgUnitsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgUnits * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgUnits * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgUnits * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgUnits * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgUnits * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgUnits * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgUnits * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrent )( 
            IUsgUnits * This,
            /* [in] */ IUnknown *pControlObject,
            /* [retval][out] */ USG_UNIT *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCurrent )( 
            IUsgUnits * This,
            /* [in] */ IUnknown *pControlObject,
            /* [in] */ USG_UNIT *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValues )( 
            IUsgUnits * This,
            /* [in] */ IUnknown *pControlObject,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgUnitsVtbl;

    interface IUsgUnits
    {
        CONST_VTBL struct IUsgUnitsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgUnits_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgUnits_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgUnits_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgUnits_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgUnits_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgUnits_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgUnits_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgUnits_GetCurrent(This,pControlObject,pVal)	\
    ( (This)->lpVtbl -> GetCurrent(This,pControlObject,pVal) ) 

#define IUsgUnits_SetCurrent(This,pControlObject,pVal)	\
    ( (This)->lpVtbl -> SetCurrent(This,pControlObject,pVal) ) 

#define IUsgUnits_GetValues(This,pControlObject,pVal)	\
    ( (This)->lpVtbl -> GetValues(This,pControlObject,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgUnits_INTERFACE_DEFINED__ */


#ifndef __ISampleGrabberFilter_INTERFACE_DEFINED__
#define __ISampleGrabberFilter_INTERFACE_DEFINED__

/* interface ISampleGrabberFilter */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISampleGrabberFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4591F5BF-FBB2-4D6E-BDAD-E629E0635FFB")
    ISampleGrabberFilter : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FilterObj( 
            /* [in] */ LONG_PTR FilterIndex,
            /* [retval][out] */ IUnknown **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISampleGrabberFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISampleGrabberFilter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISampleGrabberFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISampleGrabberFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISampleGrabberFilter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISampleGrabberFilter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISampleGrabberFilter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISampleGrabberFilter * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FilterObj )( 
            ISampleGrabberFilter * This,
            /* [in] */ LONG_PTR FilterIndex,
            /* [retval][out] */ IUnknown **pVal);
        
        END_INTERFACE
    } ISampleGrabberFilterVtbl;

    interface ISampleGrabberFilter
    {
        CONST_VTBL struct ISampleGrabberFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISampleGrabberFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISampleGrabberFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISampleGrabberFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISampleGrabberFilter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISampleGrabberFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISampleGrabberFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISampleGrabberFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISampleGrabberFilter_get_FilterObj(This,FilterIndex,pVal)	\
    ( (This)->lpVtbl -> get_FilterObj(This,FilterIndex,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISampleGrabberFilter_INTERFACE_DEFINED__ */


#ifndef __IUsgWindowRF_INTERFACE_DEFINED__
#define __IUsgWindowRF_INTERFACE_DEFINED__

/* interface IUsgWindowRF */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgWindowRF;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5E2238EF-0DA8-48C4-84C5-E5E61194045D")
    IUsgWindowRF : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWindow( 
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVolume( 
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVolume( 
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LinesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WidthRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DepthsRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VolumesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgWindowRFVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgWindowRF * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgWindowRF * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgWindowRF * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgWindowRF * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgWindowRF * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgWindowRF * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgWindowRF * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWindow )( 
            IUsgWindowRF * This,
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IUsgWindowRF * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IUsgWindowRF * This,
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVolume )( 
            IUsgWindowRF * This,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IUsgWindowRF * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVolume )( 
            IUsgWindowRF * This,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LinesRange )( 
            IUsgWindowRF * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WidthRange )( 
            IUsgWindowRF * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DepthsRange )( 
            IUsgWindowRF * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VolumesRange )( 
            IUsgWindowRF * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        END_INTERFACE
    } IUsgWindowRFVtbl;

    interface IUsgWindowRF
    {
        CONST_VTBL struct IUsgWindowRFVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgWindowRF_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgWindowRF_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgWindowRF_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgWindowRF_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgWindowRF_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgWindowRF_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgWindowRF_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgWindowRF_SetWindow(This,startLine,endLine,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetWindow(This,startLine,endLine,startDepth,endDepth) ) 

#define IUsgWindowRF_GetWindow(This,startLine,endLine,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetWindow(This,startLine,endLine,startDepth,endDepth) ) 

#define IUsgWindowRF_SetWidth(This,startLine,endLine)	\
    ( (This)->lpVtbl -> SetWidth(This,startLine,endLine) ) 

#define IUsgWindowRF_SetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetVolume(This,startDepth,endDepth) ) 

#define IUsgWindowRF_GetWidth(This,startLine,endLine)	\
    ( (This)->lpVtbl -> GetWidth(This,startLine,endLine) ) 

#define IUsgWindowRF_GetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetVolume(This,startDepth,endDepth) ) 

#define IUsgWindowRF_get_LinesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_LinesRange(This,pVal) ) 

#define IUsgWindowRF_get_WidthRange(This,pVal)	\
    ( (This)->lpVtbl -> get_WidthRange(This,pVal) ) 

#define IUsgWindowRF_get_DepthsRange(This,pVal)	\
    ( (This)->lpVtbl -> get_DepthsRange(This,pVal) ) 

#define IUsgWindowRF_get_VolumesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_VolumesRange(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgWindowRF_INTERFACE_DEFINED__ */


#ifndef __IUsgStreamEnable_INTERFACE_DEFINED__
#define __IUsgStreamEnable_INTERFACE_DEFINED__

/* interface IUsgStreamEnable */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgStreamEnable;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33857397-F4BB-4B97-9776-39C50736F8BA")
    IUsgStreamEnable : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enable( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enable( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgStreamEnableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgStreamEnable * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgStreamEnable * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgStreamEnable * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgStreamEnable * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgStreamEnable * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgStreamEnable * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgStreamEnable * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enable )( 
            IUsgStreamEnable * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enable )( 
            IUsgStreamEnable * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IUsgStreamEnableVtbl;

    interface IUsgStreamEnable
    {
        CONST_VTBL struct IUsgStreamEnableVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgStreamEnable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgStreamEnable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgStreamEnable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgStreamEnable_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgStreamEnable_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgStreamEnable_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgStreamEnable_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgStreamEnable_get_Enable(This,pVal)	\
    ( (This)->lpVtbl -> get_Enable(This,pVal) ) 

#define IUsgStreamEnable_put_Enable(This,newVal)	\
    ( (This)->lpVtbl -> put_Enable(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgStreamEnable_INTERFACE_DEFINED__ */


#ifndef __IUsgDataSourceRF_INTERFACE_DEFINED__
#define __IUsgDataSourceRF_INTERFACE_DEFINED__

/* interface IUsgDataSourceRF */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgDataSourceRF;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3D9B6FC0-2AB7-4CFE-8B04-32D939F80DFE")
    IUsgDataSourceRF : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgDataSourceRFVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgDataSourceRF * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgDataSourceRF * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgDataSourceRF * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgDataSourceRF * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgDataSourceRF * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgDataSourceRF * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgDataSourceRF * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgDataSourceRF * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgDataSourceRF * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgDataSourceRF * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgDataSourceRFVtbl;

    interface IUsgDataSourceRF
    {
        CONST_VTBL struct IUsgDataSourceRFVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgDataSourceRF_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgDataSourceRF_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgDataSourceRF_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgDataSourceRF_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgDataSourceRF_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgDataSourceRF_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgDataSourceRF_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgDataSourceRF_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgDataSourceRF_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgDataSourceRF_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgDataSourceRF_INTERFACE_DEFINED__ */


#ifndef __IUsgMultiBeam_INTERFACE_DEFINED__
#define __IUsgMultiBeam_INTERFACE_DEFINED__

/* interface IUsgMultiBeam */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgMultiBeam;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5431298D-7FDF-439F-9D23-1DAFDE059F19")
    IUsgMultiBeam : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Current( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Values( 
            /* [retval][out] */ IUsgValues **pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgMultiBeamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgMultiBeam * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgMultiBeam * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgMultiBeam * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgMultiBeam * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgMultiBeam * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgMultiBeam * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgMultiBeam * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IUsgMultiBeam * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Current )( 
            IUsgMultiBeam * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Values )( 
            IUsgMultiBeam * This,
            /* [retval][out] */ IUsgValues **pVal);
        
        END_INTERFACE
    } IUsgMultiBeamVtbl;

    interface IUsgMultiBeam
    {
        CONST_VTBL struct IUsgMultiBeamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgMultiBeam_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgMultiBeam_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgMultiBeam_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgMultiBeam_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgMultiBeam_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgMultiBeam_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgMultiBeam_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgMultiBeam_get_Current(This,pVal)	\
    ( (This)->lpVtbl -> get_Current(This,pVal) ) 

#define IUsgMultiBeam_put_Current(This,newVal)	\
    ( (This)->lpVtbl -> put_Current(This,newVal) ) 

#define IUsgMultiBeam_get_Values(This,pVal)	\
    ( (This)->lpVtbl -> get_Values(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgMultiBeam_INTERFACE_DEFINED__ */


#ifndef __IUsgFrameROI_INTERFACE_DEFINED__
#define __IUsgFrameROI_INTERFACE_DEFINED__

/* interface IUsgFrameROI */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgFrameROI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("525055A7-B4AD-4A89-85E8-8FCCF51F9D38")
    IUsgFrameROI : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWindow( 
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVolume( 
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVolume( 
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LinesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WidthRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DepthsRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VolumesRange( 
            /* [retval][out] */ VALUE_STEPPING *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgFrameROIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgFrameROI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgFrameROI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgFrameROI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgFrameROI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgFrameROI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgFrameROI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgFrameROI * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWindow )( 
            IUsgFrameROI * This,
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IUsgFrameROI * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IUsgFrameROI * This,
            /* [in] */ LONG startLine,
            /* [in] */ LONG endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVolume )( 
            IUsgFrameROI * This,
            /* [in] */ LONG startDepth,
            /* [in] */ LONG endDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWidth )( 
            IUsgFrameROI * This,
            /* [out] */ LONG *startLine,
            /* [out] */ LONG *endLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVolume )( 
            IUsgFrameROI * This,
            /* [out] */ LONG *startDepth,
            /* [out] */ LONG *endDepth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LinesRange )( 
            IUsgFrameROI * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WidthRange )( 
            IUsgFrameROI * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DepthsRange )( 
            IUsgFrameROI * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VolumesRange )( 
            IUsgFrameROI * This,
            /* [retval][out] */ VALUE_STEPPING *pVal);
        
        END_INTERFACE
    } IUsgFrameROIVtbl;

    interface IUsgFrameROI
    {
        CONST_VTBL struct IUsgFrameROIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgFrameROI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgFrameROI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgFrameROI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgFrameROI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgFrameROI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgFrameROI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgFrameROI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgFrameROI_SetWindow(This,startLine,endLine,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetWindow(This,startLine,endLine,startDepth,endDepth) ) 

#define IUsgFrameROI_GetWindow(This,startLine,endLine,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetWindow(This,startLine,endLine,startDepth,endDepth) ) 

#define IUsgFrameROI_SetWidth(This,startLine,endLine)	\
    ( (This)->lpVtbl -> SetWidth(This,startLine,endLine) ) 

#define IUsgFrameROI_SetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> SetVolume(This,startDepth,endDepth) ) 

#define IUsgFrameROI_GetWidth(This,startLine,endLine)	\
    ( (This)->lpVtbl -> GetWidth(This,startLine,endLine) ) 

#define IUsgFrameROI_GetVolume(This,startDepth,endDepth)	\
    ( (This)->lpVtbl -> GetVolume(This,startDepth,endDepth) ) 

#define IUsgFrameROI_get_LinesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_LinesRange(This,pVal) ) 

#define IUsgFrameROI_get_WidthRange(This,pVal)	\
    ( (This)->lpVtbl -> get_WidthRange(This,pVal) ) 

#define IUsgFrameROI_get_DepthsRange(This,pVal)	\
    ( (This)->lpVtbl -> get_DepthsRange(This,pVal) ) 

#define IUsgFrameROI_get_VolumesRange(This,pVal)	\
    ( (This)->lpVtbl -> get_VolumesRange(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgFrameROI_INTERFACE_DEFINED__ */


#ifndef __IUsgProbeElementsTest_INTERFACE_DEFINED__
#define __IUsgProbeElementsTest_INTERFACE_DEFINED__

/* interface IUsgProbeElementsTest */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgProbeElementsTest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("06AF0C84-B1E3-411a-9363-E732D1659BC0")
    IUsgProbeElementsTest : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TestResult( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NumberOfElements( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetElementsTestResult( 
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ LONG *pStates) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgProbeElementsTestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgProbeElementsTest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgProbeElementsTest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgProbeElementsTest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgProbeElementsTest * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgProbeElementsTest * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgProbeElementsTest * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgProbeElementsTest * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TestResult )( 
            IUsgProbeElementsTest * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NumberOfElements )( 
            IUsgProbeElementsTest * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetElementsTestResult )( 
            IUsgProbeElementsTest * This,
            /* [in] */ LONG startIndex,
            /* [in] */ LONG entries,
            /* [out] */ LONG *pStates);
        
        END_INTERFACE
    } IUsgProbeElementsTestVtbl;

    interface IUsgProbeElementsTest
    {
        CONST_VTBL struct IUsgProbeElementsTestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgProbeElementsTest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgProbeElementsTest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgProbeElementsTest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgProbeElementsTest_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgProbeElementsTest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgProbeElementsTest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgProbeElementsTest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgProbeElementsTest_get_TestResult(This,pVal)	\
    ( (This)->lpVtbl -> get_TestResult(This,pVal) ) 

#define IUsgProbeElementsTest_get_NumberOfElements(This,pVal)	\
    ( (This)->lpVtbl -> get_NumberOfElements(This,pVal) ) 

#define IUsgProbeElementsTest_GetElementsTestResult(This,startIndex,entries,pStates)	\
    ( (This)->lpVtbl -> GetElementsTestResult(This,startIndex,entries,pStates) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgProbeElementsTest_INTERFACE_DEFINED__ */


#ifndef __IUsgTissueMotionDetector_INTERFACE_DEFINED__
#define __IUsgTissueMotionDetector_INTERFACE_DEFINED__

/* interface IUsgTissueMotionDetector */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IUsgTissueMotionDetector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7C9EB8CB-04DF-4F02-A254-E3F63EFED9B5")
    IUsgTissueMotionDetector : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DetectionResult( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Timeout( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Timeout( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableDetection( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableDetection( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUsgTissueMotionDetectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUsgTissueMotionDetector * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUsgTissueMotionDetector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUsgTissueMotionDetector * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUsgTissueMotionDetector * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUsgTissueMotionDetector * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUsgTissueMotionDetector * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUsgTissueMotionDetector * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DetectionResult )( 
            IUsgTissueMotionDetector * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Timeout )( 
            IUsgTissueMotionDetector * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Timeout )( 
            IUsgTissueMotionDetector * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EnableDetection )( 
            IUsgTissueMotionDetector * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EnableDetection )( 
            IUsgTissueMotionDetector * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } IUsgTissueMotionDetectorVtbl;

    interface IUsgTissueMotionDetector
    {
        CONST_VTBL struct IUsgTissueMotionDetectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUsgTissueMotionDetector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUsgTissueMotionDetector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUsgTissueMotionDetector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUsgTissueMotionDetector_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUsgTissueMotionDetector_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUsgTissueMotionDetector_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUsgTissueMotionDetector_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUsgTissueMotionDetector_get_DetectionResult(This,pVal)	\
    ( (This)->lpVtbl -> get_DetectionResult(This,pVal) ) 

#define IUsgTissueMotionDetector_get_Timeout(This,pVal)	\
    ( (This)->lpVtbl -> get_Timeout(This,pVal) ) 

#define IUsgTissueMotionDetector_put_Timeout(This,newVal)	\
    ( (This)->lpVtbl -> put_Timeout(This,newVal) ) 

#define IUsgTissueMotionDetector_get_EnableDetection(This,pVal)	\
    ( (This)->lpVtbl -> get_EnableDetection(This,pVal) ) 

#define IUsgTissueMotionDetector_put_EnableDetection(This,newVal)	\
    ( (This)->lpVtbl -> put_EnableDetection(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUsgTissueMotionDetector_INTERFACE_DEFINED__ */



#ifndef __Usgfw2Lib_LIBRARY_DEFINED__
#define __Usgfw2Lib_LIBRARY_DEFINED__

/* library Usgfw2Lib */
/* [helpstring][version][uuid] */ 


















































































/* [uuid] */ struct  DECLSPEC_UUID("52EA1133-C021-4c4a-8B32-C142447788DA") USG_UNIT_RANGE
    {
    /* [helpstring] */ tagUnitBase UnitBase;
    /* [helpstring] */ SCALE_FACTOR ScaleFactorMin;
    /* [helpstring] */ SCALE_FACTOR ScaleFactorMax;
    } ;
typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("5B051C8C-0261-3817-BE9B-362C908BEE58") 
enum tagUsgMixingBitmapFlags
    {
        UsgMixingBitmap_Disable	= 0x1,
        UsgMixingBitmap_hDC	= 0x2,
        UsgMixingBitmap_hBitmap	= 0x4,
        UsgMixingBitmap_SrcColorKey	= 0x8,
        UsgMixingBitmap_SrcRect	= 0x10,
        UsgMixingBitmap_FilterMode	= 0x20,
        UsgMixingBitmap_SrcPerPixelAlpha	= 0x40,
        UsgMixingBitmap_MixBitmap	= 0x10000000,
        UsgMixingBitmap_SplashBitmap	= 0x20000000
    } 	tagUsgMixingBitmapFlags;

typedef tagUsgMixingBitmapFlags UsgMixingBitmapFlags;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("3D264346-97C7-3888-B739-2FC8BB17DC82") 
enum tagScanMode
    {
        SCAN_MODE_UNKNOWN	= 0,
        SCAN_MODE_B	= 1,
        SCAN_MODE_M	= 2,
        SCAN_MODE_A	= 3,
        SCAN_MODE_PW	= 4,
        SCAN_MODE_BM	= 5,
        SCAN_MODE_BA	= 6,
        SCAN_MODE_BPW	= 7,
        SCAN_MODE_CFM	= 8,
        SCAN_MODE_BCFM	= 9,
        SCAN_MODE_BCFM_PW	= 10,
        SCAN_MODE_PDI	= 11,
        SCAN_MODE_BPDI	= 12,
        SCAN_MODE_BPDI_PW	= 13,
        SCAN_MODE_DPDI	= 14,
        SCAN_MODE_BDPDI	= 15,
        SCAN_MODE_BDPDI_PW	= 16,
        SCAN_MODE_B2	= 17,
        SCAN_MODE_PWAUDIO	= 18,
        SCAN_MODE_3D	= 19,
        SCAN_MODE_CW	= 20,
        SCAN_MODE_CM	= 21,
        SCAN_MODE_BM_CM	= 22,
        SCAN_MODE_M_CM	= 23,
        SCAN_MODE_RFB	= 24,
        SCAN_MODE_ELAST	= 25,
        SCAN_MODE_BELAST	= 26,
        SCAN_MODES_CUSTOM	= 1000
    } 	tagScanMode;

typedef tagScanMode SCAN_MODE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("638553AF-150E-3F68-B21D-A6B932984091") 
enum tagImageEnhancement
    {
        IMAGE_ENHANC_SHARPEN	= 1,
        IMAGE_ENHANC_SHARPENMORE	= 2,
        IMAGE_ENHANC_SMOOTH	= 3,
        IMAGE_ENHANC_SMOOTHMORE	= 4
    } 	tagImageEnhancement;

typedef tagImageEnhancement IMAGE_ENHANCEMENT;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("A1364206-1E78-3284-95E1-5038B4CD20DA") 
enum tagClearView
    {
        SRF_CV1	= 1,
        SRF_CV2	= 2,
        SRF_CV3	= 3,
        SRF_CV4	= 4,
        SRF_CV5	= 5,
        SRF_CV6	= 6,
        SRF_CV7	= 7,
        SRF_CV8	= 8,
        SRF_CV9	= 9,
        SRF_CV10	= 10,
        SRF_CV11	= 11,
        SRF_CV12	= 12,
        SRF_CV13	= 13,
        SRF_CV14	= 14,
        SRF_CV15	= 15,
        SRF_CV16	= 16,
        SRF_PV1	= 101,
        SRF_PV2	= 102,
        SRF_PV3	= 103,
        SRF_PV4	= 104,
        SRF_PV5	= 105,
        SRF_PV6	= 106,
        SRF_PV7	= 107,
        SRF_PV8	= 108,
        SRF_PV9	= 109,
        SRF_PV10	= 110,
        SRF_PV11	= 111,
        SRF_PV12	= 112,
        SRF_PV13	= 113,
        SRF_PV14	= 114,
        SRF_PV15	= 115,
        SRF_PV16	= 116,
        SRF_NV1	= 201,
        SRF_NV2	= 202,
        SRF_NV3	= 203,
        SRF_NV4	= 204,
        SRF_NV5	= 205,
        SRF_NV6	= 206,
        SRF_NV7	= 207,
        SRF_NV8	= 208,
        SRF_NV9	= 209,
        SRF_NV10	= 210,
        SRF_NV11	= 211,
        SRF_NV12	= 212,
        SRF_NV13	= 213,
        SRF_NV14	= 214,
        SRF_NV15	= 215,
        SRF_NV16	= 216,
        SRF_NVC1	= 301,
        SRF_NVC2	= 302,
        SRF_NVC3	= 303,
        SRF_NVC4	= 304,
        SRF_NVC5	= 305,
        SRF_NVC6	= 306,
        SRF_NVC7	= 307,
        SRF_NVC8	= 308,
        SRF_NVC9	= 309,
        SRF_NVC10	= 310,
        SRF_NVC11	= 311,
        SRF_NVC12	= 312,
        SRF_NVC13	= 313,
        SRF_NVC14	= 314,
        SRF_NVC15	= 315,
        SRF_NVC16	= 316,
        SRF_QV1	= 401,
        SRF_QV2	= 402,
        SRF_QV3	= 403,
        SRF_QV4	= 404,
        SRF_QV5	= 405,
        SRF_QV6	= 406,
        SRF_QV7	= 407,
        SRF_QV8	= 408,
        SRF_QV9	= 409,
        SRF_QV10	= 410,
        SRF_QV11	= 411,
        SRF_QV12	= 412,
        SRF_QV13	= 413,
        SRF_QV14	= 414,
        SRF_QV15	= 415,
        SRF_QV16	= 416,
        SRF_NVO1	= 501,
        SRF_NVO2	= 502,
        SRF_NVO3	= 503,
        SRF_NVO4	= 504,
        SRF_NVO5	= 505,
        SRF_NVO6	= 506,
        SRF_NVO7	= 507,
        SRF_NVO8	= 508,
        SRF_NVO9	= 509,
        SRF_NVO10	= 510,
        SRF_NVO11	= 511,
        SRF_NVO12	= 512,
        SRF_NVO13	= 513,
        SRF_NVO14	= 514,
        SRF_NVO15	= 515,
        SRF_NVO16	= 516
    } 	tagClearView;

typedef tagClearView CLEAR_VIEW;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("78CB2F44-C9E5-31BF-ABFF-A5BD251DA72D") 
enum tagLineDensity
    {
        LINE_DENSITY_LOW	= 8,
        LINE_DENSITY_MEDIUM_FAST	= 14,
        LINE_DENSITY_MEDIUM	= 16,
        LINE_DENSITY_STANDARD_FAST	= 22,
        LINE_DENSITY_STANDARD	= 24,
        LINE_DENSITY_HIGH	= 32
    } 	tagLineDensity;

typedef tagLineDensity LINE_DENSITY;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("14A99036-9440-3167-BADE-D274B9AD099B") 
enum tagFocusMode
    {
        FOCUS_MODE_MULTI	= 1,
        FOCUS_MODE_DYNAMIC	= 2
    } 	tagFocusMode;

typedef tagFocusMode FOCUS_MODE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("9761F566-3E4C-337E-A895-5C6D86749CC8") 
enum tagFileCreateFlags
    {
        FCF_OPEN	= 0,
        FCF_CREATE	= 1,
        FCF_RECREATE	= 2
    } 	tagFileCreateFlags;

typedef tagFileCreateFlags FILE_CREATE_FLAGS;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("7686B895-2578-35E0-94AA-C71B59556EE0") 
enum tagFileSaveFlags
    {
        FSF_ALL	= 0,
        FSF_CUSTOM_DATA_ONLY	= 1,
        FSF_RAW_DATA_ONLY	= 2
    } 	tagFileSaveFlags;

typedef tagFileSaveFlags FILE_SAVE_FLAGS;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("59CA167D-852A-33B0-AF40-5BFA63C97CBD") 
enum tagTgcControlMode
    {
        TGC_CTRLMODE_DEPTHFIXED	= 1,
        TGC_CTRLMODE_IMAGEFIXED	= 2
    } 	tagTgcControlMode;

typedef tagTgcControlMode TGC_CTRLMODE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("49C4C844-EBA1-3A8D-B476-C86B6C66FF69") 
enum tagScanType
    {
        SCAN_TYPE_UNKNOWN	= 0,
        SCAN_TYPE_NORMAL	= 1,
        SCAN_TYPE_VIRTUALCONVEX	= 2,
        SCAN_TYPE_COMPOUND	= 3,
        SCAN_TYPE_COMPOUND_SLOPLEFT	= 4,
        SCAN_TYPE_COMPOUND_SLOPRIGHT	= 5
    } 	tagScanType;

typedef tagScanType SCAN_TYPE;

typedef /* [v1_enum][uuid][public] */  DECLSPEC_UUID("5BF80B5B-C76B-3E38-ADD4-226D401A736F") 
enum tagScanModeCaps
    {
        MODECAPS_ScanType_Steering	= 0,
        MODECAPS_ScanType_Compound	= 1,
        MODECAPS_ScanType_VirtualConvex	= 2,
        MODECAPS_ScanType_CompoundSlop	= 3
    } 	tagScanModeCaps;

typedef tagScanModeCaps SCAN_MODE_CAPS;


EXTERN_C const IID LIBID_Usgfw2Lib;

EXTERN_C const CLSID CLSID_Usgfw2;

#ifdef __cplusplus

class DECLSPEC_UUID("4185D3A0-FC5F-42D8-8840-ED8E1D103C1F")
Usgfw2;
#endif

EXTERN_C const CLSID CLSID_UsgPaletteCalculator;

#ifdef __cplusplus

class DECLSPEC_UUID("994320B8-A50D-476F-AA50-50D0EFF76ED3")
UsgPaletteCalculator;
#endif

EXTERN_C const CLSID CLSID_UsgControl;

#ifdef __cplusplus

class DECLSPEC_UUID("25ACD2F1-5350-4EA8-AE84-67C19C7F692B")
UsgControl;
#endif

EXTERN_C const CLSID CLSID_UsgDataView;

#ifdef __cplusplus

class DECLSPEC_UUID("82CB5897-C237-42BB-BADD-F6C6966EE929")
UsgDataView;
#endif

EXTERN_C const CLSID CLSID_Usgfw2Debug;

#ifdef __cplusplus

class DECLSPEC_UUID("8E094E9D-FE44-431D-8881-6B308779304E")
Usgfw2Debug;
#endif
#endif /* __Usgfw2Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize(     unsigned long *, unsigned long            , HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal(  unsigned long *, unsigned char *, HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal(unsigned long *, unsigned char *, HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree(     unsigned long *, HBITMAP * ); 

unsigned long             __RPC_USER  HDC_UserSize(     unsigned long *, unsigned long            , HDC * ); 
unsigned char * __RPC_USER  HDC_UserMarshal(  unsigned long *, unsigned char *, HDC * ); 
unsigned char * __RPC_USER  HDC_UserUnmarshal(unsigned long *, unsigned char *, HDC * ); 
void                      __RPC_USER  HDC_UserFree(     unsigned long *, HDC * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize64(     unsigned long *, unsigned long            , HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal64(  unsigned long *, unsigned char *, HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal64(unsigned long *, unsigned char *, HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree64(     unsigned long *, HBITMAP * ); 

unsigned long             __RPC_USER  HDC_UserSize64(     unsigned long *, unsigned long            , HDC * ); 
unsigned char * __RPC_USER  HDC_UserMarshal64(  unsigned long *, unsigned char *, HDC * ); 
unsigned char * __RPC_USER  HDC_UserUnmarshal64(unsigned long *, unsigned char *, HDC * ); 
void                      __RPC_USER  HDC_UserFree64(     unsigned long *, HDC * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


