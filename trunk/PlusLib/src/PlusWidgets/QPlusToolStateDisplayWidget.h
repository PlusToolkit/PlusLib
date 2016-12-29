/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef TOOLSTATEDISPLAYWIDGET_H
#define TOOLSTATEDISPLAYWIDGET_H

// Local includes
#include "PlusConfigure.h"
#include "PlusWidgetsExport.h"

// Qt includes
#include <QWidget>

class QLabel;
class QTextEdit;
class vtkPlusChannel;

//-----------------------------------------------------------------------------

/*! \class QPlusToolStateDisplayWidget
 * \brief Widget that shows state of all tools available to the tracker
 * \ingroup PlusAppCommonWidgets
 */
class PlusWidgetsExport QPlusToolStateDisplayWidget : public QWidget
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent parent
  * \param aFlags widget flag
  */
  QPlusToolStateDisplayWidget(QWidget* aParent = 0, Qt::WindowFlags aFlags = 0);

  /*!
  * Destructor
  */
  ~QPlusToolStateDisplayWidget();

  /*!
  * Read tools and set up widget
  * \param aDataCollector Data collector instance
  * \param aConnectionSuccessful Flag if connection was successful (sets to uninitialized if not and displays default appearance)
  * \return Success flag
  */
  PlusStatus InitializeTools(vtkPlusChannel* aChannel, bool aConnectionSuccessful);

  /*!
  * Get tool statuses and display them
  */
  PlusStatus Update();

  /*!
  * Get initialization state
  * \return Initialization state
  */
  bool IsInitialized();

protected:
  vtkPlusChannel*           m_SelectedChannel;
  std::vector<QLabel*>      m_ToolNameLabels;
  std::vector<QTextEdit*>   m_ToolStateLabels;
  bool                      m_Initialized;
};

#endif