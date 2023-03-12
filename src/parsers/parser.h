#ifndef PARSER_H
#define PARSER_H

#include <QVariantList>
#include <QSet>
#include <QDebug>

#include <stdexcept>

using QStringSet = QSet<QString>;

#ifndef QLATIN1STRING_OPERATOR
QLatin1String constexpr operator ""_QL1(const char *literal, size_t size) {
    return QLatin1String(literal, size);
}
#define QLATIN1STRING_OPERATOR
#endif

QLatin1Char constexpr operator ""_QL1(const char chr) {
    return QLatin1Char(chr);
}

class Parser {
    Q_GADGET
public:
    class Error : public std::runtime_error {
public:
        int pos           = 0;
        int length        = 1;
        QVariantList args = {};

        explicit Error(const QString &whatArg       = QString(), int pos = -1,
                       int length                   = 0,
                       const QVariantList &args     = {});
        explicit Error(const char *whatArg, int pos = -1,
                       int length                   = 0,
                       const QVariantList &args     = {});

        QString toLocalizedMessage() const;

        bool operator==(const Error &o) const;
    };
    using Errors = QVector<Error>;

    Parser();
    explicit Parser(const QString &input);

    int pos() const;
    void setPos(int newPos);

    QChar inline curChar() const {
        return m_curChar;
    }

    QString text() const;
    void setText(const QString &newText);
    void setText(QString &&newText);

    bool parse();
    bool parse(const QString &text);
    bool parse(QString &&text);

    Errors errors() const;

    QStringSet spans() const;



protected:
    enum EatOption { // For use in eat() method
        NoOption    = 0x0,
        SkipLeftWs  = 0x1,
        SkipRightWs = 0x2,
        SkipBothWs  = 0x3,
    };
    Q_DECLARE_FLAGS(EatOptions, EatOption);

    Errors m_errors;
    QStringSet m_spans;

    void throwError [[noreturn]](const QString &msg,
                                 const QVariantList &args = {});
    void throwError [[noreturn]](const char *msg,
                                 const QVariantList &args = {});
    void throwError [[noreturn]](const QString &msg, const QVariantList &args,
                                 int pos, int length = 0);
    void throwError [[noreturn]](const char *msg, const QVariantList &args,
                                 int pos, int length = 0);

    void reportError(const char *msg, const QVariantList &args = {});
    void reportError(const char *msg, const QVariantList &args,
                     int pos, int length = 0);

    void advance(int n = 1);

    bool expect(QChar chr);
    QString eat(QChar chr, EatOptions options = NoOption);
    QStringRef getUntil(QChar chr);
    QStringRef getRest();
    QString getWithCharset(const QString &charset);
    QString getWithCharset(const QLatin1String &charset);
    QString getWithRegex(const QString &pattern);
    QString getWithRegex(const QRegularExpression &regex);
    QStringRef peek(int n) const;
    QStringRef peekUntil(QChar chr) const;
    QString skipWs(bool once = true);

    QString getQuotedString();

    QString spanText(const QStringRef& textRef);
    QString spanText(const QString& text);
    QString spanText(QString&& text);
    QString spanText(int start);

protected:
    virtual bool parseImpl() {
        return false;
    };

private:
    QString m_text;
    int m_pos = 0;
    QChar m_curChar;
};

#endif // PARSER_H
