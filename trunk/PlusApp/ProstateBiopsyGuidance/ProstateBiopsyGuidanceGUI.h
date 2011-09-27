#ifndef ProstateBiopsyGuidanceGUI_H
#define ProstateBiopsyGuidanceGUI_H

#include "PlusConfigure.h"
#include "vtkVideoBuffer.h"
// QT parental class includes
#include "ui_ProstateBiopsyGuidance.h"
#include <QWidget>

class ProstateBiopsyGuidance; 
class QProgressDialog; 
class QCloseEvent;
class QTimer;
class DeviceSetSelectorWidget;
class ToolStateDisplayWidget; 
class StatusIcon; 

 #include <QDialog>

 class QCheckBox;
 class QDialogButtonBox;
 class QGroupBox;
 class QLabel;
 class QLineEdit;
 class QPushButton;

 class ProstateBiopsyGuidanceGUI : public QDialog
 {
     Q_OBJECT

 public:
     ProstateBiopsyGuidanceGUI(QWidget *parent = 0);
	 	 
 private:
     QLabel *label;
     QLineEdit *lineEdit;
     QCheckBox *caseCheckBox;
     QCheckBox *fromStartCheckBox;
     QCheckBox *wholeWordsCheckBox;
     QCheckBox *searchSelectionCheckBox;
     QCheckBox *backwardCheckBox;
     QDialogButtonBox *buttonBox;
     QPushButton *findButton;
     QPushButton *moreButton;
     QWidget *extension;
public slots:

		PlusStatus  saveData(vtkVideoBuffer *buffer_RF,std::string outputVideoBufferSequenceFileName);
		PlusStatus  acquireData(vtkVideoBuffer *buffer_RF, int type,std::string savedBufferName);
		PlusStatus  startShaker();
		PlusStatus	initialize();
//		PlusStatus	AcquireBModeData();
//		PlusStatus	SaveBModeData();
//		PlusStatus  StopDataAquisition();
		PlusStatus  startBiopsyProcess();
		PlusStatus	stopDataAquisition();
		PlusStatus	stopShaker();
		PlusStatus	deleteBuffer(vtkVideoBuffer *buffer_RF);

 };

 #endif