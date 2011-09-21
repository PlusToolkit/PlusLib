#ifndef ProstateBiopsyGuidanceGUI_H
#define ProstateBiopsyGuidanceGUI_H

#include "PlusConfigure.h"
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
		PlusStatus SaveRFData();
		PlusStatus startShaker();

 };

 #endif