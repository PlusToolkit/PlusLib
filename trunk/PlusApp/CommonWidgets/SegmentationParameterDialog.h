/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef SEGMENTATIONPARAMETERDIALOG_H
#define SEGMENTATIONPARAMETERDIALOG_H

#include "ui_SegmentationParameterDialog.h"

#include "PlusConfigure.h"

#include <QDialog>

class QTimer;

class FidPatternRecognition;

class vtkActor;
class vtkDataCollector;
class vtkImageActor;
class vtkImageVisualizer;
class vtkPolyData;
class vtkROIModeHandler;
class vtkSpacingModeHandler;

//-----------------------------------------------------------------------------

/*! \class SegmentationParameterDialog 
 * \brief Segmentation parameter setting dialog class
 * \ingroup PlusAppCommonWidgets
 */
class SegmentationParameterDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	* Constructor
	* \param aParent Parent object
	* \param aDataCollector Data collector (needed for the image output)
	*/
	SegmentationParameterDialog(QWidget* aParent, vtkDataCollector* aDataCollector);

	/*!
	* Destructor
	*/
	~SegmentationParameterDialog();

	/*!
	* Return canvas renderer
  * \return Canvas Renderer
	*/
  vtkRenderer* GetCanvasRenderer() { return m_CanvasRenderer; };

	/*!
	* Return image frame size
  * \param Output array for image dimensions
  * \return Success flag
	*/
  PlusStatus GetFrameSize(int aImageDimensions[2]);

	/*!
	* Set ROI values to spinBoxes (if a values is -1, it is not set - because usually only 2 values change at a time)
  * \return Success flag
	*/
  PlusStatus SetROI(int aXMin, int aYMin, int aXMax, int aYMax);

	/*!
	* Get ROI values from spinBoxes
  * \return Success flag
	*/
  PlusStatus GetROI(int &aXMin, int &aYMin, int &aXMax, int &aYMax);

	/*!
	* Compute and set spacing according to summed measured length (got from the mode handler)
  * \return Success flag
	*/
  PlusStatus ComputeSpacingFromMeasuredLengthSum();

	/*!
	* Get reference width from spinbox value
  * \return Reference width mm
	*/
  double GetSpacingReferenceWidth();

	/*!
	* Get reference height from spinbox value
  * \return Reference height mm
	*/
  double GetSpacingReferenceHeight();

  /*!
	* Get original spacing
  * \return Mm per pixel spacing (from input configuration)
	*/
  double GetApproximateSpacingMmPerPixel() { return m_ApproximateSpacingMmPerPixel; };

protected:
	/*!
	* Initialize visualization (actors, cameras etc.)
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

  /*!
	* Parses configuration data and fills form (segmentation parameters)
  * \return Success flag
	*/
  PlusStatus ReadConfiguration();

  /*!
	* Write configuration XML data based on the input fields on the GUI
  * \return Success flag
	*/
  PlusStatus WriteConfiguration();

  /*!
	* Switch to ROI mode - canvas events will answer to events of ROI mode
  * \return Success flag
	*/
	PlusStatus SwitchToROIMode();

  /*!
	* Switch to Spacing mode - canvas events will answer to events of Spacing mode
  * \return Success flag
	*/
	PlusStatus SwitchToSpacingMode();

  /*!
	* Segments the currently displayed image and draws the result on the canvas
  * \return Success flag
	*/
  PlusStatus SegmentCurrentImage();

protected slots:
	/*!
	* Applies the configuration to the data element and closes window
	*/
	void ApplyAndCloseClicked();

	/*!
	* Shows config file saver dialog, saves the configuration file and closes window
	*/
	void SaveAndCloseClicked();

	/*!
	* Slot handling ROI groupbox toggle
  * \param aOn True if checked, false if unchecked
	*/
  void GroupBoxROIToggled(bool aOn);

	/*!
	* Slot handling spacing groupbox toggle
  * \param aOn True if checked, false if unchecked
	*/
  void GroupBoxSpacingToggled(bool aOn);

	/*!
	* Resize event handler
  * \param aEvent Resize event
	*/
  virtual void resizeEvent(QResizeEvent* aEvent);

	/*!
	* Slot catching refresh timer events and refreshing the canvas
	*/
  void UpdateCanvas();

	/*!
	* Freeze / Unfreeze image
  * \param aOn True if checked (freeze), false if unchecked (unfreeze)
	*/
  void FreezeImage(bool aOn);

	/*!
	* Slot handling ROI XMin value change
  * \param aValue New value
	*/
  void ROIXMinChanged(int aValue);

	/*!
	* Slot handling ROI YMin value change
  * \param aValue New value
	*/
  void ROIYMinChanged(int aValue);

	/*!
	* Slot handling ROI XMax value change
  * \param aValue New value
	*/
  void ROIXMaxChanged(int aValue);

	/*!
	* Slot handling ROI YMax value change
  * \param aValue New value
	*/
  void ROIYMaxChanged(int aValue);

	/*!
	* Slot handling reference width value change
  * \param aValue New value
	*/
  void ReferenceWidthChanged(int aValue);

	/*!
	* Slot handling reference height value change
  * \param aValue New value
	*/
  void ReferenceHeightChanged(int aValue);

	/*!
	* Slot handling opening circle radius value change
  * \param aValue New value
	*/
  void OpeningCircleRadiusChanged(double aValue);

	/*!
	* Slot handling opening bar size value change
  * \param aValue New value
	*/
  void OpeningBarSizeChanged(double aValue);

	/*!
	* Slot handling line pair distance error value change
  * \param aValue New value
	*/
  void LinePairDistanceErrorChanged(double aValue);

	/*!
	* Slot handling angle difference value change
  * \param aValue New value
	*/
  void AngleDifferenceChanged(double aValue);

	/*!
	* Slot handling minimum theta value change
  * \param aValue New value
	*/
  void MinThetaChanged(double aValue);

	/*!
	* Slot handling maximum theta value change
  * \param aValue New value
	*/
  void MaxThetaChanged(double aValue);

  /*!
	* Slot handling angle tolerance value change
  * \param aValue New value
	*/
  void AngleToleranceChanged(double aValue);

	/*!
	* Slot handling line 3rd point distance value change
  * \param aValue New value
	*/
  void CollinearPointsMaxDistanceFromLineChanged(double aValue);

	/*!
	* Slot handling image threshold value change
  * \param aValue New value
	*/
  void ImageThresholdChanged(double aValue);

	/*!
	* Slot handling original intensity for dots chechbox toggle
  * \param aOn New state
	*/
  void OriginalIntensityForDotsToggled(bool aOn);

protected:
  /*! Data collector */
  vtkDataCollector*         m_DataCollector;

	/*! Actor for displaying segmented points */
	vtkActor*                 m_SegmentedPointsActor;

	/*! Poly data for holding the segmented points */
	vtkPolyData*              m_SegmentedPointsPolyData;

	/*! Poly data for holding the fiducial candidates */
	vtkPolyData*              m_CandidatesPolyData;

  /*! ROI mode handler callback command instance */
  vtkROIModeHandler*        m_ROIModeHandler;

  /*! Spacing mode handler callback command instance */
  vtkSpacingModeHandler*    m_SpacingModeHandler;

  /*! Renderer for the canvas */
	vtkRenderer*			        m_CanvasRenderer; 

  /*! 2D Image Visualization */
  vtkImageVisualizer*       m_ImageVisualizer;

  /*! Timer for refreshing the canvas */
  QTimer*                   m_CanvasRefreshTimer;

  /*! Original mm per pixel spacing (from input configuration) */
  double                    m_ApproximateSpacingMmPerPixel;

  /*! Pattern recognition object for segmenting the images */
  FidPatternRecognition*    m_PatternRecognition;

  /*! Flag indicating if image is frozen (using Freeze button) */
  bool                      m_ImageFrozen;

protected:
	Ui::SegmentationParameterDialog ui;
};

#endif 
