#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "../parser.h"
#include "nodes/singlevaluenode.h"
#include "nodes/stringnode.h"
#include "nodes/rootnode.h"
#include "nodes/literalnode.h"
#include "parsenodecache.h"
#include "schema/schemarootnode.h"

#include <QJsonObject>
#include <QObject>

#include <stdexcept>

namespace Command {
    class SchemaParser : public Parser {
        Q_GADGET

public:
        SchemaParser();
        using Parser::Parser;

        struct Result {
            NodePtr    tree;
            QStringSet spans;
        };

        static void setSchema(const QString &filepath);
        static void loadSchema(const QString &filepath);
        static Schema::RootNode * schema();

        Result parsingResult();

        QSharedPointer<Command::BoolNode> brigadier_bool();
        QSharedPointer<Command::DoubleNode> brigadier_double(
            const QVariantMap &props = {});
        QSharedPointer<Command::FloatNode> brigadier_float(
            const QVariantMap &props = {});
        QSharedPointer<Command::IntegerNode> brigadier_integer(
            const QVariantMap &props = {});
        QSharedPointer<Command::LiteralNode> brigadier_literal();
        QSharedPointer<Command::StringNode> brigadier_string(
            const QVariantMap &props = {});

        NodePtr parse();

        const ParseNodeCache &cache() const;
        static void setTestMode(bool value);


protected:
        static inline bool m_testMode = false;

        QString peekLiteral() const;
        QString getLiteralString();

        bool canContinue(Schema::Node **schemaNode, int depth);
        bool parseBySchema(const Schema::Node *schemaNode, int depth = 0);

        template<typename T>
        void checkMin(T value, T min) {
            if (value < min)
                error(QT_TRANSLATE_NOOP("Command::Parser::Error",
                                        "The value must be greater than or equal to %1"),
                      { min });
        }

        template<typename T>
        void checkMax(T value, T max) {
            if (value > max)
                error(QT_TRANSLATE_NOOP("Command::Parser::Error",
                                        "The value must be lesser than or equal to %1"),
                      { max });
        }

        template<typename T>
        void checkRange(T value, T min, T max) {
            checkMin(value, min);
            checkMax(value, max);
        }

        template <typename T>
        constexpr static int getTypeEnumId() {
            if constexpr (std::is_base_of_v<ArgumentNode, T>) {
                return static_cast<int>(nodeTypeEnum<T,
                                                     ArgumentNode::ParserType>);
            } else {
                return static_cast<int>(nodeTypeEnum<T, ParseNode::Kind>);
            }
        };

        template<typename Type, typename Class, typename ... Args1,
                 typename ... Args2>
        auto callWithCache(QSharedPointer<Type> (Class::*funcPtr)(Args1 ...),
                           Class *that, const QString &literal,
                           Args2 &&... args) {
            //if (m_testMode) {
            return (that->*funcPtr)(std::forward<Args1>(args)...);
            //}

            /*
               constexpr int retTypeId = getTypeEnumId<Type>();
               CacheKey      key{ retTypeId, literal };

               if (m_cache.contains(key)) {
                const auto value = qSharedPointerCast<Type>(m_cache[key]);
                Q_ASSERT(value != nullptr);
                advance(value->length());
                return value;
               }
               auto value =
                (that->*funcPtr)(std::forward<decltype(args)>(args)...);
               Q_ASSERT(value != nullptr);
               m_cache.emplace(retTypeId, literal, value);

               return value;
             */
        }

        virtual NodePtr invokeMethod(ArgumentNode::ParserType parserType,
                                     const QVariantMap &props);

private:
        ParseNodeCache m_cache;
        QSharedPointer<Command::RootNode> m_tree;
        static inline const QRegularExpression m_decimalNumRegex{
            QStringLiteral(R"([+-]?(?:\d+\.\d+|\.\d+|\d+\.|\d+))") };
        static inline Schema::RootNode m_schemaGraph;
    };
}

#endif /* COMMANDPARSER_H */
