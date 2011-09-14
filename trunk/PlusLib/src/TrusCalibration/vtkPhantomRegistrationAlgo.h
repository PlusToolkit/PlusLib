#ifndef __vtkvtkPhantomRegistrationAlgo_h
#define __vtkvtkPhantomRegistrationAlgo_h

#include "vtkObject.h"
#include "PlusConfigure.h"
#include "vtkTransform.h"
#include "vtkPoints.h"

class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for phantom registration toolbox - singleton class
*/
class vtkPhantomRegistrationAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPhantomRegistrationAlgo,vtkObject);

  /*!
	* \brief New
	*/
	static vtkPhantomRegistrationAlgo *New();

public:
	/*!
	* \brief Performs landmark registration to determine transformation from phantom reference to phantom
	* \return Success flag
	*/
	PlusStatus Register();

	/*!
	* \brief Read phantom definition (landmarks)
	* \param aConfig Root XML data element containing the tool calibration
	* \return Success flag
	*/
	PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

	/*!
	* \brief Save result to XML data element
	* \param aConfig XML data element containing the stylus calibration
	* \return Success flag
	*/
	PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

	/*!
	* \brief Gets defined landmark name
	* \param aIndex Index of the landmark
	* \return Name string
	*/
  std::string GetDefinedLandmarkName(int aIndex) { return this->DefinedLandmarkNames[aIndex]; };

public:
  // Set/Get macros
	vtkGetMacro(RegistrationError, double);

  vtkGetObjectMacro(PhantomToPhantomReferenceTransform, vtkTransform); 
  vtkSetObjectMacro(PhantomToPhantomReferenceTransform, vtkTransform);

  vtkGetObjectMacro(DefinedLandmarks, vtkPoints);
  vtkGetObjectMacro(RecordedLandmarks, vtkPoints);

protected:
  vtkSetObjectMacro(DefinedLandmarks, vtkPoints);
  vtkSetObjectMacro(RecordedLandmarks, vtkPoints);

protected:
	/*!
	* \brief Constructor
	*/
	vtkPhantomRegistrationAlgo();

	/*!
	* \brief Destructor
	*/
	virtual	~vtkPhantomRegistrationAlgo();

protected:
	//! Point array holding the defined landmarks from the configuration file
	vtkPoints*								DefinedLandmarks;

	//! Names of the defined phantom landmarks from the configuration file
	std::vector<std::string>	DefinedLandmarkNames;

	//! Point array holding the recorded landmarks
	vtkPoints*								RecordedLandmarks;

  //! Phantom to phantom reference transform - the result of the registration
	vtkTransform*							PhantomToPhantomReferenceTransform;

	//! The mean error of the landmark registration in mm
	double									  RegistrationError;
};

#endif
