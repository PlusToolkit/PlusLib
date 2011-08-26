#include "StatusIcon.h"

#include "vtkCallbackCommand.h"

#include <QEvent>
#include <QGridLayout>
#include <QSizePolicy>

//----------------------------------------------------------------------
class vtkDisplayMessageCallback : public vtkCallbackCommand
{
public:
	static vtkDisplayMessageCallback *New()
	{
		vtkDisplayMessageCallback *cb = new vtkDisplayMessageCallback();
		return cb;
	}

	virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
	{
    if (vtkCommand::UserEvent == eventId) {
      char* callDataChars = reinterpret_cast<char*>(callData);
      std::string callDataString(callDataChars);

      unsigned int pos = callDataString.find('|');
      const int logLevel = atoi(callDataString.substr(0, pos).c_str());
      callDataString = callDataString.substr(pos+1);

      m_StatusIcon->AddMessage(callDataString.c_str(), logLevel);
    }
	}

  void SetStatusIcon(StatusIcon* statusIcon) { m_StatusIcon = statusIcon; }

private:
  StatusIcon* m_StatusIcon;
};
 
//-----------------------------------------------------------------------------

StatusIcon::StatusIcon(QWidget* aParent, Qt::WFlags aFlags)
  : QWidget(aParent, aFlags)
  , m_MessageListWidget(NULL)
  , m_MessageTextEdit(NULL)
  , m_Level(PlusLogger::LOG_LEVEL_INFO)
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
  cb->SetStatusIcon(this);
  m_DisplayMessageCallbackTag = PlusLogger::Instance()->AddObserver(vtkCommand::UserEvent, cb);

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
  PlusLogger::Instance()->RemoveObserver(m_DisplayMessageCallbackTag);
}

//-----------------------------------------------------------------------------

void StatusIcon::AddMessage(const char* aMessage, const int aLevel)
{
  // Re-color dot if necessary
  switch (aLevel) {
    case PlusLogger::LOG_LEVEL_ERROR:
      if (m_Level > PlusLogger::LOG_LEVEL_ERROR) {
        m_Level = PlusLogger::LOG_LEVEL_ERROR;
        m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotRed.png" ) );
      }

      m_MessageTextEdit->setTextColor(QColor::fromRgb(223, 0, 0));

      break;

    case PlusLogger::LOG_LEVEL_WARNING:
      if (m_Level > PlusLogger::LOG_LEVEL_WARNING) {
        m_Level = PlusLogger::LOG_LEVEL_WARNING;
        m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotOrange.png" ) );
      }

      m_MessageTextEdit->setTextColor(QColor::fromRgb(255, 128, 0));

      break;

    default:
      m_MessageTextEdit->setTextColor(Qt::black);
      break;
  }

  m_MessageTextEdit->append(aMessage);
}

//-----------------------------------------------------------------------------

PlusStatus StatusIcon::ConstructMessageListWidget()
{
  LOG_TRACE("ToolStateDisplayWidget::ConstructMessageListWidget"); 

  // (Re-)Create message list widget
  if (m_MessageListWidget != NULL) {
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

  grid->addWidget(m_MessageTextEdit);
  m_MessageListWidget->setLayout(grid);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool StatusIcon::eventFilter(QObject *obj, QEvent *ev)
{
  if ( ev->type() == QEvent::MouseButtonPress ) {
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(ev);
    if (mouseEvent != NULL) {
      if (mouseEvent->buttons() == Qt::LeftButton) {

        if ((m_MessageListWidget == NULL) || (! m_MessageListWidget->isVisible())) {
          m_MessageListWidget->move( mapToGlobal( QPoint( m_DotLabel->x() - m_MessageListWidget->width() + 30, m_DotLabel->y() - m_MessageListWidget->height() - 40 ) ) );
          m_MessageListWidget->show();

        } else {
          m_Level = PlusLogger::LOG_LEVEL_INFO;
          m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );

          m_MessageListWidget->hide();
        }

        return true;
      }
    }
  } else if ( (obj == m_MessageListWidget) && (ev->type() == QEvent::Close) ) {
    m_Level = PlusLogger::LOG_LEVEL_INFO;
    m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotGreen.png" ) );
  }

  // Pass the event on to the parent class
  return QWidget::eventFilter( obj, ev );
}
