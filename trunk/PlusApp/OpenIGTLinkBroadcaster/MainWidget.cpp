
#include "MainWidget.h"


#include <iostream>
#include <sstream>
#include <string>

#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"



MainWidget
::MainWidget( QWidget *parent )
: QWidget( parent )
{
	ui.setupUi( this );
	
	this->DataCollector = NULL;
	
	
  QTimer *timer = new QTimer( this );
  connect( timer, SIGNAL( timeout() ), this, SLOT( timerFuction() ) );
  timer->start( 10 );
}



void
MainWidget
::SetDataCollector( vtkDataCollector* in )
{
  this->DataCollector = in;
}



void
MainWidget
::Initialize()
{
  if ( this->DataCollector == NULL )
    {
    std::cout << "Cannot initialize without Data Collector." << std::endl;
    return;
    }
  
  this->DataCollector->Initialize();
  this->DataCollector->Stop();
}



void
MainWidget
::initializeClicked()
{
  std::cout << "Initialize clicked." << std::endl;
  this->Initialize();
}



void
MainWidget
::startClicked()
{
  this->DataCollector->Start();
}



void
MainWidget
::stopClicked()
{
  this->DataCollector->Stop();
}



void
MainWidget
::timerFuction()
{
  vtkSmartPointer< vtkMatrix4x4 > mMain = vtkSmartPointer< vtkMatrix4x4 >::New();
  double timestamp = 0.0;
  long flags = 0;
  
  std::stringstream ss;
  ss.precision( 2 );
  
  if ( this->DataCollector->GetInitialized() )
    {
    this->DataCollector->GetTransformWithTimestamp( mMain, timestamp, flags, this->DataCollector->GetDefaultToolPortNumber() );
    if ( flags & ( TR_MISSING | TR_OUT_OF_VIEW ) )
      {
      ss << "Target out of view";
      }
    else
      {
      ss << std::fixed << mMain->GetElement( 0, 3 ) << "  " << mMain->GetElement( 1, 3 ) << "  " << mMain->GetElement( 2, 3 );
      }
    }
  else
    {
    ss << "Data sources not initialized.";
    }
  
  
  ui.positionLabel->setText( ss.str().c_str() );
}
