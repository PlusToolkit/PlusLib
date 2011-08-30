#ifndef CONFIGFILESAVERDIALOG_H
#define CONFIGFILESAVERDIALOG_H

#include "ui_ConfigFileSaverDialog.h"

#include "PlusConfigure.h"

#include <QDialog>

class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Configuration file saver dialog class
*/
class ConfigFileSaverDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent Parent object
	* \param aConfigurationData Root XML element of the configuration data
	*/
	ConfigFileSaverDialog(QWidget* aParent, vtkXMLDataElement* aConfigurationData);

	/*!
	* \brief Destructor
	*/
	~ConfigFileSaverDialog();

  /*!
	* \brief Set destination directory to search in from registry value
	*/
  void SetDestinationDirectoryFromRegistry(); 

	/*!
	* \brief Set destination directory for configuration file
	* \param aDirectory Input configuration directory
	*/
	void SetDestinationDirectory(std::string aDirectory);

protected:
  /*!
	* \brief Parses configuration data and fills form (title and description)
  * \return Success flag
	*/
  PlusStatus FillFormWithConfigurationData(); 

protected slots:
	/*!
	* \brief Pops up open directory dialog and saves the selected one into application
	*/
	void OpenDestinationDirectoryClicked();

	/*!
	* \brief Pops up save file dialog and saves the configuration file with the changed fields into that directory
	*/
	void SaveClicked();

protected:
	//! Destination directory path
	QString             m_DestinationDirectory;

  //! Root XML element of the configuration data
  vtkXMLDataElement*  m_ConfigurationData;

protected:
	Ui::ConfigFileSaverDialog ui;
};

#endif 
