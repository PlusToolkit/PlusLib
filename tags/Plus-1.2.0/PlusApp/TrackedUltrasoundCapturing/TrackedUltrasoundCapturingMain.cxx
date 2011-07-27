#include "TrackedUltrasoundCapturingGUI.h"

// QT includes
#include <qapplication.h>


int main( int argc, char** argv )
{
  // QT Stuff
  QApplication app( argc, argv, true );

  TrackedUltrasoundCapturingGUI* mainwindow = TrackedUltrasoundCapturingGUI::New();

  app.setMainWidget(mainwindow);
  mainwindow->show();

  int retCode = app.exec();

  delete mainwindow; 

  return retCode; 
}

