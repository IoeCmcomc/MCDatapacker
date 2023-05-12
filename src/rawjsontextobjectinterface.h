
#ifndef RAWJSONTEXTOBJECTINTERFACE_H
#define RAWJSONTEXTOBJECTINTERFACE_H

#include <QTextObjectInterface>

enum RawJsonProperty {
    TextObfuscated = QTextCharFormat::UserProperty + 0x100, // bool
    TranslateKey,                                           // QString
    TranslateArgs,
    ScoreboardName,                                         // QString
    ScoreboardObjective,                                    // QString
    ScoreboardValue,                                        //QString
    Selector,                                               // QString
    Separator,
    Keybind,                                                // QString
    Nbt,                                                    // QString
    NbtInterpret,                                           // bool
    NbtBlock,                                               // QString
    NbtEntity,                                              // QString
    NbtStorage,                                             // QString
};

class RawJsonTextEdit;

class RawJsonTextObjectInterface : public QObject, public QTextObjectInterface {
    Q_OBJECT
public:
    RawJsonTextObjectInterface(RawJsonTextEdit *textEdit,
                               const QString &iconStr);

    virtual QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                                 const QTextFormat& format) override;
    virtual void drawObject(QPainter *painter, const QRectF& rect,
                            QTextDocument *doc, int posInDocument,
                            const QTextFormat& format) override;
    virtual QString displayText(const QTextFormat& format) const = 0;

protected:
    QString m_iconStr;
    RawJsonTextEdit *m_textEdit = nullptr;
};

class TranslateTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit TranslateTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const override;
};

class ScoreboardTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit ScoreboardTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const override;
};

class KeybindTextObjectInterface : public RawJsonTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit KeybindTextObjectInterface(RawJsonTextEdit *textEdit);

    QString displayText(const QTextFormat &format) const override;
};

#endif // RAWJSONTEXTOBJECTINTERFACE_H
