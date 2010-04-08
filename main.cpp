/*
 * File: main.cpp
 * Author: Markus Biggel
 * Date: 2007-03-27
 * License: GPL v3
 *
*/

#include <QtCore>
#include <QtGui>
#include <QDialog>
#include <stdio.h>
#include "GeneratedFiles/ui_MainDlg.h"
#include "MainDlgHandler.h"


int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  MainDlgHandler m;
  m.show();
  return app.exec();
}
