#include "fCalMainWindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	fCalMainWindow mainWindow;
	mainWindow.show();

	mainWindow.Initialize();

	return app.exec();
}
