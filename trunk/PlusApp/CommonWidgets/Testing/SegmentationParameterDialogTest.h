/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __fCalMainWindow_h
#define __fCalMainWindow_h

#include "PlusConfigure.h"

#include <QDialog>

class vtkDataCollector;
class DeviceSetSelectorWidget;

//-----------------------------------------------------------------------------

/*! \class SegmentationParameterDialogTest 
 *
 * \brief Qt application for testing SegmentationParameterDialog and for using the dialog outside fCal
 *
 * \ingroup PlusAppCommonWidgets
 *
 */
class SegmentationParameterDialogTest : public QDialog
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	SegmentationParameterDialogTest(QWidget *parent = 0, Qt::WFlags flags = 0);

	/*!
	* \brief Destructor
	*/
	~SegmentationParameterDialogTest();

protected slots:
	/*!
	* \brief Connect to devices described in the argument configuration file in response by clicking on the Connect button
	* \param aConfigFile DeviceSet configuration file path and name
	*/
	void ConnectToDevicesByConfigFile(std::string aConfigFile);

	/*!
	* \brief Shows configuration file saver dialog that enables saving the new segmentation parameters
	*/
  void SaveConfigurationClicked();

protected:
	/*!
	 * \brief Read configuration file and start data collection
	 * \return Success flag
	 */
  PlusStatus StartDataCollection();

protected:
  //! Device set selector widget
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

	//! Data collector object
	vtkDataCollector*	        m_DataCollector;

  //! Save configuration button
  QPushButton*              m_SaveButton;

};

#endif // __fCalMainWindow_h
