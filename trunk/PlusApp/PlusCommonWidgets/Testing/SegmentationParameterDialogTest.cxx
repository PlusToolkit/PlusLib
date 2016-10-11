/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include <QTest>
#include <QObject>

class SegmentationParameterDialogTest : public QObject
{
  Q_OBJECT
private slots:
  void t1()
  {
    QVERIFY(true);
  }
};

QTEST_MAIN(SegmentationParameterDialogTest)
#include "SegmentationParameterDialogTest.moc"