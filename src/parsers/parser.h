#ifndef PARSER_H
#define PARSER_H

#include <QVariantList>
#include <QCoreApplication>

#include <stdexcept>

using StringHash = QHash<QStringView, QString>;
//using StringHash = std::unordered_map<QStringView, QString>;

//using StringHash = QSet<QString>;

QLatin1Char constexpr operator ""_QL1(const char chr) {
    return QLatin1Char(chr);
}

namespace Command {
    class McfunctionParser;
}

class Parser {
    Q_DECLARE_TR_FUNCTIONS(Parser);
public:
    class Error : public std::runtime_error {
        Q_DECLARE_TR_FUNCTIONS(Parser);
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
    explicit Parser(const QString &text);
    explicit Parser(QString &&text);
    virtual ~Parser() {
    };

    int pos() const;
    void setPos(int newPos);

    QChar curChar() const;

    QString text() const;
    QStringView textView() const;
    void setText(const QString &newText);
    void setText(QString &&newText);
    void setText(QStringView text);

    bool parse();
    bool parse(const QString &text);
    bool parse(QString &&text);
    bool parse(QStringView text);

    Errors errors() const;
    // The below method is added to avoid clazy-range-loop-detach warning
    Errors &errors();

    StringHash spans() const;

    friend Command::McfunctionParser;

protected:
    enum EatOption { // For use in eat() method
        NoOption    = 0x0,
        SkipLeftWs  = 0x1,
        SkipRightWs = 0x2,
        SkipBothWs  = 0x3,
    };
    Q_DECLARE_FLAGS(EatOptions, EatOption);

    Errors m_errors;
    StringHash m_spans;

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
    QStringView advanceView(QStringView sv);

    bool expect(QChar chr, const char *errMsg = nullptr);
    QString eat(QChar chr, const char *errMsg = nullptr,
                EatOptions options            = NoOption);
    QStringView getUntil(QChar chr);
    QStringView getRest();
    QStringView getWithCharset(const QString &charset);
    QStringView getWithCharset(const QLatin1String &charset);
    QStringView getWithRegex(const QString &pattern);
    QStringView getWithRegex(const QRegularExpression &regex);
    QStringView peek(int n) const;
    QStringView peekNext(int n) const;
    QStringView peekUntil(QChar chr) const;
    QStringView peekRest() const;
    QString skipWs(bool once = true);

    QString getQuotedString();

    QString spanText(QStringView textView);
    QString spanText(const QString& text);
    QString spanText(QString&& text);
    QString spanText(int start);

    virtual bool parseImpl() {
        return false;
    };

private:
    QStringView m_text;
    QString m_srcText;
    int m_pos = 0;
    QChar m_curChar;
};

static_assert(qIsRelocatable<Parser::Error>() == false);
Q_DECLARE_TYPEINFO(Parser::Error, Q_RELOCATABLE_TYPE);

#endif // PARSER_H
