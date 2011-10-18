/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include <QtGui/QApplication>
#include "SegmentationParameterDialogTest.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	SegmentationParameterDialogTest segmentationParameterDialogTest;
	segmentationParameterDialogTest.show();

	return app.exec();
}
