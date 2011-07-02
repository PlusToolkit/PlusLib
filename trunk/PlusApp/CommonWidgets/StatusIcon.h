#ifndef STATUSICON_H
#define STATUSICON_H

#include <QWidget>

#include "PlusConfigure.h"

#include <QLabel>

//-----------------------------------------------------------------------------

/*!
* \brief TODO
*/
class StatusIcon : public QWidget
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	StatusIcon(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~StatusIcon();

	/*!
	* \brief Callback function for logger to display messages in popup window
	* \param aMessage Message string to display
	* \param aLevel Log level of the message
	*/
	static void AddMessage(const char* aMessage, const int aLevel);

signals:

protected slots:

protected:
	/*!
	* \brief Filters events if this object has been installed as an event filter for the watched object
	* \param obj object
	* \param ev event
	* \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
	*/
	bool eventFilter(QObject *obj, QEvent *ev);

	//TODO
	PlusStatus ConstructMessageListWidget();

protected:
	//! List of messages to display
	static QList<QPair<QString, int>>*	m_MessageList;

	//! TODO
	QLabel*								m_DotLabel;

	//! TODO
	QFrame*								m_MessageListWidget;

	//! TODO
	int									m_Level;
};

#endif
