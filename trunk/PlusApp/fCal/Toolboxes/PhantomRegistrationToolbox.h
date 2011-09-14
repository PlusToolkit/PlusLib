#ifndef RECORDPHANTOMPOINTSTOOLBOX_H
#define RECORDPHANTOMPOINTSTOOLBOX_H

#include "ui_PhantomRegistrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkPhantomRegistrationAlgo;
class vtkActor;
class vtkPolyData;
class vtkRenderer;

//-----------------------------------------------------------------------------

/*!
* \brief Phantom registration toolbox view class
*/
class PhantomRegistrationToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParentMainWindow Parent main window
	* \param aFlags widget flag
	*/
	PhantomRegistrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~PhantomRegistrationToolbox();

	/*!
	* \brief Initialize toolbox (load session data) - implementation of a pure virtual function
	*/
	void Initialize();

	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
	*/
	void RefreshContent();

	/*!
	* \brief Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
	*/
	void SetDisplayAccordingToState();

	/*!
	* \brief Return phantom registration algorithm object
  * \return Phantom registration algo
	*/
  vtkPhantomRegistrationAlgo* GetPhantomRegistrationAlgo() { return m_PhantomRegistration; };

protected:
	/*!
	* \brief Initialize 3D visualization
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

	/*!
	* \brief Put state into in progress if all prerequisites are done
	* \return Success flag
	*/
	PlusStatus Start();

protected slots:
  /*!
	* \brief Slot handling open phantom definition button click
	*/
	void OpenPhantomDefinition();

	/*!
	* \brief Slot handling open stylus calibration button click
	*/
	void OpenStylusCalibration();

	/*!
	* \brief Slot handling record button click
	*/
	void RecordPoint();

	/*!
	* \brief Slot handling undo button click
	*/
	void Undo();

	/*!
	* \brief Slot handling reset button click
	*/
	void Reset();

	/*!
	* \brief Slot handling save button click
	*/
	void Save();

protected:
  //! Phantom registration algorithm
  vtkPhantomRegistrationAlgo* m_PhantomRegistration;

	//! Renderer for the canvas
	vtkRenderer*							  m_PhantomRenderer;

	//! Actor for displaying the phantom geometry in phantom canvas
	vtkActor*								    m_PhantomActor;

	//! Actor for displaying the defined landmark from the configuration file
	vtkActor*								    m_RequestedLandmarkActor;

	//! Polydata holding the requested landmark for highlighting in phantom canvas
	vtkPolyData*							  m_RequestedLandmarkPolyData;

  //! Index of current landmark
	int										      m_CurrentLandmarkIndex;

protected:
  Ui::PhantomRegistrationToolbox ui;

};

#endif
