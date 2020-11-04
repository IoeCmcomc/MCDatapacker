#ifndef RAWJSONTEXTEDIT_H
#define RAWJSONTEXTEDIT_H

#include <QFrame>

namespace Ui {
class RawJsonTextEdit;
}

class RawJsonTextEdit : public QFrame
{
    Q_OBJECT

public:
    explicit RawJsonTextEdit(QWidget *parent = nullptr);
    ~RawJsonTextEdit();

private:
    Ui::RawJsonTextEdit *ui;
};

#endif // RAWJSONTEXTEDIT_H
