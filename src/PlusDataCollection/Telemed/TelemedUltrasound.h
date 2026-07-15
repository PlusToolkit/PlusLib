/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef TelemedUltrasound_h
#define TelemedUltrasound_h

#include "igsioCommon.h"
#include "Usgfw2.h"

// STL includes
#include <vector>

class TelemedUltrasound : public IUsgDeviceChangeSink
{
public:

  TelemedUltrasound();
  virtual ~TelemedUltrasound();

  PlusStatus Connect(int probeId = 0);
  void Disconnect();

  unsigned char* CaptureFrame();
  unsigned long GetBufferSize() {return m_FrameBuffer.size();}
  void GetFrameSize(FrameSizeType& frameSize) { frameSize = m_FrameSize; }

  /*! Must be called before connect to take effect */
  void SetMaximumFrameSize(const FrameSizeType& maxFrameSize);

  void FreezeDevice(bool freeze);

  PlusStatus GetFrequencyMhz(double& freqMHz);
  PlusStatus SetFrequencyMhz(double freqMHz);

  /*! Tissue harmonic imaging mode, a value of the Telemed THI_MODE enumeration: THI_MODE1 (conventional, non-harmonic imaging), THI_MODE2 (harmonic imaging), or THI_MODE2_ITHI (pulse inversion harmonic imaging) */
  PlusStatus GetThiMode(int& thiMode);
  PlusStatus SetThiMode(int thiMode);

  PlusStatus SetDepthMm(double depthMm);
  PlusStatus GetDepthMm(double& depthMm);

  PlusStatus SetGainPercent(double gainPercent);
  PlusStatus GetGainPercent(double& gainPercent);

  PlusStatus SetPowerDb(double powerDb);
  PlusStatus GetPowerDb(double& powerDb);

  PlusStatus SetFocusDepthPercent(double focusDepthPercent);
  PlusStatus GetFocusDepthPercent(double& focusDepthPercent);

  PlusStatus SetDynRangeDb(double dynRangeDb);
  PlusStatus GetDynRangeDb(double& dynRangeDb);

  /*! Set the time gain compensation [initial, mid, far gain] of B-mode ultrasound (valid range: 0-100) */
  PlusStatus SetTimeGainCompensation(const std::vector<double>& tgc);
  /*! Get the time gain compensation [initial, mid, far gain] of B-mode ultrasound (valid range: 0-100) */
  PlusStatus GetTimeGainCompensation(std::vector<double>& tgc);

  /*! Enable/disable speckle reduction (ClearView) filtering */
  PlusStatus SetSpeckleReductionEnabled(bool enabled);
  PlusStatus GetSpeckleReductionEnabled(bool& enabled);

  /*! Speckle reduction (ClearView) filter method, a value of the CLEAR_VIEW enumeration (SRF_...) */
  PlusStatus SetSpeckleReductionMethod(int method);
  PlusStatus GetSpeckleReductionMethod(int& method);

  PlusStatus SetDynamicFocusEnabled(bool enabled);
  PlusStatus GetDynamicFocusEnabled(bool& enabled);

  PlusStatus SetFocusesNumber(int focusesNumber);
  PlusStatus GetFocusesNumber(int& focusesNumber);

  PlusStatus SetFocusSet(int focusSet);
  PlusStatus GetFocusSet(int& focusSet);

  PlusStatus SetFrameAveraging(int frameAveraging);
  PlusStatus GetFrameAveraging(int& frameAveraging);

  PlusStatus SetViewAreaPercent(int viewAreaPercent);
  PlusStatus GetViewAreaPercent(int& viewAreaPercent);

  PlusStatus SetLineDensity(int lineDensity);
  PlusStatus GetLineDensity(int& lineDensity);

  PlusStatus SetImageEnhancementEnabled(bool enabled);
  PlusStatus GetImageEnhancementEnabled(bool& enabled);

  PlusStatus SetImageEnhancementMethod(int method);
  PlusStatus GetImageEnhancementMethod(int& method);

  PlusStatus SetRejection(int rejection);
  PlusStatus GetRejection(int& rejection);

  PlusStatus SetNegative(bool enabled);
  PlusStatus GetNegative(bool& enabled);

  PlusStatus SetScanDirection(bool changed);
  PlusStatus GetScanDirection(bool& changed);

  PlusStatus SetRotateImage(int degrees);
  PlusStatus GetRotateImage(int& degrees);

protected:
  std::vector<unsigned char> m_FrameBuffer;
  FrameSizeType m_FrameSize;
  FrameSizeType m_MaximumFrameSize;

private:
  IUsgfw2* m_usgfw2;
  IUsgDataView* m_data_view;
  IProbe* m_probe;
  IUsgMixerControl* m_mixer_control;
  IUsgDepth* m_depth_ctrl;
  IUsgPower* m_b_power_ctrl;
  IUsgGain* m_b_gain_ctrl;
  IUsgDynamicRange* m_b_dynrange_ctrl;
  IUsgProbeFrequency3* m_b_frequency_ctrl;
  IUsgFocus* m_b_focus_ctrl;
  IUsgClearView* m_b_clearview_ctrl;
  IUsgFrameAvg* m_b_frame_avg_ctrl;
  IUsgViewArea* m_b_view_area_ctrl;
  IUsgLineDensity* m_b_line_density_ctrl;
  IUsgImageEnhancement* m_b_image_enhancement_ctrl;
  IUsgRejection2* m_b_rejection_ctrl;
  IUsgImageOrientation* m_b_image_orientation_ctrl;
  IUsgPalette* m_b_palette_ctrl;
  IUsgTgc* m_b_tgc_ctrl;

  // Palette calculator is a standalone helper COM object (not bound to the data view/scan mode,
  // unlike the controls above) used to compute grayscale palette values (gamma/brightness/contrast/
  // negative), which must then be pushed to m_b_palette_ctrl to actually affect the live image.
  IUsgPaletteCalculator* m_palette_calculator;
  bool m_negative;

  IConnectionPoint* m_usg_device_change_cpnt; // connection point for device change events
  DWORD m_usg_device_change_cpnt_cookie;

  IConnectionPoint* m_usg_control_change_cpnt; // connection point for control change events
  DWORD m_usg_control_change_cpnt_cookie;

  void CreateUsgControl(IUsgDataView* data_view, const IID& type_id, ULONG scan_mode, ULONG stream_id, void** ctrl);
  void ReleaseUsgControls(bool release_usgfw2);

  long GetDepth();
  void DepthSetPrevNext(int dir);

  LPCWSTR GetInterfaceNameByGuid(BSTR ctrlGUID);
  LPCWSTR GetModeNameById(LONG scanMode);

private:
  HWND ImageWindowHandle;
  HBITMAP DataHandle;
  std::vector<unsigned char> MemoryBitmapBuffer;
  BITMAP Bitmap;

public:
  void CreateUsgControls(int probeId=0);

public:
  // IUnknown
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
  ULONG m_refCount;

  // IDispatch
  virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo);
  virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo);
  virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(const IID& riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
  virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, const IID& riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

  // IUsgCtrlChangeCommon
  virtual HRESULT STDMETHODCALLTYPE OnControlChanged(REFIID riidCtrl, ULONG scanMode, LONG streamId, IUsgControl* pControlObj, LONG dispId, LONG flags);
  virtual HRESULT STDMETHODCALLTYPE OnControlChangedBSTR(BSTR ctrlGUID, LONG scanMode, LONG streamId, IUsgControl* pControlObject, LONG dispId, LONG flags);

  // IUsgDeviceChangeSink
  virtual HRESULT STDMETHODCALLTYPE OnProbeArrive(IUnknown* pUsgProbe, ULONG* reserved);
  virtual HRESULT STDMETHODCALLTYPE OnBeamformerArrive(IUnknown* pUsgBeamformer, ULONG* reserved);
  virtual HRESULT STDMETHODCALLTYPE OnProbeRemove(IUnknown* pUsgProbe, ULONG* reserved);
  virtual HRESULT STDMETHODCALLTYPE OnBeamformerRemove(IUnknown* pUsgBeamformer, ULONG* reserved);
  virtual HRESULT STDMETHODCALLTYPE OnProbeStateChanged(IUnknown* pUsgProbe, ULONG* reserved);
  virtual HRESULT STDMETHODCALLTYPE OnBeamformerStateChanged(IUnknown* pUsgBeamformer, ULONG* reserved);
};

#endif //TelemedUltrasound