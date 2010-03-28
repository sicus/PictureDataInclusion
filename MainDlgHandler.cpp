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
      QMessageBox::critical(this,"Invalid Image File", "Invalid Image File");
    } else {
      m_mainDlg->fileNameLable->setText(m_filename);
      m_mainDlg->imageSizeLabel->setText( QString::number(m_image->width() * m_image->height()) );
      m_mainDlg->insertTextBtn->setEnabled(true);
      m_mainDlg->extractTextBtn->setEnabled(true);
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
  
  int x = 0;
  int y = 0;
  int color = 0;  // 0 = r, 1 = g, 2 = b
  char currentChar;
  QByteArray bytesToWrite;
  bytesToWrite += BT_STRING;
  bytesToWrite += BT_VERSION;
  bytesToWrite += m_mainDlg->textEditBox->toPlainText().toLocal8Bit();
  bytesToWrite += '\0';
  QColor pixel(m_image->pixel(x,y));
  
  for(int i=0; i < bytesToWrite.size(); i++){
    currentChar = bytesToWrite[i];
    //printf("\nwrite byte: %02X %c\n",currentChar,currentChar);
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
	QColor pixel(m_image->pixel(x,y));
      }
    }
  }
}

void MainDlgHandler::extractTextFromImage() {
  if(!m_image || m_image->isNull())
    return;
  
  QString text;
  int x = 0;
  int y = 0;
  int imageSize = m_image->width() * m_image->height();
  int color = 0;
  unsigned char currentByte;
  int version = 0x0;
  unsigned char bit=0x0;
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
    
    //printf("%02X\n",currentByte);
    //fflush(stdout);
    
    if(i < 2) {
      header += currentByte;
    }
    else if(i == 2) {
      version = (int)currentByte;
    }
    else if(i == 3) {
      if(header != BT_STRING || version != (int)BT_VERSION) {
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
