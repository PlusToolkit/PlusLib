/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusVideoSourceFactory_h
#define __vtkPlusVideoSourceFactory_h

#include "PlusConfigure.h"
#include "vtkObject.h" 

class vtkPlusVideoSource; 

/*!
  \class vtkPlusVideoSourceFactory 
  \brief Factory class of supported video sources

  This class is a factory class of supported video sources to localize the object creation code.

  \ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkPlusVideoSourceFactory: public vtkObject
{
public:
  
  static vtkPlusVideoSourceFactory *New();
  vtkTypeRevisionMacro(vtkPlusVideoSourceFactory,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Print all supported video sources with SDK version number */ 
  virtual void PrintAvailableVideoSources(ostream& os, vtkIndent indent);

  /*! Create a new video source instance from video source type, delete previous video source if's not NULL */ 
  PlusStatus CreateInstance(const char* aVideoSourceType, vtkPlusVideoSource* &aVideoSource);

protected:
  vtkPlusVideoSourceFactory();
  virtual ~vtkPlusVideoSourceFactory();

  /*! Function pointer for storing New() static methods of vtkPlusVideoSource classes */ 
  typedef vtkPlusVideoSource* (*PointerToVideoSource)(); 
  /*! Map video source types and the New() static methods of vtkPlusVideoSource classes */ 
  std::map<std::string,PointerToVideoSource> VideoSourceTypes; 

private:
  vtkPlusVideoSourceFactory(const vtkPlusVideoSourceFactory&);
  void operator=(const vtkPlusVideoSourceFactory&);

}; 

#endif 