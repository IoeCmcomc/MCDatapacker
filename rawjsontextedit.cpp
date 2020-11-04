#include "rawjsontextedit.h"
#include "ui_rawjsontextedit.h"

RawJsonTextEdit::RawJsonTextEdit(QWidget *parent) :
      QFrame(parent),
      ui(new Ui::RawJsonTextEdit)
{
    ui->setupUi(this);
}

RawJsonTextEdit::~RawJsonTextEdit()
{
    delete ui;
}
