/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __SegmentationParameterDialogTest_h
#define __SegmentationParameterDialogTest_h

#include "PlusConfigure.h"

#include <QDialog>

class vtkDataCollector;
class DeviceSetSelectorWidget;

//-----------------------------------------------------------------------------

/*! \class SegmentationParameterDialogTest 
 * \brief Qt application for testing SegmentationParameterDialog and for using the dialog outside fCal
 * \ingroup PlusAppCommonWidgets
 */
class SegmentationParameterDialogTest : public QDialog
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent parent
  * \param aFlags widget flag
  */
  SegmentationParameterDialogTest(QWidget *parent = 0, Qt::WFlags flags = 0);

  /*!
  * Destructor
  */
  ~SegmentationParameterDialogTest();

  /*!
    Set verification of the saved configuration file
    \param aBaseLineFileName Path and filename of the baseline to use for comparison
  */
  void SetSavedConfigurationFileVerification(std::string aBaseLineFileName);

protected slots:
  /*!
  * Connect to devices described in the argument configuration file in response by clicking on the Connect button
  * \param aConfigFile DeviceSet configuration file path and name
  */
  void ConnectToDevicesByConfigFile(std::string aConfigFile);

  /*!
  * Shows configuration file saver dialog that enables saving the new segmentation parameters
  */
  void SaveConfigurationClicked();

protected:
  /*!
   * Read configuration file and start data collection
   * \return Success flag
   */
  PlusStatus StartDataCollection();

  /*! Verify saved configuration file by comparing it to a baseline */
  PlusStatus VerifySavedConfigurationFile();

protected:
  /*! Device set selector widget */
  DeviceSetSelectorWidget*  m_DeviceSetSelectorWidget;

  /*! Data collector object */
  vtkDataCollector*          m_DataCollector;

  /*! Save configuration button */
  QPushButton*              m_SaveButton;

  /*! Path and filename of the baseline to use for comparison when verifying saved configuration file content */
  std::string               m_VerificationBaselineFileName;
};

#endif // __SegmentationParameterDialogTest_h
