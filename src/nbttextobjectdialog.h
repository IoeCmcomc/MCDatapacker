#ifndef NBTTEXTOBJECTDIALOG_H
#define NBTTEXTOBJECTDIALOG_H

#include "abstracttextobjectdialog.h"

namespace Ui {
    class NbtTextObjectDialog;
}

class NbtTextObjectDialog : public AbstractTextObjectDialog {
    Q_OBJECT

public:
    explicit NbtTextObjectDialog(QWidget *parent = nullptr);
    ~NbtTextObjectDialog();

    void fromTextFormat(const QTextFormat &format) final;
    QTextFormat toTextFormat() const final;

protected:
    void changeEvent(QEvent *e);

    void updateOkButton();

private:
    Ui::NbtTextObjectDialog *ui;
};

#endif // NBTTEXTOBJECTDIALOG_H
