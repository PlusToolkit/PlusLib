#ifndef RECORDPHANTOMPOINTSCONTROLLER_H
#define RECORDPHANTOMPOINTSCONTROLLER_H

#include "AbstractToolboxController.h"

#include <vector>

class vtkActor;
class vtkRenderer;
class vtkMatrix4x4;
class vtkPolyData;
class vtkPoints;
class vtkTransform;
class vtkAxesActor;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for phantom registration toolbox - singleton class
*/
class PhantomRegistrationController : public AbstractToolboxController
{
public:
	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static PhantomRegistrationController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~PhantomRegistrationController();

	/*!
	* \brief Initialize - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Initialize();

	/*!
	* \brief Clear - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Clear();

	/*!
	* \brief Acquire new tracker positions and updates toolbox and canvas - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus DoAcquisition();

	/*!
	* \brief Put state into in progress if all prerequisites are done - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Start();

	/*!
	* \brief Stop - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Stop();

	/*!
	* \brief Load phantom registration from XML file
	* \param aFile XML file name and path
	* \return Success flag
	*/
	PlusStatus LoadPhantomRegistrationFromFile(std::string aFile);

	/*!
	* \brief Load phantom registration from file
	* \param aConfig XML data element containing the phantom registration
	* \return Success flag
	*/
	PlusStatus LoadPhantomRegistration(vtkXMLDataElement* aConfig);

	/*!
	* \brief Save result to XML file
	* \param aFile XML file name and path
	*/
	PlusStatus SavePhantomRegistrationToFile(std::string aFile);

	/*!
	* \brief Save result to XML file
	* \param aConfig XML data element containing the phantom registration
	* \return Success flag
	*/
	PlusStatus SavePhantomRegistration(vtkXMLDataElement* aConfig);

	/*!
	* \brief Get current stylus position string to display
	* \return Current position of stylus
	*/
	std::string GetPositionString();

	/*!
	* \brief Getter function
	* \return Phantom renderer on toolbox
	*/
	vtkRenderer* GetPhantomRenderer();

	/*!
	* \brief Getter function
	* \return Number of landmarks to acquire
	*/
	int GetNumberOfLandmarks();

	/*!
	* \brief Getter function
	* \return Number of landmarks acquired so far
	*/
	int GetCurrentLandmarkIndex();

	/*!
	* \brief Getter function
	* \return String containing the name of the current landmark
	*/
	std::string GetCurrentLandmarkName();

	/*!
	* \brief Getter function TODO there will be no separate phantom definition file
	* \return String containing the phantom definition file path and name
	*/
	std::string GetPhantomDefinitionFileName();

	/*!
	* \brief Get phantom to phantom reference transform (result of phantom registration and landmark registration)
	* \return Phantom to phantom reference transform
	*/
	vtkTransform* GetPhantomToPhantomReferenceTransform();

	/*!
	* \brief Load phantom definition of phantom definition XML
	* \param Path and filename of input XML
	*/
	PlusStatus LoadPhantomDefinitionFromFile(std::string aFile);

	/*!
	* \brief Load phantom definition from file
	* \param aConfig XML data element containing the phantom definition
	* \return Success flag
	*/
	PlusStatus LoadPhantomDefinition(vtkXMLDataElement* aConfig);

	/*!
	* \brief Sets request recording flag to true, so at the next acquisition the position is recorded
	*/
	void RequestRecording();

	/*!
	* \brief Undo last recording
	*/
	void Undo();

	/*!
	* \brief Resets recording
	*/
	void Reset();

	/*!
	* \brief Performs landmark registration to determine transformation from phantom reference to phantom
	* \return Success flag
	*/
	PlusStatus Register();

protected:
	/*!
	* \brief Constructor
	*/
	PhantomRegistrationController();

	/*!
	* \brief Initialize 3D visualization
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

	/*!
	* \brief Acquires new position from main tool of the tracker
	* \param aPosition Out parameter for containing acquired position
	* \param aReference Flag if the reference is needed (if false, the main tool is acquired) - by default it is false
	* \return Acquired transform if successful, else NULL
	*/
	vtkSmartPointer<vtkMatrix4x4> AcquireStylusTipTrackerPosition(double aPosition[4], bool aReference = false);

protected:
	//! Renderer for the canvas
	vtkRenderer*							m_PhantomRenderer;

	//! Actor for displaying the phantom geometry in phantom canvas
	vtkActor*								m_PhantomBodyActor;

	//! Actor for displaying the phantom geometry in main canvas
	vtkActor*								m_RegisteredPhantomBodyActor;

	//! Actor for displaying the current position of the stylus
	vtkActor*								m_StylusActor;

	//! Actor for displaying the recorded landmarks
	vtkActor*								m_LandmarksActor;

	//! Actor for displaying the defined landmarks from the configuration file
	vtkActor*								m_RequestedLandmarksActor;

	//! Actor for displaying the axes of the reference coordinate system
	vtkAxesActor*							m_AxesActor;

	//! The measured landmarks
	vtkPolyData*							m_LandmarksPolyData;

	//! Polydata holding the requested landmark for highlighting in phantom canvas
	vtkPolyData*							m_RequestedLandmarkPolyData;

	//! Point array holding the defined landmarks from the configuration file
	vtkPoints*								m_DefinedLandmarks;

	//! Phantom to phantom reference transform - the result of the process
	vtkTransform*							m_PhantomToPhantomReferenceTransform;

	//! Phantom model to phantom transform (from phantom definition file)
	vtkTransform*							m_ModelToPhantomTransform;

	//! Names of the phantom landmarks
	std::vector<std::string>				m_LandmarkNames;

	//! Stylus position as string
	std::string								m_PositionString;

	//! Index of current landmark
	int										m_CurrentLandmarkIndex;

	//! Flag to trigger landmark recording - if it is set then at the next acquisition the position is recorded
	bool									m_RecordRequested;

	//! The mean error of the landmark registration in mm
	double									m_RegistrationError;

private:
	//! Instance of the singleton
	static PhantomRegistrationController*	m_Instance;
};

#endif