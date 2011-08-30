#ifndef SEGMENTATIONPARAMETERDIALOG_H
#define SEGMENTATIONPARAMETERDIALOG_H

#include "ui_SegmentationParameterDialog.h"

#include "PlusConfigure.h"

#include <QDialog>

class QTimer;

class vtkDataCollector;
class vtkActor;
class vtkImageActor;
class vtkPolyData;

//-----------------------------------------------------------------------------

/*!
* \brief Segmentation parameter setting dialog class
*/
class SegmentationParameterDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent Parent object
	* \param aDataCollector Data collector (needed for the image output and the configuration data element)
	*/
	SegmentationParameterDialog(QWidget* aParent, vtkDataCollector* aDataCollector);

	/*!
	* \brief Destructor
	*/
	~SegmentationParameterDialog();

protected:
	/*!
	* \brief Initialize visualization (actors, cameras etc.)
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

  /*!
	* \brief Parses configuration data and fills form (segmentation parameters)
  * \return Success flag
	*/
  PlusStatus ReadConfiguration();

  /*!
	* \brief Write configuration XML data based on the input fields on the GUI
  * \return Success flag
	*/
  PlusStatus WriteConfiguration();

  /*!
	* \brief Calculate image camera parameters
  * \return Success flag
	*/
	PlusStatus CalculateImageCameraParameters();

  /*!
	* \brief Switch to ROI mode - canvas events will answer to events of ROI mode
  * \return Success flag
	*/
	PlusStatus SwitchToROIMode();

  /*!
	* \brief Switch to Spacing mode - canvas events will answer to events of Spacing mode
  * \return Success flag
	*/
	PlusStatus SwitchToSpacingMode();

protected slots:
	/*!
	* \brief Saves the configuration into the data element and closes window
	*/
	void SaveAndExitClicked();

	/*!
	* \brief Slot handling ROI groupbox toggle
  * \param aOn True if checked, false if unchecked
	*/
  void GroupBoxROIToggled(bool aOn);

	/*!
	* \brief Slot handling spacing groupbox toggle
  * \param aOn True if checked, false if unchecked
	*/
  void GroupBoxSpacingToggled(bool aOn);

	/*!
	* \brief Resize event handler
  * \param aEvent Resize event
	*/
  virtual void resizeEvent(QResizeEvent* aEvent);

	/*!
	* \brief Slot catching refresh timer events and refreshing the canvas
	*/
  void UpdateCanvas();

	/*!
	* \brief Freeze / Unfreeze image
  * \param aOn True if checked (freeze), false if unchecked (unfreeze)
	*/
  void FreezeImage(bool aOn);

protected:
  //! Data collector
  vtkDataCollector* m_DataCollector;

	//! Actor displaying the image
	vtkImageActor*	  m_CanvasImageActor;

	//! Actor for displaying segmented points
	vtkActor*         m_SegmentedPointsActor;

	//! Poly data for holding the segmented points
	vtkPolyData*      m_SegmentedPointsPolyData;

	//! Actor for displaying fiducial candidates
	vtkActor*         m_CandidatesActor;

	//! Poly data for holding the fiducial candidates
	vtkPolyData*      m_CandidatesPolyData;

	//! Renderer for the canvas
	vtkRenderer*			m_CanvasRenderer; 

  //! Timer for refreshing the canvas
  QTimer*           m_CanvasRefreshTimer;

protected:
	Ui::SegmentationParameterDialog ui;
};

#endif 
