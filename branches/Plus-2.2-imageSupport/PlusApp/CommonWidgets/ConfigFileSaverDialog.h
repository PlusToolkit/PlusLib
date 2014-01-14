/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef CONFIGFILESAVERDIALOG_H
#define CONFIGFILESAVERDIALOG_H

#include "ui_ConfigFileSaverDialog.h"

#include "PlusConfigure.h"

#include <QDialog>

class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*! \class ConfigFileSaverDialog 
 * \brief Configuration file saver dialog class
 * \ingroup PlusAppCommonWidgets
 */
class ConfigFileSaverDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	* Constructor
	* \param aParent Parent object
	*/
	ConfigFileSaverDialog(QWidget* aParent);

	/*!
	* Destructor
	*/
	~ConfigFileSaverDialog();

	/*!
	* Set destination directory for configuration file
	* \param aDirectory Input configuration directory
	*/
	void SetDestinationDirectory(std::string aDirectory);

protected:
  /*!
	* Parses configuration data and fills form (title and description)
  * \return Success flag
	*/
  PlusStatus ReadConfiguration(); 

protected slots:
	/*!
	* Pops up open directory dialog and saves the selected one into application
	*/
	void OpenDestinationDirectoryClicked();

	/*!
	* Pops up save file dialog and saves the configuration file with the changed fields into that directory
	*/
	void SaveClicked();

protected:
	/*! Destination directory path */
	QString             m_DestinationDirectory;

protected:
	Ui::ConfigFileSaverDialog ui;
};

#endif 
