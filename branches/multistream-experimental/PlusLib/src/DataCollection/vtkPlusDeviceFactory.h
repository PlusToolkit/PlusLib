/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkTrackerFactory_h
#define __vtkTrackerFactory_h

#include "PlusConfigure.h"
#include "vtkObject.h" 

class vtkTracker; 
class vtkXMLDataElement; 

/*!
  \class vtkTrackerFactory 
  \brief Factory class of supported trackers

  This class is a factory class of supported trackers to localize the object creation code.

  \ingroup PlusLibTracking
*/ 
class VTK_EXPORT vtkTrackerFactory: public vtkObject
{
public:
  
  static vtkTrackerFactory *New();
  vtkTypeRevisionMacro(vtkTrackerFactory,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Print all supported trackers with SDK version number */
  virtual void PrintAvailableTrackers(ostream& os, vtkIndent indent);

  /*! Create a new tracker instance from tracker type, delete previous tracker if's not NULL */ 
  PlusStatus CreateInstance(const char* aTrackerType, vtkTracker* &aTracker);

protected:
  vtkTrackerFactory();
  virtual ~vtkTrackerFactory();

  /*! Function pointer for storing New() static methods of vtkTracker classes */ 
  typedef vtkTracker* (*PointerToTracker)(); 
  /*! Map tracker types and the New() static methods of vtkTracker classes */ 
  std::map<std::string,PointerToTracker> TrackerTypes; 

private:
  vtkTrackerFactory(const vtkTrackerFactory&);
  void operator=(const vtkTrackerFactory&);

}; 

#endif 