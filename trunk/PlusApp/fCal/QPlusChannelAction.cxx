/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "QPlusChannelAction.h"

//-----------------------------------------------------------------------------
QPlusChannelAction::QPlusChannelAction(const QString& text, QObject* parent, bool aIsSeparator, vtkPlusChannel* ownerChannel)
  : QAction(text, parent)
  , m_IsSeparator(aIsSeparator)
  , m_OwnerChannel(ownerChannel)
{
}

//-----------------------------------------------------------------------------
void QPlusChannelAction::activated()
{
  emit channelSelected(m_OwnerChannel);
}