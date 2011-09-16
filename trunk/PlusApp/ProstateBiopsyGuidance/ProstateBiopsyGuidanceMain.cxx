#include "ProstateBiopsyGuidanceGUI.h"
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
// QT includes
#include <qapplication.h>
#include "ProstateBiopsyGuidance.h"




int main( int argc, char** argv )
{
     QApplication app(argc, argv);
     ProstateBiopsyGuidanceGUI dialog;
     return dialog.exec();

}