#include <QtGui/QApplication>
#include "prostatebiopsyguidancegui.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProstateBiopsyGuidanceGUI w;
	w.show();

	return a.exec();
}