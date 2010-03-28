#ifndef MAINDLGHANDLER_H
#define MAINDLGHANDLER_H

#include <QDialog>
#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include "GeneratedFiles/ui_MainDlg.h"

#define BT_STRING  "BT"
#define BT_VERSION 0x01

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
    void textChanged();
    void insertTextToImage();
    void extractTextFromImage();
    void saveImage();
};

#endif
