/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "StatusIcon.h"
#include <QEvent>
#include <QGridLayout>
#include <QMenu>
#include <QSizePolicy>
#include <QScrollBar>
#include <QCoreApplication>
//-----------------------------------------------------------------------------
namespace
{
  const QString errorHtml = "<font color=\"#DF0000\">";
  const QString warningHtml = "<font color=\"#FF8000\">";
  const QString infoHtml = "<font color=\"Black\">";
  const QString endHtml = "</font>";
}
//-----------------------------------------------------------------------------

StatusIcon::StatusIcon(QWidget* aParent, Qt::WFlags aFlags)
  : QWidget(aParent, aFlags)
  , m_Level(vtkPlusLogger::LOG_LEVEL_INFO)
  , m_DotLabel(NULL)
  , m_MessageListWidget(NULL)
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

StatusIcon::~StatusIcon()
{
  vtkPlusLogger::Instance()->RemoveObserver(m_DisplayMessageCallbackTag);
}

//-----------------------------------------------------------------------------

void StatusIcon::AddMessage(QString aInputString)
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
      message = errorHtml;
      break;
    case vtkPlusLogger::LOG_LEVEL_WARNING:
      if (m_Level > vtkPlusLogger::LOG_LEVEL_WARNING) {
        m_Level = vtkPlusLogger::LOG_LEVEL_WARNING;
        m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotOrange.png" ) );
      }
      message = warningHtml;
      break;
    default:
      message = infoHtml;
      break;
  }

  message = message.append(aInputString.right( aInputString.size() - pos - 1 )).append(endHtml);

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

PlusStatus StatusIcon::ConstructMessageListWidget()
{
  LOG_TRACE("ToolStateDisplayWidget::ConstructMessageListWidget"); 

  // (Re-)Create message list widget
  if (m_MessageListWidget != NULL) 
  {
    delete m_MessageListWidget;
    m_MessageListWidget = NULL;
  }

  m_MessageListWidget = new QFrame(this, Qt::Tool);
  m_MessageListWidget->setMinimumSize(480, 480);
  m_MessageListWidget->setMaximumSize(960, 760);
  m_MessageListWidget->resize(480, 480);

  if (m_MessageListWidget == NULL) {
    LOG_ERROR("Message list widget cannot be created!");
    return PLUS_FAIL;
  }

  // Install event filter to properly handle close
  m_MessageListWidget->installEventFilter(this);

  // Set inner part of message list widget
  QGridLayout* grid = new QGridLayout();
  grid->setSpacing(0);
  grid->setContentsMargins(0, 0, 0, 0);
  grid->setAlignment(Qt::AlignTop);

  m_MessageTextEdit = new QTextEdit(m_MessageListWidget);
  m_MessageTextEdit->setWordWrapMode(QTextOption::NoWrap);
  m_MessageTextEdit->setReadOnly(true);
  m_MessageTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_MessageTextEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(CreateCustomContextMenu(const QPoint&)));

  grid->addWidget(m_MessageTextEdit);
  m_MessageListWidget->setLayout(grid);

  QTextCursor tc = m_MessageTextEdit->textCursor();
  tc.movePosition( QTextCursor::End );
  m_MessageTextEdit->setTextCursor( tc );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool StatusIcon::eventFilter(QObject *obj, QEvent *ev)
{
  if ( ev->type() == QEvent::MouseButtonPress ) 
  {
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(ev);
    if (mouseEvent != NULL) 
    {
      if (mouseEvent->buttons() == Qt::LeftButton) 
      {
        if ((m_MessageListWidget == NULL) || (! m_MessageListWidget->isVisible())) 
        {
          m_Level = vtkPlusLogger::LOG_LEVEL_INFO;
          m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );

          QTextCursor cursor(m_MessageTextEdit->textCursor());
          cursor.movePosition(QTextCursor::End);
          cursor.movePosition(QTextCursor::StartOfLine);
          m_MessageTextEdit->setTextCursor(cursor);

          m_MessageListWidget->move( mapToGlobal( QPoint( m_DotLabel->x() - m_MessageListWidget->width(), m_DotLabel->y() - m_MessageListWidget->height() - 40 ) ) );
          m_MessageListWidget->show();

        }
        else
        {
          ResetIconState();
          m_MessageListWidget->hide();
        }

        return true;
      }
    }
  }
  else if ( (obj == m_MessageListWidget) && (ev->type() == QEvent::Close) ) 
  {
    m_Level = vtkPlusLogger::LOG_LEVEL_INFO;
    m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );
  }

  // Pass the event on to the parent class
  return QWidget::eventFilter( obj, ev );
}

//-----------------------------------------------------------------------------

void StatusIcon::CreateCustomContextMenu( const QPoint& aPoint )
{
  QMenu *menu = m_MessageTextEdit->createStandardContextMenu();
  QAction* clear = new QAction("Clear", this);
  menu->addAction(clear);
  connect(clear, SIGNAL(triggered()), this, SLOT(ClearMessageList()));
  menu->exec(m_MessageTextEdit->mapToGlobal(aPoint));
  delete menu;
}

//-----------------------------------------------------------------------------

void StatusIcon::ClearMessageList()
{
  m_MessageTextEdit->clear();
  ResetIconState();
  m_MessageListWidget->hide();
}

//-----------------------------------------------------------------------------

void StatusIcon::ResetIconState()
{
  m_Level = vtkPlusLogger::LOG_LEVEL_INFO;
  m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );
}

//-----------------------------------------------------------------------------
void StatusIcon::SetMaxMessageCount( int count )
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

    emit AddMessage(QString::fromAscii(callDataChars));
  }
}
