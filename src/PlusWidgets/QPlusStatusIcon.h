/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef STATUSICON_H
#define STATUSICON_H

// Local includes
#include "PlusConfigure.h"
#include "PlusWidgetsExport.h"

// VTK includes
#include <vtkCallbackCommand.h>

// Qt includes
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QTimer>

//-----------------------------------------------------------------------------

/*! \class vtkDisplayMessageCallback
 * \brief Callback command class that catches the log message event and adds it to the text field of the status icon
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayMessageCallback : public QObject, public vtkCallbackCommand
{
  Q_OBJECT

public:
  static vtkDisplayMessageCallback* New()
  {
    vtkDisplayMessageCallback* cb = new vtkDisplayMessageCallback();
    return cb;
  }

  vtkDisplayMessageCallback()
    : QObject()
  { }

  virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);

signals:
  void AddMessage(QString);
};

//-----------------------------------------------------------------------------

/*! \class QPlusStatusIcon
 *
 * Widget that shows the current status of the application and displays all log messages occurred
 *
 * \ingroup PlusAppCommonWidgets
 *
 */
class PlusWidgetsExport QPlusStatusIcon : public QWidget
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent parent
  * \param aFlags widget flag
  */
  QPlusStatusIcon(QWidget* aParent = 0, Qt::WindowFlags aFlags = 0);

  /*!
  * Destructor
  */
  ~QPlusStatusIcon();

  void SetMaxMessageCount(int count);

protected:
  /*!
  * Filters events if this object has been installed as an event filter for the watched object
  * \param obj object
  * \param ev event
  * \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
  */
  bool eventFilter(QObject* obj, QEvent* ev);

  void resizeEvent(QResizeEvent* event);

  /*!
  * Creates message frame and fills up message field with the massages
  * \return Success flag
  */
  PlusStatus ConstructMessageListWidget();

  /*!
  * Reset the icon to a nice green icon
  */
  void ResetIconState();

  /*!
  * Parse a message and add it to the text edit
  */
  void ParseMessage(QString& aInputString);

  /*!
  * Filter the text in the message box with the text in the line edit
  */
  void ApplyFilter();

protected slots:
  /*!
  * Create an extended context menu with the clear option
  */
  void CreateCustomContextMenu(const QPoint& aPoint);

  /*!
  * Clear the message box
  */
  void ClearMessageList();

  /*!
  * Apply filter button was clicked
  */
  void ClearFilterButtonClicked();

  /*!
  * Detect changes in the line edit text
  */
  void FilterLineEditEdited(const QString&);

  /*!
  * Apply the filter if the timer expires
  */
  void ApplyFilterTimerFired();

public slots:
  /*!
  * Callback function for logger to display messages in popup window
  * \return aInputString Log level and message in a string
  */
  void AddMessage(QString aInputString);

protected:
  /*! State level of the widget ( no errors (>2): green , warning (2): orange , error (1): red ) */
  int           m_Level;

  /*! Label representing the colored dot for of this widget */
  QLabel*       m_DotLabel;

  /*! Frame containing the field of messages */
  QFrame*       m_MessageListFrame;

  /*! Field containing the messages */
  QTextEdit*    m_MessageTextEdit;

  /*! Field for filter contents */
  QLineEdit*    m_FilterLineEdit;
  /*! Button to apply the filter to the message log */
  QPushButton*  m_ClearFilterButton;
  /*! Timer to prevent rapid re-filtering */
  QTimer        m_FilterInputTimer;

  /*! Tag number of the display message callback */
  unsigned long m_DisplayMessageCallbackTag;
  unsigned long m_DisplayWideMessageCallbackTag;

  /*! Contents of the entire log */
  std::vector<QString> m_MessageLog;

  /* Allow a maximum number of messages to be logged */
  int m_MaxMessageCount;
};

#endif