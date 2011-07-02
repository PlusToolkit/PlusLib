#ifndef STATUSICON_H
#define STATUSICON_H

#include <QWidget>

#include "PlusConfigure.h"

#include <QLabel>
#include <QTextEdit>

//-----------------------------------------------------------------------------

/*!
* \brief TODO
*/
class StatusIcon : public QWidget
{
	Q_OBJECT

public:
	/*!
	* \brief Instance getter
	* \return Instance object
	*/
	static StatusIcon* GetInstance();

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

	//TODO
	QList<QPair<QString, int>>* GetMessageList();

	//TODO
	void SetLevel(int aLevel);

	//TODO
	int GetLevel();

	//TODO
	QLabel* GetDotLabel();

protected:
	//! List of messages to display
	QList<QPair<QString, int>>*	m_MessageList;

	//! TODO
	QLabel*						m_DotLabel;

	//! TODO
	QFrame*						m_MessageListWidget;

	//! TODO
	QTextEdit*					m_MessageTextEdit;

	//! TODO
	int							m_Level;

	// Hack: move cursor to bottom of page if it is at the top (could not solve simple scrolling, had to play with cursot)
	int							m_PreviousScroll;

private:
	//! Instance of the singleton
	static StatusIcon*			Instance;
};

#endif
