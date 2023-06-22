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

// Adapted from: https://stackoverflow.com/a/3057522/12682038
template<typename T>
struct identity {
    using type = T;
};

class RawJsonTextEditor : public QFrame {
    Q_OBJECT

public:
    explicit RawJsonTextEditor(QWidget *parent = nullptr);
    ~RawJsonTextEditor();

    QTextEdit * getTextEdit();
    bool isEmpty() const;
    void setDarkMode(bool value);
    void setOneLine(bool value);

    QJsonValue toJson() const;
    void fromJson(const QJsonValue &root);

    QJsonArray toJsonObjects() const;
    void fromJsonObjects(const QJsonArray &arr);

public /*slots*/ :
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    void setStrike(bool strike);
    void setObfuscated(bool obfuscated);
    void setColor(QColor color);
    void colorBtnToggled(bool checked);
    void insertObjectBtnClicked(RawJsonTextEdit::TextObject objectId);

protected:
    void mergeCurrentFormat(const QTextCharFormat &format);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *) override;

private /*slots*/ :
    enum SourceFormat {
        JSON,
        HTML,
        InlineJSON,
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
        insertNonEmptyProp(obj, key, fmt, propId, identity<T>());
    }

    template<typename T>
    void insertNonEmptyProp(QJsonObject &obj, const char *key,
                            const QTextFormat &fmt, const int propId,
                            identity<T>) const {
        if (const T && v = fmt.property(propId).value<T>(); !v.isEmpty()) {
            obj.insert(QLatin1String(key), v);
        }
    }

    void insertNonEmptyProp(QJsonObject &obj, const char *key,
                            const QTextFormat &fmt,
                            const int propId, identity<QJsonValue>) const {
        using T = QJsonValue;
        if (const T && v = fmt.property(propId).value<T>();
            !v.isUndefined() && !v.isNull()) {
            obj.insert(QLatin1String(key), v);
        }
    }

    void insertNonEmptyProp(QJsonObject &obj, const char *key,
                            const QTextFormat &fmt,
                            const int propId, identity<bool>) const {
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
    void appendBlockToArray(QJsonArray &arr, const QTextBlock &block) const;
};

#endif /* RAWJSONTEXTEDITOR_H */
