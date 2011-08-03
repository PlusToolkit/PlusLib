#include "FreehandMainWindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	FreehandMainWindow w;
	w.show();

	w.Initialize();

	return a.exec();
}
