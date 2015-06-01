/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef TelemedUltrasound_h
#define TelemedUltrasound_h

#include "Usgfw2_h.h"
/*
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
*/

#include <vector>

class TelemedUltrasound : public IUsgDeviceChangeSink
{
public:

  TelemedUltrasound();
  virtual ~TelemedUltrasound();

  PlusStatus Connect();
  void Disconnect();

  unsigned char* CaptureFrame();
  unsigned long GetBufferSize() {return m_FrameBuffer.size();}
  unsigned int GetFrameHeight() {return m_FrameHeight;}
  unsigned int GetFrameWidth() {return m_FrameWidth;}

  void FreezeDevice(bool freeze);

  PlusStatus SetDepthMm(double depthMm);

  /*! Must be called before connect to take effect */
  void SetMaximumFrameSize(int maxFrameSize[2]);

protected:

  std::vector<unsigned char> m_FrameBuffer;
  unsigned int m_FrameHeight;
  unsigned int m_FrameWidth;

  int m_MaximumFrameSize[2];

private:
	HWND m_usg_wnd;
	IUsgfw2* m_usgfw2;
	IUsgDataView* m_data_view;
	IProbe* m_probe;
	IUsgMixerControl* m_mixer_control;
	IUsgDepth* m_depth_ctrl;
	IUsgGain* m_b_gain_ctrl;
	IUsgProbeFrequency3* m_b_frequency_ctrl;

	IConnectionPoint* m_usg_device_change_cpnt; // connection point for device change events
	DWORD m_usg_device_change_cpnt_cookie; 

	IConnectionPoint* m_usg_control_change_cpnt; // connection point for control change events
	DWORD m_usg_control_change_cpnt_cookie;

	void CreateUsgControl(IUsgDataView* data_view, const IID& type_id, ULONG scan_mode, ULONG stream_id, void** ctrl);
	void ReleaseUsgControls(bool release_usgfw2);

	long GetDepth();
	void DepthSetPrevNext(int dir);

  // thi = tissue harmonic imaging
	PlusStatus GetBModeFrequency(double &freqMHz, bool& thi);
	void B_FrequencySetPrevNext(int dir);

	PlusStatus GetBModeGain(long &val_cur, long &val_idx, long &val_count);
	void B_GainSetByIdx(int idx);

  LPCWSTR GetInterfaceNameByGuid(BSTR ctrlGUID);
  LPCWSTR GetModeNameById(LONG scanMode);

private:
//	DWORD m_create_usg_controls_thread_id;
//	HANDLE m_create_usg_controls_thread;

    HWND ImageWindowHandle;
    HBITMAP DataHandle;
    std::vector<unsigned char> MemoryBitmapBuffer;
    BITMAP Bitmap;
    BITMAPINFO	BitmapInfo;
    BYTE *RfDataBuffer;
    static const int samplesPerLine = 2048;

public:
	void CreateUsgControls();

public:
	// IUnknown
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	ULONG m_refCount;

	// IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo);
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(const IID &riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	// IUsgCtrlChangeCommon
	virtual HRESULT STDMETHODCALLTYPE OnControlChanged(REFIID riidCtrl, ULONG scanMode, LONG streamId, IUsgControl *pControlObj, LONG dispId, LONG flags);
	virtual HRESULT STDMETHODCALLTYPE OnControlChangedBSTR(BSTR ctrlGUID, LONG scanMode, LONG streamId, IUsgControl *pControlObject, LONG dispId, LONG flags);

	// IUsgDeviceChangeSink
	virtual HRESULT STDMETHODCALLTYPE OnProbeArrive(IUnknown* pUsgProbe, ULONG* reserved);
	virtual HRESULT STDMETHODCALLTYPE OnBeamformerArrive(IUnknown* pUsgBeamformer, ULONG* reserved);
	virtual HRESULT STDMETHODCALLTYPE OnProbeRemove(IUnknown* pUsgProbe, ULONG* reserved);
	virtual HRESULT STDMETHODCALLTYPE OnBeamformerRemove(IUnknown* pUsgBeamformer, ULONG* reserved);
	virtual HRESULT STDMETHODCALLTYPE OnProbeStateChanged(IUnknown* pUsgProbe, ULONG* reserved);
	virtual HRESULT STDMETHODCALLTYPE OnBeamformerStateChanged(IUnknown* pUsgBeamformer, ULONG* reserved);

 

};


#define RELEASE_INTERFACE(pObj) \
  if(pObj) {\
  pObj->Release(); pObj = NULL;}

#endif //TelemedUltrasound
