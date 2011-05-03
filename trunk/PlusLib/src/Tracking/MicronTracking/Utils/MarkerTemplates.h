#ifndef __MARKERTEMPLATES_H__
#define __MARKERTEMPLATES_H__

#include <iostream>
#include <atlbase.h>  // Need for CComBSTR / CComVariant
#include "ImportStatements.h"
#include "utilityFunctions.h"

using std::vector;
using std::string;

class MarkerTemplates
{
public:
  MarkerTemplates(_MarkersPtr markers);
  ~MarkerTemplates();

  int refreshTemplates(vector<string> &tmplsNameStrings, 
             vector<string> &tmplsErrorStrings,
             vector<string> &tmplsWarnStrings);
  void validateTemplates(vector<string> &tmplsValidateWarnStrings);

  // Match tolerance
  void setTemplateMatchTolerance(double tolerance, vector<string> &tmplsWarnStrings);
  double getTemplateMatchTolerance();
  double getTemplateMatchToleranceDefault();

  // Predictive interleave
  void setPredictiveFramesInterleave(float pi);
  float getPredictiveFrameInterleave();

  // Predictive tracking- 0 false, 1 true
  void setPredictiveTracking(short b);
  long getPredictiveTracking();

  // Auto Adjust cam exposure after every call to processFrame().
  void setAdjustCamAfterEveryProcess(short b);
  short getAdjustCamAfterEveryProcess();

  // Templates name
  void setTemplateName(int markerIndex, string name);
  string getTemplateName(int markerIndex);
  
  // Number of templates loaded
  inline long getNumOfAllTemplates(){return m_numOfTemplates;};



private:
  vector<string> m_vTemplateNames;
  bool isNewTemplateNameValid(string newName);
  long m_numOfTemplates;
  _MarkersPtr m_markers;

};

#endif
