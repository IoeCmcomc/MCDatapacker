#ifndef ENTITYNAMESTEXTOBJECTDIALOG_H
#define ENTITYNAMESTEXTOBJECTDIALOG_H

#include "abstracttextobjectdialog.h"

namespace Ui {
    class EntityNamesTextObjectDialog;
}

class EntityNamesTextObjectDialog : public AbstractTextObjectDialog {
    Q_OBJECT

public:
    explicit EntityNamesTextObjectDialog(QWidget *parent = nullptr);
    ~EntityNamesTextObjectDialog();

    void fromTextFormat(const QTextFormat &format) final;
    QTextFormat toTextFormat() const final;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EntityNamesTextObjectDialog *ui;

    void onSelectorEditChanged(const QString &text);
};

#endif // ENTITYNAMESTEXTOBJECTDIALOG_H
