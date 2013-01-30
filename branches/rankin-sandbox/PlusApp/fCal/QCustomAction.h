/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __QCustomAction_h
#define __QCustomAction_h

#include <QAction>

class QCustomAction : public QAction
{
  Q_OBJECT

public:
  QCustomAction(const QString &text, QObject* parent, bool aIsSeparator = false);
  bool IsSeparator(){ return m_IsSeparator; }

public slots:
  void activated();

signals:
  void deviceSelected(std::string&);

private:
  bool m_IsSeparator;
};

#endif // __QCustomAction_h
