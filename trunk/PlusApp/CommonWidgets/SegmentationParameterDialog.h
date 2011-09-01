#ifndef SEGMENTATIONPARAMETERDIALOG_H
#define SEGMENTATIONPARAMETERDIALOG_H

#include "ui_SegmentationParameterDialog.h"

#include "PlusConfigure.h"

#include <QDialog>

class QTimer;

class vtkDataCollector;
class vtkCalibrationController;

class vtkActor;
class vtkImageActor;
class vtkPolyData;

class vtkROIModeHandler;
class vtkSpacingModeHandler;

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

	/*!
	* \brief Return canvas renderer
  * \return Canvas Renderer
	*/
  vtkRenderer* GetCanvasRenderer() { return m_CanvasRenderer; };

	/*!
	* \brief Return image frame size
  * \param Output array for image dimensions
  * \return Success flag
	*/
  PlusStatus GetFrameSize(int aImageDimensions[3]);

	/*!
	* \brief Set ROI values to spinBoxes (if a values is -1, it is not set - because usually only 2 values change at a time)
  * \return Success flag
	*/
  PlusStatus SetROI(int aXMin, int aYMin, int aXMax, int aYMax);

	/*!
	* \brief Get ROI values from spinBoxes
  * \return Success flag
	*/
  PlusStatus GetROI(int &aXMin, int &aYMin, int &aXMax, int &aYMax);

	/*!
	* \brief Compute and set spacing according to summed measured length (got from the mode handler)
  * \return Success flag
	*/
  PlusStatus ComputeSpacingFromMeasuredLengthSum();

	/*!
	* \brief Get reference width from spinbox value
  * \return Reference width mm
	*/
  double GetSpacingReferenceWidth();

	/*!
	* \brief Get reference height from spinbox value
  * \return Reference height mm
	*/
  double GetSpacingReferenceHeight();

  /*!
	* \brief Get original spacing
  * \return Mm per pixel spacing (from input configuration)
	*/
  double GetApproximateSpacingMmPerPixel() { return m_ApproximateSpacingMmPerPixel; };

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

  /*!
	* \brief Draw indicators of US orientation
  * \return Success flag
	*/
  PlusStatus DrawUSOrientationIndicators();

  /*!
	* \brief Segments the currently displayed image and draws the result on the canvas
  * \return Success flag
	*/
  PlusStatus SegmentCurrentImage();

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

	/*!
	* \brief Slot handling ROI XMin value change
  * \param aValue New value
	*/
  void ROIXMinChanged(int aValue);

	/*!
	* \brief Slot handling ROI YMin value change
  * \param aValue New value
	*/
  void ROIYMinChanged(int aValue);

	/*!
	* \brief Slot handling ROI XMax value change
  * \param aValue New value
	*/
  void ROIXMaxChanged(int aValue);

	/*!
	* \brief Slot handling ROI YMax value change
  * \param aValue New value
	*/
  void ROIYMaxChanged(int aValue);

	/*!
	* \brief Slot handling reference width value change
  * \param aValue New value
	*/
  void ReferenceWidthChanged(int aValue);

	/*!
	* \brief Slot handling reference height value change
  * \param aValue New value
	*/
  void ReferenceHeightChanged(int aValue);

	/*!
	* \brief Slot handling opening circle radius value change
  * \param aValue New value
	*/
  void OpeningCircleRadiusChanged(double aValue);

	/*!
	* \brief Slot handling opening bar size value change
  * \param aValue New value
	*/
  void OpeningBarSizeChanged(double aValue);

	/*!
	* \brief Slot handling line length error value change
  * \param aValue New value
	*/
  void LineLengthErrorChanged(double aValue);

	/*!
	* \brief Slot handling line pair distance error value change
  * \param aValue New value
	*/
  void LinePairDistanceErrorChanged(double aValue);

	/*!
	* \brief Slot handling line error value change
  * \param aValue New value
	*/
  void LineErrorChanged(double aValue);

	/*!
	* \brief Slot handling angle difference value change
  * \param aValue New value
	*/
  void AngleDifferenceChanged(double aValue);

	/*!
	* \brief Slot handling minimum theta value change
  * \param aValue New value
	*/
  void MinThetaChanged(double aValue);

	/*!
	* \brief Slot handling maximum theta value change
  * \param aValue New value
	*/
  void MaxThetaChanged(double aValue);

	/*!
	* \brief Slot handling line 3rd point distance value change
  * \param aValue New value
	*/
  void Line3rdPointDistChanged(double aValue);

	/*!
	* \brief Slot handling image threshold value change
  * \param aValue New value
	*/
  void ImageThresholdChanged(double aValue);

	/*!
	* \brief Slot handling original intensity for dots chechbox toggle
  * \param aOn New state
	*/
  void OriginalIntensityForDotsToggled(bool aOn);

protected:
  //! Data collector
  vtkDataCollector*         m_DataCollector;

	//! Actor displaying the image
	vtkImageActor*	          m_CanvasImageActor;

	//! Actor for displaying segmented points
	vtkActor*                 m_SegmentedPointsActor;

	//! Poly data for holding the segmented points
	vtkPolyData*              m_SegmentedPointsPolyData;

	//! Actor for displaying fiducial candidates
	vtkActor*                 m_CandidatesActor;

	//! Poly data for holding the fiducial candidates
	vtkPolyData*              m_CandidatesPolyData;

  //! ROI mode handler callback command instance
  vtkROIModeHandler*        m_ROIModeHandler;

  //! Spacing mode handler callback command instance
  vtkSpacingModeHandler*    m_SpacingModeHandler;

  //! Renderer for the canvas
	vtkRenderer*			        m_CanvasRenderer; 

  //! Timer for refreshing the canvas
  QTimer*                   m_CanvasRefreshTimer;

  //! Original mm per pixel spacing (from input configuration)
  double                    m_ApproximateSpacingMmPerPixel;

  //! Calibration controller for segmenting the images
  vtkCalibrationController* m_CalibrationController;

  //! Flag indigating if image is frozen (using Freeze button)
  bool                      m_ImageFrozen;

protected:
	Ui::SegmentationParameterDialog ui;
};

#endif 
