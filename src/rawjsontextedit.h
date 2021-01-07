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

protected slots:
    void updateFormatButtons();

private:
    Ui::RawJsonTextEdit *ui;
    QMenu colorMenu;
    bool isDarkMode      = false;
    bool isOneLine       = false;
    QColor currTextColor = QColor("#55FF55");

    const QMap<QString, QString> colorHexes = {
        { "black",
          "#000000" },
        { "dark_blue",
          "#0000aa" },
        { "dark_green",
          "#00aa00" },
        { "dark_aqua",
          "#00aaaa" },
        { "dark_red",
          "#aa0000" },
        { "dark_purple",
          "#aa00aa" },
        { "glod",
          "#ffAA00" },
        { "gray",
          "#aaaaaa" },
        { "dark_gray",
          "#555555" },
        { "blue",
          "#5555ff" },
        { "green",
          "#55ff55" },
        { "aqua",
          "#55ffff" },
        { "red",
          "#ff5555" },
        { "light_purple",
          "#ff55ff" },
        { "yellow",
          "#ffff55" },
        { "white",
          "#ffffff" } };

    void initColorMenu();
    QJsonObject JsonToComponent(const QJsonValue &root);
    void appendJsonObject(const QJsonObject &root,
                          const QTextCharFormat &optFmt = QTextCharFormat());
};

#endif /* RAWJSONTEXTEDIT_H */
