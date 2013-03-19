/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __CaptureControlWidget_h
#define __CaptureControlWidget_h

#include "PlusConfigure.h"
#include "ui_CaptureControlWidget.h"
#include "vtkVirtualDiscCapture.h"
#include <QString>
#include <QWidget>

class QTimer;

//-----------------------------------------------------------------------------

/*! \class CaptureControlWidget 
* \brief Control vtkVirtualDiscCapture devices
* \ingroup PlusAppCaptureClient
*/
class CaptureControlWidget : public QWidget
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent Parent main window
  * \param aFlags widget flag
  */
  CaptureControlWidget(QWidget* aParent);

  /*!
  * Destructor
  */
  ~CaptureControlWidget();

  virtual void UpdateBasedOnState();

  virtual double GetMaximumFrameRate() const;

  virtual void SetCaptureDevice(vtkVirtualDiscCapture& aDevice);

  virtual vtkVirtualDiscCapture* GetCaptureDevice(){ return m_Device; }

protected:
  /*!
  * Saves recorded tracked frame list to file
  * \param aOutput Output file
  * \return Success flag
  */
  PlusStatus SaveToMetafile(std::string aOutput);

  /*!
  * Save data to file
  */
  PlusStatus WriteToFile(QString& aFilename);

  /*!
  * Display a result icon for a set duration
  */
  void ShowResultIcon(bool success, int timerInMSec = 2000);

protected slots:
  void StartStopButtonPressed();

  void Update();

  void UpdateResultIcon();

  void SaveButtonPressed();

  void SaveAsButtonPressed();

  void SamplingRateChanged(int aValue);

  void ClearButtonPressed();

protected:
  /*! Timer triggering the */
  QTimer* m_UpdateTimer;

  /*! device to interact with */
  vtkVirtualDiscCapture* m_Device;

  QTimer* m_ResultDisplayTimer;

  bool m_ResultTimerActive;

protected:
  Ui::CaptureControlWidget ui;
};

#endif
