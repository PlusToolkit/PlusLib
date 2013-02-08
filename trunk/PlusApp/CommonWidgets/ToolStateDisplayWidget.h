/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef TOOLSTATEDISPLAYWIDGET_H
#define TOOLSTATEDISPLAYWIDGET_H

#include "PlusConfigure.h"
#include <QLabel>
#include <QTextEdit>
#include <QWidget>

class vtkPlusChannel;

//-----------------------------------------------------------------------------

/*! \class ToolStateDisplayWidget 
 * \brief Widget that shows state of all tools available to the tracker
 * \ingroup PlusAppCommonWidgets
 */
class ToolStateDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	/*!
	* Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	ToolStateDisplayWidget(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* Destructor
	*/
	~ToolStateDisplayWidget();

	/*!
	* Read tools and set up widget
	* \param aDataCollector Data collector instance
	* \param aConnectionSuccessful Flag if connection was successful (sets to uninitialized if not and displays default appearance)
	* \return Success flag
	*/
	PlusStatus InitializeTools(vtkPlusChannel* aChannel, bool aConnectionSuccessful);

	/*!
	* Get tool statuses and display them
	*/
	PlusStatus Update();

	/*!
	* Get initialization state
	* \return Initialization state
	*/
	bool IsInitialized();

	/*!
	* Get desired height
	* \return Desired widget height in pixels (according to the number of active tools)
	*/
	int GetDesiredHeight();

protected:
	/*! Data source */
	vtkPlusChannel*		m_SelectedChannel;

	/*! List of tool name labels */
	std::vector<QLabel*>	m_ToolNameLabels;

	/*! List of tool status labels */
	std::vector<QTextEdit*>	m_ToolStateLabels;

	/*! Flag showing if the widget is initialized (the tools are loaded) */
	bool					m_Initialized;
};

#endif
