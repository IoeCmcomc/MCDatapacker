#ifndef RAWJSONTEXTEDITOR_H
#define RAWJSONTEXTEDITOR_H

#include "rawjsontextedit.h"

#include <QFrame>
#include <QTextEdit>
#include <QMenu>
#include <QColor>
#include <QJsonValue>

namespace Ui {
    class RawJsonTextEditor;
}

class RawJsonTextEditor : public QFrame {
    Q_OBJECT

public:
    explicit RawJsonTextEditor(QWidget *parent = nullptr);
    ~RawJsonTextEditor();

    QTextEdit * getTextEdit();
    void setDarkMode(bool value);
    void setOneLine(bool value);

    QJsonValue toJson() const;
    void fromJson(const QJsonValue &root);

public /*slots*/ :
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    void setStrike(bool strike);
    void setObfuscated(bool obfuscated);
    void setColor(QColor color);
    void colorBtnToggled(bool checked);

protected:
    void mergeCurrentFormat(const QTextCharFormat &format);
    bool eventFilter(QObject *obj, QEvent *event) override;

private /*slots*/ :
    enum SourceFormat {
        JSON,
        HTML,
        Markdown,
    };

    using Property = RawJsonTextEdit::FormatProperty;

    void updateFormatButtons();
    void selectCustomColor();
    void updateEditors(int tabIndex);
    void readSourceEditor(int format);
    void writeSourceEditor(int format);

private:
    Ui::RawJsonTextEditor *ui;
    QJsonValue m_json;
    QMenu colorMenu;
    bool isDarkMode      = false;
    bool isOneLine       = false;
    QColor currTextColor = QColor(85, 255, 85);

    void initColorMenu();
    QJsonObject JsonToComponent(const QJsonValue &root);
    void appendJsonObject(const QJsonObject &root,
                          const QTextCharFormat &optFmt = QTextCharFormat());
    void checkColorBtn();
    void setColorBtnIcon(const QColor &color);
};

#endif /* RAWJSONTEXTEDITOR_H */
