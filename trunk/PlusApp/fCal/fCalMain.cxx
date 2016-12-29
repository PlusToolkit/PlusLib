/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "fCalMainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  fCalMainWindow mainWindow;
  mainWindow.showMaximized();

  mainWindow.Initialize();

  return app.exec();
}
