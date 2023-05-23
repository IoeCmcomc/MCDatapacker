#ifndef SCOREBOARDTEXTOBJECTDIALOG_H
#define SCOREBOARDTEXTOBJECTDIALOG_H

#include "abstracttextobjectdialog.h"

namespace Ui {
    class ScoreboardTextObjectDialog;
}

class ScoreboardTextObjectDialog : public AbstractTextObjectDialog
{
    Q_OBJECT

public:
    explicit ScoreboardTextObjectDialog(QWidget *parent = nullptr);
    ~ScoreboardTextObjectDialog();

    void fromTextFormat(const QTextFormat &format) final;
    QTextFormat toTextFormat() const final;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ScoreboardTextObjectDialog *ui;

    void updateOkButton();
};

#endif // SCOREBOARDTEXTOBJECTDIALOG_H
