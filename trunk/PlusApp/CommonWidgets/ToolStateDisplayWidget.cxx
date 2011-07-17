#include "ToolStateDisplayWidget.h"

#include "vtkDataCollector.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

//#include <QEvent>
#include <QGridLayout>

//-----------------------------------------------------------------------------

ToolStateDisplayWidget::ToolStateDisplayWidget(QWidget* aParent, Qt::WFlags aFlags)
	: QWidget(aParent, aFlags)
	, m_DataCollector(NULL)
	, m_Initialized(false)
{
	m_ToolNameLabels.clear();
	m_ToolStateLabels.clear();

	// Create default appearance
	QGridLayout* grid = new QGridLayout(this, 1, 1, 0, 0, "");
	QLabel* uninitializedLabel = new QLabel(tr("Tool state display is unavailable until not connected to a device set."), this);
	uninitializedLabel->setWordWrap(true);
	grid->addWidget(uninitializedLabel);
	m_ToolNameLabels.push_back(uninitializedLabel);
	this->setLayout(grid);

	// Install event filter that is called on any event
	//this->installEventFilter(this);
}

//-----------------------------------------------------------------------------

ToolStateDisplayWidget::~ToolStateDisplayWidget()
{
	m_ToolNameLabels.clear();
	m_ToolStateLabels.clear();

	m_DataCollector = NULL;
}

//-----------------------------------------------------------------------------

PlusStatus ToolStateDisplayWidget::InitializeTools(vtkDataCollector* aDataCollector)
{
	m_DataCollector = aDataCollector;

	if ((m_DataCollector == NULL) || (m_DataCollector->GetTracker() == NULL)) {
		LOG_ERROR("Data collector or tracker is missing!");
		return PLUS_FAIL;
	}

	// Clear former content
	for (std::vector<QLabel*>::iterator it = m_ToolNameLabels.begin(); it != m_ToolNameLabels.end(); ++it) {
		delete (*it);
	}
	m_ToolNameLabels.clear();
	for (std::vector<QTextEdit*>::iterator it = m_ToolStateLabels.begin(); it != m_ToolStateLabels.end(); ++it) {
		delete (*it);
	}
	m_ToolStateLabels.clear();
	delete this->layout();

	// Set up layout
	QGridLayout* grid = new QGridLayout();
	grid->setHorizontalSpacing(9);
	grid->setVerticalSpacing(4);
	grid->setContentsMargins(4, 4, 4, 4);

	int defaultToolNumber = m_DataCollector->GetTracker()->GetDefaultTool();
	int referenceToolNumber = m_DataCollector->GetTracker()->GetReferenceTool();

	for (int i=0; i<m_DataCollector->GetTracker()->GetNumberOfTools(); ++i) {
		vtkTrackerTool *tool = m_DataCollector->GetTracker()->GetTool(i);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			LOG_WARNING("Tool " << i << " is not enabled or not present");
		}

		// Assemble tool name and add label to layout and label list
		QString role("");
		if (i == defaultToolNumber) {
			role.append(tr(" (Default)"));
		}
		if (i == referenceToolNumber) {
			role.append(tr(" (Reference)"));
		}

		QString toolNameString = QString("%1%2: %3").arg(i).arg(role).arg(tool->GetToolName());

		QLabel* toolNameLabel = new QLabel(this);
		toolNameLabel->setText(toolNameString);
		grid->addWidget(toolNameLabel, i, 0);
		m_ToolNameLabels.push_back(toolNameLabel);

		// Create tool status label and add it to layout and label list
		QTextEdit* toolStateLabel = new QTextEdit("N/A", this);
		m_ToolStateLabels.push_back(toolStateLabel);
	}
	
	this->setLayout(grid);

	m_Initialized = true;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool ToolStateDisplayWidget::IsInitialized()
{
	return m_Initialized;
}

//-----------------------------------------------------------------------------

PlusStatus ToolStateDisplayWidget::Update()
{
	if (! m_Initialized) {
		LOG_ERROR("Widget is not inialized!");
		return PLUS_FAIL;
	}
	if (m_ToolStateLabels.size() != m_DataCollector->GetTracker()->GetNumberOfTools()) {
		LOG_ERROR("Tool number inconsistency!");
		return PLUS_FAIL;
	}

	for (int i=0; i<m_DataCollector->GetTracker()->GetNumberOfTools(); ++i) {
		vtkTrackerTool *tool = m_DataCollector->GetTracker()->GetTool(i);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			LOG_WARNING("Tool " << i << " is not enabled or not present");
		}

		TrackerStatus status = TR_MISSING;
		TrackerBufferItem* latestItem = NULL;
		if (tool->GetBuffer()->GetLatestTrackerBufferItem(latestItem) != ITEM_OK) {
			LOG_WARNING("Latest tracker buffer item is not available");
			m_ToolStateLabels.at(i)->setText("BUFFER ERROR");
			m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(223, 0, 0));
		} else {
			switch (latestItem->GetStatus()) {
				case (TR_OK):
					m_ToolStateLabels.at(i)->setText("OK");
					m_ToolStateLabels.at(i)->setTextColor(Qt::green);
					break;
				case (TR_MISSING):
					m_ToolStateLabels.at(i)->setText("MISSING");
					m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(223, 0, 0));
					break;
				case (TR_OUT_OF_VIEW):
					m_ToolStateLabels.at(i)->setText("OUT OF VIEW");
					m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(255, 128, 0));
					break;
				case (TR_OUT_OF_VOLUME):
					m_ToolStateLabels.at(i)->setText("OUT OF VOLUME");
					m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(255, 128, 0));
					break;
				case (TR_REQ_TIMEOUT):
					m_ToolStateLabels.at(i)->setText("TIMEOUT");
					m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(223, 0, 0));
					break;
				default:
					m_ToolStateLabels.at(i)->setText("UNKNOWN");
					m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(223, 0, 0));
					break;
			}
		}
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool ToolStateDisplayWidget::eventFilter(QObject *obj, QEvent *ev)
{
	/*
	if ( obj == this ) {
		if ( ev->type() == QEvent::Enter ) {
			m_PreviousScroll = 0;
			// Construct and show message list widget
			if (ConstructMessageListWidget() == PLUS_SUCCESS) {
				m_MessageListWidget->move( mapToGlobal( QPoint( m_DotLabel->x() - m_MessageListWidget->maximumWidth() + 38, m_DotLabel->y() - m_MessageListWidget->maximumHeight() - 6 ) ) );
				m_MessageListWidget->show();
			}
		} else if ( ev->type() == QEvent::Leave ) {
			if (m_MessageListWidget) {
				m_MessageListWidget->hide();
			}
		} else {
			// Pass the event on to the parent class
			return QWidget::eventFilter( obj, ev );
		}
	}
	*/
	return true;
}
