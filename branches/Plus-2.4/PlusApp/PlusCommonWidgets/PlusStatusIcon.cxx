/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusStatusIcon.h"
#include <QCoreApplication>
#include <QGridLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
namespace
{
  const QString ERROR_HTML = "<font color=\"#DF0000\">";
  const QString WARNING_HTML = "<font color=\"#FF8000\">";
  const QString INFO_HTML = "<font color=\"Black\">";
  const QString END_HTML = "</font>";
}
//-----------------------------------------------------------------------------

PlusStatusIcon::PlusStatusIcon(QWidget* aParent, Qt::WindowFlags aFlags)
  : QWidget(aParent, aFlags)
  , m_Level(vtkPlusLogger::LOG_LEVEL_INFO)
  , m_DotLabel(NULL)
  , m_MessageListFrame(NULL)
  , m_MessageTextEdit(NULL)
  , m_DisplayMessageCallbackTag(0)
  , m_MaxMessageCount(vtkPlusLogger::UnlimitedLogMessages())
{
  this->setMinimumSize(18, 16);
  this->setMaximumSize(18, 16);
  this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  // Set up layout and create dot label
  QGridLayout* grid = new QGridLayout();
  grid->setSpacing(0);
  grid->setContentsMargins(0, 0, 2, 0);

  m_DotLabel = new QLabel(this);
  m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );
  grid->addWidget(m_DotLabel);

  this->setLayout(grid);

  if (m_DotLabel == NULL) {
    LOG_ERROR("Status icon cannot be initialized!");
    return;
  }

  // Set callback for logger to display errors
  vtkSmartPointer<vtkDisplayMessageCallback> cb = vtkSmartPointer<vtkDisplayMessageCallback>::New();
  m_DisplayMessageCallbackTag = vtkPlusLogger::Instance()->AddObserver(vtkCommand::UserEvent, cb);

  connect( cb, SIGNAL( AddMessage(QString) ), this, SLOT( AddMessage(QString) ) );

  // Install event filter that is called on any event
  this->installEventFilter(this);

  if (ConstructMessageListWidget() != PLUS_SUCCESS) {
    LOG_ERROR("Message list widget cannot be initialized!");
    return;
  }
}

//-----------------------------------------------------------------------------

PlusStatusIcon::~PlusStatusIcon()
{
  vtkPlusLogger::Instance()->RemoveObserver(m_DisplayMessageCallbackTag);
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::AddMessage(QString aInputString)
{
  // No matter what store the log entry to be able to reconstruct it later
  m_MessageLog.push_back(aInputString);

  if( m_MaxMessageCount != vtkPlusLogger::UnlimitedLogMessages() && m_MessageTextEdit->document()->lineCount() > m_MaxMessageCount )
  {
    // Clear earliest messages
    int linesToDelete( m_MessageTextEdit->document()->lineCount()-m_MaxMessageCount*0.80 );
    m_MessageLog.erase(m_MessageLog.begin(), m_MessageLog.begin()+linesToDelete);
  }

  if( m_FilterLineEdit->text().isEmpty() || aInputString.contains(m_FilterLineEdit->text(), Qt::CaseInsensitive) )
  {
    this->ParseMessage(aInputString);
  }
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::ParseMessage(QString &aInputString)
{
  // Parse input string and extract log level and the message
  bool ok;
  unsigned int pos = aInputString.indexOf('|');

  int logLevel = aInputString.left(pos).toInt(&ok);
  if (! ok) {
    logLevel = -1;
  }

  QString message;

  // Re-color dot and message text if necessary
  switch (logLevel) {
  case vtkPlusLogger::LOG_LEVEL_ERROR:
    if (m_Level > vtkPlusLogger::LOG_LEVEL_ERROR) {
      m_Level = vtkPlusLogger::LOG_LEVEL_ERROR;
      m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotRed.png" ) );
    }
    message = ERROR_HTML;
    break;
  case vtkPlusLogger::LOG_LEVEL_WARNING:
    if (m_Level > vtkPlusLogger::LOG_LEVEL_WARNING) {
      m_Level = vtkPlusLogger::LOG_LEVEL_WARNING;
      m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotOrange.png" ) );
    }
    message = WARNING_HTML;
    break;
  default:
    message = INFO_HTML;
    break;
  }

  message = message.append(aInputString.right( aInputString.size() - pos - 1 )).append(END_HTML);

  if( m_MaxMessageCount != vtkPlusLogger::UnlimitedLogMessages() && m_MessageTextEdit->document()->lineCount() > m_MaxMessageCount )
  {   
    QTextCursor tc = m_MessageTextEdit->textCursor();

    // Store original selection
    int originalSelectionStart=tc.selectionStart();
    int originalSelectionLength=tc.selectionEnd()-originalSelectionStart;

    // Keep only the most recent 80% of the lines, delete the rest
    int linesToDelete=m_MessageTextEdit->document()->lineCount()-m_MaxMessageCount*0.80;
    tc.movePosition( QTextCursor::Start );
    for (int i=0; i<linesToDelete; i++)
    {
      tc.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
      tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    }
    originalSelectionStart -= (tc.selectionEnd()-tc.selectionStart());
    tc.removeSelectedText();

    // Restore original selection
    if (originalSelectionStart>0 && originalSelectionLength>0)
    {
      // if there was a selection, then restore it
      tc.movePosition(QTextCursor::Start);
      tc.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,originalSelectionStart);
      m_MessageTextEdit->setTextCursor( tc );
      m_MessageTextEdit->ensureCursorVisible();
      tc.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,originalSelectionLength);
      m_MessageTextEdit->setTextCursor( tc );
      m_MessageTextEdit->ensureCursorVisible();
    }
    else
    {
      // there was no selection, so just follow the tail
      m_MessageTextEdit->moveCursor (QTextCursor::End);
      m_MessageTextEdit->setTextCursor( tc );
      QCoreApplication::processEvents(); // need to call this to update the max position of the slider
      QScrollBar *vScrollBar = m_MessageTextEdit->verticalScrollBar();
      if (vScrollBar)
      {
        vScrollBar->triggerAction(QScrollBar::SliderToMaximum);
      }
    }
  }

  m_MessageTextEdit->append(message);
}

//-----------------------------------------------------------------------------

PlusStatus PlusStatusIcon::ConstructMessageListWidget()
{
  LOG_TRACE("ToolStateDisplayWidget::ConstructMessageListWidget"); 

  // (Re-)Create message list widget
  if (m_MessageListFrame != NULL) 
  {
    delete m_MessageListFrame;
    m_MessageListFrame = NULL;
    m_MessageTextEdit = NULL;
    m_FilterLineEdit = NULL;
    m_ClearFilterButton = NULL;
  }

  m_MessageListFrame = new QFrame(this, Qt::Window|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);
  m_MessageListFrame->setMinimumSize(480, 480);
  m_MessageListFrame->resize(480, 480);

  if (m_MessageListFrame == NULL) {
    LOG_ERROR("Message list widget cannot be created!");
    return PLUS_FAIL;
  }

  // Install event filter to properly handle close
  m_MessageListFrame->installEventFilter(this);

  // Set inner part of message list widget
  QVBoxLayout* vbox = new QVBoxLayout(m_MessageListFrame);
  vbox->setSpacing(0);
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->setAlignment(Qt::AlignTop);
  
  m_MessageTextEdit = new QTextEdit(m_MessageListFrame);
  m_MessageTextEdit->setWordWrapMode(QTextOption::NoWrap);
  m_MessageTextEdit->setReadOnly(true);
  m_MessageTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_MessageTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_MessageTextEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(CreateCustomContextMenu(const QPoint&)));
  vbox->addWidget(m_MessageTextEdit);

  // Add filter controls
  QFrame* filterFrame = new QFrame();
  filterFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHBoxLayout* filterLayout = new QHBoxLayout(filterFrame);
  filterLayout->setDirection(QBoxLayout::RightToLeft);
  filterLayout->setContentsMargins(0,0,0,0);
  filterLayout->setSpacing(0);
  m_ClearFilterButton = new QPushButton();
  m_ClearFilterButton->setIcon( QIcon( ":/icons/Resources/icon_ClearText.png" ) );
  m_ClearFilterButton->setToolTip(tr("Clear filter"));
  m_ClearFilterButton->setEnabled(false);
  filterLayout->addWidget(m_ClearFilterButton);
  connect(m_ClearFilterButton, SIGNAL(clicked()), this, SLOT(ClearFilterButtonClicked()));
  m_FilterLineEdit = new QLineEdit();
  m_FilterLineEdit->setText("");
  m_FilterLineEdit->setMinimumWidth(150);
  m_FilterLineEdit->setAlignment(Qt::AlignRight);
  connect(m_FilterLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(FilterLineEditEdited(const QString&)));
  filterLayout->addWidget(m_FilterLineEdit);
  filterLayout->addWidget(new QLabel("Message log filter: "));
  filterLayout->addStretch(1);
  vbox->addWidget(filterFrame);

  QTextCursor tc = m_MessageTextEdit->textCursor();
  tc.movePosition( QTextCursor::End );
  m_MessageTextEdit->setTextCursor( tc );

  connect(&m_FilterInputTimer, SIGNAL(timeout()), this, SLOT(ApplyFilterTimerFired()));

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool PlusStatusIcon::eventFilter(QObject *obj, QEvent *ev)
{
  if ( ev->type() == QEvent::MouseButtonPress ) 
  {
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(ev);
    if (mouseEvent != NULL) 
    {
      if (mouseEvent->buttons() == Qt::LeftButton) 
      {
        if ((m_MessageListFrame == NULL) || (! m_MessageListFrame->isVisible())) 
        {
          m_Level = vtkPlusLogger::LOG_LEVEL_INFO;
          m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );

          QTextCursor cursor(m_MessageTextEdit->textCursor());
          cursor.movePosition(QTextCursor::End);
          cursor.movePosition(QTextCursor::StartOfLine);
          m_MessageTextEdit->setTextCursor(cursor);

          m_MessageListFrame->move( mapToGlobal( QPoint( m_DotLabel->x() - m_MessageListFrame->width(), m_DotLabel->y() - m_MessageListFrame->height() - 40 ) ) );
          m_MessageListFrame->show();

        }
        else
        {
          ResetIconState();
          m_MessageListFrame->hide();
        }

        return true;
      }
    }
  }
  else if ( (obj == m_MessageListFrame) && (ev->type() == QEvent::Close) ) 
  {
    m_Level = vtkPlusLogger::LOG_LEVEL_INFO;
    m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );
  }

  // Pass the event on to the parent class
  return QWidget::eventFilter( obj, ev );
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::CreateCustomContextMenu( const QPoint& aPoint )
{
  QMenu *menu = m_MessageTextEdit->createStandardContextMenu();
  QAction* clear = new QAction("Clear", this);
  menu->addAction(clear);
  connect(clear, SIGNAL(triggered()), this, SLOT(ClearMessageList()));
  menu->exec(m_MessageTextEdit->mapToGlobal(aPoint));
  delete menu;
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::ClearMessageList()
{
  m_MessageTextEdit->clear();
  ResetIconState();
  m_MessageListFrame->hide();
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::ApplyFilterTimerFired()
{
  this->ApplyFilter();
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::ClearFilterButtonClicked()
{
  // clear log
  m_FilterLineEdit->clear();
  m_ClearFilterButton->setEnabled(false);
  this->ApplyFilter();
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::FilterLineEditEdited(const QString&)
{
  m_ClearFilterButton->setEnabled(!m_FilterLineEdit->text().isEmpty());

  if( m_FilterInputTimer.isActive() )
  {
    m_FilterInputTimer.stop();
  }

  m_FilterInputTimer.setSingleShot(true);
  m_FilterInputTimer.start(500);
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::ApplyFilter()
{
  // Clear the log
  m_MessageTextEdit->clear();

  // iterate over entire log, check for filter, if so, add message
  for( std::vector<QString>::iterator it = m_MessageLog.begin(); it != m_MessageLog.end(); ++it )
  {
    QString& entry(*it);
    if( m_FilterLineEdit->text().isEmpty() || entry.contains(m_FilterLineEdit->text(), Qt::CaseInsensitive) )
    {
      this->ParseMessage(entry);
    }
  }
}

//-----------------------------------------------------------------------------

void PlusStatusIcon::ResetIconState()
{
  m_Level = vtkPlusLogger::LOG_LEVEL_INFO;
  m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );
}

//-----------------------------------------------------------------------------
void PlusStatusIcon::SetMaxMessageCount( int count )
{
  if( count < 0 )
  {
    count = vtkPlusLogger::UnlimitedLogMessages();
  }
  this->m_MaxMessageCount = count;
}

//-----------------------------------------------------------------------------
void vtkDisplayMessageCallback::Execute( vtkObject *caller, unsigned long eventId, void *callData )
{
  if (vtkCommand::UserEvent == eventId)
  {
    char* callDataChars = reinterpret_cast<char*>(callData);

    emit AddMessage(QString::fromLatin1(callDataChars));
  }
}
