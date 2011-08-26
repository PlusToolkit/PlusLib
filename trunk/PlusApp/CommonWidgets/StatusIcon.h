#ifndef STATUSICON_H
#define STATUSICON_H

#include <QWidget>

#include "PlusConfigure.h"

#include <QLabel>
#include <QTextEdit>

//-----------------------------------------------------------------------------

/*!
* \brief Widget that shows the current status of the application and displays all log messages occurred
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
  void AddMessage(const char* aMessage, const int aLevel);

protected:
  /*!
  * \brief Filters events if this object has been installed as an event filter for the watched object
  * \param obj object
  * \param ev event
  * \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
  */
  bool eventFilter(QObject *obj, QEvent *ev);

  /*!
  * \brief Creates message frame and fills up message field with the massages
  * \return Success flag
  */
  PlusStatus ConstructMessageListWidget();

protected:
  //! State level of the widget ( no errors (>2): green , warning (2): orange , error (1): red )
  int							            m_Level;

  //! Label representing the colored dot for of this widget
  QLabel*					          	m_DotLabel;

  //! Frame containing the field of messages
  QFrame*						          m_MessageListWidget;

  //! Field containing the messages
  QTextEdit*					        m_MessageTextEdit;

  //! Tag number of the display message callback
  unsigned long               m_DisplayMessageCallbackTag;
};

#endif
