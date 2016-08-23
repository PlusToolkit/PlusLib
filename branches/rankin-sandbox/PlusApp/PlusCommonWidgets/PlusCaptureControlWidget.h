/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __CaptureControlWidget_h
#define __CaptureControlWidget_h

#include "PlusConfigure.h"
#include "ui_PlusCaptureControlWidget.h"
#include "vtkPlusVirtualDiscCapture.h"
#include <QString>
#include <QWidget>

class QTimer;

//-----------------------------------------------------------------------------

/*! \class PlusCaptureControlWidget 
* \brief Control vtkPlusVirtualDiscCapture devices
* \ingroup PlusAppCaptureClient
*/
class PlusCaptureControlWidget : public QWidget
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent Parent main window
  * \param aFlags widget flag
  */
  PlusCaptureControlWidget(QWidget* aParent);

  /*!
  * Destructor
  */
  ~PlusCaptureControlWidget();

  virtual void UpdateBasedOnState();

  virtual double GetMaximumFrameRate() const;

  virtual void SetCaptureDevice(vtkPlusVirtualDiscCapture& aDevice);

  virtual vtkPlusVirtualDiscCapture* GetCaptureDevice(){ return m_Device; }

  virtual void SetEnableCapturing(bool aCapturing);

  virtual void SaveFile();

  virtual void Clear();

  virtual bool CanSave() const;

  virtual bool CanRecord() const;

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
  PlusStatus WriteToFile(const QString& aFilename);

  /*!
  * Display a result icon for a set duration
  */
  void SendStatusMessage(const std::string& aMessage);

signals:
  void EmitStatusMessage(const std::string&);

protected slots:
  /*!
  * Take snapshot (record the current frame only)
  */
  void TakeSnapshot();

  void StartStopButtonPressed();

  void SaveButtonPressed();

  void SaveAsButtonPressed();

  void SamplingRateChanged(int aValue);

  void ClearButtonPressed();

protected:
  /*! device to interact with */
  vtkPlusVirtualDiscCapture* m_Device;

protected:
  Ui::CaptureControlWidget ui;
};

#endif
