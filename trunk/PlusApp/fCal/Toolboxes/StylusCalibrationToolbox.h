#ifndef __StylusCalibrationToolbox_h
#define __StylusCalibrationToolbox_h

#include "ui_StylusCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkPivotCalibrationAlgo;

//-----------------------------------------------------------------------------

/*!
* \brief Stylus calibration toolbox view class
*/
class StylusCalibrationToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
  * \param aParentMainWindow Parent main window
	* \param aFlags Widget flags
	*/
	StylusCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~StylusCalibrationToolbox();

  /*!
	* \brief Initialize - implementation of a pure virtual function
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
	* \brief Return pivot calibration algorithm object
  * \return Pivot calibration algo
	*/
  vtkPivotCalibrationAlgo* GetPivotCalibrationAlgo() { return m_PivotCalibration; };

protected slots:
	/*!
	* \brief Start calibration
	*/
	void Start();

	/*!
	* \brief Stop calibration
	*/
	void Stop();

	/*!
	* \brief Save result to XML file
	*/
	void Save();

	/*!
	* \brief Slot handling change of stylus calibration number of points
	* \param aLevel New number of points
	*/
	void NumberOfStylusCalibrationPointsChanged(int aNumberOfPoints);

  /*!
  * \brief Acquire stylus position and add it to the algorithm (called by the acquisition timer in tool visualizer)
  */
  void AddStylusPositionToCalibration();

protected:
  //! Pivot calibration algorithm
  vtkPivotCalibrationAlgo*  m_PivotCalibration;

	//! Number of points to acquire
	int									      m_NumberOfPoints;

	//! Number of points acquired so far
	int									      m_CurrentPointNumber;

	//! Stylus or stylus tip position (depending on the state) as string
	std::string							  m_StylusPositionString;

protected:
	Ui::StylusCalibrationToolbox ui;

};

#endif
