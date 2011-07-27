
#ifndef OIGTL_MAIN_WIDGET_H
#define OIGTL_MAIN_WIDGET_H


#include <QtGui>

#include "ui_MainWidget.h"

#include "vtkSmartPointer.h"

#include "vtkDataCollector.h"



class MainWidget : public QWidget
{
Q_OBJECT

public:
	
  MainWidget( QWidget *parent = 0 );
  
  void SetDataCollector( vtkDataCollector* in );
  
  void Initialize();
  
	
private slots:

  void initializeClicked();
  void startClicked();
  void stopClicked();
  
  void timerFuction();
  

private:
	
  Ui::MainWidget ui;
  
  vtkDataCollector* DataCollector;
  
};

#endif
