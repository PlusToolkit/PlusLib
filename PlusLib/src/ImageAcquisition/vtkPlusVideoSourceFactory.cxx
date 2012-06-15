/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPlusVideoSourceFactory.h"
#include "vtkPlusVideoSource.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
// Video sources
#include "vtkSavedDataVideoSource.h"

//#ifdef PLUS_USE_MATROX_IMAGING
//#include "vtkMILVideoSource2.h"
//#endif

#ifdef WIN32
#ifdef PLUS_USE_VFW_VIDEO
#include "vtkWin32VideoSource2.h"
#endif
//#else
//#ifdef USE_LINUX_VIDEO
//#include "vtkV4L2VideoSource2.h"
//#endif

#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
#include "vtkSonixVideoSource.h"
#include "vtkSonixPortaVideoSource.h"
#endif

#ifdef PLUS_USE_BKPROFOCUS_VIDEO
#include "vtkBkProFocusVideoSource.h"
#endif

#ifdef PLUS_USE_ICCAPTURING_VIDEO
#include "vtkICCapturingSource.h"
#endif

#ifdef PLUS_USE_OpenIGTLink
#include "vtkOpenIGTLinkVideoSource.h"
#endif

#ifdef PLUS_USE_EPIPHAN
#include "vtkEpiphanVideoSource.h"
#endif

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusVideoSourceFactory, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPlusVideoSourceFactory);

//----------------------------------------------------------------------------
vtkPlusVideoSourceFactory::vtkPlusVideoSourceFactory()
{	
  VideoSourceTypes["None"]=NULL; 
  VideoSourceTypes["SavedDataset"]=(PointerToVideoSource)&vtkSavedDataVideoSource::New; 
  VideoSourceTypes["NoiseVideo"]=(PointerToVideoSource)&vtkPlusVideoSource::New; 
#ifdef PLUS_USE_OpenIGTLink
  VideoSourceTypes["OpenIGTLinkVideo"]=(PointerToVideoSource)&vtkOpenIGTLinkVideoSource::New; 
#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  VideoSourceTypes["SonixVideo"]=(PointerToVideoSource)&vtkSonixVideoSource::New; 
  VideoSourceTypes["SonixPortaVideo"]=(PointerToVideoSource)&vtkSonixPortaVideoSource::New; 
#endif 
#ifdef PLUS_USE_BKPROFOCUS_VIDEO
  VideoSourceTypes["BkProFocus"]=(PointerToVideoSource)&vtkBkProFocusVideoSource::New; 
#endif 
#ifdef PLUS_USE_MATROX_IMAGING
  VideoSourceTypes["MatroxImaging"]=(PointerToVideoSource)&vtkMILVideoSource2::New; 
#endif 
#ifdef PLUS_USE_VFW_VIDEO
  VideoSourceTypes["VFWVideo"]=(PointerToVideoSource)&vtkWin32VideoSource2::New; 
#endif 
#ifdef PLUS_USE_ICCAPTURING_VIDEO
  VideoSourceTypes["ICCapturing"]=(PointerToVideoSource)&vtkICCapturingSource::New; 
#endif 
#ifdef PLUS_USE_LINUX_VIDEO
  VideoSourceTypes["LinuxVideo"]=(PointerToVideoSource)&vtkV4L2LinuxSource2::New; 
#endif 
#ifdef PLUS_USE_EPIPHAN
  VideoSourceTypes["Epiphan"]=(PointerToVideoSource)&vtkEpiphanVideoSource::New; 
#endif 
}

//----------------------------------------------------------------------------
vtkPlusVideoSourceFactory::~vtkPlusVideoSourceFactory()
{
}

//----------------------------------------------------------------------------
void vtkPlusVideoSourceFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  this->PrintAvailableVideoSources(os, indent); 

}

//----------------------------------------------------------------------------
void vtkPlusVideoSourceFactory::PrintAvailableVideoSources(ostream& os, vtkIndent indent)
{
  os << indent << "Supported video capturing devices: " << std::endl; 
  std::map<std::string,PointerToVideoSource>::iterator it; 
  for ( it = VideoSourceTypes.begin(); it != VideoSourceTypes.end(); ++it)
  {
    if ( it->second != NULL )
    {
      vtkPlusVideoSource* videoSource = (*it->second)(); 
      os << indent.GetNextIndent() << "- " << it->first << " (ver: " << videoSource->GetSdkVersion() << ")" << std::endl; 
      videoSource->Delete();
      videoSource=NULL;
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVideoSourceFactory::CreateInstance(const char* aVideoSourceType, vtkPlusVideoSource* &aVideoSource)
{
  if ( aVideoSource != NULL )
  {
    aVideoSource->Delete(); 
    aVideoSource = NULL; 
  }

  std::string videoSourceType; 
  if ( aVideoSourceType == NULL ) 
  {
    LOG_WARNING("Video source type is NULL, set to default: None"); 
    videoSourceType = "None"; 
  }
  else
  {
    videoSourceType = aVideoSourceType; 
  }

  if ( VideoSourceTypes.find(videoSourceType) != VideoSourceTypes.end() )
  {
    if ( VideoSourceTypes[videoSourceType] != NULL )
    { // Call tracker New() function if tracker not NULL
      aVideoSource = (*VideoSourceTypes[videoSourceType])(); 
    }
  }
  else
  {
    LOG_ERROR("Unknown video source type: " << videoSourceType);
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}
