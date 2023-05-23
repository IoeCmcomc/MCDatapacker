#ifndef RAWJSONTEXTEDITOR_H
#define RAWJSONTEXTEDITOR_H

#include "rawjsontextedit.h"

#include <QFrame>
#include <QMenu>
#include <QColor>
#include <QVersionNumber>
#include <QJsonObject>

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

    using Property   = RawJsonTextEdit::FormatProperty;
    using TextObject = RawJsonTextEdit::TextObject;

    void updateFormatButtons();
    void selectCustomColor();
    void updateEditors(int tabIndex);
    void readSourceEditor(int format);
    void writeSourceEditor(int format);

private:
    Ui::RawJsonTextEditor *ui;
    QMenu colorMenu;
    QJsonValue m_json;
    QColor currTextColor = QColor(85, 255, 85);
    bool isDarkMode      = false;
    bool isOneLine       = false;

    template<typename T>
    void insertNonEmptyProp(QJsonObject &obj, const char *key,
                            const QTextFormat &fmt, const int propId) const {
        if (const T && v = fmt.property(propId).value<T>(); !v.isEmpty()) {
            obj.insert(QLatin1String(key), v);
        }
    }
    template<>
    void insertNonEmptyProp<QJsonValue>(QJsonObject &obj, const char *key,
                                        const QTextFormat &fmt,
                                        const int propId) const {
        using T = QJsonValue;
        if (const T && v = fmt.property(propId).value<T>();
            !v.isUndefined() && !v.isNull()) {
            obj.insert(QLatin1String(key), v);
        }
    }
    template<>
    void insertNonEmptyProp<bool>(QJsonObject &obj, const char *key,
                                  const QTextFormat &fmt,
                                  const int propId) const {
        if (fmt.hasProperty(propId)) {
            obj.insert(QLatin1String(key), fmt.boolProperty(propId));
        }
    }

    void initColorMenu();
    QJsonObject JsonToComponent(const QJsonValue &root);
    void appendJsonObject(const QJsonObject &root,
                          const QTextCharFormat &optFmt = QTextCharFormat());
    void checkColorBtn();
    void setColorBtnIcon(const QColor &color);
    void mergeObjectComponent(QJsonObject &component,
                              const QTextCharFormat &fmt) const;
};

#endif /* RAWJSONTEXTEDITOR_H */
