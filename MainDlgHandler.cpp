#include "MainDlgHandler.h"

MainDlgHandler::MainDlgHandler() {
  m_mainDlg = new Ui::MainDlg;
  m_mainDlg->setupUi(this);
  m_image = NULL;
  QObject::connect(m_mainDlg->loadImageBtn,SIGNAL(clicked()),this,SLOT(loadFileBtnClicked()));
}

MainDlgHandler::~MainDlgHandler() {
}

void MainDlgHandler::loadFileBtnClicked() {
  QString fileName = QFileDialog::getOpenFileName((QWidget*)this,"Open Imagefile",".","PNG File (*.png)");
  if(!fileName.isNull()) {
    m_filename = fileName;
    m_image = new QImage(m_filename);
    if(m_image == NULL || m_image->isNull()) {
      m_filename = "";
      m_image = NULL;
      QMessageBox::critical(this,"Invalid Image File", "Invalid Image File");
    } else {
      m_mainDlg->fileNameLable->setText(m_filename);
    }
    
  }
}
