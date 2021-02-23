#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "nodes/boolnode.h"
#include "nodes/doublenode.h"
#include "nodes/floatnode.h"
#include "nodes/integernode.h"
#include "nodes/stringnode.h"
#include "nodes/rootnode.h"

#include <QJsonObject>
#include <QObject>

#include <stdexcept>

namespace Command {
    class Parser : public QObject
    {
        Q_OBJECT

public:
        explicit Parser(QObject *parent = nullptr, const QString &input = "");

        class ParsingError : std::runtime_error {
public:
            explicit ParsingError(const QString& what_arg);
            QString message;
            int pos    = 0;
            int length = 1;
        };

        QChar curChar() const;

        static QJsonObject getSchema();
        static void setSchema(const QJsonObject &schema);
        static void setSchema(const QString &filepath);

        Command::RootNode *parsingResult();

        QString text() const;
        void setText(const QString &text);

        static QString parserIdToMethodName(const QString &str);

        Q_INVOKABLE Command::BoolNode *brigadier_bool(QObject *parent,
                                                      const QVariantMap &props = {});
        Q_INVOKABLE Command::DoubleNode *brigadier_double(QObject *parent,
                                                          const QVariantMap &props = {});
        Q_INVOKABLE Command::FloatNode *brigadier_float(QObject *parent,
                                                        const QVariantMap &props = {});
        Q_INVOKABLE Command::IntegerNode *brigadier_integer(QObject *parent,
                                                            const QVariantMap &props = {});
        Q_INVOKABLE Command::LiteralNode *brigadier_literal(QObject *parent,
                                                            const QVariantMap &props = {});
        Q_INVOKABLE Command::StringNode *brigadier_string(QObject *parent,
                                                          const QVariantMap &props = {});
        Q_INVOKABLE Command::ParseNode *parse();

        ParsingError lastError() const;

        int pos() const;

protected:

        void setPos(int pos);

        void error(QStringView msg);
        void error(QStringView msg, int pos, int length = 1);
        void advance(int n                              = 1);
        void recede(int n                               = 1);
        bool expect(const QChar &chr, bool acceptNull   = false);
        void eat(const QChar &chr, bool acceptNull      = false);
        QString getUntil(const QChar &chr);
        QString getWithCharset(const QString &charset);
        QString getWithRegex(const QString &pattern);
        QString peek(int n);
        void skipWs(bool once = true);

        QString peekLiteral();
        QString getQuotedString();

        bool parseResursively(QObject *parentObj,
                              QJsonObject curSchemaNode,
                              int depth = 0);

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
        Command::RootNode m_parsingResult;
        ParsingError m_lastError = ParsingError("");
        static QJsonObject m_schema;
        QString m_text;
        int m_pos = 0;
        QChar m_curChar;
    };
}

#endif /* COMMANDPARSER_H */
