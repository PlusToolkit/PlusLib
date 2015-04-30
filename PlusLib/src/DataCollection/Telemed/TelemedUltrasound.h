/*=========================================================================

Program:   TelemedUltrasound
Module:
Language:  C++
Date:      $Date: 2005/06/06 21:51:37 $
Version:
Author:    Julien Jomier, Vincent Le Digarcher

Copyright (c) 2002 CADDLab @ UNC. All rights reserved.
See itkUNCCopyright.txt for details.

Copyright © 1992-2005, TELEMED Ltd.
Vilnius/Lithuania
http://www.telemed.lt/


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/



#ifndef TelemedUltrasound_h
#define TelemedUltrasound_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <initguid.h>
#include <strmif.h>
#include <Control.h>
#include <usgfw.h>
#include <usgfw2.h>
//#include "Usgfw2_h.h"
#include <multfreq.h>
#include <usgscanb.h>
#include <usgscanm.h>
#include "DeviceFilter.h"

#include <vector>

class TelemedUltrasound
{
public:

  enum {unknown_mode = 0, b_mode, bm_mode, m_mode};

  void OnProbeRemove();
  void OnBeamformerRemove();
  void ReleaseDevices();
  DeviceFilter* GetBeamformerForProbe(DeviceFilter *pProbeDevice);
  //void UpdateMenu(CCmdUI* pCmdUI);
  TelemedUltrasound();
  virtual ~TelemedUltrasound();

  DeviceFilter* GetFirstProbe();

  bool Initialize();
  bool Finalize();
  void ListAttachedProbes();

  bool SetFreqIndex(int nIndex);
  int GetFrequency(int nIndex);
  int GetCurrentFreqIndex();
  int GetFreqsNumber();
  bool IsProbeFreqSupported();
  void GetContrastRange(int &nMin, int &nMax);
  void GetBrightnessRange(int &nMin, int &nMax);
  void GetGammaRange(int& nMinGamma, int& nMaxGamma);
  int GetContrast();
  int GetBrightness();
  int GetGamma();
  void SetContrast(int contrast);
  void SetBrightness(int brightness);
  void SetGamma(int gamma);
  UINT GetScanMode();
  unsigned char* CaptureFrame();
  void Stop();
  void Pause();
  void Run();
  bool IsFreezed();
  void FreezeDevice(bool freeze);

  unsigned long GetBufferSize() {return m_BufferSize;}
  unsigned int GetFrameHeight() {return m_FrameHeight;}
  unsigned int GetFrameWidth() {return m_FrameWidth;}

  void ProgSize(int nSize);
  void ProgBeamsNumber(int nBeams);

  void SetGainValue(int GainPerCent);
  int GetGainPerCent();

  void SetPowerValue(int PowerPerCent);
  void SetDynRangeValue(int DynRangeValue);

  //void SetDepth(long nDepthValue);
  //long GetDepth();

/*
// B-mode command handler
void TelemedUltrasound::OnBmode();

void TelemedUltrasound::OnUpdateBmode(CCmdUI* pCmdUI);

// FREEZE command handler
void TelemedUltrasound::OnFreeze();

void TelemedUltrasound::OnUpdateFreeze(CCmdUI* pCmdUI);

// M-mode command handler
void TelemedUltrasound::OnMmode();

void TelemedUltrasound::OnUpdateMmode(CCmdUI* pCmdUI);
*/

protected:

  void OnNewProbe();
  void OnNewBeamformer();

  bool AssignProbe(DeviceFilter &DeviceFilter);
  HRESULT QueryInterfaceFromChain(REFIID iid, void** ppvObject);
  void InitializeObjects();

  unsigned long m_BufferSize;
  unsigned int m_FrameHeight;
  unsigned int m_FrameWidth;

  //typedef CTypedPtrArray<CPtrArray,IBaseFilter*> CBaseFiltersArray;
  typedef std::vector< IBaseFilter* > CBaseFiltersArray;

  std::vector< DeviceFilter* > m_ProbesArray;    // array of the pointers to the probes DeviceFilter objects
  std::vector< DeviceFilter* > m_BeamformersArray;  // array of the pointers to the transducers DeviceFilters objects
  IPin* FindPin(IBaseFilter *pBaseFilter, PIN_DIRECTION PinDir, const AM_MEDIA_TYPE *pMediaType = NULL);

  IMediaEventEx* m_pMediaEvent;    // this interface notifies about graph events
  IGraphBuilder *m_pGraphBuilder;
  IFilterGraph *m_pFilterGraph;
  IUsgProbeFrequency* m_pProbeFreq;
  std::vector< unsigned int > m_AvailableFrequencies;
  int m_nCurrentFrequency;

  UINT m_nScanMode;
  bool m_fFreeze;
  enum {b_stream = 0, m_stream,streams};

  CBaseFiltersArray m_StreamFilters[streams];

  void ProgPalette();
  int m_nContrast;
  int m_nBrightness;
  int m_nGamma;
  void DisconnectChains();
  bool CreateModeChain(UINT Mode);
  HRESULT CreateChain(IFilterGraph* pFilterGraph, IPin *pSourcePin, CBaseFiltersArray *pIntFiltersArray);

  // maybe use Usgfw2Lib:: before IUsgDepth
  //IUsgDepth* m_pDepth;
  IUsgBFormatControl* m_pFormatB;
  IUltrasonicPower* m_pUsgPower;
  IUltrasonicDynamicRange* m_pUsgDynRange;
  IUltrasonicGain* m_pUsgGain;

  CRITICAL_SECTION m_csSampleCallback;

private:
  void ReleaseObjects();
  bool m_fObjectsInitialized;
  DeviceFilter *m_pProbeDevice;
  DeviceFilter *m_pBeamformerDevice;

};


#define RELEASE_INTERFACE(pObj) \
  if(pObj) {\
  pObj->Release(); pObj = NULL;}

#endif //TelemedUltrasound
