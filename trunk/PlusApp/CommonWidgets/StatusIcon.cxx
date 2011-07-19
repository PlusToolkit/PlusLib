#include "StatusIcon.h"

#include <QEvent>
#include <QGridLayout>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

StatusIcon *StatusIcon::Instance = NULL;

//-----------------------------------------------------------------------------

StatusIcon* StatusIcon::GetInstance()
{
	if(StatusIcon::Instance == NULL) {
		LOG_WARNING("An instance must be created first using New method");
		return NULL;
	}

	// return the instance
	return StatusIcon::Instance;
}

//-----------------------------------------------------------------------------

StatusIcon* StatusIcon::New(QWidget* aParent)
{
	if (StatusIcon::Instance == NULL) {
		StatusIcon::Instance = new StatusIcon(aParent);
		return StatusIcon::Instance;
	} else {
		LOG_ERROR("An instance of StatusIcon has been already created");
		return NULL; // Null instead of the instance because it is crucial that the application uses only one instance. Using the same instance multiple times can result in unexpexted behavior
	}
}

//-----------------------------------------------------------------------------

StatusIcon::StatusIcon(QWidget* aParent, Qt::WFlags aFlags)
	: QWidget(aParent, aFlags)
	, m_MessageListWidget(NULL)
	, m_MessageTextEdit(NULL)
	, m_MessageList(NULL)
	, m_Level(PlusLogger::LOG_LEVEL_INFO)
	, m_PreviousScroll(0)
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

	// Create message list (static)
	if (m_MessageList == NULL) {
		m_MessageList = new QList<QPair<QString, int>>();
	}

	if (m_DotLabel == NULL) {
		LOG_ERROR("Status icon cannot be initialized!");
		return;
	}

	// Install event filter that is called on any event
	this->installEventFilter(this);

	// Set instance of the object to the recently created
	this->Instance = this;
}

//-----------------------------------------------------------------------------

StatusIcon::~StatusIcon()
{
	if (m_MessageList != NULL) {
		delete m_MessageList;
		m_MessageList = NULL;
	}
}

//-----------------------------------------------------------------------------

QList<QPair<QString, int>>* StatusIcon::GetMessageList()
{
	return m_MessageList;
}

//-----------------------------------------------------------------------------

void StatusIcon::SetLevel(int aLevel)
{
	LOG_TRACE("ToolStateDisplayWidget::SetLevel(" << aLevel << ")"); 

	m_Level = aLevel;
}

//-----------------------------------------------------------------------------

int StatusIcon::GetLevel()
{
	LOG_TRACE("ToolStateDisplayWidget::GetLevel"); 

	return m_Level;
}

//-----------------------------------------------------------------------------

QLabel* StatusIcon::GetDotLabel()
{
	return m_DotLabel;
}

//-----------------------------------------------------------------------------

void StatusIcon::AddMessage(const char* aMessage, const int aLevel)
{
	QPair<QString, int> message(QString::fromAscii(aMessage), aLevel);

	StatusIcon* statusIcon = StatusIcon::GetInstance();
	statusIcon->GetMessageList()->append(message);

	// Re-color dot if necessary
	switch (aLevel) {
		case PlusLogger::LOG_LEVEL_ERROR:
			if (statusIcon->GetLevel() > PlusLogger::LOG_LEVEL_ERROR) {
				statusIcon->SetLevel(PlusLogger::LOG_LEVEL_ERROR);
				statusIcon->GetDotLabel()->setPixmap( QPixmap( ":/icons/Resources/icon_DotRed.png" ) );
			}
			break;
		case PlusLogger::LOG_LEVEL_WARNING:
			if (statusIcon->GetLevel() > PlusLogger::LOG_LEVEL_WARNING) {
				statusIcon->SetLevel(PlusLogger::LOG_LEVEL_WARNING);
				statusIcon->GetDotLabel()->setPixmap( QPixmap( ":/icons/Resources/icon_DotOrange.png" ) );
			}
			break;
		default:
			break;
	}
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

	m_MessageListWidget = new QFrame(this, Qt::ToolTip);
	m_MessageListWidget->setBackgroundColor(QColor::fromRgb(255, 255, 192));
	m_MessageListWidget->setMinimumSize(480, 480);
	m_MessageListWidget->setMaximumSize(480, 480);
	m_MessageListWidget->setFrameShape(QFrame::Box);
	m_MessageListWidget->setFrameShadow(QFrame::Raised);
	m_MessageListWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	if (m_MessageListWidget == NULL) {
		LOG_ERROR("Message list widget cannot be created!");
		return PLUS_FAIL;
	}

	// Set inner part of message list widget
	QGridLayout* grid = new QGridLayout();
	grid->setSpacing(4);
	grid->setContentsMargins(4, 4, 4, 4);
	grid->setAlignment(Qt::AlignTop);

	m_MessageTextEdit = new QTextEdit(m_MessageListWidget);
	m_MessageTextEdit->setWordWrapMode(QTextOption::WordWrap);
	m_MessageTextEdit->setReadOnly(true);

	// Add messages as labels
	if (! m_MessageList->empty()) {
		QListIterator<QPair<QString, int>> messagesIterator(*m_MessageList);
		while (messagesIterator.hasNext()) {
			QPair<QString, int> messagePair(messagesIterator.next());

			QString message(messagePair.first);

			const int messageLevel = messagePair.second;
			switch (messageLevel) {
				case PlusLogger::LOG_LEVEL_ERROR:
					m_MessageTextEdit->setTextColor(QColor::fromRgb(223, 0, 0));
					break;
				case PlusLogger::LOG_LEVEL_WARNING:
					m_MessageTextEdit->setTextColor(QColor::fromRgb(255, 128, 0));
					break;
				default:
					m_MessageTextEdit->setTextColor(Qt::black);
					break;
			}
			m_MessageTextEdit->append(message);
		}
	} else {
		m_MessageTextEdit->setText("There are no messages yet...");
	}

	grid->addWidget(m_MessageTextEdit);
	m_MessageListWidget->setLayout(grid);

	// Set cursor to the end of the log
	QTextCursor cursor(m_MessageTextEdit->textCursor());
	cursor.movePosition(QTextCursor::End);
	m_MessageTextEdit->setTextCursor(cursor);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool StatusIcon::eventFilter(QObject *obj, QEvent *ev)
{
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
		} else if ( ev->type() == QEvent::Wheel ) {
			QWheelEvent* wheelEv = dynamic_cast<QWheelEvent*>(ev);
			if (wheelEv != NULL) {
				QTextCursor cursor(m_MessageTextEdit->textCursor());
				if (wheelEv->delta() < 0) {
					if (m_PreviousScroll < 0) { // Hack: move cursor to bottom of page if it is at the top (could not solve simple scrolling, had to play with cursot)
						cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 36); // TODO Compute line count from heights instead of this constant
					}
					cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 2);
					cursor.movePosition(QTextCursor::EndOfLine);
					m_PreviousScroll = 1;
				} else {
					if ((m_PreviousScroll > 0) || (cursor.atEnd())) {
						cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 36);
					}
					cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, 2);
					cursor.movePosition(QTextCursor::StartOfLine);
					m_PreviousScroll = -1;
				}
				m_MessageTextEdit->setTextCursor(cursor);
			}
		} else {
			// Pass the event on to the parent class
			return QWidget::eventFilter( obj, ev );
		}
	}

	return true;
}
