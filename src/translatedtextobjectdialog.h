#ifndef TRANSLATEDTEXTOBJECTDIALOG_H
#define TRANSLATEDTEXTOBJECTDIALOG_H

#include "abstracttextobjectdialog.h"

namespace Ui {
    class TranslatedTextObjectDialog;
}

class TranslatedTextObjectDialog : public AbstractTextObjectDialog {
    Q_OBJECT

public:
    explicit TranslatedTextObjectDialog(QWidget *parent = nullptr);
    ~TranslatedTextObjectDialog();

    void fromTextFormat(const QTextFormat &format) final;
    QTextFormat toTextFormat() const final;

protected:
    void changeEvent(QEvent *e) final;

private:
    void onTranslateIdChanged(const QString &text);

private:
    Ui::TranslatedTextObjectDialog *ui;
};

#endif // TRANSLATEDTEXTOBJECTDIALOG_H
