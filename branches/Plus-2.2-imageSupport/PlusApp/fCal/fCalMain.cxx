/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "fCalMainWindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
  Q_INIT_RESOURCE(CommonWidgets);

	fCalMainWindow mainWindow;
	mainWindow.show();

	mainWindow.Initialize();

	return app.exec();
}
