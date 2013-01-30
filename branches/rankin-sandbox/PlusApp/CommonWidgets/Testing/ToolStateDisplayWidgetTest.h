/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __ToolStateDisplayWidgetTest_h
#define __ToolStateDisplayWidgetTest_h

#include "PlusConfigure.h"

#include <QDialog>

class vtkDataCollector;
class DeviceSetSelectorWidget;
class ToolStateDisplayWidget;

//-----------------------------------------------------------------------------

/*! \class ToolStateDisplayWidgetTest 
 * \brief Qt application for testing ToolStateDisplayWidget
 * \ingroup PlusAppCommonWidgets
 */
class ToolStateDisplayWidgetTest : public QDialog
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	ToolStateDisplayWidgetTest(QWidget *parent = 0, Qt::WFlags flags = 0);

	/*!
	* \brief Destructor
	*/
	~ToolStateDisplayWidgetTest();

protected slots:
	/*!
	* \brief Connect to devices described in the argument configuration file in response by clicking on the Connect button
	* \param aConfigFile DeviceSet configuration file path and name
	*/
	void ConnectToDevicesByConfigFile(std::string aConfigFile);

  /*!
   * Refreshes the tool display widget
   */
  void RefreshToolDisplay();

protected:
	/*!
	 * \brief Read configuration file and start data collection
	 * \return Success flag
	 */
  PlusStatus StartDataCollection();

protected:
  /*! Device set selector widget */
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

	/*! Tool state display widget */
	ToolStateDisplayWidget*		m_ToolStateDisplayWidget;

	/*! Data collector object */
	vtkDataCollector*	        m_DataCollector;

};

#endif // __ToolStateDisplayWidgetTest_h
