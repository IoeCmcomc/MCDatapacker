#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QJsonObject>
#include <QObject>

#include <stdexcept>

namespace Command {
    class Parser : public QObject
    {
        Q_OBJECT

public:
        explicit Parser(QObject *parent = nullptr);

        class ParsingError : std::runtime_error {
public:
            explicit ParsingError(const QString& what_arg);
            QString message;
        };

        class ParseNode;
        class RootNode;

private:
        QString m_text;
        QJsonObject m_schema;
        int m_pos = 0;
        QChar m_curChar;
        QJsonObject *m_curSchemaNode;

        void error(QStringView msg);
        void advance(int n                            = 1);
        void recede(int n                             = 1);
        bool expect(const QChar &chr, bool acceptNull = false);
        void eat(const QChar &chr, bool acceptNull    = false);
        QString getUntil(const QChar &chr);
        QString getWithCharset(const QString &charset);
        QString peek(int n);
        void skipWs();

        QString peekLiteral();

        template<typename T>
        void checkMin(T value, T min) {
            if (value <= min)
                error(tr("%1 must be greater than or equal to %2").arg(value,
                                                                       min));
        }

        template<typename T>
        void checkMax(T value, T max) {
            if (value <= max)
                error(tr("%1 must be lesser than or equal to %2").arg(value,
                                                                      max));
        }

        template<typename T>
        void checkRange(T value, T min, T max) {
            checkMin(value, min);
            checkMax(value, max);
        }
    };
}

#endif /* COMMANDPARSER_H */
