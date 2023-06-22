
#ifndef RAWJSONTEXTOBJECTINTERFACE_H
#define RAWJSONTEXTOBJECTINTERFACE_H

#include <QTextObjectInterface>

enum RawJsonProperty {
    TextObfuscated = QTextCharFormat::UserProperty + 0x100, // bool
    TranslateKey,                                           // QString
    TranslateArgs,                                          //QJsonArray
    TranslateFallback,                                      // QString
    ScoreboardName,                                         // QString
    ScoreboardObjective,                                    // QString
    ScoreboardValue,                                        //QString
    Selector,                                               // QString
    Separator,                                              // QJsonValue
    Keybind,                                                // QString
    NbtPath,                                                // QString
    NbtInterpret,                                           // bool
    NbtBlock,                                               // QString
    NbtEntity,                                              // QString
    NbtStorage,                                             // QString
};

class RawJsonTextEdit;
class AbstractTextObjectDialog;

class RawJsonTextObjectInterface : public QObject, public QTextObjectInterface {
    Q_OBJECT

public:
    RawJsonTextObjectInterface(RawJsonTextEdit *textEdit,
                               const QString &iconStr);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                         const QTextFormat &format) override;
    void drawObject(QPainter *painter, const QRectF &rect,
                    QTextDocument *doc, int posInDocument,
                    const QTextFormat &format) override;
    virtual QString displayText(const QTextFormat &format) const = 0;
    virtual bool editObject(QTextFormat& format, const QRectF& rect);

protected:
    QString m_iconStr;
    RawJsonTextEdit *m_textEdit = nullptr;

    bool execEditorDialog(QTextFormat &format,
                          AbstractTextObjectDialog *dialog);
};

class TranslatedTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit TranslatedTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const final;
    bool editObject(QTextFormat& format, const QRectF& rect) final;
};

class ScoreboardTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit ScoreboardTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const final;
    bool editObject(QTextFormat& format, const QRectF& rect) final;
};

class EntityNamesTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit EntityNamesTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const final;
    bool editObject(QTextFormat& format, const QRectF& rect) final;
};

class KeybindTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit KeybindTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const final;
    bool editObject(QTextFormat& format, const QRectF& rect) final;
};

class NbtTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit NbtTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const final;
    bool editObject(QTextFormat& format, const QRectF& rect) final;
};

#endif // RAWJSONTEXTOBJECTINTERFACE_H
