#include "ToolStateDisplayWidget.h"

#include "vtkDataCollector.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include <QGridLayout>

//-----------------------------------------------------------------------------

ToolStateDisplayWidget::ToolStateDisplayWidget(QWidget* aParent, Qt::WFlags aFlags)
	: QWidget(aParent, aFlags)
	, m_DataCollector(NULL)
	, m_Initialized(false)
	, m_NumberOfActiveTools(-1)
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

PlusStatus ToolStateDisplayWidget::InitializeTools(vtkDataCollector* aDataCollector, bool aConnectionSuccessful)
{
	LOG_TRACE("ToolStateDisplayWidget::InitializeTools"); 

	// Clear former content
	if (this->layout()) {
		delete this->layout();
	}
	for (std::vector<QLabel*>::iterator it = m_ToolNameLabels.begin(); it != m_ToolNameLabels.end(); ++it) {
		delete (*it);
	}
	m_ToolNameLabels.clear();
	for (std::vector<QTextEdit*>::iterator it = m_ToolStateLabels.begin(); it != m_ToolStateLabels.end(); ++it) {
		delete (*it);
	}
	m_ToolStateLabels.clear();

	// If connection was unsuccessful, create default appearance
	if (! aConnectionSuccessful) {
		QGridLayout* grid = new QGridLayout(this, 1, 1, 0, 0, "");
		QLabel* uninitializedLabel = new QLabel(tr("Tool state display is unavailable until not connected to a device set."), this);
		uninitializedLabel->setWordWrap(true);
		grid->addWidget(uninitializedLabel);
		m_ToolNameLabels.push_back(uninitializedLabel);
		this->setLayout(grid);

		m_Initialized = false;

		return PLUS_SUCCESS;
	}


	m_DataCollector = aDataCollector;

	// Fail if data collector or tracker is not initialized (once the content was deleted)
	if ((m_DataCollector == NULL) || (m_DataCollector->GetTracker() == NULL)) {
		LOG_ERROR("Data collector or tracker is missing!");
		return PLUS_FAIL;
	}

	// Set up layout
	QGridLayout* grid = new QGridLayout(this, m_DataCollector->GetTracker()->GetNumberOfTools(), 2, 1);
	grid->setHorizontalSpacing(2);
	grid->setVerticalSpacing(4);
	grid->setContentsMargins(4, 4, 4, 4);

	m_NumberOfActiveTools = 0;

	for (int i=0; i<m_DataCollector->GetTracker()->GetNumberOfTools(); ++i) {
		vtkTrackerTool *tool = m_DataCollector->GetTracker()->GetTool(i);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			LOG_DEBUG("Tool " << i << " is not enabled or not present");
			continue;
		} else {
			m_NumberOfActiveTools++;
		}

		// Assemble tool name and add label to layout and label list
    std::string toolType;
		vtkTracker::ConvertToolTypeToString(tool->GetToolType(), toolType);
		QString toolNameString = QString("%1: %2.%3").arg(i).arg(toolType.c_str()).arg(tool->GetToolName());

		QLabel* toolNameLabel = new QLabel(this);
		toolNameLabel->setText(toolNameString);
		QSizePolicy sizePolicyNameLabel(QSizePolicy::Expanding, QSizePolicy::Preferred);
		sizePolicyNameLabel.setHorizontalStretch(2);
		toolNameLabel->setSizePolicy(sizePolicyNameLabel);
		grid->addWidget(toolNameLabel, i, 0, Qt::AlignLeft);
		m_ToolNameLabels.push_back(toolNameLabel);

		// Create tool status label and add it to layout and label list
		QTextEdit* toolStateLabel = new QTextEdit("N/A", this);
		toolStateLabel->setTextColor(QColor::fromRgb(96, 96, 96));
		toolStateLabel->setMaximumHeight(18);
		toolStateLabel->setLineWrapMode(QTextEdit::NoWrap);
		toolStateLabel->setReadOnly(true);
		toolStateLabel->setFrameShape(QFrame::NoFrame);
		toolStateLabel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		toolStateLabel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		toolStateLabel->setAlignment(Qt::AlignRight);
		QSizePolicy sizePolicyStateLabel(QSizePolicy::Preferred, QSizePolicy::Fixed);
		sizePolicyStateLabel.setHorizontalStretch(1);
		toolStateLabel->setSizePolicy(sizePolicyStateLabel);
		grid->addWidget(toolStateLabel, i, 1, Qt::AlignRight);
		m_ToolStateLabels.push_back(toolStateLabel);
	}
	
	this->setLayout(grid);

	m_Initialized = true;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool ToolStateDisplayWidget::IsInitialized()
{
	LOG_TRACE("ToolStateDisplayWidget::IsInitialized"); 

	return m_Initialized;
}

//-----------------------------------------------------------------------------

int ToolStateDisplayWidget::GetDesiredHeight()
{
	LOG_TRACE("ToolStateDisplayWidget::GetDesiredHeight"); 

	return (m_NumberOfActiveTools>0 ? m_NumberOfActiveTools * 23 : 23);
}

//-----------------------------------------------------------------------------

PlusStatus ToolStateDisplayWidget::Update()
{
	if (! m_Initialized) {
		LOG_ERROR("Widget is not inialized!");
		return PLUS_FAIL;
	}
	if (m_ToolStateLabels.size() != m_NumberOfActiveTools) {
		LOG_ERROR("Tool number inconsistency!");
		return PLUS_FAIL;
	}

	for (int i=0; i<m_DataCollector->GetTracker()->GetNumberOfTools(); ++i) {
		vtkTrackerTool *tool = m_DataCollector->GetTracker()->GetTool(i);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			continue;
		}

		TrackerStatus status = TR_MISSING;
		TrackerBufferItem latestItem;
		if (tool->GetBuffer()->GetLatestTrackerBufferItem(&latestItem) != ITEM_OK) {
			LOG_WARNING("Latest tracker buffer item is not available");
			m_ToolStateLabels.at(i)->setText("BUFFER ERROR");
			m_ToolStateLabels.at(i)->setTextColor(QColor::fromRgb(223, 0, 0));
		} else {
			switch (latestItem.GetStatus()) {
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
