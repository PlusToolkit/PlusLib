#include "CurrentCameras.h"

CurrentCameras::CurrentCameras(_CamerasPtr cams)
{
  m_currCameras = cams;
  this->init();
}

/****************************/
CurrentCameras::~CurrentCameras()
{
  if ( m_currCameras != NULL )
  {
    delete this->m_currCameras;
    this->m_currCameras = NULL;
//    this->m_currCameras->Detach();
//    this->m_currCameras->Release();
//    this->m_currCamera->Release();
  }
//  this->m_persist->Release();
}

/****************************/
void CurrentCameras::init()
{
  m_persist = NULL;
  this->m_isCameraAttached = FALSE;
  this->m_isCaptureEnabled = TRUE;
  xp = new double();

}

/****************************/
int CurrentCameras::setupCameras()
{
  char currDir[255];
  mti_utils::getCurrPath(currDir);
  HRESULT hr;
  hr = this->m_currCameras->AttachAvailableCameras(0, 0, currDir);
  if (SUCCEEDED(hr))
  {
    this->m_cameraCount = m_currCameras->Count();
    if (this->m_cameraCount > 0)
    {
      this->m_isCameraAttached = true;
      if ( this->selectCamera(0) != 1 )
        return 0;

        this->m_currCameras->ShutterPreference = m_persist->Retrieve("CameraShutter", this->m_currCameras->ShutterPreferenceDefault);

      //Populate the m_vSerialNums vector with the serial numbers,
      //m_vXResolutions with XResolutions and m_vYResolutions with YResolutions of the attached camera(s).
      for (int i=0; i < this->m_cameraCount; i++)
      {
        this->m_vSerialNums.push_back(this->m_currCameras->Item(i)->SerialNumber);
        this->m_vXResolutions.push_back(this->m_currCameras->Item(i)->XRes);
        this->m_vYResolutions.push_back(this->m_currCameras->Item(i)->YRes);
        this->m_vSensorNums.push_back(this->m_currCameras->Item(i)->SensorsNum);
      }

      return 1;
    }
  }

  //Note(To be Done):  cam to cam registration goes here and updating menus
  return 0;
}

/****************************/
long CurrentCameras::getSerialNum(int camIndex)
{
  if (camIndex == -1)
    return this->m_vSerialNums[this->m_camIndex];
  else if (camIndex < m_cameraCount  && camIndex > -1)
    return this->m_vSerialNums[camIndex];
  else //The camIndex is not valid
    return 0;
}

/****************************/
long CurrentCameras::getXResolution(int camIndex)
{
  if (camIndex == -1)
    return this->m_vXResolutions[this->m_camIndex];
  else if (camIndex < m_cameraCount  && camIndex > -1)
    return this->m_vXResolutions[camIndex];
  else //The camIndex is not valid
    return 0;
}

/****************************/
long CurrentCameras::getYResolution(int camIndex)
{
  if (camIndex == -1)
    return this->m_vYResolutions[this->m_camIndex];
  else if (camIndex < m_cameraCount  && camIndex > -1)
    return this->m_vYResolutions[camIndex];
  else //The camIndex is not valid
    return 0;
}

/****************************/
int CurrentCameras::getSensorsNum(int camIndex)
{
  if (camIndex == -1)
    return this->m_vSensorNums[this->m_camIndex];
  else if (camIndex < m_cameraCount && camIndex > -1)
    return this->m_vSensorNums[camIndex];
  else // The cam index is not valid
    return 0;
}

/****************************/
_CameraPtr CurrentCameras::getCurrCamera()
{
  return this->m_currCamera;
}

/****************************/
/* Returns 1 if successful, 0 if not. */
int CurrentCameras::selectCamera(int camIndex)
{
  if (camIndex > this->m_cameraCount - 1)
    return 0;

  this->m_camIndex = camIndex;
  this->m_currCamera = this->m_currCameras->Item(m_camIndex);
  return 1;
}

/****************************/
int CurrentCameras::grabFrame(int camIndex)
{
  if( this->m_currCameras != NULL )
  {
    // if camIndex is -1 grab the frame from all the cameras
    if (camIndex == -1)
      if (this->m_currCameras->GrabFrame(NULL)) return 1;
    //otherwise grab the frame of the specified camera
    else
      if (this->m_currCameras->GrabFrame(this->m_currCameras->Item(camIndex))) return 1;
  }
  this->m_markerStatus = mti_utils::MTI_GRAB_FRAME_ERROR;
  return 0;
}

/****************************/
void CurrentCameras::processCurrentFrame()
{
  //this->m_markers->AutoAdjustCameraExposure = true;
  this->m_markers->ProcessFrame(this->m_currCamera);
  this->showIdentifiedVectors();
}

/****************************/
double CurrentCameras::findDistance(_variant_t* v1, _variant_t* v2)
{
  double acc = 0.0;
  SAFEARRAY* sa1 = v1->parray;
  SAFEARRAY* sa2 = v2->parray;
  double* a1;
  double* a2;
  HRESULT h1;
  HRESULT h2;
  h1 = SafeArrayAccessData(sa1, reinterpret_cast<void**>(&a1));
  h2 = SafeArrayAccessData(sa2, reinterpret_cast<void**>(&a2));
  SafeArrayUnaccessData(sa1);
  SafeArrayUnaccessData(sa2);

//  for ( int y = 0; y < 3 ; y++)
//  {
//    std::cout << " a1: " << a1[0] << " a2: " << a1[1] << " a3: " << a1[2] << std::endl;
//    std::cout << " b1: " << a2[0] << " b2: " << a2[1] << " b3: " << a2[2] << std::endl;
//  }

  if ( SUCCEEDED(h1) && SUCCEEDED(h2) )
  {
    for (int i=0; i< sa1->rgsabound->cElements; i++)
      acc = acc + (a1[i] - a2[i]) * (a1[i] - a2[i]);
    return sqrt(acc);
  }
  else
    return 0;
}

/****************************/
void CurrentCameras::detachCameras()
{
  if (this->m_isCameraAttached)
  {
    if ( this->m_currCamera != NULL ) 
      this->m_currCameras->Detach();
  }
  this->m_isCameraAttached = false;
}

/****************************/
void CurrentCameras::getLeftRightImageArray(unsigned char* &lImageArray, unsigned char* &rImageArray, int camIndex)
{
  const int ROW=640;
  const int COL=480;

  // Create a two dimensional SAFEARRAY with 640 x 480 dimension to access the image array.

  // VT_UI1 is unsigned char as the image data are 1bit (similar to BYTE datatype in VB)
  static int arraysCreated = 0;

  SAFEARRAYBOUND saf1[] = { {ROW,0}, {COL,0} };
  SAFEARRAYBOUND saf2[] = { {ROW,0}, {COL,0} };
  
  if (arraysCreated != 0)
  {
    SafeArrayDestroy(sa1);
    SafeArrayDestroy(sa2);
  }
  sa1 = SafeArrayCreate(VT_UI1,2,saf1);
  sa2 = SafeArrayCreate(VT_UI1,2,saf2);
  arraysCreated = 1;

  // Get the image data array into the SAFEARRAY sa1 and sa2.
  HRESULT r = this->m_currCamera->GetImages(&sa1, &sa2);

  // Accesss the SAFEARRAY data
  lImageArray = reinterpret_cast<unsigned char*>(sa1->pvData);
  rImageArray = reinterpret_cast<unsigned char*>(sa2->pvData);
}

/****************************/
void CurrentCameras::getLeftRightImageArrayHalfSize(unsigned char* &lImageArray, unsigned char* &rImageArray, int camIndex)
{
  const int ROW= 320;
  const int COL= 240;

  // Create a two dimensional SAFEARRAY with 640 x 480 dimension to access the image array.

  // VT_UI1 is unsigned char as the image data are 1bit (similar to BYTE datatype in VB)
  static int arraysCreated = 0;

  SAFEARRAYBOUND saf1[] = { {ROW,0}, {COL,0} };
  SAFEARRAYBOUND saf2[] = { {ROW,0}, {COL,0} };
  
  if (arraysCreated != 0)
  {
    SafeArrayDestroy(sa1);
    SafeArrayDestroy(sa2);
  }
  sa1 = SafeArrayCreate(VT_UI1,2,saf1);
  sa2 = SafeArrayCreate(VT_UI1,2,saf2);
  arraysCreated = 1;

  // Get the image data array into the SAFEARRAY sa1 and sa2.
  HRESULT r = this->m_currCamera->GetImagesHalfSize(&sa1, &sa2);

  // Accesss the SAFEARRAY data
  lImageArray = reinterpret_cast<unsigned char*>(sa1->pvData);
  rImageArray = reinterpret_cast<unsigned char*>(sa2->pvData);
}

/****************************/
void CurrentCameras::showIdentifiedVectors()
{
  _CollectionPtr IMarkers;
  IMarkers = this->m_markers->GetIdentified(NULL);
  VARIANT v;
  _MarkerPtr m=0;
  v.vt=VT_INT;
  int i=0;

  this->m_identifiedMarkersCount = IMarkers->Count();
  if (this->m_identifiedMarkersCount == 0 )
  {
    this->m_markerStatus = mti_utils::MTI_NO_MARKER_CAPTURED;
    return;
  }
  
  this->m_markerStatus = mti_utils::MTI_MARKER_CAPTURED;

  this->m_2dvRotations.clear();
  this->m_2dvTranslations.clear();
  
  static short xPointsArrayCreated = 0;
  int markerNum = 1;
  int facetNum = 1;
  this->m_vIdentifiedMarkersXPoints.clear();
  for (markerNum = 1; markerNum <= m_identifiedMarkersCount; markerNum++)
  {
    v.intVal = markerNum;
    m = IMarkers->Item(&v);
    if (m->WasIdentified(NULL))
    {
      // Storing the number of facets in each marker
      _CollectionPtr IFacets = m->IdentifiedFacets(NULL);
      this->m_vNumOfFacetsInEachMarker.clear();
      this->m_vNumOfFacetsInEachMarker.push_back(IFacets->Count());

      vXPointsTemp.clear();
      for (facetNum = 1; facetNum < IFacets->Count()+1; facetNum++)
      {
        v.intVal = facetNum;
        _FacetPtr Ft = IFacets->Item(&v);
        // Extracting the xpoints
        SAFEARRAYBOUND saf[] = { {2,0}, {4,0}, {2,0} };
        // Programming note: I have to deal with the mem leak issue here. The following two lines of 
        // code fixes the mem leak but results that all the vectors in the m_vIdentifiedMarkersXPoints values 
        // will be the same. So commented that out for now.
//        if (!xPointsArrayCreated == 0)
//          SafeArrayDestroy(saXPoints);
                  
        saXPoints = SafeArrayCreate(VT_R8, 3, saf);
        saXPoints = Ft->GetIdentifiedXPoints(this->m_currCamera);
        xp = reinterpret_cast<double*>(saXPoints->pvData);
        vXPointsTemp.push_back(xp);/**/
        xPointsArrayCreated = 1;
//        for (int row1 = 0; row1 < 2; row1++)
//          for (int row2 = 0; row2 < 4; row2++)
//            for (int row3 = 0; row3 < 2; row3++)
//              std::cout << " xpoint[" << row1 << "][" << row2 << "][" << row3 << "] is: " << *(xp + (row1 * 8)+((row3 * 4) + row2)) << std::endl;
      }
      this->m_vIdentifiedMarkersXPoints.push_back(vXPointsTemp);
    }
    _Xform3DPtr Marker2CurrCameraXf;
    Marker2CurrCameraXf = m->Marker2CameraXf(this->m_currCamera);
//    std::cout << markerNum << ". (" ;

    // Declare a temp vector to hold the translation values and then push_back it into the corresponding container vector
    vector<double> vTransTemp;
    vTransTemp.clear();
    if (Marker2CurrCameraXf != NULL )
    {
      for (i = 0 ; i < 3; i++)
      {
//        std::cout << std::setw(5) << std::right << std::fixed << std::setprecision(2) <<  Marker2CurrCameraXf->GetShift(i);
//        if (i < 2 ) std::cout << ", ";
        vTransTemp.push_back(Marker2CurrCameraXf->GetShift(i));
      }
      vTransTemp.push_back(1);
    }
    this->m_2dvTranslations.push_back(vTransTemp);
    

//    std::cout <<  ")" << std::endl;

    // Declare a temp vector to hold the rotation matrix values and then push_back it into the corresponding container vector
//    std::cout << "Rotation Matrix for Marker " << markerNum << " is " << std::endl;
    vector<double> vRotTemp;
    vRotTemp.clear();
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
      {
        vRotTemp.push_back(Marker2CurrCameraXf->GetRot(j,k));
//        std::cout << std::setw(5) <<  std::fixed << std::right << std::setprecision(5) << " R" << j+1 <<  k+1 << " : " << Marker2CurrCameraXf->GetRot(k,j) << std::endl;
      }
    this->m_2dvRotations.push_back(vRotTemp);
//    std::cout << std::endl;
  }


}

/****************************/
void CurrentCameras::getIdentifiedMarkersXPoints(double* &xp, int mIndex, int fIndex)
{
  xp = this->m_vIdentifiedMarkersXPoints[mIndex][fIndex];
}

/****************************/
void CurrentCameras::getNumOfFacetsInEachMarker(vector<int> &nof)
{
  nof = this->m_vNumOfFacetsInEachMarker;
}

/****************************/
void CurrentCameras::getRotations(vector<double> &vRot, int markerIdx)
{
  vRot = this->m_2dvRotations[markerIdx];
}

/****************************/
void CurrentCameras::getTranslations(vector<double> &vTrans, int markerIdx)
{
  vTrans = this->m_2dvTranslations[markerIdx];
}

/****************************/
void CurrentCameras::setShutterPreference(int sh, int camIndex)
{
  //this->m_shutterPreference = sh;
  if (camIndex == -1)
    this->m_currCameras->ShutterPreference = sh;
  else if (camIndex < this->m_cameraCount && camIndex > -1)
    this->m_currCameras->Item(camIndex)->ShutterPreference = sh;
  else // if camIndex is invalid
    return;
}

/****************************/
int CurrentCameras::getShutterPreference(int camIndex)
{
  if (camIndex == -1)
    return this->m_currCameras->Item(this->m_camIndex)->ShutterPreference;
  if ( camIndex > -1 && camIndex < this->m_cameraCount)
    return this->m_currCameras->Item(camIndex)->ShutterPreference;
  else // if camIndex is not valid
    return 0;
}

/****************************/
int CurrentCameras::getShutterPreferenceDefualt()
{
  return this->m_currCameras->ShutterPreferenceDefault;
}

/****************************/
void CurrentCameras::setShutterTime(float t, int camIndex)
{
  if ( camIndex == -1 ) // Set all the cameras
  {
    for (int i = 0; i < this->m_cameraCount; i++)
      this->m_currCameras->Item(i)->ShutterTime0to1 = t;
  }
  else if (camIndex > -1 && camIndex < this->m_cameraCount )
    this->m_currCameras->Item(camIndex)->ShutterTime0to1 = t;
  else // If camIndex is not valid
    return;
}

/****************************/
float CurrentCameras::getShutterTime(int camIndex)
{
  if ( camIndex == -1 )
    return this->m_currCameras->Item(this->m_camIndex)->ShutterTime0to1;
  else if (camIndex < this->m_cameraCount && camIndex > -1 )
    return this->m_currCameras->Item(camIndex)->ShutterTime0to1;
  else // If camIndex is not valid
    return 0;
}

/****************************/
void CurrentCameras::setAutoExposure(short b, int camIndex)
{
  bool a = (b==0) ? false : true;

  if ( camIndex == -1 ) // Set all the cameras
  {
    for (int i = 0; i< this->m_cameraCount; i++)
      this->m_currCameras->Item(i)->AutoExposure = a;
  }
  else if (camIndex < this->m_cameraCount && camIndex > -1 )
    this->m_currCameras->Item(camIndex)->PutAutoExposure(a);
  else
    return;
}

/****************************/
short CurrentCameras::getAutoExposure(int camIndex)
{
  return this->m_currCameras->Item(camIndex)->AutoExposure;
} 

/****************************/
void CurrentCameras::adjustExposure(float e, int camIndex)
{
  if ( camIndex == -1 )
    for ( int i = 0; i < this->m_cameraCount; i++)
      this->m_currCameras->Item(i)->AdjustExposure(e);
  else if (camIndex > -1 && camIndex < this->m_cameraCount)
    this->m_currCameras->Item(camIndex)->AdjustExposure(e);
  else // If camIndex not valid
    return;
}

/****************************/
void CurrentCameras::setLatestFrameTime(double t, int camIndex)
{
  if ( camIndex == -1 )
  {
    for ( int i = 0; i < this->m_cameraCount; i++)
      this->m_currCameras->Item(i)->FrameMTTimeSecs = t;
  }
  else if (camIndex > -1 && camIndex < this->m_cameraCount )
    this->m_currCameras->Item(camIndex)->FrameMTTimeSecs = t;
  else // If camIndex not valid
    return;
}

/****************************/
double CurrentCameras::getLatestFrameTime(int camIndex)
{
  if ( camIndex == -1 )
    return this->m_currCameras->Item(this->m_currCamera)->FrameMTTimeSecs;
  else if ( camIndex > -1 && camIndex < this->m_cameraCount )
    return this->m_currCameras->Item(camIndex)->FrameMTTimeSecs;
  else // If camIndex not valid
    return 0;
}

/****************************/
float CurrentCameras::getLatestFramePixAvg(int camIndex)
{
  unsigned char avg = 1;
  if ( camIndex == -1 )
    return this->m_currCameras->Item(this->m_currCamera)->FrameAverage(&avg);
  else if ( camIndex > -1 && camIndex < this->m_cameraCount )
    return this->m_currCameras->Item(camIndex)->FrameAverage(&avg);
  else // If camIndex is not valid
    return 0;
}

/****************************/
int CurrentCameras::getBitsPerPixel(int camIndex)
{
  if (camIndex == -1)
    return this->m_currCamera->BitsPerPixel;
  else if (camIndex > -1 && camIndex < this->m_cameraCount)
    return this->m_currCameras->Item(camIndex)->BitsPerPixel;
  else // If camIndex is invalid.
    return 0;
}

/****************************/
long CurrentCameras::getNumOfFramesGrabbed(int camIndex)
{
  if (camIndex == -1)
    return this->m_currCamera->FramesGrabbed;
  else if (camIndex > -1 && camIndex < this->m_cameraCount)
    return this->m_currCamera->FramesGrabbed;
  else // If camIndex not valid
    return 0;
}

/****************************/
int CurrentCameras::getLatestFramePixHistogram(long* &aPH,unsigned char rate,int camIndex)
{
  const ARRAY_SIZE = 256;
  SAFEARRAY* sa;
  SAFEARRAYBOUND sab[] = { ARRAY_SIZE,0 };
  sa = SafeArrayCreate(VT_I4, 1, sab);
  sa = this->m_currCamera->FrameHistogram(&rate);

  HRESULT h = SafeArrayAccessData(sa, reinterpret_cast<void**>(&aPH));
  if (SUCCEEDED(h))
  {
    SafeArrayUnaccessData(sa);
    return 1;
  }
    else
    return 0;
}

