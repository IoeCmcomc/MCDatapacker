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

    QJsonValue toJson() const;
    void fromJson(const QJsonValue &root);

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

private /*slots*/ :
    void updateFormatButtons();
    void selectCustomColor();

private:
    Ui::RawJsonTextEdit *ui;
    QMenu colorMenu;
    bool isDarkMode      = false;
    bool isOneLine       = false;
    QColor currTextColor = QColor(85, 255, 85);

    const QMap<QString, QString> colorHexes = {
        { QStringLiteral("black"),        "#000000" },
        { QStringLiteral("dark_blue"),    "#0000aa" },
        { QStringLiteral("dark_green"),   "#00aa00" },
        { QStringLiteral("dark_aqua"),    "#00aaaa" },
        { QStringLiteral("dark_red"),     "#aa0000" },
        { QStringLiteral("dark_purple"),  "#aa00aa" },
        { QStringLiteral("glod"),         "#ffAA00" },
        { QStringLiteral("gray"),         "#aaaaaa" },
        { QStringLiteral("dark_gray"),    "#555555" },
        { QStringLiteral("blue"),         "#5555ff" },
        { QStringLiteral("green"),        "#55ff55" },
        { QStringLiteral("aqua"),         "#55ffff" },
        { QStringLiteral("red"),          "#ff5555" },
        { QStringLiteral("light_purple"), "#ff55ff" },
        { QStringLiteral("yellow"),       "#ffff55" },
        { QStringLiteral("white"),        "#ffffff" }
    };

    void initColorMenu();
    QJsonObject JsonToComponent(const QJsonValue &root);
    void appendJsonObject(const QJsonObject &root,
                          const QTextCharFormat &optFmt = QTextCharFormat());
};

#endif /* RAWJSONTEXTEDIT_H */
