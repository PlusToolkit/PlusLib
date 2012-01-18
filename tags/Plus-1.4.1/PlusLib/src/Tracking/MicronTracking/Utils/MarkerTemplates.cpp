#include "MarkerTemplates.h"

MarkerTemplates::MarkerTemplates(_MarkersPtr markers)
{
  this->m_markers = markers;
}

/****************************/
MarkerTemplates::~MarkerTemplates()
{
  if (this->m_markers != NULL)
  {
    delete this->m_markers;
    this->m_markers = NULL;
  }
//  this->m_markers->Release();
}

/****************************/
int MarkerTemplates::refreshTemplates(vector<string> &tmplsNameStrings,
                    vector<string> &tmplsErrorStrings,
                    vector<string> &tmplsWarnStrings)
{
  this->m_vTemplateNames.clear();
  this->m_vTemplateNames.resize(0);

  //If error in loading the files insided the Markers folder, return 0
  if (mti_utils::getFileNamesFromDirectory( m_vTemplateNames,"Markers", TRUE) == 0)
    return 0;

  HRESULT hr = this->m_markers->ClearTemplates();
  if (SUCCEEDED(hr))
  {
    for ( int i = 0 ; i < m_vTemplateNames.size() ; i++ )
    {
      _MarkerPtr loadedMarker(__uuidof(Marker));
      _PersistencePtr persistForLoadedMarker(__uuidof(Persistence));
      persistForLoadedMarker->Path = m_vTemplateNames[i].c_str();

      mtCompletionCode complCode;
      BSTR errorString;
      errorString = SysAllocString(L"");
      _bstr_t inputString;

      complCode = loadedMarker->RestoreTemplate(persistForLoadedMarker, inputString, &errorString);
      if ( complCode == mtSuccess )
        tmplsNameStrings.push_back((char*)loadedMarker->Name);
      else
        tmplsErrorStrings.push_back((char*)errorString);

      this->m_markers->AddTemplate(loadedMarker);
      loadedMarker.Release();
      persistForLoadedMarker.Release();
    }

    //Collect the error messages in the _Collection structure and loop through it
    this->validateTemplates(tmplsWarnStrings);
    return 1;
  }
  this->m_numOfTemplates = this->m_markers->TemplatesCount;
  return 0;
}

/****************************/
/** */
void MarkerTemplates::validateTemplates(vector<string> &templsValidateWarnStrings)
{
  _CollectionPtr pColl;
  pColl = this->m_markers->ValidateTemplates();

  CComVariant v;
  ULONG elementsFetched;
  //Grab a pointer to the _NewEnum of the _Collection interface
  IUnknownPtr pEnumUnk;
  pEnumUnk = pColl->_NewEnum();
  IEnumVARIANTPtr pEnum(pEnumUnk);
  while(pEnum->Next(1, &v, &elementsFetched) == S_OK)
  {
    char sz[100];
    WideCharToMultiByte(CP_ACP, 0, v.bstrVal, -1, sz, sizeof(sz), NULL, NULL);
    templsValidateWarnStrings.push_back(sz);
  }
}

/****************************/
void MarkerTemplates::setTemplateMatchTolerance(double tolerance, vector<string> &templsWarnStrings)
{
  this->m_markers->TemplateMatchToleranceMM = tolerance;
  this->validateTemplates(templsWarnStrings);
}

/****************************/
double MarkerTemplates::getTemplateMatchTolerance()
{
  return this->m_markers->TemplateMatchToleranceMM;
}

/****************************/
double MarkerTemplates::getTemplateMatchToleranceDefault()
{
  return this->m_markers->TemplateMatchToleranceMMDefault;
}

/****************************/
void MarkerTemplates::setPredictiveFramesInterleave(float pi)
{
  this->m_markers->PredictiveFramesInterleave = pi;
}

/****************************/
float MarkerTemplates::getPredictiveFrameInterleave()
{
  return this->m_markers->PredictiveFramesInterleave;
}

/****************************/
void MarkerTemplates::setPredictiveTracking(short b)
{
  this->m_markers->PredictiveTrackingEnabled = b;
}

/****************************/
long MarkerTemplates::getPredictiveTracking()
{
  return this->m_markers->PredictiveTrackingEnabled;
}

/****************************/
void MarkerTemplates::setAdjustCamAfterEveryProcess(short b)
{
  this->m_markers->AutoAdjustCameraExposure = b;
}

/****************************/
short MarkerTemplates::getAdjustCamAfterEveryProcess()
{
  return this->m_markers->AutoAdjustCameraExposure;
}

/****************************/
void MarkerTemplates::setJitterFilterFraction(float jf, int markerIndex)
{
  this->m_markers->GetTemplateItem(markerIndex)->JitterFilteringFraction = jf;
}

/****************************/
float MarkerTemplates::getJitterFilterFraction(int markerIndex)
{
  return this->m_markers->GetTemplateItem(markerIndex)->JitterFilteringFraction;
}

/****************************/
void MarkerTemplates::setTemplateName(int markerIndex, string name)
{
  _bstr_t newName = name.c_str();
  if ( this->isNewTemplateNameValid(name) )
    this->m_markers->GetTemplateItem(markerIndex)->Name = newName;
  mti_utils::renameFile(this->getTemplateName(markerIndex), name, "Markers");
}

/****************************/
string MarkerTemplates::getTemplateName(int markerIndex)
{
  return (char*)this->m_markers->GetTemplateItem(markerIndex)->Name;
}

/****************************/
bool MarkerTemplates::isNewTemplateNameValid(string newName)
{
  vector<string> templateNames;
  mti_utils::getFileNamesFromDirectory(templateNames, "Markers", FALSE);
  for (int i=0; i<templateNames.size(); i++)
    if (templateNames[i] == newName)
      return false;
  return true;
}
