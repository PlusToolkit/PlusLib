#include "StatusIcon.h"

#include <QEvent>
#include <QGridLayout>
#include <QSizePolicy>
#include <QFrame>
#include <QPalette>

QList<QPair<QString, int>>* StatusIcon::m_MessageList = NULL;

//-----------------------------------------------------------------------------

StatusIcon::StatusIcon(QWidget* aParent, Qt::WFlags aFlags)
	: QWidget(aParent, aFlags)
	, m_MessageListWidget(NULL)
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

	// Create message list (static)
	if (StatusIcon::m_MessageList == NULL) {
		StatusIcon::m_MessageList = new QList<QPair<QString, int>>();
	}

	if (m_DotLabel == NULL) {
		LOG_ERROR("Status icon cannot be initialized!");
		return;
	}

	// Install event filter that is called on any event
	this->installEventFilter(this);
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

void StatusIcon::AddMessage(const char* aMessage, const int aLevel)
{
	QPair<QString, int> message(QString::fromAscii(aMessage), aLevel);

	StatusIcon::m_MessageList->append(message);
}

//-----------------------------------------------------------------------------

PlusStatus StatusIcon::ConstructMessageListWidget()
{
	// (Re-)Create message list widget
	if (m_MessageListWidget != NULL) {
		delete m_MessageListWidget;
		m_MessageListWidget = NULL;
	}

	m_MessageListWidget = new QFrame(this, Qt::ToolTip);
	m_MessageListWidget->setBackgroundColor(QColor::fromRgb(255, 255, 223));
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

	// Add messages as labels
	if (! m_MessageList->empty()) {
		QListIterator<QPair<QString, int>> messagesIterator(*m_MessageList);
		while (messagesIterator.hasNext()) {
			QPair<QString, int> messagePair(messagesIterator.next());

			QString message(messagePair.first);
			QLabel* messageLabel = new QLabel(message, m_MessageListWidget);

			const int messageLevel = messagePair.second;
			QColor messageColor = QColor();
			switch (messageLevel) {
				case PlusLogger::LOG_LEVEL_ERROR:
					messageColor.setRgb(192, 0, 0);
					if (m_Level > PlusLogger::LOG_LEVEL_ERROR) {
						m_Level = PlusLogger::LOG_LEVEL_ERROR;
						m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotRed.png" ) );
					}
					break;
				case PlusLogger::LOG_LEVEL_WARNING:
					if (m_Level > PlusLogger::LOG_LEVEL_WARNING) {
						m_Level = PlusLogger::LOG_LEVEL_WARNING;
						m_DotLabel->setPixmap( QPixmap( ":/icons/Resources/icon_DotOrange.png" ) );
					}
					messageColor.setRgb(255, 128, 0);
					break;
				default:
					messageColor.setRgb(0, 0, 0);
					break;
			}
			QPalette messagePalette = QPalette();
			messagePalette.setColor(QPalette::Text, messageColor);
			messageLabel->setPalette(messagePalette);

			grid->addWidget(messageLabel);
		}
	} else {
		QLabel* messageLabel = new QLabel("There are no messages yet...", m_MessageListWidget);
		grid->addWidget(messageLabel);
	}

	m_MessageListWidget->setLayout(grid);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool StatusIcon::eventFilter(QObject *obj, QEvent *ev)
{
	if ( obj == this ) {
		if ( ev->type() == QEvent::Enter ) {
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

	return true;
}
