/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusServerLauncherMainWindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

  // Hide console window - we can see log messages using the status icon anyway
#ifdef _WIN32
  HWND hwnd = GetConsoleWindow();
  ShowWindow(hwnd, 0);
#endif

	PlusServerLauncherMainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
