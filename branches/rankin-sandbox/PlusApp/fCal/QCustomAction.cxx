/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "QCustomAction.h"

QCustomAction::QCustomAction(const QString &text, QObject* parent, bool aIsSeparator)
: QAction(text, parent)
, m_IsSeparator(aIsSeparator)
{

}

void QCustomAction::activated()
{
  emit deviceSelected(this->text().toStdString());
}