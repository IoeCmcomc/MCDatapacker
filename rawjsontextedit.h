#ifndef RAWJSONTEXTEDIT_H
#define RAWJSONTEXTEDIT_H

#include <QFrame>
#include <QTextEdit>
#include <QMenu>
#include <QColor>

namespace Ui {
    class RawJsonTextEdit;
}

class RawJsonTextEdit : public QFrame
{
    Q_OBJECT

public:
    explicit RawJsonTextEdit(QWidget *parent = nullptr);
    ~RawJsonTextEdit();

    QTextEdit* getTextEdit();
    void setDarkMode(bool value);
    void setOneLine(bool value);

public slots:
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    void setStrike(bool strike);
    void setColor(QColor color);
    void colorBtnToggled(bool checked);

protected:
    void mergeCurrentFormat(const QTextCharFormat &format);
    bool eventFilter(QObject *obj, QEvent *event) override;

protected slots:
    void updateFormatButtons();

private:
    Ui::RawJsonTextEdit *ui;
    QMenu colorMenu;
    bool isDarkMode      = false;
    bool isOneLine       = false;
    QColor currTextColor = QColor("#55FF55");

    void initColorMenu();
};

#endif /* RAWJSONTEXTEDIT_H */
