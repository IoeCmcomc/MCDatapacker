#ifndef ABSTRACTTEXTOBJECTDIALOG_H
#define ABSTRACTTEXTOBJECTDIALOG_H

#include <QDialog>
#include <QTextFormat>

class AbstractTextObjectDialog : public QDialog {
    Q_OBJECT

public:
    using QDialog::QDialog;

    virtual void fromTextFormat(const QTextFormat &format) = 0;
    virtual QTextFormat toTextFormat() const               = 0;

protected:
    QTextFormat m_format;
};

#endif // ABSTRACTTEXTOBJECTDIALOG_H
