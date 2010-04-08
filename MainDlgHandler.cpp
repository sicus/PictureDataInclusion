/*
 * File: MainDlgHandler.cpp
 * Author: Markus Biggel
 * Date: 2007-03-27
 * License: GPL v3
 *
*/

#include "MainDlgHandler.h"

MainDlgHandler::MainDlgHandler() {
  m_mainDlg = new Ui::MainDlg;
  m_mainDlg->setupUi(this);
  m_image = NULL;
  QObject::connect(m_mainDlg->loadImageBtn,SIGNAL(clicked()),this,SLOT(loadFileBtnClicked()));
  QObject::connect(m_mainDlg->textEditBox,SIGNAL(textChanged()),this,SLOT(textChanged()));
  QObject::connect(m_mainDlg->insertTextBtn,SIGNAL(clicked()),this,SLOT(insertTextToImage()));
  QObject::connect(m_mainDlg->saveImageBtn,SIGNAL(clicked()),this,SLOT(saveImage()));
  QObject::connect(m_mainDlg->extractTextBtn,SIGNAL(clicked()),this,SLOT(extractTextFromImage()));
  QObject::connect(m_mainDlg->insertFileBtn,SIGNAL(clicked()),this,SLOT(insertFileBtnClicked()));
  QObject::connect(m_mainDlg->extractFileBtn,SIGNAL(clicked()),this,SLOT(extractFileFromImage()));
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
      m_mainDlg->insertTextBtn->setEnabled(false);
      m_mainDlg->extractTextBtn->setEnabled(false);
      m_mainDlg->insertFileBtn->setEnabled(false);
      m_mainDlg->extractFileBtn->setEnabled(false);
      QMessageBox::critical(this,"Invalid Image File", "Invalid Image File");
    } else {
      m_mainDlg->fileNameLable->setText(m_filename);
      m_mainDlg->imageSizeLabel->setText( QString::number(m_image->width() * m_image->height()) );
      m_mainDlg->insertTextBtn->setEnabled(true);
      m_mainDlg->insertFileBtn->setEnabled(true);
      m_mainDlg->extractTextBtn->setEnabled(false);
      m_mainDlg->extractFileBtn->setEnabled(false);
      
      QByteArray header;
      int x = 0;
      int y = 0;
      int color = 0;
      unsigned char currentByte;
      unsigned char bit=0x0;
      
      for(int i=0; i<4; i++) {
	currentByte = 0x0;
	for(int j=0; j<8; j++) {
	  QColor pixel(m_image->pixel(x,y));
	  bit = 0x0;
	  if( color == 0 ) {
	    if( pixel.red() & 0x01 )
	      bit = 0x01;
	  }else if(color == 1) {
	    if( pixel.green() & 0x01 )
	      bit = 0x01;
	  } else if(color == 2) {
	    if( pixel.blue() & 0x01 )
	      bit = 0x01;
	  }
	  
	  if(bit)
	    currentByte = currentByte | bit<<(7-j);
	  
	  
	  color++;
	  if(color > 2)
	  {
	    
	    color = 0;
	    x++;
	    if(x >= m_image->width()) {
	      x=0;
	      y++;
	    } // if width
	  } // if color
	}// for 8
	header += currentByte;
      } // for 4
      QString bthead;
      bthead += header[0];
      bthead += header[1];
      m_imageVersion = header[2];
      m_imageType = header[3];
      if(bthead == "BT") {
	if( m_imageVersion == (char)0x01 || (header[2] == (char)BT_VERSION && m_imageType == (char)BT_TYPE_PLAINTEXT) ) {
	  m_mainDlg->includedData->setText("Text");
	  m_mainDlg->extractTextBtn->setEnabled(true);
	}
	else if( m_imageVersion == (char)BT_VERSION && m_imageType == (char)BT_TYPE_BINARY_FILE) {
	  m_mainDlg->extractFileBtn->setEnabled(true);
	  m_mainDlg->includedData->setText("File");
	}
      }
    }
  }
}

void MainDlgHandler::textChanged() {
  int textSize = m_mainDlg->textEditBox->toPlainText().size();
  textSize += 4;
  int sizeNeeded = textSize / 3;
  sizeNeeded++;
  sizeNeeded *= 8;
  m_mainDlg->sizeNeededLabel->setText( QString::number(sizeNeeded) );
  if(m_image && !m_image->isNull())
    m_mainDlg->sizeFreeLabel->setText( QString::number( ( (m_image->width() * m_image->height())) - sizeNeeded ) );
}

void MainDlgHandler::insertTextToImage() {
  if(!m_image || m_image->isNull())
    return;
  
  int imageSize = m_image->width() * m_image->height();
  int textSize = m_mainDlg->textEditBox->toPlainText().size();
  textSize += 4;
  int sizeNeeded = textSize / 3;
  sizeNeeded++;
  sizeNeeded *= 8;
  
  if(sizeNeeded > imageSize) {
    QMessageBox::critical(this,"Image too small", "Image too small");
    return;
  }
  
  QByteArray bytesToWrite;
  bytesToWrite += BT_STRING;
  bytesToWrite += BT_VERSION;
  bytesToWrite += BT_TYPE_PLAINTEXT;
  bytesToWrite += (char)0x00;
  bytesToWrite += (char)0x00;
  bytesToWrite += (char)0x00;
  bytesToWrite += (char)0x00;
  bytesToWrite += m_mainDlg->textEditBox->toPlainText().toLocal8Bit();
  bytesToWrite += '\0';
  
  writeData(bytesToWrite);
}

void MainDlgHandler::writeData(QByteArray bytesToWrite) {
  int x = 0;
  int y = 0;
  int color = 0;  // 0 = r, 1 = g, 2 = b
  char currentChar;
  QColor pixel(m_image->pixel(x,y));
  
  for(int i=0; i < bytesToWrite.size()+1; i++){
    if( i < bytesToWrite.size())
      currentChar = bytesToWrite[i];
    else
      currentChar = 0x00;

    for(int j=7; j>=0; j--) {
      if( (0x01<<j) & currentChar ) {
	if(color == 0) {
	  int red = pixel.red();
	  red = red|0x01;
	  pixel.setRed(red);
	}
	else if(color == 1) {
	  int green = pixel.green();
	  green = green|0x01;
	  pixel.setGreen(green);
	}
	else if(color == 2) {
	  int blue = pixel.blue();
	  blue = blue|0x01;
	  pixel.setBlue(blue);
	}
      } else {
	if(color == 0) {
	  int red = pixel.red();
	  red = red & 0xfe;
	  pixel.setRed(red);
	}
	else if(color == 1) {
	  int green = pixel.green();
	  green = green & 0xfe;
	  pixel.setGreen(green);
	}
	else if(color == 2) {
	  int blue = pixel.blue();
	  blue = blue & 0xfe;
	  pixel.setBlue(blue);
	}
      }
      
      color++;
      if(color >= 3 ) {
	color=0;
	m_image->setPixel(x,y,pixel.rgb());
	x++;
	if(x >= m_image->width()) {
	  x=0;
	  y++;
	}
	pixel.setRgb(m_image->pixel(x,y));
      }
    }
  }
}

void MainDlgHandler::extractTextFromImage() {
  if(!m_image || m_image->isNull())
    return;
  if(m_imageVersion == 0x01) {
    readTextVersion1();
    return;
  } else if(m_imageVersion == 0x02) {
    if(m_imageType == BT_TYPE_PLAINTEXT) {
      readTextVersion2();
      return;
    }
  }
}

void MainDlgHandler::extractFileFromImage() {
  if(m_imageVersion == 0x02) {
    if(m_imageType == BT_TYPE_BINARY_FILE) {
      readBinaryFileVersion2();
      return;
    }
  }
}

void MainDlgHandler::readBinaryFileVersion2() {
  QByteArray data;
  QString outFileName;
  bool gotFileName = false;
  int imageSize = m_image->width() * m_image->height();
  int x = 0;
  int y = 0;
  int color = 0;
  unsigned char currentByte;
  unsigned char bit=0x0;
  int version = 0x0;
  int binarySize = 0;
  QString header = "";
  
  for(int i=0; i<imageSize; i++) {
    currentByte = 0x0;
    for(int j=0; j<8; j++) {
      QColor pixel(m_image->pixel(x,y));
      bit = 0x0;
      if( color == 0 ) {
	if( pixel.red() & 0x01 )
	  bit = 0x01;
      }else if(color == 1) {
	if( pixel.green() & 0x01 )
	  bit = 0x01;
      } else if(color == 2) {
	if( pixel.blue() & 0x01 )
	  bit = 0x01;
      }
      
      if(bit)
	currentByte = currentByte | bit<<(7-j);
      
      
      color++;
      if(color > 2)
      {
	
	color = 0;
	x++;
	if(x >= m_image->width()) {
	  x=0;
	  y++;
	  if(x >= m_image->width() && y >= m_image->height()) {
	    QMessageBox::critical(this,"Error extracting file", "Invalid size of File");
	    return;
	  }
	}
      }
    }
    
    if(i < 2) {
      header += currentByte;
    }
    else if(i == 2) {
      version = (int)currentByte;
    } else if(i >= 4 && i <= 7) {
      binarySize = binarySize<<8;
      binarySize += (int)currentByte;
    }
    
    if(i >= 8) {
      if(gotFileName) {
	data += currentByte;
      } else {
	outFileName += currentByte;
	if( currentByte == 0x0 ) {
	  gotFileName = true;
	  imageSize = binarySize + i +1;
	}
      }
    }
  } // for readData
  
  if(gotFileName) {
    QString writeFileName = QFileDialog::getSaveFileName(this,"Save file As","./"+outFileName,"*");
    if(!writeFileName.isNull()) {
      QFile outFile(writeFileName);
      outFile.open(QIODevice::WriteOnly);
      outFile.write(data);
      outFile.close();
    }
  }
}

void MainDlgHandler::readTextVersion2() {
  QString text;
  int imageSize = m_image->width() * m_image->height();
  int x = 0;
  int y = 0;
  int color = 0;
  unsigned char currentByte;
  unsigned char bit=0x0;
  int version = 0x0;
  QString header = "";
  
  for(int i=0; i<imageSize; i++) {
    currentByte = 0x0;
    for(int j=0; j<8; j++) {
      QColor pixel(m_image->pixel(x,y));
      bit = 0x0;
      if( color == 0 ) {
	if( pixel.red() & 0x01 )
	  bit = 0x01;
      }else if(color == 1) {
	if( pixel.green() & 0x01 )
	  bit = 0x01;
      } else if(color == 2) {
	if( pixel.blue() & 0x01 )
	  bit = 0x01;
      }
      
      if(bit)
	currentByte = currentByte | bit<<(7-j);
      
      
      color++;
      if(color > 2)
      {
	
	color = 0;
	x++;
	if(x >= m_image->width()) {
	  x=0;
	  y++;
	}
      }
    }
    
    if(i < 2) {
      header += currentByte;
    }
    else if(i == 2) {
      version = (int)currentByte;
    }
    
    if(i >= 8) {
      if(currentByte != 0x0)
	text += currentByte;
      if(currentByte == 0x0) {
	break;
      }
    }
  }
  
  m_mainDlg->textEditBox->setPlainText(text);
}

void MainDlgHandler::readTextVersion1() {
  QString text;
  int imageSize = m_image->width() * m_image->height();
  int x = 0;
  int y = 0;
  int color = 0;
  unsigned char currentByte;
  unsigned char bit=0x0;
  int version = 0x0;
  QString header = "";
  
  for(int i=0; i<imageSize; i++) {
    currentByte = 0x0;
    for(int j=0; j<8; j++) {
      QColor pixel(m_image->pixel(x,y));
      bit = 0x0;
      if( color == 0 ) {
	if( pixel.red() & 0x01 )
	  bit = 0x01;
      }else if(color == 1) {
	if( pixel.green() & 0x01 )
	  bit = 0x01;
      } else if(color == 2) {
	if( pixel.blue() & 0x01 )
	  bit = 0x01;
      }
      
      if(bit)
	currentByte = currentByte | bit<<(7-j);
      
      
      color++;
      if(color > 2)
      {
	
	color = 0;
	x++;
	if(x >= m_image->width()) {
	  x=0;
	  y++;
	}
      }
    }
    
    if(i < 2) {
      header += currentByte;
    }
    else if(i == 2) {
      version = (int)currentByte;
    }
    else if(i == 3) {
      if(header != BT_STRING) {
	QMessageBox::information(this,"Image does not contain a Message","Image does not contain a Message or wrong Version");
	return;
      }
    }
    
    if(i >= 3) {
      if(currentByte != 0x0)
	text += currentByte;
    }
    if(currentByte == 0x0) {
      break;
    }
  }
  
  m_mainDlg->textEditBox->setPlainText(text);
}

void MainDlgHandler::insertFileBtnClicked() {
  if(!m_image || m_image->isNull())
    return;
  
  QString insertFileName = QFileDialog::getOpenFileName(this,"File to insert",".","All Files (*.*)");
  if(insertFileName.isNull())
    return;
  
  QFile inputFile(insertFileName);
  if(!inputFile.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this,"Unable to open file","Unable to open File");
  }
  
  // path abschneiden
  QString insertFileNamePlain = insertFileName.right(insertFileName.size() - insertFileName.lastIndexOf("/") -1);
  
  int imageSize = m_image->width() * m_image->height();
  // 3 byte default header (BT[version] 4 byte + 1 byte data type
  int dataSize = 3 + 1;
  // 4 byte datasize + length of filename + filename NULL termination
  dataSize += 4 + insertFileNamePlain.size() + 1;
  dataSize += inputFile.size();
  
  int sizeNeeded = (dataSize * 8 / 3) +1;  
  
  
  if(sizeNeeded > imageSize) {
    QMessageBox::critical(this,"Image too small", "Image too small");
    return;
  }
  
  unsigned int filesize = (unsigned int)inputFile.size();
  
  QByteArray bytesToWrite;
  bytesToWrite += BT_STRING;
  bytesToWrite += BT_VERSION;
  bytesToWrite += BT_TYPE_BINARY_FILE;
  
  char tmp[4];
  tmp[0] = 0x00;
  tmp[1] = 0x00;
  tmp[2] = 0x00;
  tmp[3] = 0x00;
  memcpy(tmp,&filesize,4);
  bytesToWrite += tmp[3];
  bytesToWrite += tmp[2];
  bytesToWrite += tmp[1];
  bytesToWrite += tmp[0];
  
  bytesToWrite += insertFileNamePlain.toLocal8Bit();
  bytesToWrite += '\0';
  bytesToWrite += inputFile.readAll();
  
  QMessageBox::information(this,"bla",QString::number((int)inputFile.size()));
  
  inputFile.close();
  writeData(bytesToWrite);
  
  m_mainDlg->sizeNeededLabel->setText(QString::number(sizeNeeded));
  m_mainDlg->sizeFreeLabel->setText(QString::number(imageSize - sizeNeeded));
  
}

void MainDlgHandler::saveImage() {
  if(!m_image || m_image->isNull()) {
    QMessageBox::critical(this,"no image loaded","no image loaded");
    return;
  }
  QString saveFileName = QFileDialog::getSaveFileName((QWidget*)this,"Save Imagefile",".","PNG File (*.png)");
  if(saveFileName.isNull() || saveFileName.isEmpty())
    return;
  
  m_image->save(saveFileName,"png",50);
}
