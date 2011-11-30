/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "ToolStateDisplayWidget.h"

#include "vtkDataCollector.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

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
  for (std::vector<QLabel*>::iterator it = m_ToolNameLabels.begin(); it != m_ToolNameLabels.end(); ++it)
  {
    delete (*it);
  }
  m_ToolNameLabels.clear();
  for (std::vector<QTextEdit*>::iterator it = m_ToolStateLabels.begin(); it != m_ToolStateLabels.end(); ++it)
  {
    delete (*it);
  }
  m_ToolStateLabels.clear();

  // If connection was unsuccessful, create default appearance
  if (! aConnectionSuccessful)
  {
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
  if (m_DataCollector == NULL)
  {
    LOG_ERROR("Data collector is missing!");
    return PLUS_FAIL;
  }

  // Get transforms
  std::vector<PlusTransformName> transformNames;
  TrackedFrame trackedFrame;
  m_DataCollector->GetTrackedFrame(&trackedFrame);
  trackedFrame.GetCustomFrameTransformNameList(transformNames);

  // Set up layout
  QGridLayout* grid = new QGridLayout(this, transformNames.size(), 2, 1);
  grid->setHorizontalSpacing(2);
  grid->setVerticalSpacing(4);
  grid->setContentsMargins(4, 4, 4, 4);

  m_ToolStateLabels.resize(transformNames.size(), NULL);

  int i;
  std::vector<PlusTransformName>::iterator it;
  for (it = transformNames.begin(), i = 0; it != transformNames.end(); ++it, ++i)
  {
    // Assemble tool name and add label to layout and label list
    QString toolNameString = QString("%1: %2").arg(i).arg(it->From().c_str());

    QLabel* toolNameLabel = new QLabel(this);
    toolNameLabel->setText(toolNameString);
    toolNameLabel->setToolTip(toolNameString);
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
    m_ToolStateLabels[i] = toolStateLabel;
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

  if ( m_DataCollector == NULL )
  {
    return 23; 
  }

  // Get transforms
  std::vector<PlusTransformName> transformNames;
  TrackedFrame trackedFrame;
  m_DataCollector->GetTrackedFrame(&trackedFrame);
  trackedFrame.GetCustomFrameTransformNameList(transformNames);

  int numberOfTools = transformNames.size();

  return ((numberOfTools > 0) ? (numberOfTools * 23) : 23);
}

//-----------------------------------------------------------------------------

PlusStatus ToolStateDisplayWidget::Update()
{
  if (! m_Initialized)
  {
    LOG_ERROR("Widget is not inialized!");
    return PLUS_FAIL;
  }

  // Re-initialize widget if enabled statuses have changed
  int numberOfDisplayedTools = 0;

  // Get transforms
  std::vector<PlusTransformName> transformNames;
  TrackedFrame trackedFrame;
  m_DataCollector->GetTrackedFrame(&trackedFrame);
  trackedFrame.GetCustomFrameTransformNameList(transformNames);

  std::vector<PlusTransformName>::iterator transformIt;
  std::vector<QTextEdit*>::iterator labelIt;
  for (transformIt = transformNames.begin(), labelIt = m_ToolStateLabels.begin(); transformIt != transformNames.end(); ++transformIt, ++labelIt)
  {
    QTextEdit* label = (*labelIt);

    if (label == NULL)
    {
      LOG_WARNING("Invalid tool state label");
      continue;
    }

    TrackerStatus status = TR_MISSING;
    if (trackedFrame.GetCustomFrameTransformStatus(*transformIt, status) != PLUS_SUCCESS)
    {
      std::string transformNameStr;
      transformIt->GetTransformName(transformNameStr);
      LOG_WARNING("Unable to get transform status for transform" << transformNameStr);
      label->setText("STATUS ERROR");
      label->setTextColor(QColor::fromRgb(223, 0, 0));
    }
    else
    {
      switch (status)
      {
        case (TR_OK):
          label->setText("OK");
          label->setTextColor(Qt::green);
          break;
        case (TR_MISSING):
          label->setText("MISSING");
          label->setTextColor(QColor::fromRgb(223, 0, 0));
          break;
        case (TR_OUT_OF_VIEW):
          label->setText("OUT OF VIEW");
          label->setTextColor(QColor::fromRgb(255, 128, 0));
          break;
        case (TR_OUT_OF_VOLUME):
          label->setText("OUT OF VOLUME");
          label->setTextColor(QColor::fromRgb(255, 128, 0));
          break;
        case (TR_REQ_TIMEOUT):
          label->setText("TIMEOUT");
          label->setTextColor(QColor::fromRgb(223, 0, 0));
          break;
        default:
          label->setText("UNKNOWN");
          label->setTextColor(QColor::fromRgb(223, 0, 0));
          break;
      }
    }
  }

  if (labelIt != m_ToolStateLabels.end())
  {
    LOG_WARNING("Tool number inconsistency!");

    if (InitializeTools(m_DataCollector, true) != PLUS_SUCCESS)
    {
      LOG_ERROR("Re-initializing tool state widget failed");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}
