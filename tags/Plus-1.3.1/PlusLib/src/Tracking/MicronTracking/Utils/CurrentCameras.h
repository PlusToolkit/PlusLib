#ifndef __CURRENTCAMERAS_H__
#define __CURRENTCAMERAS_H__

#include <iostream>
#include <iomanip>
#include <math.h>

#include "ImportStatements.h"
#include "utilityFunctions.h"

#define ROTATION_MATRIX_SIZE 9

class CurrentCameras
{
public:

  CurrentCameras(_CamerasPtr cams);
  ~CurrentCameras();

  void    init();
  int      setupCameras();
  _CameraPtr  getCurrCamera();
  int      selectCamera(int camIndex);
  void    refreshMarkerTemplates();
  int      grabFrame(int camIndex);
  void    processCurrentFrame();
  void    showIdentifiedVectors();
  void    detachCameras();

  long    getSerialNum(int camIndex);
  long    getXResolution(int camIndex);
  long    getYResolution(int camIndex);
  int     getSensorsNum(int camIndex);

  inline void setPersist(_PersistencePtr persist){ m_persist = persist; };

  inline void  setMarkers(_MarkersPtr markers){ m_markers = markers; };
  inline int  getIdentifiedMarkersCount(){return m_identifiedMarkersCount;};

  inline int  getCameraCount(){ return m_cameraCount; };
  inline int  getCurrCamIndex(){return m_camIndex;};
  inline int  getMarkerStatus(){return m_markerStatus;};
  void    getNumOfFacetsInEachMarker(vector<int> &nof);

  void    getRotations(vector<double> &vRot, int markerIdx);
  void    getTranslations(vector<double> &vTrans, int markerIdx);

  void    setShutterPreference(int sh, int camIndex);
  int      getShutterPreference(int camIndex);
  int      getShutterPreferenceDefualt();

  void    setShutterTime(float t, int camIndex);
  float    getShutterTime(int camIndex);

  void    setAutoExposure(short b, int camIndex);
  short    getAutoExposure(int camIndex);
  void    adjustExposure(float e, int camIndex);

  void    setLatestFrameTime(double t, int camIndex);
  double    getLatestFrameTime(int camIndex);

  float    getLatestFramePixAvg(int camIndex);

  int      getBitsPerPixel(int camIndex);

  long    getNumOfFramesGrabbed(int camIndex);
  int     getLatestFramePixHistogram(long* &aPH,unsigned char rate,int camIndex);

  void    getLeftRightImageArray(unsigned char* &lImageArray, unsigned char* &rImageArray, int camIndex);
  void    getLeftRightImageArrayHalfSize(unsigned char* &lImageArray, unsigned char* &rImageArray, int camIndex);
  
  void    getIdentifiedMarkersXPoints( double* &xp, int mIndex, int fIndex);

private:  
  _CamerasPtr    m_currCameras;
  _CameraPtr    m_currCamera;
  _PersistencePtr m_persist;
  _MarkersPtr    m_markers;

  int        m_camIndex;
  int        m_cameraCount;
//  int        m_shutterPreference;
  bool      m_isCameraAttached;
  bool      m_isCaptureEnabled;

  vector<long>  m_vSerialNums;
  vector<long>  m_vXResolutions;
  vector<long>  m_vYResolutions;
  vector<int>    m_vSensorNums;

  int        m_identifiedMarkersCount;
  vector<int>    m_vNumOfFacetsInEachMarker;

  // A two dimensional vector for storing the rotation matrices. The first dimension is the marker number.
  // The second dimension is the vecotor of rotation matrices
  vector< vector<double> > m_2dvRotations;
  // A two dimensional vector for storing the translations. 
  vector< vector<double> > m_2dvTranslations;
  // A two dimensional vector for storing the xpoints of the vectors. The first dimension is the marker number.
  // The second dimension is the facet number.
  vector< vector<double*> > m_vIdentifiedMarkersXPoints;


  int        m_markerStatus;

  SAFEARRAY* sa1;
  SAFEARRAY* sa2;
  
  vector<double*> vXPointsTemp;
  SAFEARRAY* saXPoints;
  double* xp;


  double findDistance(_variant_t* v1, _variant_t* v2);
};
#endif
