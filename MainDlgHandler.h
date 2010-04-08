/*
 * File: MainDlgHandler.h
 * Author: Markus Biggel
 * Date: 2007-03-27
 * License: GPL v3
 *
*/

#ifndef MAINDLGHANDLER_H
#define MAINDLGHANDLER_H

#include <QDialog>
#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include "GeneratedFiles/ui_MainDlg.h"

#define BT_STRING  "BT"
#define BT_VERSION 0x02

// Data Types
#define BT_TYPE_PLAINTEXT 0x01
#define BT_TYPE_BINARY_FILE 0x02

class MainDlgHandler : public QDialog
{
  Q_OBJECT
  protected:
    QString m_filename;
    QImage *m_image;
    Ui::MainDlg *m_mainDlg;
    char m_imageVersion;
    char m_imageType;
    
    void writeData(QByteArray bytesToWrite);
    void readTextVersion1();
    void readTextVersion2();
    void readBinaryFileVersion2();
    
  public:
    MainDlgHandler();
    ~MainDlgHandler();
    
    
  public slots:
    void loadFileBtnClicked();
    void textChanged();
    void insertTextToImage();
    void extractTextFromImage();
    void saveImage();
    void insertFileBtnClicked();
    void extractFileFromImage();
};

#endif
