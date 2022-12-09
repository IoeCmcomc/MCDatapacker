#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "nodes/boolnode.h"
#include "nodes/doublenode.h"
#include "nodes/floatnode.h"
#include "nodes/integernode.h"
#include "nodes/stringnode.h"
#include "nodes/rootnode.h"
#include "parsenodecache.h"

#include <QJsonObject>
#include <QObject>

#include <stdexcept>

namespace Command {
    class Parser : public QObject
    {
        Q_OBJECT

public:
        explicit Parser(QObject *parent = nullptr, const QString &input = "");

        class Error : public std::runtime_error {
public:
            explicit Error(const QString &whatArg   = QString(), int pos = -1,
                           int length               = 0,
                           const QVariantList &args = {});
            uint pos          = 0;
            uint length       = 1;
            QVariantList args = {};

            QString toLocalizedMessage() const;
        };
        QChar curChar() const;

        static QJsonObject getSchema();
        static void setSchema(const QJsonObject &schema);
        static void setSchema(const QString &filepath);

        QSharedPointer<Command::RootNode> parsingResult();

        QString text() const;
        void setText(const QString &text);

        static QString parserIdToMethodName(const QString &str);

        Q_INVOKABLE QSharedPointer<Command::BoolNode> brigadier_bool();
        Q_INVOKABLE QSharedPointer<Command::DoubleNode> brigadier_double(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::FloatNode> brigadier_float(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::IntegerNode> brigadier_integer(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::LiteralNode> brigadier_literal();
        Q_INVOKABLE QSharedPointer<Command::StringNode> brigadier_string(
            const QVariantMap &props = {});
        QSharedPointer<Command::ParseNode> parse();

        Error lastError() const;

        int pos() const;
        const ParseNodeCache &cache() const;
        static void setTestMode(bool value) {
            Parser::m_testMode = value;
        }


protected:
        static bool m_testMode;

        void setPos(int pos);

        void error(const QString &msg, const QVariantList &args = {});
        void error [[noreturn]] (const QString &msg, const QVariantList &args,
                                 int pos, int length = 0);
        void advance(int n = 1);
        void recede(int n  = 1);

        bool expect(const QChar &chr, bool acceptNull = false);
        void eat(const QChar &chr, bool acceptNull    = false);
        QString getUntil(const QChar &chr);
        QString getRest();
        QString getWithCharset(const QString &charset);
        QString getWithRegex(const QString &pattern);
        QString getWithRegex(const QRegularExpression &regex);
        QString peek(int n);
        void skipWs(bool once = true);

        QString peekLiteral();
        QString getQuotedString();

        bool processCurSchemaNode(int depth, QJsonObject &curSchemaNode);
        bool parseResursively(QJsonObject curSchemaNode,
                              int depth = 0);

        template<typename T>
        void checkMin(T value, T min) {
            if (value < min)
                error(QT_TRANSLATE_NOOP("Command:Parser::Error",
                                        "The value must be greater than or equal to %1"),
                      { min });
        }

        template<typename T>
        void checkMax(T value, T max) {
            if (value > max)
                error(QT_TRANSLATE_NOOP("Command:Parser::Error",
                                        "The value must be lesser than or equal to %1"),
                      { max });
        }

        template<typename T>
        void checkRange(T value, T min, T max) {
            checkMin(value, min);
            checkMax(value, max);
        }

        template<typename Type, typename Class, typename ... Args1,
                 typename ... Args2>
        auto callWithCache(QSharedPointer<Type> (Class::*funcPtr)(Args1 ...),
                           Class *that, const QString &literal,
                           Args2 &&... args) {
            const int startPos  = pos();
            const int retTypeId = qMetaTypeId<QSharedPointer<Type> >();

            Q_ASSERT(retTypeId != 0);
            CacheKey key{ retTypeId, literal, startPos };

            if (m_cache.contains(key)) {
                const auto value = qSharedPointerCast<Type>(m_cache[key]);
                Q_ASSERT(value != nullptr);
                advance(value->length());
                return value;
            } else if (key.pos = -1; m_cache.contains(key)) {     /* Not taking position into account */
                /* Make a copy of the cached object */
                auto value = QSharedPointer<Type>::create(*qSharedPointerCast<Type>(
                                                              m_cache[key]));
                /* Change its pos to the start pos */
                value->setPos(startPos);
                if ((literal.length() == value->length()) && value->isVaild())
                    m_cache.emplace(retTypeId, literal, startPos, value);
                advance(value->length());
                return value;
            }
            auto value =
                (that->*funcPtr)(std::forward<decltype(args)>(args)...);
            Q_ASSERT(value != nullptr);
            m_cache.emplace(retTypeId, literal, startPos, value);

            return value;
        }

        void printMethods();
private:
        ParseNodeCache m_cache;
        Error m_lastError;
        QSharedPointer<Command::RootNode> m_parsingResult = nullptr;
        const QRegularExpression m_literalStrRegex        = QRegularExpression(
            R"([a-zA-Z0-9-_.*<=>]+)");
        const QRegularExpression m_decimalNumRegex = QRegularExpression(
            R"([+-]?(?:\d+\.\d+|\.\d+|\d+\.|\d+))");
        static QJsonObject m_schema;
        QString m_text;
        uint m_pos = 0;
        QChar m_curChar;
    };
}

#endif /* COMMANDPARSER_H */
