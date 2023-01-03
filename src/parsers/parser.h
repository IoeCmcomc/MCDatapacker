#ifndef PARSER_H
#define PARSER_H

#include <QVariantList>
#include <QSet>

#include <stdexcept>

using QStringSet = QSet<QString>;

class Parser {
    Q_GADGET
public:
    class Error : public std::runtime_error {
public:
        int pos           = 0;
        int length        = 1;
        QVariantList args = {};

        explicit Error(const QString &whatArg   = QString(), int pos = -1,
                       int length               = 0,
                       const QVariantList &args = {});

        QString toLocalizedMessage() const;

        bool operator==(const Error &o) const;
    };
    using Errors = QVector<Error>;

    Parser();
    explicit Parser(const QString &input);

    int pos() const;
    void setPos(int newPos);

    QChar curChar() const;

    QString text() const;
    void setText(const QString &newText);

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

    void error(const QString &msg, const QVariantList &args = {});
    void error [[noreturn]] (const QString &msg, const QVariantList &args,
                             int pos, int length = 0);
    void advance(int n = 1);

    bool expect(QChar chr);
    QString eat(QChar chr, EatOptions options = NoOption);
    QStringRef getUntil(QChar chr);
    QStringRef getRest();
    QString getWithCharset(const QString &charset);
    QString getWithRegex(const QString &pattern);
    QString getWithRegex(const QRegularExpression &regex);
    QStringRef peek(int n);
    QStringRef peekUntil(QChar chr);
    QString skipWs(bool once = true);

    QString getQuotedString();

    QString spanText(const QStringRef& textRef);
    QString spanText(const QString& text);
    QString spanText(QString&& text);
    QString spanText(int start);

private:
    QString m_text;
    int m_pos = 0;
    QChar m_curChar;
};

#endif // PARSER_H
