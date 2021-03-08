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
            explicit Error(const QString &whatArg   = "", int pos = -1,
                           int length               = 0,
                           const QVariantList &args = {});
            int pos           = 0;
            int length        = 1;
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

        Q_INVOKABLE QSharedPointer<Command::BoolNode> brigadier_bool(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::DoubleNode> brigadier_double(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::FloatNode> brigadier_float(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::IntegerNode> brigadier_integer(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::LiteralNode> brigadier_literal(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::StringNode> brigadier_string(
            const QVariantMap &props = {});
        Q_INVOKABLE QSharedPointer<Command::ParseNode> parse();

        Error lastError() const;

        int pos() const;
        const ParseNodeCache &cache() const;


protected:
        void setPos(int pos);

        void error(const QString &msg, const QVariantList &args = {});
        void error(const QString &msg, const QVariantList &args,
                   int pos, int length                = 1);
        void advance(int n                            = 1);
        void recede(int n                             = 1);
        bool expect(const QChar &chr, bool acceptNull = false);
        void eat(const QChar &chr, bool acceptNull    = false);
        QString getUntil(const QChar &chr);
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

        virtual QSharedPointer<ParseNode> QVariantToParseNodeSharedPointer(
            const QVariant &vari);

        template<typename T>
        void checkMin(T value, T min) {
            if (value < min)
                error(
                    QString("The value must be greater than or equal to %1").arg(
                        min));
        }

        template<typename T>
        void checkMax(T value, T max) {
            if (value > max)
                error(
                    QString("The value must be lesser than or equal to %1").arg(
                        max));
        }

        template<typename T>
        void checkRange(T value, T min, T max) {
            checkMin(value, min);
            checkMax(value, max);
        }
        void printMethods();
private:
        ParseNodeCache m_cache;
        Error m_lastError;
        QSharedPointer<Command::RootNode> m_parsingResult = nullptr;
        QRegularExpression m_literalStrRegex              = QRegularExpression(
            R"([a-zA-Z0-9-_.*<=>]+)");
        QRegularExpression m_decimalNumRegex = QRegularExpression(
            R"([+-]?(?:\d+\.\d+|\.\d+|\d+\.|\d+)(?:[eE]\d+)?)");
        static QJsonObject m_schema;
        QString m_text;
        int m_pos = 0;
        QChar m_curChar;
    };
}

#define QVARIANT_CAST_SHARED_POINTER_IF_BRANCH(Type)               \
    if (typeId == qMetaTypeId<QSharedPointer<Command::Type> >()) { \
        return qSharedPointerCast<Command::ParseNode>(             \
            vari.value<QSharedPointer<Command::Type> >());         \
    }
#define QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(Type) \
    else QVARIANT_CAST_SHARED_POINTER_IF_BRANCH(Type)

#endif /* COMMANDPARSER_H */
