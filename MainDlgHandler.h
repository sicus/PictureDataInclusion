#ifndef MAINDLGHANDLER_H
#define MAINDLGHANDLER_H

#include <QDialog>
#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include "GeneratedFiles/ui_MainDlg.h"


class MainDlgHandler : public QDialog
{
  Q_OBJECT
  protected:
    QString m_filename;
    QImage *m_image;
    Ui::MainDlg *m_mainDlg;
    
  public:
    MainDlgHandler();
    ~MainDlgHandler();
    
    
  public slots:
    void loadFileBtnClicked();
};

#endif
