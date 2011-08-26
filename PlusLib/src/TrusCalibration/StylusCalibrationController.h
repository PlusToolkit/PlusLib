#ifndef STYLUSCALIBRATIONCONTROLLER_H
#define STYLUSCALIBRATIONCONTROLLER_H

#include "AbstractToolboxController.h"

class vtkTransform;
class vtkMatrix4x4;
class vtkPolyData;
class vtkActor;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for stylus calibration toolbox - singleton class
*/
class StylusCalibrationController : public AbstractToolboxController
{
public:
	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static StylusCalibrationController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~StylusCalibrationController();

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
	* \brief Start calibration - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Start();

	/*!
	* \brief Stop calibration - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Stop();

	/*!
	* \brief Load stylus calibration (stylus tip to stylus transform) from file
	* \param aFile Stylus cailbration transform XML
	* \return Success flag
	*/
	PlusStatus LoadStylusCalibrationFromFile(std::string aFile);

	/*!
	* \brief Load stylus calibration (stylus tip to stylus transform) from file
	* \param aConfig XML data element containing the stylus calibration
	* \return Success flag
	*/
	PlusStatus LoadStylusCalibration(vtkXMLDataElement* aConfig);

	/*!
	* \brief Save result to XML file
	* \param aConfig XML data element containing the stylus calibration
	* \return Success flag
	*/
	PlusStatus SaveStylusCalibration(vtkXMLDataElement* aConfig);

	/*!
	* \brief Getter function
	* \return Number of points to acquire
	*/
	int GetNumberOfPoints();

	/*!
	* \brief Setter function
	* \param Number of points to acquire
	*/
	void SetNumberOfPoints(int aNumOfPoints);

	/*!
	* \brief Getter function
	* \return Number of points acquired so far
	*/
	int GetCurrentPointNumber();

	/*!
	* \brief Getter function
	* \return Stylus port number
	*/
	int GetStylusPortNumber();

	/*!
	* \brief Getter function
	* \return Uncertainty (standard deviation), precision of the calibration result
	*/
	double GetPrecision();

	/*!
	* \brief Get stylus tip to stylus transform (calibration result)
	* \return Stylus tip to stylus transform
	*/
	vtkTransform* GetStylustipToStylusTransform();

	/*!
	* \brief Assembles the bounding box string to display
	* \return String containing the bounding box size
	*/
	std::string GetBoundingBoxString();

	/*!
	* \brief Get current stylus position string to display
	* \return Current position of stylus
	*/
	std::string GetPositionString();

	/*!
	* \brief Get calibration result string to display
	* \return Calibration result - stylus tip to stylus transform - string
	*/
	std::string GetStylustipToStylusTransformString();

	/*!
	* \brief Re-initialize stylus calibration matrix to stylus tool from m_StylustipToStylusTransform (in case the user has changed device set in the meantime)
	* \return Success flag
	*/
	PlusStatus FeedStylusCalibrationMatrixToTool();

	/*!
	* \brief Load stylus model from tracker tool object. If it is not present then create a default model
	* \param aActor Actor to add the model to. If NULL, the model is added to m_StylusActor member actor of this class
	* \return Success flag
	*/
	PlusStatus LoadStylusModel(vtkActor* aActor = NULL);

protected:
	/*!
	* \brief Constructor
	*/
	StylusCalibrationController();

	/*!
	* \brief Initialize 3D visualization
	* \return Success flag
	*/
	PlusStatus DetermineStylusPortNumber();

	/*!
	* \brief Initialize 3D visualization
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

	/*!
	* \brief Acquires new position from stylus tool of the tracker
	* \param aPosition Out parameter for containing acquired position
	* \param aReference Flag if the reference is needed (if false, the main tool is acquired) - by default it is false
	* \return Acquired transform if successful, else NULL
	*/
	vtkSmartPointer<vtkMatrix4x4> AcquireStylusTrackerPosition(double aPosition[4], bool aReference = false);

	/*!
	* \brief Do the stylus calibration
	* \return Success flag
	*/
	PlusStatus CalibrateStylus();

	/*!
	* \brief Display stylus tooltip (after calibration is complete)
	*/
	void DisplayStylus();

protected:
	//! Number of points to acquire
	int									m_NumberOfPoints;

	//! Number of points acquired so far
	int									m_CurrentPointNumber;

	//! Frame count in tracker when recording is started
	int									m_StartingFrame;

	//! Port number of the stylus tool found in the configuration file
	int									m_StylusPortNumber;

	//! Bounding box (0,1 - min and max of X axis; 2,3 - Y; 4,5 - Z)
	double								m_BoundingBox[6];

	//! Stylustip to stylus transform - the result of the calibration
	vtkTransform*						m_StylustipToStylusTransform;

	//! Uncertainty (standard deviation), error of the calibration result in mm
	double								m_CalibrationError;

	//! Stylus or stylus tip position (depenting on the state) as string
	std::string							m_PositionString;

	//! Polydata holding the input points
	vtkPolyData*						m_InputPolyData;

	//! Actor for displaying the points in 3D
	vtkActor*							m_InputActor;

	//! Polydata holding the stylus tip point after calibration
	vtkPolyData*						m_StylusTipPolyData;

	//! Actor for displaying the stylus tip point after calibration
	vtkActor*							m_StylusTipActor;

	//! Actor for displaying the current position of the stylus
	vtkActor*							m_StylusActor;

private:
	//! Instance of the singleton
	static StylusCalibrationController*	m_Instance;
};

#endif
