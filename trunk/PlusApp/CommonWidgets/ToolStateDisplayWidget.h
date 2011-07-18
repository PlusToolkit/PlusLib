#ifndef TOOLSTATEDISPLAYWIDGET_H
#define TOOLSTATEDISPLAYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>

#include "PlusConfigure.h"

class vtkDataCollector;

//-----------------------------------------------------------------------------

/*!
* \brief Widget that shows state of all tools available to the tracker
*/
class ToolStateDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	ToolStateDisplayWidget(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~ToolStateDisplayWidget();

	/*!
	* \brief Read tools and set up widget
	* \param aDataCollector Data collector instance
	* \param aConnectionSuccessful Flag if connection was successful (sets to uninitialized if not and displays default appearance)
	* \return Success flag
	*/
	PlusStatus InitializeTools(vtkDataCollector* aDataCollector, bool aConnectionSuccessful);

	/*!
	* \brief Get tool statuses and display them
	*/
	PlusStatus Update();

	/*!
	* \brief Getter function
	* \return Initialization state
	*/
	bool IsInitialized();

protected:
	//! Data collector
	vtkDataCollector*		m_DataCollector;

	//! List of tool name labels
	std::vector<QLabel*>	m_ToolNameLabels;

	//! List of tool status labels
	std::vector<QTextEdit*>	m_ToolStateLabels;

	//! Flag showing if the widget is initialized (the tools are loaded)
	bool					m_Initialized;

};

#endif
