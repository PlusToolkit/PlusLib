/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorVideoSource_h
#define __vtkUsSimulatorVideoSource_h

#include "vtkPlusVideoSource.h"
#include "vtkUsSimulatorAlgo.h"
#include "vtkTracker.h"
#include "vtkTransformRepository.h"

class vtkVideoBuffer; 

class VTK_EXPORT vtkUsSimulatorVideoSource;

/*!
  \class vtkUsSimulatorVideoSource 
  \brief Class for providing VTK video input interface from simulated ultrasound
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkUsSimulatorVideoSource : public vtkPlusVideoSource
{
public:
	vtkTypeRevisionMacro(vtkUsSimulatorVideoSource,vtkPlusVideoSource);
	void PrintSelf(ostream& os, vtkIndent indent);   
	static vtkUsSimulatorVideoSource* New();

  /*! Read configuration from xml data */
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Get ultrasound simulator */
  vtkGetObjectMacro(UsSimulator, vtkUsSimulatorAlgo); 

  /*! Set ultrasound simulator */
  vtkSetObjectMacro(Tracker, vtkTracker); 
  /*! Get ultrasound simulator */
  vtkGetObjectMacro(Tracker, vtkTracker); 

protected:
  /*! Set ultrasound simulator */
  vtkSetObjectMacro(UsSimulator, vtkUsSimulatorAlgo); 

  /*! Set transform repository */
	vtkSetObjectMacro(TransformRepository, vtkTransformRepository);

protected:
	/*! Constructor */
	vtkUsSimulatorVideoSource();
	/*! Destructor */
	virtual ~vtkUsSimulatorVideoSource();

  /*! Connect to device */
	virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
	virtual PlusStatus InternalDisconnect();

  /*! The internal function which actually does the grab.  */
	PlusStatus InternalGrab();

protected:
  /*! Ultrasound simulator */
  vtkUsSimulatorAlgo* UsSimulator;

  /*! Tracker used in the simulator */
  vtkTracker* Tracker;

  /* Transform repository */
  vtkTransformRepository* TransformRepository;

private:
	static vtkUsSimulatorVideoSource* Instance;
	vtkUsSimulatorVideoSource(const vtkUsSimulatorVideoSource&);  // Not implemented.
	void operator=(const vtkUsSimulatorVideoSource&);  // Not implemented.
};

#endif
