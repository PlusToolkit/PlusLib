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





//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TelemedUltrasound.cpp: implementation of the TelemedUltrasound class.//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <comutil.h>
#include <stdio.h>

#include "TelemedUltrasound.h"
#define __STREAMS__
#include <ks.h>
#include <ksproxy.h>
#include <math.h>

// DirectShow SDK
#include <dshow.h>
#include <qedit.h>    // to declarate sample grabber

#define MAX_DEVICE_COMMANDS 16

#undef MAX
#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#undef MIN
#define MIN(a,b)  ((a) < (b) ? (a) : (b))


/** Constructor */
TelemedUltrasound::TelemedUltrasound()
{
  m_pBeamformerDevice = NULL;
  m_pProbeDevice = NULL;
  m_nScanMode = unknown_mode;
  m_fFreeze = false;
  m_fObjectsInitialized = false;
  m_pFilterGraph = NULL;
  m_pGraphBuilder = NULL;
  m_pMediaEvent = NULL;
  m_nGamma = 0;
  m_nBrightness = 0;
  m_nContrast = 0;
  m_pProbeFreq = NULL;
  m_nCurrentFrequency = -1;
  m_BufferSize = 0;
  m_FrameHeight = 0;
  m_FrameWidth = 0;
  CoInitialize(NULL);
  InitializeCriticalSection(&m_csSampleCallback);
}

/** Destructor */
TelemedUltrasound::~TelemedUltrasound()
{
  // If the objects haven't been released already
  if(m_pFilterGraph)
  {
    this->ReleaseObjects();
    CoUninitialize();
  }
}

/** Initialize the Ultrasound system */
bool TelemedUltrasound::Initialize()
{
  this->OnNewBeamformer();
  this->OnNewProbe();
  this->ListAttachedProbes();
  this->InitializeObjects();
  if(this->GetFirstProbe() == NULL) // If there is no probe connected (prevents from accessing to the value of a NULL pointer, when calling "AssignProbe()")
  {
    return false;
  }
  else if(!this->AssignProbe(*(this->GetFirstProbe())))
  {
    std::cout << "Cannot AssignProbe!" << std::endl;
    return false;
  }

  return true;
}

/** Finalize the Ultrasound system */
bool TelemedUltrasound::Finalize()
{
  // If the objects haven't been released already
  if(m_pFilterGraph)
  {
    ReleaseObjects();
    CoUninitialize();
  }
  return true;
}

/**
*  method enumerates all beamformars in the system
*  and adds new beamformers in the list
*/
void TelemedUltrasound::OnNewBeamformer()
{

  HRESULT hr;
  IBaseFilter* pBaseFilter = NULL;
  ICreateDevEnum* pSysDevEnum = NULL;
  hr = CoCreateInstance(CLSID_SystemDeviceEnum,
    NULL,CLSCTX_ALL,
    IID_ICreateDevEnum,
    (LPVOID*)&pSysDevEnum);
  if(FAILED(hr))
    return;
  IEnumMoniker* pEnumCat = NULL;
  hr = pSysDevEnum->CreateClassEnumerator(CLSID_UltrasonicBeamformer,&pEnumCat,0);
  RELEASE_INTERFACE(pSysDevEnum);
  if(hr != S_OK)
  {
    return;
  }

  IMoniker* pMoniker;
  ULONG cFetched;
  DeviceFilter* pFilter;
  while(pEnumCat->Next(1,&pMoniker,&cFetched) == S_OK)
  {
    LPOLESTR pName = NULL;
    if(pMoniker->GetDisplayName(NULL,NULL,&pName) != S_OK)
    {
      RELEASE_INTERFACE(pMoniker);
      continue;
    }
    // check if the device is already in the list
    int nItems = m_BeamformersArray.size();
    int i;
    for(i = 0; i < nItems; i++)
    {
      pFilter = m_BeamformersArray.at(i);
      if(!pFilter)
      {
        continue;
      }
      char name[20];
      WideCharToMultiByte( CP_ACP,WC_COMPOSITECHECK,pName,-1,name,sizeof(name),NULL,NULL );
      if(std::string(name).compare(pFilter->m_strFilterPath))
      {
        // if device path is the same - we have this device in the list
        break;
      }
    }
    if(i < nItems)
    {
      RELEASE_INTERFACE(pMoniker);
      CoTaskMemFree(pName);
      continue;
    }

    IPropertyBag* pPropBag;
    pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pPropBag);

    VARIANT varName;
    VariantInit(&varName);
    hr = pPropBag->Read(L"FriendlyName",&varName,0);
    if(SUCCEEDED(hr) && (varName.vt == VT_BSTR))
    {

      pFilter = new DeviceFilter();

      char filterPath[20];
      char *filterName;
      WideCharToMultiByte( CP_ACP,WC_COMPOSITECHECK,pName,-1,filterPath,sizeof(filterPath),NULL,NULL );
      filterName = _com_util::ConvertBSTRToString(varName.bstrVal);
      pFilter->m_strFilterPath = std::string(filterPath);
      pFilter->m_strFilterName = std::string(filterName);
      if(pMoniker->BindToObject(NULL,NULL,IID_IBaseFilter,(LPVOID*)&pFilter->m_pBaseFilter) == S_OK)
      {
        m_BeamformersArray.push_back(pFilter);
      }
      else
      {
        delete pFilter;
      }
      pFilter = NULL;
    }
    CoTaskMemFree(pName);
    VariantClear(&varName);


    RELEASE_INTERFACE(pPropBag);
    RELEASE_INTERFACE(pMoniker);
  }
  RELEASE_INTERFACE(pEnumCat);
}

/**
*  method enumerates all probes in the system
*  and adds new beamformers in the list
*/
void TelemedUltrasound::OnNewProbe()
{
  HRESULT hr;
  IBaseFilter* pBaseFilter = NULL;
  ICreateDevEnum* pSysDevEnum = NULL;
  hr = CoCreateInstance(CLSID_SystemDeviceEnum,
    NULL,CLSCTX_ALL,
    IID_ICreateDevEnum,
    (LPVOID*)&pSysDevEnum);
  if(FAILED(hr))
  {
    return;
  }
  IEnumMoniker* pEnumCat = NULL;
  hr = pSysDevEnum->CreateClassEnumerator(CLSID_UltrasonicTransducer,&pEnumCat,0);
  RELEASE_INTERFACE(pSysDevEnum);
  if(hr != S_OK)
  {
    return;
  }

  IMoniker* pMoniker;
  ULONG cFetched;
  DeviceFilter* pFilter;
  while(pEnumCat->Next(1,&pMoniker,&cFetched) == S_OK)
  {
    // get probe device path
    LPOLESTR pName = NULL;
    if(pMoniker->GetDisplayName(NULL,NULL,&pName) != S_OK)
    {
      RELEASE_INTERFACE(pMoniker);
      continue;
    }

    // check if this probe already is in our list
    int nItems = m_ProbesArray.size();
    int i;
    for(i = 0; i < nItems; i++)
    {
      pFilter = m_ProbesArray.at(i);
      if(!pFilter)
      {
        continue;
      }
      char name[20];
      WideCharToMultiByte( CP_ACP,WC_COMPOSITECHECK,pName,-1,name,sizeof(name),NULL,NULL );
      if(std::string(name).compare(pFilter->m_strFilterPath))
      {// strings are equal
        break;
      }
    }
    if(i < nItems)
    {
      RELEASE_INTERFACE(pMoniker);
      CoTaskMemFree(pName);
      continue;
    }

    IPropertyBag* pPropBag;
    pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pPropBag);

    VARIANT varName;
    VariantInit(&varName);
    hr = pPropBag->Read(L"FriendlyName",&varName,0);
    if(SUCCEEDED(hr) && (varName.vt == VT_BSTR))
    {

      pFilter = new DeviceFilter();

      char filterPath[20];
      char *filterName;
      WideCharToMultiByte( CP_ACP,WC_COMPOSITECHECK,pName,-1,filterPath,sizeof(filterPath),NULL,NULL );
      filterName = _com_util::ConvertBSTRToString(varName.bstrVal);
      pFilter->m_strFilterPath = std::string(filterPath);
      pFilter->m_strFilterName = std::string(filterName);
      if(pMoniker->BindToObject(NULL,NULL,IID_IBaseFilter,(LPVOID*)&pFilter->m_pBaseFilter) == S_OK)
      {
        m_ProbesArray.push_back(pFilter);
      }
      else
      {
        delete pFilter;
      }
      pFilter = NULL;
    }
    CoTaskMemFree(pName);
    VariantClear(&varName);

    // release interfaces
    RELEASE_INTERFACE(pPropBag);
    RELEASE_INTERFACE(pMoniker);
  }
  RELEASE_INTERFACE(pEnumCat);
}

/**
*  method Display the List of Attached Probes
*/
void TelemedUltrasound::ListAttachedProbes()
{
  std::cout << "ListAttachedProbes: " << std::endl;
  int nItems = m_ProbesArray.size();
  for (int i = 0; i < MIN(nItems,MAX_DEVICE_COMMANDS); i++)
  {
    DeviceFilter* pDeviceFilter = m_ProbesArray.at(i);
    if(!pDeviceFilter
      || !pDeviceFilter->m_pBaseFilter
      || pDeviceFilter->m_strFilterName.empty()
      || pDeviceFilter->m_strFilterPath.empty()
      )
      break;

    std::cout << i << ") " << pDeviceFilter->m_strFilterName << std::endl;
  }
}

/**
* Get the first Probe of The List
*/
DeviceFilter* TelemedUltrasound::GetFirstProbe()
{
  if(m_ProbesArray.size()<1)
  {
    std::cout << "GetFirstProbe : No Probe Found" << std::endl;
    return NULL;
  }

  return m_ProbesArray.at(0);
}

/**
* method adds probe strings into file menu of the application
*/
/*
void TelemedUltrasound::UpdateMenu(CCmdUI *pCmdUI)
{
  CMenu* pMenu = pCmdUI->m_pMenu;
  if (m_strOriginal.empty() && pMenu != NULL)
  pMenu->GetMenuString(pCmdUI->m_nID, m_strOriginal, MF_BYCOMMAND);

  if(m_ProbesArray.size() == 0)
  {
  if (!m_strOriginal.empty())
  pCmdUI->SetText(m_strOriginal);
  pCmdUI->Enable(FALSE);
  return;
  }

  if (pCmdUI->m_pMenu == NULL)
  return;

  int i;
  for (i = 0; i < MAX_DEVICE_COMMANDS; i++)
  pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + i, MF_BYCOMMAND);


  int nItems = m_ProbesArray.size();
  for (i = 0; i < MIN(nItems,MAX_DEVICE_COMMANDS); i++)
  {
  DeviceFilter* pDeviceFilter = m_ProbesArray.at(i);
  if(!pDeviceFilter
  || !pDeviceFilter->m_pBaseFilter
  || pDeviceFilter->m_strFilterName.empty()
  || pDeviceFilter->m_strFilterPath.empty()
  )
  break;



  UINT uFlags = MF_STRING | MF_BYPOSITION;


  POSITION pos = GetFirstDocPosition();
  while(pos)
  {
  TelemedUltrasound* pDocument = (TelemedUltrasound*)GetNextDoc(pos);
  if(pDocument->ContainEqualObject(pDeviceFilter->m_pBaseFilter))
  uFlags |= MF_CHECKED;

  };

  pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
  uFlags, pCmdUI->m_nID++,
  pDeviceFilter->m_strFilterName);
  }

  // update end menu count
  pCmdUI->m_nIndex--; // point to last menu added
  pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();

  pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}
*/


/**
*   Get The good Beamformer for the Probe
*/

DeviceFilter* TelemedUltrasound::GetBeamformerForProbe(DeviceFilter *pProbeDevice)
{
  DeviceFilter* pBeamformerDevice = NULL;
  do
  {
    if(!pProbeDevice || !pProbeDevice->m_pBaseFilter)
      break;


    IPin* pPin = FindPin(pProbeDevice->m_pBaseFilter,PINDIR_OUTPUT);
    if(!pPin)
      break;

    IKsPin* pKsPin;
    HRESULT hr = pPin->QueryInterface(IID_IKsPin,(LPVOID*)&pKsPin);
    RELEASE_INTERFACE(pPin);
    if(hr != S_OK)
      break;

    KSMULTIPLE_ITEM *pmiProbe;
    hr = pKsPin->KsQueryMediums(&pmiProbe);
    RELEASE_INTERFACE(pKsPin);
    if(hr != S_OK)
      break;

    int nProbeMediums = pmiProbe->Count;
    REGPINMEDIUM* pProbeMediums = (REGPINMEDIUM*)(pmiProbe + 1);
    if(!pmiProbe->Count)
    {
      CoTaskMemFree(pmiProbe);
      break;
    }



    int nItems = m_BeamformersArray.size();
    for(int i = 0; i < nItems; i++)
    {
      DeviceFilter* pDevice = m_BeamformersArray.at(i);
      if(!pDevice || !pDevice->m_pBaseFilter)
      {
        continue;
      }

      pPin = FindPin(pDevice->m_pBaseFilter,PINDIR_INPUT);
      if(!pPin)
      {
        continue;
      }

      hr = pPin->QueryInterface(IID_IKsPin,(LPVOID*)&pKsPin);
      if(hr != S_OK)
      {
        RELEASE_INTERFACE(pPin);
        continue;
      }

      KSMULTIPLE_ITEM *pmiBeamformer;
      hr = pKsPin->KsQueryMediums(&pmiBeamformer);
      RELEASE_INTERFACE(pKsPin);
      if(hr != S_OK)
      {
        RELEASE_INTERFACE(pPin);
        continue;
      }
      int nBeamformerMediums = pmiBeamformer->Count;
      REGPINMEDIUM* pBeamformerMediums = (REGPINMEDIUM*)(pmiBeamformer + 1);

      if(!nBeamformerMediums)
      {
        CoTaskMemFree(pmiBeamformer);
        RELEASE_INTERFACE(pPin);
        continue;
      }

      for(int j = 0; j < nBeamformerMediums; j++, pBeamformerMediums++)
      {
        if((pBeamformerMediums->clsMedium == pProbeMediums->clsMedium)
          && (pBeamformerMediums->dw1 == pProbeMediums->dw1))
        {
          FILTER_INFO* pInfo = new FILTER_INFO;
          ZeroMemory(pInfo,sizeof(FILTER_INFO));
          pDevice->m_pBaseFilter->QueryFilterInfo(pInfo);
          if(pInfo->pGraph)
          {
            delete pInfo;
          }
          else
          {
            delete pInfo;
            pBeamformerDevice = new DeviceFilter(*pDevice);
            break;
          }
        }
      }

      CoTaskMemFree(pmiBeamformer);
      RELEASE_INTERFACE(pPin);
      if(pBeamformerDevice)
      {
        break;
      }
    }


    CoTaskMemFree(pmiProbe);

  } while(false);
  return pBeamformerDevice;
}

/**
* Release the Devices
*/
void TelemedUltrasound::ReleaseDevices()
{
  int nItems = m_ProbesArray.size();
  int i;
  for(i = 0; i < nItems; i++)
  {
    DeviceFilter* pDeviceFilter = m_ProbesArray.at(i);
    if(pDeviceFilter)
      delete pDeviceFilter;
  }
  m_ProbesArray.clear();
  m_ProbesArray.clear();
  nItems = m_BeamformersArray.size();
  for(i = 0; i < nItems; i++)
  {
    DeviceFilter* pDeviceFilter = m_BeamformersArray.at(i);
    if(pDeviceFilter)
      delete pDeviceFilter;
  }
  m_BeamformersArray.clear();
}


/**
* Remove the beamformer
*/
void TelemedUltrasound::OnBeamformerRemove()
{

  HRESULT hr;
  IBaseFilter* pBaseFilter = NULL;
  ICreateDevEnum* pSysDevEnum = NULL;
  hr = CoCreateInstance(CLSID_SystemDeviceEnum,
    NULL,CLSCTX_ALL,
    IID_ICreateDevEnum,
    (LPVOID*)&pSysDevEnum);
  if(FAILED(hr))
    return;
  IEnumMoniker* pEnumCat = NULL;
  hr = pSysDevEnum->CreateClassEnumerator(CLSID_UltrasonicBeamformer,&pEnumCat,0);
  RELEASE_INTERFACE(pSysDevEnum);
  if(hr != S_OK)
  {
    return;
  }

  IMoniker* pMoniker;
  ULONG cFetched;
  DeviceFilter* pFilter;
  int nItems = m_BeamformersArray.size();
  for(int i = 0; i < nItems; i++)
  {
    pFilter = m_BeamformersArray.at(i);
    if(!pFilter)
      continue;
    pFilter->m_fChecked = false;
  }
  while(pEnumCat->Next(1,&pMoniker,&cFetched) == S_OK)
  {
    LPOLESTR pName = NULL;
    if(pMoniker->GetDisplayName(NULL,NULL,&pName) != S_OK)
    {
      RELEASE_INTERFACE(pMoniker);
      continue;
    }


    nItems = m_BeamformersArray.size();
    int i;
    for(i = 0; i < nItems; i++)
    {
      pFilter = m_BeamformersArray.at(i);
      if(!pFilter)
        continue;
      char name[20];
      WideCharToMultiByte( CP_ACP,WC_COMPOSITECHECK,pName,-1,name,sizeof(name),NULL,NULL );
      if(std::string(name).compare(pFilter->m_strFilterPath))
      {

        pFilter->m_fChecked = true;
        break;
      }
    }

    RELEASE_INTERFACE(pMoniker);
    CoTaskMemFree(pName);
  }
  RELEASE_INTERFACE(pEnumCat);

  while(true)
  {
    nItems = m_BeamformersArray.size();
    int i;
    for(i = 0; i < nItems; i++)
    {
      pFilter = m_BeamformersArray.at(i);
      if(!pFilter->m_fChecked)
      {
        m_BeamformersArray.erase(m_BeamformersArray.begin()+i);
        delete pFilter;
        break;
      }
    }
    if(i >= nItems)
    {
      break;
    }
  };
}

/**
*  Handle a remove of a Probe
*/
void TelemedUltrasound::OnProbeRemove()
{

  HRESULT hr;
  IBaseFilter* pBaseFilter = NULL;
  ICreateDevEnum* pSysDevEnum = NULL;
  hr = CoCreateInstance(CLSID_SystemDeviceEnum,
    NULL,CLSCTX_ALL,
    IID_ICreateDevEnum,
    (LPVOID*)&pSysDevEnum);
  if(FAILED(hr))
  {
    return;
  }
  IEnumMoniker* pEnumCat = NULL;
  hr = pSysDevEnum->CreateClassEnumerator(CLSID_UltrasonicTransducer,&pEnumCat,0);
  RELEASE_INTERFACE(pSysDevEnum);
  if(hr != S_OK)
  {
    return;
  }

  IMoniker* pMoniker;
  ULONG cFetched;
  DeviceFilter* pFilter;
  int nItems = m_ProbesArray.size();
  for(int i = 0; i < nItems; i++)
  {
    pFilter = m_ProbesArray.at(i);
    if(!pFilter)
    {
      continue;
    }
    pFilter->m_fChecked = false;
  }
  while(pEnumCat->Next(1,&pMoniker,&cFetched) == S_OK)
  {
    LPOLESTR pName = NULL;
    if(pMoniker->GetDisplayName(NULL,NULL,&pName) != S_OK)
    {
      RELEASE_INTERFACE(pMoniker);
      continue;
    }

    nItems = m_ProbesArray.size();
    int i;
    for(i = 0; i < nItems; i++)
    {
      pFilter = m_ProbesArray.at(i);
      if(!pFilter)
      {
        continue;
      }
      char name[20];
      WideCharToMultiByte( CP_ACP,WC_COMPOSITECHECK,pName,-1,name,sizeof(name),NULL,NULL );
      if(std::string(name).compare(pFilter->m_strFilterPath))
      {
        pFilter->m_fChecked = true;
        break;
      }
    }

    RELEASE_INTERFACE(pMoniker);
    CoTaskMemFree(pName);
  }
  RELEASE_INTERFACE(pEnumCat);

  while(true)
  {
    nItems = m_ProbesArray.size();
    int i;
    for(i = 0; i < nItems; i++)
    {
      pFilter = m_ProbesArray.at(i);
      if(!pFilter->m_fChecked)
      {
        m_ProbesArray.erase(m_ProbesArray.begin()+i);
        delete pFilter;
        break;
      }
    }
    if(i >= nItems)
    {
      break;
    }
  };
}



/**
*  the method enumerates filter pins and returns pointer to the pin with selected direction
*  if there are not such pins the method returns NULL
*/
IPin* TelemedUltrasound::FindPin(IBaseFilter *pBaseFilter, PIN_DIRECTION PinDir, const AM_MEDIA_TYPE *pMediaType)
{
  IEnumPins* pEnumPins;
  IPin* pPin = NULL;
  if(!pBaseFilter)
  {
    return pPin;
  }
  HRESULT hr = pBaseFilter->EnumPins(&pEnumPins);
  if(FAILED(hr))
  {
    return pPin;
  }
  while(pEnumPins->Next(1,&pPin,NULL) == S_OK)
  {
    PIN_DIRECTION pdPinDir;
    pPin->QueryDirection(&pdPinDir);
    if(PinDir == pdPinDir)
    {
      if(pMediaType)
      {
        // method was designed to check supported format
        // but for now we do not check is, just pin direction
        /*
        // ïðîâåðÿåì îñíîâíûå GUID
        IEnumMediaTypes* pEnumMediaTypes = NULL;
        hr = pPin->EnumMediaTypes(&pEnumMediaTypes);
        if(SUCCEEDED(hr))
        {
        AM_MEDIA_TYPE* pPinMediaType;
        bool fSuccess = false;
        while(pEnumMediaTypes->Next(1,&pPinMediaType,NULL) == S_OK)
        {
        if(IsEqualGUID(pMediaType->majortype,pPinMediaType->majortype)
        && IsEqualGUID(pMediaType->subtype,pPinMediaType->subtype)
        && IsEqualGUID(pMediaType->formattype, pPinMediaType->formattype))
        fSuccess = true;
        ::DeleteMediaType(pPinMediaType);
        if(fSuccess)
        break;
        }
        RELEASE_INTERFACE(pEnumMediaTypes);
        if(fSuccess)
        break;
        }
        */

      }
      else
      {
        break;
      }
    }
    RELEASE_INTERFACE(pPin);
  }
  RELEASE_INTERFACE(pEnumPins);
  return pPin;
}







////////////////////////////////
// TelemedUltrasound commands //
////////////////////////////////


/** method queries all filters and pins for indicated interface */

HRESULT TelemedUltrasound::QueryInterfaceFromChain(REFIID iid, void **ppvObject)
{

  HRESULT hResult = E_NOINTERFACE;
  do
  {
    if(!m_fObjectsInitialized)
    {
      break;
    }
    IEnumPins* pEnumPins;
    IPin* pPin;

    // check of probe filter and it's pin
    if(m_pProbeDevice && m_pProbeDevice->m_pBaseFilter)
    {
      hResult = m_pProbeDevice->m_pBaseFilter->QueryInterface(iid,ppvObject);
      if(hResult == S_OK)
      {
        break;
      }
      if(m_pProbeDevice->m_pBaseFilter->EnumPins(&pEnumPins) == S_OK)
      {
        while(pEnumPins->Next(1,&pPin,NULL) == S_OK)
        {
          hResult = pPin->QueryInterface(iid,ppvObject);
          RELEASE_INTERFACE(pPin);
          if(hResult == S_OK)
          {
            break;
          }
        }
        RELEASE_INTERFACE(pEnumPins);
        if(hResult == S_OK)
        {
          break;
        }
      }
    }

    // check of beamformer filter and it's pins
    if(m_pBeamformerDevice && m_pBeamformerDevice->m_pBaseFilter)
    {
      hResult = m_pBeamformerDevice->m_pBaseFilter->QueryInterface(iid,ppvObject);
      if(hResult == S_OK)
      {
        break;
      }
      if(m_pBeamformerDevice->m_pBaseFilter->EnumPins(&pEnumPins) == S_OK)
      {
        while(pEnumPins->Next(1,&pPin,NULL) == S_OK)
        {
          hResult = pPin->QueryInterface(iid,ppvObject);
          RELEASE_INTERFACE(pPin);
          if(hResult == S_OK)
          {
            break;
          }
        }
        RELEASE_INTERFACE(pEnumPins);
        if(hResult == S_OK)
        {
          break;
        }
      }
    }

    // check software filters and their pins
    for(int i = 0; i < streams; i++)
    {
      int nItems = m_StreamFilters[i].size();
      for(int j = 0; j < nItems; j++)
      {
        IBaseFilter *pBaseFilter = m_StreamFilters[i].at(j);
        hResult = pBaseFilter->QueryInterface(iid, ppvObject);
        if(hResult == S_OK)
        {
          break;
        }

        if(pBaseFilter->EnumPins(&pEnumPins) == S_OK)
        {
          while(pEnumPins->Next(1, &pPin, NULL) == S_OK)
          {
            hResult = pPin->QueryInterface(iid, ppvObject);
            RELEASE_INTERFACE(pPin);
            if(hResult == S_OK)
            {
              break;
            }
          }
          RELEASE_INTERFACE(pEnumPins);
          if(hResult == S_OK)
          {
            break;
          }
        }
      }
      if(hResult == S_OK)
      {
        break;
      }
    }

  } while(false);
  if(hResult != S_OK)
    *ppvObject = NULL;
  return hResult;
}

/** assigns the indicated probe to the document */
bool TelemedUltrasound::AssignProbe(DeviceFilter &deviceFilter)
{
  bool fRetValue = false;
  // first we need to release the previous objects
  if(m_pProbeDevice)
  {
    delete m_pProbeDevice;
  }

  if(m_pBeamformerDevice)
  {
    delete m_pBeamformerDevice;
  }
  m_pProbeDevice = NULL;
  m_pBeamformerDevice = NULL;

  // create copy of indicated probe device
  m_pProbeDevice = new DeviceFilter(deviceFilter);


  do
  {
    // get from template beamformer device that owns that probe
    m_pBeamformerDevice = this->GetBeamformerForProbe(m_pProbeDevice);
    if(!m_pBeamformerDevice)
    {
      break;
    }


    if(!m_pFilterGraph)
    {
      break;
    }

    m_pFilterGraph->AddFilter(m_pProbeDevice->m_pBaseFilter,NULL);
    m_pFilterGraph->AddFilter(m_pBeamformerDevice->m_pBaseFilter,NULL);

    IPin* pOutputPin = FindPin(m_pProbeDevice->m_pBaseFilter,PINDIR_OUTPUT);
    if(!pOutputPin)
    {
      break;
    }

    IPin* pInputPin = FindPin(m_pBeamformerDevice->m_pBaseFilter,PINDIR_INPUT);
    if(!pInputPin)
    {
      RELEASE_INTERFACE(pOutputPin);
      break;
    }

    // connect probe to beamformer
    HRESULT hr = m_pFilterGraph->ConnectDirect(pOutputPin, pInputPin, NULL);
    RELEASE_INTERFACE(pInputPin);
    RELEASE_INTERFACE(pOutputPin);

    if(hr != S_OK)
    {
      break;
    }

    // create chain for scanning in B-mode
    fRetValue = CreateModeChain(b_mode);
  } while(false);

  if(!fRetValue)
  {
    delete m_pProbeDevice;
    delete m_pBeamformerDevice;
    m_pProbeDevice = NULL;
    m_pBeamformerDevice = NULL;
  }
  else
  {
    m_nScanMode = b_mode;
    Pause();
  }

  // now we are trying to initialize multifrequency library
  do
  {
    m_AvailableFrequencies.clear();
    m_nCurrentFrequency = -1;
    if(!m_pProbeDevice)
    {
      break;
    }

    if(!m_pProbeFreq)
    {
      break;
    }

    std::string pProbeName = m_pProbeDevice->m_strFilterName;
    if(pProbeName.empty())
    {
      break;
    }

    LPOLESTR name = NULL;
    MultiByteToWideChar(CP_ACP,WC_COMPOSITECHECK,pProbeName.c_str(),sizeof(pProbeName),name,-1);
    HRESULT hr = m_pProbeFreq->SetProbe(name);

    if(hr != S_OK)
    {
      break;
    }

    // get the list of available frequencies for the probe
    long nFreqsNumber;
    hr = m_pProbeFreq->GetFrequenciesNumber(&nFreqsNumber);
    if(hr != S_OK)
    {
      // error getting number of supported frequencies
      // the method returns the following error codes
      // E_POINTER - if parameter is NULL (not our case)
      // E_NOINTERFACE - if probe name has not set (not our case too)
      // E_NOINTERFACE - if library don't support multifrequency feature for this probe.
      break;
    }

    // OK. Library supports multifrequency for the probe
    long nFetched = 0;
    ULONG *pFreqs = new ULONG[nFreqsNumber];
    hr = m_pProbeFreq->GetFrequenciesValues(0, nFreqsNumber, pFreqs, &nFetched);
    if(hr != S_OK)
    {
      delete [] pFreqs;
      break;
    }

    for(long i = 0; i < nFetched; i++)
    {
      m_AvailableFrequencies.push_back(*(pFreqs + i));
    }
    delete [] pFreqs;

    // set frequency for probe. In we can set any frequency from the
    // array of available values, but now we set the first one
    m_nCurrentFrequency = 0;
    if(m_pBeamformerDevice
      && m_pBeamformerDevice->m_pBaseFilter)
    {
      m_pProbeFreq->SetFrequency(m_AvailableFrequencies.at(m_nCurrentFrequency),m_pBeamformerDevice->m_pBaseFilter);
    }
  } while(false);

  m_fFreeze = true;

  return fRetValue;
}

/** To Get m_fFreeze */
bool TelemedUltrasound::IsFreezed()
{
  return m_fFreeze;
}

/** Initialize the Objects */
void TelemedUltrasound::InitializeObjects()
{
  if(m_fObjectsInitialized)
    return;

  ReleaseObjects();

  // create objects that we need to work in DirectShow
  do
  {
    IBaseFilter* pBaseFilter;
    HRESULT hr;

    // Filter graph
    hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC, IID_IGraphBuilder,(LPVOID*)&m_pGraphBuilder);
    if(FAILED(hr))
    {
      std::cout << "CoCreateInstance Failed" << std::endl;
      RELEASE_INTERFACE(m_pGraphBuilder);
      break;
    }

    // graph builder
    hr = m_pGraphBuilder->QueryInterface(IID_IFilterGraph,(LPVOID*)&m_pFilterGraph);

    if(FAILED(hr))
    {
      std::cout << "m_pGraphBuilder Failed" << std::endl;
      RELEASE_INTERFACE(m_pFilterGraph);
      m_pFilterGraph = NULL;
      break;
    }

    // media events
    hr = m_pFilterGraph->QueryInterface(IID_IMediaEventEx, reinterpret_cast<PVOID *>(&m_pMediaEvent));
    if(FAILED(hr))
    {
      std::cout << "m_pMediaEvent Failed" << std::endl;
      m_pMediaEvent = NULL;
    }
    else
    {
    }

    // scan-converter for B-mode
    hr = CoCreateInstance(CLSID_ScanConverterModeB,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(LPVOID*)&pBaseFilter);
    if(hr == S_OK)
    {
      hr = m_pFilterGraph->AddFilter(pBaseFilter,NULL);
      if(SUCCEEDED(hr))
      {
        m_StreamFilters[b_stream].push_back(pBaseFilter);
      }
      else
      {
        std::cout << "pBaseFilter Failed" << std::endl;
        RELEASE_INTERFACE(pBaseFilter);
      }
    }else
    {
      std::cout << "Failed1" << std::endl;
    }

    // sample grabber for B-mode
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL,CLSCTX_INPROC_SERVER ,IID_IBaseFilter, (LPVOID*)&pBaseFilter);
    if(hr == S_OK)
    {
      hr = m_pFilterGraph->AddFilter(pBaseFilter,NULL);
      if(SUCCEEDED(hr))
        m_StreamFilters[b_stream].push_back(pBaseFilter);
      else
      {
        std::cout << "pBaseFilter2 Failed" << std::endl;
        RELEASE_INTERFACE(pBaseFilter);
      }
    }else
    {
      std::cout << "Failed2" << std::endl;
    }

    hr = CoCreateInstance(CLSID_NullRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(LPVOID*)&pBaseFilter);
    if(hr == S_OK)
    {
      hr = m_pFilterGraph->AddFilter(pBaseFilter,NULL);
      if(SUCCEEDED(hr))
        m_StreamFilters[b_stream].push_back(pBaseFilter);
      else
      {
        RELEASE_INTERFACE(pBaseFilter);
        std::cout << "pBaseFilter3 Failed" << std::endl;
      }
    }else
    {
      std::cout << "Failed3" << std::endl;
    }

    // video renderer for B-mode
    /*hr = CoCreateInstance(CLSID_VideoRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(LPVOID*)&pBaseFilter);
    if(hr == S_OK)
    {
    hr = m_pFilterGraph->AddFilter(pBaseFilter,NULL);
    if(SUCCEEDED(hr))
    m_StreamFilters[b_stream].push_back(pBaseFilter);
    else
    {
    RELEASE_INTERFACE(pBaseFilter);
    std::cout << "pBaseFilter3 Failed" << std::endl;
    }
    }else
    {
      std::cout << "Failed3" << std::endl;
    }
    */
    /*
    // scan-converter for M-mode
    hr = CoCreateInstance(CLSID_ScanConverterModeM,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(LPVOID*)&pBaseFilter);
    if(hr == S_OK)
    {
    hr = m_pFilterGraph->AddFilter(pBaseFilter,NULL);
    if(SUCCEEDED(hr))
    m_StreamFilters[m_stream].push_back(pBaseFilter);
    else
    {
    RELEASE_INTERFACE(pBaseFilter);
    std::cout << "pBaseFilter4 Failed" << std::endl;
    }
    }else
    {
    std::cout << "Failed4" << std::endl;
    }

    // video renderer for M-mode
    hr = CoCreateInstance(CLSID_VideoRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(LPVOID*)&pBaseFilter);
    if(hr == S_OK)
    {
    hr = m_pFilterGraph->AddFilter(pBaseFilter,NULL);
    if(SUCCEEDED(hr))
    m_StreamFilters[m_stream].push_back(pBaseFilter);
    else
    {
    RELEASE_INTERFACE(pBaseFilter);
    std::cout << "pBaseFilter5 Failed" << std::endl;
    }
    }
    else
    {
    std::cout << "Failed5" << std::endl;
    }

    // multifrequency library
    hr = CoCreateInstance(CLSID_UsgProbeFrequency, NULL, CLSCTX_INPROC_SERVER, IID_IUsgProbeFrequency,(LPVOID*)&m_pProbeFreq);
    if(hr != S_OK)
    {
    m_pProbeFreq = NULL;
    std::cout << "m_pProbeFreq Failed" << std::endl;
    }
    */


  } while(false);

  // create sample grabber callback
  //m_pSampleGrabberCB = new CSampleGrabberCB;
  //m_pSampleGrabberCB->AddRef();

  // connect callback to sample grabber
  // find SampleGrabber filter in the array of the b-mode stream filters
  int nItems = m_StreamFilters[b_stream].size();
  ISampleGrabber* pSampleGrabber = NULL;
  for(int i = 0; i < nItems; i++)
  {
    IBaseFilter *pBaseFilter = m_StreamFilters[b_stream].at(i);
    if(!pBaseFilter)
      continue;

    HRESULT hr = pBaseFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&pSampleGrabber);
    if(hr != S_OK)
      continue;

    // SampleGrabber is found
    //hr = pSampleGrabber->SetCallback(m_pSampleGrabberCB,
    //  0);    // 0 means that we want to process 'SampleCB' method

    // Set one-shot mode and buffering.
    hr = pSampleGrabber->SetOneShot(TRUE);
    if(hr != S_OK)
    {
      std::cout<< "Cannot SetOneShot" <<std::endl;
      return;
    }

    pSampleGrabber->SetBufferSamples(TRUE);
    if(hr != S_OK)
    {
      std::cout<< "Cannot SetBufferSamples" <<std::endl;
      return;
    }

    RELEASE_INTERFACE(pSampleGrabber);
  }


  m_fObjectsInitialized = true;

}


/** ReleaseObjects */
void TelemedUltrasound::ReleaseObjects()
{
  // stop graph and release DirectShow objects

  IBaseFilter* pBaseFilter;
  Stop();

  int nItems = m_StreamFilters[b_stream].size();
  int i;
  for(i = 0; i < nItems; i++)
  {
    pBaseFilter = m_StreamFilters[b_stream].at(i);
    if(!pBaseFilter)
      continue;

    if(m_pFilterGraph)
      m_pFilterGraph->RemoveFilter(pBaseFilter);
    RELEASE_INTERFACE(pBaseFilter);
  }
  m_StreamFilters[b_stream].clear();

  nItems = m_StreamFilters[m_stream].size();
  for(i = 0; i < nItems; i++)
  {
    pBaseFilter = m_StreamFilters[m_stream].at(i);
    if(!pBaseFilter)
      continue;

    if(m_pFilterGraph)
      m_pFilterGraph->RemoveFilter(pBaseFilter);
    RELEASE_INTERFACE(pBaseFilter);
  }
  m_StreamFilters[m_stream].clear();

  if(m_pProbeDevice && m_pProbeDevice->m_pBaseFilter)
    m_pFilterGraph->RemoveFilter(m_pProbeDevice->m_pBaseFilter);

  if(m_pBeamformerDevice && m_pBeamformerDevice->m_pBaseFilter)
    m_pFilterGraph->RemoveFilter(m_pBeamformerDevice->m_pBaseFilter);

  RELEASE_INTERFACE(m_pMediaEvent);
  RELEASE_INTERFACE(m_pGraphBuilder);
  RELEASE_INTERFACE(m_pFilterGraph);
  RELEASE_INTERFACE(m_pProbeFreq);

  m_fObjectsInitialized = false;
}

/**
*  method creates chain in indicated filter graph
* chain is created from selected pin and contains from filters from pIntFilterArray
*/
HRESULT TelemedUltrasound::CreateChain(IFilterGraph* pFilterGraph, IPin *pSourcePin, CBaseFiltersArray *pIntFiltersArray)
{
  HRESULT hr = E_FAIL;
  do
  {
    if(!pSourcePin || !pIntFiltersArray || !pFilterGraph)
      break;

    IPin *pOutputPin = pSourcePin;
    IPin *pInputPin = NULL;
    pOutputPin->AddRef();
    int nItems = pIntFiltersArray->size();
    for(int i = 0; i < nItems; i++)
    {
      IBaseFilter *pBaseFilter = pIntFiltersArray->at(i);
      if(!pBaseFilter)
      {
        std::cout<< "!pBaseFilter" <<std::endl;
        continue;
      }
      pInputPin = FindPin(pBaseFilter,PINDIR_INPUT,NULL);
      if(!pInputPin)
      {
        std::cout<< "!pInputPin" <<std::endl;
        continue;
      }
      hr = pFilterGraph->ConnectDirect(pOutputPin, pInputPin, NULL);
      RELEASE_INTERFACE(pInputPin);
      RELEASE_INTERFACE(pOutputPin);
      if(hr != S_OK)
      {
        std::cout<< "pFilterGraph->ConnectDirect failed" <<std::endl;
        break;
      }

      pOutputPin = FindPin(pBaseFilter,PINDIR_OUTPUT,NULL);
      if(!pOutputPin)
      {
        break;
      }
    }

    if((hr == S_OK) && pOutputPin)
    {
      IGraphBuilder* pGraphBuilder = NULL;
      hr = pFilterGraph->QueryInterface(IID_IGraphBuilder,(LPVOID*)&pGraphBuilder);
      if(hr == S_OK)
      {
        hr = pGraphBuilder->Render(pOutputPin);
        RELEASE_INTERFACE(pGraphBuilder);
      }
      else
      {
        std::cout<< "!pFilterGraph->QueryInterface(IID_IGraphBuilder failed" <<std::endl;
      }
    }

    RELEASE_INTERFACE(pOutputPin);
    if(hr != S_OK)
    {
      // if creation failed we destroy the part of chain that we have created
      int i;
      for(i = 0; i < nItems; i++)
      {
        IBaseFilter *pBaseFilter = pIntFiltersArray->at(i);
        if(!pBaseFilter)
          continue;
        IEnumPins *pEnumPins = NULL;
        hr = pBaseFilter->EnumPins(&pEnumPins);
        if(FAILED(hr))
          continue;
        IPin *pin;
        while(pEnumPins->Next(1,&pin,NULL) == S_OK)
        {
          pFilterGraph->Disconnect(pin);
          RELEASE_INTERFACE(pin);
        }
        RELEASE_INTERFACE(pEnumPins);
      }
      hr = E_FAIL;
    }
    //    std::cout<< "End CreateChain" <<std::endl;
  } while(false);

  return hr;
}

// FREEZE command handler
void TelemedUltrasound::FreezeDevice(bool freeze)
{
  std::cout<< "TelemedUltrasound::FreezeDevice("<<freeze<<")" <<std::endl;
  if(freeze)
  {
    Pause();
  }
  else
  {
    Run();
  }
  m_fFreeze = freeze;
}


/*
// B-mode command handler
void TelemedUltrasound::OnBmode()
{
  std::cout<< "TelemedUltrasound::OnBmode" <<std::endl;
  // TODO: Add your command handler code here
  if(m_nScanMode == b_mode)
    return;

  Stop();
  DisconnectChains();
  switch(m_nScanMode)
  {
    case bm_mode:
    m_nScanMode = m_mode;
    break;
    case m_mode:
    m_nScanMode = bm_mode;
    break;
  }
  CreateModeChain(m_nScanMode);
  //UpdateAllViews(NULL);
  if(m_fFreeze)
    Pause();
  else
    Run();
}

void TelemedUltrasound::OnUpdateBmode(CCmdUI* pCmdUI)
{
  // TODO: Add your command update UI handler code here
  // std::cout<< "TelemedUltrasound::OnUpdateBmode" <<std::endl;
  pCmdUI->SetCheck(m_nScanMode == m_mode ? 0 : 1);
}

void TelemedUltrasound::OnUpdateFreeze(CCmdUI* pCmdUI)
{
  // TODO: Add your command update UI handler code here
  // std::cout<< "TelemedUltrasound::OnUpdateFreeze" <<std::endl;
  pCmdUI->SetCheck(m_fFreeze ? 1 : 0);
}

// M-mode command handler
void TelemedUltrasound::OnMmode()
{
  // TODO: Add your command handler code here
  std::cout<< "TelemedUltrasound::OnMmode" <<std::endl;
  if(m_nScanMode == m_mode)
  return;

  Stop();
  DisconnectChains();
  switch(m_nScanMode)
  {
    case bm_mode:
    m_nScanMode = b_mode;
    break;
    case b_mode:
    m_nScanMode = bm_mode;
    break;
  }
  CreateModeChain(m_nScanMode);
  //UpdateAllViews(NULL);
  if(m_fFreeze)
    Pause();
  else
    Run();
}

void TelemedUltrasound::OnUpdateMmode(CCmdUI* pCmdUI)
{
  // TODO: Add your command update UI handler code here
  // std::cout<< "TelemedUltrasound::OnUpdateMmode" <<std::endl;
  pCmdUI->SetCheck(m_nScanMode == b_mode ? 0 : 1);

}
*/

/** Pause */
void TelemedUltrasound::Pause()
{
  std::cout<< "TelemedUltrasound::Pause" <<std::endl;
  if(!m_pFilterGraph)
    return;
  IMediaControl* pMediaControl;
  HRESULT hr = m_pFilterGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
  if(hr != S_OK)
    return;
  hr = pMediaControl->Pause();
  RELEASE_INTERFACE(pMediaControl);
}


/** Run */

// Function added by Matthieu Heitz
void TelemedUltrasound::Run()
{
  std::cout<< "TelemedUltrasound::Run" <<std::endl;
  if(!m_pFilterGraph)
    return;
  IMediaControl* pMediaControl;
  HRESULT hr = m_pFilterGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
  if(hr != S_OK)
    return;
  hr = pMediaControl->Run();
  //long evCode;
  //m_pMediaEvent->WaitForCompletion(INFINITE, &evCode); // Wait till it's done.
  RELEASE_INTERFACE(pMediaControl);
}


/** Stop */
void TelemedUltrasound::Stop()
{
  std::cout<< "TelemedUltrasound::Stop" <<std::endl;
  if(!m_pFilterGraph)
    return;
  IMediaControl* pMediaControl;
  HRESULT hr = m_pFilterGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
  if(hr != S_OK)
    return;
  hr = pMediaControl->Stop();
  RELEASE_INTERFACE(pMediaControl);
}

/** Capture a Frame */
unsigned char* TelemedUltrasound::CaptureFrame()
{
  EnterCriticalSection(&m_csSampleCallback);

  if(!m_pFilterGraph)
  {
    std::cout << "CaptureFrame() : No m_pFilterGraph" << std::endl;
    return NULL;
  }

  IMediaControl* pMediaControl;
  HRESULT hr = m_pFilterGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
  if(hr != S_OK)
  {
    std::cout << "CaptureFrame() : cannot query IID_IMediaControl" << std::endl;
    return NULL;
  }
  hr = pMediaControl->Run();
  long evCode;
  m_pMediaEvent->WaitForCompletion(INFINITE, &evCode); // Wait till it's done.

  ISampleGrabber* pSampleGrabber = NULL;
  int nItems = m_StreamFilters[b_stream].size();
  for(int i = 0; i < nItems; i++)
  {
    IBaseFilter *pBaseFilter = m_StreamFilters[b_stream].at(i);
    if(!pBaseFilter)
      continue;

    hr = pBaseFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&pSampleGrabber);
    if(hr != S_OK)
      continue;

    // SampleGrabber is found
    // Find the required buffer size.
    long cbBuffer = 0;
    hr = pSampleGrabber->GetCurrentBuffer(&cbBuffer, NULL);

    //Allocate the array and call the method a second time to copy the buffer:

    unsigned char *pBuffer = new unsigned char[cbBuffer];
    if (!pBuffer)
    {
      std::cout << "CaptureFrame() : Cannot allocate buffer (" << cbBuffer << ")" << std::endl;
      // Out of memory. Return an error code.
    }

    hr = pSampleGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);

/*
    unsigned char *tab;
    tab=pBuffer;

    std::ofstream fichier_R("test_R.txt", std::ios::out | std::ios::trunc);  //déclaration du flux et ouverture du fichier
    std::ofstream fichier_G("test_G.txt", std::ios::out | std::ios::trunc);
    std::ofstream fichier_B("test_B.txt", std::ios::out | std::ios::trunc);

    if(fichier_R && fichier_G && fichier_B)  // si l'ouverture a réussi
    {
      // instructions
      fichier_R<<"Image R: "<<std::endl;
      fichier_G<<"Image G: "<<std::endl;
      fichier_B<<"Image B: "<<std::endl;
      for(int j=0;j<cbBuffer;j=j+3*2)
      {
        char destR[4],destG[4],destB[4];
        sprintf_s(destR,4,"%d",(int)tab[j]);
        sprintf_s(destG,4,"%d",(int)tab[j+1]);
        sprintf_s(destB,4,"%d",(int)tab[j+2]);
        fichier_R<<destR<<" ";
        fichier_G<<destG<<" ";
        fichier_B<<destB<<" ";
        if(j%512==0)
        {
            fichier_R<<std::endl;
            fichier_G<<std::endl;
            fichier_B<<std::endl;
        }
      }
      fichier_R.close();  // on referme le fichier
      fichier_G.close();  // on referme le fichier
      fichier_B.close();  // on referme le fichier

    }
    else  // sinon
      std::cerr << "Erreur à l'ouverture !" << std::endl;
    */


    AM_MEDIA_TYPE mt;
    hr = pSampleGrabber->GetConnectedMediaType(&mt);
    if (FAILED(hr))
    {
      // Return error code.
    }
    // Examine the format block.
    VIDEOINFOHEADER *pVih;
    if ((mt.formattype == FORMAT_VideoInfo) &&
      (mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
      (mt.pbFormat != NULL) )
    {
      pVih = (VIDEOINFOHEADER*)mt.pbFormat;
    }
    else
    {
      // Wrong format. Free the format block and return an error.
      //FreeMediaType(mt);
      return NULL;// VFW_E_INVALIDMEDIATYPE;
    }

    m_BufferSize = cbBuffer;
    m_FrameHeight = pVih->bmiHeader.biHeight;
    m_FrameWidth = pVih->bmiHeader.biWidth;

    LeaveCriticalSection(&m_csSampleCallback);
    return pBuffer;

    /*
    // Find the current bit depth.
    HDC hdc = GetDC(NULL);

    // You can use the media type to access the BITMAPINFOHEADER information.
    // For example, the following code draws the bitmap using GDI:
    SetDIBitsToDevice(
    hdc, 0, 0,
    pVih->bmiHeader.biWidth,
    pVih->bmiHeader.biHeight,
    0, 0,
    0,
    pVih->bmiHeader.biHeight,
    pBuffer,
    (BITMAPINFO*)&pVih->bmiHeader,
    DIB_RGB_COLORS
    );
    */
    // Free the format block when you are done:
    //FreeMediaType(mt);
    //}

    RELEASE_INTERFACE(pSampleGrabber);
  }


  RELEASE_INTERFACE(pMediaControl);
  std::cout<<"Before return NULL "<<std::endl;
  return NULL;
}

/** method creates graph chain for selected scan mode */
bool TelemedUltrasound::CreateModeChain(UINT Mode)
{
  bool RetValue = false;
  int i;
  do
  {
    if(!m_pProbeDevice || !m_pBeamformerDevice)
      break;

    // enumeration of beamformer output pins
    IPin* pOutputPin;
    IEnumPins *pEnumPins;
    HRESULT hr = m_pBeamformerDevice->m_pBaseFilter->EnumPins(&pEnumPins);
    if(FAILED(hr))
      break;

    bool fRoutedStream[streams];
    for(i=0; i < streams; i++)
      fRoutedStream[i] = false;

    while(pEnumPins->Next(1,&pOutputPin,NULL) == S_OK)
    {
      PIN_DIRECTION PinDir;
      pOutputPin->QueryDirection(&PinDir);
      if(PinDir != PINDIR_OUTPUT)
      {
        RELEASE_INTERFACE(pOutputPin);
        continue;
      }
      // for each output pin of the beamformer we try
      // to create chain
      for(i = 0; i < streams; i++)
      {
        switch(Mode)
        {
        case b_mode:
          if(i == m_stream)
            continue;
          break;
        case m_mode:
          if(i == b_stream)
            continue;
          break;
        case bm_mode:
          break;
        default:
          continue;
          break;
        }
        if(!fRoutedStream[i])
        {
          if(CreateChain(m_pFilterGraph,pOutputPin,&m_StreamFilters[i]) == S_OK)
          {
            fRoutedStream[i] = true;
          }
          else
          {
            std::cout<< "Cannot CreateChain" <<std::endl;
          }
        }
      }
      RELEASE_INTERFACE(pOutputPin);
    }

    RELEASE_INTERFACE(pEnumPins);
    for(i = 0; i < streams; i++)
      RetValue = RetValue || fRoutedStream[i];

  } while(false);
  return RetValue;
}


/**
* method disconnects all filters in the graph except connection between
* probe and beamformer filters. Graph must be in the stopped state when
* this method is called
*/
void TelemedUltrasound::DisconnectChains()
{
  std::cout<< "TelemedUltrasound::DisconnectChains" <<std::endl;
  // we just enumerate all pins for all filters in the graph
  // and disconnect them
  if(!m_pFilterGraph)
    return;

  do
  {
    if(!m_pBeamformerDevice || !m_pBeamformerDevice->m_pBaseFilter)
      break;

    HRESULT hr;
    IEnumPins* pEnumPins;
    hr = m_pBeamformerDevice->m_pBaseFilter->EnumPins(&pEnumPins);
    if(hr != S_OK)
      break;

    IPin* pOutputPin;
    while(pEnumPins->Next(1,&pOutputPin,NULL) == S_OK)
    {
      PIN_DIRECTION PinDir;
      pOutputPin->QueryDirection(&PinDir);
      if(PinDir != PINDIR_OUTPUT)
      {
        RELEASE_INTERFACE(pOutputPin);
        continue;
      }

      hr = m_pFilterGraph->Disconnect(pOutputPin);
      RELEASE_INTERFACE(pOutputPin);
    }


    RELEASE_INTERFACE(pEnumPins);

  } while(false);

  for(int i = 0; i < streams; i++)
  {
    int nItems = m_StreamFilters[i].size();
    for(int j = 0; j < nItems; j++)
    {
      IBaseFilter* pBaseFilter = m_StreamFilters[i].at(j);
      if(!pBaseFilter)
        continue;
      IEnumPins* pEnumPins;
      HRESULT hr = pBaseFilter->EnumPins(&pEnumPins);
      if(hr != S_OK)
        continue;
      IPin* pPin;
      while(pEnumPins->Next(1,&pPin,NULL) == S_OK)
      {
        hr = m_pFilterGraph->Disconnect(pPin);
        RELEASE_INTERFACE(pPin);
      }
      RELEASE_INTERFACE(pEnumPins);
    }
  }
}

/** Get the Scan Mode */
UINT TelemedUltrasound::GetScanMode()
{

  std::cout<< "TelemedUltrasound::GetScanMode" <<std::endl;
  return m_nScanMode;
}

/** Set the Gamma */
void TelemedUltrasound::SetGamma(int gamma)
{
  std::cout<< "TelemedUltrasound::SetGamma" <<std::endl;
  m_nGamma = MIN(50, MAX(gamma, -50));
  ProgPalette();

}

/** Set the Brightness */
void TelemedUltrasound::SetBrightness(int brightness)
{
  m_nBrightness = MIN(50, MAX(brightness, -50));
  ProgPalette();
}

/** Set the Contrast */
void TelemedUltrasound::SetContrast(int contrast)
{
  std::cout<< "TelemedUltrasound::SetContrast" <<std::endl;
  m_nContrast = MIN(50, MAX(contrast, -50));
  ProgPalette();
}

/** Get the Gamma */
int TelemedUltrasound::GetGamma()
{
  return m_nGamma;
}

/** Get the Brightness */
int TelemedUltrasound::GetBrightness()
{
  return m_nBrightness;
}

/** Get the Contrast */
int TelemedUltrasound::GetContrast()
{
  return m_nContrast;
}

/** Get the GammaRange */
void TelemedUltrasound::GetGammaRange(int &nMinGamma, int &nMaxGamma)
{
  nMinGamma = -50;
  nMaxGamma = 50;
}

/** Get the GammaRange */
void TelemedUltrasound::GetBrightnessRange(int &nMin, int &nMax)
{
  nMin = -50;
  nMax = 50;
}

/** Get the ContrastRange */
void TelemedUltrasound::GetContrastRange(int &nMin, int &nMax)
{
  nMin = -50;
  nMax = 50;
}


/*
* method implements creation of the gray palette with
* selected gamma, brightness and contrast values
*/
void TelemedUltrasound::ProgPalette()
{
  RECT rcPalette={0,0,255,255};

  std::cout<<"Entered ProgPalette"<<std::endl;

  double dblGamma = 1 + abs(m_nGamma) * 0.1;
  if(m_nGamma > 0.)
    dblGamma = 1. / dblGamma;

  rcPalette.right -= m_nContrast * 2;
  rcPalette.left += m_nContrast * 2;

  rcPalette.right -= m_nBrightness * 2;
  rcPalette.left -= m_nBrightness * 2;

  if(rcPalette.left < 0)
  {
    rcPalette.top = -rcPalette.left;
    rcPalette.left = 0;
  }
  if(rcPalette.right > 255)
  {
    rcPalette.bottom -= (rcPalette.right - 255);
    rcPalette.right = 255;
  }

  PPALETTEENTRY ppe = new PALETTEENTRY[256];
  int i;
  for(i = 0; i < MIN(256,rcPalette.left); i++)
  {
    ppe[i].peBlue =
      ppe[i].peGreen =
      ppe[i].peRed = (BYTE)rcPalette.top;
    ppe[i].peFlags = 0;
  }
  for(; i < MAX(256,rcPalette.right); i++)
  {
    ppe[i].peBlue =
      ppe[i].peGreen =
      ppe[i].peRed = (BYTE)(rcPalette.top + pow((double)(i - rcPalette.left)/(double)(rcPalette.right-rcPalette.left),dblGamma) * (double)(rcPalette.top-rcPalette.bottom));
    ppe[i].peFlags = 0;
  }

  for(; i < 256; i++)
  {
    ppe[i].peBlue =
      ppe[i].peGreen =
      ppe[i].peRed = (BYTE)rcPalette.bottom;
    ppe[i].peFlags = 0;
  }


  IUltrasonicPalette *pPalette;
  if(QueryInterfaceFromChain(IID_IUltrasonicPalette, (LPVOID*)&pPalette) == S_OK)
  {
    pPalette->SetUsgPalette(0,256,ppe);
    RELEASE_INTERFACE(pPalette);
  }
  else
  {
    std::cout<<"Error Could not QueryInterfaceFromChain for UltrasonicPalette"<<std::endl;
  }

  delete [] ppe;

  std::cout<<"exit ProgPalette"<<std::endl;

}

/** IsProbeFreqSupported */
bool TelemedUltrasound::IsProbeFreqSupported()
{
  if(!m_pProbeDevice)
    return false;
  if(m_AvailableFrequencies.size() == 0)
    return false;
  return true;
}

/** GetFreqsNumber */
int TelemedUltrasound::GetFreqsNumber()
{
  return m_AvailableFrequencies.size();
}

/** Get the Current Freq Index */
int TelemedUltrasound::GetCurrentFreqIndex()
{
  return m_nCurrentFrequency;
}

/** Get the  Frequency */
int TelemedUltrasound::GetFrequency(int nIndex)
{
  int nItems = m_AvailableFrequencies.size();
  if((nIndex >= 0)
    && (nIndex < nItems))
    return m_AvailableFrequencies[nIndex];

  return 0;
}

/** Set the  Frequency Index*/
bool TelemedUltrasound::SetFreqIndex(int nIndex)
{
  if(!m_pProbeDevice)
    return false;
  if(!m_pBeamformerDevice)
    return false;
  if(!m_pProbeFreq)
    return false;

  int nItems = m_AvailableFrequencies.size();
  if(!nItems)
    return false;

  if((nIndex < 0)
    || (nIndex >= nItems))
    return false;

  return m_pProbeFreq->SetFrequency(m_AvailableFrequencies[nIndex],m_pBeamformerDevice->m_pBaseFilter) == S_OK;

}


/**
* method calculates format of B-pin to get indicated scan depth
* the parameter is desired scan depth in cm
*/
void TelemedUltrasound::ProgSize(int nSize)
{
  //std::cout<<"Entered ProgSize"<<std::endl;

  this->QueryInterfaceFromChain(IID_IUsgBFormatControl, (LPVOID*)&m_pFormatB);

  if(!m_pFormatB)
    return;
  AM_MEDIA_TYPE* pFormat;

  PBULTRASOUND_STREAM_CONFIG_CAPS pCaps = new BULTRASOUND_STREAM_CONFIG_CAPS;

  HRESULT hr = m_pFormatB->GetStreamCaps(&pFormat,pCaps);
  if(SUCCEEDED(hr))
    m_pFormatB->ReleaseFormat(pFormat);
  hr = m_pFormatB->GetCurrentFormat(&pFormat);

  PBULTRASOUND_INFO pFrameInfo = (PBULTRASOUND_INFO)pFormat->pbFormat;

  // calculate time to receive one ultrasonic point in ns
  double time = 2. * (double)nSize * 10. * 1000. / 1.54 / pCaps->MaxInputPoints;

  // align time per point according format capabilities
  int nCoeff = (int)ceil((time - (double)pCaps->MinPointTime) / (double)pCaps->CropPointTimeGranuality);
  pFrameInfo->PointTime =
    pFrameInfo->UsgFrameInfoHeader.TimePerPoint = nCoeff * pCaps->CropPointTimeGranuality + pCaps->MinPointTime;

  // now we need to calculate how many points correspond indicated scan depth
  DWORD nPoints = (int)ceil(2 * (double)nSize * 10. * 1000. / 1.54 / pFrameInfo->PointTime);
  nPoints = MIN(pCaps->MaxInputPoints, MAX(nPoints, pCaps->MinInputPoints));

  pFrameInfo->UsgFrameInfoHeader.PointsPerBeam = nPoints;
  pFrameInfo->rcSource.left =
    pFrameInfo->rcTarget.left = 0;
  pFrameInfo->rcSource.right =
    pFrameInfo->rcTarget.right = nPoints;


  pFrameInfo->rcSource.right -= pFrameInfo->rcSource.left;
  pFrameInfo->rcSource.left = 0;

  hr = m_pFormatB->SetCurrentFormat(pFormat);

  m_pFormatB->ReleaseFormat(pFormat);
  delete pCaps;

}



/** the method sets the number of beams in the ultrasonic frame */
void TelemedUltrasound::ProgBeamsNumber(int nBeams)
{

  this->QueryInterfaceFromChain(IID_IUsgBFormatControl, (LPVOID*)&m_pFormatB);

  if(!m_pFormatB)
    return;
  AM_MEDIA_TYPE* pFormat;

  PBULTRASOUND_STREAM_CONFIG_CAPS pCaps = new BULTRASOUND_STREAM_CONFIG_CAPS;

  HRESULT hr = m_pFormatB->GetStreamCaps(&pFormat,pCaps);
  if(SUCCEEDED(hr))
    m_pFormatB->ReleaseFormat(pFormat);
  hr = m_pFormatB->GetCurrentFormat(&pFormat);

  PBULTRASOUND_INFO pFrameInfo = (PBULTRASOUND_INFO)pFormat->pbFormat;

  // check beams amount
  // checking crop granuality not shown
  nBeams = MAX(pCaps->MinInputBeams, MIN(pCaps->MaxInputBeams, (DWORD)nBeams));

  pFrameInfo->UsgFrameInfoHeader.BeamsPerFrame = nBeams;
  pFrameInfo->UsgFrameInfoHeader.FrameSize = pFrameInfo->UsgFrameInfoHeader.Pitch * pFrameInfo->UsgFrameInfoHeader.BeamsPerFrame;
  pFrameInfo->rcTarget.top = 0;
  pFrameInfo->rcTarget.bottom = nBeams;

  // we don't control the first input beam position, so center beams.

  pFrameInfo->rcSource.top = MAX(0,pCaps->MaxInputBeams - pFrameInfo->UsgFrameInfoHeader.BeamsPerFrame) / 2;
  pFrameInfo->rcSource.bottom = pFrameInfo->rcSource.top + pFrameInfo->UsgFrameInfoHeader.BeamsPerFrame;


  hr = m_pFormatB->SetCurrentFormat(pFormat);

  m_pFormatB->ReleaseFormat(pFormat);
  delete pCaps;
}

/** Set GainValue  */
void TelemedUltrasound::SetGainValue(int GainPerCent)
{
  if(GainPerCent>100)
  {
    std::cerr<<"SetGainValue is over 100, should be between 0 and 100"<<std::endl;
  }
  else if(GainPerCent<0)
  {
    std::cerr<<"SetGainValue is under 0, should be between 0 and 100"<<std::endl;
  }
  else
  {
    this->QueryInterfaceFromChain(IID_IUltrasonicGain,(LPVOID*)&m_pUsgGain);

    // get gain minimum and maximum values
    VALUE_RANGE ValueRange;
    m_pUsgGain->GetGainRange(&ValueRange);
    int nGainValue ;
    nGainValue = (GainPerCent*(ValueRange.Maximum - ValueRange.Minimum)/100) + ValueRange.Minimum;

    m_pUsgGain->SetGain((PLONG)&nGainValue);
  }
}

/** Get the Per Cent of the Gain */
int TelemedUltrasound::GetGainPerCent()
{
  LONG nGain;
  this->QueryInterfaceFromChain(IID_IUltrasonicGain,(LPVOID*)&m_pUsgGain);
  m_pUsgGain->GetGain((PLONG)&nGain);

  // get gain minimum and maximum values
  VALUE_RANGE ValueRange;

  m_pUsgGain->GetGainRange(&ValueRange);

  nGain = (nGain - ValueRange.Minimum) * 100 / (ValueRange.Maximum - ValueRange.Minimum);

  return nGain;
}

/** Set the Power */
void TelemedUltrasound::SetPowerValue(int PowerPerCent)
{
  if(PowerPerCent>100)
  {
    std::cerr<<"PowerValue is over 100, should be between 0 and 100"<<std::endl;
  }
  else if(PowerPerCent<0)
  {
    std::cerr<<"PowerValue is under 0, should be between 0 and 100"<<std::endl;
  }
  else
  {
    this->QueryInterfaceFromChain(IID_IUltrasonicPower,(LPVOID*)&m_pUsgPower);

    // get power minimum and maximum values
    VALUE_RANGE ValueRange;
    m_pUsgPower->GetPowerRange(&ValueRange);
    int nPowerValue ;
    nPowerValue = (PowerPerCent*(ValueRange.Maximum - ValueRange.Minimum)/100) + ValueRange.Minimum;

    m_pUsgPower->SetPower((PLONG)&nPowerValue);
  }
}

/** Set the Dynamic Range Value */
void TelemedUltrasound::SetDynRangeValue(int DynRangeValue)
{
  if(DynRangeValue > 80 || DynRangeValue < 38 || (DynRangeValue-38)%6 != 0)
  {
    std::cerr<<"Wrong Dynamic Range value, should be between 38 and 80, by steps of 6. Correct values : 38, 44, 50, 56, 62, 68, 74, 80"<<std::endl;
  }
  else
  {
    this->QueryInterfaceFromChain(IID_IUltrasonicDynamicRange,(LPVOID*)&m_pUsgDynRange);

    int nDynRangeValue;
    nDynRangeValue = ((DynRangeValue-38)/6);

    m_pUsgDynRange->SetDRange((PLONG)&nDynRangeValue);
  }
}

// To add these function, we have to include Usgfw2_h.h, which is not in the include dir of the Usgfw SDK. It's situated in samples_cpp_vs2005\save_to_file\sample

//void TelemedUltrasound::SetDepth(double nDepthValue)
//{
//  HRESULT hr = m_pDepth->put_Current(nDepthValue);
//  if(hr != S_OK)
//  {
//    std::cout<<"Impossible to set Depth"<<std::endl;
//    break;
//  }
//}
//
//double TelemedUltrasound::GetDepth()
//{
//  double *nDepthValue;
//  HRESULT hr = m_pDepth->get_Current(nDepthValue);
//  if(hr != S_OK)
//  {
//    std::cout<<"Impossible to get Depth"<<std::endl;
//    break;
//  }
//  return *nDepthValue;
//}

